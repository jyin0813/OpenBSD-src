/*
 * ip.c (C) 1995 Darren Reed
 *
 * The author provides this program as-is, with no gaurantee for its
 * suitability for any specific purpose.  The author takes no responsibility
 * for the misuse/abuse of this program and provides it for the sole purpose
 * of testing packet filter policies.  This file maybe distributed freely
 * providing it is not modified and that this notice remains in tact.
 */
#ifndef	lint
static	char	sccsid[] = "%W% %G% (C)1995";
#endif
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in_systm.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#ifndef	linux
#include <netinet/if_ether.h>
#include <netinet/ip_var.h>
#include <netinet/tcpip.h>
#endif
#include "ip_compat.h"
#ifdef	linux
#include "tcpip.h"
#endif


static	char	*ipbuf = NULL, *ethbuf = NULL;


u_short	chksum(buf,len)
u_short	*buf;
int	len;
{
	u_long	sum = 0;
	int	nwords = len >> 1;

	for(; nwords > 0; nwords--)
		sum += *buf++;
	sum = (sum>>16) + (sum & 0xffff);
	sum += (sum >>16);
	return (~sum);
}


int	send_ether(nfd, buf, len, gwip)
int	nfd, len;
char	*buf;
struct	in_addr	gwip;
{
	static	struct	in_addr	last_gw;
	static	char	last_arp[6] = { 0, 0, 0, 0, 0, 0};
	ether_header_t	*eh;
	char	*s;
	int	err;

	if (!ethbuf)
		ethbuf = (char *)calloc(1, 65536+1024);
	s = ethbuf;
	eh = (ether_header_t *)s;

	bcopy((char *)buf, s + sizeof(*eh), len);
	if (gwip.s_addr == last_gw.s_addr)
		bcopy(last_arp, eh->ether_dhost, 6);
	else if (arp((char *)&gwip, &eh->ether_dhost) == -1)
	    {
		perror("arp");
		free(buf);
		return -2;
	    }
	eh->ether_type = htons((u_short)ETHERTYPE_IP);
	last_gw.s_addr = gwip.s_addr;
	err = sendip(nfd, s, sizeof(*eh) + len);
	free(buf);
	return err;
}


/*
 */
int	send_ip(nfd, mtu, ip, gwip)
int	nfd, mtu;
ip_t	*ip;
struct	in_addr	gwip;
{
	static	struct	in_addr	last_gw;
	static	char	last_arp[6] = { 0, 0, 0, 0, 0, 0};
	static	u_short	id = 0;
	ether_header_t	*eh;
	ip_t	ipsv;
	int	err;

	if (!ipbuf)
		ipbuf = (char *)malloc(65536);
	eh = (ether_header_t *)ipbuf;

	bzero(&eh->ether_shost, sizeof(eh->ether_shost));
	if (gwip.s_addr == last_gw.s_addr)
		bcopy(last_arp, eh->ether_dhost, 6);
	else if (arp((char *)&gwip, &eh->ether_dhost) == -1)
	    {
		perror("arp");
		return -2;
	    }
	eh->ether_type = htons((u_short)ETHERTYPE_IP);

	bcopy((char *)ip, (char *)&ipsv, sizeof(*ip));
	last_gw.s_addr = gwip.s_addr;
	ip->ip_len = htons(ip->ip_len);
	ip->ip_off = htons(ip->ip_off);
	if (!ip->ip_v)
		ip->ip_v   = IPVERSION;
	if (!ip->ip_id)
		ip->ip_id  = htons(id++);
	if (!ip->ip_ttl)
		ip->ip_ttl = 60;

	if (sizeof(*eh) + ntohs(ip->ip_len) < mtu)
	    {
		ip->ip_sum = 0;
		ip->ip_sum = chksum(ip, ip->ip_hl << 2);

		bcopy((char *)ip, ipbuf + sizeof(*eh), ntohs(ip->ip_len));
		err =  sendip(nfd, ipbuf, sizeof(*eh) + ntohs(ip->ip_len));
	    }
	else
	    {
		/*
		 * Actually, this is bogus because we're putting all IP
		 * options in every packet, which isn't always what should be
		 * done.  Will do for now.
		 */
		ether_header_t	eth;
		char	optcpy[48], ol;
		char	*s;
		int	i, iplen, sent = 0, ts, hlen, olen;

		hlen = ip->ip_hl << 2;
		if (mtu < (hlen + 8)) {
			fprintf(stderr, "mtu (%d) < ip header size (%d) + 8\n",
				mtu, hlen);
			fprintf(stderr, "can't fragment data\n");
			return -2;
		}
		ol = (ip->ip_hl << 2) - sizeof(*ip);
		for (i = 0, s = (char*)(ip + 1); ol > 0; )
			if (*s == IPOPT_EOL) {
				optcpy[i++] = *s;
				break;
			} else if (*s == IPOPT_NOP) {
				s++;
				ol--;
			} else
			    {
				olen = (int)(*(u_char *)(s + 1));
				ol -= olen;
				if (IPOPT_COPIED(*s))
				    {
					bcopy(s, optcpy + i, olen);
					i += olen;
					s += olen;
				    }
			    }
		if (i)
		    {
			/*
			 * pad out
			 */
			while ((i & 3) && (i & 3) != 3)
				optcpy[i++] = IPOPT_NOP;
			if (i & 3 == 3)
				optcpy[i++] = IPOPT_EOL;
		    }

		bcopy((char *)eh, (char *)&eth, sizeof(eth));
		s = (char *)ip + hlen;
		iplen = ntohs(ip->ip_len) - hlen;
		ip->ip_off |= htons(IP_MF);

		while (1)
		    {
			if ((sent + (mtu - hlen)) >= iplen)
			    {
				ip->ip_off ^= htons(IP_MF);
				ts = iplen - sent;
			    }
			else
				ts = (mtu - hlen);
			ip->ip_off &= htons(0xe000);
			ip->ip_off |= htons(sent >> 3);
			ts += hlen;
			ip->ip_len = htons(ts);
			ip->ip_sum = 0;
			ip->ip_sum = chksum(ip, hlen);
			bcopy((char *)ip, ipbuf + sizeof(*eh), hlen);
			bcopy(s + sent, ipbuf + sizeof(*eh) + hlen, ts - hlen);
			err =  sendip(nfd, ipbuf, sizeof(*eh) + ts);

			bcopy((char *)&eth, ipbuf, sizeof(eth));
			sent += (ts - hlen);
			if (!(ntohs(ip->ip_off) & IP_MF))
				break;
			else if (!(ip->ip_off & htons(0x1fff)))
			    {
				hlen = i + sizeof(*ip);
				ip->ip_hl = (sizeof(*ip) + i) >> 2;
				bcopy(optcpy, (char *)(ip + 1), i);
			    }
		    }
	    }

	bcopy((char *)&ipsv, (char *)ip, sizeof(*ip));
	return err;
}


/*
 * send a tcp packet.
 */
int	send_tcp(nfd, mtu, ip, gwip)
int	nfd, mtu;
ip_t	*ip;
struct	in_addr	gwip;
{
	static	tcp_seq	iss = 2;
	struct	tcpiphdr *ti;
	int	thlen, i;
	u_long	lbuf[20];

	ti = (struct tcpiphdr *)lbuf;
	bzero((char *)ti, sizeof(*ti));
	thlen = sizeof(tcphdr_t);
	ip->ip_p = IPPROTO_TCP;
	ti->ti_pr = ip->ip_p;
	ti->ti_src = ip->ip_src;
	ti->ti_dst = ip->ip_dst;
	bcopy((char *)ip + (ip->ip_hl << 2),
	      (char *)&ti->ti_sport, sizeof(tcphdr_t));

	if (!ti->ti_win)
		ti->ti_win = htons(4096);
	if (!ti->ti_seq)
		ti->ti_seq = htonl(iss);
	iss += 64;

	if ((ti->ti_flags == TH_SYN) && !ip->ip_off)
	    {
		ip = (ip_t *)realloc((char *)ip, ntohs(ip->ip_len) + 4);
		i = sizeof(struct tcpiphdr) / sizeof(long);
		lbuf[i] = htonl(0x020405b4);
		bcopy((char *)(lbuf + i), (char*)ip + ntohs(ip->ip_len),
		      sizeof(u_long));
		thlen += 4;
	    }
	if (!ti->ti_off)
		ti->ti_off = thlen >> 2;
	ti->ti_len = htons(thlen);
	ip->ip_len = (ip->ip_hl << 2) + thlen;
	ti->ti_sum = 0;
	ti->ti_sum = chksum(ti, thlen + sizeof(ip_t));

	bcopy((char *)&ti->ti_sport,
	      (char *)ip + (ip->ip_hl << 2), thlen);
	return send_ip(nfd, mtu, ip, gwip);
}


/*
 * send a udp packet.
 */
int	send_udp(nfd, mtu, ip, gwip)
int	nfd, mtu;
ip_t	*ip;
struct	in_addr	gwip;
{
	struct	tcpiphdr *ti;
	int	thlen, i;
	u_long	lbuf[20];

	ti = (struct tcpiphdr *)lbuf;
	bzero((char *)ti, sizeof(*ti));
	thlen = sizeof(udphdr_t);
	ti->ti_pr = ip->ip_p;
	ti->ti_src = ip->ip_src;
	ti->ti_dst = ip->ip_dst;
	bcopy((char *)ip + (ip->ip_hl << 2),
	      (char *)&ti->ti_sport, sizeof(udphdr_t));

	ti->ti_len = htons(thlen);
	ip->ip_len = (ip->ip_hl << 2) + thlen;
	ti->ti_sum = 0;
	ti->ti_sum = chksum(ti, thlen + sizeof(ip_t));

	bcopy((char *)&ti->ti_sport,
	      (char *)ip + (ip->ip_hl << 2), sizeof(udphdr_t));
	return send_ip(nfd, mtu, ip, gwip);
}


/*
 * send an icmp packet.
 */
int	send_icmp(nfd, mtu, ip, gwip)
int	nfd, mtu;
ip_t	*ip;
struct	in_addr	gwip;
{
	struct	icmp	*ic;

	ic = (struct icmp *)((char *)ip + (ip->ip_hl << 2));

	ic->icmp_cksum = 0;
	ic->icmp_cksum = chksum((char *)ic, sizeof(struct icmp));

	return send_ip(nfd, mtu, ip, gwip);
}


int	send_packet(nfd, mtu, ip, gwip)
int	nfd, mtu;
ip_t	*ip;
struct	in_addr	gwip;
{
        switch (ip->ip_p)
        {
        case IPPROTO_TCP :
                return send_tcp(nfd, mtu, ip, gwip);
        case IPPROTO_UDP :
                return send_udp(nfd, mtu, ip, gwip);
        case IPPROTO_ICMP :
                return send_icmp(nfd, mtu, ip, gwip);
        default :
                return send_ip(nfd, mtu, ip, gwip);
        }
}

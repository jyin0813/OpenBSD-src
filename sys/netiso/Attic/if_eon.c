/*	$OpenBSD: if_eon.c,v 1.24 2004/06/21 23:50:37 tholo Exp $	*/
/*	$NetBSD: if_eon.c,v 1.15 1996/05/09 22:29:37 scottr Exp $	*/

/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)if_eon.c	8.1 (Berkeley) 6/10/93
 */

/***********************************************************
		Copyright IBM Corporation 1987

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * ARGO Project, Computer Sciences Dept., University of Wisconsin - Madison
 */
/*
 *	EON rfc
 *  Layer between IP and CLNL
 *
 * TODO:
 * Put together a current rfc986 address format and get the right offset
 * for the nsel
 */

#ifdef EON
#define NEON 1


#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/buf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/types.h>

#include <machine/cpu.h>	/* XXX for setsoftnet().  This must die. */

#include <net/if.h>
#include <net/if_types.h>
#include <net/if_dl.h>
#include <net/netisr.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/if_ether.h>

#include <netiso/iso.h>
#include <netiso/iso_var.h>
#include <netiso/iso_snpac.h>
#include <netiso/argo_debug.h>
#include <netiso/iso_errno.h>
#include <netiso/eonvar.h>

#include <sys/stdarg.h>

#define EOK 0

struct ifnet    eonif[1];

void
eonprotoinit()
{
	(void) eonattach();
}

struct eon_llinfo eon_llinfo;
#define PROBE_OK 0;


/*
 * FUNCTION:		eonattach
 *
 * PURPOSE:			autoconf attach routine
 *
 * RETURNS:			void
 */

void
eonattach()
{
	struct ifnet *ifp = eonif;

#ifdef ARGO_DEBUG
	if (argo_debug[D_EON]) {
		printf("eonattach()\n");
	}
#endif
	snprintf(ifp->if_xname, sizeof ifp->if_xname, "eon%d", 0);
	ifp->if_mtu = ETHERMTU;
	ifp->if_softc = NULL;
	/* since everything will go out over ether or token ring */

	ifp->if_ioctl = eonioctl;
	ifp->if_output = eonoutput;
	ifp->if_type = IFT_EON;
	ifp->if_addrlen = 5;
	ifp->if_hdrlen = EONIPLEN;
	ifp->if_flags = IFF_BROADCAST;
	if_attach(ifp);
	eonioctl(ifp, SIOCSIFADDR, (caddr_t) ifp->if_addrlist.tqh_first);
	eon_llinfo.el_qhdr.link =
		eon_llinfo.el_qhdr.rlink = &(eon_llinfo.el_qhdr);

#ifdef ARGO_DEBUG
	if (argo_debug[D_EON]) {
		printf("eonattach()\n");
	}
#endif
}


/*
 * FUNCTION:		eonioctl
 *
 * PURPOSE:			io controls - ifconfig
 *				need commands to
 *					link-UP (core addr) (flags: ES, IS)
 *					link-DOWN (core addr) (flags: ES, IS)
 *				must be callable from kernel or user
 *
 * RETURNS:			nothing
 */
int
eonioctl(ifp, cmd, data)
	struct ifnet *ifp;
	u_long          cmd;
	caddr_t data;
{
	int             s = splimp();
	int    error = 0;

#ifdef ARGO_DEBUG
	if (argo_debug[D_EON]) {
		printf("eonioctl (cmd 0x%lx) \n", cmd);
	}
#endif

	switch (cmd) {
		struct ifaddr *ifa;

	case SIOCSIFADDR:
		if ((ifa = (struct ifaddr *) data) != NULL) {
			ifp->if_flags |= IFF_UP;
			if (ifa->ifa_addr->sa_family != AF_LINK)
				ifa->ifa_rtrequest = eonrtrequest;
		}
		break;
	}
	splx(s);
	return (error);
}


void
eoniphdr(hdr, loc, ro, class, zero)
	struct route   *ro;
	struct eon_iphdr *hdr;
	caddr_t         loc;
	int		class, zero;
{
	struct mbuf     mhead;
	struct sockaddr_in *sin = satosin(&ro->ro_dst);
	if (zero) {
		bzero((caddr_t) hdr, sizeof(*hdr));
		bzero((caddr_t) ro, sizeof(*ro));
	}
	sin->sin_family = AF_INET;
	sin->sin_len = sizeof(*sin);
	bcopy(loc, (caddr_t) & sin->sin_addr, sizeof(struct in_addr));
	/*
	 * If there is a cached route,
	 * check that it is to the same destination
	 * and is still up.  If not, free it and try again.
	 */
	if (ro->ro_rt) {
		struct sockaddr_in *dst = satosin(rt_key(ro->ro_rt));
		if ((ro->ro_rt->rt_flags & RTF_UP) == 0 ||
		    sin->sin_addr.s_addr != dst->sin_addr.s_addr) {
			RTFREE(ro->ro_rt);
			ro->ro_rt = (struct rtentry *) 0;
		}
	}
	rtalloc(ro);
	if (ro->ro_rt)
		ro->ro_rt->rt_use++;
	hdr->ei_ip.ip_dst = sin->sin_addr;
	hdr->ei_ip.ip_p = IPPROTO_EON;
	hdr->ei_ip.ip_ttl = MAXTTL;
	hdr->ei_eh.eonh_class = class;
	hdr->ei_eh.eonh_vers = EON_VERSION;
	hdr->ei_eh.eonh_csum = 0;
	mhead.m_data = (caddr_t) & hdr->ei_eh;
	mhead.m_len = sizeof(struct eon_hdr);
	mhead.m_next = 0;
#ifdef ARGO_DEBUG
	if (argo_debug[D_EON]) {
		printf("eonoutput : gen csum (%p, offset %d, datalen %d)\n",
		       &mhead, _offsetof(struct eon_hdr, eonh_csum),
		       sizeof(struct eon_hdr));
	}
#endif
	iso_gen_csum(&mhead,
	      _offsetof(struct eon_hdr, eonh_csum), sizeof(struct eon_hdr));
}
/*
 * FUNCTION:		eonrtrequest
 *
 * PURPOSE:			maintains list of direct eon recipients.
 *					sets up IP route for rest.
 *
 * RETURNS:			nothing
 */
void
eonrtrequest(cmd, rt, info)
	int cmd;
	struct rtentry *rt;
	struct rt_addrinfo *info;
{
	unsigned long   zerodst = 0;
	caddr_t         ipaddrloc = (caddr_t) & zerodst;
	struct eon_llinfo *el = (struct eon_llinfo *) rt->rt_llinfo;
	struct sockaddr *gate;

	/*
	 * Common Housekeeping
	 */
	switch (cmd) {
	case RTM_DELETE:
		if (el) {
			remque(&(el->el_qhdr));
			if (el->el_iproute.ro_rt)
				RTFREE(el->el_iproute.ro_rt);
			Free(el);
			rt->rt_llinfo = 0;
		}
		return;

	case RTM_ADD:
	case RTM_RESOLVE:
		rt->rt_rmx.rmx_mtu = lo0ifp->if_mtu;	/* unless better below */
		R_Malloc(el, struct eon_llinfo *, sizeof(*el));
		rt->rt_llinfo = (caddr_t) el;
		if (el == 0)
			return;
		Bzero(el, sizeof(*el));
		insque(&(el->el_qhdr), &eon_llinfo.el_qhdr);
		el->el_rt = rt;
		break;
	}
	if (info && (gate = info->rti_info[RTAX_GATEWAY]))	/*XXX*/
		switch (gate->sa_family) {
		case AF_LINK:
#define SDL(x) ((struct sockaddr_dl *)x)
			if (SDL(gate)->sdl_alen == 1)
				el->el_snpaoffset = *(u_char *) LLADDR(SDL(gate));
			else
				ipaddrloc = LLADDR(SDL(gate));
			break;
		case AF_INET:
			ipaddrloc = (caddr_t) & satosin(gate)->sin_addr;
			break;
		default:
			return;
		}
	el->el_flags |= RTF_UP;
	eoniphdr(&el->el_ei, ipaddrloc, &el->el_iproute, EON_NORMAL_ADDR, 0);
	if (el->el_iproute.ro_rt)
		rt->rt_rmx.rmx_mtu = el->el_iproute.ro_rt->rt_rmx.rmx_mtu
			- sizeof(el->el_ei);
}

/*
 * FUNCTION:		eonoutput
 *
 * PURPOSE:		prepend an eon header and hand to IP
 * ARGUMENTS:	 	(ifp) is points to the ifnet structure for this
 *			unit/device (m)  is an mbuf *, *m is a CLNL packet
 *			(dst) is a destination address - have to interp. as
 *			multicast or broadcast or real address.
 *
 * RETURNS:		unix error code
 *
 * NOTES:
 *
 */
int
eonoutput(ifp, m, sdst, rt)
	struct ifnet   *ifp;
	struct mbuf *m;	/* packet */
	struct sockaddr *sdst;		/* destination addr */
	struct rtentry *rt;
{
	struct sockaddr_iso *dst = (struct sockaddr_iso *) sdst;
	struct eon_llinfo *el;
	struct eon_iphdr *ei;
	struct route   *ro;
	int             datalen;
	struct mbuf    *mh;
	int             error = 0, class = 0, alen = 0;
	caddr_t         ipaddrloc = NULL;
	static struct eon_iphdr eon_iphdr;
	static struct route route;

#ifdef ARGO_DEBUG
	if (argo_debug[D_EON]) {
		printf("eonoutput \n");
	}
#endif

	ifp->if_opackets++;
	if (rt == 0 || (el = (struct eon_llinfo *) rt->rt_llinfo) == 0) {
		if (dst->siso_family == AF_LINK) {
			struct sockaddr_dl *sdl = (struct sockaddr_dl *) dst;

			ipaddrloc = LLADDR(sdl);
			alen = sdl->sdl_alen;
		} else if (dst->siso_family == AF_ISO &&
			   dst->siso_data[0] == AFI_SNA) {
			alen = dst->siso_nlen - 1;
			ipaddrloc = (caddr_t) dst->siso_data + 1;
		}
		switch (alen) {
		case 5:
			class = 4[(u_char *) ipaddrloc];
		case 4:
			ro = &route;
			ei = &eon_iphdr;
			eoniphdr(ei, ipaddrloc, ro, class, 1);
			goto send;
		}
einval:
		error = EINVAL;
		goto flush;
	}
	if ((el->el_flags & RTF_UP) == 0) {
		eonrtrequest(RTM_CHANGE, rt, (struct sockaddr *) 0);
		if ((el->el_flags & RTF_UP) == 0) {
			error = EHOSTUNREACH;
			goto flush;
		}
	}
	if ((m->m_flags & M_PKTHDR) == 0) {
		printf("eon: got non headered packet\n");
		goto einval;
	}
	ei = &el->el_ei;
	ro = &el->el_iproute;
	if (el->el_snpaoffset) {
		if (dst->siso_family == AF_ISO) {
			bcopy((caddr_t) & dst->siso_data[el->el_snpaoffset],
			      (caddr_t) & ei->ei_ip.ip_dst, sizeof(ei->ei_ip.ip_dst));
		} else
			goto einval;
	}
send:
	/* put an eon_hdr in the buffer, prepended by an ip header */
	datalen = m->m_pkthdr.len + EONIPLEN;
	MGETHDR(mh, M_DONTWAIT, MT_HEADER);
	if (mh == (struct mbuf *) 0)
		goto flush;
	M_MOVE_HDR(mh, m);
	mh->m_next = m;
	m = mh;
	MH_ALIGN(m, sizeof(struct eon_iphdr));
	m->m_len = sizeof(struct eon_iphdr);
	m->m_pkthdr.len = datalen;
	ei->ei_ip.ip_len = htons(datalen);
	ifp->if_obytes += datalen;
	*mtod(m, struct eon_iphdr *) = *ei;

#ifdef ARGO_DEBUG
	if (argo_debug[D_EON]) {
		printf("eonoutput dst ip addr : %x\n", ei->ei_ip.ip_dst.s_addr);
		printf("eonoutput ip_output : eonip header:\n");
		dump_buf(ei, sizeof(struct eon_iphdr));
	}
#endif

	error = ip_output(m, (struct mbuf *) 0, ro, (void *)NULL, (void *)NULL);
	m = 0;
	if (error) {
		ifp->if_oerrors++;
		ifp->if_opackets--;
		ifp->if_obytes -= datalen;
	}
flush:
	if (m)
		m_freem(m);
	return error;
}

void
eoninput(struct mbuf *m, ...)
{
	int             iphlen;
	struct eon_hdr *eonhdr;
	struct ip *iphdr;
	struct ifnet   *eonifp;
	int             s;
	va_list ap;

	va_start(ap, m);
	iphlen = va_arg(ap, int);
	va_end(ap);

	eonifp = &eonif[0];	/* kludge - really want to give CLNP the ifp
				 * for eon, not for the real device */

#ifdef ARGO_DEBUG
	if (argo_debug[D_EON]) {
		printf("eoninput() %p m_data %p m_len 0x%x dequeued\n",
		       m, (m ? m->m_data : 0), m ? m->m_len : 0);
	}
#endif

	if (m == 0)
		return;
	if (iphlen > sizeof(struct ip))
		ip_stripoptions(m, (struct mbuf *) 0);
	if (m->m_len < EONIPLEN) {
		if ((m = m_pullup(m, EONIPLEN)) == 0) {
			IncStat(es_badhdr);
	drop:
#ifdef ARGO_DEBUG
			if (argo_debug[D_EON]) {
				printf("eoninput: DROP \n");
			}
#endif
			eonifp->if_ierrors++;
			if (m != 0)
				m_freem(m);
			return;
		}
	}
	eonif->if_ibytes += m->m_pkthdr.len;
	iphdr = mtod(m, struct ip *);
	/* do a few checks for debugging */
	if (iphdr->ip_p != IPPROTO_EON) {
		IncStat(es_badhdr);
		goto drop;
	}
	/* temporarily drop ip header from the mbuf */
	m->m_data += sizeof(struct ip);
	eonhdr = mtod(m, struct eon_hdr *);
	if (iso_check_csum(m, sizeof(struct eon_hdr)) != EOK) {
		IncStat(es_badcsum);
		goto drop;
	}
	m->m_data -= sizeof(struct ip);

#ifdef ARGO_DEBUG
	if (argo_debug[D_EON]) {
		printf("eoninput csum ok class 0x%x\n", eonhdr->eonh_class);
		printf("eoninput: eon header:\n");
		dump_buf(eonhdr, sizeof(struct eon_hdr));
	}
#endif

	/* checks for debugging */
	if (eonhdr->eonh_vers != EON_VERSION) {
		IncStat(es_badhdr);
		goto drop;
	}
	m->m_flags &= ~(M_BCAST | M_MCAST);
	switch (eonhdr->eonh_class) {
	case EON_BROADCAST:
		IncStat(es_in_broad);
		m->m_flags |= M_BCAST;
		break;
	case EON_NORMAL_ADDR:
		IncStat(es_in_normal);
		break;
	case EON_MULTICAST_ES:
		IncStat(es_in_multi_es);
		m->m_flags |= M_MCAST;
		break;
	case EON_MULTICAST_IS:
		IncStat(es_in_multi_is);
		m->m_flags |= M_MCAST;
		break;
	}
	eonifp->if_ipackets++;

	{
		/* put it on the CLNP queue and set soft interrupt */
		struct ifqueue *ifq;
		extern struct ifqueue clnlintrq;

		m->m_pkthdr.rcvif = eonifp;	/* KLUDGE */
#ifdef ARGO_DEBUG
		if (argo_debug[D_EON]) {
			printf("eoninput to clnl IFQ\n");
		}
#endif
		ifq = &clnlintrq;
		s = splimp();
		if (IF_QFULL(ifq)) {
			IF_DROP(ifq);
			m_freem(m);
			eonifp->if_iqdrops++;
			eonifp->if_ipackets--;
			splx(s);
			return;
		}
		IF_ENQUEUE(ifq, m);
#ifdef ARGO_DEBUG
		if (argo_debug[D_EON]) {
			printf(
			       "%p enqueued on clnp Q: m_len 0x%x m_type 0x%x m_data %p\n",
			       m, m->m_len, m->m_type, m->m_data);
			dump_buf(mtod(m, caddr_t), m->m_len);
		}
#endif
		schednetisr(NETISR_ISO);
		splx(s);
	}
}

void *
eonctlinput(cmd, sa, dummy)
	int             cmd;
	struct sockaddr *sa;
	void *dummy;
{
	struct sockaddr_in *sin = (struct sockaddr_in *) sa;
#ifdef ARGO_DEBUG
	if (argo_debug[D_EON]) {
		printf("eonctlinput: cmd 0x%x addr: ", cmd);
		dump_isoaddr((struct sockaddr_iso *) sin);
		printf("\n");
	}
#endif

	if (cmd < 0 || cmd >= PRC_NCMDS)
		return NULL;

	IncStat(es_icmp[cmd]);
	switch (cmd) {

	case PRC_QUENCH:
	case PRC_QUENCH2:
		/* TODO: set the dec bit */
		break;
	case PRC_TIMXCEED_REASS:
	case PRC_ROUTEDEAD:
	case PRC_HOSTUNREACH:
	case PRC_UNREACH_NET:
	case PRC_IFDOWN:
	case PRC_UNREACH_HOST:
	case PRC_HOSTDEAD:
	case PRC_TIMXCEED_INTRANS:
		/* TODO: mark the link down */
		break;

	case PRC_UNREACH_PROTOCOL:
	case PRC_UNREACH_PORT:
	case PRC_UNREACH_SRCFAIL:
	case PRC_REDIRECT_NET:
	case PRC_REDIRECT_HOST:
	case PRC_REDIRECT_TOSNET:
	case PRC_REDIRECT_TOSHOST:
	case PRC_MSGSIZE:
	case PRC_PARAMPROB:
		/* printf("eonctlinput: ICMP cmd 0x%x\n", cmd ); */
		break;
	}
	return NULL;
}

#endif

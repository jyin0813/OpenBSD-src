/*	$OpenBSD: print.c,v 1.5 2004/02/24 15:35:56 henning Exp $	*/

/* Turn data structures into printable text. */

/*
 * Copyright (c) 1995, 1996, 1997, 1998 The Internet Software Consortium.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of The Internet Software Consortium nor the names
 *    of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INTERNET SOFTWARE CONSORTIUM AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNET SOFTWARE CONSORTIUM OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This software has been written for the Internet Software Consortium
 * by Ted Lemon <mellon@fugue.com> in cooperation with Vixie
 * Enterprises.  To learn more about the Internet Software Consortium,
 * see ``http://www.vix.com/isc''.  To learn more about Vixie
 * Enterprises, see ``http://www.vix.com''.
 */

#include "dhcpd.h"

char *
print_hw_addr(int htype, int hlen, unsigned char *data)
{
	static char habuf[49];
	char *s;
	int i;

	if (htype == 0 || hlen == 0)
		goto bad;
	else {
		int slen = sizeof(habuf);
		s = habuf;
		for (i = 0; i < hlen; i++) {
			int j;
			j = snprintf(s, slen, "%02x", data[i]);
			if (j <= 0)
				goto bad;

			s += strlen(s);
			slen -= strlen(s) + 1;
			*s++ = ':';
		}
		*--s = 0;
	}
	return (habuf);
bad:
	strlcpy(habuf, "<null>", sizeof(habuf));
	return (habuf);

}

void
dump_packet(struct packet *tp)
{
	struct dhcp_packet *tdp = tp->raw;

	debug("packet length %d", tp->packet_length);
	debug("op = %d  htype = %d  hlen = %d  hops = %d",
	    tdp->op, tdp->htype, tdp->hlen, tdp->hops);
	debug("xid = %x  secs = %d  flags = %x",
	    tdp->xid, tdp->secs, tdp->flags);
	debug("ciaddr = %s", inet_ntoa(tdp->ciaddr));
	debug("yiaddr = %s", inet_ntoa(tdp->yiaddr));
	debug("siaddr = %s", inet_ntoa(tdp->siaddr));
	debug("giaddr = %s", inet_ntoa(tdp->giaddr));
	debug("chaddr = %02x:%02x:%02x:%02x:%02x:%02x",
	    ((unsigned char *)tdp->chaddr)[0],
	    ((unsigned char *)tdp->chaddr)[1],
	    ((unsigned char *)tdp->chaddr)[2],
	    ((unsigned char *)tdp->chaddr)[3],
	    ((unsigned char *)tdp->chaddr)[4],
	    ((unsigned char *)tdp->chaddr)[5]);
	debug("filename = %s", tdp->file);
	debug("server_name = %s", tdp->sname);
	if (tp->options_valid) {
		int i;

		for (i = 0; i < 256; i++)
			if (tp->options[i].data)
				debug("  %s = %s",
				    dhcp_options[i].name,
				    pretty_print_option(i,
					tp->options[i].data,
					tp->options[i].len, 1, 1));
	}
	debug("%s", "");
}

void
dump_raw(unsigned char *buf, int len)
{
	int i, j;
	char lbuf[80];
	int llen = sizeof(lbuf);
	int lbix = 0;

	lbuf[0] = 0;

	for (i = 0; i < len; i++) {
		if ((i & 15) == 0) {
			if (lbix)
				note(lbuf);
			j = snprintf(lbuf, llen, "%03x:", i);
			if (j >= llen)
				return;
			lbix += j;
			llen -= j;
		} else if ((i & 7) == 0) {
			lbuf[lbix++] = ' ';
			len--;
		}
		j = snprintf(&lbuf[lbix], llen, " %02x", buf[i]);
		if (j >= llen)
			return;
		lbix += j;
		llen -= j;
	}
	note(lbuf);
}

void
hash_dump(struct hash_table *table)
{
	int i;
	struct hash_bucket *bp;

	if (!table)
		return;

	for (i = 0; i < table->hash_count; i++) {
		if (!table->buckets[i])
			continue;
		note("hash bucket %d:", i);
		for (bp = table->buckets[i]; bp; bp = bp->next)
			if (bp->len)
				dump_raw(bp->name, bp->len);
			else
				note((char *)bp->name);
	}
}

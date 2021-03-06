/*	$NetBSD: if_le.c,v 1.9 1995/11/25 01:31:09 cgd Exp $	*/

/*-
 * Copyright (c) 1995 Charles M. Hannum.  All rights reserved.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell and Rick Macklem.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
 *	@(#)if_le.c	8.2 (Berkeley) 11/16/93
 */

#include "bpfilter.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/syslog.h>
#include <sys/socket.h>
#include <sys/device.h>

#include <net/if.h>

#ifdef INET
#include <netinet/in.h>
#include <netinet/if_ether.h>
#endif

#include <machine/autoconf.h>
#include <machine/rpb.h>

#include <alpha/tc/tc.h>
#include <alpha/tc/asic.h>
#include <alpha/tc/if_levar.h>
#include <dev/ic/am7990reg.h>
#define	LE_NEED_BUF_CONTIG
#define	LE_NEED_BUF_GAP2
#define	LE_NEED_BUF_GAP16
#include <dev/ic/am7990var.h>

/* access LANCE registers */
void lewritereg();
#define	LERDWR(cntl, src, dst)	{ (dst) = (src); wbflush(); }
#define	LEWREG(src, dst)	lewritereg(&(dst), (src))

#define LE_OFFSET_RAM		0x0
#define LE_OFFSET_LANCE		0x100000
#define LE_OFFSET_ROM		0x1c0000

extern caddr_t le_iomem;

#define	LE_SOFTC(unit)	lecd.cd_devs[unit]
#define	LE_DELAY(x)	DELAY(x)

int lematch __P((struct device *, void *, void *));
void leattach __P((struct device *, struct device *, void *));
int leintr __P((void *));

struct cfdriver lecd = {
	NULL, "le", lematch, leattach, DV_IFNET, sizeof (struct le_softc)
};

integrate void
lewrcsr(sc, port, val)
	struct le_softc *sc;
	u_int16_t port, val;
{
	struct lereg1 *ler1 = sc->sc_r1;

	LEWREG(port, ler1->ler1_rap);
	LERDWR(port, val, ler1->ler1_rdp);
}

integrate u_int16_t
lerdcsr(sc, port)
	struct le_softc *sc;
	u_int16_t port;
{
	struct lereg1 *ler1 = sc->sc_r1;
	u_int16_t val;

	LEWREG(port, ler1->ler1_rap);
	LERDWR(0, ler1->ler1_rdp, val);
	return (val);
}

int
lematch(parent, match, aux)
	struct device *parent;
	void *match, *aux;
{
	struct cfdata *cf = match;
	struct confargs *ca = aux;
#ifdef notdef /* XXX */
	struct tc_cfloc *tc_locp;
	struct asic_cfloc *asic_locp;
#endif

#ifdef notdef /* XXX */
	tclocp = (struct tc_cfloc *)cf->cf_loc;
#endif

	/* XXX CHECK BUS */
	/* make sure that we're looking for this type of device. */
#ifdef notdef
	if (!BUS_MATCHNAME(ca, "PMAD-BA "))
#endif
	if (!BUS_MATCHNAME(ca, "lance"))
		return (0);

#ifdef notdef /* XXX */
	/* make sure the unit matches the cfdata */
	if ((cf->cf_unit != tap->ta_unit &&
	     tap->ta_unit != TA_ANYUNIT) ||
	    (tclocp->cf_slot != tap->ta_slot &&
	     tclocp->cf_slot != TC_SLOT_WILD) ||
	    (tclocp->cf_offset != tap->ta_offset &&
	     tclocp->cf_offset != TC_OFFSET_WILD))
		return (0);
#endif

	return (1);
}

void
leattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	register struct le_softc *sc = (void *)self;
	struct confargs *ca = aux;
	u_char *cp;
	int i;

	if (sc->sc_dev.dv_unit == 0 && (hwrpb->rpb_type == ST_DEC_3000_300 ||
	    hwrpb->rpb_type == ST_DEC_3000_500)) {
		/* It's on the system ASIC */
		volatile u_int *ldp;

		sc->sc_r1 = (struct lereg1 *)BUS_CVTADDR(ca);
		sc->sc_r1 = TC_DENSE_TO_SPARSE(sc->sc_r1);
		sc->sc_mem = (void *)le_iomem;
/* XXX */	cp = (u_char *)ASIC_SYS_ETHER_ADDRESS(asic_base);

		sc->sc_copytodesc = copytobuf_gap2;
		sc->sc_copyfromdesc = copyfrombuf_gap2;
		sc->sc_copytobuf = copytobuf_gap16;
		sc->sc_copyfrombuf = copyfrombuf_gap16;
		sc->sc_zerobuf = zerobuf_gap16;

		/*
		 * And enable Lance dma through the asic.
		 */
		ldp = (volatile u_int *)ASIC_REG_LANCE_DMAPTR(asic_base);
		*ldp = (((u_int64_t)le_iomem << 3) & ~(u_int64_t)0x1f) |
			(((u_int64_t)le_iomem >> 29) & 0x1f);
		*(volatile u_int *)ASIC_REG_CSR(asic_base) |=
		    ASIC_CSR_DMAEN_LANCE;
		wbflush();
	} else {
		/* It's on the turbochannel proper */
		sc->sc_r1 = (struct lereg1 *)
		    (BUS_CVTADDR(ca) + LE_OFFSET_LANCE);
		sc->sc_mem = (void *)
		    (BUS_CVTADDR(ca) + LE_OFFSET_RAM);
		cp = (u_char *)(BUS_CVTADDR(ca) + LE_OFFSET_ROM + 2);

		sc->sc_copytodesc = copytobuf_contig;
		sc->sc_copyfromdesc = copyfrombuf_contig;
		sc->sc_copytobuf = copytobuf_contig;
		sc->sc_copyfrombuf = copyfrombuf_contig;
		sc->sc_zerobuf = zerobuf_contig;
	}

	sc->sc_conf3 = 0;
	sc->sc_addr = 0;
	sc->sc_memsize = 65536;

	/*
	 * Get the ethernet address out of rom
	 */
	for (i = 0; i < sizeof(sc->sc_arpcom.ac_enaddr); i++) {
		sc->sc_arpcom.ac_enaddr[i] = *cp;
		cp += 4;
	}

	sc->sc_arpcom.ac_if.if_name = lecd.cd_name;
	leconfig(sc);

	BUS_INTR_ESTABLISH(ca, leintr, sc);
	/* XXX YEECH!!! */
	*(volatile u_int *)ASIC_REG_IMSK(asic_base) |= ASIC_INTR_LANCE;
	wbflush();
}

/*
 * Write a lance register port, reading it back to ensure success. This seems
 * to be necessary during initialization, since the chip appears to be a bit
 * pokey sometimes.
 */
void
lewritereg(regptr, val)
	register volatile u_short *regptr;
	register u_short val;
{
	register int i = 0;

	while (*regptr != val) {
		*regptr = val;
		wbflush();
		if (++i > 10000) {
			printf("le: Reg did not settle (to x%x): x%x\n", val,
			    *regptr);
			return;
		}
		DELAY(100);
	}
}

#include <dev/ic/am7990.c>

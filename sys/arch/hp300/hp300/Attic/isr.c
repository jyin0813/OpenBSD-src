/*	$OpenBSD: isr.c,v 1.2 1997/01/12 15:13:17 downsj Exp $	*/
/*	$NetBSD: isr.c,v 1.5 1996/12/09 17:38:25 thorpej Exp $	*/

/*-
 * Copyright (c) 1996 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Adam Glass, Gordon W. Ross, and Jason R. Thorpe.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Link and dispatch interrupts.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/vmmeter.h>
#include <net/netisr.h>

#include <machine/cpu.h>

#include <hp300/hp300/isr.h>

typedef LIST_HEAD(, isr) isr_list_t;
isr_list_t isr_list[NISR];

u_short	hp300_bioipl, hp300_netipl, hp300_ttyipl, hp300_impipl;

extern	int intrcnt[];		/* from locore.s */

void	isrcomputeipl __P((void));

void
isrinit()
{
	int i;

	/* Initialize the ISR lists. */
	for (i = 0; i < NISR; ++i) {
		LIST_INIT(&isr_list[i]);
	}

	/* Default interrupt priorities. */
	hp300_bioipl = hp300_netipl = hp300_ttyipl = hp300_impipl =
	    (PSL_S|PSL_IPL3);
}

/*
 * Scan all of the ISRs, recomputing the interrupt levels for the spl*()
 * calls.  This doesn't have to be fast.
 */
void
isrcomputeipl()
{
	struct isr *isr;
	int ipl;

	/* Start with low values. */
	hp300_bioipl = hp300_netipl = hp300_ttyipl = hp300_impipl =
	    (PSL_S|PSL_IPL3);

	for (ipl = 0; ipl < NISR; ipl++) {
		for (isr = isr_list[ipl].lh_first; isr != NULL;
		    isr = isr->isr_link.le_next) {
			/*
			 * Bump up the level for a given priority,
			 * if necessary.
			 */
			switch (isr->isr_priority) {
			case ISRPRI_BIO:
				if (ipl > PSLTOIPL(hp300_bioipl))
					hp300_bioipl = IPLTOPSL(ipl);
				break;

			case ISRPRI_NET:
				if (ipl > PSLTOIPL(hp300_netipl))
					hp300_netipl = IPLTOPSL(ipl);
				break;

			case ISRPRI_TTY:
			case ISRPRI_TTYNOBUF:
				if (ipl > PSLTOIPL(hp300_ttyipl))
					hp300_ttyipl = IPLTOPSL(ipl);
				break;

			default:
				printf("priority = %d\n", isr->isr_priority);
				panic("isrcomputeipl: bad priority");
			}
		}
	}

	/*
	 * Enforce `bio <= net <= tty <= imp'
	 */

	if (hp300_netipl < hp300_bioipl)
		hp300_netipl = hp300_bioipl;

	if (hp300_ttyipl < hp300_netipl)
		hp300_ttyipl = hp300_netipl;

	if (hp300_impipl < hp300_ttyipl)
		hp300_impipl = hp300_ttyipl;
}

void
isrprintlevels()
{

#ifdef DEBUG
	printf("psl: bio = 0x%x, net = 0x%x, tty = 0x%x, imp = 0x%x\n",
	    hp300_bioipl, hp300_netipl, hp300_ttyipl, hp300_impipl);
#endif

	printf("interrupt levels: bio = %d, net = %d, tty = %d\n",
	    PSLTOIPL(hp300_bioipl), PSLTOIPL(hp300_netipl),
	    PSLTOIPL(hp300_ttyipl));
}

/*
 * Establish an interrupt handler.
 * Called by driver attach functions.
 */
void *
isrlink(func, arg, ipl, priority)
	int (*func) __P((void *));
	void *arg;
	int ipl;
	int priority;
{
	struct isr *newisr, *curisr;
	isr_list_t *list;

	if ((ipl < 0) || (ipl >= NISR))
		panic("isrlink: bad ipl %d", ipl);

	newisr = (struct isr *)malloc(sizeof(struct isr), M_DEVBUF, M_NOWAIT);
	if (newisr == NULL)
		panic("isrlink: can't allocate space for isr");

	/* Fill in the new entry. */
	newisr->isr_func = func;
	newisr->isr_arg = arg;
	newisr->isr_ipl = ipl;
	newisr->isr_priority = priority;

	/*
	 * Some devices are particularly sensitive to interrupt
	 * handling latency.  The DCA, for example, can lose many
	 * characters if its interrupt isn't handled with reasonable
	 * speed.
	 *
	 * To work around this problem, each device can give itself a
	 * "priority".  An unbuffered DCA would give itself a higher
	 * priority than a SCSI device, for example.
	 *
	 * This is necessary because of the flat spl scheme employed by
	 * the hp300.  Each device can be set from ipl 3 to ipl 5, which
	 * in turn means that splbio, splnet, and spltty must all be at
	 * spl5.
	 *
	 * Don't blame me...I just work here.
	 */

	/*
	 * Get the appropriate ISR list.  If the list is empty, no
	 * additional work is necessary; we simply insert ourselves
	 * at the head of the list.
	 */
	list = &isr_list[ipl];
	if (list->lh_first == NULL) {
		LIST_INSERT_HEAD(list, newisr, isr_link);
		goto compute;
	}

	/*
	 * A little extra work is required.  We traverse the list
	 * and place ourselves after any ISRs with our current (or
	 * higher) priority.
	 */
	for (curisr = list->lh_first; curisr->isr_link.le_next != NULL;
	    curisr = curisr->isr_link.le_next) {
		if (newisr->isr_priority > curisr->isr_priority) {
			LIST_INSERT_BEFORE(curisr, newisr, isr_link);
			goto compute;
		}
	}

	/*
	 * We're the least important entry, it seems.  We just go
	 * on the end.
	 */
	LIST_INSERT_AFTER(curisr, newisr, isr_link);

 compute:
	/* Compute new interrupt levels. */
	isrcomputeipl();
	return (newisr);
}

/*
 * Disestablish an interrupt handler.
 */
void
isrunlink(arg)
	void *arg;
{
	struct isr *isr = arg;

	LIST_REMOVE(isr, isr_link);
	free(isr, M_DEVBUF);
	isrcomputeipl();
}

/*
 * This is the dispatcher called by the low-level
 * assembly language interrupt routine.
 */
void
isrdispatch(evec)
	int evec;		/* format | vector offset */
{
	struct isr *isr;
	isr_list_t *list;
	int handled, ipl, vec;
	static int straycount, unexpected;

	vec = (evec & 0xfff) >> 2;
	if ((vec < ISRLOC) || (vec >= (ISRLOC + NISR)))
		panic("isrdispatch: bad vec 0x%x\n");
	ipl = vec - ISRLOC;

	intrcnt[ipl]++;
	cnt.v_intr++;

	list = &isr_list[ipl];
	if (list->lh_first == NULL) {
		printf("intrhand: ipl %d unexpected\n", ipl);
		if (++unexpected > 10)
			panic("isrdispatch: too many unexpected interrupts");
		return;
	}

	/* Give all the handlers a chance. */
	for (isr = list->lh_first ; isr != NULL; isr = isr->isr_link.le_next)
		handled |= (*isr->isr_func)(isr->isr_arg);

	if (handled)
		straycount = 0;
	else if (++straycount > 50)
		panic("isrdispatch: too many stray interrupts");
	else
		printf("isrdispatch: stray level %d interrupt\n", ipl);
}

/*
 * XXX Why on earth isn't this in a common file?!
 */
void
netintr()
{
#ifdef INET
	if (netisr & (1 << NETISR_ARP)) {
		netisr &= ~(1 << NETISR_ARP);
		arpintr();
	}
	if (netisr & (1 << NETISR_IP)) {
		netisr &= ~(1 << NETISR_IP);
		ipintr();
	}
#endif
#ifdef NS
	if (netisr & (1 << NETISR_NS)) {
		netisr &= ~(1 << NETISR_NS);
		nsintr();
	}
#endif
#ifdef ISO
	if (netisr & (1 << NETISR_ISO)) {
		netisr &= ~(1 << NETISR_ISO);
		clnlintr();
	}
#endif
#ifdef CCITT
	if (netisr & (1 << NETISR_CCITT)) {
		netisr &= ~(1 << NETISR_CCITT);
		ccittintr();
	}
#endif
#include "ppp.h"
#if NPPP > 0
	if (netisr & (1 << NETISR_PPP)) {
		netisr &= ~(1 << NETISR_PPP);
		pppintr();
	}
#endif
}

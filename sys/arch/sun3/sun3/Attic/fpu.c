/*	$OpenBSD: fpu.c,v 1.8 2002/03/14 01:26:47 millert Exp $	*/
/*	$NetBSD: fpu.c,v 1.9 1996/11/20 18:57:29 gwr Exp $	*/

/*-
 * Copyright (c) 1996 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Gordon W. Ross.
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
 * Floating Point Unit (MC68881)
 * Probe for the FPU at autoconfig time.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/device.h>

#include <machine/control.h>
#include <machine/cpu.h>
#include <machine/frame.h>
#include <machine/machdep.h>
#include <machine/mon.h>
#include <machine/psl.h>

#include "interreg.h"

static char *fpu_descr[] = {
#ifdef	FPU_EMULATE
	"emulator", 		/* 0 */
#else
	"no math support",	/* 0 */
#endif
	"mc68881",			/* 1 */
	"mc68882",			/* 2 */
	"?" };

static int fpu_probe(void);

void
initfpu()
{
	char *descr;
	int enab_reg;

	/* Set the FPU bit in the "system enable register" */
	enab_reg = get_control_byte((char *) SYSTEM_ENAB);
	enab_reg |= SYSTEM_ENAB_FPP;
	set_control_byte((char *) SYSTEM_ENAB, enab_reg);

	fputype = fpu_probe();
	if ((0 <= fputype) && (fputype <= 2))
		descr = fpu_descr[fputype];
	else
		descr = "unknown type";

	printf("fpu: %s\n", descr);

	if (fputype == 0) {
		/* Might as well turn the enable bit back off. */
		enab_reg = get_control_byte((char *) SYSTEM_ENAB);
		enab_reg &= ~SYSTEM_ENAB_FPP;
		set_control_byte((char *) SYSTEM_ENAB, enab_reg);
	}
}

static int
fpu_probe()
{
	label_t	faultbuf;
	struct fpframe null_fpframe;

	nofault = &faultbuf;
	if (setjmp(&faultbuf)) {
		nofault = NULL;
		return(0);
	}
	bzero(&null_fpframe, sizeof(null_fpframe));
	m68881_restore(&null_fpframe);
	nofault = NULL;
	return(1);
}

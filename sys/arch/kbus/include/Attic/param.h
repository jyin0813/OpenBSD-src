/*	$OpenBSD: param.h,v 1.1 1997/10/14 07:25:31 gingold Exp $	*/
/*	$NetBSD: param.h,v 1.12 1995/06/26 06:56:05 cgd Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
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
 *	@(#)param.h	5.8 (Berkeley) 6/28/91
 */

/*
 * Machine dependent constants for NS 32532.
 *
 *   Derived from the i386 param.h by Phil Nelson.
 *
 */

#ifndef _MACHINE_PARAM_H_
#define _MACHINE_PARAM_H_

#ifdef _KERNEL
#include <machine/cpu.h>
#endif

#define MACHINE 	"Series5"
#define _MACHINE 	Series5
#define MACHINE_ARCH	"sparc"
#define _MACHINE_ARCH	sparc
#define MID_MACHINE	MID_SPARC

/*
 * Round p (pointer or byte index) up to a correctly-aligned value
 * for all data types (int, long, ...).   The result is u_int and
 * must be cast to any desired pointer type.
 */
#define ALIGNBYTES	(sizeof(double) - 1)
#define	ALIGN(p)	(((u_int)(p) + ALIGNBYTES) &~ ALIGNBYTES)

#define	PGSHIFT		13		/* LOG2(NBPG) */
#define NBPG		(1 << PGSHIFT)	/* bytes/page */
#define	PGOFSET		(NBPG-1)	/* byte offset into page */
#define	NPTEPG		(NBPG/(sizeof (struct pte)))

#define	SEGSHIFT	24		/* LOG2(NBSEG) */
#define NBSEG		(1 << SEGSHIFT)	/* bytes/segments.  */
#define	SEGOFSET	(NBSEG-1)	/* byte offset into segment  */

#define	KERNBASE	0xff000000	/* start of kernel .text seg. */
#define	BTOPKERNBASE	((u_long)KERNBASE >> PGSHIFT)

#define	DEV_BSIZE	512
#define	DEV_BSHIFT	9		/* log2(DEV_BSIZE) */
#define BLKDEV_IOSIZE	4096		/* Was 2048 (pan) */
#define	MAXPHYS		(64 * 1024)	/* max raw I/O transfer size */

#define	CLSIZE		1
#define	CLSIZELOG2	0

/* NOTE: SSIZE, SINCR and UPAGES must be multiples of CLSIZE */
#define	SSIZE	1		/* initial stack size/NBPG */
#define	SINCR	1		/* increment of stack/NBPG */

#define	UPAGES	2		/* pages of u-area */
#define USPACE (UPAGES * NBPG)

/*
 * Constants related to network buffer management.
 * MCLBYTES must be no larger than CLBYTES (the software page size), and,
 * on machines that exchange pages of input or output buffers with mbuf
 * clusters (MAPPED_MBUFS), MCLBYTES must also be an integral multiple
 * of the hardware page size.
 */
#ifndef	MSIZE
#define	MSIZE		128		/* size of an mbuf */
#endif	/* MSIZE */

#ifndef	MCLSHIFT
#define	MCLSHIFT	11		/* convert bytes to m_buf clusters */
#endif	/* MCLSHIFT */
#define	MCLBYTES	(1 << MCLSHIFT)	/* size of a m_buf cluster */
#define	MCLOFSET	(MCLBYTES - 1)	/* offset within a m_buf cluster */

#ifndef NMBCLUSTERS
#define	NMBCLUSTERS	512		/* map size, max cluster allocation */
#endif

/*
 * Size of kernel malloc arena in CLBYTES-sized logical pages
 */ 
#ifndef NKMEMCLUSTERS
#define	NKMEMCLUSTERS	(2048*1024/CLBYTES)
#endif

/*
 * Some macros for units conversion
 */

/* pages ("clicks") to disk blocks */
#define	ctod(x)		((x) << (PGSHIFT - DEV_BSHIFT))
#define	dtoc(x)		((x) >> (PGSHIFT - DEV_BSHIFT))

/* clicks to bytes */
#define	ctob(x)		((x) << PGSHIFT)
#define	btoc(x)		(((x) + PGOFSET) >> PGSHIFT)

/* bytes to disk blocks */
#define	btodb(x)	((x) >> DEV_BSHIFT)
#define	dbtob(x)	((x) << DEV_BSHIFT)


/*
 * Map a ``block device block'' to a file system block.
 * This should be device dependent, and will be if we
 * add an entry to cdevsw/bdevsw for that purpose.
 * For now though just use DEV_BSIZE.
 */
#define	bdbtofsb(bn)	((bn) / (BLKDEV_IOSIZE/DEV_BSIZE))


/*
 * Mach derived conversion macros
 */
#define series5_round_pdr(x)	((((unsigned)(x)) + NBPDR - 1) & ~(NBPDR-1))
#define series5_trunc_pdr(x)	((unsigned)(x) & ~(NBPDR-1))
#define series5_round_page(x)	((((unsigned)(x)) + NBPG - 1) & ~(NBPG-1))
#define series5_trunc_page(x)	((unsigned)(x) & ~(NBPG-1))
#define series5_btod(x)		((unsigned)(x) >> PDRSHIFT)
#define series5_dtob(x)		((unsigned)(x) << PDRSHIFT)
#define series5_btop(x)		((unsigned)(x) >> PGSHIFT)
#define series5_ptob(x)		((unsigned)(x) << PGSHIFT)

#ifndef __ASSEMBLER__
#ifndef _KERNEL
#define	DELAY(n)	{ volatile int N = (n); while (--N > 0); }
#else
extern void delay __P((unsigned int));
#define DELAY(n)		delay (n)
#endif

#if 0
/* Macros to read and write from absolute addresses.  */
#define WR_ADR(type,adr,val)	(*((volatile type *)(adr))=(val))
#define RD_ADR(type,adr)	(*((volatile type *)(adr)))
#endif
#endif

#endif

/*	$OpenBSD: kdbparam.h,v 1.2 1996/06/06 23:06:44 deraadt Exp pefo $	*/
/*	$NetBSD: kdbparam.h,v 1.4 1994/10/26 21:09:42 cgd Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell.
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
 *	@(#)kdbparam.h	8.1 (Berkeley) 6/10/93
 */

/*
 * Machine dependent definitions for kdb.
 */

#if BYTE_ORDER == LITTLE_ENDIAN
#define kdbshorten(w)	((w) & 0xFFFF)
#define	kdbbyte(w)	((w) & 0xFF)
#define	kdbitol(a,b)	((long)(((b) << 16) | ((a) & 0xFFFF)))
#define	kdbbtol(a)	((long)(a))
#endif

#define LPRMODE		"%R"
#define OFFMODE		"+%R"

#define	SETBP(ins)	BREAK_BRKPT

/* return the program counter value modified if we are in a delay slot */
#define	kdbgetpc(pcb)		(kdbvar[kdbvarchk('t')] < 0 ? \
	(pcb).pcb_regs[34] + 4 : (pcb).pcb_regs[34])
#define	kdbishiddenreg(p)	((p) >= &kdbreglist[33])
#define	kdbisbreak(type)	(((type) & CR_EXC_CODE) == 0x24)

/* check for address wrap around */
#define	kdbaddrwrap(addr,newaddr)	(((addr)^(newaddr)) >> 31)

/* declare machine dependent routines defined in kadb.c */
void	kdbprinttrap __P((unsigned, unsigned));
void	kdbsetsstep __P((void));
void	kdbclrsstep __P((void));
void	kdbreadc __P((char *));
void	kdbwrite __P((char *, int));
void	kdbprintins __P((int, long));
void	kdbstacktrace __P((int));
char	*kdbmalloc __P((int));

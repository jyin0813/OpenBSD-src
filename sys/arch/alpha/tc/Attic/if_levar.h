/*	$NetBSD: if_levar.h,v 1.1 1995/06/28 01:48:26 cgd Exp $	*/

/*-
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
 *	@(#)if_lereg.h	8.1 (Berkeley) 6/10/93
 */

/* Local Area Network Controller for Ethernet (LANCE) registers */
struct lereg1 {
	volatile u_int16_t	ler1_rdp;	/* data port */
	int16_t	pad0;
	int32_t	pad1;
	volatile u_int16_t	ler1_rap;	/* register select port */
	int16_t	pad2;
	int32_t	pad3;
};

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * arpcom.ac_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 */
struct le_softc {
	struct	device sc_dev;		/* base structure */
	struct	arpcom sc_arpcom;	/* Ethernet common part */

	void	(*sc_copytodesc)	/* Copy to descriptor */
		    __P((struct le_softc *, void *, int, int));
	void	(*sc_copyfromdesc)	/* Copy from descriptor */
		    __P((struct le_softc *, void *, int, int));

	void	(*sc_copytobuf)		/* Copy to buffer */
		    __P((struct le_softc *, void *, int, int));
	void	(*sc_copyfrombuf)	/* Copy from buffer */
		    __P((struct le_softc *, void *, int, int));
	void	(*sc_zerobuf)		/* and Zero bytes in buffer */
		    __P((struct le_softc *, int, int));

	u_int16_t sc_conf3;		/* CSR3 value */

	void	*sc_mem;		/* base addr of RAM -- CPU's view */
	u_long	sc_addr;		/* base addr of RAM -- LANCE's view */
	u_long	sc_memsize;		/* size of RAM */

	int	sc_nrbuf;		/* number of receive buffers */
	int	sc_ntbuf;		/* number of transmit buffers */
	int	sc_last_rd;
	int	sc_first_td, sc_last_td, sc_no_td;

	int	sc_initaddr;
	int	sc_rmdaddr;
	int	sc_tmdaddr;
	int	sc_rbufaddr;
	int	sc_tbufaddr;

#ifdef LEDEBUG
	int	sc_debug;
#endif

	struct	lereg1 *sc_r1;		/* LANCE registers */
};

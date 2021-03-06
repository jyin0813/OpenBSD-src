/*	$OpenBSD: kbd.h,v 1.9 2002/03/14 03:16:00 millert Exp $	*/
/*	$NetBSD: kbd.h,v 1.6 1996/03/31 22:21:35 pk Exp $ */

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
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
 *	@(#)kbd.h	8.1 (Berkeley) 6/11/93
 */

/*
 * Keyboard `registers'.  (This should be called kbd_reg.h but we need to
 * be compatible.)
 */

/*
 * Control codes sent from type 2, 3, and 4 keyboards.
 *
 * Note that KBD_RESET is followed by a keyboard ID, while KBD_IDLE is not.
 * KBD_IDLE does not take the place of any `up' transitions (it merely occurs
 * after them).
 */
#define	KBD_RESET	0xff		/* keyboard `reset' response */
#define	KBD_IDLE	0x7f		/* keyboard `all keys are up' code */
#define	KBD_LAYOUT	0xfe		/* keyboard `get layout' response */

/* Keyboard IDs */
#define	KB_SUN2		2		/* type 2 keyboard */
#define	KB_SUN3		3		/* type 3 keyboard */
#define	KB_SUN4		4		/* type 4 keyboard */

/* Key codes are in 0x00..0x7e; KBD_UP is set if the key goes up */
#define	KBD_KEYMASK	0x7f		/* keyboard key mask */
#define	KBD_UP		0x80		/* keyboard `up' transition */

/* Keyboard codes needed to recognize the L1-A sequence */
#define	KBD_L1		1		/* keyboard code for `L1' key */
#define	KBD_A		77		/* keyboard code for `A' key */

/* Control codes sent to the various keyboards */
#define	KBD_CMD_RESET	1		/* reset keyboard */
#define	KBD_CMD_BELL	2		/* turn bell on */
#define	KBD_CMD_NOBELL	3		/* turn bell off */
#define	KBD_CMD_CLICK	10		/* turn keyclick on */
#define	KBD_CMD_NOCLICK	11		/* turn keyclick off */
#define KBD_CMD_SETLED	14		/* set LED state (type 4 kbd) */
#define KBD_CMD_GLAYOUT	15		/* get DIP switch (type 4 kbd) */

#define	LED_NUM_LOCK	0x1
#define	LED_COMPOSE	0x2
#define	LED_SCROLL_LOCK	0x4
#define	LED_CAPS_LOCK	0x8

#define	CAPSLOCK	0
#define	SHIFTLOCK	1
#define	LEFTSHIFT	2
#define	RIGHTSHIFT	3
#define	LEFTCTRL	4
#define	RIGHTCTRL	5
#define	ALTGRAPH	9
#define	ALT		10
#define	NUMLOCK		11

#define	SYSTEMBIT	1

#define	SHIFTKEYS	0x100
#define	BUCKYBITS	0x200
#define	FUNNY		0x300
#define	NOP		0x300
#define	OOPS		0x301
/*	HOLE		0x302 defined in kbio.h */
#define	RESET		0x306
#define	ERROR		0x307
#define	IDLE		0x308
#define	COMPOSE		0x309
#define	NONL		0x30a

#define	FA_CLASS	0x400
#define	FA_UMLAUT	0x400
#define	FA_CFLEX	0x401
#define	FA_TILDE	0x402
#define	FA_CEDILLA	0x403
#define	FA_ACUTE	0x404
#define	FA_GRAVE	0x405
#define	FA_CLASS_LAST	0x405

#define	STRING		0x500
#define	KTAB_STRLEN	10
#define	HOMEARROW	0x00
#define	UPARROW		0x01
#define	DOWNARROW	0x02
#define	LEFTARROW	0x03
#define	RIGHTARROR	0x04

#define	FUNCKEYS	0x600
#define	LEFTFUNC	0x600
#define	RIGHTFUNC	0x610
#define	TOPFUNC		0x620
#define	BOTTOMFUNC	0x630
#define	LF(n)		(LEFTFUNC+(n)-1)
#define	RF(n)		(RIGHTFUNC+(n)-1)
#define	TF(n)		(TOPFUNC+(n)-1)
#define	BF(n)		(BOTTOMFUNC+(n)-1)

#define	PADKEYS		0x700
#define	PADEQUAL	0x700
#define	PADSLASH	0x701
#define	PADSTAR		0x702
#define	PADMINUS	0x703
#define	PADSEP		0x704
#define	PAD7		0x705
#define	PAD8		0x706
#define	PAD9		0x707
#define	PADPLUS		0x708
#define	PAD4		0x709
#define	PAD5		0x70A
#define	PAD6		0x70B
#define	PAD1		0x70C
#define	PAD2		0x70D
#define	PAD3		0x70E
#define	PAD0		0x70F
#define	PADDOT		0x710
#define	PADENTER	0x711

#ifdef _KERNEL
void 	kbd_serial(struct tty *, void (*)(struct tty *), void (*)(struct tty *));
void 	ms_serial(struct tty *, void (*)(struct tty *), void (*)(struct tty *));
void 	kbd_rint(int);
void 	ms_rint(int);
void 	kbd_ascii(struct tty *);
int	kbd_docmd(int, int);
#endif

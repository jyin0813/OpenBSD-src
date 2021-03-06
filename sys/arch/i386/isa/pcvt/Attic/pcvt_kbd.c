/*	$OpenBSD: pcvt_kbd.c,v 1.41 2001/05/16 12:49:45 ho Exp $	*/

/*
 * Copyright (c) 1992, 1995 Hellmuth Michaelis and Joerg Wunsch.
 *
 * Copyright (c) 1992, 1993 Brian Dunford-Shore and Holger Veit.
 *
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz and Don Ahn.
 *
 * This code is derived from software contributed to 386BSD by
 * Holger Veit.
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
 *	This product includes software developed by Hellmuth Michaelis,
 *	Brian Dunford-Shore and Joerg Wunsch.
 * 4. The name authors may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @(#)pcvt_kbd.c, 3.32, Last Edit-Date: [Tue Oct  3 11:19:48 1995]
 *
 */

/*---------------------------------------------------------------------------*
 *
 *	pcvt_kbd.c	VT220 Driver Keyboard Interface Code
 *	----------------------------------------------------
 *	-hm	------------ Release 3.00 --------------
 *	-hm	integrating NetBSD-current patches
 *	-jw	introduced kbd_emulate_pc() if scanset > 1
 *	-hm	patch from joerg for timeout in kbd_emulate_pc()
 *	-hm	starting to implement alt-shift/ctrl key mappings
 *	-hm	Gateway 2000 Keyboard fix from Brian Moore
 *	-hm	some #if adjusting for NetBSD 0.9
 *	-hm	split off pcvt_kbd.h
 *	-hm	applying Joerg's patches for FreeBSD 2.0
 *	-hm	patch from Martin, PCVT_NO_LED_UPDATE
 *	-hm	PCVT_VT220KEYB patches from Lon Willet
 *	-hm	PR #399, patch from Bill Sommerfeld: Return with PCVT_META_ESC
 *	-hm	allow keyboard-less kernel boot for serial consoles and such ..
 *	-hm	patch from Lon Willett for led-update and showkey()
 *	-hm	patch from Lon Willett to fix mapping of Control-R scancode
 *	-hm	delay patch from Martin Husemann after port-i386 ml-discussion
 *	-hm	added PCVT_NONRESP_KEYB_TRY definition to doreset()
 *	-hm	keyboard code bugfix from Jukka A. Ukkonen (ukkonen@csc.fi)
 *	-hm	---------------- Release 3.30 -----------------------
 *	-hm	patch from Frank van der Linden for keyboard state per VT
 *	-hm	removed KBDGLEDS and KBDSLEDS ioctls
 *	-hm	---------------- Release 3.32 -----------------------
 *
 *---------------------------------------------------------------------------*/

#include "vt.h"
/* #if NVT > 0 */

#include <sys/ttydefaults.h>	/* CSTOP, CSTART for XON/XOFF scrlck emul. */
#include "pcvt_hdr.h"		/* global include */

#define LEDSTATE_UPDATE_PENDING (1 << 3)
#define LEDSTATE_UPDATING	(1 << 4)

static void fkey1(void), fkey2(void),  fkey3(void),  fkey4(void);
static void fkey5(void), fkey6(void),  fkey7(void),  fkey8(void);
static void fkey9(void), fkey10(void), fkey11(void), fkey12(void);

static void sfkey1(void), sfkey2(void),  sfkey3(void),  sfkey4(void);
static void sfkey5(void), sfkey6(void),  sfkey7(void),  sfkey8(void);
static void sfkey9(void), sfkey10(void), sfkey11(void), sfkey12(void);

static void cfkey1(void), cfkey2(void),  cfkey3(void),  cfkey4(void);
static void cfkey5(void), cfkey6(void),  cfkey7(void),  cfkey8(void);
static void cfkey9(void), cfkey10(void), cfkey11(void), cfkey12(void);

static inline int kbd_wait_output(void);
static inline int kbd_wait_input(void);
int kbd_response(void);

static void	doreset ( void );
static void	ovlinit ( int force );
static void 	settpmrate ( int rate );
static void	setlockkeys ( int snc );
static int	kbc_8042cmd ( int val );
static int	getokeydef ( unsigned key, struct kbd_ovlkey *thisdef );
static int 	getckeydef ( unsigned key, struct kbd_ovlkey *thisdef );
static int	rmkeydef ( int key );
static int	setkeydef ( struct kbd_ovlkey *data );
static u_char *	xlatkey2ascii( U_short key );

#if !PCVT_NO_LED_UPDATE
static int	ledstate  = LEDSTATE_UPDATE_PENDING;	/* keyboard led's */
#endif
static int	tpmrate   = KBD_TPD500|KBD_TPM100;
static u_char	altkpflag = 0;
static u_short	altkpval  = 0;

static u_short *scrollback_savedscreen = (u_short *)0;
static size_t scrnsv_size = (size_t)-1;
static void scrollback_save_screen ( void );
static void scrollback_restore_screen ( void );

extern int kbd_reset;

#include "pcvt_kbd.h"		/* tables etc */

/*---------------------------------------------------------------------------*
 *	function to switch to another virtual screen
 *---------------------------------------------------------------------------*/
static void
do_vgapage(int page)
{
	if (critical_scroll)		/* executing critical region ? */
		switch_page = page;	/* yes, auto switch later */
	else
		vgapage(page);		/* no, switch now */
}

/*
 * This code from Lon Willett enclosed in #if PCVT_UPDLED_LOSES_INTR is
 * disabled because it crashes FreeBSD 1.1.5.1 at boot time.
 * The cause is obviously that the timeout queue is not yet initialized
 * timeout is called from here the first time.
 * Anyway it is a pointer in the right direction so it is included for
 * reference here.
 */

#define PCVT_UPDLED_LOSES_INTR	0	/* disabled for now */

#if PCVT_UPDLED_LOSES_INTR

/*---------------------------------------------------------------------------*
 *	check for lost keyboard interrupts
 *---------------------------------------------------------------------------*/

/*
 * The two commands to change the LEDs generate two KEYB_R_ACK responses
 * from the keyboard, which aren't explicitly checked for (maybe they
 * should be?).  However, when a lot of other I/O is happening, one of
 * the interrupts sometimes gets lost (I'm not sure of the details of
 * how and why and what hardware this happens with).
 *
 * This is a real problem, because normally the keyboard is only polled
 * by pcrint(), and no more interrupts will be generated until the ACK
 * has been read.  So the keyboard is hung.  This code polls a little
 * while after changing the LEDs to make sure that this hasn't happened.
 *
 * XXX Quite possibly we should poll the kbd on a regular basis anyway,
 * in the interest of robustness.  It may be possible that interrupts
 * get lost other times as well.
 */
 /* Previous comment obsolete, update_led() now interrupt driven */

struct timeout kbd_led_intr_to;

static void
check_for_lost_intr (void *arg)
{
	if (inb(CONTROLLER_CTRL) & STATUS_OUTPBF) {
		int opri = spltty();
		(void)pcrint();
		splx(opri);
	}
}

#endif /* PCVT_UPDLED_LOSES_INTR */

/*---------------------------------------------------------------------------*
 *	update keyboard led's
 *---------------------------------------------------------------------------*/
void
update_led(u_char cause)
{
#if !PCVT_NO_LED_UPDATE
	/* Don't update LED's unless necessary. */

	int opri, new_ledstate;

	if (!keyboard_type) return; /* allow disconnected kbd operation */

	opri = spltty();
	new_ledstate = ((vsp->scroll_lock) | (vsp->num_lock * 2) |
			(vsp->caps_lock * 4));
#if 0
	if (new_ledstate != ledstate) {
#endif	/* because of switch_screen() and vgapage() changes */
		if ((cause == KBD_SCROLL) || (cause == KBD_NUM) ||
		    (cause == KBD_CAPS) ||
		    ((cause == 1) && (!do_initialization)) ||
		    ((cause == 2) && (!do_initialization)) ||
		    ((cause == KEYB_R_RESEND) &&
		    (ledstate == LEDSTATE_UPDATE_PENDING))) {

			if (kbd_cmd(KEYB_C_LEDS) != 0) {
				printf("pcvt: kbd led cmd timeout\n");
				goto bail;
			}
			ledstate = LEDSTATE_UPDATE_PENDING;
			if (cause == KEYB_R_RESEND)
				printf("pcvt: kbd led cmd resend\n");
			goto bail;
		}

		/*
		 * For some keyboards or keyboard controllers, it is an
		 * error to issue a command without waiting long enough
		 * for an ACK for the previous command.  The keyboard
		 * gets confused, and responds with KEYB_R_RESEND, but
		 * we ignore that.  Wait for the ACK here.  The busy
		 * waiting doesn't matter much, since we lose anyway by
		 * busy waiting to send the command.
		 *
		 * XXX actually wait for any response, since we can't
		 * handle normal scancodes here.
		 *
		 * XXX all this should be interrupt driven.  Issue only
		 * one command at a time wait for a ACK before proceeding.
		 * Retry after a timeout or on receipt of a KEYB_R_RESEND.
		 * KEYB_R_RESENDs seem to be guaranteed by working
		 * keyboard controllers with broken (or disconnected)
		 * keyboards.  There is another code for keyboard
		 * reconnects.  The keyboard hardware is very simple and
		 * well designed :-).
		 */
		/*
		 * Previous comment obsolete, update_led() now interrupt driven
		 */

		if (((cause == KEYB_R_ACK) &&
			(ledstate == LEDSTATE_UPDATE_PENDING)) ||
		    ((cause == KEYB_R_RESEND) &&
			(ledstate == LEDSTATE_UPDATING))) {

			if (kbd_cmd(new_ledstate) != 0) {
				printf("pcvt: kbd led data timeout\n");
				goto bail;
			}
			ledstate = LEDSTATE_UPDATING;
			if (cause == KEYB_R_RESEND)
				printf("pcvt:kbd led data resend\n");
			goto bail;
		}

		if ((cause == KEYB_R_ACK) && (ledstate == LEDSTATE_UPDATING)) {
			ledstate = new_ledstate;
			goto bail;
		}

#if PCVT_UPDLED_LOSES_INTR
		timeout_add(&kbd_led_intr_to, hz);
#endif /* PCVT_UPDLED_LOSES_INTR */
#if 0
	}
#endif	/* because of switch_screen() and vgapage() changes */
bail:
	splx(opri);
#endif /* !PCVT_NO_LED_UPDATE */
}

/*---------------------------------------------------------------------------*
 *	set typematic rate
 *---------------------------------------------------------------------------*/
static void
settpmrate(int rate)
{
	int opri, response1, response2;

	opri = spltty();
	tpmrate = rate & 0x7f;

	if (kbd_cmd(KEYB_C_TYPEM) != 0) {
		printf("pcvt: kbd tpm cmd timeout\n");
		goto fail;
	}
	response1 = kbd_response();		/* wait for ACK */

	if (kbd_cmd(tpmrate) != 0) {
		printf("pcvt: kbd tpm data timeout\n");
		goto fail;
	}
	response2 = kbd_response();		/* wait for ACK */

	if (response1 != KEYB_R_ACK || response2 != KEYB_R_ACK) {
		printf("pcvt: kbd tpm cmd not ack'd (resp %#x %#x)\n",
		   response1, response2);
	}
fail:
	splx(opri);
}

/*---------------------------------------------------------------------------*
 *	Pass command to keyboard controller (8042)
 *---------------------------------------------------------------------------*/
static inline int
kbd_wait_output()
{
	u_int i;

	/* > 100 msec */
	for (i = 100; i; i--) {
		if ((inb(CONTROLLER_CTRL) & STATUS_INPBF) == 0) {
			PCVT_KBD_DELAY();
			return (1);
		}
		DELAY(1000);
	}
	return (0);
}

static inline int
kbd_wait_input()
{
	u_int i;

	/* > 500 msec */
	for (i = 500; i; i--) {
		if ((inb(CONTROLLER_CTRL) & STATUS_OUTPBF) != 0) {
			PCVT_KBD_DELAY();
			return (1);
		}
		DELAY(1000);
	}
	return (0);
}

static int
kbc_8042cmd(int val)
{
	if (!kbd_wait_output())
		return (-1);
	outb(CONTROLLER_CTRL, val);

	return (0);
}

/*---------------------------------------------------------------------------*
 *	Pass command to keyboard itself
 *---------------------------------------------------------------------------*/
int
kbd_cmd(int val)
{
	if (!kbd_wait_output())
		return (-1);
	outb(CONTROLLER_DATA, val);

	return (0);
}

/*---------------------------------------------------------------------------*
 *	Read response from keyboard
 *	NB: make sure to call spltty() before kbd_cmd(), kbd_response().
 *---------------------------------------------------------------------------*/
int
kbd_response(void)
{
	u_char ch;

	if (!kbd_wait_input())
		return (-1);
	ch = inb(CONTROLLER_DATA);

	return (ch);
}

/*---------------------------------------------------------------------------*
 *	switch to/from PC scan code emulation mode
 *---------------------------------------------------------------------------*/
void
kbd_setmode(int mode)
{
#if PCVT_SCANSET > 1		/* switch only if we are running scancode 2 */
	int cmd;
#if PCVT_USEKBDSEC
	cmd = COMMAND_SYSFLG | COMMAND_IRQEN;
#else
	cmd = COMMAND_INHOVR | COMMAND_SYSFLG | COMMAND_IRQEN;
#endif

	if (mode == K_RAW)		/* switch to scancode 1 ? */
		cmd |= COMMAND_PCSCAN;	/*     yes, setup command */

	kbc_8042cmd(CONTR_WRITE);
	kbd_cmd(cmd);
	
#endif /* PCVT_SCANSET > 1 */

	if (mode == K_RAW)
		shift_down = meta_down = altgr_down = ctrl_down = 0;
}


#ifndef PCVT_NONRESP_KEYB_TRY
#define PCVT_NONRESP_KEYB_TRY	25	/* no of times to try to detect	*/
#endif					/* a nonresponding keyboard	*/

/*---------------------------------------------------------------------------*
 *	try to force keyboard into a known state ..
 *---------------------------------------------------------------------------*/
static
void doreset(void)
{
	int again = 0;
	int once = 0;
	int response, opri;
	unsigned int wait_retries, seen_negative_response;

	/* Enable interrupts and keyboard, etc. */
	if (kbc_8042cmd(CONTR_WRITE) != 0)
		printf("pcvt: timeout controller write cmd\n");

#if PCVT_USEKBDSEC		/* security enabled */

#  if PCVT_SCANSET == 2
#    define KBDINITCMD COMMAND_SYSFLG|COMMAND_IRQEN
#  else /* PCVT_SCANSET != 2 */
#    define KBDINITCMD COMMAND_PCSCAN|COMMAND_SYSFLG|COMMAND_IRQEN
#  endif /* PCVT_SCANSET == 2 */

#else /* ! PCVT_USEKBDSEC */	/* security disabled */

#  if PCVT_SCANSET == 2
#    define KBDINITCMD COMMAND_INHOVR|COMMAND_SYSFLG|COMMAND_IRQEN
#  else /* PCVT_SCANSET != 2 */
#    define KBDINITCMD COMMAND_PCSCAN|COMMAND_INHOVR|COMMAND_SYSFLG\
	|COMMAND_IRQEN
#  endif /* PCVT_SCANSET == 2 */

#endif /* PCVT_USEKBDSEC */

	if (kbd_cmd(KBDINITCMD) != 0)
		printf("pcvt: timeout writing kbd init cmd\n");

	/*
	 * Discard any stale keyboard activity.  The 0.1 boot code isn't
	 * very careful and sometimes leaves a KEYB_R_RESEND.
	 */
	while (1) {
		if (inb(CONTROLLER_CTRL) & STATUS_OUTPBF)
			kbd_response();
		else {
			DELAY(10000);
			if (!(inb(CONTROLLER_CTRL) & STATUS_OUTPBF))
				break;
		}
	}

	/* Start keyboard reset */
	opri = spltty();

	if (kbd_cmd(KEYB_C_RESET) != 0)
		printf("pcvt: kbd reset cmd timeout\n");

	/* Wait for the first response to reset and handle retries */
	while ((response = kbd_response()) != KEYB_R_ACK) {
		if (response < 0) {
			if (!again)	/* print message only once ! */
				printf("pcvt: response != ack\n");
			response = KEYB_R_RESEND;
		}
		if (response == KEYB_R_RESEND) {
			if (!again)	/* print message only once ! */
				printf("pcvt: got KEYB_R_RESEND\n");

			if (++again > PCVT_NONRESP_KEYB_TRY) {
				printf("pcvt: no kbd detected\n");
				keyboard_type = KB_UNKNOWN;
				splx(opri);
				return;
			}

			if ((kbd_cmd(KEYB_C_RESET) != 0) && (once == 0)) {
				once++;		/* print message only once ! */
				printf("pcvt: timeout for loop\n");
			}
		}
	}

	/* Wait for the second response to reset */

	wait_retries = seen_negative_response = 0;

	while ((response = kbd_response()) != KEYB_R_SELFOK) {
		/*
		 *  Let's be a little more tolerant here...
		 *  Receiving a -1 could indicate that the keyboard
		 *  is not ready to answer just yet.
		 *  Such cases have been noticed with e.g. Alps Membrane.
		 */

		if (response < 0)
			seen_negative_response = 1;

		if (seen_negative_response && (wait_retries >= 10)) {
			printf("pcvt: response != OK\n");

			/*
			 * If KEYB_R_SELFOK never arrives, the loop will
			 * finish here unless the keyboard babbles or
			 * STATUS_OUTPBF gets stuck.
			 */
			break;
		}
		wait_retries++;
	}

#if PCVT_SCANSET == 1
	/* 
	 * Pcvt has been compiled for scanset 1, which requires that
	 * the mainboard controller translates. If it is not able to,
	 * try to set the keyboard to XT mode so that pcvt will see AT
	 * scan codes after all. If it fails, we're out of luck.
	 */
	kbc_8042cmd(CONTR_READ);
	response = kbd_response();

	if (!(response & COMMAND_PCSCAN)) {
		if (kbd_cmd(KEYB_C_SCANSET) != 0)
			printf("pcvt: kbd SCANSET cmd timeout\n");
		else if (kbd_cmd(1) != 0)
			printf("pcvt: kbd SCANSET data timeout\n");
		else
			printf("pcvt: kbd set to XT mode\n");
	 }
#endif
	splx(opri);

#if PCVT_KEYBDID

query_kbd_id:
	opri = spltty();

	if (kbd_cmd(KEYB_C_ID) != 0) {
		printf("pcvt: timeout for kbd ID cmd\n");
		keyboard_type = KB_UNKNOWN;
	}
	else {
r_entry:
		if ((response = kbd_response()) == KEYB_R_MF2ID1) {
			switch ((response = kbd_response())) {
			case KEYB_R_RESEND:
				/*
				 *  Let's give other priority levels
				 *  a chance instead of blocking at
				 *  tty level.
				 */
				splx(opri);
				goto query_kbd_id;
					
			case KEYB_R_MF2ID2:
			case KEYB_R_MF2ID2HP:
			case KEYB_R_MF2ID2TP:
			case KEYB_R_MF2ID2TP2:
				keyboard_type = KB_MFII;
				break;

			default:
				printf("pcvt: kbdid (resp 2 = %d)\n", response);
				keyboard_type = KB_UNKNOWN;
				break;
			}
		}
		else if (response == KEYB_R_ACK)
			goto r_entry;
		else if (response == -1)
			keyboard_type = KB_AT;
		else
			printf("pcvt: kbdid (resp 1 = %d)\n", response);
	}
	splx(opri);

#else /* PCVT_KEYBDID */
	keyboard_type = KB_MFII;	/* force it .. */
#endif /* PCVT_KEYBDID */
}

/*---------------------------------------------------------------------------*
 *	init keyboard code
 *---------------------------------------------------------------------------*/
void
kbd_code_init(void)
{
	doreset();
	ovlinit(0);
	keyboard_is_initialized = 1;
#if !PCVT_NO_LED_UPDATE && PCVT_UPDLED_LOSES_INTR
	timeout_set(&kbd_led_intr_to, check_for_lost_intr, NULL);
#endif
}

/*---------------------------------------------------------------------------*
 *	init keyboard code, this initializes the keyboard subsystem
 *	just "a bit" so the very very first ddb session is able to
 *	get proper keystrokes - in other words, it's a hack ....
 *---------------------------------------------------------------------------*/
void
kbd_code_init1(void)
{
	doreset();
	keyboard_is_initialized = 1;
}

/*---------------------------------------------------------------------------*
 *	init keyboard overlay table
 *---------------------------------------------------------------------------*/
static
void ovlinit(int force)
{
	int i;

	if (force || ovlinitflag == 0) {
		if (ovlinitflag == 0)
		    ovltbl = (Ovl_tbl *)malloc(sizeof(Ovl_tbl) * OVLTBL_SIZE,
					       M_DEVBUF, M_WAITOK);

		for(i = 0; i < OVLTBL_SIZE; i++) {
			ovltbl[i].keynum =
			ovltbl[i].type = 0;
			ovltbl[i].unshift[0] =
			ovltbl[i].shift[0] =
			ovltbl[i].ctrl[0] =
			ovltbl[i].altgr[0] = 0;
			ovltbl[i].subu =
			ovltbl[i].subs =
			ovltbl[i].subc =
			ovltbl[i].suba = KBD_SUBT_STR;	/* just strings .. */
		}
		for(i = 0; i <= MAXKEYNUM; i++)
			key2ascii[i].type &= KBD_MASK;

		ovlinitflag = 1;
	}
}

/*---------------------------------------------------------------------------*
 *	get original key definition
 *---------------------------------------------------------------------------*/
static int
getokeydef(unsigned key, Ovl_tbl *thisdef)
{
	if (key == 0 || key > MAXKEYNUM)
		return (EINVAL);

	thisdef->keynum = key;
	thisdef->type = key2ascii[key].type;

	if (key2ascii[key].unshift.subtype == STR) {
		bcopy((u_char *)(key2ascii[key].unshift.what.string),
		    thisdef->unshift, CODE_SIZE);
		thisdef->subu = KBD_SUBT_STR;
	}
	else {
		bcopy("", thisdef->unshift, CODE_SIZE);
		thisdef->subu = KBD_SUBT_FNC;
	}

	if (key2ascii[key].shift.subtype == STR) {
		bcopy((u_char *)(key2ascii[key].shift.what.string),
		    thisdef->shift, CODE_SIZE);
		thisdef->subs = KBD_SUBT_STR;
	}
	else {
		bcopy("", thisdef->shift,CODE_SIZE);
		thisdef->subs = KBD_SUBT_FNC;
	}

	if (key2ascii[key].ctrl.subtype == STR) {
		bcopy((u_char *)(key2ascii[key].ctrl.what.string),
		    thisdef->ctrl, CODE_SIZE);
		thisdef->subc = KBD_SUBT_STR;
	}
	else {
		bcopy("",thisdef->ctrl,CODE_SIZE);
		thisdef->subc = KBD_SUBT_FNC;
	}

	/* deliver at least anything for ALTGR settings ... */
	if (key2ascii[key].unshift.subtype == STR) {
		bcopy((u_char *)(key2ascii[key].unshift.what.string),
		    thisdef->altgr, CODE_SIZE);
		thisdef->suba = KBD_SUBT_STR;
	}
	else {
		bcopy("", thisdef->altgr, CODE_SIZE);
		thisdef->suba = KBD_SUBT_FNC;
	}
	return (0);
}

/*---------------------------------------------------------------------------*
 *	get current key definition
 *---------------------------------------------------------------------------*/
static int
getckeydef(unsigned key, Ovl_tbl *thisdef)
{
	u_short type = key2ascii[key].type;

	if (key > MAXKEYNUM)
		return (EINVAL);

	if (type & KBD_OVERLOAD)
		*thisdef = ovltbl[key2ascii[key].ovlindex];
	else
		getokeydef(key,thisdef);

	return (0);
}

/*---------------------------------------------------------------------------*
 *	translate keynumber and returns ptr to associated ascii string
 *	if key is bound to a function, executes it, and ret empty ptr
 *---------------------------------------------------------------------------*/
static u_char *
xlatkey2ascii(U_short key)
{
	int		n;
	static u_char	capchar[2] = {0, 0};
#if PCVT_META_ESC
	static u_char	metachar[3] = {0x1b, 0, 0};
#else
	static u_char	metachar[2] = {0, 0};
#endif
	static Ovl_tbl	thisdef;
	static u_char altgr_shft_key[KBDMAXOVLKEYSIZE];
	
	void		(*fnc)(void);

	/* ignore the NON-KEY */
	if (key == 0)
		return (0);

	/* get the current ASCII value */
	getckeydef(key & 0x7F, &thisdef);

	thisdef.type &= KBD_MASK;

	if (key & 0x80) {		/* special handling of ALT-KEYPAD */
		/* is the ALT Key released? */
		if (thisdef.type == KBD_META || thisdef.type == KBD_ALTGR) {
			if (altkpflag) { /* have we been in altkp mode? */
				capchar[0] = altkpval;
				altkpflag = 0;
				altkpval = 0;
				return (capchar);
			}
		}
		return (0);
	}

	switch (thisdef.type) {		/* convert the keys */
	case KBD_BREAK:
	case KBD_ASCII:
	case KBD_FUNC:
		fnc = NULL;
		more_chars = NULL;

		if (altgr_down) {
			/* XXX this is hack to support simple AltGr + Shift
	 	 	 * remapping. This should work for KOI-8 keymap style.
			 */
			if (shift_down) {
                		altgr_shft_key[0] = *(u_char*)thisdef.altgr+040;
                		more_chars = (u_char*)altgr_shft_key;
                	}
                	else
				more_chars = (u_char *)thisdef.altgr;
		}
		else if (!ctrl_down && (shift_down || vsp->shift_lock)) {
			if (key2ascii[key].shift.subtype == STR)
				more_chars = (u_char *)thisdef.shift;
			else
				fnc = key2ascii[key].shift.what.func;
		     }
		else if (ctrl_down) {
			if (key2ascii[key].ctrl.subtype == STR)
				more_chars = (u_char *)thisdef.ctrl;
			else
				fnc = key2ascii[key].ctrl.what.func;
		     }
		else {
			if (key2ascii[key].unshift.subtype == STR)
				more_chars = (u_char *)thisdef.unshift;
			else
				fnc = key2ascii[key].unshift.what.func;
		}

		if (fnc)
			(*fnc)();	/* execute function */

		if ((more_chars != NULL) && (more_chars[1] == 0)) {
			if (vsp->caps_lock && more_chars[0] >= 'a'
			   && more_chars[0] <= 'z') {
				capchar[0] = *more_chars - ('a' - 'A');
				more_chars = capchar;
			}
			if (meta_down) {
#if PCVT_META_ESC
				metachar[1] = *more_chars;
#else
				metachar[0] = *more_chars | 0x80;
#endif
				more_chars = metachar;
			}
		}
		return (more_chars);

	case KBD_KP:
		fnc = NULL;
		more_chars = NULL;

		if (meta_down) {
			switch (key) {
			case 95:	/* / */
				altkpflag = 0;
				more_chars = (u_char *)"\033OQ";
				return (more_chars);

			case 100:	/* * */
				altkpflag = 0;
				more_chars = (u_char *)"\033OR";
				return (more_chars);

			case 105:	/* - */
				altkpflag = 0;
		 		more_chars = (u_char *)"\033OS";
				return (more_chars);
			}
		}

		if (meta_down || altgr_down) {
			if ((n = keypad2num[key-91]) >= 0) {
				if (!altkpflag) {
					/* start ALT-KP mode */
					altkpflag = 1;
					altkpval = 0;
				}
				altkpval *= 10;
				altkpval += n;
			}
			else
				altkpflag = 0;
			return (0);
		}

		if (!vsp->num_lock) {
			if (key2ascii[key].shift.subtype == STR)
				more_chars = (u_char *)thisdef.shift;
			else
				fnc = key2ascii[key].shift.what.func;
		}
		else {
			if (key2ascii[key].unshift.subtype == STR)
				more_chars = (u_char *)thisdef.unshift;
			else
				fnc = key2ascii[key].unshift.what.func;
		}

		if (fnc)
			(*fnc)();	/* execute function */

		return (more_chars);

	case KBD_CURSOR:
		fnc = NULL;
		more_chars = NULL;

		if (vsp->ckm) {
			if (key2ascii[key].shift.subtype == STR)
				more_chars = (u_char *)thisdef.shift;
			else
				fnc = key2ascii[key].shift.what.func;
		}
		else {
			if (key2ascii[key].unshift.subtype == STR)
				more_chars = (u_char *)thisdef.unshift;
			else
				fnc = key2ascii[key].unshift.what.func;
		}

		if (fnc)
			(*fnc)();	/* execute function */

		return (more_chars);

	case KBD_NUM:		/*  special kp-num handling */
		more_chars = NULL;

		if (meta_down)
			more_chars = (u_char *)"\033OP"; /* PF1 */
		else {
			vsp->num_lock ^= 1;
			update_led(KBD_NUM);
		}
		return (more_chars);

	case KBD_RETURN:
		more_chars = NULL;

		if (!vsp->num_lock)
			more_chars = (u_char *)thisdef.shift;
		else
			more_chars = (u_char *)thisdef.unshift;

		if (vsp->lnm && (*more_chars == '\r'))
			more_chars = (u_char *)"\r\n"; /* CR LF */

		if (meta_down) {
#if PCVT_META_ESC
			metachar[1] = *more_chars;
#else
			metachar[0] = *more_chars | 0x80;
#endif
			more_chars = metachar;
		}
		return (more_chars);

	case KBD_META:		/* these keys are	*/
	case KBD_ALTGR:		/*  handled directly	*/
	case KBD_SCROLL:	/*  by the keyboard	*/
	case KBD_CAPS:		/*  handler - they are	*/
	case KBD_SHFTLOCK:	/*  ignored here	*/
	case KBD_CTL:
	case KBD_NONE:
	default:
		return (0);
	}
}

/*---------------------------------------------------------------------------*
 *	get keystrokes from the keyboard.
 *	if noblock = 0, wait until a key is pressed.
 *	else return NULL if no characters present.
 *---------------------------------------------------------------------------*/

#if PCVT_KBD_FIFO
extern	u_char	pcvt_kbd_fifo[];
extern	int	pcvt_kbd_rptr;
extern	short	pcvt_kbd_count;
#endif

u_char *
sgetc(int noblock)
{
#if PCVT_KBD_FIFO
	int		s;
#endif
	u_char *cp, dt, key;
	u_short	type;
	static u_char kbd_lastkey = 0; /* last keystroke */
	static char	keybuf[2] = {0}; /* the second 0 is a delimiter! */

	static struct {
		u_char extended: 1;	/* extended prefix seen */
		u_char ext1: 1;		/* extended prefix 1 seen */
		u_char breakseen: 1;	/* break code seen */
		u_char vshift: 1;	/* virtual shift pending */
		u_char vcontrol: 1;	/* virtual control pending */
		u_char sysrq: 1;	/* sysrq pressed */
	} kbd_status = {0};

loop:

#ifdef XSERVER

#if PCVT_KBD_FIFO

	if (noblock == 31337) {
		vsp->scrolling = 1;
		goto scroll_reset;
	}

	/* see if there is data from the keyboard available either from */
	/* the keyboard fifo or from the 8042 keyboard controller	*/

	if ((( noblock) && (pcvt_kbd_count)) ||
	    ((!noblock) && (inb(CONTROLLER_CTRL) & STATUS_OUTPBF))) {
		if (!noblock) {		/* source = 8042 */
			PCVT_KBD_DELAY();	/* 7 us delay */
			dt = inb(CONTROLLER_DATA);	/* get from obuf */
		}
		else {			/* source = keyboard fifo */
			dt = pcvt_kbd_fifo[pcvt_kbd_rptr++];
			s = spltty();
			pcvt_kbd_count--;
			splx(s);
			if (pcvt_kbd_rptr >= PCVT_KBD_FIFO_SZ)
				pcvt_kbd_rptr = 0;
		}

#else /* !PCVT_KB_FIFO */

	/* see if there is data from the keyboard available from the 8042 */
	if (inb(CONTROLLER_CTRL) & STATUS_OUTPBF) {
		PCVT_KBD_DELAY();		/* 7 us delay */
		dt = inb(CONTROLLER_DATA);	/* yes, get data */
#endif /* !PCVT_KBD_FIFO */


		if ((dt == KEYB_R_ACK) || (dt == KEYB_R_RESEND))
			update_led(dt); /* handle ACK/NACK correctly in X */

		/*
		 * If x mode is active, only care for locking keys, then
		 * return the scan code instead of any key translation.
		 * Additionally, this prevents us from any attempts to
		 * execute pcvt internal functions caused by keys (such
		 * as screen flipping).
		 */
 		if (vsp->kbd_state == K_RAW) {
			keybuf[0] = dt;
			return ((u_char *)keybuf);
		}
	}

#else /* !XSERVER */

#  if PCVT_KBD_FIFO

	/* see if there is data from the keyboard available either from */
	/* the keyboard fifo or from the 8042 keyboard controller	*/

	if ((( noblock) && (pcvt_kbd_count)) ||
	    ((!noblock) && (inb(CONTROLLER_CTRL) & STATUS_OUTPBF))) {
		if (!noblock) {		/* source = 8042 */
			PCVT_KBD_DELAY();	/* 7 us delay */
			dt = inb(CONTROLLER_DATA);
		}
		else {			/* source = keyboard fifo */
			dt = pcvt_kbd_fifo[pcvt_kbd_rptr++]; /* yes, get it ! */
			s = spltty();
			pcvt_kbd_count--;
			splx(s);
			if (pcvt_kbd_rptr >= PCVT_KBD_FIFO_SZ)
				pcvt_kbd_rptr = 0;
		}
	}

#else /* !PCVT_KBD_FIFO */

	/* see if there is data from the keyboard available from the 8042 */
	if (inb(CONTROLLER_CTRL) & STATUS_OUTPBF) {
		PCVT_KBD_DELAY();		/* 7 us delay */
		dt = inb(CONTROLLER_DATA);	/* yes, get data ! */
	}

#endif /* !PCVT_KBD_FIFO */

#endif /* !XSERVER */

	else {
		if (noblock)
			return (NULL);
		else
			goto loop;
	}

	/* lets look what we got */
	switch (dt) {
	case KEYB_R_ACK:	/* acknowledge after command has rx'd*/
	case KEYB_R_RESEND:	/* keyboard wants us to resend cmnd */
		update_led(dt);	/* handle ACK/NACK correctly, no X */
		break;
	case KEYB_R_OVERRUN0:	/* keyboard buffer overflow */
#if PCVT_SCANSET == 2
	case KEYB_R_SELFOK:	/* keyboard selftest ok */
#endif /* PCVT_SCANSET == 2 */
	case KEYB_R_ECHO:	/* keyboard response to KEYB_C_ECHO */
	case KEYB_R_SELFBAD:	/* keyboard selftest FAILED */
	case KEYB_R_DIAGBAD:	/* keyboard self diagnostic failure */
	case KEYB_R_OVERRUN1:	/* keyboard buffer overflow */
		break;

	case KEYB_R_EXT1:	/* keyboard extended scancode pfx 2 */
		kbd_status.ext1 = 1;
		/* FALLTHROUGH */

	case KEYB_R_EXT0:	/* keyboard extended scancode pfx 1 */
		kbd_status.extended = 1;
		break;

#if PCVT_SCANSET == 2
	case KEYB_R_BREAKPFX:	/* break code prefix for set 2 and 3 */
		kbd_status.breakseen = 1;
		break;
#endif /* PCVT_SCANSET == 2 */

	default:
		goto regular;	/* regular key */
	}

	if (noblock)
		return (NULL);
	else
		goto loop;

	/* got a normal scan key */
regular:
#if PCVT_SCANSET == 1
	kbd_status.breakseen = dt & 0x80 ? 1 : 0;
	dt &= 0x7f;
#endif	/* PCVT_SCANSET == 1 */

	/*   make a keycode from scan code	*/
	if (dt >= sizeof scantokey / sizeof(u_char))
		key = 0;
	else
		key = kbd_status.extended ? extscantokey[dt] : scantokey[dt];

	if (kbd_status.ext1 && key == 64)
		/* virtual control key */
		key = 129;

	kbd_status.extended = kbd_status.ext1 = 0;

	if ((key == 85) && shift_down &&
	    (kbd_lastkey != 85 || !kbd_status.breakseen)) {
		/* removing of visual regions for mouse console support */
		if (IS_SEL_EXISTS(vsp)) 
			remove_selection(); /* remove current selection before 
					       leaving this screen */
		if (IS_MOUSE_VISIBLE(vsp)) {
			mouse_hide();
			vsp->mouse_flags &= ~MOUSE_VISIBLE;
		}
		/* end of visual regions part */
		if (vsp->scr_offset && vsp->scr_offset >= vsp->row) {
			if (!vsp->scrolling) {
				vsp->scrolling += vsp->row - 1;
				if (vsp->Scrollback) {
					scrollback_save_screen();
					if (vsp->scr_offset == vsp->max_off) {
						bcopy(vsp->Scrollback +
						      vsp->maxcol,
						      vsp->Scrollback,
						      vsp->maxcol *
						      vsp->max_off * CHR);
						vsp->scr_offset--;
					}
					bcopy(vsp->Crtat + vsp->cur_offset -
					      vsp->col, vsp->Scrollback +
				      	      (vsp->scr_offset + 1) *
					      vsp->maxcol, vsp->maxcol * CHR);
				}

				if (vsp->cursor_on)
					sw_cursor(0);
			}

			vsp->scrolling += vsp->screen_rows - 1;
			if (vsp->scrolling > vsp->scr_offset)
				vsp->scrolling = vsp->scr_offset;

			bcopy(vsp->Scrollback + ((vsp->scr_offset -
			      vsp->scrolling) * vsp->maxcol), vsp->Crtat,
			      vsp->screen_rows * vsp->maxcol * CHR);
		}

		kbd_lastkey = 85;
		goto loop;
	}
	else if ((key == 86) && shift_down &&
		(kbd_lastkey != 86 || !kbd_status.breakseen)) {
scroll_reset:
		/* removing of visual regions for mouse console support */
		if (IS_SEL_EXISTS(vsp)) 
			remove_selection(); /* remove current selection before 
					       leaving this screen */
		if (IS_MOUSE_VISIBLE(vsp)) {
			mouse_hide();
			vsp->mouse_flags &= ~MOUSE_VISIBLE;
		}
		/* end of visual regions part */
		if (vsp->scrolling > 0) {
			vsp->scrolling -= vsp->screen_rows - 1;
			if (vsp->scrolling < 0)
				vsp->scrolling = 0;

			if (vsp->scrolling <= vsp->row) {
				vsp->scrolling = 0;
				scrollback_restore_screen();
			}
			else {
				if (vsp->scrolling + 2 < vsp->screen_rows)
					fillw(user_attr | ' ',
					      (caddr_t)vsp->Crtat,
					      vsp->screen_rows * vsp->maxcol);

				bcopy(vsp->Scrollback + ((vsp->scr_offset -
			      	      vsp->scrolling) * vsp->maxcol),
			              vsp->Crtat, (vsp->scrolling + 2 <
				      vsp->screen_rows ? vsp->scrolling + 2 :
				      vsp->screen_rows) * vsp->maxcol * CHR);
			}
		}

		if (vsp->scrolling == 0) {
			if (vsp->cursor_on)
				sw_cursor(1);
		}

		if (noblock == 31337)
			return (NULL);

		if (key != 86)
			goto regular;
		else {
			kbd_lastkey = 86;
			goto loop;
		}
	}
	else if (vsp->scrolling && key != 128 && key != 44 && key != 57 &&
		 key != 85 && key != 86) {
			vsp->scrolling = 1;
			goto scroll_reset;
	     }

	if (kbd_reset && (key == 76) && ctrl_down && (meta_down||altgr_down)) {
		printf("\nconsole halt requested: going down.\n");
		kbd_reset = 0;
		psignal(initproc, SIGUSR1);
	}

#if NDDB > 0 || defined(DDB)		 /*   Check for cntl-alt-esc	*/
  	if ((key == 110) && ctrl_down && (meta_down || altgr_down)) {
 		static u_char in_Debugger;

 		if (!in_Debugger) {
 			in_Debugger = 1;
			if (db_console)
	 			Debugger();
 			in_Debugger = 0;

 			if (noblock)
 				return (NULL);
 			else
 				goto loop;
 		}
 	}
#endif /* NDDB > 0 || defined(DDB) */

	/* look for keys with special handling */
	if (key == 128) {
		/*
		 * virtual shift; sent around PrtScr, and around the arrow
		 * keys if the NumLck LED is on
		 */
		kbd_status.vshift = !kbd_status.breakseen;
		key = 0;	/* no key */
	}
	else if (key == 129) {
		/*
		 * virtual control - the most ugly thingie at all
		 * the Pause key sends:
		 * <virtual control make> <numlock make> <virtual control
		 * break> <numlock break>
		 */
		if (!kbd_status.breakseen)
			kbd_status.vcontrol = 1;
		/* else: let the numlock hook clear this */
		key = 0;	/* no key */
	}
	else if (key == 90) {
		/* NumLock, look whether this is rather a Pause */
		if (kbd_status.vcontrol)
			key = 126;
		/*
		 * if this is the final break code of a Pause key,
		 * clear the virtual control status, too
		 */
		if (kbd_status.vcontrol && kbd_status.breakseen)
			kbd_status.vcontrol = 0;
	}
	else if (key == 127) {
		/*
		 * a SysRq; some keyboards are brain-dead enough to
		 * repeat the SysRq key make code by sending PrtScr
		 * make codes; other keyboards do not repeat SysRq
		 * at all. We keep track of the SysRq state here.
		 */
		kbd_status.sysrq = !kbd_status.breakseen;
	}
	else if (key == 124) {
		/*
		 * PrtScr; look whether this is really PrtScr or rather
		 * a silly repeat of a SysRq key
		 */
		if (kbd_status.sysrq)
			/* ignore the garbage */
			key = 0;
	}

	/* in NOREPEAT MODE ignore the key if it was the same as before */

	if (!kbrepflag && key == kbd_lastkey && !kbd_status.breakseen) {
		if (noblock)
			return (NULL);
		else
			goto loop;
	}

	type = key2ascii[key].type;

	if (type & KBD_OVERLOAD)
		type = ovltbl[key2ascii[key].ovlindex].type;

	type &= KBD_MASK;

	keybuf[0] = 0;
	switch (type) {
	case KBD_SHFTLOCK:
		if (!kbd_status.breakseen && key != kbd_lastkey)
			vsp->shift_lock ^= 1;
		break;

	case KBD_CAPS:
		if (!kbd_status.breakseen && key != kbd_lastkey) {
			vsp->caps_lock ^= 1;
			update_led(KBD_CAPS);
		}
		break;

	case KBD_SCROLL:
		if (!kbd_status.breakseen && key != kbd_lastkey) {
			vsp->scroll_lock ^= 1;
			update_led(KBD_SCROLL);

			if (!(vsp->scroll_lock))
				keybuf[0] = CSTART;
			else
				keybuf[0] = CSTOP;
		}
		break;

	case KBD_SHIFT:
		shift_down = kbd_status.breakseen ? 0 : 1;
		break;

	case KBD_META:
		meta_down = kbd_status.breakseen ? 0 : 0x80;
		break;

	case KBD_ALTGR:
		altgr_down = kbd_status.breakseen ? 0 : 1;
		break;

	case KBD_CTL:
		ctrl_down = kbd_status.breakseen ? 0 : 1;
		break;

	case KBD_NONE:
	default:
		break;			/* deliver a key */
	}

	if (kbd_status.breakseen) {
		key |= 0x80;
		kbd_status.breakseen = 0;
		kbd_lastkey = 0; /* -hv- I know this is a bug with */
	}			 /* N-Key-Rollover, but I ignore that */
	else			 /* because avoidance is too complicated */
		kbd_lastkey = key;

	cp = xlatkey2ascii(key);	/* have a key */

	if (cp)                         /* link ^S/^Q to scrlck led */
		if (((*cp == CSTOP) && (!vsp->scroll_lock)) ||
		    ((*cp == CSTART) && (vsp->scroll_lock))) {
			vsp->scroll_lock ^= 1;
			update_led(KBD_SCROLL);
		}

	if (keybuf[0])                  /* XON/XOFF scrlck emul. */
		cp = (u_char *)keybuf;

	if (cp == NULL && !noblock)
		goto loop;

	return (cp);
}

/*---------------------------------------------------------------------------*
 *	reflect status of locking keys & set led's
 *---------------------------------------------------------------------------*/
static void
setlockkeys(int snc)
{
	vsp->scroll_lock = snc & 1;
	vsp->num_lock	 = (snc & 2) ? 1 : 0;
	vsp->caps_lock	 = (snc & 4) ? 1 : 0;
	update_led(1);
}

/*---------------------------------------------------------------------------*
 *	remove a key definition
 *---------------------------------------------------------------------------*/
static int
rmkeydef(int key)
{
	register Ovl_tbl *ref;

	if (key == 0 || key > MAXKEYNUM)
		return (EINVAL);

	if (key2ascii[key].type & KBD_OVERLOAD) {
		ref = &ovltbl[key2ascii[key].ovlindex];
		ref->keynum = 0;
		ref->type = 0;
		ref->unshift[0] =
		ref->shift[0] =
		ref->ctrl[0] =
		ref->altgr[0] = 0;
		key2ascii[key].type &= KBD_MASK;
	}
	return (0);
}

/*---------------------------------------------------------------------------*
 *	overlay a key
 *---------------------------------------------------------------------------*/
static int
setkeydef(Ovl_tbl *data)
{
	int i;

	if ( data->keynum > MAXKEYNUM		 ||
	    (data->type & KBD_MASK) == KBD_BREAK ||
	    (data->type & KBD_MASK) > KBD_SHFTLOCK)
		return (EINVAL);

	data->unshift[KBDMAXOVLKEYSIZE] =
	data->shift[KBDMAXOVLKEYSIZE] =
	data->ctrl[KBDMAXOVLKEYSIZE] =
	data->altgr[KBDMAXOVLKEYSIZE] = 0;

	data->subu =
	data->subs =
	data->subc =
	data->suba = KBD_SUBT_STR;		/* just strings .. */

	data->type |= KBD_OVERLOAD;		/* mark overloaded */

	/* if key already overloaded, use that slot else find free slot */

	if (key2ascii[data->keynum].type & KBD_OVERLOAD)
		i = key2ascii[data->keynum].ovlindex;
	else {
		for (i = 0; i < OVLTBL_SIZE; i++)
			if (ovltbl[i].keynum==0)
				break;

		if (i == OVLTBL_SIZE)
			return (ENOSPC); /* no space, abuse of ENOSPC(!) */
	}

	ovltbl[i] = *data;		/* copy new data string */

	key2ascii[data->keynum].type |= KBD_OVERLOAD; 	/* mark key */
	key2ascii[data->keynum].ovlindex = i;

	return (0);
}

/*---------------------------------------------------------------------------*
 *	keyboard ioctl's entry
 *---------------------------------------------------------------------------*/
int
kbdioctl(Dev_t dev, u_long cmd, caddr_t data, int flag)
{
	int key;

	switch (cmd) {
	case KBDRESET:
		doreset();
		ovlinit(1);
		settpmrate(KBD_TPD500 | KBD_TPM100);
		setlockkeys(0);
		break;

	case KBDGTPMAT:
		*(int *)data = tpmrate;
		break;

	case KBDSTPMAT:
		settpmrate(*(int *)data);
		break;

	case KBDGREPSW:
		*(int *)data = kbrepflag;
		break;

	case KBDSREPSW:
		kbrepflag = (*(int *)data) & 1;
		break;

	case KBDGLOCK:
		*(int *)data = ( (vsp->scroll_lock) | (vsp->num_lock * 2) |
				 (vsp->caps_lock * 4));
		break;

	case KBDSLOCK:
		setlockkeys(*(int *)data);
		break;

	case KBDGCKEY:
		key = ((Ovl_tbl *)data)->keynum;
		return (getckeydef(key,(Ovl_tbl *)data));

	case KBDSCKEY:
		key = ((Ovl_tbl *)data)->keynum;
		return (setkeydef((Ovl_tbl *)data));

	case KBDGOKEY:
		key = ((Ovl_tbl *)data)->keynum;
		return (getokeydef(key,(Ovl_tbl *)data));

	case KBDRMKEY:
		key = *(int *)data;
		return (rmkeydef(key));

	case KBDDEFAULT:
		ovlinit(1);
		break;

	default:
		/* proceed with vga ioctls */
		return (-1);
	}
	return (0);
}

/*---------------------------------------------------------------------------*
 *	convert ISO-8859 style keycode into IBM 437
 *---------------------------------------------------------------------------*/
static inline u_char
iso2ibm(u_char c)
{
	if (c < 0x80)
		return (c);
	return (iso2ibm437[c - 0x80]);
}

/*---------------------------------------------------------------------------*
 *	build up a USL style keyboard map
 *---------------------------------------------------------------------------*/
void
get_usl_keymap(keymap_t *map)
{
	int i;

	bzero((caddr_t)map, sizeof(keymap_t));

	map->n_keys = 0x59;	/* that many keys we know about */

	for(i = 1; i < N_KEYNUMS; i++) {
		Ovl_tbl kdef;
		u_char c;
		int j;
		int idx = key2scan1[i];

		if (idx == 0 || idx >= map->n_keys)
			continue;

		getckeydef(i, &kdef);
		kdef.type &= KBD_MASK;

		switch (kdef.type) {
		case KBD_ASCII:
		case KBD_RETURN:
			map->key[idx].map[0] = iso2ibm(kdef.unshift[0]);
			map->key[idx].map[1] = iso2ibm(kdef.shift[0]);
			map->key[idx].map[2] = map->key[idx].map[3] =
				iso2ibm(kdef.ctrl[0]);
			map->key[idx].map[4] = map->key[idx].map[5] =
				iso2ibm(c = kdef.altgr[0]);
			/*
			 * XXX this is a hack
			 * since we currently do not map strings to AltGr +
			 * shift, we attempt to use the unshifted AltGr
			 * definition here and try to toggle the case
			 * this should at least work for ISO8859 letters,
			 * but also for (e.g.) russian KOI-8 style
			 */
			if ((c & 0x7f) >= 0x40)
				map->key[idx].map[5] = iso2ibm(c ^ 0x20);
			break;

		case KBD_FUNC:
			/* we are only interested in F1 thru F12 here */
			if (i >= 112 && i <= 123) {
				map->key[idx].map[0] = i - 112 + 27;
				map->key[idx].spcl = 0x80;
			}
			break;

		case KBD_SHIFT:
			c = i == 44 ? 2 /* lSh */ : 3 /* rSh */;
			goto special;

		case KBD_CAPS:
			c = 4;
			goto special;

		case KBD_NUM:
			c = 5;
			goto special;

		case KBD_SCROLL:
			c = 6;
			goto special;

		case KBD_META:
			c = 7;
			goto special;

		case KBD_CTL:
			c = 9;
special:
			for(j = 0; j < NUM_STATES; j++)
				map->key[idx].map[j] = c;
			map->key[idx].spcl = 0xff;
			break;

		default:
			break;
		}
	}
}

/*---------------------------------------------------------------------------*
 *	switch keypad to numeric mode
 *---------------------------------------------------------------------------*/
void
vt_keynum(struct video_state *svsp)
{
	svsp->num_lock = 1;
	update_led(1);
}

/*---------------------------------------------------------------------------*
 *	switch keypad to application mode
 *---------------------------------------------------------------------------*/
void
vt_keyappl(struct video_state *svsp)
{
	svsp->num_lock = 0;
	update_led(1);
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 1
 *---------------------------------------------------------------------------*/
static void
fkey1(void)
{
	if (meta_down)
		more_chars = (u_char *)"\033[23~"; /* F11 */
	else
		more_chars = (u_char *)"\033OP"; /* F1 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 2
 *---------------------------------------------------------------------------*/
static void
fkey2(void)
{
	if (meta_down)
		more_chars = (u_char *)"\033[24~"; /* F12 */
	else
		more_chars = (u_char *)"\033OQ"; /* F2 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 3
 *---------------------------------------------------------------------------*/
static void
fkey3(void)
{
	if (meta_down)
		more_chars = (u_char *)"\033[25~"; /* F13 */
	else
		more_chars = (u_char *)"\033OR"; /* F3 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 4
 *---------------------------------------------------------------------------*/
static void
fkey4(void)
{
	if (meta_down)
		more_chars = (u_char *)"\033[26~"; /* F14 */
	else
		more_chars = (u_char *)"\033OS"; /* F4 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 5
 *---------------------------------------------------------------------------*/
static void
fkey5(void)
{
	if (meta_down)
		more_chars = (u_char *)"\033[28~"; /* Help */
	else
		more_chars = (u_char *)"\033[17~"; /* F5 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 6
 *---------------------------------------------------------------------------*/
static void
fkey6(void)
{
	if (meta_down)
		more_chars = (u_char *)"\033[29~"; /* DO */
	else
		more_chars = (u_char *)"\033[18~"; /* F6 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 7
 *---------------------------------------------------------------------------*/
static void
fkey7(void)
{
	if (meta_down)
		more_chars = (u_char *)"\033[31~"; /* F17 */
	else
		more_chars = (u_char *)"\033[19~"; /* F7 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 8
 *---------------------------------------------------------------------------*/
static void
fkey8(void)
{
	if (meta_down)
		more_chars = (u_char *)"\033[32~"; /* F18 */
	else
		more_chars = (u_char *)"\033[20~"; /* F8 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 9
 *---------------------------------------------------------------------------*/
static void
fkey9(void)
{
	if (meta_down)
		more_chars = (u_char *)"\033[33~"; /* F19 */
	else
		more_chars = (u_char *)"\033[21~"; /* F9 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 10
 *---------------------------------------------------------------------------*/
static void
fkey10(void)
{
	if (meta_down)
		more_chars = (u_char *)"\033[34~"; /* F20 */
	else
		more_chars = (u_char *)"\033[29~"; /* F10 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 11
 *---------------------------------------------------------------------------*/
static void
fkey11(void)
{
	if (meta_down)
		more_chars = (u_char *)"\0x8FP"; /* PF1 */
	else
		more_chars = (u_char *)"\033[23~"; /* F11 */
}

/*---------------------------------------------------------------------------*
 *	function bound to function key 12
 *---------------------------------------------------------------------------*/
static void
fkey12(void)
{
	if (meta_down)
		more_chars = (u_char *)"\0x8FQ"; /* PF2 */
	else
		more_chars = (u_char *)"\033[24~"; /* F12 */
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 1
 *---------------------------------------------------------------------------*/
static void
sfkey1(void)
{
	if (meta_down) {
		if (vsp->ukt.length[6])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[6]]);
		else
			more_chars = (u_char *)"\033[23~"; /* F11 */
	}
	else
		do_vgapage(4);
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 2
 *---------------------------------------------------------------------------*/
static void
sfkey2(void)
{
	if (meta_down) {
		if (vsp->ukt.length[7])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[7]]);
		else
			more_chars = (u_char *)"\033[24~"; /* F12 */
	}
	else
		do_vgapage(5);
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 3
 *---------------------------------------------------------------------------*/
static void
sfkey3(void)
{
	if (meta_down) {
		if (vsp->ukt.length[8])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[8]]);
		else
			more_chars = (u_char *)"\033[25~"; /* F13 */
	}
	else
		do_vgapage(6);
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 4
 *---------------------------------------------------------------------------*/
static void
sfkey4(void)
{
	if (meta_down) {
		if (vsp->ukt.length[9])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[9]]);
		else
			more_chars = (u_char *)"\033[26~"; /* F14 */
	}
	else
		do_vgapage(7);
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 5
 *---------------------------------------------------------------------------*/
static void
sfkey5(void)
{
	if (meta_down) {
		if (vsp->ukt.length[11])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[11]]);
		else
			more_chars = (u_char *)"\033[28~"; /* Help */
	}
	else {
		if (current_video_screen <= 0)
			do_vgapage(totalscreens-1);
		else
			do_vgapage(current_video_screen - 1);
	}
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 6
 *---------------------------------------------------------------------------*/
static void
sfkey6(void)
{
	if (!meta_down) {
		if (vsp->ukt.length[0])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[0]]);
		else
			more_chars = (u_char *)"\033[17~"; /* F6 */
	}
	else if (vsp->ukt.length[12])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[12]]);
	     else
			more_chars = (u_char *)"\033[29~"; /* DO */
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 7
 *---------------------------------------------------------------------------*/
static void
sfkey7(void)
{
	if (!meta_down) {
		if (vsp->ukt.length[1])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[1]]);
		else
			more_chars = (u_char *)"\033[18~"; /* F7 */
	}
	else if (vsp->ukt.length[14])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[14]]);
	     else
			more_chars = (u_char *)"\033[31~"; /* F17 */
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 8
 *---------------------------------------------------------------------------*/
static void
sfkey8(void)
{
	if (!meta_down) {
		if (vsp->ukt.length[2])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[2]]);
		else
			more_chars = (u_char *)"\033[19~"; /* F8 */
	}
	else if (vsp->ukt.length[14])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[15]]);
	     else
			more_chars = (u_char *)"\033[32~"; /* F18 */
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 9
 *---------------------------------------------------------------------------*/
static void
sfkey9(void)
{
	if (!meta_down) {
		if (vsp->ukt.length[3])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[3]]);
		else
			more_chars = (u_char *)"\033[20~"; /* F9 */
	}
	else if (vsp->ukt.length[16])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[16]]);
	     else
			more_chars = (u_char *)"\033[33~"; /* F19 */
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 10
 *---------------------------------------------------------------------------*/
static void
sfkey10(void)
{
	if (!meta_down) {
		if (vsp->ukt.length[4])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[4]]);
		else
			more_chars = (u_char *)"\033[21~"; /* F10 */
	}
	else if (vsp->ukt.length[17])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[17]]);
	     else
			more_chars = (u_char *)"\033[34~"; /* F20 */
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 11
 *---------------------------------------------------------------------------*/
static void
sfkey11(void)
{
	if (!meta_down) {
		if (vsp->ukt.length[6])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[6]]);
		else
			more_chars = (u_char *)"\033[23~"; /* F11 */
	}
}

/*---------------------------------------------------------------------------*
 *	function bound to SHIFTED function key 12
 *---------------------------------------------------------------------------*/
static void
sfkey12(void)
{
	if (!meta_down) {
		if (vsp->ukt.length[7])	/* entry available ? */
			more_chars = (u_char *)
				&(vsp->udkbuf[vsp->ukt.first[7]]);
		else
			more_chars = (u_char *)"\033[24~"; /* F12 */
	}
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 1
 *---------------------------------------------------------------------------*/
static void
cfkey1(void)
{
	if (meta_down)
		do_vgapage(0);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 2
 *---------------------------------------------------------------------------*/
static void
cfkey2(void)
{
	if (meta_down)
		do_vgapage(1);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 3
 *---------------------------------------------------------------------------*/
static void
cfkey3(void)
{
	if (meta_down)
		do_vgapage(2);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 4
 *---------------------------------------------------------------------------*/
static void
cfkey4(void)
{
	if (meta_down)
		do_vgapage(3);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 5
 *---------------------------------------------------------------------------*/
static void
cfkey5(void)
{
	if (meta_down)
		do_vgapage(4);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 6
 *---------------------------------------------------------------------------*/
static void
cfkey6(void)
{
	if (meta_down)
		do_vgapage(5);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 7
 *---------------------------------------------------------------------------*/
static void
cfkey7(void)
{
	if (meta_down)
		do_vgapage(6);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 8
 *---------------------------------------------------------------------------*/
static void
cfkey8(void)
{
	if (meta_down)
		do_vgapage(7);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 9
 *---------------------------------------------------------------------------*/
static void
cfkey9(void)
{
	if (meta_down)
		do_vgapage(8);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 10
 *---------------------------------------------------------------------------*/
static void
cfkey10(void)
{
	if (meta_down)
		do_vgapage(9);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 11
 *---------------------------------------------------------------------------*/
static void
cfkey11(void)
{
	if (meta_down)
		do_vgapage(10);
}

/*---------------------------------------------------------------------------*
 *	function bound to control function key 12
 *---------------------------------------------------------------------------*/
static void
cfkey12(void)
{
	if (meta_down)
		do_vgapage(11);
}

/* #endif */	/* NVT > 0 */

static void
scrollback_save_screen(void)
{
	int x = spltty();
	register size_t s;

	s = sizeof(u_short) * vsp->screen_rowsize * vsp->maxcol;

	if (scrollback_savedscreen)
		free(scrollback_savedscreen, M_TEMP);

	scrnsv_size = s;

	if (!(scrollback_savedscreen = (u_short *)malloc(s, M_TEMP, M_NOWAIT))){
		splx(x);
		return;
	}
	bcopy(vsp->Crtat, scrollback_savedscreen, scrnsv_size);
	splx(x);
}

static void
scrollback_restore_screen(void)
{
	if (scrollback_savedscreen)
		bcopy(scrollback_savedscreen, vsp->Crtat, scrnsv_size);
}

/*
 * Function to handle the wheel
 * z == 1 means to scroll to the lower page
 * z == -1 means to scroll to the upper page
 */

void
scrollback_mouse(int z)
{

	/* removing of visual regions for mouse console support */
	
	if (IS_SEL_EXISTS(vsp)) {
		remove_selection(); /* remove current selection before 
				       leaving this screen */
		vsp->mouse_flags &= ~SEL_EXISTS;
	}
	if (IS_MOUSE_VISIBLE(vsp)) {
		mouse_hide();
		vsp->mouse_flags &= ~MOUSE_VISIBLE;
	}
		
	if (z <= -1)
		
	{
		if (vsp->scr_offset && vsp->scr_offset >= vsp->row) {
			if (!vsp->scrolling) {
				vsp->scrolling += vsp->row - 1;
				if (vsp->Scrollback) {
					scrollback_save_screen();
					if (vsp->scr_offset == vsp->max_off) {
						bcopy(vsp->Scrollback +
						      vsp->maxcol,
						      vsp->Scrollback,
						      vsp->maxcol *
						      vsp->max_off * CHR);
						vsp->scr_offset--;
					}
					bcopy(vsp->Crtat + vsp->cur_offset -
					      vsp->col, vsp->Scrollback +
				      	      (vsp->scr_offset + 1) *
					      vsp->maxcol, vsp->maxcol * CHR);
				}

				if (vsp->cursor_on)
					sw_cursor(0);
			}

			vsp->scrolling += vsp->screen_rows - 1;
			if (vsp->scrolling > vsp->scr_offset) {
				vsp->scrolling = vsp->scr_offset;
			}
				
			bcopy(vsp->Scrollback + ((vsp->scr_offset -
			      vsp->scrolling) * vsp->maxcol), vsp->Crtat,
			      vsp->screen_rows * vsp->maxcol * CHR);
		}
	}
	else /* positive z */	
	{
		if (IS_SEL_EXISTS(vsp)) {
			remove_selection();
			vsp->mouse_flags &= ~SEL_EXISTS;
		}
		if (vsp->scrolling > 0) {
			vsp->scrolling -= vsp->screen_rows - 1;
			if (vsp->scrolling < 0)
				vsp->scrolling = 0;

			if (vsp->scrolling <= vsp->row) {
				vsp->scrolling = 0;
				scrollback_restore_screen();
			}
			else {
				if (vsp->scrolling + 2 < vsp->screen_rows)
					fillw(user_attr | ' ',
					      (caddr_t)vsp->Crtat,
					      vsp->screen_rows * vsp->maxcol);

				bcopy(vsp->Scrollback + ((vsp->scr_offset -
			      	      vsp->scrolling) * vsp->maxcol),
			              vsp->Crtat, (vsp->scrolling + 2 <
				      vsp->screen_rows ? vsp->scrolling + 2 :
				      vsp->screen_rows) * vsp->maxcol * CHR);
			}
		}
		if (vsp->scrolling == 0) {
			if (vsp->cursor_on)
				sw_cursor(1);
		}
	}
}
/* ------------------------------- EOF -------------------------------------*/

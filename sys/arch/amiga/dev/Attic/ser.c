/*	$OpenBSD: ser.c,v 1.14 2002/07/06 19:14:20 nordin Exp $	*/
/*	$NetBSD: ser.c,v 1.43 1998/01/12 10:40:11 thorpej Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1990 The Regents of the University of California.
 * All rights reserved.
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
 *	@(#)ser.c	7.12 (Berkeley) 6/27/91
 */
/*
 * XXX This file needs major cleanup it will never service more than one
 * XXX unit.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/ioctl.h>
#include <sys/device.h>
#include <sys/tty.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/malloc.h>
#include <sys/uio.h>
#include <sys/kernel.h>
#include <sys/syslog.h>
#include <sys/queue.h>

#include <machine/cpu.h>

#include <amiga/amiga/device.h>
#include <amiga/dev/serreg.h>
#include <amiga/amiga/custom.h>
#include <amiga/amiga/cia.h>
#include <amiga/amiga/cc.h>

#include <dev/cons.h>

#ifdef DDB
#include <ddb/db_var.h>
#endif

#include <machine/conf.h>

#include "ser.h"
#if NSER > 0

/* unit is in lower 7 bits (for now, only one unit:-))
   dialin:    open blocks until carrier present, hangup on carrier drop
   dialout:   carrier is ignored */

#define SERUNIT(dev)   (minor(dev) & 0x7f)
#define SERCUA(dev)    (minor(dev) & 0x80)

void serattach(struct device *, struct device *, void *);
int sermatch(struct device *, void *, void *);

struct ser_softc {
	struct device dev;
	struct tty *ser_tty;
	u_char ser_cua;
	int ser_swflags;
};

struct cfattach ser_ca = {
	sizeof(struct ser_softc), sermatch, serattach
};

struct cfdriver ser_cd = {
	NULL, "ser", DV_TTY, NULL, 0
};

#ifndef SEROBUF_SIZE
#define SEROBUF_SIZE 32
#endif
#ifndef SERIBUF_SIZE
#define SERIBUF_SIZE 512
#endif

#define splser() spl5()

void	serstart(struct tty *);
int	serparam(struct tty *, struct termios *); 
void	serintr(int);
int	serhwiflow(struct tty *, int);
int	sermctl(dev_t dev, int, int);
void	ser_fastint(void);
void	sereint(int, int);
static	void ser_putchar(struct tty *, u_short);
void	ser_outintr(void);
void	sercnprobe(struct consdev *);
void	sercninit(struct consdev *);
void	serinit(int, int);          
int	sercngetc(dev_t dev);
void	sercnputc(dev_t, int);
void	sercnpollc(dev_t, int);

int	ser_active;
int	ser_hasfifo;
int	nser = NSER;
#ifdef SERCONSOLE
int	serconsole = SERCONSOLE;
#else
int	serconsole = -1;
#endif
int	serconsinit;
int	serdefaultrate = TTYDEF_SPEED;
int	sermajor;

struct	vbl_node ser_vbl_node[NSER];
struct	tty ser_cons;

/* 
 * Since this UART is not particularly bright (to put it nicely), we'll
 * have to do parity stuff on our own.	This table contains the 8th bit
 * in 7bit character mode, for even parity.  If you want odd parity,
 * flip the bit.  (for generation of the table, see genpar.c)
 */

u_char	even_parity[] = {
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
};

/* 
 * Since we don't get interrupts for changes on the modem control line,
 * we'll have to fake them by comparing current settings to the settings
 * we remembered on last invocation.
 */

u_char	last_ciab_pra;

extern struct tty *constty;

extern int ser_open_speed;	/* current speed of open serial device */

#ifdef KGDB
#include <machine/remote-sl.h>

extern dev_t kgdb_dev;
extern int kgdb_rate;
extern int kgdb_debug_init;
#endif

#ifdef DEBUG
long	fifoin[17];
long	fifoout[17];
long	serintrcount[16];
long	sermintcount[16];
#endif

void	sermint(register int unit);

int
sermatch(pdp, match, auxp)
	struct device *pdp;
	void *match, *auxp;
{
	struct cfdata *cfp = match;

	if (matchname("ser", (char *)auxp) == 0 || cfp->cf_unit != 0)
		return(0);
	if (serconsole != 0 && amiga_realconfig == 0)
		return(0);
	return(1);
}


void
serattach(pdp, dp, auxp)
	struct device *pdp, *dp;
	void *auxp;
{
	u_short ir;

	ir = custom.intenar;
	if (serconsole == 0)
		DELAY(100000);

	ser_active |= 1;
	ser_vbl_node[0].function = (void (*) (void *)) sermint;
	add_vbl_function(&ser_vbl_node[0], SER_VBL_PRIORITY, (void *) 0);
#ifdef KGDB
	if (kgdb_dev == makedev(sermajor, 0)) {
		if (serconsole == 0)
			kgdb_dev = NODEV; /* can't debug over console port */
		else {
			(void)serinit(0, kgdb_rate);
			serconsinit = 1;       /* don't re-init in serputc */
			if (kgdb_debug_init == 0)
				printf(" kgdb enabled\n");
			else {
				/*
				 * Print prefix of device name,
				 * let kgdb_connect print the rest.
				 */
				printf("ser0: ");
				kgdb_connect(1);
			}
		}
	}
#endif
	/*
	 * Need to reset baud rate, etc. of next print so reset serconsinit.
	 */
	if (0 == serconsole)
		serconsinit = 0;
	if (dp)
		printf(": input fifo %d output fifo %d\n", SERIBUF_SIZE,
		    SEROBUF_SIZE);
}


/* ARGSUSED */
int
seropen(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
	int unit = SERUNIT(dev);
	struct ser_softc *sc;
	struct tty *tp;
	int s;
	int error = 0;

	if (unit >= NSER || (ser_active & (1 << unit)) == 0)
		return (ENXIO);
	sc = (struct ser_softc *)ser_cd.cd_devs[unit];

	s = spltty();
	if (!sc->ser_tty) {
		tp = sc->ser_tty = ttymalloc();
		tty_attach(tp);
	} else
		tp = sc->ser_tty;
	splx(s);

	tp->t_oproc = serstart;
	tp->t_param = serparam;
	tp->t_hwiflow = serhwiflow;

	if (!ISSET(tp->t_state, TS_ISOPEN)) {
		SET(tp->t_state, TS_WOPEN);
		ttychars(tp);
		tp->t_iflag = TTYDEF_IFLAG;
		tp->t_oflag = TTYDEF_OFLAG;
		if (ISSET(sc->ser_swflags, TIOCFLAG_CLOCAL))
			SET(tp->t_cflag, CLOCAL);
		if (ISSET(sc->ser_swflags, TIOCFLAG_CRTSCTS))
			SET(tp->t_cflag, CRTSCTS);
		if (ISSET(sc->ser_swflags, TIOCFLAG_MDMBUF))
			SET(tp->t_cflag, MDMBUF);
		tp->t_cflag = TTYDEF_CFLAG;
		tp->t_lflag = TTYDEF_LFLAG;
		tp->t_ispeed = tp->t_ospeed = serdefaultrate;

		s = spltty();

		serparam(tp, &tp->t_termios);
		ttsetwater(tp);
		
		sermctl(dev, TIOCM_DTR | TIOCM_RTS, DMSET);
		if (ISSET(sc->ser_swflags, TIOCFLAG_SOFTCAR) || SERCUA(dev) || 
		    ISSET(sermctl(dev, 0, DMGET), TIOCM_CD) ||
		    ISSET(tp->t_cflag, MDMBUF))
			SET(tp->t_state, TS_CARR_ON);
		else
			CLR(tp->t_state, TS_CARR_ON);
	} else if (ISSET(tp->t_state, TS_XCLUDE) && p->p_ucred->cr_uid != 0) {
		return (EBUSY);
	} else
		s = spltty();

	if (SERCUA(dev)) {
		if (ISSET(tp->t_state, TS_ISOPEN)) {
			/* Ah, but someone already is dialed in... */
			splx(s);
			return (EBUSY);
		}
		sc->ser_cua = 1;		/* We go into CUA mode */
	}
	tp->t_dev = dev;

	if (ISSET(flag, O_NONBLOCK)) {
		if (!SERCUA(dev) && sc->ser_cua) {
			/* Opening TTY non-blocking... but the CUA is busy */
			splx(s);
			return (EBUSY);
		}
	} else {
		while (sc->ser_cua ||
		    (!ISSET(tp->t_cflag, CLOCAL) &&
		    !ISSET(tp->t_state, TS_CARR_ON))) {
			SET(tp->t_state, TS_WOPEN);
			error = ttysleep(tp, &tp->t_rawq, TTIPRI | PCATCH,
			    ttopen, 0);
			if (!SERCUA(dev) && sc->ser_cua && error == EINTR)
				continue;
			if (error) {
				/* XXX should turn off chip if we're the
				   only waiter */
				if (SERCUA(dev)) {
					sc->ser_cua = 0;
					tp->t_dev = SERUNIT(dev);
				}
				CLR(tp->t_state, TS_WOPEN);
				splx(s);
				return error;
			}
			if (!SERCUA(dev) && sc->ser_cua)
				continue;
		}
	}

	/* This is a way to handle lost XON characters */
	if (ISSET(flag, O_TRUNC) && ISSET(tp->t_state, TS_TTSTOP)) {
		CLR(tp->t_state, TS_TTSTOP);
	        ttstart(tp);
	}
	splx(s);

	ser_open_speed = tp->t_ispeed;
	return ((*linesw[tp->t_line].l_open)(dev, tp));
}

/*ARGSUSED*/
int
serclose(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
	int unit = SERUNIT(dev);
	struct ser_softc *sc = (struct ser_softc *)ser_cd.cd_devs[unit];
	struct tty *tp = sc->ser_tty;
	int closebits;

	(*linesw[tp->t_line].l_close)(tp, flag);
	custom.adkcon = ADKCONF_UARTBRK;	/* clear break */
#ifdef KGDB
	/* 
	 * do not disable interrupts if debugging
	 */
	if (dev != kgdb_dev)
#endif
		custom.intena = INTF_RBF | INTF_TBE;	/* disable interrups */
	custom.intreq = INTF_RBF | INTF_TBE;		/* clear intr req */

	/*
	 * If HUPCL is not set, leave DTR unchanged.
	 */
	if (tp->t_cflag & HUPCL)
		closebits = 0;
	else
		closebits = sermctl(dev, 0, DMGET) & TIOCM_DTR;

	(void) sermctl(dev, closebits, DMSET);

	/*
	 * Idea from dev/isa/com.c: 
	 * sleep a bit so that other side will notice, even if we reopen
	 * immediately.
	 */
	(void)tsleep(tp, TTIPRI, ttclos, hz);
	sc->ser_cua = 0;			/* XXX spltty? */
	ttyclose(tp);
#if not_yet
	if (tp != &ser_cons) {
		remove_vbl_function(&ser_vbl_node[unit]);
		ttyfree(tp);
		sc->ser_tty = NULL;
	}
#endif
	ser_open_speed = tp->t_ispeed;
	return (0);
}

int
serread(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	int unit = SERUNIT(dev);
	struct ser_softc *sc = (struct ser_softc *)ser_cd.cd_devs[unit];
	struct tty *tp = sc->ser_tty;

	if (!tp)
		return (ENXIO);
	return ((*linesw[tp->t_line].l_read)(tp, uio, flag));
}

int
serwrite(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	int unit = SERUNIT(dev);
	struct ser_softc *sc = (struct ser_softc *)ser_cd.cd_devs[unit];
	struct tty *tp = sc->ser_tty;

	if (!tp)
		return (ENXIO);
	return ((*linesw[tp->t_line].l_write)(tp, uio, flag));
}

struct tty *
sertty(dev)
	dev_t dev;
{
	int unit = SERUNIT(dev);
	struct ser_softc *sc = (struct ser_softc *)ser_cd.cd_devs[unit];
	struct tty *tp = sc->ser_tty;

	return (tp);
}

/*
 * We don't do any processing of data here, so we store the raw code
 * obtained from the uart register.  In theory, 110kBaud gives you
 * 11kcps, so 16k buffer should be more than enough, interrupt
 * latency of 1s should never happen, or something is seriously
 * wrong..
 */

static u_short serbuf[SERIBUF_SIZE];
static u_short *sbrpt = serbuf;
static u_short *sbwpt = serbuf;
static u_short sbcnt;
static u_short sbovfl;

/*
 * This is a replacement for the lack of a hardware fifo.  32k should be
 * enough (there's only one unit anyway, so this is not going to
 * accumulate).
 */
void
ser_fastint()
{
	/* 
	 * We're at RBE-level, which is higher than VBL-level which is used
	 * to periodically transmit contents of this buffer up one layer,
	 * so no spl-raising is necessary. 
	 */
	register u_short ints, code;

	ints = custom.intreqr & INTF_RBF;
	if (ints == 0)
		return;

	/*
	 * this register contains both data and status bits!
	 */
	code = custom.serdatr;

	/* 
	 * clear interrupt 
	 */
	custom.intreq = ints;

	/*
	 * check for buffer overflow.
	 */
	if (sbcnt == SERIBUF_SIZE) {
		++sbovfl;
		return;
	}
	/*
	 * store in buffer
	 */
	*sbwpt++ = code;
	if (sbwpt == serbuf + SERIBUF_SIZE)
		sbwpt = serbuf;
	++sbcnt;
	if (sbcnt > SERIBUF_SIZE - 20)
		CLRRTS(ciab.pra);	/* drop RTS if buffer almost full */
}


void
serintr(unit)
	int unit;
{
	int s1, s2, ovfl;
	struct ser_softc *sc = (struct ser_softc *)ser_cd.cd_devs[unit];
	struct tty *tp = sc->ser_tty;

	/*
	 * Make sure we're not interrupted by another
	 * vbl, but allow level5 ints
	 */
	s1 = spltty();

	/*
	 * pass along any acumulated information
	 */
	while (sbcnt > 0 && !ISSET(tp->t_state, TS_TBLOCK)) {
		/* 
		 * no collision with ser_fastint()
		 */
		sereint(unit, *sbrpt++);

		ovfl = 0;
		/* lock against ser_fastint() */
		s2 = splser();
		sbcnt--;
		if (sbrpt == serbuf + SERIBUF_SIZE)
			sbrpt = serbuf;
		if (sbovfl != 0) {
			ovfl = sbovfl;
			sbovfl = 0;
		}
		splx(s2);
		if (ovfl != 0)
			log(LOG_WARNING, "ser0: %d ring buffer overflows.\n",
			    ovfl);
	}
	s2 = splser();
	if (sbcnt == 0 && ISSET(tp->t_state, TS_TBLOCK))
		SETRTS(ciab.pra);	/* start accepting data again */
	splx(s2);
	splx(s1);
}

void
sereint(unit, stat)
	int unit, stat;
{
	struct ser_softc *sc = (struct ser_softc *)ser_cd.cd_devs[unit];
	struct tty *tp = sc->ser_tty;
	u_char ch;
	int c;

	ch = stat & 0xff;
	c = ch;

	if (!ISSET(tp->t_state, TS_ISOPEN)) {
#ifdef KGDB
		/* we don't care about parity errors */
		if (kgdb_dev == makedev(sermajor, unit) && c == FRAME_END)
			kgdb_connect(0);	/* trap into kgdb */
#endif
		return;
	}

	/*
	 * Check for break and (if enabled) parity error.
	 */
	if ((stat & 0x1ff) == 0)
		SET(c, TTY_FE);
	else if (ISSET(tp->t_cflag, PARENB) &&
	    ((ch >> 7) + even_parity[ch & 0x7f] +
	    (ISSET(tp->t_cflag, PARODD) ? 1 : 0)))
		SET(c, TTY_PE);

	if (ISSET(stat, SERDATRF_OVRUN))
		log(LOG_WARNING, "ser0: silo overflow\n");

	(*linesw[tp->t_line].l_rint)(c, tp);
}

/*
 * This interrupt is periodically invoked in the vertical blank
 * interrupt.  It's used to keep track of the modem control lines
 * and (new with the fast_int code) to move accumulated data
 * up into the tty layer.
 */
void
sermint(unit)
	int unit;
{
	struct ser_softc *sc = (struct ser_softc *)ser_cd.cd_devs[unit];
	struct tty *tp = sc->ser_tty;
	u_char stat, last, istat;

	if (!tp)
		return;

	if (!ISSET(tp->t_state, TS_ISOPEN | TS_WOPEN)) {
		sbrpt = sbwpt = serbuf;
		return;
	}
	/*
	 * empty buffer
	 */
	serintr(unit);

	stat = ciab.pra;
	last = last_ciab_pra;
	last_ciab_pra = stat;

	/*
	 * check whether any interesting signal changed state
	 */
	istat = stat ^ last;

	if (ISSET(istat, CIAB_PRA_CD)) {
		if (!ISSET(sc->ser_swflags, TIOCFLAG_SOFTCAR) &&
		    (*linesw[tp->t_line].l_modem)(tp, ISDCD(stat)) ==
		    0) {
			CLRDTR(stat);
			CLRRTS(stat);
			ciab.pra = stat;
			last_ciab_pra = stat;
		}
	}
	if (ISSET(istat, CIAB_PRA_CTS) && ISSET(tp->t_state, TS_ISOPEN) &&
	    ISSET(tp->t_cflag, CRTSCTS)) {
#if 0
		/* the line is up and we want to do rts/cts flow control */
		if (ISCTS(stat)) {
			CLR(tp->t_state, TS_TTSTOP);
			ttstart(tp);
			/* cause tbe-int if we were stuck there */
			custom.intreq = INTF_SETCLR | INTF_TBE;
		} else
			SET(tp->t_state, TS_TTSTOP);
#else
		/* do this on hardware level, not with tty driver */
		if (ISCTS(stat)) {
			CLR(tp->t_state, TS_TTSTOP);
			/* cause TBE interrupt */
			custom.intreq = INTF_SETCLR | INTF_TBE;
		}
#endif
	}
}

int
serioctl(dev, cmd, data, flag, p)
	dev_t dev;
	u_long cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
	int unit = SERUNIT(dev);
	struct ser_softc *sc = (struct ser_softc *)ser_cd.cd_devs[unit];
	struct tty *tp = sc->ser_tty;
	int error;

	if (!tp)
		return ENXIO;

	error = (*linesw[tp->t_line].l_ioctl)(tp, cmd, data, flag, p);
	if (error >= 0)
		return (error);

	error = ttioctl(tp, cmd, data, flag, p);
	if (error >= 0)
		return (error);

	switch (cmd) {
	case TIOCSBRK:
		custom.adkcon = ADKCONF_SETCLR | ADKCONF_UARTBRK;
		break;

	case TIOCCBRK:
		custom.adkcon = ADKCONF_UARTBRK;
		break;

	case TIOCSDTR:
		(void)sermctl(dev, TIOCM_DTR | TIOCM_RTS, DMBIS);
		break;

	case TIOCCDTR:
		(void)sermctl(dev, TIOCM_DTR | TIOCM_RTS, DMBIC);
		break;

	case TIOCMSET:
		(void)sermctl(dev, *(int *) data, DMSET);
		break;

	case TIOCMBIS:
		(void)sermctl(dev, *(int *) data, DMBIS);
		break;

	case TIOCMBIC:
		(void)sermctl(dev, *(int *) data, DMBIC);
		break;

	case TIOCMGET:
		*(int *)data = sermctl(dev, 0, DMGET);
		break;

	case TIOCGFLAGS:
		*(int *)data = sc->ser_swflags;
		break;
	case TIOCSFLAGS:
		error = suser(p->p_ucred, &p->p_acflag); 
		if (error != 0)
			return (EPERM); 

		sc->ser_swflags = *(int *)data;
                sc->ser_swflags &= /* only allow valid flags */
                    (TIOCFLAG_SOFTCAR | TIOCFLAG_CLOCAL | TIOCFLAG_CRTSCTS |
		    TIOCFLAG_MDMBUF | TIOCFLAG_PPS);
		break;
	default:
		return (ENOTTY);
	}

	return (0);
}

int
serparam(tp, t)
	struct tty *tp;
	struct termios *t;
{
	struct ser_softc *sc =
	    (struct ser_softc *)ser_cd.cd_devs[SERUNIT(tp->t_dev)];
	int ospeed = 0;
	tcflag_t cflag, oldcflag;
	int rec_mode, s;
	
	cflag = t->c_cflag;

	if (t->c_ospeed > 0) {
		if (t->c_ospeed < 110)
			return (EINVAL);
		ospeed = SERBRD(t->c_ospeed);
	}

	if (t->c_ispeed && t->c_ispeed != t->c_ospeed)
		return (EINVAL);

	switch (t->c_cflag & CSIZE) {
	case CS7:
		if (!ISSET(t->c_cflag, PARENB))
			return (EINVAL);
		rec_mode = 0;				/* 8 bits */
		break;
	case CS8:
		rec_mode = ISSET(t->c_cflag, PARENB) ? 1 : 0;
		break;
	default:
		return (EINVAL);
	}

	s = spltty();

	/* 
	 * copy to tty
	 */
	ser_open_speed = tp->t_ispeed = t->c_ispeed;
	tp->t_ospeed = t->c_ospeed;
	oldcflag = tp->t_cflag;
	tp->t_cflag = cflag;

	/*
	 * enable interrupts
	 */
	custom.intena = INTF_SETCLR | INTF_RBF | INTF_TBE;
	last_ciab_pra = ciab.pra;

	if (t->c_ospeed == 0)
		(void)sermctl(tp->t_dev, 0, DMSET);	/* hang up line */
	else {
		/* 
		 * (re)enable DTR
		 * and set baud rate & 8/9-bit mode.
		 */
		(void)sermctl(tp->t_dev, TIOCM_DTR | TIOCM_RTS, DMSET);
		custom.serper = (rec_mode << 15) | ospeed;
	}

	/*
	 * If DCD is off and MDMBUF is changed, ask the tty layer if we should
	 * stop the device.                                      
	 */                                                       
	if (!ISDCD(last_ciab_pra) &&
	    !ISSET(sc->ser_swflags, TIOCFLAG_SOFTCAR) &&
	    ISSET(oldcflag, MDMBUF) != ISSET(tp->t_cflag, MDMBUF) &&
	    (*linesw[tp->t_line].l_modem)(tp, 0) == 0) {
		CLRDTR(ciab.pra);
	}

	/* Just to be sure... */
	splx(s);
	serstart(tp);
	return (0);
}

int serhwiflow(tp, flag)
        struct tty *tp;
        int flag;
{
#if 0
	printf ("serhwiflow %d\n", flag);
#endif
        if (flag)
		CLRRTS(ciab.pra);
	else
	        SETRTS(ciab.pra);
        return (1);
}

static void
ser_putchar(tp, c)
	struct tty *tp;
	u_short c;
{
	if ((tp->t_cflag & CSIZE) == CS7 || ISSET(tp->t_cflag, PARENB))
		c &= 0x7f;

	/*
	 * handle parity if necessary
	 */
	if (ISSET(tp->t_cflag, PARENB)) {
		if (even_parity[c])
			c |= 0x80;
		if (ISSET(tp->t_cflag, PARODD))
			c ^= 0x80;
	}
	/* 
	 * add stop bit(s)
	 */
	if (ISSET(tp->t_cflag, CSTOPB))
		c |= 0x300;
	else
		c |= 0x100;

	custom.serdat = c;
}


static u_char ser_outbuf[SEROBUF_SIZE];
static u_char *sob_ptr = ser_outbuf, *sob_end = ser_outbuf;

void
ser_outintr()
{
	struct ser_softc *sc = (struct ser_softc *)ser_cd.cd_devs[0];
	struct tty *tp = sc->ser_tty;
	int s;

	s = spltty();

	if (tp == 0)
		goto out;

	if (!ISSET(custom.intreqr, INTF_TBE))
		goto out;

	/*
	 * clear interrupt
	 */
	custom.intreq = INTF_TBE;

	if (sob_ptr == sob_end) {
		CLR(tp->t_state, TS_BUSY | TS_FLUSH);
		if (tp->t_line)
			(*linesw[tp->t_line].l_start)(tp);
		else
			serstart(tp);
		goto out;
	}

	/*
	 * Do hardware flow control here.  if the CTS line goes down, don't
	 * transmit anything.  That way, we'll be restarted by the periodic
	 * interrupt when CTS comes back up. 
	 */
	if (ISCTS(ciab.pra))
		ser_putchar(tp, *sob_ptr++);
	else
		CLRCTS(last_ciab_pra);	/* Remember that CTS is off */
out:
	splx(s);
}

void
serstart(tp)
	struct tty *tp;
{
	int cc, s, hiwat;
	
	hiwat = 0;

	if (!ISSET(tp->t_state, TS_ISOPEN))
		return;

	s = spltty();
	if (ISSET(tp->t_state, TS_TIMEOUT | TS_TTSTOP))
		goto out;

	cc = tp->t_outq.c_cc;
	if (cc <= tp->t_lowat) {
		if (ISSET(tp->t_state, TS_ASLEEP)) {
			CLR(tp->t_state, TS_ASLEEP);
			wakeup((caddr_t)&tp->t_outq);
		}
		selwakeup(&tp->t_wsel);
	}
	if (cc == 0 || ISSET(tp->t_state, TS_BUSY))
		goto out;

	/*
	 * We only do bulk transfers if using CTSRTS flow control, not for
	 * (probably sloooow) ixon/ixoff devices.
	 */
	if (!ISSET(tp->t_cflag, CRTSCTS))
		cc = 1;

	/*
	 * Limit the amount of output we do in one burst
	 * to prevent hogging the CPU.
	 */
	if (cc > SEROBUF_SIZE) {
		hiwat++;
		cc = SEROBUF_SIZE;
	}
	cc = q_to_b(&tp->t_outq, ser_outbuf, cc);
	if (cc > 0) {
		SET(tp->t_state, TS_BUSY);

		sob_ptr = ser_outbuf;
		sob_end = ser_outbuf + cc;

		/*
		 * Get first character out, then have TBE-interrupts blow out
		 * further characters, until buffer is empty, and TS_BUSY gets
		 * cleared. 
		 */
		ser_putchar(tp, *sob_ptr++);
	}
out:
	splx(s);
}

/*
 * Stop output on a line.
 */
/*ARGSUSED*/
int
serstop(tp, flag)
	struct tty *tp;
	int flag;
{
	int s;

	s = spltty();
	if (ISSET(tp->t_state, TS_BUSY) && !ISSET(tp->t_state, TS_TTSTOP))
		SET(tp->t_state, TS_FLUSH);
	splx(s);
	return (0);
}

int
sermctl(dev, bits, how)
	dev_t dev;
	int bits, how;
{
	int s;
	u_char ub = 0;

	/*
	 * convert TIOCM* mask into CIA mask
	 * which is active low
	 */
	if (how != DMGET) {
		ub = 0;
		if (ISSET(bits, TIOCM_DTR))
			SET(ub, CIAB_PRA_DTR);
		if (ISSET(bits, TIOCM_RTS))
			SET(ub, CIAB_PRA_RTS);
		if (ISSET(bits, TIOCM_CTS))
			SET(ub, CIAB_PRA_CTS);
		if (ISSET(bits, TIOCM_CD))
			SET(ub, CIAB_PRA_CD);
		if (ISSET(bits, TIOCM_RI))
			SET(ub, CIAB_PRA_SEL);	/* collision with /dev/par ! */
		if (ISSET(bits, TIOCM_DSR))
			SET(ub, CIAB_PRA_DSR);
	}
	s = spltty();
	switch (how) {
	case DMSET:
		/* invert and set */
		ciab.pra = ~ub;
		break;

	case DMBIC:
		ciab.pra |= ub;
		ub = ~ciab.pra;
		break;

	case DMBIS:
		ciab.pra &= ~ub;
		ub = ~ciab.pra;
		break;

	case DMGET:
		ub = ~ciab.pra;
		break;
	}
	splx(s);

	bits = 0;
	if (ISSET(ub, CIAB_PRA_DTR))
		SET(bits, TIOCM_DTR);
	if (ISSET(ub, CIAB_PRA_RTS))
		SET(bits, TIOCM_RTS);
	if (ISSET(ub, CIAB_PRA_CTS))
		SET(bits, TIOCM_CTS);
	if (ISSET(ub, CIAB_PRA_CD))
		SET(bits, TIOCM_CD);
	if (ISSET(ub, CIAB_PRA_SEL))
		SET(bits, TIOCM_RI);
	if (ISSET(ub, CIAB_PRA_DSR))
		SET(bits, TIOCM_DSR);

	return (bits);
}

/*
 * Following are all routines needed for SER to act as console
 */
void
sercnprobe(cp)
	struct consdev *cp;
{
	int unit = CONUNIT;
	
	/* locate the major number */
	for (sermajor = 0; sermajor < nchrdev; sermajor++)
		if (cdevsw[sermajor].d_open == (void *)seropen)
			break;

	
	unit = CONUNIT;			/* XXX: ick */

	/*
	 * initialize required fields
	 */
	cp->cn_dev = makedev(sermajor, unit);
	if (serconsole == unit)
		cp->cn_pri = CN_REMOTE;
	else 
		cp->cn_pri = CN_NORMAL;
#ifdef KGDB
	if (major(kgdb_dev) == 1)	/* XXX */
		kgdb_dev = makedev(sermajor, minor(kgdb_dev));
#endif
}

void
sercninit(cp)
	struct consdev *cp;
{
	int unit;

	unit = SERUNIT(cp->cn_dev);

	serinit(unit, serdefaultrate);
	serconsole = unit;
	serconsinit = 1;
}

void
serinit(unit, rate)
	int unit, rate;
{
	int s;

	s = splser();
	/*
	 * might want to fiddle with the CIA later ???
	 */
	custom.serper = (rate >= 110 ? SERBRD(rate) : 0);
	splx(s);
}

int
sercngetc(dev)
	dev_t dev;
{
	u_short stat;
	int c, s;

	s = splser();
	/*
	 * poll
	 */
	while (!ISSET(stat = custom.serdatr & 0xffff, SERDATRF_RBF))
		;
	c = stat & 0xff;
	/*
	 * clear interrupt
	 */
	custom.intreq = INTF_RBF;
	splx(s);
	return(c);
}

/*
 * Console kernel output character routine.
 */
void
sercnputc(dev, c)
	dev_t dev;
	int c;
{
	register int timo;
	int s;

	s = splhigh();

	if (serconsinit == 0) {
		(void)serinit(SERUNIT(dev), serdefaultrate);
		serconsinit = 1;
	}

	/*
	 * wait for any pending transmission to finish 
	 */
	timo = 50000;
	while (!ISSET(custom.serdatr, SERDATRF_TBE) && --timo);

	/*
	 * transmit char.
	 */
	custom.serdat = (c & 0xff) | 0x100;

	/* 
	 * wait for this transmission to complete
	 */
	timo = 1500000;
	while (!ISSET(custom.serdatr, SERDATRF_TBE) && --timo)
		;

	/*
	 * Wait for the device (my vt100..) to process the data, since we
	 * don't do flow-control with cnputc
	 */
	for (timo = 0; timo < 30000; timo++)
		;

	/* 
	 * clear any interrupts generated by this transmission
	 */
	custom.intreq = INTF_TBE;
	splx(s);
}

void
sercnpollc(dev, on)
	dev_t dev;
	int on;
{
}
#endif

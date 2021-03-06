/*	$OpenBSD: zs_kgdb.c,v 1.6 2002/03/14 01:26:46 millert Exp $	*/
/*	$NetBSD: zs_kgdb.c,v 1.9 1996/11/20 18:57:04 gwr Exp $	*/

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
 * Hooks for kgdb when attached vi the z8530 driver
 * XXX - not tested yet...
 *
 * To use this, build a kernel with: option KGDB, and
 * boot that kernel with "-d".  (The kernel will call
 * zs_kgdb_init, kgdb_connect.)  When the console prints
 * "kgdb waiting..." you run "gdb -k kernel" and then
 * connect to the remote using: "target remote /dev/ttyX"
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/device.h>
#include <sys/conf.h>
#include <sys/ioctl.h>
#include <sys/kernel.h>
#include <sys/syslog.h>

#include <dev/ic/z8530reg.h>
#include <machine/z8530var.h>

#include <machine/remote-sl.h>

/* The Sun3 provides a 4.9152 MHz clock to the ZS chips. */
#define PCLK	(9600 * 512)	/* PCLK pin input clock rate */
#define ZSHARD_PRI	6	/* Wired on the CPU board... */

#define ZS_DELAY()			delay(2)

/* The layout of this is hardware-dependent (padding, order). */
struct zschan {
	volatile u_char	zc_csr;		/* ctrl,status, and indirect access */
	u_char		zc_xxx0;
	volatile u_char	zc_data;	/* data */
	u_char		zc_xxx1;
};

extern int kgdb_dev;
extern int kgdb_rate;

struct zschan * zs_get_chan_addr(int zsc_unit, int channel);

extern int  zs_getc(void *arg);
extern void zs_putc(void *arg,	int c);

struct zsops zsops_kgdb;

static u_char zs_kgdb_regs[16] = {
	0,	/* 0: CMD (reset, etc.) */
	ZSWR1_RIE,	/* NOT: (ZSWR1_TIE | ZSWR1_SIE) */
	0x18 + ZSHARD_PRI,	/* IVECT */
	ZSWR3_RX_8 | ZSWR3_RX_ENABLE,
	ZSWR4_CLK_X16 | ZSWR4_ONESB | ZSWR4_EVENP,
	ZSWR5_TX_8 | ZSWR5_TX_ENABLE,
	0,	/* 6: TXSYNC/SYNCLO */
	0,	/* 7: RXSYNC/SYNCHI */
	0,	/* 8: alias for data port */
	ZSWR9_MASTER_IE,
	0,	/*10: Misc. TX/RX control bits */
	ZSWR11_TXCLK_BAUD | ZSWR11_RXCLK_BAUD,
	14,	/*12: BAUDLO (default=9600) */
	0,	/*13: BAUDHI (default=9600) */
	ZSWR14_BAUD_FROM_PCLK | ZSWR14_BAUD_ENA,
	ZSWR15_BREAK_IE | ZSWR15_DCD_IE,
};

/*
 * This replaces "zs_reset()" in the sparc driver.
 */
static void
zs_setparam(cs, iena, rate)
	struct zs_chanstate *cs;
	int iena;
	int rate;
{
	int s, tconst;

	bcopy(zs_kgdb_regs, cs->cs_preg, 16);

	if (iena == 0) {
		cs->cs_preg[1] = 0;
	}

	/* Initialize the speed, etc. */
	tconst = BPS_TO_TCONST(cs->cs_brg_clk, rate);
	cs->cs_preg[5] |= ZSWR5_DTR | ZSWR5_RTS;
	cs->cs_preg[12] = tconst;
	cs->cs_preg[13] = tconst >> 8;

	s = splhigh();
	zs_loadchannelregs(cs);
	splx(s);
}

/*
 * Set up for kgdb; called at boot time before configuration.
 * KGDB interrupts will be enabled later when zs0 is configured.
 */
void
zs_kgdb_init()
{
	struct zs_chanstate cs;
	volatile struct zschan *zc;
	int channel, zsc_unit;

	if (major(kgdb_dev) != ZSTTY_MAJOR)
		return;

	/* Note: (ttya,ttyb) on zsc1, and (ttyc,ttyd) on zsc0 */
	zsc_unit = 2 - (kgdb_dev & 2);
	channel  =      kgdb_dev & 1;
	printf("zs_kgdb_init: attaching zstty%d at %d baud\n",
		   channel, kgdb_rate);

	/* Setup temporary chanstate. */
	bzero((caddr_t)&cs, sizeof(cs));
	zc = zs_get_chan_addr(zsc_unit, channel);
	cs.cs_reg_csr  = &zc->zc_csr;
	cs.cs_reg_data = &zc->zc_data;
	cs.cs_channel = channel;
	cs.cs_brg_clk = PCLK / 16;

	/* Now set parameters. (interrupts disabled) */
	zs_setparam(&cs, 0, kgdb_rate);

	/* Store the getc/putc functions and arg. */
	kgdb_attach(zs_getc, zs_putc, (void *)zc);
}

/*
 * This is a "hook" called by zstty_attach to allow the tty
 * to be "taken over" for exclusive use by kgdb.
 * Return non-zero if this is the kgdb port.
 *
 * Set the speed to kgdb_rate, CS8, etc.
 */
int
zs_check_kgdb(cs, dev)
	struct zs_chanstate *cs;
	int dev;
{

	if (dev != kgdb_dev)
		return (0);

	/*
	 * Yes, this is the kgdb port.  Finish the autoconfig
	 * message and set up the port for our exclusive use.
	 */
	printf(" (kgdb)\n");

	cs->cs_private = NULL;
	cs->cs_ops = &zsops_kgdb;

	/* Now set parameters. (interrupts enabled) */
	zs_setparam(&cs, 1, kgdb_rate);

	return (1);
}

/*
 * KGDB framing character received: enter kernel debugger.  This probably
 * should time out after a few seconds to avoid hanging on spurious input.
 */
void
zskgdb()
{
	int unit = minor(kgdb_dev);

	printf("zstty%d: kgdb interrupt\n", unit);
	/* This will trap into the debugger. */
	kgdb_connect(1);
}


/****************************************************************
 * Interface to the lower layer (zscc)
 ****************************************************************/

int kgdb_input_lost;

static void
zs_kgdb_rxint(cs)
	register struct zs_chanstate *cs;
{
	register u_char c, rr1;

	/* Read the input data ASAP. */
	c = zs_read_data(cs);

	/* Save the status register too. */
	rr1 = zs_read_reg(cs, 1);

	if (rr1 & (ZSRR1_FE | ZSRR1_DO | ZSRR1_PE)) {
		/* Clear the receive error. */
		zs_write_csr(cs, ZSWR0_RESET_ERRORS);
	}

	if (c == FRAME_START) {
		zskgdb();
	} else {
		kgdb_input_lost++;
	}
}

static void
zs_kgdb_txint(cs)
	register struct zs_chanstate *cs;
{
	register int rr0;

	rr0 = zs_read_csr(cs);
	zs_write_csr(cs, ZSWR0_RESET_TXINT);
}

static void
zs_kgdb_stint(cs)
	register struct zs_chanstate *cs;
{
	register int rr0;

	rr0 = zs_read_csr(cs);
	zs_write_csr(cs, ZSWR0_RESET_STATUS);
}

static void
zs_kgdb_softint(cs)
	struct zs_chanstate *cs;
{
	printf("zs_kgdb_softint?\n");
}

struct zsops zsops_kgdb = {
	zs_kgdb_rxint,	/* receive char available */
	zs_kgdb_stint,	/* external/status */
	zs_kgdb_txint,	/* xmit buffer empty */
	zs_kgdb_softint,	/* process software interrupt */
};

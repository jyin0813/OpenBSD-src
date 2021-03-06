/*	$OpenBSD: fd.c,v 1.23 1996/10/16 12:46:22 deraadt Exp $	*/
/*	$NetBSD: fd.c,v 1.90 1996/05/12 23:12:03 mycroft Exp $	*/

/*-
 * Copyright (c) 1993, 1994, 1995 Charles Hannum.
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Don Ahn.
 *
 * Portions Copyright (c) 1993, 1994 by
 *  jc@irbs.UUCP (John Capo)
 *  vak@zebub.msk.su (Serge Vakulenko)
 *  ache@astral.msk.su (Andrew A. Chernov)
 *  joerg_wunsch@uriah.sax.de (Joerg Wunsch)
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
 *	@(#)fd.c	7.4 (Berkeley) 5/25/91
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/device.h>
#include <sys/disklabel.h>
#include <sys/dkstat.h>
#include <sys/disk.h>
#include <sys/buf.h>
#include <sys/malloc.h>
#include <sys/uio.h>
#include <sys/mtio.h>
#include <sys/proc.h>
#include <sys/syslog.h>
#include <sys/queue.h>

#include <machine/cpu.h>
#include <machine/bus.h>
#include <machine/conf.h>
#include <machine/intr.h>
#include <machine/ioctl_fd.h>

#include <dev/isa/isavar.h>
#include <dev/isa/isadmavar.h>
#include <i386/isa/fdreg.h>

#include <dev/ic/mc146818reg.h>			/* for NVRAM access */
#include <i386/isa/nvram.h>

#include <i386/isa/fdlink.h>

/* XXX misuse a flag to identify format operation */
#define B_FORMAT B_XXX

#define b_cylin b_resid

/* fd_type struct now in ioctl_fd.h */

/* The order of entries in the following table is important -- BEWARE! */
struct fd_type fd_types[] = {
        { 18,2,36,2,0xff,0xcf,0x1b,0x6c,80,2880,1,FDC_500KBPS,"1.44MB"    }, /* 1.44MB diskette */
        { 15,2,30,2,0xff,0xdf,0x1b,0x54,80,2400,1,FDC_500KBPS, "1.2MB"    }, /* 1.2 MB AT-diskettes */
        {  9,2,18,2,0xff,0xdf,0x23,0x50,40, 720,2,FDC_300KBPS, "360KB/AT" }, /* 360kB in 1.2MB drive */
        {  9,2,18,2,0xff,0xdf,0x2a,0x50,40, 720,1,FDC_250KBPS, "360KB/PC" }, /* 360kB PC diskettes */
        {  9,2,18,2,0xff,0xdf,0x2a,0x50,80,1440,1,FDC_250KBPS, "720KB"    }, /* 3.5" 720kB diskette */
        {  9,2,18,2,0xff,0xdf,0x23,0x50,80,1440,1,FDC_300KBPS, "720KB/x"  }, /* 720kB in 1.2MB drive */
        {  9,2,18,2,0xff,0xdf,0x2a,0x50,40, 720,2,FDC_250KBPS, "360KB/x"  }, /* 360kB in 720kB drive */
};

/* software state, per disk (with up to 4 disks per ctlr) */
struct fd_softc {
	struct device sc_dev;
	struct disk sc_dk;

	struct fd_type *sc_deftype;	/* default type descriptor */
	struct fd_type *sc_type;	/* current type descriptor */

	daddr_t	sc_blkno;	/* starting block number */
	int sc_bcount;		/* byte count left */
 	int sc_opts;			/* user-set options */
	int sc_skip;		/* bytes already transferred */
	int sc_nblks;		/* number of blocks currently tranferring */
	int sc_nbytes;		/* number of bytes currently tranferring */

	int sc_drive;		/* physical unit number */
	int sc_flags;
#define	FD_OPEN		0x01		/* it's open */
#define	FD_MOTOR	0x02		/* motor should be on */
#define	FD_MOTOR_WAIT	0x04		/* motor coming up */
	int sc_cylin;		/* where we think the head is */

	void *sc_sdhook;	/* saved shutdown hook for drive. */

	TAILQ_ENTRY(fd_softc) sc_drivechain;
	int sc_ops;		/* I/O ops since last switch */
	struct buf sc_q;	/* head of buf chain */
};

/* floppy driver configuration */
int fdprobe __P((struct device *, void *, void *));
void fdattach __P((struct device *, struct device *, void *));

struct cfattach fd_ca = {
	sizeof(struct fd_softc), fdprobe, fdattach
};

struct cfdriver fd_cd = {
	NULL, "fd", DV_DISK
};

void fdgetdisklabel __P((struct fd_softc *));
int fd_get_parms __P((struct fd_softc *));
void fdstrategy __P((struct buf *));
void fdstart __P((struct fd_softc *));
int fdintr __P((struct fdc_softc *));

struct dkdriver fddkdriver = { fdstrategy };

void fd_set_motor __P((struct fdc_softc *fdc, int reset));
void fd_motor_off __P((void *arg));
void fd_motor_on __P((void *arg));
void fdfinish __P((struct fd_softc *fd, struct buf *bp));
int fdformat __P((dev_t, struct fd_formb *, struct proc *));
__inline struct fd_type *fd_dev_to_type __P((struct fd_softc *, dev_t));
void fdretry __P((struct fd_softc *));
void fdtimeout __P((void *));

int
fdprobe(parent, match, aux)
	struct device *parent;
	void *match, *aux;
{
	struct fdc_softc *fdc = (void *)parent;
	struct cfdata *cf = match;
	struct fdc_attach_args *fa = aux;
	int drive = fa->fa_drive;
	bus_chipset_tag_t bc = fdc->sc_bc;
	bus_io_handle_t ioh = fdc->sc_ioh;
	int n;

	if (cf->cf_loc[0] != -1 && cf->cf_loc[0] != drive)
		return 0;
	/*
	 * XXX
	 * This is to work around some odd interactions between this driver
	 * and SMC Ethernet cards.
	 */
	if (cf->cf_loc[0] == -1 && drive >= 2)
		return 0;

	/*
	 * We want to keep the flags config gave us.
	 */
	fa->fa_flags = cf->cf_flags;

	/* select drive and turn on motor */
	bus_io_write_1(bc, ioh, fdout, drive | FDO_FRST | FDO_MOEN(drive));
	/* wait for motor to spin up */
	delay(250000);
	out_fdc(bc, ioh, NE7CMD_RECAL);
	out_fdc(bc, ioh, drive);
	/* wait for recalibrate */
	delay(2000000);
	out_fdc(bc, ioh, NE7CMD_SENSEI);
	n = fdcresult(fdc);
#ifdef FD_DEBUG
	{
		int i;
		printf("fdprobe: status");
		for (i = 0; i < n; i++)
			printf(" %x", fdc->sc_status[i]);
		printf("\n");
	}
#endif
	if (n != 2 || (fdc->sc_status[0] & 0xf8) != 0x20)
		return 0;
	/* turn off motor */
	bus_io_write_1(bc, ioh, fdout, FDO_FRST);

	return 1;
}

/*
 * Controller is working, and drive responded.  Attach it.
 */
void
fdattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct fdc_softc *fdc = (void *)parent;
	struct fd_softc *fd = (void *)self;
	struct fdc_attach_args *fa = aux;
	struct fd_type *type = fa->fa_deftype;
	int drive = fa->fa_drive;

	if (!type || (fa->fa_flags & 0x10)) {
		/* The config has overridden this. */
		switch (fa->fa_flags & 0x07) {
		/* 1 is reserved for 2.88MB */
		case 2:	/* 1.44MB */
			type = &fd_types[0];
			break;
		case 3: /* 1.2MB */
			type = &fd_types[1];
			break;
		case 4: /* 720K */
			type = &fd_types[4];
			break;
		case 5: /* 360K */
			type = &fd_types[3];
			break;
		}
	}

	if (type)
		printf(": %s %d cyl, %d head, %d sec\n", type->name,
		    type->tracks, type->heads, type->sectrac);
	else
		printf(": density unknown\n");

	fd->sc_cylin = -1;
	fd->sc_drive = drive;
	fd->sc_deftype = type;
	fdc->sc_fd[drive] = fd;

	/*
	 * Initialize and attach the disk structure.
	 */
	fd->sc_dk.dk_name = fd->sc_dev.dv_xname;
	fd->sc_dk.dk_driver = &fddkdriver;
	disk_attach(&fd->sc_dk);

	dk_establish(&fd->sc_dk, &fd->sc_dev);
	/* Needed to power off if the motor is on when we halt. */
	fd->sc_sdhook = shutdownhook_establish(fd_motor_off, fd);
}

/*
 * Translate nvram type into internal data structure.  Return NULL for
 * none/unknown/unusable.
 */
struct fd_type *
fd_nvtotype(fdc, nvraminfo, drive)
	char *fdc;
	int nvraminfo, drive;
{
	int type;

	type = (drive == 0 ? nvraminfo : nvraminfo << 4) & 0xf0;
	switch (type) {
	case NVRAM_DISKETTE_NONE:
		return NULL;
	case NVRAM_DISKETTE_12M:
		return &fd_types[1];
	case NVRAM_DISKETTE_TYPE5:
	case NVRAM_DISKETTE_TYPE6:
		/* XXX We really ought to handle 2.88MB format. */
	case NVRAM_DISKETTE_144M:
		return &fd_types[0];
	case NVRAM_DISKETTE_360K:
		return &fd_types[3];
	case NVRAM_DISKETTE_720K:
		return &fd_types[4];
	default:
		printf("%s: drive %d: unknown device type 0x%x\n",
		    fdc, drive, type);
		return NULL;
	}
}

__inline struct fd_type *
fd_dev_to_type(fd, dev)
	struct fd_softc *fd;
	dev_t dev;
{
	int type = FDTYPE(dev);

	if (type > (sizeof(fd_types) / sizeof(fd_types[0])))
		return NULL;
	return type ? &fd_types[type - 1] : fd->sc_deftype;
}

void
fdstrategy(bp)
	register struct buf *bp;	/* IO operation to perform */
{
	struct fd_softc *fd;
	int unit = FDUNIT(bp->b_dev);
	int sz;
 	int s;

	/* Valid unit, controller, and request? */
	if (unit >= fd_cd.cd_ndevs ||
	    (fd = fd_cd.cd_devs[unit]) == 0 ||
	    bp->b_blkno < 0 ||
	    ((bp->b_bcount % FDC_BSIZE) != 0 &&
	     (bp->b_flags & B_FORMAT) == 0)) {
		bp->b_error = EINVAL;
		goto bad;
	}

	/* If it's a null transfer, return immediately. */
	if (bp->b_bcount == 0)
		goto done;

	sz = howmany(bp->b_bcount, FDC_BSIZE);

	if (bp->b_blkno + sz > fd->sc_type->size) {
		sz = fd->sc_type->size - bp->b_blkno;
		if (sz == 0)
			/* If exactly at end of disk, return EOF. */
			goto done;
		if (sz < 0) {
			/* If past end of disk, return EINVAL. */
			bp->b_error = EINVAL;
			goto bad;
		}
		/* Otherwise, truncate request. */
		bp->b_bcount = sz << DEV_BSHIFT;
	}

 	bp->b_cylin = bp->b_blkno / (FDC_BSIZE / DEV_BSIZE) / fd->sc_type->seccyl;

#ifdef FD_DEBUG
	printf("fdstrategy: b_blkno %d b_bcount %d blkno %d cylin %d sz %d\n",
	    bp->b_blkno, bp->b_bcount, fd->sc_blkno, bp->b_cylin, sz);
#endif

	/* Queue transfer on drive, activate drive and controller if idle. */
	s = splbio();
	disksort(&fd->sc_q, bp);
	untimeout(fd_motor_off, fd); /* a good idea */
	if (!fd->sc_q.b_active)
		fdstart(fd);
#ifdef DIAGNOSTIC
	else {
		struct fdc_softc *fdc = (void *)fd->sc_dev.dv_parent;
		if (fdc->sc_state == DEVIDLE) {
			printf("fdstrategy: controller inactive\n");
			fdcstart(fdc);
		}
	}
#endif
	splx(s);
	return;

bad:
	bp->b_flags |= B_ERROR;
done:
	/* Toss transfer; we're done early. */
	bp->b_resid = bp->b_bcount;
	biodone(bp);
}

void
fdstart(fd)
	struct fd_softc *fd;
{
	struct fdc_softc *fdc = (void *)fd->sc_dev.dv_parent;
	int active = fdc->sc_drives.tqh_first != 0;

	/* Link into controller queue. */
	fd->sc_q.b_active = 1;
	TAILQ_INSERT_TAIL(&fdc->sc_drives, fd, sc_drivechain);

	/* If controller not already active, start it. */
	if (!active)
		fdcstart(fdc);
}

void
fdfinish(fd, bp)
	struct fd_softc *fd;
	struct buf *bp;
{
	struct fdc_softc *fdc = (void *)fd->sc_dev.dv_parent;

	/*
	 * Move this drive to the end of the queue to give others a `fair'
	 * chance.  We only force a switch if N operations are completed while
	 * another drive is waiting to be serviced, since there is a long motor
	 * startup delay whenever we switch.
	 */
	if (fd->sc_drivechain.tqe_next && ++fd->sc_ops >= 8) {
		fd->sc_ops = 0;
		TAILQ_REMOVE(&fdc->sc_drives, fd, sc_drivechain);
		if (bp->b_actf) {
			TAILQ_INSERT_TAIL(&fdc->sc_drives, fd, sc_drivechain);
		} else
			fd->sc_q.b_active = 0;
	}
	bp->b_resid = fd->sc_bcount;
	fd->sc_skip = 0;
	fd->sc_q.b_actf = bp->b_actf;

	biodone(bp);
	/* turn off motor 5s from now */
	timeout(fd_motor_off, fd, 5 * hz);
	fdc->sc_state = DEVIDLE;
}

int
fdread(dev, uio, flags)
	dev_t dev;
	struct uio *uio;
	int flags;
{

	return (physio(fdstrategy, NULL, dev, B_READ, minphys, uio));
}

int
fdwrite(dev, uio, flags)
	dev_t dev;
	struct uio *uio;
	int flags;
{

	return (physio(fdstrategy, NULL, dev, B_WRITE, minphys, uio));
}

void
fd_set_motor(fdc, reset)
	struct fdc_softc *fdc;
	int reset;
{
	struct fd_softc *fd;
	u_char status;
	int n;

	if ((fd = fdc->sc_drives.tqh_first) != NULL)
		status = fd->sc_drive;
	else
		status = 0;
	if (!reset)
		status |= FDO_FRST | FDO_FDMAEN;
	for (n = 0; n < 4; n++)
		if ((fd = fdc->sc_fd[n]) && (fd->sc_flags & FD_MOTOR))
			status |= FDO_MOEN(n);
	bus_io_write_1(fdc->sc_bc, fdc->sc_ioh, fdout, status);
}

void
fd_motor_off(arg)
	void *arg;
{
	struct fd_softc *fd = arg;
	int s;

	s = splbio();
	fd->sc_flags &= ~(FD_MOTOR | FD_MOTOR_WAIT);
	fd_set_motor((struct fdc_softc *)fd->sc_dev.dv_parent, 0);
	splx(s);
}

void
fd_motor_on(arg)
	void *arg;
{
	struct fd_softc *fd = arg;
	struct fdc_softc *fdc = (void *)fd->sc_dev.dv_parent;
	int s;

	s = splbio();
	fd->sc_flags &= ~FD_MOTOR_WAIT;
	if ((fdc->sc_drives.tqh_first == fd) && (fdc->sc_state == MOTORWAIT))
		(void) fdintr(fdc);
	splx(s);
}

int
fdopen(dev, flags, mode, p)
	dev_t dev;
	int flags;
	int mode;
	struct proc *p;
{
 	int unit;
	struct fd_softc *fd;
	struct fd_type *type;

	unit = FDUNIT(dev);
	if (unit >= fd_cd.cd_ndevs)
		return ENXIO;
	fd = fd_cd.cd_devs[unit];
	if (fd == 0)
		return ENXIO;
	type = fd_dev_to_type(fd, dev);
	if (type == NULL)
		return ENXIO;

	if ((fd->sc_flags & FD_OPEN) != 0 &&
	    fd->sc_type != type)
		return EBUSY;

	fd->sc_type = type;
	fd->sc_cylin = -1;
	fd->sc_flags |= FD_OPEN;

	return 0;
}

int
fdclose(dev, flags, mode, p)
	dev_t dev;
	int flags;
	int mode;
	struct proc *p;
{
	struct fd_softc *fd = fd_cd.cd_devs[FDUNIT(dev)];

	fd->sc_flags &= ~FD_OPEN;
	fd->sc_opts &= ~FDOPT_NORETRY;
	return 0;
}

int
fdsize(dev)
	dev_t dev;
{

	/* Swapping to floppies would not make sense. */
	return -1;
}

int
fddump(dev, blkno, va, size)
	dev_t dev;
	daddr_t blkno;
	caddr_t va;
	size_t size;
{

	/* Not implemented. */
	return ENXIO;
}

/*
 * Called from the controller.
 */
int
fdintr(fdc)
	struct fdc_softc *fdc;
{
#define	st0	fdc->sc_status[0]
#define	cyl	fdc->sc_status[1]
	struct fd_softc *fd;
	struct buf *bp;
	bus_chipset_tag_t bc = fdc->sc_bc;
	bus_io_handle_t ioh = fdc->sc_ioh;
	int read, head, sec, i, nblks;
	struct fd_type *type;
	struct fd_formb *finfo = NULL;

loop:
	/* Is there a transfer to this drive?  If not, deactivate drive. */
	fd = fdc->sc_drives.tqh_first;
	if (fd == NULL) {
		fdc->sc_state = DEVIDLE;
		return 1;
	}

	bp = fd->sc_q.b_actf;
	if (bp == NULL) {
		fd->sc_ops = 0;
		TAILQ_REMOVE(&fdc->sc_drives, fd, sc_drivechain);
		fd->sc_q.b_active = 0;
		goto loop;
	}

	if (bp->b_flags & B_FORMAT)
	    finfo = (struct fd_formb *)bp->b_data;

	switch (fdc->sc_state) {
	case DEVIDLE:
		fdc->sc_errors = 0;
		fd->sc_skip = 0;
		fd->sc_bcount = bp->b_bcount;
		fd->sc_blkno = bp->b_blkno / (FDC_BSIZE / DEV_BSIZE);
		untimeout(fd_motor_off, fd);
		if ((fd->sc_flags & FD_MOTOR_WAIT) != 0) {
			fdc->sc_state = MOTORWAIT;
			return 1;
		}
		if ((fd->sc_flags & FD_MOTOR) == 0) {
			/* Turn on the motor, being careful about pairing. */
			struct fd_softc *ofd = fdc->sc_fd[fd->sc_drive ^ 1];
			if (ofd && ofd->sc_flags & FD_MOTOR) {
				untimeout(fd_motor_off, ofd);
				ofd->sc_flags &= ~(FD_MOTOR | FD_MOTOR_WAIT);
			}
			fd->sc_flags |= FD_MOTOR | FD_MOTOR_WAIT;
			fd_set_motor(fdc, 0);
			fdc->sc_state = MOTORWAIT;
			/* Allow .25s for motor to stabilize. */
			timeout(fd_motor_on, fd, hz / 4);
			return 1;
		}
		/* Make sure the right drive is selected. */
		fd_set_motor(fdc, 0);

		/* fall through */
	case DOSEEK:
	doseek:
		if (fd->sc_cylin == bp->b_cylin)
			goto doio;

		out_fdc(bc, ioh, NE7CMD_SPECIFY);/* specify command */
		out_fdc(bc, ioh, fd->sc_type->steprate);
		out_fdc(bc, ioh, 6);		/* XXX head load time == 6ms */

		out_fdc(bc, ioh, NE7CMD_SEEK);	/* seek function */
		out_fdc(bc, ioh, fd->sc_drive);	/* drive number */
		out_fdc(bc, ioh, bp->b_cylin * fd->sc_type->step);

		fd->sc_cylin = -1;
		fdc->sc_state = SEEKWAIT;

		fd->sc_dk.dk_seek++;
		disk_busy(&fd->sc_dk);

		timeout(fdtimeout, fd, 4 * hz);
		return 1;

	case DOIO:
	doio:
		type = fd->sc_type;
		if (finfo)
		    fd->sc_skip = (char *)&(finfo->fd_formb_cylno(0)) -
			(char *)finfo;
		sec = fd->sc_blkno % type->seccyl;
		nblks = type->seccyl - sec;
		nblks = min(nblks, fd->sc_bcount / FDC_BSIZE);
		nblks = min(nblks, FDC_MAXIOSIZE / FDC_BSIZE);
		fd->sc_nblks = nblks;
		fd->sc_nbytes = finfo ? bp->b_bcount : nblks * FDC_BSIZE;
		head = sec / type->sectrac;
		sec -= head * type->sectrac;
#ifdef DIAGNOSTIC
		{int block;
		 block = (fd->sc_cylin * type->heads + head) * type->sectrac + sec;
		 if (block != fd->sc_blkno) {
			 printf("fdintr: block %d != blkno %d\n", block, fd->sc_blkno);
#ifdef DDB
			 Debugger();
#endif
		 }}
#endif
		read = bp->b_flags & B_READ ? DMAMODE_READ : DMAMODE_WRITE;
#ifdef NEWCONFIG
		at_dma(read, bp->b_data + fd->sc_skip, fd->sc_nbytes,
		    fdc->sc_drq);
#else
		isa_dmastart(read, bp->b_data + fd->sc_skip, fd->sc_nbytes,
		    fdc->sc_drq);
#endif
		bus_io_write_1(bc, ioh, fdctl, type->rate);
#ifdef FD_DEBUG
		printf("fdintr: %s drive %d track %d head %d sec %d nblks %d\n",
		    read ? "read" : "write", fd->sc_drive, fd->sc_cylin, head,
		    sec, nblks);
#endif
		if (finfo) {
                        /* formatting */
			if (out_fdc(bc, ioh, NE7CMD_FORMAT) < 0) {
			    fdc->sc_errors = 4;
			    fdretry(fd);
			    goto loop;
			}
                        out_fdc(bc, ioh, (head << 2) | fd->sc_drive);
                        out_fdc(bc, ioh, finfo->fd_formb_secshift);
                        out_fdc(bc, ioh, finfo->fd_formb_nsecs);
                        out_fdc(bc, ioh, finfo->fd_formb_gaplen);
                        out_fdc(bc, ioh, finfo->fd_formb_fillbyte);
		} else {
			if (read)
				out_fdc(bc, ioh, NE7CMD_READ);	/* READ */
			else
				out_fdc(bc, ioh, NE7CMD_WRITE);	/* WRITE */
			out_fdc(bc, ioh, (head << 2) | fd->sc_drive);
			out_fdc(bc, ioh, fd->sc_cylin);		/* track */
			out_fdc(bc, ioh, head);
			out_fdc(bc, ioh, sec + 1);		/* sector +1 */
			out_fdc(bc, ioh, type->secsize);	/* sector size */
			out_fdc(bc, ioh, type->sectrac);	/* sectors/track */
			out_fdc(bc, ioh, type->gap1);		/* gap1 size */
			out_fdc(bc, ioh, type->datalen);	/* data length */
		}
		fdc->sc_state = IOCOMPLETE;

		disk_busy(&fd->sc_dk);

		/* allow 2 seconds for operation */
		timeout(fdtimeout, fd, 2 * hz);
		return 1;				/* will return later */

	case SEEKWAIT:
		untimeout(fdtimeout, fd);
		fdc->sc_state = SEEKCOMPLETE;
		/* allow 1/50 second for heads to settle */
		timeout(fdcpseudointr, fdc, hz / 50);
		return 1;

	case SEEKCOMPLETE:
		disk_unbusy(&fd->sc_dk, 0);	/* no data on seek */

		/* Make sure seek really happened. */
		out_fdc(bc, ioh, NE7CMD_SENSEI);
		if (fdcresult(fdc) != 2 || (st0 & 0xf8) != 0x20 ||
		    cyl != bp->b_cylin * fd->sc_type->step) {
#ifdef FD_DEBUG
			fdcstatus(&fd->sc_dev, 2, "seek failed");
#endif
			fdretry(fd);
			goto loop;
		}
		fd->sc_cylin = bp->b_cylin;
		goto doio;

	case IOTIMEDOUT:
#ifdef NEWCONFIG
		at_dma_abort(fdc->sc_drq);
#else
		isa_dmaabort(fdc->sc_drq);
#endif
	case SEEKTIMEDOUT:
	case RECALTIMEDOUT:
	case RESETTIMEDOUT:
		fdretry(fd);
		goto loop;

	case IOCOMPLETE: /* IO DONE, post-analyze */
		untimeout(fdtimeout, fd);

		disk_unbusy(&fd->sc_dk, (bp->b_bcount - bp->b_resid));

		if (fdcresult(fdc) != 7 || (st0 & 0xf8) != 0) {
#ifdef NEWCONFIG
			at_dma_abort(fdc->sc_drq);
#else
			isa_dmaabort(fdc->sc_drq);
#endif
#ifdef FD_DEBUG
			fdcstatus(&fd->sc_dev, 7, bp->b_flags & B_READ ?
			    "read failed" : "write failed");
			printf("blkno %d nblks %d\n",
			    fd->sc_blkno, fd->sc_nblks);
#endif
			fdretry(fd);
			goto loop;
		}
#ifdef NEWCONFIG
		at_dma_terminate(fdc->sc_drq);
#else
		read = bp->b_flags & B_READ ? DMAMODE_READ : DMAMODE_WRITE;
		isa_dmadone(read, bp->b_data + fd->sc_skip, fd->sc_nbytes,
		    fdc->sc_drq);
#endif
		if (fdc->sc_errors) {
			diskerr(bp, "fd", "soft error", LOG_PRINTF,
			    fd->sc_skip / FDC_BSIZE, (struct disklabel *)NULL);
			printf("\n");
			fdc->sc_errors = 0;
		}
		fd->sc_blkno += fd->sc_nblks;
		fd->sc_skip += fd->sc_nbytes;
		fd->sc_bcount -= fd->sc_nbytes;
		if (!finfo && fd->sc_bcount > 0) {
			bp->b_cylin = fd->sc_blkno / fd->sc_type->seccyl;
			goto doseek;
		}
		fdfinish(fd, bp);
		goto loop;

	case DORESET:
		/* try a reset, keep motor on */
		fd_set_motor(fdc, 1);
		delay(100);
		fd_set_motor(fdc, 0);
		fdc->sc_state = RESETCOMPLETE;
		timeout(fdtimeout, fd, hz / 2);
		return 1;			/* will return later */

	case RESETCOMPLETE:
		untimeout(fdtimeout, fd);
		/* clear the controller output buffer */
		for (i = 0; i < 4; i++) {
			out_fdc(bc, ioh, NE7CMD_SENSEI);
			(void) fdcresult(fdc);
		}

		/* fall through */
	case DORECAL:
		out_fdc(bc, ioh, NE7CMD_RECAL);	/* recalibrate function */
		out_fdc(bc, ioh, fd->sc_drive);
		fdc->sc_state = RECALWAIT;
		timeout(fdtimeout, fd, 5 * hz);
		return 1;			/* will return later */

	case RECALWAIT:
		untimeout(fdtimeout, fd);
		fdc->sc_state = RECALCOMPLETE;
		/* allow 1/30 second for heads to settle */
		timeout(fdcpseudointr, fdc, hz / 30);
		return 1;			/* will return later */

	case RECALCOMPLETE:
		out_fdc(bc, ioh, NE7CMD_SENSEI);
		if (fdcresult(fdc) != 2 || (st0 & 0xf8) != 0x20 || cyl != 0) {
#ifdef FD_DEBUG
			fdcstatus(&fd->sc_dev, 2, "recalibrate failed");
#endif
			fdretry(fd);
			goto loop;
		}
		fd->sc_cylin = 0;
		goto doseek;

	case MOTORWAIT:
		if (fd->sc_flags & FD_MOTOR_WAIT)
			return 1;		/* time's not up yet */
		goto doseek;

	default:
		fdcstatus(&fd->sc_dev, 0, "stray interrupt");
		return 1;
	}
#ifdef DIAGNOSTIC
	panic("fdintr: impossible");
#endif
#undef	st0
#undef	cyl
}

void
fdtimeout(arg)
	void *arg;
{
	struct fd_softc *fd = arg;
	struct fdc_softc *fdc = (void *)fd->sc_dev.dv_parent;
	int s;

	s = splbio();
#ifdef DEBUG
	log(LOG_ERR,"fdtimeout: state %d\n", fdc->sc_state);
#endif
	fdcstatus(&fd->sc_dev, 0, "timeout");

	if (fd->sc_q.b_actf)
		fdc->sc_state++;
	else
		fdc->sc_state = DEVIDLE;

	(void) fdintr(fdc);
	splx(s);
}

void
fdretry(fd)
	struct fd_softc *fd;
{
	struct fdc_softc *fdc = (void *)fd->sc_dev.dv_parent;
	struct buf *bp = fd->sc_q.b_actf;

	if (fd->sc_opts & FDOPT_NORETRY)
	    goto fail;
	switch (fdc->sc_errors) {
	case 0:
		/* try again */
		fdc->sc_state = DOSEEK;
		break;

	case 1: case 2: case 3:
		/* didn't work; try recalibrating */
		fdc->sc_state = DORECAL;
		break;

	case 4:
		/* still no go; reset the bastard */
		fdc->sc_state = DORESET;
		break;

	default:
	fail:
		diskerr(bp, "fd", "hard error", LOG_PRINTF,
		    fd->sc_skip / FDC_BSIZE, (struct disklabel *)NULL);
		printf(" (st0 %b st1 %b st2 %b cyl %d head %d sec %d)\n",
		    fdc->sc_status[0], NE7_ST0BITS,
		    fdc->sc_status[1], NE7_ST1BITS,
		    fdc->sc_status[2], NE7_ST2BITS,
		    fdc->sc_status[3], fdc->sc_status[4], fdc->sc_status[5]);

		bp->b_flags |= B_ERROR;
		bp->b_error = EIO;
		fdfinish(fd, bp);
	}
	fdc->sc_errors++;
}

int
fdioctl(dev, cmd, addr, flag, p)
	dev_t dev;
	u_long cmd;
	caddr_t addr;
	int flag;
	struct proc *p;
{
	struct fd_softc *fd = fd_cd.cd_devs[FDUNIT(dev)];
	struct disklabel buffer;
	int error;

	switch (cmd) {
	case MTIOCTOP:
		if (((struct mtop *)addr)->mt_op != MTOFFL)
			return EIO;
		return (0);
	case DIOCGDINFO:
		bzero(&buffer, sizeof(buffer));

		buffer.d_secpercyl = fd->sc_type->seccyl;
		buffer.d_type = DTYPE_FLOPPY;
		buffer.d_secsize = FDC_BSIZE;

		if (readdisklabel(dev, fdstrategy, &buffer, NULL) != NULL)
			return EINVAL;

		*(struct disklabel *)addr = buffer;
		return 0;

	case DIOCWLABEL:
		if ((flag & FWRITE) == 0)
			return EBADF;
		/* XXX do something */
		return 0;

	case DIOCWDINFO:
		if ((flag & FWRITE) == 0)
			return EBADF;

		error = setdisklabel(&buffer, (struct disklabel *)addr, 0, NULL);
		if (error)
			return error;

		error = writedisklabel(dev, fdstrategy, &buffer, NULL);
		return error;

        case FD_FORM:
                if((flag & FWRITE) == 0)
                        return EBADF;  /* must be opened for writing */
                else if(((struct fd_formb *)addr)->format_version !=
                        FD_FORMAT_VERSION)
                        return EINVAL; /* wrong version of formatting prog */
                else
                        return fdformat(dev, (struct fd_formb *)addr, p);
                break;

        case FD_GTYPE:                  /* get drive type */
                *(struct fd_type *)addr = *fd->sc_type;
		return 0;

        case FD_GOPTS:                  /* get drive options */
                *(int *)addr = fd->sc_opts;
                return 0;
                
        case FD_SOPTS:                  /* set drive options */
                fd->sc_opts = *(int *)addr;
		return 0;

	default:
		return ENOTTY;
	}

#ifdef DIAGNOSTIC
	panic("fdioctl: impossible");
#endif
}

int
fdformat(dev, finfo, p)
        dev_t dev;
        struct fd_formb *finfo;
        struct proc *p;
{
        int rv = 0, s;
	struct fd_softc *fd = fd_cd.cd_devs[FDUNIT(dev)];
	struct fd_type *type = fd->sc_type;
        struct buf *bp;

        /* set up a buffer header for fdstrategy() */
        bp = (struct buf *)malloc(sizeof(struct buf), M_TEMP, M_NOWAIT);
        if(bp == 0)
                return ENOBUFS;
        bzero((void *)bp, sizeof(struct buf));
        bp->b_flags = B_BUSY | B_PHYS | B_FORMAT;
        bp->b_proc = p;
        bp->b_dev = dev;

        /*
         * calculate a fake blkno, so fdstrategy() would initiate a
         * seek to the requested cylinder
         */
        bp->b_blkno = (finfo->cyl * (type->sectrac * type->heads)
                + finfo->head * type->sectrac) * FDC_BSIZE / DEV_BSIZE;

        bp->b_bcount = sizeof(struct fd_idfield_data) * finfo->fd_formb_nsecs;
        bp->b_data = (caddr_t)finfo;
        
#ifdef DEBUG
	printf("fdformat: blkno %x count %x\n", bp->b_blkno, bp->b_bcount);
#endif

        /* now do the format */
        fdstrategy(bp);

        /* ...and wait for it to complete */
        s = splbio();
        while(!(bp->b_flags & B_DONE))
        {
                rv = tsleep((caddr_t)bp, PRIBIO, "fdform", 0);
                if(rv == EWOULDBLOCK)
		    /*break*/;
        }
        splx(s);
        
        if(rv == EWOULDBLOCK) {
                /* timed out */
                rv = EIO;
		/* XXX what to do to the buf? it will eventually fall
		   out as finished, but ... ?*/
		/*biodone(bp);*/
	}
        if(bp->b_flags & B_ERROR)
                rv = bp->b_error;
        free(bp, M_TEMP);
        return rv;
}

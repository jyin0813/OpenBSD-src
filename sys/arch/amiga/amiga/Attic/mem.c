/*	$OpenBSD: mem.c,v 1.19 2002/06/11 03:25:31 miod Exp $	*/
/*	$NetBSD: mem.c,v 1.18 1997/02/02 07:17:14 thorpej Exp $	*/

/*
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1982, 1986, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department.
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
 *	@(#)mem.c	8.3 (Berkeley) 1/12/94
 */

/*
 * Memory special file
 */

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/systm.h>
#include <sys/uio.h>
#include <sys/malloc.h>
#include <sys/proc.h>

#include <machine/conf.h>
#include <machine/cpu.h>

#include <uvm/uvm_extern.h>

extern int kernel_reload_write(struct uio *uio);
extern u_int lowram;
caddr_t devzeropage;

int mmopen(dev_t, int, int, struct proc *);
int mmclose(dev_t, int, int, struct proc *);
int mmrw(dev_t, struct uio *, int);
paddr_t mmmmap(dev_t, off_t, int);
int mmioctl(dev_t, u_long, caddr_t, int, struct proc *);

/*ARGSUSED*/
int
mmopen(dev, flag, mode, p)
	dev_t		dev;
	int		flag, mode;
	struct proc	*p;
{

	switch (minor(dev)) {
		case 0:
		case 1:
		case 2:
		case 12:
		case 20:
			return (0);
		default:
			return (ENXIO);
	}
}

/*ARGSUSED*/
int
mmclose(dev, flag, mode, p)
	dev_t		dev;
	int		flag, mode;
	struct proc	*p;
{

	return (0);
}

/*ARGSUSED*/
int
mmrw(dev, uio, flags)
	dev_t dev;
	struct uio *uio;
	int flags;
{
	register vm_offset_t o, v;
	register int c;
	register struct iovec *iov;
	int error = 0;
	static int physlock;

	if (minor(dev) == 0) {
		/* lock against other uses of shared vmmap */
		while (physlock > 0) {
			physlock++;
			error = tsleep((caddr_t)&physlock, PZERO | PCATCH,
			    "mmrw", 0);
			if (error)
				return (error);
		}
		physlock = 1;
	}
	while (uio->uio_resid > 0 && error == 0) {
		iov = uio->uio_iov;
		if (iov->iov_len == 0) {
			uio->uio_iov++;
			uio->uio_iovcnt--;
			if (uio->uio_iovcnt < 0)
				panic("mmrw");
			continue;
		}
		switch (minor(dev)) {

		/* minor device 0 is physical memory */
		case 0:
			v = uio->uio_offset;
#ifndef DEBUG
			/* allow reads only in RAM (except for DEBUG) */
			if (v >= 0xFFFFFFFC || v < lowram) {
				error = EFAULT;
				goto unlock;
			}
#endif
			pmap_enter(pmap_kernel(), (vm_offset_t)vmmap,
			    trunc_page(v), uio->uio_rw == UIO_READ ?
			    VM_PROT_READ : VM_PROT_WRITE, PMAP_WIRED);
			pmap_update(pmap_kernel());
			o = uio->uio_offset & PGOFSET;
			c = min(uio->uio_resid, (int)(NBPG - o));
			error = uiomove((caddr_t)vmmap + o, c, uio);
			pmap_remove(pmap_kernel(), (vm_offset_t)vmmap,
			    (vm_offset_t)vmmap + NBPG);
			pmap_update(pmap_kernel());
			continue;

		/* minor device 1 is kernel memory */
		case 1:
			v = uio->uio_offset;
			c = min(iov->iov_len, MAXPHYS);
			if (!uvm_kernacc((caddr_t)v, c,
			    uio->uio_rw == UIO_READ ? B_READ : B_WRITE))
				return (EFAULT);
			if (v < NBPG) {
#ifdef DEBUG
				/*
				 * For now, return zeros on read of page 0
				 * and EFAULT for writes.
				 */
				if (uio->uio_rw == UIO_READ) {
					if (devzeropage == NULL) {
						devzeropage = (caddr_t)
						    malloc(PAGE_SIZE, M_TEMP,
						    M_WAITOK);
						bzero(devzeropage, PAGE_SIZE);
					}
					c = min(c, NBPG - (int)v);
					v = (vm_offset_t)devzeropage;
				} else
#endif
					return (EFAULT);
			}
			error = uiomove((caddr_t)v, c, uio);
			continue;

		/* minor device 2 is EOF/RATHOLE */
		case 2:
			if (uio->uio_rw == UIO_WRITE)
				uio->uio_resid = 0;
			return (0);

		/*
		 * minor device 12 (/dev/zero) is source of nulls on read,
		 * rathole on write
		 */
		case 12:
			if (uio->uio_rw == UIO_WRITE) {
				c = iov->iov_len;
				break;
			}
			if (devzeropage == NULL) {
				devzeropage = (caddr_t)
				    malloc(PAGE_SIZE, M_TEMP, M_WAITOK);
				bzero(devzeropage, PAGE_SIZE);
			}
			c = min(iov->iov_len, PAGE_SIZE);
			error = uiomove(devzeropage, c, uio);
			continue;

		/*
		 * minor device 20 (/dev/reload) represents magic memory
		 * which you can write a kernel image to, causing a reboot
		 * into that kernel
		 */
		case 20:
			if (uio->uio_rw == UIO_READ)
				return 0;
			error = kernel_reload_write(uio);
			continue;

		default:
			return (ENXIO);
		}
		if (error)
			break;
		iov->iov_base += c;
		iov->iov_len -= c;
		uio->uio_offset += c;
		uio->uio_resid -= c;
	}
	if (minor(dev) == 0) {
#ifndef DEBUG
unlock:
#endif
		if (physlock > 1)
			wakeup((caddr_t)&physlock);
		physlock = 0;
	}
	return (error);
}

paddr_t
mmmmap(dev, off, prot)
	dev_t dev;
	off_t off;
	int prot;
{
	return (-1);
}

int
mmioctl(dev, cmd, data, flags, p)
	dev_t dev;
	u_long cmd;
	caddr_t data;
	int flags;
	struct proc *p;
{
	return (EOPNOTSUPP);
}

/*	$OpenBSD: subr_xxx.c,v 1.11 2010/09/24 13:21:30 matthew Exp $	*/
/*	$NetBSD: subr_xxx.c,v 1.10 1996/02/04 02:16:51 christos Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 *	@(#)subr_xxx.c	8.1 (Berkeley) 6/10/93
 */

/*
 * Miscellaneous trivial functions, including many
 * that are often inline-expanded or done in assembler.
 */
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/conf.h>

#include <machine/cpu.h>

/*
 * Unsupported device function (e.g. writing to read-only device).
 */
int
enodev(void)
{

	return (ENODEV);
}

/*
 * Unconfigured device function; driver not configured.
 */
int
enxio(void)
{

	return (ENXIO);
}

/*
 * Unsupported ioctl function.
 */
int
enoioctl(void)
{

	return (ENOTTY);
}

/*
 * Unsupported system function.
 * This is used for an otherwise-reasonable operation
 * that is not supported by the current system binary.
 */
int
enosys(void)
{

	return (ENOSYS);
}

/*
 * Return error for operation not supported
 * on a specific object or file type.
 */
/*ARGSUSED*/
int
eopnotsupp(void *v)
{

	return (EOPNOTSUPP);
}

/*
 * Generic null operation, always returns success.
 */
/*ARGSUSED*/
int
nullop(void *v)
{

	return (0);
}

struct bdevsw *
bdevsw_lookup(dev_t dev)
{
	return (&bdevsw[major(dev)]);
}

struct cdevsw *
cdevsw_lookup(dev_t dev)
{
	return (&cdevsw[major(dev)]);
}

/*
 * Convert a character device number to a block device number.
 */
dev_t
chrtoblk(dev_t dev)
{
	int blkmaj;

	if (major(dev) >= nchrdev || major(dev) >= nchrtoblktbl)
		return (NODEV);
	blkmaj = chrtoblktbl[major(dev)];
	if (blkmaj == NODEV)
		return (NODEV);
	return (makedev(blkmaj, minor(dev)));
}

/*
 * Convert a block device number to a character device number.
 */
dev_t
blktochr(dev_t dev)
{
	int blkmaj = major(dev);
	int i;

	if (blkmaj >= nblkdev)
		return (NODEV);
	for (i = 0; i < nchrtoblktbl; i++)
		if (blkmaj == chrtoblktbl[i])
			return (makedev(i, minor(dev)));
	return (NODEV);
}

/*
 * Check that we're in a context where it's okay to sleep.
 */
void
assertwaitok(void)
{
	splassert(IPL_NONE);
#ifdef DIAGNOSTIC
	if (curcpu()->ci_mutex_level != 0)
		panic("assertwaitok: non-zero mutex count: %d",
		    curcpu()->ci_mutex_level);
#endif
}

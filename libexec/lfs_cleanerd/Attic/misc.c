/*	$OpenBSD: misc.c,v 1.6 2003/06/11 14:24:46 deraadt Exp $	*/

/*-
 * Copyright (c) 1992, 1993
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
 */

#ifndef lint
/*static char sccsid[] = "@(#)misc.c	8.1 (Berkeley) 6/4/93";*/
static char rcsid[] = "$OpenBSD: misc.c,v 1.6 2003/06/11 14:24:46 deraadt Exp $";
#endif /* not lint */

#include <sys/types.h>

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

extern char *special;

void
err(const int fatal, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	(void)fprintf(stderr, "%s: ", special);
	(void)vfprintf(stderr, fmt, ap);
	va_end(ap);
	if (errno)
		(void)fprintf(stderr, " %s", strerror(errno));
	(void)fprintf(stderr, "\n");
	if (fatal)
		exit(1);
}

void
get(int fd, off_t off, void *p, size_t len)
{
	int rbytes;

	if (lseek(fd, off, SEEK_SET) < 0)
		err(1, "%s: %s", special, strerror(errno));
	if ((rbytes = read(fd, p, len)) < 0)
		err(1, "%s: %s", special, strerror(errno));
	if (rbytes != len)
		err(1, "%s: short read (%d, not %d)", special, rbytes, len);
}

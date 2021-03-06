/*
 * Copyright (c) 1988 Regents of the University of California.
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

#if defined(LIBC_SCCS) && !defined(lint)
static char *rcsid = "$OpenBSD: __strsignal.c,v 1.10 2005/05/01 19:39:02 tom Exp $";
#endif /* LIBC_SCCS and not lint */

#ifdef NLS
#define catclose	_catclose
#define catgets		_catgets
#define catopen		_catopen
#include <nl_types.h>
#endif

#define sys_siglist	_sys_siglist

#include <stdio.h>
#include <limits.h>
#include <signal.h>
#include <string.h>

static char *
itoa(char *buffer, size_t buffer_size, unsigned int num)
{
	char *p = buffer + buffer_size;

	*--p = '\0';
	while (num >= 10 && p > buffer + 1) {
		*--p = (num % 10) + '0';
		num /= 10;
	}
	/* num < 10 || p == buffer + 1 */
	*--p = (num % 10) + '0';
	return p;
}

char *
__strsignal(int num, char *buf)
{
#define	UPREFIX	"Unknown signal: "
	unsigned int signum;

#ifdef NLS
	nl_catd catd ;
	catd = catopen("libc", 0);
#endif

	signum = num;				/* convert to unsigned */
	if (signum < NSIG) {
#ifdef NLS
		strlcpy(buf, catgets(catd, 2, signum,
		    (char *)sys_siglist[signum]), NL_TEXTMAX);
#else
		return((char *)sys_siglist[signum]);
#endif
	} else {
#define MAXINTDIGS 11
		char str[MAXINTDIGS];

#ifdef NLS
		strlcpy(buf, catgets(catd, 1, 0xffff, UPREFIX), NL_TEXTMAX);
#else
		strlcpy(buf, UPREFIX, NL_TEXTMAX);
#endif
		strlcat(buf, itoa(str, sizeof(str), signum), NL_TEXTMAX);
	}

#ifdef NLS
	catclose(catd);
#endif

	return buf;
}

/*	$OpenBSD: sigaction.c,v 1.1 1998/07/23 21:20:03 millert Exp $	*/

/****************************************************************************
 * Copyright (c) 1998 Free Software Foundation, Inc.                        *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/

/****************************************************************************
 *  Author: Zeyd M. Ben-Halim <zmbenhal@netcom.com> 1992,1995               *
 *     and: Eric S. Raymond <esr@snark.thyrsus.com>                         *
 ****************************************************************************/

#include <curses.priv.h>

/* This file provides sigaction() emulation using sigvec() */
/* Use only if this is non POSIX system */

#if !HAVE_SIGACTION
#include <signal.h>
#include <SigAction.h>

MODULE_ID("$From: sigaction.c,v 1.7 1998/02/11 12:13:53 tom Exp $")

int
sigaction (int sig, sigaction_t * sigact, sigaction_t * osigact)
{
  return sigvec(sig, sigact, osigact);
}

int
sigemptyset (sigset_t * mask)
{
  *mask = 0;
  return 0;
}

int
sigprocmask (int mode, sigset_t * mask, sigset_t * omask)
   {
   sigset_t current = sigsetmask(0);

   if (omask) *omask = current;

   if (mode==SIG_BLOCK)
      current |= *mask;
   else if (mode==SIG_UNBLOCK)
      current &= ~*mask;
   else if (mode==SIG_SETMASK)
      current = *mask;

   sigsetmask(current);
   return 0;
   }

int
sigsuspend (sigset_t * mask)
{
  return sigpause (*mask);
}

int
sigdelset (sigset_t * mask, int sig)
{
  *mask &= ~sigmask (sig);
  return 0;
}

int
sigaddset (sigset_t * mask, int sig)
{
  *mask |= sigmask (sig);
  return 0;
}
#else
extern void _nc_sigaction(void);	/* quiet's gcc warning */
void _nc_sigaction(void) { } /* nonempty for strict ANSI compilers */
#endif

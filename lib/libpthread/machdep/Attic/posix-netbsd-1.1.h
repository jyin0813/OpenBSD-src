/* ==== posix.h ============================================================
 * Copyright (c) 1993 by Chris Provenzano, proven@athena.mit.edu	
 *
 * Description : Convert an Ultrix-4.2 system to a more or less POSIX system.
 *
 * posix-netbsd-1.0.h,v 1.53 1995/02/17 03:41:34 proven Exp
 *
 *  1.00 93/07/20 proven
 *      -Started coding this file.
 */

#ifndef _PTHREAD_POSIX_H_
#define _PTHREAD_POSIX_H_

#include <sys/cdefs.h>

/* More stuff for compiling */
#if defined(__GNUC__)
#define __INLINE                extern inline
#else
#define __INLINE                static
#endif

/* Make sure we have size_t defined */
#include <pthread/types.h>

#ifndef __WAIT_STATUS
#define __WAIT_STATUS	int *
#endif

#endif

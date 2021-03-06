/*	$OpenBSD: pthread_join.c,v 1.3 2003/07/31 21:48:05 deraadt Exp $	*/
/*
 * Copyright (c) 1993, 1994, 1995, 1996 by Chris Provenzano and contributors, 
 * proven@mit.edu All rights reserved.
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
 *	This product includes software developed by Chris Provenzano,
 *	the University of California, Berkeley, and contributors.
 * 4. Neither the name of Chris Provenzano, the University, nor the names of
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY CHRIS PROVENZANO AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL CHRIS PROVENZANO, THE REGENTS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */ 

/* ==== test_pthread_join.c =================================================
 * Copyright (c) 1993 by Chris Provenzano, proven@athena.mit.edu
 *
 * Description : Test pthread_join(). Run this after test_create()
 *
 *  1.23 94/05/04 proven
 *      -Started coding this file.
 */

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

/* This thread yields so the creator has a live thread to wait on */
static void *
new_thread_1(void * new_buf)
{
	int i;

	snprintf((char *)new_buf, 512, "New thread %%d stack at %p\n", &i);
	pthread_yield();	/* (ensure parent can wait on live thread) */
	sleep(1);
	return(new_buf);
	PANIC("return");
}

/* This thread doesn't yield so the creator has a dead thread to wait on */
static void *
new_thread_2(void * new_buf)
{
	int i;

	snprintf((char *)new_buf, 512, "New thread %%d stack at %p\n", &i);
	return(new_buf);
	PANIC("return");
}

int
main(int argc, char *argv[])
{
	char buf[256], *status;
	pthread_t thread;
	int debug = 1;
	int i = 0;

	if (debug)
		printf("Original thread stack at %p\n", &i);

	CHECKr(pthread_create(&thread, NULL, new_thread_1, (void *)buf));
	CHECKr(pthread_join(thread, (void **)(&status)));
	if (debug) 
		printf(status, ++i);

	/* Now have the created thread finishing before the join. */
	CHECKr(pthread_create(&thread, NULL, new_thread_2, (void *)buf));
	pthread_yield();
	sleep(1); /* (ensure thread is dead) */
	CHECKr(pthread_join(thread, (void **)(&status)));

	if (debug)
		printf(status, ++i);

	SUCCEED;
}


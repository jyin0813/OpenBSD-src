/*	$OpenBSD: libdl.c,v 1.2 2002/05/24 03:44:37 deraadt Exp $ */

/*
 * Copyright (c) 1998 Per Fogelstrom, Opsycon AB
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
 *	This product includes software developed under OpenBSD by
 *	Per Fogelstrom, Opsycon AB, Sweden.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 *	All functions here are just stubs that will be overridden
 *	by the real functions in ld.so when dynamic loading is
 *	performed at exec. The dl library is provided as a link
 *	helper so we can link a program using the dl functions
 *	without getting any unresolved references.
 */

void *dlopen(const char *libname, int how) __attribute__((weak));
void *dlsym(void *handle, const char *name) __attribute__((weak));
int dlctl(void *handle, int command, void *data) __attribute__((weak));
int dlclose(void *handle)__attribute__((weak));
const char * dlerror() __attribute__((weak));
#include <stdio.h>

void *
dlopen(const char *libname, int how)
{
	printf("ERROR! libdl incorrectly linked!\n");
	return NULL;
}

void *
dlsym(void *handle, const char *name)
{
	printf("ERROR! libdl incorrectly linked!\n");
	return NULL;
}

int
dlctl(void *handle, int command, void *data)
{
	printf("ERROR! libdl incorrectly linked!\n");
	return 0;
}

int
dlclose(void *handle)
{
	printf("ERROR! libdl incorrectly linked!\n");
	return 0;
}

const char *
dlerror()
{
	printf("ERROR! libdl incorrectly linked!\n");
	return "ERROR! libdl incorrectly linked!\n";
}

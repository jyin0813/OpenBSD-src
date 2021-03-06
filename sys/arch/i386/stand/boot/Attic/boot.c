/*	$OpenBSD: boot.c,v 1.16 1997/09/02 21:37:38 mickey Exp $	*/

/*
 * Copyright (c) 1997 Michael Shalayeff
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Michael Shalayeff.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/param.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <libsa.h>
#include "cmd.h"

const char *const kernels[] = {
	"/bsd",  "/bsd.gz",
	"/obsd", "/obsd.gz",
	NULL
};

extern	const char version[];
struct cmd_state cmd;

void
boot(bootdev)
	dev_t	bootdev;
{
	register const char *bootfile = kernels[0];
	register int i = 0, f;

	machdep();

	devboot(bootdev, cmd.bootdev);
	strncpy(cmd.image, bootfile, sizeof(cmd.image));
	cmd.boothowto = 0;
	cmd.conf = "/etc/boot.conf";
	cmd.addr = (void *)0x100000;
	cmd.timeout = 5;

	f = read_conf();

	printf(">> OpenBSD BOOT %s\n", version);

	while (1) {
		if (f <= 0) /* no boot.conf, or no boot cmd in there */
			do {
				printf("boot> ");
			} while(!getcmd());
		f = 0;

		printf("booting %s: ", cmd.path);
		exec(cmd.path, cmd.addr, cmd.boothowto);

		if (kernels[++i] == NULL)
			bootfile = kernels[i=0];
		else
			bootfile = kernels[i];

		cmd.timeout++;
		printf(" failed(%d). will try %s\n", errno, bootfile);
		strncpy(cmd.image, bootfile, sizeof(cmd.image));
	}
}

#ifdef _TEST
int
main()
{
	boot(0);
	return 0;
}
#endif

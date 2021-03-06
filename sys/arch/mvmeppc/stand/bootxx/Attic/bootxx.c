/*	$OpenBSD: bootxx.c,v 1.3 2011/03/13 00:13:53 deraadt Exp $ */

/*
 * Copyright (c) 1994 Paul Kranenburg
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
 *      This product includes software developed by Paul Kranenburg.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This is a generic "first-stage" boot program.
 *
 * Note that this program has absolutely no filesystem knowledge!
 *
 * Instead, this uses a table of disk block numbers that are
 * filled in by the installboot program such that this program
 * can load the "second-stage" boot program.
 */

#include <sys/param.h>
#include <sys/time.h>
#include <sys/exec.h>
#include <machine/prom.h>

#include "stand.h"
#include "libsa.h"

/*
 * Boot device is derived from ROM provided information.
 */

/* This determines the largest boot program we can load. */
#define MAXBLOCKNUM	64

/*
 * These three names are known by installboot.
 * The block_table contains starting block numbers,
 * in terms of 512-byte blocks.  Each non-zero value
 * will result in a read of block_size bytes.
 */
int     	block_size = 512;	/* default */
int     	block_count = MAXBLOCKNUM;	/* length of table */
daddr32_t 	block_table[MAXBLOCKNUM] = { 0 };

extern		char *version;


main()
{
	struct open_file	f;
	char	*addr;
	int n, error;

	bootdev_type = BUGDEV_DISK;

	printf("Boot: bug device: ctrl=%d, dev=%d\n", 
		bugargs.ctrl_lun, bugargs.dev_lun);
	printf("\nbootxx: first level bootstrap program [%s]\n\n", version);

	f.f_flags = F_RAW;
	if (devopen(&f, 0, &addr)) {
		printf("bootxx: open failed\n");
		_rtt();
	}

	addr = (char *)STAGE2_RELOC;
	error = copyboot(&f, addr);
	f.f_dev->dv_close(&f);
	if (!error) {
		bugexec((void (*)())addr);
	}
	/* copyboot had a problem... */
	_rtt();
}

int
copyboot(fp, addr)
	struct open_file	*fp;
	char			*addr;
{
	int	n, i, blknum;
	struct exec *x;

	addr -= sizeof(struct exec); /* assume OMAGIC, verify below */
	x = (struct exec *)addr;

	if (!block_count) {
		printf("bootxx: no data!?!\n");
		return -1;
	}

	for (i = 0; i < block_count; i++) {

		if ((blknum = block_table[i]) == 0)
			break;

#ifdef DEBUG
		printf("bootxx: read block # %d = %d\n", i, blknum);
#endif
		if ((fp->f_dev->dv_strategy)(fp->f_devdata, F_READ,
					   blknum, block_size, addr, &n))
		{
			printf("bootxx: read failed\n");
			return -1;
		}
		if (n != block_size) {
			printf("bootxx: short read\n");
			return -1;
		}
		addr += block_size;
	}

	if (N_GETMAGIC(*x) != OMAGIC) {
		printf("bootxx: secondary bootstrap isn't in OMAGIC format\n");
		return(-1);
	}

	return 0;
}

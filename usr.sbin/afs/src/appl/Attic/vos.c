/*	$OpenBSD: vos.c,v 1.2 1999/04/30 01:59:04 art Exp $	*/
/*
 * Copyright (c) 1995, 1996, 1997, 1998, 1999 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Kungliga Tekniska
 *      H�gskolan and its contributors.
 * 
 * 4. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "appl_locl.h"
#include <sl.h>
#include "vos_local.h"

RCSID("$KTH: vos.c,v 1.55 1999/03/06 14:38:38 lha Exp $");

int vos_interactive = 0;

static SL_cmd cmds[];

/*
 * Dummy command
 */

static int
empty_cmd(int argc, char **argv)
{
    printf("%s%s has not been implemented yet!\n", PROGNAME, argv[0]);
    return 0;
}

/*
 * quit
 */

static int
quit_cmd(int argc, char **argv)
{
    printf("exiting\n");
    return 1;
}

/*
 * help
 */

static int
help_cmd(int argc, char **argv)
{
    sl_help(cmds, argc, argv);
    return 0;
}

/*
 * apropos
 */

static int
apropos_cmd(int argc, char **argv)
{
    if (argc == 0) {
	fprintf (stderr, "apropos: missing topic");
	return 0;
    }

    sl_apropos(cmds, argv[1]);
    return 0;
}


/*
 * command table
 */

static SL_cmd cmds[] = {
    {"addsite",    empty_cmd,       "not yet implemented"},
    {"apropos",    apropos_cmd,     "apropos"},
    {"backup",     empty_cmd,       "not yet implemented"},
    {"backupsys",  empty_cmd,       "not yet implemented"},
    {"changeaddr", empty_cmd,       "not yet implemented"},
    {"create",     vos_create,      "create a volume"},
    {"createentry",vos_createentry, "create a vldb entry"},
    {"delentry",   empty_cmd,       "not yet implemented"},  
    {"dump",       vos_dump,        "dump a volume"},
    {"endtrans",   vos_endtrans,    "end a transaction"},
    {"examine",    vos_examine,     "print information about a volume"},
    {"volinfo"},
    {"help",       help_cmd,        "print help"},
    {"?"},
    {"listpart",   vos_listpart,    "list partitions on a server"},
    {"listvldb",   vos_listvldb,    "list volumes in volume-location-database"},
    {"listvol",    vos_listvol,     "list volumes on a server"},
    {"lock",       empty_cmd,       "not yet implemented"},
    {"move",       empty_cmd,       "not yet implemented"},
    {"partinfo",   vos_partinfo,    "print partition information on a server"},
    {"release",    empty_cmd,       "not yet implemented"},
    {"remove",     empty_cmd,       "not yet implemented"},
    {"remsite",    empty_cmd,       "not yet implemented"},
    {"rename",     empty_cmd,       "not yet implemented"},
    {"restore",    empty_cmd,       "not yet implemented"},
    {"status",     vos_status,      "Show volume server transactions"},
    {"syncserv",   empty_cmd,       "not yet implemented"},
    {"syncvldb",   empty_cmd,       "not yet implemented"},
    {"syncsite",   vos_syncsite,    "print the syncsite"},
    {"unlock",     empty_cmd,       "not yet implemented"},
    {"unlockvldb", empty_cmd,       "not yet implemented"},
    {"zap",        empty_cmd,       "not yet implemented"},
    {"quit",       quit_cmd,        "exit interactive mode"},
    {NULL}
};

/*
 * Main program
 */

int
main(int argc, char **argv)
{
    int ret = 0;
    
    cell_init(0);
    ports_init();

    if (argc > 1)
	ret = sl_command(cmds, argc - 1, argv + 1);
    else {
	vos_interactive = 1;
	printf("vos - an arla tool for administrating AFS volumes.\n");
	printf("Type \"help\" to get a list of commands.\n");
	ret = sl_loop(cmds, __progname": ");
    }
    return ret;
}

/*	$OpenBSD: dest_tkt.c,v 1.5 1998/02/25 15:51:06 art Exp $	*/
/* $KTH: dest_tkt.c,v 1.11 1997/05/19 03:03:40 assar Exp $ */

/*
 * This source code is no longer held under any constraint of USA
 * `cryptographic laws' since it was exported legally.  The cryptographic
 * functions were removed from the code and a "Bones" distribution was
 * made.  A Commodity Jurisdiction Request #012-94 was filed with the
 * USA State Department, who handed it to the Commerce department.  The
 * code was determined to fall under General License GTDA under ECCN 5D96G,
 * and hence exportable.  The cryptographic interfaces were re-added by Eric
 * Young, and then KTH proceeded to maintain the code in the free world.
 *
 */

/* 
 *  Copyright (C) 1989 by the Massachusetts Institute of Technology
 *
 *  Export of this software from the United States of America is assumed
 *  to require a specific license from the United States Government.
 *  It is the responsibility of any person or organization contemplating
 *  export to obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of M.I.T. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 */

#include "krb_locl.h"

/*
 * dest_tkt() is used to destroy the ticket store upon logout.
 * If the ticket file does not exist, dest_tkt() returns RET_TKFIL.
 * Otherwise the function returns RET_OK on success, KFAILURE on
 * failure.
 *
 * The ticket file (TKT_FILE) is defined in "krb.h".
 */

int
dest_tkt(void)
{
    char *file = TKT_FILE;
    int i,fd;
    struct stat statb;
    char buf[BUFSIZ];

    errno = 0;
    if (lstat(file, &statb) < 0)      
	goto out;

    if (!(statb.st_mode & S_IFREG)
#ifdef notdef
	|| statb.st_mode & 077
#endif
	)
	goto out;

    if ((fd = open(file, O_RDWR, 0)) < 0)
	goto out;

    memset(buf, 0, BUFSIZ);

    for (i = 0; i < statb.st_size; i += sizeof(buf))
	if (write(fd, buf, sizeof(buf)) != sizeof(buf)) {
	    fsync(fd);
	    close(fd);
	    goto out;
	}
	

    fsync(fd);
    close(fd);
    
    unlink(file);

out:
    if (errno == ENOENT) return RET_TKFIL;
    else if (errno != 0) return KFAILURE;
    return(KSUCCESS);
}

/*	$OpenBSD: kadm.h,v 1.5 1998/05/18 02:12:46 art Exp $	*/
/* $KTH: kadm.h,v 1.15 1998/04/23 22:17:52 joda Exp $ */

/*
 * This source code is no longer held under any constraint of USA
 * `cryptographic laws' since it was exported legally.  The cryptographic
 * functions were removed from the code and a "Bones" distribution was
 * made.  A Commodity Jurisdiction Request #012-94 was filed with the
 * USA State Department, who handed it to the Commerce department.  The
 * code was determined to fall under General License GTDA under ECCN 5D96G,
 * and hence exportable.  The cryptographic interfaces were re-added by Eric
 * Young, and then KTH proceeded to maintain the code in the free world.
 */

/*-
 * Copyright (C) 1989 by the Massachusetts Institute of Technology
 *
 * Export of this software from the United States of America is assumed
 * to require a specific license from the United States Government.
 * It is the responsibility of any person or organization contemplating
 * export to obtain such a license before exporting.
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

#ifndef KADM_DEFS
#define KADM_DEFS

/*
 * kadm.h
 * Header file for the fourth attempt at an admin server
 * Doug Church, December 28, 1989, MIT Project Athena
 */

#include <kerberosIV/krb_db.h>

/* The global structures for the client and server */
typedef struct {
  struct sockaddr_in admin_addr;
  struct sockaddr_in my_addr;
  int my_addr_len;
  int admin_fd;			/* file descriptor for link to admin server */
  char sname[ANAME_SZ];		/* the service name */
  char sinst[INST_SZ];		/* the services instance */
  char krbrlm[REALM_SZ];
} Kadm_Client;

typedef struct {		/* status of the server, i.e the parameters */
   int inter;			/* Space for command line flags */
   char *sysfile;		/* filename of server */
} admin_params;			/* Well... it's the admin's parameters */

/* Largest password length to be supported */
#define MAX_KPW_LEN	128
/* Minimum allowed password length */
#define MIN_KPW_LEN	6

/* Largest packet the admin server will ever allow itself to return */
#define KADM_RET_MAX 2048

/* That's right, versions are 8 byte strings */
#define KADM_VERSTR	"KADM0.0A"
#define KADM_ULOSE	"KYOULOSE"	/* sent back when server can't
					   decrypt client's msg */
#define KADM_VERSIZE strlen(KADM_VERSTR)

/* the lookups for the server instances */
#define PWSERV_NAME  "changepw"
#define KADM_SNAME   "kerberos_master"
#define KADM_PORT    751
#define KADM_SINST   "kerberos"

/* Attributes fields constants and macros */
#define ALLOC        2
#define RESERVED     3
#define DEALLOC      4
#define DEACTIVATED  5
#define ACTIVE       6

/* Kadm_vals structure for passing db fields into the server routines */
#define FLDSZ        4

typedef struct {
    u_int8_t       fields[FLDSZ];     /* The active fields in this struct */
    char           name[ANAME_SZ];
    char           instance[INST_SZ];
    u_int32_t  key_low;
    u_int32_t  key_high;
    u_int32_t  exp_date;
    u_int16_t attributes;
    u_int8_t  max_life;
} Kadm_vals;                    /* The basic values structure in Kadm */

/* Need to define fields types here */
#define KADM_NAME       31
#define KADM_INST       30
#define KADM_EXPDATE    29
#define KADM_ATTR       28
#define KADM_MAXLIFE    27
#define KADM_DESKEY     26

/* To set a field entry f in a fields structure d */
#define SET_FIELD(f,d)  (d[3-(f/8)]|=(1<<(f%8)))

/* To set a field entry f in a fields structure d */
#define CLEAR_FIELD(f,d)  (d[3-(f/8)]&=(~(1<<(f%8))))

/* Is field f in fields structure d */
#define IS_FIELD(f,d)   (d[3-(f/8)]&(1<<(f%8)))

/* Various return codes */
#define KADM_SUCCESS    0

#define WILDCARD_STR "*"

enum acl_types {
ADDACL,
GETACL,
MODACL,
STABACL, /* not used */
DELACL
};

/* Various opcodes for the admin server's functions */
#define CHANGE_PW    2
#define ADD_ENT      3
#define MOD_ENT      4
#define GET_ENT      5
#define CHECK_PW     6 /* not used */
#define CHG_STAB     7 /* not used */
#define DEL_ENT	     8

void prin_vals __P((Kadm_vals *));
int stv_long __P((u_char *, u_int32_t *, int, int));
int vts_long __P((u_int32_t, u_char **, int));
int vts_string __P((char *, u_char **, int));
int stv_string __P((u_char *, char *, int, int, int));

int stream_to_vals __P((u_char *, Kadm_vals *, int));
int vals_to_stream __P((Kadm_vals *, u_char **));

int kadm_init_link __P((char *, char *, char *));
int kadm_change_pw __P((unsigned char *));
int kadm_change_pw_plain __P((unsigned char *, char *, char**));
int kadm_change_pw2 __P((unsigned char *, char *, char**));
int kadm_mod __P((Kadm_vals *, Kadm_vals *));
int kadm_get __P((Kadm_vals *, u_char *));
int kadm_add __P((Kadm_vals *));
int kadm_del __P((Kadm_vals *));
void kadm_vals_to_prin __P((u_char *, Principal *, Kadm_vals *));
void kadm_prin_to_vals __P((u_char *, Kadm_vals *, Principal *));
int kadm_check_pw __P((const char*));

#endif /* KADM_DEFS */

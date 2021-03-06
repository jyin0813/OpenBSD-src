/*	$OpenBSD: extract.c,v 1.16 2003/07/04 17:31:19 avsm Exp $	*/

#ifndef lint
static const char rcsid[] = "$OpenBSD: extract.c,v 1.16 2003/07/04 17:31:19 avsm Exp $";
#endif

/*
 * FreeBSD install - a package for the installation and maintainance
 * of non-core utilities.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * Jordan K. Hubbard
 * 18 July 1993
 *
 * This is the package extraction code for the add module.
 *
 */

#include <err.h>
#include "lib.h"
#include "add.h"


#define STARTSTRING "tar cf - "
#define TOOBIG(str) ((strlen(str) + FILENAME_MAX + where_count > maxargs) \
		|| (strlen(str) + FILENAME_MAX + perm_count > maxargs))

#define PUSHOUT(todir) /* push out string */				\
        if (where_count > sizeof(STARTSTRING)-1) {			\
		    strlcat(where_args, "|tar xpf - -C ", maxargs);	\
		    strlcat(where_args, todir, maxargs);		\
		    if (system(where_args)) {				\
			cleanup(0);					\
			errx(2, "can not invoke %lu byte tar pipeline: %s", \
				(u_long)strlen(where_args), where_args); \
		    }							\
		    strlcpy(where_args, STARTSTRING, maxargs);		\
		    where_count = sizeof(STARTSTRING)-1;		\
	}								\
	if (perm_count) {						\
		    apply_perms(todir, perm_args);			\
		    perm_args[0] = 0;					\
		    perm_count = 0;					\
	}

static void
rollback(char *name, char *home, plist_t *start, plist_t *stop)
{
    plist_t *q;
    char try[FILENAME_MAX], bup[FILENAME_MAX], *dir;

    dir = home;
    for (q = start; q != stop; q = q->next) {
	if (q->type == PLIST_FILE) {
	    snprintf(try, sizeof(try), "%s/%s", dir, q->name);
	    if (make_preserve_name(bup, sizeof(bup), name, try) && fexists(bup)) {
		(void)chflags(try, 0);
		(void)unlink(try);
		if (rename(bup, try))
		    pwarnx("rollback: unable to rename %s back to %s", bup, try);
	    }
	}
	else if (q->type == PLIST_CWD) {
	    if (strcmp(q->name, "."))
		dir = q->name;
	    else
		dir = home;
	}
    }
}

static int 
preserve(const char *fname) 
{
    char copy[FILENAME_MAX];
    int i;

    for (i = 0; i < 50; i++) {
    	snprintf(copy, sizeof(copy), "%s-%d", fname, i);
	if (fexists(copy))
		continue;
	if (rename(fname, copy) == 0) {
		pwarnx("conflict: renamed existing %s to %s", fname, copy);
		return 0;
	}
    }
    return -1;
}

void
extract_plist(char *home, package_t *pkg)
{
    plist_t *p = pkg->head;
    char *last_file;
    char *where_args, *perm_args, *last_chdir;
    int maxargs, where_count = 0, perm_count = 0, add_count;

    maxargs = sysconf(_SC_ARG_MAX) / 2;	/* Just use half the argument space */
    where_args = alloca(maxargs);
    if (!where_args) {
	cleanup(0);
	errx(2, "can't get argument list space");
    }
    perm_args = alloca(maxargs);
    if (!perm_args) {
	cleanup(0);
	errx(2, "can't get argument list space");
    }
    strlcpy(where_args, STARTSTRING, maxargs);
    where_count = sizeof(STARTSTRING)-1;
    perm_args[0] = 0;

    last_chdir = 0;

    /* Reset the world */
    Owner = NULL;
    Group = NULL;
    Mode = NULL;
    last_file = NULL;
    Directory = home;

    /* Do it */
    while (p) {
	char cmd[FILENAME_MAX];

	switch(p->type) {
	case PLIST_NAME:
	    PkgName = p->name;
	    if (Verbose)
		printf("extract: Package name is %s\n", p->name);
	    break;

	case PLIST_FILE:
	    last_file = p->name;
	    if (Verbose)
		printf("extract: %s/%s\n", Directory, p->name);
	    if (!Fake) {
		char try[FILENAME_MAX];

		if (strrchr(p->name,'\'')) {
		  cleanup(0);
		  errx(2, "Bogus filename \"%s\"", p->name);
		}
		
		/* first try to rename it into place */
		snprintf(try, sizeof(try), "%s/%s", Directory, p->name);
		if (fexists(try)) {
		    if (preserve(try) == -1) {
		    	pwarnx("unable to back up %s, aborting pkg_add", try);
			rollback(PkgName, home, pkg->head, p);
			return;
		    }
		}
		if (rename(p->name, try) == 0) {
		    /* try to add to list of perms to be changed and run in bulk. */
		    if (p->name[0] == '/' || TOOBIG(p->name)) {
			PUSHOUT(Directory);
		    }
		    add_count = snprintf(&perm_args[perm_count], maxargs - perm_count, "'%s' ", p->name);
		    if (add_count > maxargs - perm_count) {
			cleanup(0);
			errx(2, "oops, miscounted strings!");
		    }
		    perm_count += add_count;
		} else {
		    /* rename failed, try copying with a big tar command */
		    if (last_chdir != Directory) {
			PUSHOUT(last_chdir);
			last_chdir = Directory;
		    }
		    else if (p->name[0] == '/' || TOOBIG(p->name)) {
			PUSHOUT(Directory);
		    }
		    add_count = snprintf(&where_args[where_count], maxargs - where_count, " '%s'", p->name);
		    if (add_count > maxargs - where_count) {
			cleanup(0);
			errx(2, "oops, miscounted strings!");
		    }
		    where_count += add_count;
		    add_count = snprintf(&perm_args[perm_count],
					 maxargs - perm_count,
					 "'%s' ", p->name);
		    if (add_count > maxargs - perm_count) {
			cleanup(0);
			errx(2, "oops, miscounted strings!");
		    }
		    perm_count += add_count;
		}
	    }
	    break;

	case PLIST_CWD:
	    if (Verbose)
		printf("extract: CWD to %s\n", p->name);
	    PUSHOUT(Directory);
	    if (strcmp(p->name, ".")) {
		if (!Fake && make_hierarchy(p->name) == FAIL) {
		    cleanup(0);
		    errx(2, "unable to make directory '%s'", p->name);
		}
		Directory = p->name;
	    } else
		Directory = home;
	    break;

	case PLIST_CMD:
	    if (!format_cmd(cmd, sizeof(cmd), p->name, Directory, last_file)) {
		cleanup(0);
		if (last_file == NULL)
		    errx(2, "no last file specified for '%s' command", p->name);
		else 
		    errx(2, "'%s' command could not expand", p->name);
	    }
	    
	    PUSHOUT(Directory);
	    if (Verbose)
		printf("extract: execute '%s'\n", cmd);
	    if (!Fake && system(cmd))
		pwarnx("command '%s' failed", cmd);
	    break;

	case PLIST_CHMOD:
	    PUSHOUT(Directory);
	    Mode = p->name;
	    break;

	case PLIST_CHOWN:
	    PUSHOUT(Directory);
	    Owner = p->name;
	    break;

	case PLIST_CHGRP:
	    PUSHOUT(Directory);
	    Group = p->name;
	    break;

	case PLIST_COMMENT:
	    break;

	case PLIST_IGNORE:
	    p = p->next;
	    break;

	default:
	    break;
	}
	p = p->next;
    }
    PUSHOUT(Directory);
}

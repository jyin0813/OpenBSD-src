/*	$OpenBSD: perform.c,v 1.17 2003/08/27 06:51:26 jolan Exp $	*/

#ifndef lint
static const char rcsid[] = "$OpenBSD: perform.c,v 1.17 2003/08/27 06:51:26 jolan Exp $";
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
 * This is the main body of the create module.
 *
 */

#include "lib.h"
#include "create.h"

#include <sys/syslimits.h>
#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

static void sanity_check(void);
static void make_dist(char *, char *, char *, package_t *);

static char *home;

int
pkg_perform(char **pkgs)
{
    char *pkg = *pkgs;		/* Only one arg to create */
    char *cp;
    FILE *pkg_in, *fp;
    package_t plist;
    char *suffix;  /* What we tack on to the end of the finished package */

    /* Preliminary setup */
    sanity_check();
    if (Verbose && !PlistOnly)
	printf("Creating package %s\n", pkg);
    get_dash_string(&Comment);
    if (strlen(Comment) > MAXINDEXSIZE && !PlistOnly) {
	errx(2,"Comment string is over %d characters long:\n%s",
	   MAXINDEXSIZE, Comment);
    }
    get_dash_string(&Desc);
    if (!strcmp(Contents, "-"))
	pkg_in = stdin;
    else {
	pkg_in = fopen(Contents, "r");
	if (!pkg_in) {
	    cleanup(0);
	    errx(2, "unable to open contents file '%s' for input", Contents);
	}
    }
    plist.head = plist.tail = NULL;

    /* Break the package name into base and desired suffix (if any) */
    if ((cp = strrchr(pkg, '.')) != NULL) {
	suffix = cp + 1;
	*cp = '\0';
    }
    else
	suffix = "tgz";

    if (Prefix)
	add_plist(&plist, PLIST_CWD, Prefix);
    /* If a SrcDir override is set, add it now */
    if (SrcDir) {
	if (Verbose && !PlistOnly)
	    printf("Using SrcDir value of %s\n", SrcDir);
	add_plist(&plist, PLIST_SRC, SrcDir);
    }

    /* Stick the dependencies, if any, at the top */
    if (Pkgdeps) {
	if (Verbose && !PlistOnly)
	    printf("Registering depends:");
	while (Pkgdeps) {
	    cp = strsep(&Pkgdeps, " \t\n");
	    if (*cp) {
		add_plist(&plist, PLIST_PKGDEP, cp);
		if (Verbose && !PlistOnly)
		    printf(" %s", cp);
	    }
	}
	if (Verbose && !PlistOnly)
	    printf(".\n");
    }

    /* Put the conflicts directly after the dependencies, if any */
    if (Pkgcfl) {
	if (Verbose && !PlistOnly)
	    printf("Registering conflicts:");
	while (Pkgcfl) {
	   cp = strsep(&Pkgcfl, " \t\n");
	   if (*cp) {
		add_plist(&plist, PLIST_PKGCFL, cp);
		if (Verbose && !PlistOnly)
		    printf(" %s", cp);
	   }
	}
	if (Verbose && !PlistOnly)
	    printf(".\n");
    }

    /* Slurp in the packing list */
    read_plist(&plist, pkg_in);

    /*
     * Run down the list and see if we've named it, if not stick in a name
     * at the top.
     */
    if (find_plist(&plist, PLIST_NAME) == NULL)
	add_plist_top(&plist, PLIST_NAME, basename_of(pkg));

    /*
     * We're just here for to dump out a revised plist for the FreeBSD ports
     * hack.  It's not a real create in progress.
     */
    if (PlistOnly) {
	check_list(home, &plist);
	write_plist(&plist, stdout);
	exit(0);
    }

    /* Make a directory to stomp around in */
    home = make_playpen(PlayPen, PlayPenSize, 0);
    signal(SIGINT, cleanup);
    signal(SIGHUP, cleanup);

    /* Make first "real contents" pass over it */
    check_list(home, &plist);
    (void) umask(022);	/* make sure gen'ed directories, files don't have
			   group or other write bits. */
    /* copy_plist(home, &plist); */
    /* mark_plist(&plist); */

    /* Now put the release specific items in */
    add_plist(&plist, PLIST_CWD, ".");
    write_file(COMMENT_FNAME, Comment);
    add_plist(&plist, PLIST_IGNORE, NULL);
    add_plist(&plist, PLIST_FILE, COMMENT_FNAME);
    write_file(DESC_FNAME, Desc);
    add_plist(&plist, PLIST_IGNORE, NULL);
    add_plist(&plist, PLIST_FILE, DESC_FNAME);

    if (Install) {
	copy_file(home, Install, INSTALL_FNAME);
	add_plist(&plist, PLIST_IGNORE, NULL);
	add_plist(&plist, PLIST_FILE, INSTALL_FNAME);
    }
    if (DeInstall) {
	copy_file(home, DeInstall, DEINSTALL_FNAME);
	add_plist(&plist, PLIST_IGNORE, NULL);
	add_plist(&plist, PLIST_FILE, DEINSTALL_FNAME);
    }
    if (Require) {
	copy_file(home, Require, REQUIRE_FNAME);
	add_plist(&plist, PLIST_IGNORE, NULL);
	add_plist(&plist, PLIST_FILE, REQUIRE_FNAME);
    }
    if (Display) {
	copy_file(home, Display, DISPLAY_FNAME);
	add_plist(&plist, PLIST_IGNORE, NULL);
	add_plist(&plist, PLIST_FILE, DISPLAY_FNAME);
	add_plist(&plist, PLIST_DISPLAY, DISPLAY_FNAME);
    }
    if (Mtree) {
	copy_file(home, Mtree, MTREE_FNAME);
	add_plist(&plist, PLIST_IGNORE, NULL);
	add_plist(&plist, PLIST_FILE, MTREE_FNAME);
	add_plist(&plist, PLIST_MTREE, MTREE_FNAME);
    }

    /* Finally, write out the packing list */
    fp = fopen(CONTENTS_FNAME, "w");
    if (!fp) {
	cleanup(0);
	errx(2, "can't open file %s for writing", CONTENTS_FNAME);
    }
    write_plist(&plist, fp);
    if (fclose(fp)) {
	cleanup(0);
	errx(2, "error while closing %s", CONTENTS_FNAME);
    }

    /* And stick it into a tar ball */
    make_dist(home, pkg, suffix, &plist);

    /* Cleanup */
    free(Comment);
    free(Desc);
    free_plist(&plist);
    leave_playpen(home);
    return TRUE;	/* Success */
}

static void
make_dist(char *home, char *pkg, char *suffix, package_t *plist)
{
    char tball[FILENAME_MAX];
    plist_t *p;
    int ret;
#define DIST_MAX_ARGS 4096
    char *args[DIST_MAX_ARGS];	
    char *tempfile[DIST_MAX_ARGS/2];
    int current = 0;
    FILE *flist = 0;
    int nargs = 0;
    int i;
    pid_t pid;

    args[nargs++] = "tar";	/* argv[0] */

    if (*pkg == '/')
	snprintf(tball, FILENAME_MAX, "%s.%s", pkg, suffix);
    else
	snprintf(tball, FILENAME_MAX, "%s/%s.%s", home, pkg, suffix);

    args[nargs++] = "-c";
    args[nargs++] = "-f";
    args[nargs++] = tball;
    if (strchr(suffix, 'z'))	/* Compress/gzip? */
	args[nargs++] = "-z";
    if (Dereference)
	args[nargs++] = "-h";
    if (ExcludeFrom) {
      /* XXX this won't work until someone adds the gtar -X option
	 (--exclude-from-file) to paxtar - so long it is disabled
	 here and a warning is printed in main.c
	args[nargs++] = "-X";
	args[nargs++] = ExcludeFrom;
	*/
    }

    if (Verbose) {
        if (strchr(suffix, 'z'))
	    printf("Creating gzip'd tar ball in '%s'\n", tball);
        else
	    printf("Creating tar ball in '%s'\n", tball);
    }
    args[nargs++] = CONTENTS_FNAME;
    args[nargs++] = COMMENT_FNAME;
    args[nargs++] = DESC_FNAME;
    if (Install)
        args[nargs++] = INSTALL_FNAME;
    if (DeInstall)
	args[nargs++] = DEINSTALL_FNAME;
    if (Require)
	args[nargs++] = REQUIRE_FNAME;
    if (Display)
	args[nargs++] = DISPLAY_FNAME;
    if (Mtree)
	args[nargs++] = MTREE_FNAME;

    for (p = plist->head; p; p = p->next) {
	if (nargs > (DIST_MAX_ARGS - 2))
	    errx(2, "too many args for tar command");
	if (p->type == PLIST_FILE) {
	    if (!flist) {
	    	int fd;
	    	tempfile[current] = strdup("/tmp/tpkg.XXXXXXXXXX");
		if ((fd = mkstemp(tempfile[current])) == -1)
		    errx(2, "can't make temp file");
		if (! (flist = fdopen(fd, "w")))
		    errx(2, "can't write to temp file");
		if (strcmp(args[nargs], "-C") == 0)
		    nargs+= 2;
		args[nargs++] = "-I";
		args[nargs++] = tempfile[current++];
	    }
	    fprintf(flist, "%s\n", p->name);
	}
	else if (p->type == PLIST_CWD || p->type == PLIST_SRC) {
	    /* XXX let PLIST_SRC override PLIST_CWD */
	    if (p->type == PLIST_CWD && p->next != NULL && 
	    	p->next->type == PLIST_SRC)
		    continue;
	    if (flist) 
		fclose(flist);
	    flist = 0;
	    args[nargs] = "-C";
	    if (BaseDir) {
		    size_t size = strlen(BaseDir)+2+strlen(p->name);
		    args[nargs+1] = malloc(size);
		    if (args[nargs+1] == NULL) {
			    cleanup(0);
			    errx(2, "can't get Cwd space");
		    }
		    snprintf(args[nargs+1], size, "%s/%s", BaseDir, p->name);
	    } else
		    args[nargs+1] = p->name;
	}
	else if (p->type == PLIST_IGNORE)
	     p = p->next;
    }
    if (flist) 
    	fclose(flist);
    args[nargs] = NULL;

    /* fork/exec tar to create the package */

    if (Verbose) {
    	printf("Running \"");
    	for (i = 0; i < nargs; i++) {
		printf("%s ", args[i]);
	}
	printf("\"\n");
    }
    pid = fork();
    if ( pid < 0 )
	err(2, "failed to fork");
    else if ( pid == 0 ) {
	execv("/bin/tar", args);
	for (i = 0; i < current; i++)
	    unlink(tempfile[i]);
	err(2, "failed to execute tar command");
    }
    wait(&ret);
    for (i = 0; i < current; i++)
	unlink(tempfile[i]);
    if (BaseDir) {
    	for (i = 0; i < nargs-1; i++) {
	    if (!strcmp(args[i], "-C"))
		free(args[++i]);
	}
    }
    /* assume either signal or bad exit is enough for us */
    if (ret) {
	cleanup(0);
	errx(2, "tar command failed with code %d", ret);
    }
}

static void
sanity_check()
{
    if (!Comment) {
	cleanup(0);
	errx(2, "required package comment string is missing (-c comment)");
    }
    if (!Desc) {
	cleanup(0);
	errx(2, "required package description string is missing (-d desc)");
    }
    if (!Contents) {
	cleanup(0);
	errx(2, "required package contents list is missing (-f [-]file)");
    }
}


/* Clean up those things that would otherwise hang around */
void
cleanup(int sig)
{
    int save_errno = errno;
    static int	alreadyCleaning;
    void (*oldint)(int);
    void (*oldhup)(int);
    char buf[1024];
    oldint = signal(SIGINT, SIG_IGN);
    oldhup = signal(SIGHUP, SIG_IGN);

    if (!alreadyCleaning) {
    	alreadyCleaning = 1;
	if (sig) {
	    snprintf(buf, sizeof buf, "Signal %d received, cleaning up.\n", sig);
	    write(STDOUT_FILENO, buf, strlen(buf));
	}
	leave_playpen(home);
	if (sig)
	    _exit(1);
    }
    signal(SIGINT, oldint);
    signal(SIGHUP, oldhup);
    errno = save_errno;
}

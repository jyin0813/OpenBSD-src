/*	$OpenBSD: perform.c,v 1.16 2003/08/21 20:24:56 espie Exp $	*/

#ifndef lint
static const char rcsid[] = "$OpenBSD: perform.c,v 1.16 2003/08/21 20:24:56 espie Exp $";
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
 * This is the main body of the delete module.
 *
 */

#include <sys/param.h>
#include <sys/mount.h>
#include <err.h>
#include "lib.h"
#include "delete.h"

static int pkg_do(char *);
static void sanity_check(char *);
static int undepend(const char *, char *, int);
static char LogDir[FILENAME_MAX];
extern Boolean CleanConf;


int
pkg_perform(char **pkgs)
{
    int i, err_cnt = 0;

    for (i = 0; pkgs[i]; i++)
	err_cnt += pkg_do(pkgs[i]);
    return err_cnt;
}

static package_t Plist;

static int
trim_end(char *name)
{
   size_t n, m;
   n = strlen(name);
   m = strlen(".tgz");
   if (n > m && strcmp(name+n-m, ".tgz") == 0) {
	name[n-m] = 0;
	return 1;
   }
   m = strlen(".tar.gz");
   if (n > m && strcmp(name+n-m, ".tar.gz") == 0) {
	name[n-m] = 0;
	return 1;
   }
   m = strlen(".tar");
   if (n > m && strcmp(name+n-m, ".tar") == 0) {
	name[n-m] = 0;
	return 1;
   }
   return 0;
}

/* remove all links to the package as well */
static void
delete_pkg_links(const char *dir, const char *pkg)
{
    int base;
    int len;
    DIR *d;
    struct dirent *dp;
    struct stat sb;
    char name[FILENAME_MAX+1];

    base = open(".", O_RDONLY);
    if (base == -1)
    	return;
    if (chdir(dir) == -1) {
    	close(base);
	return;
    }
    d = opendir(".");
    if (d == NULL) {
    	fchdir(base);
	close(base);
	return;
    }
    while ((dp = readdir(d)) != NULL) {
    	if (lstat(dp->d_name, &sb) == -1 || !S_ISLNK(sb.st_mode))
	    continue;
	len = readlink(dp->d_name, name, FILENAME_MAX);
	if (len == -1)
	    continue;
	name[len] = 0;
	if (strcmp(name, pkg))
	    continue;
	unlink(dp->d_name);
    }
    closedir(d);
    fchdir(base);
    close(base);
}

/* This is seriously ugly code following.  Written very fast! */
static int
pkg_do(char *pkg)
{
    FILE *cfile;
    char home[FILENAME_MAX];
    plist_t *p;
    char *dbdir;

    set_pkg(pkg);
    /* Reset some state */
    if (Plist.head)
	free_plist(&Plist);

    dbdir = getenv(PKG_DBDIR);
    if (!dbdir)
	dbdir = DEF_LOG_DIR;
try_again:
    (void) snprintf(LogDir, sizeof(LogDir), "%s/%s", dbdir, pkg);
    if (!fexists(LogDir)) {
	if (trim_end(pkg))
	    goto try_again;
	else {
	    pwarnx("no such package installed");
	    return 1;
	}
    }
    if (!getcwd(home, FILENAME_MAX)) {
	cleanup(0);
	errx(2, "unable to get current working directory!");
    }
    if (chdir(LogDir) == FAIL) {
	pwarnx("unable to change directory to %s! deinstall failed", LogDir);
	return 1;
    }
    if (!isemptyfile(REQUIRED_BY_FNAME)) {
	char buf[512];
	pwarnx("package `%s' is required by these other packages\n"
		"and may not be deinstalled%s:",
		pkg, Force ? " (but I'll delete it anyway)" : "" );
	cfile = fopen(REQUIRED_BY_FNAME, "r");
	if (cfile) {
	    while (fgets(buf, sizeof(buf), cfile))
		fprintf(stderr, "%s", buf);
	    fclose(cfile);
	} else
	    pwarnx("cannot open requirements file `%s'", REQUIRED_BY_FNAME);
	if (!Force)
	    return 1;
    }
    sanity_check(LogDir);
    cfile = fopen(CONTENTS_FNAME, "r");
    if (!cfile) {
	pwarnx("unable to open '%s' file", CONTENTS_FNAME);
	return 1;
    }
    /* If we have a prefix, add it now */
    if (Prefix)
	add_plist(&Plist, PLIST_CWD, Prefix);
    read_plist(&Plist, cfile);
    fclose(cfile);
    p = find_plist(&Plist, PLIST_CWD);
    if (!p) {
	pwarnx("package '%s' doesn't have a prefix", pkg);
	return 1;
    }
    {
	struct statfs buffer;

	if (statfs(p->name, &buffer) == -1) {
	    pwarnx("package '%s' prefix (%s) does not exist", pkg, p->name);
	    return 1;
	}
	if (buffer.f_flags & MNT_RDONLY) {
	    pwarnx("package'%s' mount point %s is read-only", pkg,
		buffer.f_mntonname);
	    return 1;
	}
    }

    setenv(PKG_PREFIX_VNAME, p->name, 1);
    setenv("PKG_DELETE_EXTRA", (CleanConf ? "Yes" : "No"), 1);
    if (fexists(REQUIRE_FNAME)) {
	if (Verbose)
	    printf("Executing 'require' script.\n");
	vsystem("chmod +x %s", REQUIRE_FNAME);	/* be sure */
	if (vsystem("./%s %s DEINSTALL", REQUIRE_FNAME, pkg)) {
	    pwarnx("package %s fails requirements %s", pkg,
		   Force ? "" : "- not deleted");
	    if (!Force)
		return 1;
	}
    }
    if (!NoDeInstall && fexists(DEINSTALL_FNAME)) {
	if (Fake)
	    printf("Would execute de-install script at this point.\n");
	else {
	    vsystem("chmod +x %s", DEINSTALL_FNAME);	/* make sure */
	    if (vsystem("./%s %s DEINSTALL", DEINSTALL_FNAME, pkg)) {
		pwarnx("deinstall script returned error status");
		if (!Force)
		    return 1;
	    }
	}
    }
    if (chdir(home) == FAIL) {
	cleanup(0);
	errx(2, "Toto! This doesn't look like Kansas anymore!");
    }
    if (!Fake) {
	/* Some packages aren't packed right, so we need to just ignore delete_package()'s status.  Ugh! :-( */
	if (delete_package(FALSE, CleanDirs, CleanConf, CheckMD5, &Plist) == FAIL)
	    pwarnx(
	"couldn't entirely delete package (perhaps the packing list is\n"
	"incorrectly specified?)");
	if (vsystem("%s -r %s", REMOVE_CMD, LogDir)) {
	    pwarnx("couldn't remove log entry in %s, deinstall failed", LogDir);
	    if (!Force)
		return 1;
	}
	delete_pkg_links(dbdir, pkg);
    }
    for (p = Plist.head; p ; p = p->next) {
	if (p->type != PLIST_PKGDEP)
	    continue;
	if (Verbose)
	    printf("Attempting to remove dependency on package `%s'\n", p->name);
	if (!Fake)
	    findmatchingname(dbdir, p->name, undepend, pkg, 0);
    }
    return 0;
}

static void
sanity_check(char *pkg)
{
    if (!fexists(CONTENTS_FNAME)) {
	cleanup(0);
	errx(2, "installed package %s has no %s file!", pkg, CONTENTS_FNAME);
    }
}

void
cleanup(int sig)
{
    exit(1);
}

/* deppkgname is the pkg from which's +REQUIRED_BY file we are
 * about to remove pkg2delname. This function is called from
 * findmatchingname(), deppkgname is expanded from a (possible) pattern.
 */
int
undepend(const char *deppkgname, char *pkg2delname, int unused)
{
     char fname[FILENAME_MAX], ftmp[FILENAME_MAX];
     char fbuf[FILENAME_MAX];
     FILE *fp, *fpwr;
     char *tmp;
     int s;

     (void) snprintf(fname, sizeof(fname), "%s/%s/%s",
	     (tmp = getenv(PKG_DBDIR)) ? tmp : DEF_LOG_DIR,
	     deppkgname, REQUIRED_BY_FNAME);
     fp = fopen(fname, "r");
     if (fp == NULL) {
	 pwarnx("couldn't open dependency file `%s'", fname);
	 return 0;
     }
     (void) snprintf(ftmp, sizeof(ftmp), "%s.XXXXXXXXXX", fname);
     s = mkstemp(ftmp);
     if (s == -1) {
	 fclose(fp);
	 pwarnx("couldn't open temp file `%s'", ftmp);
	 return 0;
     }
     fpwr = fdopen(s, "w");
     if (fpwr == NULL) {
	 close(s);
	 fclose(fp);
	 pwarnx("couldn't fdopen temp file `%s'", ftmp);
	 remove(ftmp);
	 return 0;
     }
     while (fgets(fbuf, sizeof(fbuf), fp) != NULL) {
	 if (fbuf[strlen(fbuf)-1] == '\n')
	     fbuf[strlen(fbuf)-1] = '\0';
	 if (strcmp(fbuf, pkg2delname))		/* no match */
	     fputs(fbuf, fpwr), putc('\n', fpwr);
     }
     (void) fclose(fp);
     if (fchmod(s, 0644) == FAIL) {
	 pwarnx("error changing permission of temp file `%s'", ftmp);
	 fclose(fpwr);
	 remove(ftmp);
	 return 0;
     }
     if (fclose(fpwr) == EOF) {
	 pwarnx("error closing temp file `%s'", ftmp);
	 remove(ftmp);
	 return 0;
     }
     if (rename(ftmp, fname) == -1)
	 pwarnx("error renaming `%s' to `%s'", ftmp, fname);
     remove(ftmp);			/* just in case */
     
     return 0;
}

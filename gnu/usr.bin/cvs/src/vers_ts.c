/*
 * Copyright (c) 1992, Brian Berliner and Jeff Polk
 * Copyright (c) 1989-1992, Brian Berliner
 * 
 * You may distribute under the terms of the GNU General Public License as
 * specified in the README file that comes with the CVS 1.4 kit.
 */

#include "cvs.h"

#ifndef lint
static const char rcsid[] = "$CVSid: @(#)vers_ts.c 1.45 94/10/07 $";
USE(rcsid);
#endif

#define ctime(X)	do not use ctime, please

#ifdef SERVER_SUPPORT
static void time_stamp_server PROTO((char *, Vers_TS *));
#endif

/*
 * Fill in and return a Vers_TS structure "user" is the name of the local
 * file; entries is the entries file - preparsed for our pleasure. xfiles is
 * all source code control files, preparsed for our pleasure
 */
Vers_TS *
Version_TS (repository, options, tag, date, user, force_tag_match,
	    set_time, entries, xfiles)
    char *repository;
    char *options;
    char *tag;
    char *date;
    char *user;
    int force_tag_match;
    int set_time;
    List *entries;
    List *xfiles;
{
    Node *p;
    RCSNode *rcsdata;
    Vers_TS *vers_ts;
    struct stickydirtag *sdtp;

    /* get a new Vers_TS struct */
    vers_ts = (Vers_TS *) xmalloc (sizeof (Vers_TS));
    memset ((char *) vers_ts, 0, sizeof (*vers_ts));

    /*
     * look up the entries file entry and fill in the version and timestamp
     * if entries is NULL, there is no entries file so don't bother trying to
     * look it up (used by checkout -P)
     */
    if (entries == NULL)
    {
	sdtp = NULL;
	p = NULL;
    }
    else
    {
	p = findnode (entries, user);
	sdtp = (struct stickydirtag *) entries->list->data; /* list-private */
    }

    if (p != NULL)
    {
	Entnode *entdata = (Entnode *) p->data;

	vers_ts->vn_user = xstrdup (entdata->version);
	vers_ts->ts_rcs = xstrdup (entdata->timestamp);
	vers_ts->ts_conflict = xstrdup (entdata->conflict);
	if (!tag)
	{
	    if (!(sdtp && sdtp->aflag))
		vers_ts->tag = xstrdup (entdata->tag);
	}
	if (!date)
	{
	    if (!(sdtp && sdtp->aflag))
		vers_ts->date = xstrdup (entdata->date);
	}
	if (!options || (options && *options == '\0'))
	{
	    if (!(sdtp && sdtp->aflag))
		vers_ts->options = xstrdup (entdata->options);
	}
	vers_ts->entdata = entdata;
    }

    /*
     * -k options specified on the command line override (and overwrite)
     * options stored in the entries file
     */
    if (options)
	vers_ts->options = xstrdup (options);
    else if (sdtp && sdtp->aflag == 0)
    {
	if (!vers_ts->options)
	    vers_ts->options = xstrdup (sdtp->options);
    }
    if (!vers_ts->options)
	vers_ts->options = xstrdup ("");

    /*
     * if tags were specified on the command line, they override what is in
     * the Entries file
     */
    if (tag || date)
    {
	vers_ts->tag = xstrdup (tag);
	vers_ts->date = xstrdup (date);
    }
    else if (!vers_ts->entdata && (sdtp && sdtp->aflag == 0))
    {
	if (!vers_ts->tag)
	    vers_ts->tag = xstrdup (sdtp->tag);
	if (!vers_ts->date)
	    vers_ts->date = xstrdup (sdtp->date);
    }

    /* Now look up the info on the source controlled file */
    if (xfiles != (List *) NULL)
    {
	p = findnode (xfiles, user);
	if (p != NULL)
	{
	    rcsdata = (RCSNode *) p->data;
	    rcsdata->refcount++;
	}
	else
	    rcsdata = NULL;
    }
    else if (repository != NULL)
	rcsdata = RCS_parse (user, repository);
    else
	rcsdata = NULL;

    if (rcsdata != NULL)
    {
	/* squirrel away the rcsdata pointer for others */
	vers_ts->srcfile = rcsdata;

#ifndef DEATH_SUPPORT
	/* (is this indeed death support?  I haven't looked carefully).  */
	/* get RCS version number into vn_rcs (if appropriate) */
	if (((vers_ts->tag || vers_ts->date) && force_tag_match) ||
	    ((rcsdata->flags & VALID) && (rcsdata->flags & INATTIC) == 0))
	{
#endif
	    if (vers_ts->tag && strcmp (vers_ts->tag, TAG_BASE) == 0)
		vers_ts->vn_rcs = xstrdup (vers_ts->vn_user);
	    else
		vers_ts->vn_rcs = RCS_getversion (rcsdata, vers_ts->tag,
					    vers_ts->date, force_tag_match);
#ifndef DEATH_SUPPORT
	} 
#endif

	/*
	 * If the source control file exists and has the requested revision,
	 * get the Date the revision was checked in.  If "user" exists, set
	 * its mtime.
	 */
	if (set_time)
	{
	    struct utimbuf t;

	    memset ((char *) &t, 0, sizeof (t));
	    if (vers_ts->vn_rcs &&
		(t.actime = t.modtime = RCS_getrevtime (rcsdata,
		 vers_ts->vn_rcs, (char *) 0, 0)) != -1)
		(void) utime (user, &t);
	}
    }

    /* get user file time-stamp in ts_user */
    if (entries != (List *) NULL)
    {
#ifdef SERVER_SUPPORT
	if (server_active)
	    time_stamp_server (user, vers_ts);
	else
#endif
	    vers_ts->ts_user = time_stamp (user);
    }

    return (vers_ts);
}

#ifdef SERVER_SUPPORT

/* Set VERS_TS->TS_USER to time stamp for FILE.  */

/* Separate these out to keep the logic below clearer.  */
#define mark_lost(V)		((V)->ts_user = 0)
#define mark_unchanged(V)	((V)->ts_user = xstrdup ((V)->ts_rcs))

static void
time_stamp_server (file, vers_ts)
    char *file;
    Vers_TS *vers_ts;
{
    struct stat sb;
    char *cp;

    if (stat (file, &sb) < 0)
    {
	if (errno != ENOENT)
	    error (1, errno, "cannot stat temp file");
	if (use_unchanged)
	  {
	    /* Missing file means lost or unmodified; check entries
	       file to see which.

	       XXX FIXME - If there's no entries file line, we
	       wouldn't be getting the file at all, so consider it
	       lost.  I don't know that that's right, but it's not
	       clear to me that either choice is.  Besides, would we
	       have an RCS string in that case anyways?  */
	    if (vers_ts->entdata == NULL)
	      mark_lost (vers_ts);
	    else if (vers_ts->entdata->timestamp
		     && vers_ts->entdata->timestamp[0] == '=')
	      mark_unchanged (vers_ts);
	    else
	      mark_lost (vers_ts);
	  }
	else
	  {
	    /* Missing file in the temp directory means that the file
	       was not modified.  */
	    mark_unchanged (vers_ts);
	  }
    }
    else if (sb.st_mtime == 0)
    {
	if (use_unchanged)
	  /* We shouldn't reach this case any more!  */
	  abort ();

	/* Special code used by server.c to indicate the file was lost.  */
	mark_lost (vers_ts);
    }
    else
    {
	vers_ts->ts_user = xmalloc (25);
	cp = asctime (gmtime (&sb.st_mtime));	/* copy in the modify time */
	cp[24] = 0;
	(void) strcpy (vers_ts->ts_user, cp);
    }
}

#endif /* SERVER_SUPPORT */
/*
 * Gets the time-stamp for the file "file" and returns it in space it
 * allocates
 */
char *
time_stamp (file)
    char *file;
{
    struct stat sb;
    char *cp;
    char *ts;

    if (stat (file, &sb) < 0)
    {
	ts = NULL;
    }
    else
    {
	ts = xmalloc (25);
	cp = asctime (gmtime (&sb.st_mtime));	/* copy in the modify time */
	cp[24] = 0;
	(void) strcpy (ts, cp);
    }

    return (ts);
}

/*
 * free up a Vers_TS struct
 */
void
freevers_ts (versp)
    Vers_TS **versp;
{
    if ((*versp)->srcfile)
	freercsnode (&((*versp)->srcfile));
    if ((*versp)->vn_user)
	free ((*versp)->vn_user);
    if ((*versp)->vn_rcs)
	free ((*versp)->vn_rcs);
    if ((*versp)->ts_user)
	free ((*versp)->ts_user);
    if ((*versp)->ts_rcs)
	free ((*versp)->ts_rcs);
    if ((*versp)->options)
	free ((*versp)->options);
    if ((*versp)->tag)
	free ((*versp)->tag);
    if ((*versp)->date)
	free ((*versp)->date);
    if ((*versp)->ts_conflict)
	free ((*versp)->ts_conflict);
    free ((char *) *versp);
    *versp = (Vers_TS *) NULL;
}

/* filesubr.c --- subroutines for dealing with files under OS/2
   Jim Blandy <jimb@cyclic.com> and Karl Fogel <kfogel@cyclic.com>

   This file is part of GNU CVS.

   GNU CVS is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* These functions were moved out of subr.c because they need different
   definitions under operating systems (like, say, Windows NT) with different
   file system semantics.  */

#include <io.h>

#include "cvs.h"

#ifndef lint
static const char rcsid[] = "$CVSid:$";
USE(rcsid);
#endif

/*
 * I don't know of a convenient way to test this at configure time, or else
 * I'd certainly do it there.  -JimB
 */
#if defined(NeXT)
#define LOSING_TMPNAM_FUNCTION
#endif

static int deep_remove_dir PROTO((const char *path));

/*
 * Copies "from" to "to".
 */
void
copy_file (from, to)
    const char *from;
    const char *to;
{
    struct stat sb;
    struct utimbuf t;
    int fdin, fdout;

    if (trace)
#ifdef SERVER_SUPPORT
	(void) fprintf (stderr, "%c-> copy(%s,%s)\n",
			(server_active) ? 'S' : ' ', from, to);
#else
	(void) fprintf (stderr, "-> copy(%s,%s)\n", from, to);
#endif
    if (noexec)
	return;

    if ((fdin = open (from, O_RDONLY | O_BINARY)) < 0)
	error (1, errno, "cannot open %s for copying", from);
    if (fstat (fdin, &sb) < 0)
	error (1, errno, "cannot fstat %s", from);
    if ((fdout = open (to, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY,
					   (int) sb.st_mode & 07777)) < 0)
	error (1, errno, "cannot create %s for copying", to);
    if (sb.st_size > 0)
    {
	char buf[BUFSIZ];
	int n;

	for (;;) 
	{
	    n = read (fdin, buf, sizeof(buf));
	    if (n == -1)
	    {
#ifdef EINTR
		if (errno == EINTR)
		    continue;
#endif
		error (1, errno, "cannot read file %s for copying", from);
	    }
            else if (n == 0) 
		break;
  
	    if (write(fdout, buf, n) != n) {
		error (1, errno, "cannot write file %s for copying", to);
	    }
	}

#ifdef HAVE_FSYNC
	if (fsync (fdout)) 
	    error (1, errno, "cannot fsync file %s after copying", to);
#endif
    }

    if (close (fdin) < 0) 
	error (0, errno, "cannot close %s", from);
    if (close (fdout) < 0)
	error (1, errno, "cannot close %s", to);

    /* now, set the times for the copied file to match those of the original */
    memset ((char *) &t, 0, sizeof (t));
    t.actime = sb.st_atime;
    t.modtime = sb.st_mtime;
    (void) utime (to, &t);
}

/*
 * link a file, if possible.  Warning: the Windows NT version of this
 * function just copies the file, so only use this function in ways
 * that can deal with either a link or a copy.
 */
int
link_file (from, to)
    const char *from;
    const char *to;
{
    copy_file (from, to);
    return 0;
}

/* FIXME-krp: these functions would benefit from caching the char * &
   stat buf.  */

/*
 * Returns non-zero if the argument file is a directory, or is a symbolic
 * link which points to a directory.
 */
int
isdir (file)
    const char *file;
{
    struct stat sb;

    if (stat (file, &sb) < 0)
	return (0);
    return (S_ISDIR (sb.st_mode));
}

/*
 * Returns non-zero if the argument file is a symbolic link.
 */
int
islink (file)
    const char *file;
{
#ifdef S_ISLNK
    struct stat sb;

    if (lstat (file, &sb) < 0)
	return (0);
    return (S_ISLNK (sb.st_mode));
#else
    return (0);
#endif
}

/*
 * Returns non-zero if the argument file exists.
 */
int
isfile (file)
    const char *file;
{
    struct stat sb;

    if (stat (file, &sb) < 0)
	return (0);
    return (1);
}

/*
 * Returns non-zero if the argument file is readable.
 * XXX - must be careful if "cvs" is ever made setuid!
 */
int
isreadable (file)
    const char *file;
{
    return (access (file, R_OK) != -1);
}

/*
 * Returns non-zero if the argument file is writable
 * XXX - muct be careful if "cvs" is ever made setuid!
 */
int
iswritable (file)
    const char *file;
{
    return (access (file, W_OK) != -1);
}

/*
 * Returns non-zero if the argument file is accessable according to
 * mode.  If compiled with SETXID_SUPPORT also works if cvs has setxid
 * bits set.
 */
int
isaccessible (file, mode)
    const char *file;
    const int mode;
{
    return access(file, mode) == 0;
}


/*
 * Open a file and die if it fails
 */
FILE *
open_file (name, mode)
    const char *name;
    const char *mode;
{
    FILE *fp;

    if ((fp = fopen (name, mode)) == NULL)
	error (1, errno, "cannot open %s", name);
    return (fp);
}

/*
 * Make a directory and die if it fails
 */
void
make_directory (name)
    const char *name;
{
    struct stat buf;

    if (stat (name, &buf) == 0 && (!S_ISDIR (buf.st_mode)))
	    error (0, 0, "%s already exists but is not a directory", name);
    if (!noexec && mkdir (name) < 0)
	error (1, errno, "cannot make directory %s", name);
}

/*
 * Make a path to the argument directory, printing a message if something
 * goes wrong.
 */
void
make_directories (name)
    const char *name;
{
    char *cp;

    if (noexec)
	return;

    if (mkdir (name) == 0 || errno == EACCESS)
	return;
    if (! existence_error (errno))
    {
	error (0, errno, "cannot make path to %s", name);
	return;
    }
    if ((cp = strrchr (name, '/')) == NULL)
	return;
    *cp = '\0';
    make_directories (name);
    *cp++ = '/';
    if (*cp == '\0')
	return;
    (void) mkdir (name);
}

/*
 * Change the mode of a file, either adding write permissions, or removing
 * all write permissions.  Adding write permissions honors the current umask
 * setting.
 */
void
xchmod (fname, writable)
    char *fname;
    int writable;
{
    char *attrib_cmd;
    char *attrib_option;
    char *whole_cmd;
    char *p;
    char *q;

    if (!isfile (fname))
	return ENOENT;

    attrib_cmd = "attrib "; /* No, really? */

    if (writable)
        attrib_option = "-r ";  /* make writeable */
    else
        attrib_option = "+r ";  /* make read-only */
        
    whole_cmd = xmalloc (strlen (attrib_cmd)
                         + strlen (attrib_option)
                         + strlen (fname)
                         + 1);

    strcpy (whole_cmd, attrib_cmd);
    strcat (whole_cmd, attrib_option);

    /* Copy fname to the end of whole_cmd, translating / to \.
	   Attrib doesn't take / but many parts of CVS rely
       on being able to use it.  */
    p = whole_cmd + strlen (whole_cmd);
    q = fname;
    while (*q)
    {
	if (*q == '/')
	    *p++ = '\\';
	else
	    *p++ = *q;
	++q;
    }
    *p = '\0';

    system (whole_cmd);
    free (whole_cmd);
}


/* Read the value of a symbolic link.
   Under OS/2, this function always returns EINVAL.  */
int
readlink (char *path, char *buf, int buf_size)
{
    errno = EINVAL;
    return -1;
}

/*
 * unlink a file, if possible.
 */
int
unlink_file (f)
    const char *f;
{
    if (trace)
#ifdef SERVER_SUPPORT
	(void) fprintf (stderr, "%c-> unlink(%s)\n",
			(server_active) ? 'S' : ' ', f);
#else
	(void) fprintf (stderr, "-> unlink(%s)\n", f);
#endif
    if (noexec)
	return (0);

   /* Win32 unlink is stupid - it fails if the file is read-only.
    * OS/2 is similarly stupid.  It does have a remove() function,
    * but the documentation does not make clear why remove() is or
    * isn't preferable to unlink().  I'll use unlink() because the
    * name is closer to our interface, what the heck.  Also, we know
    * unlink()'s error code when trying to remove a directory.
    */
    xchmod (f, 1);
    return (unlink (f));
}

/*
 * Unlink a file or dir, if possible.  If it is a directory do a deep
 * removal of all of the files in the directory.  Return -1 on error
 * (in which case errno is set).
 */
int
unlink_file_dir (f)
    const char *f;
{
    if (trace)
#ifdef SERVER_SUPPORT
	(void) fprintf (stderr, "%c-> unlink_file_dir(%s)\n",
			(server_active) ? 'S' : ' ', f);
#else
	(void) fprintf (stderr, "-> unlink_file_dir(%s)\n", f);
#endif
    if (noexec)
	return (0);

    if (unlink_file (f) != 0)
    {
	/* under OS/2, unlink returns EACCESS if the path
	   is a directory.  */
        if (errno == EACCESS)
                return deep_remove_dir (f);
        else
		/* The file wasn't a directory and some other
		 * error occured
		 */
                return -1;
    }
    /* We were able to remove the file from the disk */
    return 0;
}

/* Remove a directory and everything it contains.  Returns 0 for
 * success, -1 for failure (in which case errno is set).
 */

static int
deep_remove_dir (path)
    const char *path;
{
    DIR		  *dirp;
    struct dirent *dp;
    char	   buf[PATH_MAX];

    if ( rmdir (path) != 0 && errno == EACCESS )
    {
	if ((dirp = opendir (path)) == NULL)
	    /* If unable to open the directory return
	     * an error
	     */
	    return -1;

	while ((dp = readdir (dirp)) != NULL)
	{
	    if (strcmp (dp->d_name, ".") == 0 ||
			strcmp (dp->d_name, "..") == 0)
		continue;

	    sprintf (buf, "%s/%s", path, dp->d_name);

	    if (unlink_file (buf) != 0 )
	    {
		if (errno == EACCES)
		{
		    if (deep_remove_dir (buf))
		    {
			closedir (dirp);
			return -1;
		    }
		}
		else
		{
		    /* buf isn't a directory, or there are
		     * some sort of permision problems
		     */
		    closedir (dirp);
		    return -1;
		}
	    }
	}
	closedir (dirp);
	return rmdir (path);
    }
    /* Was able to remove the directory return 0 */
    return 0;
}


/*
 * Rename a file and die if it fails
 */
void
rename_file (from, to)
    const char *from;
    const char *to;
{
    if (trace)
#ifdef SERVER_SUPPORT
	(void) fprintf (stderr, "%c-> rename(%s,%s)\n",
			(server_active) ? 'S' : ' ', from, to);
#else
	(void) fprintf (stderr, "-> rename(%s,%s)\n", from, to);
#endif
    if (noexec)
	return;

    unlink_file (to);
    if (rename (from, to) != 0)
	error (1, errno, "cannot rename file %s to %s", from, to);
}


/* Read NCHARS bytes from descriptor FD into BUF.
   Return the number of characters successfully read.
   The number returned is always NCHARS unless end-of-file or error.  */
static size_t
block_read (fd, buf, nchars)
    int fd;
    char *buf;
    size_t nchars;
{
    char *bp = buf;
    size_t nread;

    do 
    {
	nread = read (fd, bp, nchars);
	if (nread == (size_t)-1)
	{
#ifdef EINTR
	    if (errno == EINTR)
		continue;
#endif
	    return (size_t)-1;
	}

	if (nread == 0)
	    break; 

	bp += nread;
	nchars -= nread;
    } while (nchars != 0);

    return bp - buf;
} 

    
/*
 * Compare "file1" to "file2". Return non-zero if they don't compare exactly.
 */
int
xcmp (file1, file2)
    const char *file1;
    const char *file2;
{
    char *buf1, *buf2;
    struct stat sb1, sb2;
    int fd1, fd2;
    int ret;

    if ((fd1 = open (file1, O_RDONLY | O_BINARY)) < 0)
	error (1, errno, "cannot open file %s for comparing", file1);
    if ((fd2 = open (file2, O_RDONLY | O_BINARY)) < 0)
	error (1, errno, "cannot open file %s for comparing", file2);
    if (fstat (fd1, &sb1) < 0)
	error (1, errno, "cannot fstat %s", file1);
    if (fstat (fd2, &sb2) < 0)
	error (1, errno, "cannot fstat %s", file2);

    /* A generic file compare routine might compare st_dev & st_ino here 
       to see if the two files being compared are actually the same file.
       But that won't happen in CVS, so we won't bother. */

    if (sb1.st_size != sb2.st_size)
	ret = 1;
    else if (sb1.st_size == 0)
	ret = 0;
    else
    {
	/* FIXME: compute the optimal buffer size by computing the least
	   common multiple of the files st_blocks field */
	size_t buf_size = 8 * 1024;
	size_t read1;
	size_t read2;

	buf1 = xmalloc (buf_size);
	buf2 = xmalloc (buf_size);

	do 
	{
	    read1 = block_read (fd1, buf1, buf_size);
	    if (read1 == (size_t)-1)
		error (1, errno, "cannot read file %s for comparing", file1);

	    read2 = block_read (fd2, buf2, buf_size);
	    if (read2 == (size_t)-1)
		error (1, errno, "cannot read file %s for comparing", file2);

	    /* assert (read1 == read2); */

	    ret = memcmp(buf1, buf2, read1);
	} while (ret == 0 && read1 == buf_size);

	free (buf1);
	free (buf2);
    }
	
    (void) close (fd1);
    (void) close (fd2);
    return (ret);
}


/* The equivalence class mapping for filenames.
   OS/2 filenames are case-insensitive, but case-preserving.  Both /
   and \ are path element separators. 
   Thus, this table maps both upper and lower case to lower case, and
   both / and \ to /.  

   Much thanks to Jim Blandy, who already invented this wheel in the
   Windows NT port. */

#if 0
main ()
{
  int c;

  for (c = 0; c < 256; c++)
    {
      int t;

      if (c == '\\')
        t = '/';
      else
        t = tolower (c);
      
      if ((c & 0x7) == 0x0)
         printf ("    ");
      printf ("0x%02x,", t);
      if ((c & 0x7) == 0x7)
         putchar ('\n');
      else if ((c & 0x7) == 0x3)
         putchar (' ');
    }
}
#endif


unsigned char
OS2_filename_classes[] =
{
    0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07,
    0x08,0x09,0x0a,0x0b, 0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17,
    0x18,0x19,0x1a,0x1b, 0x1c,0x1d,0x1e,0x1f,
    0x20,0x21,0x22,0x23, 0x24,0x25,0x26,0x27,
    0x28,0x29,0x2a,0x2b, 0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33, 0x34,0x35,0x36,0x37,
    0x38,0x39,0x3a,0x3b, 0x3c,0x3d,0x3e,0x3f,
    0x40,0x61,0x62,0x63, 0x64,0x65,0x66,0x67,
    0x68,0x69,0x6a,0x6b, 0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73, 0x74,0x75,0x76,0x77,
    0x78,0x79,0x7a,0x5b, 0x2f,0x5d,0x5e,0x5f,
    0x60,0x61,0x62,0x63, 0x64,0x65,0x66,0x67,
    0x68,0x69,0x6a,0x6b, 0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73, 0x74,0x75,0x76,0x77,
    0x78,0x79,0x7a,0x7b, 0x7c,0x7d,0x7e,0x7f,
    0x80,0x81,0x82,0x83, 0x84,0x85,0x86,0x87,
    0x88,0x89,0x8a,0x8b, 0x8c,0x8d,0x8e,0x8f,
    0x90,0x91,0x92,0x93, 0x94,0x95,0x96,0x97,
    0x98,0x99,0x9a,0x9b, 0x9c,0x9d,0x9e,0x9f,
    0xa0,0xa1,0xa2,0xa3, 0xa4,0xa5,0xa6,0xa7,
    0xa8,0xa9,0xaa,0xab, 0xac,0xad,0xae,0xaf,
    0xb0,0xb1,0xb2,0xb3, 0xb4,0xb5,0xb6,0xb7,
    0xb8,0xb9,0xba,0xbb, 0xbc,0xbd,0xbe,0xbf,
    0xc0,0xc1,0xc2,0xc3, 0xc4,0xc5,0xc6,0xc7,
    0xc8,0xc9,0xca,0xcb, 0xcc,0xcd,0xce,0xcf,
    0xd0,0xd1,0xd2,0xd3, 0xd4,0xd5,0xd6,0xd7,
    0xd8,0xd9,0xda,0xdb, 0xdc,0xdd,0xde,0xdf,
    0xe0,0xe1,0xe2,0xe3, 0xe4,0xe5,0xe6,0xe7,
    0xe8,0xe9,0xea,0xeb, 0xec,0xed,0xee,0xef,
    0xf0,0xf1,0xf2,0xf3, 0xf4,0xf5,0xf6,0xf7,
    0xf8,0xf9,0xfa,0xfb, 0xfc,0xfd,0xfe,0xff,
};

/* Like strcmp, but with the appropriate tweaks for file names.
   Under OS/2, filenames are case-insensitive but case-preserving, and
   both \ and / are path element separators.  */ 
int
fncmp (const char *n1, const char *n2)
{
    while (*n1 && *n2
           && (OS2_filename_classes[(unsigned char) *n1]
	       == OS2_filename_classes[(unsigned char) *n2]))
        n1++, n2++;
    return (OS2_filename_classes[(unsigned char) *n1]
            - OS2_filename_classes[(unsigned char) *n1]);
}

/* Fold characters in FILENAME to their canonical forms.  
   If FOLD_FN_CHAR is not #defined, the system provides a default
   definition for this.  */
void
fnfold (char *filename)
{
    while (*filename)
    {
        *filename = FOLD_FN_CHAR (*filename);
	filename++;
    }
}


/* Return non-zero iff FILENAME is absolute.
   Trivial under Unix, but more complicated under other systems.  */
int
isabsolute (filename)
    const char *filename;
{
    return (ISDIRSEP (filename[0])
            || (filename[0] != '\0'
                && filename[1] == ':'
                && ISDIRSEP (filename[2])));
}

/* Return a pointer into PATH's last component.  */
char *
last_component (char *path)
{
    char *scan;
    char *last = 0;

    for (scan = path; *scan; scan++)
        if (ISDIRSEP (*scan))
	    last = scan;

    if (last)
        return last + 1;
    else
        return path;
}


/* Read data from INFILE, and copy it to OUTFILE. 
   Open INFILE using INFLAGS, and OUTFILE using OUTFLAGS.
   This is useful for converting between CRLF and LF line formats.  */
void
convert_file (char *infile,  int inflags,
	      char *outfile, int outflags)
{
    int infd, outfd;
    char buf[8192];
    int len;

    if ((infd = open (infile, inflags, S_IREAD | S_IWRITE)) < 0)
        error (1, errno, "couldn't read %s", infile);
    if ((outfd = open (outfile, outflags, S_IREAD | S_IWRITE)) < 0)
        error (1, errno, "couldn't write %s", outfile);

    while ((len = read (infd, buf, sizeof (buf))) > 0)
        if (write (outfd, buf, len) < 0)
	    error (1, errno, "error writing %s", outfile);
    if (len < 0)
        error (1, errno, "error reading %s", infile);

    if (close (outfd) < 0)
        error (0, errno, "warning: couldn't close %s", outfile);
    if (close (infd) < 0)
        error (0, errno, "warning: couldn't close %s", infile);
}

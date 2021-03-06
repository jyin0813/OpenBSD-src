/* config.h --- configuration file for OS/2
   Karl Fogel <kfogel@cyclic.com> --- Oct 1995  */

/* This file lives in the os2/ subdirectory, which is only included
 * in your header search path if you're working under IBM C++,
 * and use os2/makefile (with GNU make for OS/2).  Thus, this is the
 * right place to put configuration information for OS/2.
 */


/* You bet! */
#define __STDC__ 1

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#undef _ALL_SOURCE

/* Define if using alloca.c.  */
#undef C_ALLOCA

/* Define if type char is unsigned and you are not using gcc.  */
/* We wrote a little test program whose output suggests that char is
   signed on this system.  Go back and check the verdict when CVS
   is configured on floss...  */
#undef __CHAR_UNSIGNED__

/* Define to empty if the keyword does not work.  */
/* Const is working.  */
#undef const

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* This shouldn't matter, but pro forma:  */
#undef CRAY_STACKSEG_END

/* Define to `int' if <sys/types.h> doesn't define.  */
/* OS/2 doesn't have gid_t.  It doesn't even really have group
   numbers, I think.  This will take more thought to get right, but
   let's get it running first.  */
#define gid_t int

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1
/* OS/2 has alloca() in <stdlib.h>! */
#define ALLOCA_IN_STDLIB 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
/* but calls it _alloca and says it returns void *.  We provide our
   own header file.  */
/* OS/2 declares alloca in `stdlib.h'. */
/* #define HAVE_ALLOCA_H 1 */
#undef HAVE_ALLOCA_H

/* Define if you support file names longer than 14 characters.  */
/* We support long file names, but not long corporate acronyms. */
#define HAVE_LONG_FILE_NAMES 1

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
/* If POSIX.1 requires this, why doesn't WNT have it?  */
/* Maybe POSIX only says that if it is present, it must behave a
   certain way, but that it can simply be not present too.  I
   dunno. */
/* Anyway, OS/2 ain't got it. */
#undef HAVE_SYS_WAIT_H

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
/* Documentation says yup; haven't verified experimentally. */
#define HAVE_UTIME_NULL 1

/* We don't appear to have inline functions, so just expand "inline"
   to "". */
#define inline 

/* Define if on MINIX.  */
/* Hah.  */
#undef _MINIX

/* Define to `int' if <sys/types.h> doesn't define.  */
#define mode_t int

/* Define to `int' if <sys/types.h> doesn't define.  */
#define pid_t int

/* Define if the system does not provide POSIX.1 features except
   with this defined.  */
/* This string doesn't appear anywhere in the system header files,
   so I assume it's irrelevant.  */
#undef _POSIX_1_SOURCE

/* Define if you need to in order for stat and other things to work.  */
/* Same as for _POSIX_1_SOURCE, above.  */
#undef _POSIX_SOURCE

/* Define as the return type of signal handlers (int or void).  */
/* IBMCPP manual indicates they are void.  */
#define RETSIGTYPE void

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* sys/types.h doesn't define it, but stdio.h does, which cvs.h
   #includes, so things should be okay.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown
 */
/* This shouldn't matter, but pro forma:  */
#undef STACK_DIRECTION

/* Define if the `S_IS*' macros in <sys/stat.h> do not work properly. */
/* sys/stat.h apparently doesn't even have them; setting this will let
   ../lib/system.h define them. */
#define STAT_MACROS_BROKEN 1
 
/* Define if you have the ANSI C header files.  */
/* We have at least a reasonable facsimile thereof. */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
/* We don't have <sys/time.h> at all.  Why isn't there a definition
   for HAVE_SYS_TIME_H anywhere in config.h.in?  */
#undef TIME_WITH_SYS_TIME

/* Define to `int' if <sys/types.h> doesn't define.  */
#define uid_t int

/* Define if you have MIT Kerberos version 4 available.  */
/* We don't. */
#undef HAVE_KERBEROS

/* Define if you want CVS to be able to be a remote repository client.  */
/* That's all we want.  */
#define CLIENT_SUPPORT

/* Define if you want CVS to be able to serve repositories to remote
   clients.  */
/* No server support yet.  Note that you don't have to define
   CLIENT_SUPPORT or SERVER_SUPPORT to enable the non-remote code;
   that's always there.  */
#undef SERVER_SUPPORT

/* the path to the gnu diff program on your system  */
/* We don't need this for CLIENT side.  */
#undef DIFF

/* the path to the gnu grep program on your system  */
/* We don't need this for CLIENT side.  */
#undef GREP

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* Define if you have the connect function.  */
/* Not used?  */
/* It appears to be used in client.c now... don't know yet it OS/2 has it. */
#define HAVE_CONNECT

/* Define if you have the fchdir function.  */
#undef HAVE_FCHDIR

/* Define if you have the fchmod function.  */
#undef HAVE_FCHMOD

/* Define if you have the fsync function.  */
#undef HAVE_FSYNC

/* Define if you have the ftime function.  */
#define HAVE_FTIME 1

/* Define if you have the ftruncate function.  */
#undef HAVE_FTRUNCATE

/* Define if you have the getpagesize function.  */
#undef HAVE_GETPAGESIZE

/* Define if you have the krb_get_err_text function.  */
#undef HAVE_KRB_GET_ERR_TEXT

/* Define if you have the mkfifo function.  */
#undef HAVE_MKFIFO

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the setvbuf function.  */
#define HAVE_SETVBUF 1

/* Define if you have the sigaction function.  */
#undef HAVE_SIGACTION

/* Define if you have the sigblock function.  */
#undef HAVE_SIGBLOCK

/* Define if you have the sigprocmask function.  */
#undef HAVE_SIGPROCMASK

/* Define if you have the sigsetmask function.  */
#undef HAVE_SIGSETMASK

/* Define if you have the sigvec function.  */
#undef HAVE_SIGVEC

/* Define if you have the timezone function.  */
/* Hmm, I actually rather think it's an extern long
   variable; that message was mechanically generated
   by autoconf.  And I don't see any actual uses of
   this function in the code anyway, hmm.  */
#undef HAVE_TIMEZONE

/* Define if you have the vfork function.  */
#undef HAVE_VFORK

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define if you have the <direct.h> header file.  */
#define HAVE_DIRECT_H 1

/* Define if you have the <dirent.h> header file.  */
/* We have our own dirent.h and dirent.c. */
#define HAVE_DIRENT_H 1

/* Define if you have the <errno.h> header file.  */
#define HAVE_ERRNO_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <io.h> header file.  */
/* Low-level Unix I/O routines like open, creat, etc.  */
#define HAVE_IO_H 1

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <ndbm.h> header file.  */
#undef HAVE_NDBM_H

/* Define if you have the <ndir.h> header file.  */
#undef HAVE_NDIR_H

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <sys/bsdtypes.h> header file.  */
#undef HAVE_SYS_BSDTYPES_H

/* Define if you have the <sys/dir.h> header file.  */
#undef HAVE_SYS_DIR_H

/* Define if you have the <sys/ndir.h> header file.  */
#undef HAVE_SYS_NDIR_H

/* Define if you have the <sys/param.h> header file.  */	
#undef HAVE_SYS_PARAM_H

/* Define if you have the <sys/select.h> header file.  */
#undef HAVE_SYS_SELECT_H

/* Define if you have the <sys/time.h> header file.  */
#undef HAVE_SYS_TIME_H

/* Define if you have the <sys/timeb.h> header file.  */
#define HAVE_SYS_TIMEB_H 1

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H

/* Define if you have the <utime.h> header file.  */
#undef HAVE_UTIME_H

/* Define if you have the <sys/utime.h> header file.  */
#define HAVE_SYS_UTIME_H 1

/* Define if you have the inet library (-linet).  */
#undef HAVE_LIBINET

/* Define if you have the nsl library (-lnsl).  */
/* This is not used anywhere in the source code.  */
#undef HAVE_LIBNSL

/* Define if you have the nsl_s library (-lnsl_s).  */
#undef HAVE_LIBNSL_S

/* Define if you have the socket library (-lsocket).  */
/* This isn't ever used either.  */
#undef HAVE_LIBSOCKET

/* Under OS/2, mkdir only takes one argument.  */
#define CVS_MKDIR os2_mkdir
extern int os2_mkdir (const char *PATH, int MODE);

/* This function doesn't exist under OS/2; we provide a stub. */
extern int readlink (char *path, char *buf, int buf_size);

/* This is just a call to GetCurrentProcessID.  */
extern pid_t getpid (void);

/* We definitely have prototypes.  */
#define USE_PROTOTYPES 1

/* Under OS/2, filenames are case-insensitive, and both / and \
   are path component separators.  */
#define FOLD_FN_CHAR(c) (OS2_filename_classes[(unsigned char) (c)])
extern unsigned char OS2_filename_classes[];

/* Is the character C a path name separator?  Under OS/2, you can use
   either / or \.  */
#define ISDIRSEP(c) (FOLD_FN_CHAR(c) == '/')

/* Like strcmp, but with the appropriate tweaks for file names.
   Under OS/2, filenames are case-insensitive but case-preserving,
   and both \ and / are path element separators.  */
extern int fncmp (const char *n1, const char *n2);

/* Fold characters in FILENAME to their canonical forms.  
   If FOLD_FN_CHAR is not #defined, the system provides a default
   definition for this.  */
extern void fnfold (char *FILENAME);

/* #define this if your system terminates lines in text files with
   CRLF instead of plain LF, and your I/O functions automatically
   translate between using LF in memory and CRLF on disk, unless you
   specifically tell them not to.  */
#define LINES_CRLF_TERMINATED 1

/* Read data from INFILE, and copy it to OUTFILE. 
   Open INFILE using INFLAGS, and OUTFILE using OUTFLAGS.
   This is useful for converting between CRLF and LF line formats.  */
extern void convert_file (char *INFILE,  int INFLAGS,
			  char *OUTFILE, int OUTFLAGS);

/* This is where old bits go to die under OS/2 as well as WinNT.  */
#define DEVNULL "nul"

/* Comment markers for some OS/2-specific file types.  */
/* Actually, these come from WinNT, but what the heck. */
#define SYSTEM_COMMENT_TABLE \
    "mak", "# ",    			/* makefile */                    \
    "rc",  " * ",   			/* MS Windows resource file */    \
    "dlg", " * ",   			/* MS Windows dialog file */      \
    "frm", "' ",    			/* Visual Basic form */           \
    "bas", "' ",    			/* Visual Basic code */

/* Make sure that we don't try to perform operations on RCS files on the
   local machine.  I think I neglected to apply some changes from
   MHI's port in that area of code, or found some issues I didn't want
   to deal with.  */
#define CLIENT_ONLY

/* We actually do have a transparent rsh, whew. */
#undef RSH_NOT_TRANSPARENT
/* But it won't be transparent unless we ask it nicely! */
#define RSH_NEEDS_BINARY_FLAG 1

/* OS/2 doesn't really have user/group permissions, at least not
   according to the C library manual pages.  So we'll make decoys. */
#define NEED_DECOY_PERMISSIONS 1     /* see system.h */

/* See client.c.  Setting execute bits with chmod seems to lose under
   OS/2, although in some places the documentation grudgingly admits
   to the existence of execute bits. */
#define EXECUTE_PERMISSION_LOSES 1



/* For the access() function, for which OS/2 has no pre-defined
   mnemonic masks. */
#define R_OK 04
#define W_OK 02
#define F_OK 00
#define X_OK R_OK  /* I think this is right for OS/2. */

/* For getpid() */
#include <process.h>

/* So "tcpip.h" gets included in lib/system.h: */
#define USE_OWN_TCPIP_H 1
/* The IBM TCP/IP library gets initialized in main(): */
#define INITIALIZE_SOCKET_SUBSYSTEM init_sockets
extern void init_sockets();

/* Under OS/2, we have our own popen() and pclose()... */
#define USE_OWN_POPEN 1
/* ... and we use popenRW to start the rsh server. */
#define START_RSH_WITH_POPEN_RW 1

/*
 * This tells the client that it must use send()/recv() to talk to the
 * server if it is connected to the server via a socket.  Sigh.
 * Windows 95 also cannot convert sockets to file descriptors,
 * apparently.
 */
#define NO_SOCKET_TO_FD 1

/* chmod() doesn't seem to work -- IBM's own example program does not
 * behave as its documentation claims, in fact!  I suspect that
 * DosSetPathInfo is the way to go, but can't seem to make that work
 * either.  For now, we can deal with some cases by invoking the DOS
 * "attrib" command via system().  */
#define CHMOD_BROKEN 1

/* Rule Number 1 of OS/2 Programming: If the function you're looking
   for doesn't exist, try putting "Dos" in front of it. */
#ifndef sleep
#define sleep(x) DosSleep(((long)(x))*1000L)
#endif /* sleep */

/* Set to 1 for some debugging messages. */
#if 0
#define KFF_DEBUG(call) printf("*** %s:%d: ", __FILE__, __LINE__); \
                        call; fflush(stdout);
#else
#define KFF_DEBUG(call)
#endif

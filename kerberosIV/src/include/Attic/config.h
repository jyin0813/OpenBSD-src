/* include/config.h.  Generated automatically by configure.  */
/* include/config.h.in.  Generated automatically from configure.in by autoheader.  */
/* $KTH: acconfig.h,v 1.106 2000/11/05 17:14:13 joda Exp $ */


#ifndef RCSID
#define RCSID(msg) \
static /**/const char *const rcsid[] = { (const char *)rcsid, "\100(#)" msg }
#endif

#if 0
#define BINDIR "/usr/athena/bin"
#define LIBDIR "/usr/athena/lib"
#define LIBEXECDIR "/usr/athena/libexec"
#define SBINDIR "/usr/athena/sbin"
#endif

#define HAVE_INT8_T 1
#define HAVE_INT16_T 1
#define HAVE_INT32_T 1
#define HAVE_INT64_T 1
#define HAVE_U_INT8_T 1
#define HAVE_U_INT16_T 1
#define HAVE_U_INT32_T 1
#define HAVE_U_INT64_T 1

/* Maximum values on all known systems */
#define MaxHostNameLen (64+4)
#define MaxPathLen (1024+4)



/* Define if realloc(NULL) doesn't work. */
/* #undef BROKEN_REALLOC */

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Define this to the kerberos database directory. */
/* #undef DB_DIR */

/* Set this to the type of des-quad-cheksum to use. */
#define DES_QUAD_DEFAULT DES_QUAD_GUESS

/* Define this to be the directory where the dictionary for cracklib resides.
   */
/* #undef DICTPATH */

/* define if prototype of gethostbyaddr is compatible with struct hostent
   *gethostbyaddr(const void *, size_t, int) */
/* #undef GETHOSTBYADDR_PROTO_COMPATIBLE */

/* define if prototype of gethostbyname is compatible with struct hostent
   *gethostbyname(const char *) */
#define GETHOSTBYNAME_PROTO_COMPATIBLE 1

/* define if prototype of getservbyname is compatible with struct servent
   *getservbyname(const char *, const char *) */
#define GETSERVBYNAME_PROTO_COMPATIBLE 1

/* define if prototype of getsockname is compatible with int getsockname(int,
   struct sockaddr*, socklen_t*) */
#define GETSOCKNAME_PROTO_COMPATIBLE 1

/* Define if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix). */
/* #undef HAVE_ALLOCA_H */

/* Define if you have the `arc4random' function. */
#define HAVE_ARC4RANDOM 1

/* Define if you have the <arpa/ftp.h> header file. */
#define HAVE_ARPA_FTP_H 1

/* Define if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define if you have the <arpa/nameser.h> header file. */
#define HAVE_ARPA_NAMESER_H 1

/* Define if you have the <arpa/telnet.h> header file. */
#define HAVE_ARPA_TELNET_H 1

/* Define if you have the `asnprintf' function. */
/* #undef HAVE_ASNPRINTF */

/* Define if you have the `asprintf' function. */
#define HAVE_ASPRINTF 1

/* Define if you have the `atexit' function. */
#define HAVE_ATEXIT 1

/* Define if you have the <bsdsetjmp.h> header file. */
/* #undef HAVE_BSDSETJMP_H */

/* Define if you have the <bsd/bsd.h> header file. */
/* #undef HAVE_BSD_BSD_H */

/* Define if you have the `bswap16' function. */
/* #undef HAVE_BSWAP16 */

/* Define if you have the `bswap32' function. */
/* #undef HAVE_BSWAP32 */

/* Define if you have the <capability.h> header file. */
/* #undef HAVE_CAPABILITY_H */

/* Define if you have the `cap_set_proc' function. */
/* #undef HAVE_CAP_SET_PROC */

/* Define if you have the `cgetent' function. */
#define HAVE_CGETENT 1

/* Define if you have the `chown' function. */
#define HAVE_CHOWN 1

/* Define if you have the `chroot' function. */
#define HAVE_CHROOT 1

/* Define if you have the <config.h> header file. */
/* #undef HAVE_CONFIG_H */

/* Define if you have the `copyhostent' function. */
/* #undef HAVE_COPYHOSTENT */

/* Define if you have the `crypt' function. */
#define HAVE_CRYPT 1

/* Define if you have the <crypt.h> header file. */
/* #undef HAVE_CRYPT_H */

/* Define if you have the <curses.h> header file. */
#define HAVE_CURSES_H 1

/* Define if you have the `daemon' function. */
#define HAVE_DAEMON 1

/* define if you have a berkeley db1/2 library */
#define HAVE_DB1 1

/* define if you have a berkeley db3 library */
/* #undef HAVE_DB3 */

/* Define if you have the `dbm_firstkey' function. */
#define HAVE_DBM_FIRSTKEY 1

/* Define if you have the <dbm.h> header file. */
#define HAVE_DBM_H 1

/* Define if you have the `dbopen' function. */
#define HAVE_DBOPEN 1

/* Define if you have the <db_185.h> header file. */
/* #undef HAVE_DB_185_H */

/* Define if you have the `db_create' function. */
/* #undef HAVE_DB_CREATE */

/* Define if you have the <db.h> header file. */
#define HAVE_DB_H 1

/* define if you have ndbm compat in db */
/* #undef HAVE_DB_NDBM */

/* Define if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you have the `dlopen' function. */
#define HAVE_DLOPEN 1

/* Define if you have the `dn_expand' function. */
#define HAVE_DN_EXPAND 1

/* Define if you have the `ecalloc' function. */
/* #undef HAVE_ECALLOC */

/* Define if you have the `el_init' function. */
#define HAVE_EL_INIT 1

/* Define if you have the `emalloc' function. */
/* #undef HAVE_EMALLOC */

/* define if your system declares environ */
/* #undef HAVE_ENVIRON_DECLARATION */

/* Define if you have the `erealloc' function. */
/* #undef HAVE_EREALLOC */

/* Define if you have the `err' function. */
#define HAVE_ERR 1

/* Define if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define if you have the `errx' function. */
#define HAVE_ERRX 1

/* Define if you have the <err.h> header file. */
#define HAVE_ERR_H 1

/* Define if you have the `estrdup' function. */
/* #undef HAVE_ESTRDUP */

/* Define if you have the `fattach' function. */
/* #undef HAVE_FATTACH */

/* Define if you have the `fchmod' function. */
#define HAVE_FCHMOD 1

/* Define if you have the `fchown' function. */
#define HAVE_FCHOWN 1

/* Define if you have the `fcntl' function. */
#define HAVE_FCNTL 1

/* Define if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define if you have the `flock' function. */
#define HAVE_FLOCK 1

/* Define if you have the `fnmatch' function. */
#define HAVE_FNMATCH 1

/* Define if you have the <fnmatch.h> header file. */
#define HAVE_FNMATCH_H 1

/* Define if you have the `forkpty' function. */
/* #undef HAVE_FORKPTY */

/* Define if el_init takes four arguments. */
/* #undef HAVE_FOUR_VALUED_EL_INIT */

/* Define if you have the `freeaddrinfo' function. */
#define HAVE_FREEADDRINFO 1

/* Define if you have the `freehostent' function. */
/* #undef HAVE_FREEHOSTENT */

/* Define if you have the `frevoke' function. */
/* #undef HAVE_FREVOKE */

/* Define if you have the `gai_strerror' function. */
#define HAVE_GAI_STRERROR 1

/* Define if you have the <gdbm/ndbm.h> header file. */
/* #undef HAVE_GDBM_NDBM_H */

/* Define if you have the `getaddrinfo' function. */
#define HAVE_GETADDRINFO 1

/* Define if you have the `getattr' function. */
/* #undef HAVE_GETATTR */

/* Define if you have the `getconfattr' function. */
/* #undef HAVE_GETCONFATTR */

/* Define if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define if you have the `getdtablesize' function. */
#define HAVE_GETDTABLESIZE 1

/* Define if you have the `getegid' function. */
#define HAVE_GETEGID 1

/* Define if you have the `geteuid' function. */
#define HAVE_GETEUID 1

/* Define if you have the `getgid' function. */
#define HAVE_GETGID 1

/* Define if you have the `gethostbyname' function. */
#define HAVE_GETHOSTBYNAME 1

/* Define if you have the `gethostbyname2' function. */
#define HAVE_GETHOSTBYNAME2 1

/* Define if you have the `gethostname' function. */
#define HAVE_GETHOSTNAME 1

/* Define if you have the `getifaddrs' function. */
#define HAVE_GETIFADDRS 1

/* Define if you have the `getipnodebyaddr' function. */
/* #undef HAVE_GETIPNODEBYADDR */

/* Define if you have the `getipnodebyname' function. */
/* #undef HAVE_GETIPNODEBYNAME */

/* Define if you have the `getlogin' function. */
#define HAVE_GETLOGIN 1

/* Define if you have a working getmsg. */
/* #undef HAVE_GETMSG */

/* Define if you have the `getnameinfo' function. */
#define HAVE_GETNAMEINFO 1

/* Define if you have the `getopt' function. */
#define HAVE_GETOPT 1

/* Define if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define if you have the `getpriority' function. */
#define HAVE_GETPRIORITY 1

/* Define if you have the `getprogname' function. */
/* #undef HAVE_GETPROGNAME */

/* Define if you have the `getpwnam_r' function. */
/* #undef HAVE_GETPWNAM_R */

/* Define if you have the `getrlimit' function. */
#define HAVE_GETRLIMIT 1

/* Define if you have the `getservbyname' function. */
#define HAVE_GETSERVBYNAME 1

/* Define if you have the `getsockopt' function. */
#define HAVE_GETSOCKOPT 1

/* Define if you have the `getspnam' function. */
/* #undef HAVE_GETSPNAM */

/* Define if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the `gettosbyname' function. */
/* #undef HAVE_GETTOSBYNAME */

/* Define if you have the `getudbnam' function. */
/* #undef HAVE_GETUDBNAM */

/* Define if you have the `getuid' function. */
#define HAVE_GETUID 1

/* Define if you have the `getusershell' function. */
#define HAVE_GETUSERSHELL 1

/* define if you have a glob() that groks GLOB_BRACE, GLOB_NOCHECK,
   GLOB_QUOTE, GLOB_TILDE, and GLOB_LIMIT */
#define HAVE_GLOB 1

/* Define if you have the `grantpt' function. */
/* #undef HAVE_GRANTPT */

/* Define if you have the <grp.h> header file. */
#define HAVE_GRP_H 1

/* Define if you have the `hstrerror' function. */
#define HAVE_HSTRERROR 1

/* Define if you have the `h_errlist' variable. */
#define HAVE_H_ERRLIST 1

/* define if your system declares h_errlist */
/* #undef HAVE_H_ERRLIST_DECLARATION */

/* Define if you have the `h_errno' variable. */
#define HAVE_H_ERRNO 1

/* define if your system declares h_errno */
#define HAVE_H_ERRNO_DECLARATION 1

/* Define if you have the `h_nerr' variable. */
#define HAVE_H_NERR 1

/* define if your system declares h_nerr */
/* #undef HAVE_H_NERR_DECLARATION */

/* Define if you have the <ifaddrs.h> header file. */
#define HAVE_IFADDRS_H 1

/* Define if you have the in6addr_loopback variable */
#define HAVE_IN6ADDR_LOOPBACK 1

/* Define if you have the `inet_aton' function. */
#define HAVE_INET_ATON 1

/* Define if you have the `inet_ntop' function. */
#define HAVE_INET_NTOP 1

/* Define if you have the `inet_pton' function. */
#define HAVE_INET_PTON 1

/* Define if you have the `initgroups' function. */
#define HAVE_INITGROUPS 1

/* Define if you have the `initstate' function. */
#define HAVE_INITSTATE 1

/* Define if you have the `innetgr' function. */
#define HAVE_INNETGR 1

/* Define if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have the <io.h> header file. */
/* #undef HAVE_IO_H */

/* Define if you have IPv6. */
#define HAVE_IPV6 1

/* Define if you have the `iruserok' function. */
#define HAVE_IRUSEROK 1

/* Define if you have the `issetugid' function. */
#define HAVE_ISSETUGID 1

/* Define if you have the <lastlog.h> header file. */
/* #undef HAVE_LASTLOG_H */

/* Define if you have the <libutil.h> header file. */
/* #undef HAVE_LIBUTIL_H */

/* Define if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if you have the <login.h> header file. */
/* #undef HAVE_LOGIN_H */

/* Define if you have the `logout' function. */
#define HAVE_LOGOUT 1

/* Define if you have the `logwtmp' function. */
#define HAVE_LOGWTMP 1

/* Define if the system has the type `long long'. */
#define HAVE_LONG_LONG 1

/* Define if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define if /bin/ls has a `-A' flag. */
#define HAVE_LS_A 1

/* Define if you have the <maillock.h> header file. */
/* #undef HAVE_MAILLOCK_H */

/* Define if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define if you have the `mktime' function. */
#define HAVE_MKTIME 1

/* Define if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* define if you have a ndbm library */
#define HAVE_NDBM 1

/* Define if you have the <ndbm.h> header file. */
#define HAVE_NDBM_H 1

/* Define if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define if you have the <netinet6/in6.h> header file. */
/* #undef HAVE_NETINET6_IN6_H */

/* Define if you have the <netinet6/in6_var.h> header file. */
#define HAVE_NETINET6_IN6_VAR_H 1

/* Define if you have the <netinet/in6.h> header file. */
/* #undef HAVE_NETINET_IN6_H */

/* Define if you have the <netinet/in6_machtypes.h> header file. */
/* #undef HAVE_NETINET_IN6_MACHTYPES_H */

/* Define if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define if you have the <netinet/in_systm.h> header file. */
#define HAVE_NETINET_IN_SYSTM_H 1

/* Define if you have the <netinet/ip.h> header file. */
#define HAVE_NETINET_IP_H 1

/* Define if you have the <netinet/tcp.h> header file. */
#define HAVE_NETINET_TCP_H 1

/* Define if you have the <net/if.h> header file. */
#define HAVE_NET_IF_H 1

/* Define if you have the <net/if_tun.h> header file. */
#define HAVE_NET_IF_TUN_H 1

/* Define if you have the <net/if_var.h> header file. */
/* #undef HAVE_NET_IF_VAR_H */

/* Define if NDBM really is DB (creates files *.db) */
#define HAVE_NEW_DB 1

/* Define if you have the `odm_initialize' function. */
/* #undef HAVE_ODM_INITIALIZE */

/* Define if you have the `on_exit' function. */
/* #undef HAVE_ON_EXIT */

/* Define if you have the `openpty' function. */
#define HAVE_OPENPTY 1

/* define to use openssl's libcrypto */
/* #undef HAVE_OPENSSL */

/* define if your system declares optarg */
#define HAVE_OPTARG_DECLARATION 1

/* define if your system declares opterr */
#define HAVE_OPTERR_DECLARATION 1

/* define if your system declares optind */
#define HAVE_OPTIND_DECLARATION 1

/* define if your system declares optopt */
#define HAVE_OPTOPT_DECLARATION 1

/* Define to enable basic OSF C2 support. */
/* #undef HAVE_OSFC2 */

/* Define if you have the `parsetos' function. */
/* #undef HAVE_PARSETOS */

/* Define if you have the <paths.h> header file. */
#define HAVE_PATHS_H 1

/* Define if you have the `pidfile' function. */
#define HAVE_PIDFILE 1

/* Define if you have the `ptsname' function. */
/* #undef HAVE_PTSNAME */

/* Define if you have the <pty.h> header file. */
/* #undef HAVE_PTY_H */

/* Define if you have the `putenv' function. */
#define HAVE_PUTENV 1

/* Define if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define if you have the `rand' function. */
#define HAVE_RAND 1

/* Define if you have the `random' function. */
#define HAVE_RANDOM 1

/* Define if you have the `rcmd' function. */
#define HAVE_RCMD 1

/* Define if you have a readline compatible library. */
#define HAVE_READLINE 1

/* Define if you have the `readv' function. */
#define HAVE_READV 1

/* Define if you have the `recvmsg' function. */
#define HAVE_RECVMSG 1

/* Define if you have the <resolv.h> header file. */
#define HAVE_RESOLV_H 1

/* Define if you have the `res_search' function. */
#define HAVE_RES_SEARCH 1

/* Define if you have the `revoke' function. */
#define HAVE_REVOKE 1

/* Define if you have the <rpcsvc/ypclnt.h> header file. */
#define HAVE_RPCSVC_YPCLNT_H 1

/* Define if you have the <sac.h> header file. */
/* #undef HAVE_SAC_H */

/* Define if the system has the type `sa_family_t'. */
#define HAVE_SA_FAMILY_T 1

/* Define if you have the <security/pam_modules.h> header file. */
/* #undef HAVE_SECURITY_PAM_MODULES_H */

/* Define if you have the `sendmsg' function. */
#define HAVE_SENDMSG 1

/* Define if you have the `setegid' function. */
#define HAVE_SETEGID 1

/* Define if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define if you have the `seteuid' function. */
#define HAVE_SETEUID 1

/* Define if you have the `setitimer' function. */
#define HAVE_SETITIMER 1

/* Define if you have the `setlim' function. */
/* #undef HAVE_SETLIM */

/* Define if you have the `setlogin' function. */
#define HAVE_SETLOGIN 1

/* Define if you have the `setpcred' function. */
/* #undef HAVE_SETPCRED */

/* Define if you have the `setpgid' function. */
#define HAVE_SETPGID 1

/* Define if you have the `setpriority' function. */
#define HAVE_SETPRIORITY 1

/* Define if you have the `setproctitle' function. */
#define HAVE_SETPROCTITLE 1

/* Define if you have the `setprogname' function. */
/* #undef HAVE_SETPROGNAME */

/* Define if you have the `setregid' function. */
#define HAVE_SETREGID 1

/* Define if you have the `setresgid' function. */
/* #undef HAVE_SETRESGID */

/* Define if you have the `setresuid' function. */
/* #undef HAVE_SETRESUID */

/* Define if you have the `setreuid' function. */
#define HAVE_SETREUID 1

/* Define if you have the `setsid' function. */
#define HAVE_SETSID 1

/* Define if you have the `setsockopt' function. */
#define HAVE_SETSOCKOPT 1

/* Define if you have the `setstate' function. */
#define HAVE_SETSTATE 1

/* Define if you have the `setutent' function. */
/* #undef HAVE_SETUTENT */

/* Define if you have the `sgi_getcapabilitybyname' function. */
/* #undef HAVE_SGI_GETCAPABILITYBYNAME */

/* Define if you have the <shadow.h> header file. */
/* #undef HAVE_SHADOW_H */

/* Define if you have the <siad.h> header file. */
/* #undef HAVE_SIAD_H */

/* Define if SIAENTITY has field ouid. */
/* #undef HAVE_SIAENTITY_OUID */

/* Define if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* define if you have a working snprintf */
#define HAVE_SNPRINTF 1

/* Define if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define if the system has the type `socklen_t'. */
#define HAVE_SOCKLEN_T 1

/* Define if you have the `srandomdev' function. */
#define HAVE_SRANDOMDEV 1

/* Define if you have the <standards.h> header file. */
/* #undef HAVE_STANDARDS_H */

/* Define if you have the <stdint.h> header file. */
/* #undef HAVE_STDINT_H */

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Huh? */
/* #undef HAVE_STRANGE_INT8_T */

/* Define if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define if you have the `strftime' function. */
#define HAVE_STRFTIME 1

/* Define if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the `strlcat' function. */
#define HAVE_STRLCAT 1

/* Define if you have the `strlcpy' function. */
#define HAVE_STRLCPY 1

/* Define if you have the `strlwr' function. */
/* #undef HAVE_STRLWR */

/* Define if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define if you have the `strndup' function. */
/* #undef HAVE_STRNDUP */

/* Define if you have the `strnlen' function. */
/* #undef HAVE_STRNLEN */

/* Define if you have the <stropts.h> header file. */
/* #undef HAVE_STROPTS_H */

/* Define if you have the `strptime' function. */
#define HAVE_STRPTIME 1

/* Define if you have the `strsep' function. */
#define HAVE_STRSEP 1

/* Define if you have the `strsep_copy' function. */
/* #undef HAVE_STRSEP_COPY */

/* Define if you have the `strsvis' function. */
/* #undef HAVE_STRSVIS */

/* Define if you have the `strtok_r' function. */
#define HAVE_STRTOK_R 1

/* Define if the system has the type `struct addrinfo'. */
#define HAVE_STRUCT_ADDRINFO 1

/* Define if the system has the type `struct ifaddrs'. */
#define HAVE_STRUCT_IFADDRS 1

/* Define if the system has the type `struct sockaddr'. */
#define HAVE_STRUCT_SOCKADDR 1

/* Define if struct sockaddr has field sa_len. */
#define HAVE_STRUCT_SOCKADDR_SA_LEN 1

/* Define if the system has the type `struct sockaddr_storage'. */
#define HAVE_STRUCT_SOCKADDR_STORAGE 1

/* define if you have struct spwd */
/* #undef HAVE_STRUCT_SPWD */

/* Define if `st_blksize' is member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_BLKSIZE 1

/* Define if struct tm has field tm_gmtoff. */
#define HAVE_STRUCT_TM_TM_GMTOFF 1

/* Define if struct tm has field tm_zone. */
#define HAVE_STRUCT_TM_TM_ZONE 1

/* Define if struct utmpx has field ut_exit. */
/* #undef HAVE_STRUCT_UTMPX_UT_EXIT */

/* Define if struct utmpx has field ut_syslen. */
/* #undef HAVE_STRUCT_UTMPX_UT_SYSLEN */

/* Define if struct utmp has field ut_addr. */
/* #undef HAVE_STRUCT_UTMP_UT_ADDR */

/* Define if struct utmp has field ut_host. */
#define HAVE_STRUCT_UTMP_UT_HOST 1

/* Define if struct utmp has field ut_id. */
/* #undef HAVE_STRUCT_UTMP_UT_ID */

/* Define if struct utmp has field ut_pid. */
/* #undef HAVE_STRUCT_UTMP_UT_PID */

/* Define if struct utmp has field ut_type. */
/* #undef HAVE_STRUCT_UTMP_UT_TYPE */

/* Define if struct utmp has field ut_user. */
/* #undef HAVE_STRUCT_UTMP_UT_USER */

/* define if struct winsize is declared in sys/termios.h */
#define HAVE_STRUCT_WINSIZE 1

/* Define if you have the `strunvis' function. */
#define HAVE_STRUNVIS 1

/* Define if you have the `strupr' function. */
/* #undef HAVE_STRUPR */

/* Define if you have the `strvis' function. */
#define HAVE_STRVIS 1

/* Define if you have the `strvisx' function. */
#define HAVE_STRVISX 1

/* Define if you have the `svis' function. */
/* #undef HAVE_SVIS */

/* Define if you have the `swab' function. */
#define HAVE_SWAB 1

/* Define if you have the `sysconf' function. */
#define HAVE_SYSCONF 1

/* Define if you have the `sysctl' function. */
#define HAVE_SYSCTL 1

/* Define if you have the `syslog' function. */
#define HAVE_SYSLOG 1

/* Define if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define if you have the <sys/bitypes.h> header file. */
/* #undef HAVE_SYS_BITYPES_H */

/* Define if you have the <sys/bswap.h> header file. */
/* #undef HAVE_SYS_BSWAP_H */

/* Define if you have the <sys/capability.h> header file. */
/* #undef HAVE_SYS_CAPABILITY_H */

/* Define if you have the <sys/category.h> header file. */
/* #undef HAVE_SYS_CATEGORY_H */

/* Define if you have the <sys/file.h> header file. */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/filio.h> header file. */
#define HAVE_SYS_FILIO_H 1

/* Define if you have the <sys/ioccom.h> header file. */
#define HAVE_SYS_IOCCOM_H 1

/* Define if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/locking.h> header file. */
/* #undef HAVE_SYS_LOCKING_H */

/* Define if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/proc.h> header file. */
#define HAVE_SYS_PROC_H 1

/* Define if you have the <sys/ptyio.h> header file. */
/* #undef HAVE_SYS_PTYIO_H */

/* Define if you have the <sys/ptyvar.h> header file. */
/* #undef HAVE_SYS_PTYVAR_H */

/* Define if you have the <sys/pty.h> header file. */
/* #undef HAVE_SYS_PTY_H */

/* Define if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/sockio.h> header file. */
#define HAVE_SYS_SOCKIO_H 1

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/stream.h> header file. */
/* #undef HAVE_SYS_STREAM_H */

/* Define if you have the <sys/stropts.h> header file. */
/* #undef HAVE_SYS_STROPTS_H */

/* Define if you have the <sys/strtty.h> header file. */
/* #undef HAVE_SYS_STRTTY_H */

/* Define if you have the <sys/str_tty.h> header file. */
/* #undef HAVE_SYS_STR_TTY_H */

/* Define if you have the <sys/syscall.h> header file. */
#define HAVE_SYS_SYSCALL_H 1

/* Define if you have the <sys/sysctl.h> header file. */
#define HAVE_SYS_SYSCTL_H 1

/* Define if you have the <sys/termio.h> header file. */
/* #undef HAVE_SYS_TERMIO_H */

/* Define if you have the <sys/timeb.h> header file. */
#define HAVE_SYS_TIMEB_H 1

/* Define if you have the <sys/times.h> header file. */
#define HAVE_SYS_TIMES_H 1

/* Define if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/tty.h> header file. */
#define HAVE_SYS_TTY_H 1

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/uio.h> header file. */
#define HAVE_SYS_UIO_H 1

/* Define if you have the <sys/un.h> header file. */
#define HAVE_SYS_UN_H 1

/* Define if you have the <sys/utsname.h> header file. */
#define HAVE_SYS_UTSNAME_H 1

/* Define if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define if you have the <termcap.h> header file. */
#define HAVE_TERMCAP_H 1

/* Define if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define if you have the <termio.h> header file. */
/* #undef HAVE_TERMIO_H */

/* Define if you have the <term.h> header file. */
#define HAVE_TERM_H 1

/* Define if you have the `tgetent' function. */
#define HAVE_TGETENT 1

/* Define if you have the `timezone' variable. */
#define HAVE_TIMEZONE 1

/* define if your system declares timezone */
#define HAVE_TIMEZONE_DECLARATION 1

/* Define if you have the <tmpdir.h> header file. */
/* #undef HAVE_TMPDIR_H */

/* Define if you have the <ttyent.h> header file. */
#define HAVE_TTYENT_H 1

/* Define if you have the `ttyname' function. */
#define HAVE_TTYNAME 1

/* Define if you have the `ttyslot' function. */
#define HAVE_TTYSLOT 1

/* Define if you have the <udb.h> header file. */
/* #undef HAVE_UDB_H */

/* Define if you have the `ulimit' function. */
/* #undef HAVE_ULIMIT */

/* Define if you have the <ulimit.h> header file. */
/* #undef HAVE_ULIMIT_H */

/* Define if you have the `uname' function. */
#define HAVE_UNAME 1

/* Define if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if you have the `unlockpt' function. */
/* #undef HAVE_UNLOCKPT */

/* Define if you have the `unsetenv' function. */
#define HAVE_UNSETENV 1

/* Define if you have the `unvis' function. */
#define HAVE_UNVIS 1

/* Define if you have the <userconf.h> header file. */
/* #undef HAVE_USERCONF_H */

/* Define if you have the <userpw.h> header file. */
/* #undef HAVE_USERPW_H */

/* Define if you have the <usersec.h> header file. */
/* #undef HAVE_USERSEC_H */

/* Define if you have the <util.h> header file. */
#define HAVE_UTIL_H 1

/* Define if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* Define if you have the <utmpx.h> header file. */
/* #undef HAVE_UTMPX_H */

/* Define if you have the <utmp.h> header file. */
#define HAVE_UTMP_H 1

/* Define if you have the `vasnprintf' function. */
/* #undef HAVE_VASNPRINTF */

/* Define if you have the `vasprintf' function. */
#define HAVE_VASPRINTF 1

/* Define if you have the `verr' function. */
#define HAVE_VERR 1

/* Define if you have the `verrx' function. */
#define HAVE_VERRX 1

/* Define if you have the `vhangup' function. */
/* #undef HAVE_VHANGUP */

/* Define if you have the `vis' function. */
#define HAVE_VIS 1

/* Define if you have the <vis.h> header file. */
#define HAVE_VIS_H 1

/* define if you have a working vsnprintf */
#define HAVE_VSNPRINTF 1

/* Define if you have the `vsyslog' function. */
#define HAVE_VSYSLOG 1

/* Define if you have the `vwarn' function. */
#define HAVE_VWARN 1

/* Define if you have the `vwarnx' function. */
#define HAVE_VWARNX 1

/* Define if you have the <wait.h> header file. */
/* #undef HAVE_WAIT_H */

/* Define if you have the `warn' function. */
#define HAVE_WARN 1

/* Define if you have the `warnx' function. */
#define HAVE_WARNX 1

/* Define if you have the `writev' function. */
#define HAVE_WRITEV 1

/* define if struct winsize has ws_xpixel */
#define HAVE_WS_XPIXEL 1

/* define if struct winsize has ws_ypixel */
#define HAVE_WS_YPIXEL 1

/* Define if you have the `XauFileName' function. */
#define HAVE_XAUFILENAME 1

/* Define if you have the `XauReadAuth' function. */
#define HAVE_XAUREADAUTH 1

/* Define if you have the `XauWriteAuth' function. */
#define HAVE_XAUWRITEAUTH 1

/* Define if you have the `yp_get_default_domain' function. */
#define HAVE_YP_GET_DEFAULT_DOMAIN 1

/* Define if you have the `_getpty' function. */
/* #undef HAVE__GETPTY */

/* Define if you have the `_scrsize' function. */
/* #undef HAVE__SCRSIZE */

/* Define if you have the `_setsid' function. */
/* #undef HAVE__SETSID */

/* Define if you have the `_stricmp' function. */
/* #undef HAVE__STRICMP */

/* define if your compiler has __attribute__ */
#define HAVE___ATTRIBUTE__ 1

/* Define if you have the `__progname' variable. */
#define HAVE___PROGNAME 1

/* define if your system declares __progname */
/* #undef HAVE___PROGNAME_DECLARATION */

/* Define if you have the hesiod package. */
/* #undef HESIOD */

/* Define this to the path of the mail spool directory. */
/* #undef KRB4_MAILDIR */

/* Define to enable old kdestroy behavior. */
/* #undef LEGACY_KDESTROY */

/* Define if you want to match subdomains. */
/* #undef MATCH_SUBDOMAINS */

/* Define this to the location of the master key. */
/* #undef MKEYFILE */

/* define if the system is missing a prototype for asnprintf() */
/* #undef NEED_ASNPRINTF_PROTO */

/* define if the system is missing a prototype for asprintf() */
/* #undef NEED_ASPRINTF_PROTO */

/* define if the system is missing a prototype for crypt() */
/* #undef NEED_CRYPT_PROTO */

/* define if the system is missing a prototype for fclose() */
/* #undef NEED_FCLOSE_PROTO */

/* define if the system is missing a prototype for gethostname() */
/* #undef NEED_GETHOSTNAME_PROTO */

/* define if the system is missing a prototype for getusershell() */
/* #undef NEED_GETUSERSHELL_PROTO */

/* define if the system is missing a prototype for glob() */
/* #undef NEED_GLOB_PROTO */

/* define if the system is missing a prototype for hstrerror() */
/* #undef NEED_HSTRERROR_PROTO */

/* define if the system is missing a prototype for inet_aton() */
/* #undef NEED_INET_ATON_PROTO */

/* define if the system is missing a prototype for mkstemp() */
/* #undef NEED_MKSTEMP_PROTO */

/* define if the system is missing a prototype for setenv() */
/* #undef NEED_SETENV_PROTO */

/* define if the system is missing a prototype for snprintf() */
/* #undef NEED_SNPRINTF_PROTO */

/* define if the system is missing a prototype for strsep() */
/* #undef NEED_STRSEP_PROTO */

/* define if the system is missing a prototype for strtok_r() */
/* #undef NEED_STRTOK_R_PROTO */

/* define if the system is missing a prototype for unsetenv() */
/* #undef NEED_UNSETENV_PROTO */

/* define if the system is missing a prototype for utime() */
/* #undef NEED_UTIME_PROTO */

/* define if the system is missing a prototype for vasnprintf() */
/* #undef NEED_VASNPRINTF_PROTO */

/* define if the system is missing a prototype for vasprintf() */
/* #undef NEED_VASPRINTF_PROTO */

/* define if the system is missing a prototype for vsnprintf() */
/* #undef NEED_VSNPRINTF_PROTO */

/* Define if you don't wan't support for AFS. */
/* #undef NO_AFS */

/* Define if you don't want to use mmap. */
/* #undef NO_MMAP */

/* define if prototype of openlog is compatible with void openlog(const char
   *, int, int) */
#define OPENLOG_PROTO_COMPATIBLE 1

/* Define if you want OTP support in applications. */
#define OTP 1

/* Name of package */
#define PACKAGE "krb4"

/* Define if getlogin has POSIX flavour (and not BSD). */
/* #undef POSIX_GETLOGIN */

/* Define if getpwnam_r has POSIX flavour. */
/* #undef POSIX_GETPWNAM_R */

/* Define to enable new master key code. */
/* #undef RANDOM_MKEY */

/* Define if you have the readline package. */
/* #undef READLINE */

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define if you have the socks package. */
/* #undef SOCKS */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
        STACK_DIRECTION > 0 => grows toward higher addresses
        STACK_DIRECTION < 0 => grows toward lower addresses
        STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if you have working stream ptys. */
/* #undef STREAMSPTY */

/* Define if `sys_siglist' is declared by <signal.h> or <unistd.h>. */
#define SYS_SIGLIST_DECLARED 1

/* Define to what version of SunOS you are running. */
/* #undef SunOS */

/* Define if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Version number of package */
#define VERSION "1.1.1"

/* Define if RETSIGTYPE == void. */
#define VOID_RETSIGTYPE 1

/* Define if your processor stores words with the most significant byte first
   (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define if the X Window System is missing or not being used. */
/* #undef X_DISPLAY_MISSING */

/* Define if `lex' declares `yytext' as a `char *' by default, not a `char[]'.
   */
#define YYTEXT_POINTER 1

/* Define if you need to in order for stat and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
/* #undef inline */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define this to what the type sig_atomic_t should be. */
/* #undef sig_atomic_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define this to what the type ssize_t should be. */
/* #undef ssize_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */

#if defined(HAVE_SGTTY_H) && defined(__NeXT__)
#define SGTTY
#endif

#ifdef VOID_RETSIGTYPE
#define SIGRETURN(x) return
#else
#define SIGRETURN(x) return (RETSIGTYPE)(x)
#endif

#ifdef BROKEN_REALLOC
#define realloc(X, Y) isoc_realloc((X), (Y))
#define isoc_realloc(X, Y) ((X) ? realloc((X), (Y)) : malloc(Y))
#endif

/* This for compat with heimdal (or something) */
#define KRB_PUT_INT(f, t, l, s) krb_put_int((f), (t), (l), (s))

#define HAVE_KRB_ENABLE_DEBUG 1

#define HAVE_KRB_DISABLE_DEBUG 1

#define HAVE_KRB_GET_OUR_IP_FOR_REALM 1

/*
 * Set ORGANIZATION to be the desired organization string printed
 * by the 'kinit' program.  It may have spaces.
 */
#define ORGANIZATION "eBones International"

/* ftp stuff -------------------------------------------------- */

#define KERBEROS

/* telnet stuff ----------------------------------------------- */

/* define this if you have kerberos 4 */
#define KRB4 1

/* define this if you want encryption */
#define ENCRYPTION 1

/* define this if you want authentication */
#define AUTHENTICATION 1

#if defined(ENCRYPTION) && !defined(AUTHENTICATION)
#define AUTHENTICATION 1
#endif

/* Set this if you want des encryption */
#define DES_ENCRYPTION 1

/* Set this to the default system lead string for telnetd
 * can contain %-escapes: %s=sysname, %m=machine, %r=os-release
 * %v=os-version, %t=tty, %h=hostname, %d=date and time
 */
/* #undef USE_IM */

/* define this if you want diagnostics in telnetd */
#define DIAGNOSTICS 1

/* define this if you want support for broken ENV_{VALUE,VAR} systems  */
/* #undef ENV_HACK */

/*  */
#define OLD_ENVIRON 1

/* Used with login -p */
/* #undef LOGIN_ARGS */

/* set this to a sensible login */
#ifndef LOGIN_PATH
#define LOGIN_PATH BINDIR "/login"
#endif


/* ------------------------------------------------------------ */

/* Temporary fixes for krb_{rd,mk}_safe */
#define DES_QUAD_GUESS 0
#define DES_QUAD_NEW 1
#define DES_QUAD_OLD 2

/*
 * All these are system-specific defines that I would rather not have at all.
 */

/*
 * AIX braindamage!
 */
#if _AIX
#define _ALL_SOURCE
/* XXX this is gross, but kills about a gazillion warnings */
struct ether_addr;
struct sockaddr;
struct sockaddr_dl;
struct sockaddr_in;
#endif

#if defined(__sgi) || defined(sgi)
#if defined(__SYSTYPE_SVR4) || defined(_SYSTYPE_SVR4)
#define IRIX 5
#else
#define IRIX 4
#endif
#endif

/* IRIX 4 braindamage */
#if IRIX == 4 && !defined(__STDC__)
#define __STDC__ 0
#endif

/*
 * Defining this enables lots of useful (and used) extensions on
 * glibc-based systems such as Linux
 */

#define _GNU_SOURCE

/* some strange OS/2 stuff.  From <d96-mst@nada.kth.se> */

#ifdef __EMX__
#define _EMX_TCPIP
#define MAIL_USE_SYSTEM_LOCK
#endif

#ifdef ROKEN_RENAME
#include "roken_rename.h"
#endif

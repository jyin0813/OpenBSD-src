#!/bin/sh
#
# This file was produced by running the Configure script. It holds all the
# definitions figured out by Configure. Should you modify one of these values,
# do not forget to propagate your changes by running "Configure -der". You may
# instead choose to run each of the .SH files by yourself, or "Configure -S".
#

# Package name      : perl5
# Source directory  : .
# Configuration time: Tue Jul 21 10:03:27 EDT 1998
# Configured by     : doughera
# Target system     : linux fractal 2.0.34 #1 tue jun 23 10:09:17 edt 1998 i686 unknown 

Author=''
Date='$Date'
Header=''
Id='$Id'
Locker=''
Log='$Log'
Mcc='Mcc'
RCSfile='$RCSfile'
Revision='$Revision'
Source=''
State=''
_a='.a'
_exe=''
_o='.o'
afs='false'
alignbytes='4'
ansi2knr=''
aphostname=''
apiversion='5.005'
ar='ar'
archlib='/opt/perl/lib/5.005/i686-linux-thread'
archlibexp='/opt/perl/lib/5.005/i686-linux-thread'
archname='i686-linux-thread'
archobjs=''
awk='awk'
baserev='5.0'
bash=''
bin='/opt/perl/bin'
binexp='/opt/perl/bin'
bison=''
byacc='byacc'
byteorder='1234'
c=''
castflags='0'
cat='cat'
cc='cc'
cccdlflags='-fpic'
ccdlflags='-rdynamic'
ccflags='-D_REENTRANT -Dbool=char -DHAS_BOOL -I/usr/local/include'
cf_by='doughera'
cf_email='yourname@yourhost.yourplace.com'
cf_time='Tue Jul 21 10:03:27 EDT 1998'
chgrp=''
chmod=''
chown=''
clocktype='clock_t'
comm='comm'
compress=''
contains='grep'
cp='cp'
cpio=''
cpp='cpp'
cpp_stuff='42'
cppflags='-D_REENTRANT -Dbool=char -DHAS_BOOL -I/usr/local/include'
cpplast='-'
cppminus='-'
cpprun='cc -E'
cppstdin='cc -E'
cryptlib=''
csh='csh'
d_Gconvert='gcvt((x),(n),(b))'
d_access='define'
d_alarm='define'
d_archlib='define'
d_attribut='define'
d_bcmp='define'
d_bcopy='define'
d_bsd='undef'
d_bsdgetpgrp='undef'
d_bsdsetpgrp='undef'
d_bzero='define'
d_casti32='undef'
d_castneg='define'
d_charvspr='undef'
d_chown='define'
d_chroot='define'
d_chsize='undef'
d_closedir='define'
d_const='define'
d_crypt='define'
d_csh='define'
d_cuserid='define'
d_dbl_dig='define'
d_difftime='define'
d_dirnamlen='undef'
d_dlerror='define'
d_dlopen='define'
d_dlsymun='undef'
d_dosuid='undef'
d_dup2='define'
d_endgrent='define'
d_endhent='define'
d_endnent='define'
d_endpent='define'
d_endpwent='define'
d_endsent='define'
d_eofnblk='define'
d_eunice='undef'
d_fchmod='define'
d_fchown='define'
d_fcntl='define'
d_fd_macros='define'
d_fd_set='define'
d_fds_bits='define'
d_fgetpos='define'
d_flexfnam='define'
d_flock='define'
d_fork='define'
d_fpathconf='define'
d_fsetpos='define'
d_ftime='undef'
d_getgrent='define'
d_getgrps='define'
d_gethbyaddr='define'
d_gethbyname='define'
d_gethent='define'
d_gethname='undef'
d_gethostprotos='define'
d_getlogin='define'
d_getnbyaddr='define'
d_getnbyname='define'
d_getnent='define'
d_getnetprotos='define'
d_getpbyname='define'
d_getpbynumber='define'
d_getpent='define'
d_getpgid='define'
d_getpgrp2='undef'
d_getpgrp='define'
d_getppid='define'
d_getprior='define'
d_getprotoprotos='define'
d_getpwent='define'
d_getsbyname='define'
d_getsbyport='define'
d_getsent='define'
d_getservprotos='define'
d_gettimeod='define'
d_gnulibc='define'
d_grpasswd='define'
d_htonl='define'
d_index='undef'
d_inetaton='define'
d_isascii='define'
d_killpg='define'
d_lchown='undef'
d_link='define'
d_locconv='define'
d_lockf='define'
d_longdbl='define'
d_longlong='define'
d_lstat='define'
d_mblen='define'
d_mbstowcs='define'
d_mbtowc='define'
d_memcmp='define'
d_memcpy='define'
d_memmove='define'
d_memset='define'
d_mkdir='define'
d_mkfifo='define'
d_mktime='define'
d_msg='define'
d_msgctl='define'
d_msgget='define'
d_msgrcv='define'
d_msgsnd='define'
d_mymalloc='undef'
d_nice='define'
d_oldpthreads='undef'
d_oldsock='undef'
d_open3='define'
d_pathconf='define'
d_pause='define'
d_phostname='undef'
d_pipe='define'
d_poll='define'
d_portable='define'
d_pthread_yield='undef'
d_pthreads_created_joinable='define'
d_pwage='undef'
d_pwchange='undef'
d_pwclass='undef'
d_pwcomment='undef'
d_pwexpire='undef'
d_pwgecos='define'
d_pwquota='undef'
d_pwpasswd='define'
d_readdir='define'
d_readlink='define'
d_rename='define'
d_rewinddir='define'
d_rmdir='define'
d_safebcpy='define'
d_safemcpy='undef'
d_sanemcmp='define'
d_sched_yield='define'
d_seekdir='define'
d_select='define'
d_sem='define'
d_semctl='define'
d_semctl_semid_ds='define'
d_semctl_semun='define'
d_semget='define'
d_semop='define'
d_setegid='define'
d_seteuid='define'
d_setgrent='define'
d_setgrps='define'
d_sethent='define'
d_setlinebuf='define'
d_setlocale='define'
d_setnent='define'
d_setpent='define'
d_setpgid='define'
d_setpgrp2='undef'
d_setpgrp='define'
d_setprior='define'
d_setpwent='define'
d_setregid='define'
d_setresgid='undef'
d_setresuid='undef'
d_setreuid='define'
d_setrgid='undef'
d_setruid='undef'
d_setsent='define'
d_setsid='define'
d_setvbuf='define'
d_sfio='undef'
d_shm='define'
d_shmat='define'
d_shmatprototype='define'
d_shmctl='define'
d_shmdt='define'
d_shmget='define'
d_sigaction='define'
d_sigsetjmp='define'
d_socket='define'
d_sockpair='define'
d_statblks='undef'
d_stdio_cnt_lval='undef'
d_stdio_ptr_lval='define'
d_stdiobase='define'
d_stdstdio='define'
d_strchr='define'
d_strcoll='define'
d_strctcpy='define'
d_strerrm='strerror(e)'
d_strerror='define'
d_strtod='define'
d_strtol='define'
d_strtoul='define'
d_strxfrm='define'
d_suidsafe='undef'
d_symlink='define'
d_syscall='define'
d_sysconf='define'
d_sysernlst=''
d_syserrlst='define'
d_system='define'
d_tcgetpgrp='define'
d_tcsetpgrp='define'
d_telldir='define'
d_time='define'
d_times='define'
d_truncate='define'
d_tzname='define'
d_umask='define'
d_uname='define'
d_union_semun='define'
d_vfork='undef'
d_void_closedir='undef'
d_voidsig='define'
d_voidtty=''
d_volatile='define'
d_vprintf='define'
d_wait4='define'
d_waitpid='define'
d_wcstombs='define'
d_wctomb='define'
d_xenix='undef'
date='date'
db_hashtype='u_int32_t'
db_prefixtype='size_t'
defvoidused='15'
direntrytype='struct dirent'
dlext='so'
dlsrc='dl_dlopen.xs'
doublesize='8'
dynamic_ext='B DB_File Data/Dumper Fcntl GDBM_File IO IPC/SysV NDBM_File ODBM_File Opcode POSIX SDBM_File Socket Thread attrs re'
eagain='EAGAIN'
ebcdic='undef'
echo='echo'
egrep='egrep'
emacs=''
eunicefix=':'
exe_ext=''
expr='expr'
extensions='B DB_File Data/Dumper Fcntl GDBM_File IO IPC/SysV NDBM_File ODBM_File Opcode POSIX SDBM_File Socket Thread attrs re Errno'
find='find'
firstmakefile='makefile'
flex=''
fpostype='fpos_t'
freetype='void'
full_csh='/bin/csh'
full_sed='/bin/sed'
gccversion='2.7.2.3'
gidtype='gid_t'
glibpth='/usr/shlib  /shlib /lib/pa1.1 /usr/lib/large /lib /usr/lib /usr/lib/386 /lib/386 /lib/large /usr/lib/small /lib/small /usr/ccs/lib /usr/ucblib /usr/local/lib '
grep='grep'
groupcat='cat /etc/group'
groupstype='gid_t'
gzip='gzip'
h_fcntl='false'
h_sysfile='true'
hint='recommended'
hostcat='cat /etc/hosts'
huge=''
i_arpainet='define'
i_bsdioctl=''
i_db='define'
i_dbm='define'
i_dirent='define'
i_dld='undef'
i_dlfcn='define'
i_fcntl='undef'
i_float='define'
i_gdbm='define'
i_grp='define'
i_limits='define'
i_locale='define'
i_malloc='define'
i_math='define'
i_memory='undef'
i_ndbm='define'
i_netdb='define'
i_neterrno='undef'
i_niin='define'
i_pwd='define'
i_rpcsvcdbm='undef'
i_sfio='undef'
i_sgtty='undef'
i_stdarg='define'
i_stddef='define'
i_stdlib='define'
i_string='define'
i_sysdir='define'
i_sysfile='define'
i_sysfilio='undef'
i_sysin='undef'
i_sysioctl='define'
i_sysndir='undef'
i_sysparam='define'
i_sysresrc='define'
i_sysselct='define'
i_syssockio=''
i_sysstat='define'
i_systime='define'
i_systimek='undef'
i_systimes='define'
i_systypes='define'
i_sysun='define'
i_syswait='define'
i_termio='undef'
i_termios='define'
i_time='undef'
i_unistd='define'
i_utime='define'
i_values='define'
i_varargs='undef'
i_varhdr='stdarg.h'
i_vfork='undef'
incpath=''
inews=''
installarchlib='/opt/perl/lib/5.005/i686-linux-thread'
installbin='/opt/perl/bin'
installman1dir='/opt/perl/man/man1'
installman3dir='/opt/perl/man/man3'
installprivlib='/opt/perl/lib/5.005'
installscript='/opt/perl/script'
installsitearch='/opt/perl/lib/site_perl/5.005/i686-linux-thread'
installsitelib='/opt/perl/lib/site_perl/5.005'
intsize='4'
known_extensions='B DB_File Data/Dumper Fcntl GDBM_File IO IPC/SysV NDBM_File ODBM_File Opcode POSIX SDBM_File Socket Thread attrs re'
ksh=''
large=''
ld='cc'
lddlflags='-shared -L/usr/local/lib'
ldflags=' -L/usr/local/lib'
less='less'
lib_ext='.a'
libc=''
libperl='libperl.a'
libpth='/usr/local/lib /lib /usr/lib'
libs='-lnsl -lndbm -lgdbm -ldbm -ldb -ldl -lm -lpthread -lc -lposix -lcrypt'
libswanted='sfio socket inet nsl nm ndbm gdbm dbm db malloc dl dld ld sun m pthread c cposix posix ndir dir crypt ucb BSD PW x'
line='line'
lint=''
lkflags=''
ln='ln'
lns='/bin/ln -s'
locincpth='/usr/local/include /opt/local/include /usr/gnu/include /opt/gnu/include /usr/GNU/include /opt/GNU/include'
loclibpth='/usr/local/lib /opt/local/lib /usr/gnu/lib /opt/gnu/lib /usr/GNU/lib /opt/GNU/lib'
longdblsize='12'
longlongsize='8'
longsize='4'
lp=''
lpr=''
ls='ls'
lseektype='off_t'
mail=''
mailx=''
make='make'
make_set_make='#'
mallocobj=''
mallocsrc=''
malloctype='void *'
man1dir='/opt/perl/man/man1'
man1direxp='/opt/perl/man/man1'
man1ext='1'
man3dir='/opt/perl/man/man3'
man3direxp='/opt/perl/man/man3'
man3ext='3'
medium=''
mips=''
mips_type=''
mkdir='mkdir'
models='none'
modetype='mode_t'
more='more'
mv=''
myarchname='i686-linux'
mydomain='.yourplace.com'
myhostname='yourhost'
myuname='linux fractal 2.0.34 #1 tue jun 23 10:09:17 edt 1998 i686 unknown '
n='-n'
netdb_hlen_type='int'
netdb_host_type='const char *'
netdb_name_type='const char *'
netdb_net_type='unsigned long'
nm='nm'
nm_opt=''
nm_so_opt='--dynamic'
nonxs_ext='Errno'
nroff='nroff'
o_nonblock='O_NONBLOCK'
obj_ext='.o'
optimize='-O'
orderlib='false'
osname='linux'
osvers='2.0.34'
package='perl5'
pager='/usr/bin/less'
passcat='cat /etc/passwd'
patchlevel='5'
path_sep=':'
perl='perl'
perladmin='yourname@yourhost.yourplace.com'
perlpath='/opt/perl/bin/perl'
pg='pg'
phostname=''
pidtype='pid_t'
plibpth=''
pmake=''
pr=''
prefix='/opt/perl'
prefixexp='/opt/perl'
privlib='/opt/perl/lib/5.005'
privlibexp='/opt/perl/lib/5.005'
prototype='define'
ptrsize='4'
randbits='31'
ranlib=':'
rd_nodata='-1'
rm='rm'
rmail=''
runnm='false'
scriptdir='/opt/perl/script'
scriptdirexp='/opt/perl/script'
sed='sed'
selecttype='fd_set *'
sendmail='sendmail'
sh='/bin/sh'
shar=''
sharpbang='#!'
shmattype='void *'
shortsize='2'
shrpenv=''
shsharp='true'
sig_name='ZERO HUP INT QUIT ILL TRAP ABRT BUS FPE KILL USR1 SEGV USR2 PIPE ALRM TERM STKFLT CHLD CONT STOP TSTP TTIN TTOU URG XCPU XFSZ VTALRM PROF WINCH IO PWR UNUSED IOT CLD POLL '
sig_name_init='"ZERO", "HUP", "INT", "QUIT", "ILL", "TRAP", "ABRT", "BUS", "FPE", "KILL", "USR1", "SEGV", "USR2", "PIPE", "ALRM", "TERM", "STKFLT", "CHLD", "CONT", "STOP", "TSTP", "TTIN", "TTOU", "URG", "XCPU", "XFSZ", "VTALRM", "PROF", "WINCH", "IO", "PWR", "UNUSED", "IOT", "CLD", "POLL", 0'
sig_num='0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 6, 17, 29, 0'
signal_t='void'
sitearch='/opt/perl/lib/site_perl/5.005/i686-linux-thread'
sitearchexp='/opt/perl/lib/site_perl/5.005/i686-linux-thread'
sitelib='/opt/perl/lib/site_perl/5.005'
sitelibexp='/opt/perl/lib/site_perl/5.005'
sizetype='size_t'
sleep=''
smail=''
small=''
so='so'
sockethdr=''
socketlib=''
sort='sort'
spackage='Perl5'
spitshell='cat'
split=''
src='.'
ssizetype='ssize_t'
startperl='#!/opt/perl/bin/perl'
startsh='#!/bin/sh'
static_ext=' '
stdchar='char'
stdio_base='((fp)->_IO_read_base)'
stdio_bufsiz='((fp)->_IO_read_end - (fp)->_IO_read_base)'
stdio_cnt='((fp)->_IO_read_end - (fp)->_IO_read_ptr)'
stdio_filbuf=''
stdio_ptr='((fp)->_IO_read_ptr)'
strings='/usr/include/string.h'
submit=''
subversion='0'
sysman='/usr/man/man1'
tail=''
tar=''
tbl=''
tee='tee'
test='test'
timeincl='/usr/include/sys/time.h '
timetype='time_t'
touch='touch'
tr='tr'
trnl='\n'
troff=''
uidtype='uid_t'
uname='uname'
uniq='uniq'
usedl='define'
usemymalloc='n'
usenm='false'
useopcode='true'
useperlio='undef'
useposix='true'
usesfio='false'
useshrplib='false'
usethreads='define'
usevfork='false'
usrinc='/usr/include'
uuname=''
version='5.005'
vi=''
voidflags='15'
xlibpth='/usr/lib/386 /lib/386'
zcat=''
zip='zip'
# Configure command line arguments.
config_arg0='Configure'
config_args='-Dprefix=/opt/perl -Doptimize=-O -Dusethreads -Dcf_by=yourname -Dcf_email=yourname@yourhost.yourplace.com -Dperladmin=yourname@yourhost.yourplace.com -Dmydomain=.yourplace.com -Dmyhostname=yourhost -dE'
config_argc=9
config_arg1='-Dprefix=/opt/perl'
config_arg2='-Doptimize=-O'
config_arg3='-Dusethreads'
config_arg4='-Dcf_by=yourname'
config_arg5='-Dcf_email=yourname@yourhost.yourplace.com'
config_arg6='-Dperladmin=yourname@yourhost.yourplace.com'
config_arg7='-Dmydomain=.yourplace.com'
config_arg8='-Dmyhostname=yourhost'
config_arg9='-dE'
PATCHLEVEL=5
SUBVERSION=0
CONFIG=true

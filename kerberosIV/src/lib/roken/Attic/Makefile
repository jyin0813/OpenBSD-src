# Generated automatically from Makefile.in by configure.
#
# $KTH: Makefile.in,v 1.73.2.1 2000/06/23 04:37:43 assar Exp $
#

SHELL = /bin/sh

srcdir = .

CC	= gcc
LINK = $(CC)
CPP	= gcc -E
AR	= ar
RANLIB	= ranlib
DEFS	= -DHAVE_CONFIG_H
CFLAGS	=  -g -O2 $(WFLAGS)
WFLAGS	= 
AWK	= nawk

INSTALL = /usr/bin/install -c
INSTALL_DATA	= ${INSTALL} -m 644
MKINSTALLDIRS = ../../mkinstalldirs

prefix = /usr/athena
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
EXECSUFFIX = 
PICFLAGS = # 
 
LIBNAME = $(LIBPREFIX)roken
#LIBEXT = a Always build archive library and don't install!
LIBEXT = a
LIBPREFIX = lib
SHLIBEXT = nobuild
LDSHARED = ld -Bshareable
LIB = $(LIBNAME).$(LIBEXT)

SOURCES = \
	base64.c \
	chown.c \
	concat.c \
	copyhostent.c \
	daemon.c \
	emalloc.c \
	erealloc.c \
	estrdup.c \
	eread.c \
	err.c \
	errx.c \
	ewrite.c \
	fchown.c \
	flock.c \
	fnmatch.c \
	freehostent.c \
	get_window_size.c \
	getarg.c \
	getcwd.c \
	get_default_username.c \
	getdtablesize.c \
	gethostname.c \
	getipnodebyaddr.c \
	getipnodebyname.c \
	getopt.c \
	getusershell.c \
	glob.c \
	hstrerror.c \
	inaddr2str.c \
	inet_aton.c \
	inet_ntop.c \
	initgroups.c \
	iruserok.c \
	issuid.c \
	k_getpwnam.c \
	k_getpwuid.c \
	lstat.c \
	memmove.c \
	mini_inetd.c \
	mkstemp.c \
	net_read.c \
	net_write.c \
	parse_time.c \
	parse_units.c \
	print_version.c \
	putenv.c \
	resolve.c \
	rcmd.c \
	roken_gethostby.c \
	readv.c \
	setegid.c \
	setenv.c \
	seteuid.c \
	signal.c \
	simple_exec.c \
	snprintf.c \
	socket.c \
	strcasecmp.c \
	strcollect.c \
	strdup.c \
	strerror.c \
	strftime.c \
	strlcat.c \
	strlcpy.c \
	strlwr.c \
	strncasecmp.c \
	strndup.c \
	strnlen.c \
	strsep.c \
	strtok_r.c \
	strupr.c \
	tm2time.c \
	unsetenv.c \
	verify.c \
	verr.c \
	verrx.c \
	vsyslog.c \
	vwarn.c \
	vwarnx.c \
	warn.c \
	warnerr.c \
	warnx.c

EXTRA_SOURCES = \
	make-print-version.c

OBJECTS = \
	base64.o \
	concat.o \
	emalloc.o \
	eread.o \
	erealloc.o \
	estrdup.o \
	ewrite.o \
	get_default_username.o \
	get_window_size.o \
	getarg.o \
	inaddr2str.o \
	issuid.o \
	k_getpwnam.o \
	k_getpwuid.o \
	mini_inetd.o \
	net_read.o \
	net_write.o \
	parse_time.o \
	parse_units.o \
	print_version.o \
	resolve.o \
	roken_gethostby.o \
	signal.o \
	simple_exec.o \
	snprintf.o \
	socket.o \
	strcollect.o \
	tm2time.o \
	verify.o \
	warnerr.o \
	 copyhostent.o freehostent.o getipnodebyaddr.o getipnodebyname.o strlwr.o strndup.o strnlen.o strupr.o

all: $(LIB) install-roken-h

Wall:
	make CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

.c.o:
	$(CC) -c $(DEFS) -I. -I../../include -I$(srcdir) $(CFLAGS) $(CPPFLAGS) $(PICFLAGS) $<

install: all

uninstall:

TAGS: $(SOURCES) $(EXTRA_SOURCES)
	etags $(SOURCES) $(EXTRA_SOURCES)

check:

clean:
	rm -f $(LIB) *.o *.a roken.h make-roken$(EXECSUFFIX) make-roken.c \
		make-print-version$(EXECSUFFIX) print_version.h

mostlyclean: clean

distclean: clean
	rm -f Makefile *.tab.c *~

realclean: distclean
	rm -f TAGS

$(LIBNAME).a: $(OBJECTS)
	rm -f $@
	$(AR) cr $@ $(OBJECTS)
	-$(RANLIB) $@

$(LIBNAME).$(SHLIBEXT): $(OBJECTS)
	rm -f $@
	$(LDSHARED) -o $@ $(OBJECTS)

roken.h: make-roken$(EXECSUFFIX)
	@./make-roken > tmp.h ;\
	if [ -f roken.h ] && cmp -s tmp.h roken.h ; then rm -f tmp.h ; \
	else rm -f roken.h; mv tmp.h roken.h; fi

make-roken$(EXECSUFFIX): make-roken.o
	$(LINK) $(CFLAGS) -o $@ make-roken.o

make-roken.c: roken.h.in roken.awk
	$(AWK) -f $(srcdir)/roken.awk $(srcdir)/roken.h.in > make-roken.c

print_version.o: print_version.h

print_version.h: make-print-version$(EXECSUFFIX)
	@./make-print-version$(EXECSUFFIX) print_version.h

make-print-version$(EXECSUFFIX): make-print-version.o
	$(LINK) $(CFLAGS) -o $@ make-print-version.o

install-roken-h: roken.h
	@if [ -f ../../include/roken.h ] && cmp -s ../../include/roken.h roken.h ; \
	then :; else \
	echo "  $(INSTALL) roken.h ../../include/roken.h"; \
	$(INSTALL) roken.h ../../include/roken.h; fi

$(OBJECTS): ../../include/config.h roken.h

.PHONY: all Wall install uninstall check clean mostlyclean distclean realclean install-roken-h

# Generated automatically from Makefile.in by configure.
#
# $KTH: Makefile.in,v 1.31.16.2 2000/06/23 03:20:04 assar Exp $
#

SHELL = /bin/sh

srcdir = .

top_builddir=../..

CC = gcc
LINK = $(CC)
AR = ar
RANLIB = ranlib
LN_S = ln -s
DEFS = -DHAVE_CONFIG_H -DROKEN_RENAME
CFLAGS =  -g -O2 $(WFLAGS)
WFLAGS = 
LD_FLAGS = 
YACC = yacc
LEX = flex

INSTALL = /usr/bin/install -c
INSTALL_DATA	= ${INSTALL} -m 644
MKINSTALLDIRS = ../../mkinstalldirs

prefix = /usr/athena
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
bindir = ${exec_prefix}/bin
includedir = ${prefix}/include

LIB_tgetent = -ltermcap
LIB_DEPS =  -lreadline $(LIB_tgetent) -lc
build_symlink_command   = @true
install_symlink_command = @true
install_symlink_command2 = @true

PICFLAGS = 
EXECSUFFIX = 
 
LIBEXT = a
SHLIBEXT = nobuild
LIBPREFIX = lib
LIBNAME = $(LIBPREFIX)sl
sl_LIB = $(LIBNAME).$(LIBEXT)
LIB = $(sl_LIB)
LIBNAME2 = $(LIBPREFIX)ss
ss_LIB = $(LIBNAME2).$(LIBEXT)
LIB2 = $(ss_LIB)
LDSHARED = ld -Bshareable
PROGS = mk_cmds$(EXECSUFFIX)

LIB_SOURCES = sl.c ss.c
EXTRA_SOURCES = strtok_r.c snprintf.c strupr.c

SOURCES = $(LIB_SOURCES) make_cmds.c $(EXTRA_SOURCES)

LIBADD = strtok_r.o snprintf.o strupr.o

LIB_OBJECTS = sl.o ss.o $(LIBADD)

mk_cmds_OBJECTS = make_cmds.o parse.o lex.o snprintf.o

OBJECTS = $(LIB_OBJECTS) $(mk_cmds_OBJECTS)

all: $(sl_LIB) $(PROGS)

Wall:
	make CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

.c.o:
	$(CC) -c $(DEFS) -I../../include -I. -I$(srcdir) -I$(srcdir)/../des $(CFLAGS) $(CPPFLAGS) $(PICFLAGS) $<

install: all
	$(MKINSTALLDIRS) $(DESTDIR)$(includedir)/ss
	$(INSTALL_DATA) $(srcdir)/ss.h $(DESTDIR)$(includedir)/ss/ss.h
	$(MKINSTALLDIRS) $(DESTDIR)$(libdir)
	$(INSTALL) -m 555 $(sl_LIB) $(DESTDIR)$(libdir)/$(sl_LIB)
	$(INSTALL) -m 555 $(sl_LIB) $(DESTDIR)$(libdir)/$(ss_LIB)
	@true
	@true
	$(MKINSTALLDIRS) $(DESTDIR)$(bindir)
	$(INSTALL)  $(PROGS) $(DESTDIR)$(bindir)/$(PROGS)

uninstall:
	rm -f $(DESTDIR)$(includedir)/ss/ss.h
	rm -f $(DESTDIR)$(libdir)/$(sl_LIB) $(DESTDIR)$(libdir)/$(ss_LIB)
	rm -f $(DESTDIR)$(bindir)/$(PROGS)

TAGS: $(SOURCES)
	etags $(SOURCES)

check:

clean:
	rm -f $(sl_LIB) $(PROGS) lex.c parse.c parse.h *.o *.a *.so *.so.* so_locations

mostlyclean: clean

distclean: clean
	rm -f Makefile *~

realclean: distclean
	rm -f TAGS

$(LIBNAME).a: $(LIB_OBJECTS)
	rm -f $@
	$(AR) cr $@ $(LIB_OBJECTS)
	-$(RANLIB) $@

$(LIBNAME).$(SHLIBEXT): $(LIB_OBJECTS)
	rm -f $@
	$(LDSHARED) -o $@ $(LIB_OBJECTS) $(LIB_DEPS)
	@true

$(OBJECTS): ../../include/config.h

$(mk_cmds_OBJECTS): parse.h

mk_cmds$(EXECSUFFIX): $(mk_cmds_OBJECTS)
	$(LINK) $(CFLAGS) -o $@ $(mk_cmds_OBJECTS) -L../roken -lroken

parse.c: parse.h
parse.h: $(srcdir)/parse.y
	$(YACC) -d $(srcdir)/parse.y
	mv -f y.tab.h parse.h
	mv -f y.tab.c parse.c

lex.c: $(srcdir)/lex.l
	$(LEX) $(srcdir)/lex.l
	mv -f lex.yy.c lex.c

strtok_r.c:
	$(LN_S) $(srcdir)/../roken/strtok_r.c .
snprintf.c:
	$(LN_S) $(srcdir)/../roken/snprintf.c .
strupr.c:
	$(LN_S) $(srcdir)/../roken/strupr.c .

.PHONY: all Wall install uninstall check clean mostlyclean distclean realclean

# Generated automatically from Makefile.in by configure.
#
# $KTH: Makefile.in,v 1.30.2.1 2000/10/10 14:34:33 assar Exp $
#

SHELL = /bin/sh

srcdir = .

CC = gcc
LINK = $(CC)
AR = ar
RANLIB = ranlib
LN_S = ln -s
DEFS = -DHAVE_CONFIG_H -DROKEN_RENAME
CFLAGS =  -g -O2 $(WFLAGS)
WFLAGS = 
LD_FLAGS = 
EXECSUFFIX=

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

PICFLAGS = 
 
LIB_DEPS =  -lc
build_symlink_command   = @true
#install_symlink_command = @true
install_symlink_command = @true

LIBNAME = $(LIBPREFIX)com_err
#LIBEXT = a Always build archive library!
LIBEXT = a
SHLIBEXT = nobuild
LIBPREFIX = lib
LDSHARED = ld -Bshareable
LIB = $(LIBNAME).$(LIBEXT)
lib_LIBRARIES = $(LIBNAME).$(LIBEXT)
bin_PROGRAMS = compile_et$(EXECSUFFIX)
include_HEADERS = com_right.h com_err.h

SOURCES = error.c com_err.c compile_et.c
OBJECTS = error.o com_err.o $(LIBADD)
EXTRA_SOURCES =
LIBADD =

all: $(lib_LIBRARIES) $(bin_PROGRAMS) $(include_HEADERS)

Wall:
	make CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

.c.o:
	$(CC) -c $(DEFS) -I../../include -I. -I$(srcdir) $(CFLAGS) $(CPPFLAGS) $(PICFLAGS) $<

install: all
	$(MKINSTALLDIRS) $(DESTDIR)$(includedir)
	@for i in $(include_HEADERS); do \
	f=`basename $$i`; \
	if test -f "$(srcdir)/$$f" ; then x="$(srcdir)/$$f"; \
	else x="$$f"; fi ;\
	echo "$(INSTALL_DATA) $$x $(DESTDIR)$(includedir)/$$f" ;\
	$(INSTALL_DATA) $$x $(DESTDIR)$(includedir)/$$f ; done

	$(MKINSTALLDIRS) $(DESTDIR)$(libdir)
	@for i in $(lib_LIBRARIES); do \
	echo "$(INSTALL) -m 0555 $$i $(DESTDIR)$(libdir)/$$i" ;\
	$(INSTALL) -m 0555 $$i $(DESTDIR)$(libdir)/$$i ; done
	@true

	$(MKINSTALLDIRS) $(DESTDIR)$(bindir)
	@for i in $(bin_PROGRAMS); do \
	echo "$(INSTALL) -m 0555 $$i $(DESTDIR)$(bindir)/$$i" ;\
	$(INSTALL) -m 0555 $$i $(DESTDIR)$(bindir)/$$i ; done

uninstall:
	@for i in $(include_HEADERS); do \
	f=`basename $$i`; \
	echo "rm -f $(DESTDIR)$(includedir)/$$f" ;\
	rm -f $(DESTDIR)$(includedir)/$$f ; done

	@for i in $(lib_LIBRARIES); do \
	echo "rm -f $(DESTDIR)$(libdir)/$$i" ;\
	rm -f $(DESTDIR)$(libdir)/$$i ; done

	@for i in $(bin_PROGRAMS); do \
	echo "rm -f $(DESTDIR)$(bindir)/$$i" ;\
	rm -f $(DESTDIR)$(bindir)/$$i ; done

TAGS: $(SOURCES)
	etags $(SOURCES)

clean:
	rm -f $(LIB) *.o *.a *.so *.so.* so_locations \
	krb_err.c krb_err.h parse.h parse.c lex.c \
	$(lib_LIBRARIES) $(bin_PROGRAMS) $(EXTRA_SOURCES)

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
	$(LDSHARED) -o $@ $(OBJECTS) $(LIB_DEPS)
	@true

COBJ = compile_et.o parse.o lex.o

$(COBJ): parse.h

compile_et$(EXECSUFFIX): $(COBJ)
	$(LINK) $(CFLAGS) -o $@ $(COBJ) -L../roken -lroken

parse.c: parse.h
parse.h: $(srcdir)/parse.y
	$(YACC) -d $(srcdir)/parse.y
	mv -f y.tab.h parse.h
	mv -f y.tab.c parse.c

lex.c: $(srcdir)/lex.l
	$(LEX) $(srcdir)/lex.l
	mv -f lex.yy.c lex.c

snprintf.c:
	$(LN_S) $(srcdir)/../roken/snprintf.c .

strlcat.c:
	$(LN_S) $(srcdir)/../roken/strlcat.c .

strlcpy.c:
	$(LN_S) $(srcdir)/../roken/strlcpy.c .

.PHONY: all Wall install uninstall clean mostlyclean distclean realclean

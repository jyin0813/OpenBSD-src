# Generated automatically from Makefile.in by configure.
#
# $KTH: Makefile.in,v 1.14.4.1 2000/06/23 03:20:02 assar Exp $
#

# This should really be set from configure
AFSWS = /usr/afsws
KRB5 = /usr/heimdal

SHELL = /bin/sh

srcdir = .

CC = gcc
LINK = $(CC)
AR = ar
RANLIB = ranlib
DEFS = -DHAVE_CONFIG_H
CFLAGS =  -g -O2 $(WFLAGS)
WFLAGS = 

INSTALL = /usr/bin/install -c
INSTALL_DATA	= ${INSTALL} -m 644
MKINSTALLDIRS = ../../mkinstalldirs

prefix = /usr/athena
exec_prefix = ${prefix}
includedir = ${prefix}/include
libdir = ${exec_prefix}/lib

PICFLAGS = # 

LIBNAME = $(LIBPREFIX)rxkad
#LIBEXT = a Always build archive library!
LIBEXT = a
LIBPREFIX = lib
SHLIBEXT = nobuild
LDSHARED = ld -Bshareable
LIB = $(LIBNAME).$(LIBEXT)

SOURCES = rxk_locl.c rxk_clnt.c rxk_serv.c rxk_crpt.c rxk_info.c osi_alloc.c compat.c

OBJECTS = rxk_locl.o rxk_clnt.o rxk_serv.o rxk_crpt.o rxk_info.o osi_alloc.o compat.o

all: $(LIB)

Wall:
	make CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

.c.o:
	$(CC) -c $(DEFS) -I../../include -I$(srcdir) $(CFLAGS) $(CPPFLAGS) $(PICFLAGS) -I$(AFSWS)/include -I$(KRB5)/include $<

fc_test: rxk_crpt.c
	$(LINK) -DTEST -o $@ $(CPPFLAGS) $(DEFS) -I../../include -I$(srcdir) $(CFLAGS) $(PICFLAGS) -I$(AFSWS)/include -I$(KRB5)/include $(VPATH)/rxk_crpt.c

install: all
	$(MKINSTALLDIRS) $(DESTDIR)$(includedir)/rx
	$(INSTALL_DATA) $(srcdir)/rxkad.h $(DESTDIR)$(includedir)/rx/rxkad.h
	$(MKINSTALLDIRS) $(DESTDIR)$(libdir)
	$(INSTALL_DATA)  $(LIB) $(DESTDIR)$(libdir)/$(LIB)

uninstall:
	rm -f $(DESTDIR)$(includedir)/rx/rxkad.h
	rm -f $(DESTDIR)$(libdir)/$(LIB)

TAGS: $(SOURCES)
	etags $(SOURCES)

check:

clean:
	rm -f $(LIB) *.o *.a fc_test

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

$(OBJECTS): ../../include/config.h

.PHONY: all Wall install uninstall check clean mostlyclean distclean realclean

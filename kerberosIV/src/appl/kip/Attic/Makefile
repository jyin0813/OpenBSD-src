# Generated automatically from Makefile.in by configure.
# $KTH: Makefile.in,v 1.18.4.1 2000/06/23 02:54:59 assar Exp $

SHELL = /bin/sh

srcdir = .

CC = gcc
LINK = $(CC)
AR = ar
DEFS = -DHAVE_CONFIG_H -DLIBEXECDIR="\"$(libexecdir)\""
CFLAGS =  -g -O2 $(WFLAGS)
WFLAGS = 
LD_FLAGS = 
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
LIBS = -lutil  
MKINSTALLDIRS = ../../mkinstalldirs

prefix = /usr/athena
exec_prefix = ${prefix}
libexecdir = ${exec_prefix}/libexec
libdir = ${exec_prefix}/lib
bindir = ${exec_prefix}/bin
transform=s,x,x,
EXECSUFFIX=

PROG_BIN	= kip$(EXECSUFFIX)
PROG_LIBEXEC	= kipd$(EXECSUFFIX)
SCRIPT_LIBEXEC	= kip-join-network kipd-control
PROGS = $(PROG_BIN) $(PROG_LIBEXEC) $(SCRIPT_LIBEXEC)

SOURCES_KIP     = kip.c
SOURCES_KIPD    = kipd.c
SOURCES_COMMON  = common.c

OBJECTS_KIP     = kip.o common.o
OBJECTS_KIPD    = kipd.o common.o

OBJECTS = $(OBJECTS_KIP) $(OBJECTS_KIPD)
SOURCES = $(SOURCES_KIP) $(SOURCES_KIPD) $(SOURCES_COMMON)

all: $(PROGS)

Wall:
	make CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

.c.o:
	$(CC) -c $(DEFS) -I../../include -I$(srcdir) $(CFLAGS) $(CPPFLAGS) $<

install: all
	$(MKINSTALLDIRS) $(DESTDIR)$(bindir) $(DESTDIR)$(libexecdir)
	for x in $(PROG_BIN); do \
	  $(INSTALL_PROGRAM) $$x $(DESTDIR)$(bindir)/`echo $$x | sed '$(transform)'`; \
	done
	for x in $(PROG_LIBEXEC); do \
	  $(INSTALL_PROGRAM) $$x $(DESTDIR)$(libexecdir)/`echo $$x | sed '$(transform)'`; \
	done
	for x in $(SCRIPT_LIBEXEC); do \
	  $(INSTALL_SCRIPT) $$x $(DESTDIR)$(libexecdir)/`echo $$x | sed '$(transform)'`; \
	done

uninstall:
	for x in $(PROG_BIN); do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$x | sed '$(transform)'`; \
	done
	for x in $(PROG_LIBEXEC); do \
	  rm -f $(DESTDIR)$(libexecdir)/`echo $$x | sed '$(transform)'`; \
	done
	for x in $(SCRIPT_LIBEXEC); do \
	  rm -f $(DESTDIR)$(libexecdir)/`echo $$x | sed '$(transform)'`; \
	done

TAGS: $(SOURCES)
	etags $(SOURCES)

check:

clean:
	rm -f *.a *.o $(PROGS)

mostlyclean: clean

distclean: clean
	rm -f Makefile *.tab.c *~

realclean: distclean
	rm -f TAGS

KLIB=-L../../lib/krb -lkrb -L../../lib/des -ldes
LIBROKEN=-L../../lib/roken -lroken

kip$(EXECSUFFIX): $(OBJECTS_KIP)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ $(OBJECTS_KIP) $(KLIB) $(LIBROKEN) $(LIBS) $(LIBROKEN)

kipd$(EXECSUFFIX): $(OBJECTS_KIPD)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ $(OBJECTS_KIPD) $(KLIB) $(LIBROKEN) $(LIBS) $(LIBROKEN)

$(OBJECTS): ../../include/config.h

kip-join-network: kip-join-network.in
	sed -e "s!%bindir%!$(bindir)!" $(srcdir)/kip-join-network.in > $@
	chmod +x $@

kipd-control: kipd-control.in
	sed -e "s!%bindir%!$(bindir)!" $(srcdir)/kipd-control.in > $@
	chmod +x $@

.PHONY: all Wall install uninstall check clean mostlyclean distclean realclean

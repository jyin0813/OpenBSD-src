# Generated automatically from Makefile.in by configure.
# $KTH: Makefile.in,v 1.16 1999/03/10 19:01:12 joda Exp $

SHELL = /bin/sh

srcdir = .
top_srcdir = ../..

CC = gcc
LINK = $(CC)
RANLIB = ranlib
DEFS = -DHAVE_CONFIG_H
CFLAGS =  -g -O2 $(WFLAGS)
WFLAGS = 
LD_FLAGS = 
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
LIBS = -lutil  
LIB_DBM = 
MKINSTALLDIRS = ../../mkinstalldirs

prefix = /usr/athena
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
bindir = ${exec_prefix}/bin
transform=s,x,x,
EXECSUFFIX=

PROG_SUIDBIN	= otp$(EXECSUFFIX)
PROG_BIN	= otpprint$(EXECSUFFIX)
PROGS = $(PROG_BIN) $(PROG_SUIDBIN)

SOURCES_OTP    = otp.c
SOURCES_OTPPRINT = otpprint.c

OBJECTS_OTP    = otp.o
OBJECTS_OTPPRINT    = otpprint.o

OBJECTS = $(OBJECTS_OTP) $(OBJECTS_OTPPRINT)
SOURCES = $(SOURCES_OTP) $(SOURCES_OTPPRINT)

all: $(PROGS)

.c.o:
	$(CC) -c $(DEFS) -I../../include -I$(srcdir) $(CFLAGS) $(CPPFLAGS) $<

install: all
	$(MKINSTALLDIRS) $(DESTDIR)$(bindir)
	for x in $(PROG_BIN); do \
	  $(INSTALL_PROGRAM) $$x $(DESTDIR)$(bindir)/`echo $$x | sed '$(transform)'`; \
	done
	-for x in $(PROG_SUIDBIN); do \
	  $(INSTALL_PROGRAM) -o root -m 04555 $$x $(DESTDIR)$(bindir)/`echo $$x | sed '$(transform)'`; \
	done

uninstall:
	for x in $(PROG_BIN); do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$x | sed '$(transform)'`; \
	done
	for x in $(PROG_SUIDBIN); do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$x | sed '$(transform)'`; \
	done

TAGS: $(SOURCES)
	etags $(SOURCES)

check:

clean:
	rm -f *.a *.o $(PROGS)

mostlyclean: clean

distclean: clean
	rm -f Makefile *~

realclean: distclean
	rm -f TAGS

OLIB=-L../../lib/otp -lotp -L../../lib/des -ldes
LIBROKEN=-L../../lib/roken -lroken

otp$(EXECSUFFIX): $(OBJECTS_OTP)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ $(OBJECTS_OTP) $(OLIB) $(LIBROKEN) $(LIB_DBM) $(LIBS) $(LIBROKEN)

otpprint$(EXECSUFFIX): $(OBJECTS_OTPPRINT)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ $(OBJECTS_OTPPRINT) $(OLIB) $(LIBROKEN) $(LIB_DBM) $(LIBS) $(LIBROKEN)

$(OBJECTS): ../../include/config.h

.PHONY: all install uninstall check clean mostlyclean distclean realclean

# Generated automatically from Makefile.in by configure.
# $KTH: Makefile.in,v 1.38 1999/03/10 19:01:12 joda Exp $

SHELL = /bin/sh

srcdir = .

top_builddir = ../..

CC = gcc
LINK = $(CC)
AR = ar
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
bindir = ${exec_prefix}/bin
libexecdir = ${exec_prefix}/libexec
transform=s,x,x,
EXECSUFFIX=

PROG_BIN	= pop_debug$(EXECSUFFIX)
PROG_LIBEXEC	= popper$(EXECSUFFIX)
PROGS = $(PROG_BIN) $(PROG_LIBEXEC)

SOURCES = $(POPPER_SOURCES) $(POP_DEBUG_SOURCES)
OBJECTS = $(POPPER_OBJECTS) $(POP_DEBUG_OBJECTS)

POPPER_SOURCES = \
	pop_dele.c pop_dropcopy.c pop_dropinfo.c \
	pop_get_command.c pop_init.c \
	pop_last.c pop_list.c pop_log.c \
	pop_msg.c pop_parse.c pop_pass.c pop_quit.c \
	pop_rset.c pop_send.c pop_stat.c pop_updt.c \
	pop_user.c pop_uidl.c pop_xover.c popper.c maildir.c

POP_DEBUG_SOURCES = \
	pop_debug.c

POPPER_OBJECTS = \
	pop_dele.o pop_dropcopy.o pop_dropinfo.o \
	pop_get_command.o pop_init.o \
	pop_last.o pop_list.o pop_log.o pop_msg.o \
	pop_parse.o pop_pass.o pop_quit.o pop_rset.o \
	pop_send.o pop_stat.o pop_updt.o pop_user.o \
	pop_uidl.o pop_xover.o popper.o maildir.o

POP_DEBUG_OBJECTS = \
	pop_debug.o

all: $(PROGS)

Wall:
	make CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

.c.o:
	$(CC) -c $(DEFS) -I../../include -I$(srcdir) $(CFLAGS) $(CPPFLAGS) $<

install: all
	$(MKINSTALLDIRS) $(DESTDIR)$(libexecdir)
	for x in $(PROG_LIBEXEC); do \
	  $(INSTALL_PROGRAM) $$x $(DESTDIR)$(libexecdir)/`echo $$x | sed '$(transform)'`; \
	done

uninstall:
	for x in $(PROG_LIBEXEC); do \
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
OTPLIB=-L$(top_builddir)/lib/otp -lotp

popper$(EXECSUFFIX): $(POPPER_OBJECTS)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ $(POPPER_OBJECTS) $(OTPLIB) $(KLIB) $(LIBROKEN) $(LIB_DBM) $(LIBS) $(LIBROKEN)

pop_debug$(EXECSUFFIX): $(POP_DEBUG_OBJECTS)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ $(POP_DEBUG_OBJECTS) $(KLIB) $(LIBROKEN) $(LIBS) $(LIBROKEN)

$(OBJECTS): ../../include/config.h

.PHONY: all Wall install uninstall check clean mostlyclean distclean realclean

# Generated automatically from Makefile.in by configure.
# $KTH: Makefile.in,v 1.18 1999/03/10 19:01:13 joda Exp $

SHELL = /bin/sh

srcdir = .

top_builddir = ../..

CC = gcc
LINK = $(CC)
AR = ar
DEFS = -DHAVE_CONFIG_H
CFLAGS =  -g -O2 $(WFLAGS)
WFLAGS = 
LD_FLAGS = 
INSTALL = /usr/bin/install -c
LIBS = -lutil  
MKINSTALLDIRS = ../../mkinstalldirs

prefix = /usr/athena
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
bindir = ${exec_prefix}/bin
transform=s,x,x,
EXECSUFFIX=

PROG_BIN	= sample_client$(EXECSUFFIX) \
		  simple_client$(EXECSUFFIX)
PROG_LIBEXEC	= sample_server$(EXECSUFFIX) \
		  simple_server$(EXECSUFFIX)
PROGS = $(PROG_BIN) $(PROG_LIBEXEC)

OBJECTS = sample_client.o sample_server.o simple_client.o simple_server.o
SOURCES = sample_client.c sample_server.c simple_client.c simple_server.c

all: $(PROGS)

Wall:
	make CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

.c.o:
	$(CC) -c $(DEFS) -I../../include -I$(srcdir) $(CFLAGS) $(CPPFLAGS) $<

install: all

uninstall:

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

sample_client$(EXECSUFFIX): sample_client.o
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ sample_client.o $(KLIB) $(LIBROKEN) $(LIBS) $(LIBROKEN)

simple_client$(EXECSUFFIX): simple_client.o
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ simple_client.o $(KLIB) $(LIBROKEN) $(LIBS) $(LIBROKEN)

sample_server$(EXECSUFFIX): sample_server.o
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ sample_server.o $(KLIB) $(LIBROKEN) $(LIBS) $(LIBROKEN)

simple_server$(EXECSUFFIX): simple_server.o
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ simple_server.o $(KLIB) $(LIBROKEN) $(LIBS) $(LIBROKEN)

$(OBJECTS): ../../include/config.h

.PHONY: all Wall install uninstall check clean mostlyclean distclean realclean

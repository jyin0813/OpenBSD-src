# Generated automatically from Makefile.in by configure.
# === makefile ============================================================
# Copyright (c) 1993 Chris Provenzano, proven@athena.mit.edu
#
#	Description: This file is for creating the test programs for libpthread.a
#
#	1.00 93/08/03 proven
#		-Initial cut for pthreads.
#

srctop = /dr1/my/masters/mysql/mit-pthreads
srcdir = /dr1/my/masters/mysql/mit-pthreads/lib/libpthreadutil
VPATH = /dr1/my/masters/mysql/mit-pthreads/lib/libpthreadutil
prefix= /usr/local/pthreads
exec_prefix= ${prefix}

INSTALL_PATH = ${prefix}
      BINDIR = $(INSTALL_PATH)/bin
      LIBDIR = $(INSTALL_PATH)/lib
      MANDIR = $(INSTALL_PATH)/man
      INCDIR = $(INSTALL_PATH)/include

          CC = ../../pgcc -notinstalled
 CDEBUGFLAGS = -g -O2 -Werror
      CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(ADDL_CFLAGS) -DSRCDIR=\"$(srcdir)\"
      RANLIB = ranlib

        OBJS = pthread_tad.o pthread_atexit.o
     LIBRARY = libpthreadutil.a
     HEADERS = pthreadutil.h

################################################################################
#
all : $(LIBRARY)

clean: 
	rm -f *.o $(TESTS) $(BENCHMARKS) a.out core maketmp makeout

depend:
	sed '/\#\#\# Dependencies/q' < Makefile > maketmp
	(for i in $(CSRC);do $(CPP) -M $$i;done) >> maketmp
	cp maketmp Makefile

install: $(LIBRARY)
	install $(LIBRARY) $(LIBDIR)
	for x in $(HEADERS); \
	do cp $(srcdir)/$$x $(INCDIR); \
	done

realclean: clean
	rm -f Makefile

Makefile: Makefile.in
	(cd ../.. ; sh config.status)

.c.o:
	$(CC) $(CFLAGS) -c $< 

$(LIBRARY) : ${OBJS}
	ar r new.a ${OBJS} && \
	$(RANLIB) new.a && \
	mv -f new.a $(LIBRARY)

################################################################################
### Do not remove the following line. It is for depend #########################
### Dependencies:

# === GNUmakefile =============================================================
# Copyright (c) 1991, 1992, 1993 Chris Provenzano, proven@athena.mit.edu
#
#	Description: This file is for creating libpthread.a
#
#	1.00 93/11/17 proven
#		-Put all the .o files into one file libpthread.a
#		-Initial cut for pthreads.
#

INSTALL_PATH = $(exec_prefix)

      BINDIR = $(INSTALL_PATH)/bin
      LIBDIR = $(INSTALL_PATH)/lib
      MANDIR = $(INSTALL_PATH)/man
      INCDIR = $(INSTALL_PATH)/include
   SUBINCDIR = $(INCDIR)/pthread

          AR = ar
          AS = gas
      CFLAGS = -I. -Iinclude -I$(srcdir)/include -DPTHREAD_KERNEL \
		-g -O2 -Werror
    CXXFLAGS = -I. -Iinclude -I$(srcdir)/include -DPTHREAD_KERNEL \
		-g -O2
          LD = gld

        CSRC =

 PTHREAD_DIR = pthreads stdlib stdio gen
        DIRS = $(PTHREAD_DIR)

     HEADERS =

   LIBRARIES = libpthread.a

     .CURDIR = .

# force correct default target
all:

###############################################################################
#
# Read in any special flags that config knows about
include config.flags

# What the heck.  Convert srcdir to absolute form so it looks prettier.
srcdir := $(shell cd $(srcfoo) && pwd)

################################################################################
#
# Here starts the nitty grity part of the Makefile.

all-lib : libpthread.a

include ${srcdir}/pthreads/GNUmakefile.inc
include ${srcdir}/stdlib/GNUmakefile.inc
include ${srcdir}/stdio/GNUmakefile.inc
include ${srcdir}/string/GNUmakefile.inc
include ${srcdir}/gen/GNUmakefile.inc
include ${srcdir}/net/GNUmakefile.inc
include ${srcdir}/scripts/GNUmakefile.inc

REGULAR_OBJS= $(subst .cc,.o,$(SRCS))
REGULAR_OBJS:= $(subst .c,.o,$(REGULAR_OBJS))
REGULAR_OBJS:= $(subst .S,.o,$(REGULAR_OBJS))
OBJS= $(REGULAR_OBJS) $(EXTRA_OBJS)
REALOBJS = $(addprefix obj/, $(OBJS))

$(REALOBJS) : $(config) $(types) $(paths)

# Since we do not have a list of the relevant files we do a make clean
# before copying everyting to the distribution directory.
distdir:
	$(MAKE) clean
	cp -a . $(distdir)
	# Remove symlinks that the distribution should not have.
	rm -f	$(distdir)/config.cache \
		$(distdir)/include/pthread/machdep.h \
		$(distdir)/include/pthread/posix.h \
		$(distdir)/include/sys \
		$(distdir)/machdep.c \
		$(distdir)/syscall.S \
		$(distdir)/syscall-template.S

clean: 
	rm -f a.out core maketmp makeout $(LIBRARIES) $(BINARIES) libpthread.*
	rm -rf obj
	cd tests && $(MAKE) clean && cd ..

install-lib: $(LIBRARIES) install-dirs
	for x in $(LIBRARIES); \
	do install $$x $(DESTDIR)$(LIBDIR); \
	done

# Removed make install since mysql uses this in place.
# install-lib install-include install-bin
install: 

libpthread.a: obj/libpthread.a
	rm -f libpthread.a
	ln -s obj/libpthread.a .

obj/libpthread.a: ${REALOBJS}
	rm -f libpthread.a obj/new.a obj/libpthread.a
	cd obj && \
		ar r new.a ${OBJS} && \
		$(RANLIB) new.a && \
		mv -f new.a libpthread.a && \
		cd ..

# For examining a combined symbol table, sizes, &c.
libpthread.o: ${REALOBJS}
	cd obj && ld -r -o ../libpthread.o ${OBJS} && cd ..

obj/x:
	if [ -d obj ]; then true; else mkdir obj; fi
	cp /dev/null obj/x

GNUmakefile: config.status ${srcdir}/config/GNUmakefile.in
	$(SHELL) config.status

obj/%.o: %.c obj/x
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: %.cc obj/x
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c $< -o $@

obj/%.o: %.S obj/x
	$(CC) $(CFLAGS) -c $< -o $@

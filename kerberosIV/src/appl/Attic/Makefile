# Generated automatically from Makefile.in by configure.
# $KTH: Makefile.in,v 1.31.6.1 2000/06/23 04:30:11 assar Exp $

srcdir		= .

SHELL		= /bin/sh



SUBDIRS		= sample kauth bsd movemail push afsutil \
		  popper xnlock kx kip otp ftp telnet

all:
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) all); done

Wall:
	make CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

install:
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) install); done

uninstall:
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) uninstall); done

clean:
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) clean); done

mostlyclean:	clean

distclean:
	for i in $(SUBDIRS);\
	do (cd $$i && $(MAKE) $(MFLAGS) distclean); done
	rm -f Makefile *~

realclean:
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) realclean); done

.PHONY: all Wall install uninstall clean mostlyclean distclean realclean

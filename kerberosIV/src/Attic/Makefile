# Generated automatically from Makefile.in by configure.
# $KTH: Makefile.in,v 1.36.4.1 2001/01/03 16:37:36 assar Exp $

srcdir		= .
prefix		= /usr/athena

SHELL		= /bin/sh
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = ./mkinstalldirs
TRAVELKIT = appl/kauth/kauth kuser/klist appl/telnet/telnet/telnet \
	    appl/ftp/ftp/ftp appl/kx/kx appl/kx/rxtelnet



SUBDIRS		= include lib kuser server slave admin kadmin appl man doc

all:
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) all); done

Wall:
	make CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

check:
	cd lib && $(MAKE) $(MFLAGS) check

install:
	$(MKINSTALLDIRS) $(DESTDIR)$(prefix)
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) install); done

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' install

uninstall:
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) uninstall); done

travelkit:	all
	$(MKINSTALLDIRS) tmp
	for i in $(TRAVELKIT); \
	do $(INSTALL_PROGRAM) $$i tmp/`basename $$i`; done
	(cd tmp; tar cf ../travelkit.tar `for i in $(TRAVELKIT); do basename $$i; done`)
	rm -rf tmp

travelkit-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' travelkit

TAGS:
	find . -name '*.[chyl]' -print | etags -

clean:
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) clean); done

mostlyclean:	clean

distclean:
	$(MAKE) clean
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) distclean); done
	rm -f Makefile config.status config.cache config.log version.h newversion.h.in version.h.in *~

realclean:
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) realclean); done

$(srcdir)/aclocal.m4:
	cd $(srcdir) && aclocal -I cf

.PHONY: all Wall check install install-strip uninstall travelkit travelkit-strip clean mostlyclean distclean realclean

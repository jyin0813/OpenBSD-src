# Generated automatically from Makefile.in by configure.
# $KTH: Makefile.in,v 1.19 1999/09/28 12:35:11 assar Exp $

SHELL = /bin/sh

srcdir = .

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = ../mkinstalldirs
MAKEINFO = makeinfo
TEXI2DVI = texi2dvi
TEXI2HTML = texi2html

prefix = /usr/athena
infodir = ${prefix}/info

TEXI_SOURCES = ack.texi				\
	index.texi				\
	install.texi				\
	intro.texi				\
	kth-krb.texi				\
	otp.texi				\
	problems.texi				\
	setup.texi				\
	whatis.texi

all: info

install: all installdirs
	if test -f kth-krb.info; then \
	  $(INSTALL_DATA) kth-krb.info $(DESTDIR)$(infodir)/kth-krb.info; \
	else \
	  $(INSTALL_DATA) $(srcdir)/kth-krb.info $(DESTDIR)$(infodir)/kth-krb.info; \
	fi
	if test -f $(DESTDIR)$(infodir)/dir ; then :; else \
		$(INSTALL_DATA) $(srcdir)/dir $(DESTDIR)$(infodir)/dir; \
	fi
	-if $(SHELL) -c 'install-info --version' >/dev/null 2>&1; then \
	  install-info --dir-file=$(DESTDIR)$(infodir)/dir $(DESTDIR)$(infodir)/kth-krb.info; \
	else \
	  true; \
	fi

uninstall:
	rm -f $(DESTDIR)$(infodir)/kth-krb.info

installdirs:
	$(MKINSTALLDIRS) $(DESTDIR)$(infodir)

info: kth-krb.info

kth-krb.info: $(TEXI_SOURCES)
	$(MAKEINFO) --no-split -I$(srcdir) -o $@ $(srcdir)/kth-krb.texi

dvi: kth-krb.dvi

kth-krb.dvi: $(TEXI_SOURCES)
	$(TEXI2DVI) $(srcdir)/kth-krb.texi

html: kth-krb.html

kth-krb.html: $(TEXI_SOURCES)
	$(TEXI2HTML) $(srcdir)/kth-krb.texi

clean:
	rm -f *.aux *.cp *.cps *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr

distclean: clean

mostlyclean: clean

maintainer-clean: clean
	rm -f *.info*

check:

.PHONY: all install uninstall installdirs info dvi html clean distclean mostlyclean maintainer-clean check

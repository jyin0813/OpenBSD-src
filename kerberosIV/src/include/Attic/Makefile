# Generated automatically from Makefile.in by configure.
# $KTH: Makefile.in,v 1.59.2.3 2000/12/13 14:41:37 assar Exp $

srcdir		= .

SHELL		= /bin/sh

CC		= gcc
LINK = $(CC)
DEFS		= -DHAVE_CONFIG_H -DHOST=\"i386-unknown-openbsd2.9\"
CFLAGS		=  -g -O2 $(WFLAGS)
WFLAGS		= 
LD_FLAGS	= 

INSTALL		= /usr/bin/install -c
INSTALL_DATA	= ${INSTALL} -m 644
MKINSTALLDIRS	= ../mkinstalldirs
LN_S		= ln -s
EXECSUFFIX	= 

prefix		= /usr/athena
exec_prefix	= ${prefix}
includedir	= ${prefix}/include
libdir		= ${exec_prefix}/lib



.c.o:
	$(CC) -c $(DEFS) -I. -I$(srcdir) $(CFLAGS) $(CPPFLAGS) $(PICFLAGS) $<

HEADERS = \
	acl.h com_err.h com_right.h des.h kadm.h kafs.h kdc.h \
	klog.h krb.h krb-protos.h krb-archaeology.h krb_db.h \
	ktypes.h otp.h prot.h sl.h \
	md4.h md5.h sha.h rc4.h 

LOCL_HEADERS = \
	     base64.h roken-common.h protos.h resolve.h xdbm.h	\
	     krb_log.h getarg.h parse_time.h 

CLEAN_FILES = roken.h krb_err.h kadm_err.h

BITS_OBJECTS = bits.o

SOURCES = bits.c

SUBDIRS		= sys

all: stamp-headers
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) all); done

Wall:
	$(MAKE) CFLAGS="-g -Wall -Wno-comment -Wmissing-prototypes -Wmissing-declarations -D__USE_FIXED_PROTOTYPES__"

install: all
	$(MKINSTALLDIRS) $(DESTDIR)$(includedir)
	for x in $(HEADERS); \
		do $(INSTALL_DATA) $$x $(DESTDIR)$(includedir)/$$x; done
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) install); done

uninstall:
	for x in $(HEADERS); do \
	  rm -f $(DESTDIR)$(includedir)/$$x; \
	done
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) uninstall); done

clean:
	rm -f $(HEADERS) $(LOCL_HEADERS) \
		$(CLEAN_FILES) *.o bits stamp-headers
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) clean); done

mostlyclean:	clean

distclean:
	$(MAKE) clean
	rm -f config.h version.h version.h.in
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) distclean); done
	rm -f Makefile config.status *~

realclean:
	for i in $(SUBDIRS); \
	do (cd $$i && $(MAKE) $(MFLAGS) realclean); done

acl.h:
	$(LN_S) $(srcdir)/../lib/acl/acl.h .

com_err.h:
	$(LN_S) $(srcdir)/../lib/com_err/com_err.h .
com_right.h:
	$(LN_S) $(srcdir)/../lib/com_err/com_right.h .

des.h:
	$(LN_S) $(srcdir)/../lib/des/des.h .

md4.h:
	$(LN_S) $(srcdir)/../lib/des/md4.h .

md5.h:
	$(LN_S) $(srcdir)/../lib/des/md5.h .

sha.h:
	$(LN_S) $(srcdir)/../lib/des/sha.h .

rc4.h:
	$(LN_S) $(srcdir)/../lib/des/rc4.h .

kadm.h:
	$(LN_S) $(srcdir)/../lib/kadm/kadm.h .

kafs.h:
	$(LN_S) $(srcdir)/../lib/kafs/kafs.h .

kdc.h:
	$(LN_S) $(srcdir)/../lib/kdb/kdc.h .

klog.h:
	$(LN_S) $(srcdir)/../lib/krb/klog.h .
krb-archaeology.h:
	$(LN_S) $(srcdir)/../lib/krb/krb-archaeology.h .
krb-protos.h:
	$(LN_S) $(srcdir)/../lib/krb/krb-protos.h .
krb.h:
	$(LN_S) $(srcdir)/../lib/krb/krb.h .
prot.h:
	$(LN_S) $(srcdir)/../lib/krb/prot.h .

krb_db.h:
	$(LN_S) $(srcdir)/../lib/kdb/krb_db.h .
krb_log.h:
	$(LN_S) $(srcdir)/../lib/krb/krb_log.h .

otp.h:
	$(LN_S) $(srcdir)/../lib/otp/otp.h .

base64.h:
	$(LN_S) $(srcdir)/../lib/roken/base64.h .
err.h:
	$(LN_S) $(srcdir)/../lib/roken/err.h .
fnmatch.h:
	$(LN_S) $(srcdir)/../lib/roken/fnmatch.h .
getarg.h:
	$(LN_S) $(srcdir)/../lib/roken/getarg.h .
glob.h:
	$(LN_S) $(srcdir)/../lib/roken/glob.h .
parse_time.h:
	$(LN_S) $(srcdir)/../lib/roken/parse_time.h .
resolve.h:
	$(LN_S) $(srcdir)/../lib/roken/resolve.h .
roken-common.h:
	$(LN_S) $(srcdir)/../lib/roken/roken-common.h .
xdbm.h:
	$(LN_S) $(srcdir)/../lib/roken/xdbm.h .

sl.h:
	$(LN_S) $(srcdir)/../lib/sl/sl.h .

protos.h:
	$(LN_S) $(srcdir)/protos.hin protos.h

netdb.h:
	$(LN_S) $(srcdir)/netdb.x netdb.h

bits$(EXECSUFFIX):	$(BITS_OBJECTS)
	$(LINK) $(LD_FLAGS) $(LDFLAGS) -o $@ $(BITS_OBJECTS)

bits.o: bits.c

ktypes.h: bits$(EXECSUFFIX)
	./bits$(EXECSUFFIX) $@

stamp-headers: Makefile
	$(MAKE) $(HEADERS) $(LOCL_HEADERS)
	touch stamp-headers

.PHONY: all Wall install uninstall clean mostlyclean distclean realclean

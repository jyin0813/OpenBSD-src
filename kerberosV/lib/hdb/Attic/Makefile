# $OpenBSD: Makefile,v 1.9 2003/05/13 03:13:30 hin Exp $

KRBDEPTH=../..
.include <${.CURDIR}/../../Makefile.inc>

INCLUDE_FILES=	${KRB5SRC}/lib/hdb/{hdb.h,hdb-protos.h,hdb-private.h} \
		hdb_err.h ${.CURDIR}/hdb_asn1.h

includes:	hdb_err.h hdb_asn1.h

LIB=	hdb
NOPIC=
NOPROFILE=

ASN1_SRC = asn1_Key.c asn1_Event.c asn1_HDBFlags.c asn1_hdb_entry.c \
        asn1_Salt.c hdb_asn1.h asn1_GENERATION.c

SRCS=	${ASN1_SRC} \
	keytab.c \
	hdb.c \
	common.c \
	db.c \
	mkey.c \
	ndbm.c \
	print.c \
	hdb_err.c \
	hdb_err.h

regen:
	asn1_compile ${KRB5SRC}/lib/hdb/hdb.asn1 hdb_asn1
	for f in *.x; do mv $$f `echo $$f | sed s,.x$$,.c,g`; done

CLEANFILES+=	hdb_err.[ch] hdb_asn1.c asn1.h

CFLAGS+=-I${KRB5SRC}/lib/hdb -I${KRB5SRC}/lib/asn1 -I../asn1 -I. -I../krb5

.PATH:	${KRB5SRC}/lib/hdb

hdb_err.c hdb_err.h: ${KRB5SRC}/lib/hdb/hdb_err.et
	compile_et ${KRB5SRC}/lib/hdb/hdb_err.et

.include <bsd.lib.mk>

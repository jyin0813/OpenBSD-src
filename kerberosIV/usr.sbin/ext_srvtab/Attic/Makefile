# $OpenBSD: Makefile,v 1.8 2002/05/11 00:20:19 espie Exp $

PROG	= ext_srvtab

KRBDEPTH=../..
.include "../../Makefile.inc"

.PATH:	${KRBSRC}/admin

CFLAGS	+= -I. -I${KRBSRC}/lib/kadm -I${KRBSRC}/lib/kdb
DPADD	+= ${LIBKAFS} ${LIBKRB5} ${LIBKDB} ${LIBKRB} ${LIBDES} ${LIBCRYPTO} \
	${LIBASN1}
LDADD	+= -lkafs -lkrb5 -lkdb -lkrb -ldes -lcrypto -lasn1 ${LIBROKEN}

SRCS	= ext_srvtab.c

MAN	= ${PROG}.8

.include <bsd.prog.mk>

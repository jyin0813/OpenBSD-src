#	$NetBSD: Makefile,v 1.9 1994/12/02 00:43:17 phil Exp $

SUBDIR=	bcwrap

PROG=	gnubc
CFLAGS+=-D_POSIX_SOURCE -DDOT_IS_LAST -I. -I${.CURDIR}
LFLAGS+=-I8
SRCS=	bc.c scan.c util.c main.c number.c storage.c load.c execute.c global.c
MAN=	bc.1

LDADD+=	-ll
DPADD+=	${LIBL}

CLEANFILES+= bc.c scan.c y.tab.h

.include <bsd.prog.mk>
.include <bsd.subdir.mk>

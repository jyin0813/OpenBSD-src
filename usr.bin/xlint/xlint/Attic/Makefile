#	$OpenBSD: Makefile,v 1.5 2011/11/08 10:37:10 guenther Exp $
#	$NetBSD: Makefile,v 1.2 1995/07/03 21:25:14 cgd Exp $

.PATH:	${.CURDIR}/../lint1

PROG=	xlint
SRCS=	xlint.c mem.c
MAN=	lint.1

CFLAGS+=-I${.CURDIR}/../lint1

realinstall:
	${INSTALL} ${INSTALL_COPY} -S ${INSTALL_STRIP} \
	    -o ${BINOWN} -g ${BINGRP} \
	    -m ${BINMODE} ${PROG} ${DESTDIR}${BINDIR}/lint


.include "${.CURDIR}/../../Makefile.inc"
.include <bsd.prog.mk>

#	$OpenBSD: Makefile,v 1.3 1999/05/10 16:14:07 espie Exp $

.include <bsd.own.mk>

.if (${ELF_TOOLCHAIN:L} == "no")
PROG=	ranlib
SRCS=	archive.c build.c misc.c ranlib.c touch.c
CFLAGS+=-I${.CURDIR} -I${.CURDIR}/../ar -I${.CURDIR}/../nm
MAN=	ranlib.1
VPATH=	${.CURDIR}/../ar
.else
NOPROG=
.endif

MAN+=	ranlib.5

.include <bsd.prog.mk>

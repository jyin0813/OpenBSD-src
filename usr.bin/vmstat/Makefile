#	$OpenBSD: Makefile,v 1.7 2001/06/27 06:16:50 art Exp $

PROG=	vmstat

SRCS=	dkstats.c vmstat.c
MAN=	vmstat.8
DPADD=	${LIBKVM}
LDADD=	-lkvm

.include <bsd.prog.mk>


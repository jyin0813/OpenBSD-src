#	$OpenBSD: Makefile,v 1.8 2002/01/03 01:04:59 art Exp $

REGRESS_TARGETS=do-cfman do-no3man do-scatman
REGRESS_SLOW_TARGETS=${REGRESS_TARGETS}

do-cfman:
	perl ${.CURDIR}/cfman /usr/share/man
do-no3man:
	perl ${.CURDIR}/no3man /usr/share/man
do-scatman:
	perl ${.CURDIR}/scatman /usr/share/man

.include <bsd.regress.mk>


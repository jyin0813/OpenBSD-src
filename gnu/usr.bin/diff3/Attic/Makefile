#	$OpenBSD: Makefile,v 1.3 2003/06/26 17:22:26 millert Exp $
#	$NetBSD: Makefile,v 1.7 1995/04/23 07:58:33 cgd Exp $

PROG=		diff3
SRCS=		diff3.c version.c
CFLAGS+=	-I$(.CURDIR)/../diff \
		-DHAVE_CONFIG_H -DDIFF_PROGRAM=\"/usr/bin/diff\"

.include <bsd.prog.mk>
.PATH: $(.CURDIR)/../diff

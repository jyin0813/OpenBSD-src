#	$OpenBSD: Makefile,v 1.1 2003/06/26 16:22:04 mickey Exp $

DIR=	usd/15.sed
SRCS=	sed
MACROS=	-ms

paper.txt: ${SRCS}
	${ROFF} -Tascii ${SRCS} > ${.TARGET}

.include <bsd.doc.mk>

#	$OpenBSD: Makefile,v 1.3 2004/02/01 14:22:44 jmc Exp $


DIR=	papers/fsinterface
SRCS=	fsinterface.ms
MACROS=	-ms

paper.txt: ${SRCS}
	${ROFF} -Tascii ${SRCS} > ${.TARGET}

.include <bsd.doc.mk>

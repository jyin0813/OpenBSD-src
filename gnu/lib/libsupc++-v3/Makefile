# $OpenBSD: Makefile,v 1.2 2010/05/15 17:07:57 matthieu Exp $

.include "${.CURDIR}/../libstdc++-v3/Makefile.common" 

# Static only.
LIB=	supc++

# Pulled from Makefile.common
SRCS=	${SUPSRCS}

.if ${MACHINE_ARCH} == "arm"
CFLAGS+=	-D_GLIBCXX_SJLJ_EXCEPTIONS=1
.endif

# Build a standard .a lib with PIC objects
NOPIC=
CFLAGS+= ${PICFLAG}

.include <bsd.lib.mk>

#	$OpenBSD: Makefile,v 1.5 2001/07/04 08:33:44 niklas Exp $
#	$NetBSD: Makefile,v 1.3 1995/11/17 23:23:48 gwr Exp $

SA_PROG= bootxx
SRCS=   bootxx.c conf.c

all:	${SA_PROG}

# Do not strip or remove a.out header for this one.
install: ${SA_PROG}
	${INSTALL} ${INSTALL_COPY} -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} \
	  ${SA_PROG} ${DESTDIR}${MDEC_DIR}/${SA_PROG}

.include <bsd.prog.mk>


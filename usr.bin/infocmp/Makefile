#	$OpenBSD: Makefile,v 1.2 2000/02/20 21:50:18 millert Exp $

PROG=	infocmp
SRCS=	infocmp.c dump_entry.c
DPADD=	${LIBCURSES}
LDADD=	-lcurses
CURSES= ${.CURDIR}/../../lib/libcurses
TIC= ${.CURDIR}/../tic
CFLAGS+= -I${CURSES} -I${TIC} -I${.CURDIR} -I.
CLEANFILES+= termsort.c infocmp.1
.PATH:  ${TIC}

beforedepend: termsort.c

termsort.c: ${TIC}/MKtermsort.sh
	sh ${TIC}/MKtermsort.sh awk ${CURSES}/Caps > ${.TARGET}

infocmp.1: infocmp.1tbl
	cp ${.ALLSRC} ${.TARGET}

.include <bsd.prog.mk>

#	$OpenBSD: Makefile,v 1.1 1997/09/07 19:59:05 millert Exp $
#	$NetBSD: Makefile,v 1.13 1997/08/18 10:20:13 lukem Exp $
#	from: @(#)Makefile	8.2 (Berkeley) 4/3/94

# Define SMALL to disable command line editing
CFLAGS+=-DSMALL

# Uncomment the following to provide defaults for gate-ftp operation
#CFLAGS+=-DGATE_SERVER=\"ftp-gw.host\" # -DGATE_PORT=21

# Uncomment the following for socks5 support.
#CFLAGS += -DSOCKS -include /usr/local/include/socks.h
#LDADD += -lsocks5

PROG=	ftp
SRCS=	cmds.c cmdtab.c complete.c domacro.c fetch.c ftp.c main.c ruserpass.c \
	stringlist.c util.c
MAN=
.PATH:	${.CURDIR}/../../../../usr.bin/ftp

LDADD+=	-ltermcap
DPADD+=	${LIBTERMCAP}

#COPTS+= -Wall -Wconversion -Wstrict-prototypes -Wmissing-prototypes

.include <bsd.prog.mk>

# $OpenBSD: Makefile,v 1.4 2011/04/16 07:41:08 sobrado Exp $

PROG=	user
SRCS=	user.c main.c

LDADD+=	-lutil
DPADD+=	${LIBUTIL}

LINKS+=	${BINDIR}/user ${BINDIR}/useradd
LINKS+=	${BINDIR}/user ${BINDIR}/userdel
LINKS+=	${BINDIR}/user ${BINDIR}/usermod
LINKS+=	${BINDIR}/user ${BINDIR}/group
LINKS+=	${BINDIR}/user ${BINDIR}/groupadd
LINKS+=	${BINDIR}/user ${BINDIR}/groupdel
LINKS+=	${BINDIR}/user ${BINDIR}/groupmod
LINKS+=	${BINDIR}/user ${BINDIR}/userinfo
LINKS+=	${BINDIR}/user ${BINDIR}/groupinfo

MAN=	usermgmt.conf.5 user.8 useradd.8 userdel.8 usermod.8 userinfo.8 \
	group.8 groupadd.8 groupdel.8 groupmod.8 groupinfo.8

#MLINKS=	useradd.8 adduser.8

.include <bsd.prog.mk>

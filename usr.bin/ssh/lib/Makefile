#	$OpenBSD: Makefile,v 1.62 2010/08/31 11:54:45 djm Exp $

.PATH:		${.CURDIR}/..
.include "${.CURDIR}/../Makefile.inc"

LIB=	ssh
WANTLINT=no
SRCS=	authfd.c authfile.c bufaux.c bufec.c bufbn.c buffer.c canohost.c \
	channels.c cipher.c cipher-3des1.c cipher-bf1.c cipher-ctr.c \
	cleanup.c compat.c compress.c crc32.c deattack.c fatal.c \
	hostfile.c log.c match.c nchan.c packet.c readpass.c \
	rsa.c ttymodes.c xmalloc.c atomicio.c \
	key.c dispatch.c kex.c mac.c uidswap.c uuencode.c misc.c \
	ssh-dss.c ssh-rsa.c ssh-ecdsa.c dh.c kexdh.c kexgex.c kexecdh.c \
	kexdhc.c kexgexc.c kexecdhc.c msg.c progressmeter.c dns.c \
	monitor_fdpass.c umac.c addrmatch.c schnorr.c jpake.c ssh-pkcs11.c

DEBUGLIBS= no
NOPROFILE= yes
NOPIC=	yes

install:
	@echo -n

.include <bsd.own.mk>

.if (${KERBEROS5:L} == "yes")
CFLAGS+= -DKRB5 -I${DESTDIR}/usr/include/kerberosV

SRCS+= gss-genr.c
CFLAGS+= -DGSSAPI
.endif # KERBEROS5

.include <bsd.lib.mk>

#	$OpenBSD: Makefile,v 1.9 2001/01/17 06:01:21 fgsch Exp $

PROG=	ipftest 
MAN=	ipftest.1	
SRCS=	ipt.c fil.c ipft_hx.c ipft_sn.c ipft_ef.c ipft_td.c ipft_pc.c \
	ipft_tx.c misc.c parse.c opt.c ip_frag.c ip_nat.c ip_state.c \
	ip_auth.c ip_fil.c ip_proxy.c facpri.c natparse.c common.c ifaddr.c 

.PATH: 	${.CURDIR}/../../sbin/ipf ${.CURDIR}/../../sbin/ipfstat \
	${.CURDIR}/../../sys/netinet ${.CURDIR}/../../sbin/ipnat

CFLAGS+=-I${.CURDIR}/../../sbin/ipf \
	-I${.CURDIR}

.include <bsd.prog.mk>

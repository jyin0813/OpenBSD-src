#	$OpenBSD: Makefile,v 1.2 2001/07/13 09:28:47 kjc Exp $
#	$NetBSD: Makefile,v 1.2 2000/12/16 18:57:35 thorpej Exp $

LIB=	altq
SRCS=	parser.c qop.c qop_blue.c qop_cbq.c qop_cdnr.c qop_conf.c \
	qop_dummy.c qop_errlist.c qop_fifoq.c qop_hfsc.c qop_priq.c \
	qop_red.c qop_rio.c qop_wfq.c quip_server.c

CPPFLAGS+= -DALTQ -DINET6

# we don't build the other copies of this library
NOPIC=		yes
NOPROFILE=	yes

# only needed during build
install:

.include <bsd.lib.mk>

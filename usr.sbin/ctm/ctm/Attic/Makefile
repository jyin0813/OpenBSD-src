#
# ----------------------------------------------------------------------------
# "THE BEER-WARE LICENSE" (Revision 42):
# <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
# can do whatever you want with this stuff. If we meet some day, and you think
# this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
# ----------------------------------------------------------------------------
#
# $Id: Makefile,v 1.2 1999/07/13 23:02:05 deraadt Exp $
#

PROG=		ctm
NOTYET=		ctm_ed.c
SRCS=   	ctm.c ctm_input.c ctm_pass1.c ctm_pass2.c ctm_pass3.c \
		ctm_passb.c ctm_syntax.c ctm_ed.c
MAN=		ctm.1 ctm.5
CFLAGS+=	-Wall

.if exists(${.CURDIR}/../../Makefile.inc)
.include "${.CURDIR}/../../Makefile.inc"
.endif
.include <bsd.prog.mk>

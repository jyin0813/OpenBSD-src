#	$OpenBSD: Makefile,v 1.8 1997/01/05 08:17:31 deraadt Exp $
#	$NetBSD: Makefile,v 1.23 1996/02/18 12:29:18 mycroft Exp $

STATIC=	-static

PROG=	sh
SRCS=	alias.c builtins.c cd.c echo.c error.c eval.c exec.c expand.c \
	histedit.c input.c jobs.c mail.c main.c memalloc.c miscbltin.c \
	mystring.c nodes.c options.c parser.c redir.c show.c syntax.c \
	trap.c output.c var.c
OBJS+=	arith.o arith_lex.o init.o
LDADD+=	-ll -ledit -ltermcap
DPADD+=	${LIBL} ${LIBEDIT} ${LIBTERMCAP}
LFLAGS= -8	# 8-bit lex scanner for arithmetic
CFLAGS+=-DSHELL -I. -I${.CURDIR}
.PATH:	${.CURDIR}/bltin ${.CURDIR}/../../usr.bin/printf
CLEANFILES+=\
	arith.c arith_lex.c builtins.c builtins.h init.c mkinit mknodes \
	mksyntax nodes.c nodes.h printf.o syntax.c syntax.h token.h \
	y.tab.c y.tab.h

.depend parser.o: token.h

token.h: mktokens
	sh ${.CURDIR}/mktokens

builtins.h builtins.c: ${.CURDIR}/mkbuiltins ${.CURDIR}/builtins.def
	cd ${.CURDIR}; sh mkbuiltins ${.OBJDIR}

init.c: mkinit ${SRCS}
	./mkinit ${.ALLSRC:S/^mkinit$//}

mkinit: ${.CURDIR}/mkinit.c
	${HOSTCC} ${CFLAGS} ${.CURDIR}/mkinit.c -o $@

nodes.c nodes.h: mknodes ${.CURDIR}/nodetypes ${.CURDIR}/nodes.c.pat
	./mknodes ${.CURDIR}/nodetypes ${.CURDIR}/nodes.c.pat

mknodes: ${.CURDIR}/mknodes.c
	${HOSTCC} ${CFLAGS} ${.CURDIR}/mknodes.c -o $@

syntax.c syntax.h: mksyntax
	./mksyntax

mksyntax: ${.CURDIR}/mksyntax.c ${.CURDIR}/parser.h
	${HOSTCC} ${CFLAGS} ${.CURDIR}/mksyntax.c -o $@

.include <bsd.prog.mk>

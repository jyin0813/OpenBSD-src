# $OpenBSD$

PROG=	setsockopt3a
CFLAGS+= -I${.CURDIR}/../../include

REGRESS_TARGETS=dummy

# build prog and link into test 3 directory.  Code is used by test 3
#
dummy: ${PROG}
	@cd ${.CURDIR}/../3; \
	if test -d ${__objdir} ; then \
		cd ${__objdir} ; \
	fi; \
	ln -sf ${.OBJDIR}/${PROG}

.include <bsd.regress.mk>


#	$OpenBSD: Makefile.inc,v 1.3 1997/10/20 00:33:46 deraadt Exp $

LIB_BUG_DIR=${S}/arch/mvme88k/stand/libbug

LIBBUG_DIR!= cd ${LIB_BUG_DIR}; \
     printf "xxx:\n\techo \$${.OBJDIR}\n" | ${MAKE} -r -s -f - xxx

LIBBUG=${LIBBUG_DIR}/libbug.a

$(LIBBUG):	.NOTMAIN __always_make_libbug
	@echo making sure the libbug is up to date...
	@(cd ${LIB_BUG_DIR}; ${MAKE})

__always_make_libbug:	.NOTMAIN

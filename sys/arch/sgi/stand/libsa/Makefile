#	$OpenBSD: Makefile,v 1.4 2007/11/26 10:26:02 deraadt Exp $

LIB=	sa

.PATH:	${.CURDIR}/../../../../lib/libsa

CLEANFILES += machine mips64

CFLAGS+= ${CEXTRAFLAGS} ${SAABI} -nostdinc -mno-abicalls -D_NO_ABICALLS \
	-I${.CURDIR} -I${.CURDIR}/../include -I${.CURDIR}/../.. \
	-I${.CURDIR}/../../.. -I${.CURDIR}/../../../.. \
	-I${.CURDIR}/../../../../lib/libsa \
	-I${.OBJDIR}

CPPFLAGS+= -D__INTERNAL_LIBSA_CREAD ${STANDALONE}
CPPFLAGS+= ${LIBSA_CPPFLAGS}

# stand routines
SRCS=	alloc.c exit.c exec.c getfile.c gets.c globals.c \
	memcmp.c memcpy.c memset.c printf.c snprintf.c strerror.c strncpy.c

# io routines
SRCS+=	close.c closeall.c dev.c disklabel.c dkcksum.c fstat.c ioctl.c lseek.c \
	open.c read.c stat.c write.c cread.c

# boot filesystems
SRCS+=	ufs.c nfs.c cd9660.c

${OBJS}:	${.CURDIR}/../Makefile.inc

NOPROFILE=
NOPIC=

.if !make(libdep) && !make(sadep) && !make(salibdir) && !make(obj)
.BEGIN:
	@([ -h machine ] || ln -s ${.CURDIR}/../../include machine)
	@([ -h mips64 ] || ln -s ${.CURDIR}/../../../mips64/include mips64)
.endif

install:

.include <bsd.lib.mk>

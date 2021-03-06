#	$OpenBSD: Makefile,v 1.7 2010/03/07 11:00:37 kettenis Exp $

TOP=		${.CURDIR}/..

#.include "${TOP}/Makefile.inc"

CDROM=		install${OSrev}.iso
RELXDIR?=	/home/relx-${MACHINE}
RELDIR?=	/home/rel-${MACHINE}

BASE=		${RELDIR}/base${OSrev}.tgz ${RELDIR}/comp${OSrev}.tgz \
		${RELDIR}/etc${OSrev}.tgz ${RELDIR}/game${OSrev}.tgz \
		${RELDIR}/man${OSrev}.tgz \
		${RELDIR}/bsd ${RELDIR}/bsd.rd ${RELDIR}/bsd.mp \
		${RELDIR}/INSTALL.${MACHINE}
XBASE=		${RELXDIR}/xbase${OSrev}.tgz ${RELXDIR}/xetc${OSrev}.tgz \
		${RELXDIR}/xfont${OSrev}.tgz ${RELXDIR}/xshare${OSrev}.tgz \
		${RELXDIR}/xserv${OSrev}.tgz

all: ${CDROM}

${CDROM}: ${BASE} ${XBASE}
	-rm -rf ${.OBJDIR}/cd-dir/
	-@mkdir -p ${.OBJDIR}/cd-dir/${OSREV}/${MACHINE}/

	cp -p ${BASE} ${.OBJDIR}/cd-dir/${OSREV}/${MACHINE}
	cp -p ${XBASE} ${.OBJDIR}/cd-dir/${OSREV}/${MACHINE}

	cp -p ${.CURDIR}/../../../sys/arch/${MACHINE}/stand/tbxidata/bsd.tbxi \
	    ${.OBJDIR}/cd-dir/${OSREV}/${MACHINE}
	cp -p ${RELDIR}/ofwboot ${.OBJDIR}/cd-dir/

	mkhybrid -v -v -r -part -hfs \
	    -hfs-bless ${.OBJDIR}/cd-dir/${OSREV}/${MACHINE} \
	    -map ${.CURDIR}/../../../gnu/usr.sbin/mkhybrid/src/more.mapping \
	    -A "OpenBSD ${OSREV} ${MACHINE} Install CD" \
	    -P "Copyright (c) `date +%Y` Theo de Raadt, The OpenBSD project" \
	    -p "Theo de Raadt <deraadt@openbsd.org>" \
	    -V "OpenBSD/${MACHINE}  ${OSREV} Install CD" \
	    -o ${.OBJDIR}/${CDROM} ${.OBJDIR}/cd-dir

install:
	cp ${CDROM} ${RELDIR}/
	# update the SHA256 file
	grep -v ${CDROM} ${RELDIR}/SHA256 > ${RELDIR}/SHA256.new
	sum -a sha256 ${CDROM} >> ${RELDIR}/SHA256.new
	sort ${RELDIR}/SHA256.new > ${RELDIR}/SHA256
	rm ${RELDIR}/SHA256.new

clean cleandir:
	/bin/rm -f ${CDROM}
	rm -rf cd-dir

.include <bsd.obj.mk>
.include <bsd.subdir.mk>

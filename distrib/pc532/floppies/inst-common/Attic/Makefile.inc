#	$OpenBSD: Makefile.inc,v 1.6 1997/09/26 02:10:00 millert Exp $ 
#	$NetBSD: Makefile.inc,v 1.5.4.2 1996/08/07 07:29:42 phil Exp $ 

# TOP is assumed to be defined by Makefile including this one.

CBIN=		instbin
COMMONDIR=	${TOP}/inst-common

MOUNT_POINT?=	/mnt
VND?=		svnd0
VND_DEV=	/dev/${VND}a
VND_RDEV=	/dev/r${VND}a
IMAGE?=		xxx-${REV}.fs
MDEC=		${DESTDIR}/usr/mdec

LISTS=		${COMMONDIR}/list ${.CURDIR}/list
CRUNCHCONF=	${COMMONDIR}/${CBIN}.conf
MTREE=		${COMMONDIR}/mtree.conf
ADIR=		${TOP}/../../../sys/arch/pc532
PWDSRC=		${TOP}/../../../usr.sbin/pwd_mkdb
.if exists(${PWDSRC}/obj)
PWDOBJ=		${PWDSRC}/obj
.else
PWDOBJ=		${PWDSRC}
.endif

all: netbsd
	dd if=/dev/zero of=${IMAGE} bs=18k count=80
	disklabel -r -w ${.OBJDIR}/${IMAGE} boot3 2>/dev/null
	-yes|bim -c init -c "add ${MDEC}/boot boot" -c "default 0" \
	    -c "exit" ${.OBJDIR}/${IMAGE} 2>/dev/null >/dev/null
	vnconfig -v -c ${VND_DEV} ${IMAGE}
	disklabel -w -r ${VND} boot3
	newfs -b 8192 -f 1024 -O -m 0 -o space -i 204800 -c 80 ${VND_RDEV}
	mount ${VND_DEV} ${MOUNT_POINT}
	cp netbsd ${MOUNT_POINT}/netbsd
	@df -i ${MOUNT_POINT}
	@echo ""
	umount ${MOUNT_POINT}
	vnconfig -u ${VND_DEV}

inject.fs: ${CBIN} ${LISTS} netbsd.gz 
	(cd ${PWDSRC}; ${MAKE})
	${CC} -static -o pwd_mkdb ${PWDOBJ}/*.o -lutil
.ifndef SD
	dd if=/dev/zero of=${.TARGET} bs=128k count=16
	vnconfig -v -c ${VND_DEV} ${.TARGET}
	disklabel -w -r ${VND} rd
	newfs -b 8192 -f 1024 -O -m 0 -o space -i 7168 ${VND_RDEV}
	mount ${VND_DEV} ${MOUNT_POINT}
.else
	newfs -b 8192 -f 1024 -O -m 0 -o space -i 7168 -s 4096 \
		-t 1 -u 256 /dev/r${SD}
	mount /dev/${SD} ${MOUNT_POINT}
.endif
	mtree -def ${MTREE} -p ${MOUNT_POINT}/ -u
	TOPDIR=${TOP} CURDIR=${.CURDIR} OBJDIR=${.OBJDIR} \
	    TARGDIR=${MOUNT_POINT} sh ${TOP}/runlist.sh ${LISTS}
	@echo ""
	@df -i ${MOUNT_POINT}
	@echo ""
	umount ${MOUNT_POINT}
.ifndef SD
	vnconfig -u ${VND_DEV}
.else
	dd if=/dev/${SD} of=${.TARGET} bs=128k count=16
.endif

unconfig:
	-umount -f ${MOUNT_POINT}
	-vnconfig -u ${VND_DEV}
	-/bin/rm -f ${IMAGE}

${CBIN}.mk ${CBIN}.cache ${CBIN}.c: ${CRUNCHCONF}
	crunchgen -D ${TOP}/../../.. -L ${DESTDIR}/usr/lib ${CRUNCHCONF}

${CBIN}: ${CBIN}.mk ${CBIN}.cache ${CBIN}.c
	make -f ${CBIN}.mk all

cloak: ${COMMONDIR}/cloak.c

inject: ${COMMONDIR}/inject.c

netbsd.gz:
	gzip -9 <${ADIR}/compile/DEFAULT/netbsd >netbsd.gz
#	cp ${ADIR}/compile/DEFAULT/netbsd /var/tmp/$$$$; \
#		strip /var/tmp/$$$$; \
#		gzip -9 </var/tmp/$$$$ >netbsd.gz; \
#		rm /var/tmp/$$$$


netbsd.o: inject cloak inject.fs
	cp ${ADIR}/compile/INSTALL/netbsd /var/tmp/$$$$; \
		strip /var/tmp/$$$$; \
		./inject /var/tmp/$$$$ inject.fs; \
		gzip -9 </var/tmp/$$$$ | ./cloak >netbsd.o; \
		rm /var/tmp/$$$$

go.o: ${COMMONDIR}/go.c

netbsd: netbsd.o go.o
	ld -s -z -T 240000 -e _go go.o netbsd.o ${MDEC}/zboot.o -o netbsd

clean cleandir:
	/bin/rm -f core ${IMAGE} ${CBIN} ${CBIN}.mk ${CBIN}.cache *.o *.lo *.c
	/bin/rm -f netbsd* inject* cloak pwd_mkdb

.include <bsd.obj.mk>
.include <bsd.subdir.mk>

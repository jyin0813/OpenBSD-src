#	$NetBSD: Makefile,v 1.13.4.2 1996/06/15 04:02:42 cgd Exp $

# Make a distribution for the alpha, on a spare disk.
# This creates a large, gzipped disk image in ${.OBJDIR}

TARGETS=	bin.tar.gz etc.tar.gz x11.tar.gz rz25-image.gz

.if !defined(DESTDIR) || !defined(DESTDISK)
all:
	@echo both 'DESTDIR' and 'DESTDISK' must be defined.
	@false
.else

SECPERCYL=	558
CYLS=		1476

all: ${TARGETS}

mount-fs:
	disklabel -W ${DESTDISK}
	-dd if=/dev/zero of=/dev/r${DESTDISK}c bs=`expr ${SECPERCYL} \* 512` \
	    count=${CYLS}
	disklabel -w -r ${DESTDISK} rz25
	disklabel -W ${DESTDISK}
	newfs -O /dev/r${DESTDISK}a
	newfs -O /dev/r${DESTDISK}d
	mount /dev/${DESTDISK}a ${DESTDIR}
	mkdir ${DESTDIR}/usr
	mount /dev/${DESTDISK}d ${DESTDIR}/usr

build-fs:
	cd ${.CURDIR}/../../../etc && make distribution
	if [ -d /usr/alphasrc/cygnus ]; then \
		cd /usr/alphasrc/cygnus && \
		    make prefix=${DESTDIR}/usr/local install; \
	else \
		true; \
	fi
	if [ -d /usr/alphasrc/xc ]; then \
		cd /usr/alphasrc/xc && make install install.man; \
	else \
		true; \
	fi
	cp ${.CURDIR}/../../../sys/arch/alpha/compile/GENERIC/netbsd ${DESTDIR}
	ln -s gcc ${DESTDIR}/usr/local/bin/cc
	(cd ${DESTDIR}/usr/libexec && ln -s \
	    ../local/lib/gcc-lib/alpha-unknown-netbsd*/*/cpp .)
	ln -s ../local/bin/ar ${DESTDIR}/usr/bin
	ln -s ../local/bin/as ${DESTDIR}/usr/bin
	ln -s ../local/bin/cc ${DESTDIR}/usr/bin
	ln -s ../local/bin/gcc ${DESTDIR}/usr/bin
	ln -s ../local/bin/ld ${DESTDIR}/usr/bin
	ln -s ../local/bin/nm ${DESTDIR}/usr/bin
	ln -s ../local/bin/ranlib ${DESTDIR}/usr/bin
	ln -s ../local/bin/size ${DESTDIR}/usr/bin
	ln -s ../local/bin/strip ${DESTDIR}/usr/bin
	cp -p ${DESTDIR}/usr/mdec/boot ${DESTDIR}/
	sync; sleep 1; sync; sleep 1
	${DESTDIR}/usr/mdec/installboot -v ${DESTDIR}/boot \
	    ${DESTDIR}/usr/mdec/bootxx /dev/r${DESTDISK}c

unmount-fs:
	umount ${DESTDIR}/usr ${DESTDIR}
	fsck /dev/r${DESTDISK}a /dev/r${DESTDISK}d

rz25-image.gz: mount-fs build-fs unmount-fs
	mount /dev/${DESTDISK}a ${DESTDIR}
	mount /dev/${DESTDISK}d ${DESTDIR}/usr
	mkdir ${DESTDIR}/alphadist
	cp -p ${DESTDIR}/usr/sbin/sysctl ${DESTDIR}/alphadist
	mv ${DESTDIR}/etc/rc ${DESTDIR}/alphadist/rc.real
	cp ${.CURDIR}/rc.hack ${DESTDIR}/alphadist/rc.hack
	cp -p ${DESTDIR}/alphadist/rc.hack ${DESTDIR}/etc/rc
	umount ${DESTDIR}/usr ${DESTDIR}
	/bin/rm -f $@
	dd if=/dev/r${DESTDISK}c bs=`expr ${SECPERCYL} \* 512` \
	    count=${CYLS} | gzip -9 > $@

bin.tar.gz: mount-fs build-fs
	/bin/rm -f $@
	(cd ${DESTDIR} ; find . | grep -v '^./etc' | \
	    grep -v '^./usr/X11R6' | grep -v '^./netbsd' | \
	    grep -v '^./boot' | pax -w -d | gzip -9) > $@

etc.tar.gz: mount-fs build-fs
	/bin/rm -f $@
	(cd ${DESTDIR} ; find ./etc | pax -w -d | gzip -9) > $@

x11.tar.gz: mount-fs build-fs
	/bin/rm -f $@
	(cd ${DESTDIR} ; find ./usr/X11R6 | pax -w -d | gzip -9) > $@

.endif

_SUBDIRUSE:

# clean dependencies in case somebody decides to shortcut the build.
clean:
	/bin/rm -f ${TARGETS}
	/bin/rm -f mount-fs build-fs unmount-fs

.include <bsd.own.mk>
.include <bsd.obj.mk>

#	$OpenBSD$

REGRESS_TARGETS=symlink1

symlink1:
	rm  -f ${.OBJDIR}/{a,b}
	ln -sf b ${.OBJDIR}/a
	if systrace -ae -f ${.CURDIR}/ln.policy ln -s b ${.OBJDIR}/a; then \
		exit 1; \
	fi
	if test -e ${.OBJDIR}/b; then \
		exit 1; \
	fi

.PHONY: ${REGRESS_TARGETS}

CLEANFILES+=	a b

.include <bsd.regress.mk>

# $OpenBSD: Makefile.inc,v 1.1.1.1 2005/09/19 03:34:18 kurt Exp $

.include <bsd.obj.mk>

AA_DIR=${.CURDIR}/../libaa

AA_OBJDIR!=	if [ -d $(AA_DIR)/${__objdir} ]; then \
			echo "$(AA_DIR)/${__objdir}"; \
		else \
			echo "$(AA_DIR)"; \
		fi

BB_DIR=${.CURDIR}/../libbb

BB_OBJDIR!=	if [ -d $(BB_DIR)/${__objdir} ]; then \
			echo "$(BB_DIR)/${__objdir}"; \
		else \
			echo "$(BB_DIR)"; \
		fi

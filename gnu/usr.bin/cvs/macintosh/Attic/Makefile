# Generated automatically from Makefile.in by configure.
#### Under MacOS and CodeWarrier, we use project files, not
#### this makefile.  However, we need this file in order for 'make
#### dist' to work properly on Unix machines.

srcdir     = .
top_srcdir = ..
cvs_srcdir = ../src

SHELL = /bin/sh

prefix = 
exec_prefix = ${prefix}


HEADERS = \
  config.h \
  options.h \
  maccvs.pch
  
SOURCES = \
  macos_filesys.c \
  pwd.c \
  rcmd.c \
  run.c \
  server_if.c
  
DISTFILES = \
  ${HEADERS} \
  ${SOURCES} \
  README.MacCVS \
  Makefile.in \
  services \
  maccvs.rsrc.sit.hqx \
  maccvs165.patch \
  maccvs.projects.sit.hqx

all:

.PHONY: all install uninstall
all install uninstall:

.PHONY: tags TAGS
tags TAGS:

.PHONY: ls
ls:
	@echo ${DISTFILES}

.PHONY: clean distclean realclean mostlyclean
clean distclean realclean mostlyclean:

.PHONY: lint
lint:

.PHONY: dist-dir
dist-dir:
	mkdir ${DISTDIR}
	for i in ${DISTFILES}; do \
		ln $(srcdir)/$${i} ${DISTDIR}; \
	done
	ln Makefile ${DISTDIR}

clean:
	@echo make clean does nothing in macintosh subdir


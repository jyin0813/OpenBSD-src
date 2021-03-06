#	$OpenBSD: Makefile.inc,v 1.17 2009/06/01 23:18:24 miod Exp $

.PATH:	 ${SRCDIR}/sys ${SRCDIR}/arch/${MACHINE_CPU}

SRCS+=	uthread_error.c

.if exists(${SRCDIR}/arch/${MACHINE_CPU}/_atomic_lock.c)
SRCS+=	_atomic_lock.c
.endif
.if exists(${SRCDIR}/arch/${MACHINE_CPU}/uthread_machdep_asm.S)
SRCS+=	uthread_machdep_asm.S
.endif
.if exists(${SRCDIR}/arch/${MACHINE_CPU}/uthread_machdep.c)
SRCS+=	uthread_machdep.c
.endif

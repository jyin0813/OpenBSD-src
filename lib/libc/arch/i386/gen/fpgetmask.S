/* $OpenBSD$ */
/*
 * Written by J.T. Conklin, Apr 4, 1995
 * Public domain.
 */

#include <machine/asm.h>

ENTRY(fpgetmask)
	subl $4,%esp
	fnstcw (%esp)
	movl (%esp),%eax
	notl %eax
	andl $63,%eax
	addl $4,%esp
	ret

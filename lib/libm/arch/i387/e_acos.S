/*	$OpenBSD: e_acos.S,v 1.2 2005/08/02 11:17:31 espie Exp $ */
/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

#include <machine/asm.h>

/* acos = atan (sqrt(1 - x^2) / x) */
ENTRY(acos)
	fldl	4(%esp)			/* x */
	fst	%st(1)
	fmul	%st(0)			/* x^2 */
	fld1				
	fsubp				/* 1 - x^2 */
	fsqrt				/* sqrt (1 - x^2) */
	fxch	%st(1)
	fpatan
	ret

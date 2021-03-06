/*	$OpenBSD$ */
/*
 * Written by J.T. Conklin <jtc@NetBSD.org>.
 * Public domain.
 */

#include <machine/asm.h>

#include "abi.h"

/* A float's domain isn't large enough to require argument reduction. */
ENTRY(tanf)
	XMM_ONE_ARG_FLOAT_PROLOGUE
	flds	ARG_FLOAT_ONE
	fptan
	fstp	%st(0)
	XMM_FLOAT_EPILOGUE
	ret

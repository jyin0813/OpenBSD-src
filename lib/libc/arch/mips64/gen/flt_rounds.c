/*	$OpenBSD$ */
/*
 * Written by J.T. Conklin, Apr 11, 1995
 * Public domain.
 */

#include <sys/types.h>
#include <machine/float.h>

static const int map[] = {
	1,	/* round to nearest */
	0,	/* round to zero */
	2,	/* round to positive infinity */
	3	/* round to negative infinity */
};

int
__flt_rounds()
{
	int x;

	__asm__("cfc1 %0,$31" : "=r" (x));
	__asm__("nop");
	return map[x & 0x03];
}

/*	$OpenBSD$	*/

/*
 * Written by J.T. Conklin, Apr 10, 1995
 * Public domain.
 */

#include <ieeefp.h>

fp_except
fpgetsticky()
{
	int x;

	__asm__("st %%fsr,%0" : "=m" (*&x));
	return (x >> 5) & 0x1f;
}

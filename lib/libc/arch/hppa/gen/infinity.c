/*	$OpenBSD: infinity.c,v 1.2 2001/01/24 08:19:02 mickey Exp $	*/

/* infinity.c */

#include <math.h>

/* bytes for +Infinity on a hppa */
char __infinity[] __attribute__((__aligned__(sizeof(double)))) =
    { 0x7f, 0xf0, 0, 0, 0, 0, 0, 0 };

/* mc88110 __mpn_addmul_1 -- Multiply a limb vector with a single limb and
store the product in a second limb vector.

Copyright (C) 1996 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.
*/

/* INPUT PARAMETERS
res_ptr	r2
s1_ptr	r3
size		r4
s2_limb	r5
*/

#include "sysdep.h"

	text
	align	16
	global	C_SYMBOL_NAME(__mpn_addmul_1)
C_SYMBOL_NAME(__mpn_addmul_1):
	lda	 r3,r3[r4]
	lda	 r8,r2[r4]		/* RES_PTR in r8 since r2 is retval */
	subu	 r4,r0,r4
	addu.co	 r2,r0,r0		/* r2 = cy = 0 */

	ld	 r6,r3[r4]
	addu	 r4,r4,1
	subu	 r8,r8,4
	bcnd.n	 eq0,r4,Lend
	 mulu.d	 r10,r6,r5

Loop:	ld	 r7,r8[r4]
	ld	 r6,r3[r4]
	addu.cio r9,r11,r2
	addu.ci	 r2,r10,r0
	addu.co	 r9,r9,r7
	st	 r9,r8[r4]
	addu	 r4,r4,1
	mulu.d	 r10,r6,r5
	bcnd	 ne0,r4,Loop

Lend:	ld	 r7,r8,0
	addu.cio r9,r11,r2
	addu.ci	 r2,r10,r0
	addu.co	 r9,r9,r7
	st	 r9,r8,0
	jmp.n	 r1
	 addu.ci r2,r2,r0

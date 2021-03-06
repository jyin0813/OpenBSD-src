/*	$OpenBSD$	*/

/* 
 * Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <float.h>

#if	LDBL_MANT_DIG == 113

/* (1 - 2^-113) 2^16384 */
long double MAXNUML = 1.189731495357231765085759326628007016196469e4932L;

/* 2^-113 */
long double MACHEPL = 9.629649721936179265279889712924636592690508e-35L;

/* (1 + 2^-112) 2^-16382 */
long double UFTHRESHL = 3.362103143112093506262677817321753250115591e-4932L;

/* 2^-16494 */
long double MINNUML = 6.475175119438025110924438958227646552499569e-4966L;

/* ln(MAXNUM) */
long double MAXLOGL = 1.1356523406294143949491931077970764891253E4L;

/* ln(MINNUM) */
long double MINLOGL = -1.143276959615573793352782661133116431383730e4L;

/* ln(UFTHRESH) */
/* long double MINLOGL = -1.135513711193302405887309661372784853802025e4L; */

long double PIL = 3.141592653589793238462643383279502884197169L;

long double PIO2L = 1.570796326794896619231321691639751442098585L;

long double PIO4L =  0.7853981633974483096156608458198757210492923L;

long double LOGE2L =  0.6931471805599453094172321214581765680755001L;

long double LOG2EL =  1.442695040888963407359924681001892137426646L;

long double INFINITYL = 1.0L / 0.0L;

#endif	/* LDBL_MANT_DIG == 113 */

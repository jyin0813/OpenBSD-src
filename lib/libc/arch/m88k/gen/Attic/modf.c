/*	$OpenBSD: modf.c,v 1.7 2005/08/07 16:40:14 espie Exp $	*/
/* @(#)s_modf.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/*
 * modf(double x, double *iptr) 
 * return fraction part of x, and return x's integral part in *iptr.
 * Method:
 *	Bit twiddling.
 *
 * Exception:
 *	No exception.
 */

#include "math.h"

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/*
 * from: @(#)fdlibm.h 5.1 93/09/24
 */

#ifndef _MATH_PRIVATE_H_
#define _MATH_PRIVATE_H_

#include <sys/types.h>
#include <machine/endian.h>

/* The original fdlibm code used statements like:
	n0 = ((*(int*)&one)>>29)^1;		* index of high word *
	ix0 = *(n0+(int*)&x);			* high word of x *
	ix1 = *((1-n0)+(int*)&x);		* low word of x *
   to dig two 32 bit words out of the 64 bit IEEE floating point
   value.  That is non-ANSI, and, moreover, the gcc instruction
   scheduler gets it wrong.  We instead use the following macros.
   Unlike the original code, we determine the endianness at compile
   time, not at run time; I don't see much benefit to selecting
   endianness at run time.  */

/* A union which permits us to convert between a double and two 32 bit
   ints.  */

/*
 * The arm32 port is little endian except for the FP word order which is
 * big endian.
 */

#if (BYTE_ORDER == BIG_ENDIAN) || defined(arm32)

typedef union 
{
  double value;
  struct 
  {
    u_int32_t msw;
    u_int32_t lsw;
  } parts;
} ieee_double_shape_type;

#endif

#if (BYTE_ORDER == LITTLE_ENDIAN) && !defined(arm32)

typedef union 
{
  double value;
  struct 
  {
    u_int32_t lsw;
    u_int32_t msw;
  } parts;
} ieee_double_shape_type;

#endif

/* Get two 32 bit ints from a double.  */

#define EXTRACT_WORDS(ix0,ix1,d)				\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)					\
do {								\
  ieee_double_shape_type gh_u;					\
  gh_u.value = (d);						\
  (i) = gh_u.parts.msw;						\
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)					\
do {								\
  ieee_double_shape_type gl_u;					\
  gl_u.value = (d);						\
  (i) = gl_u.parts.lsw;						\
} while (0)

/* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.parts.msw = (ix0);					\
  iw_u.parts.lsw = (ix1);					\
  (d) = iw_u.value;						\
} while (0)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)					\
do {								\
  ieee_double_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)					\
do {								\
  ieee_double_shape_type sl_u;					\
  sl_u.value = (d);						\
  sl_u.parts.lsw = (v);						\
  (d) = sl_u.value;						\
} while (0)

/* A union which permits us to convert between a float and a 32 bit
   int.  */

typedef union
{
  float value;
  u_int32_t word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

/* ieee style elementary functions */
extern double __ieee754_sqrt(double);			
extern double __ieee754_acos(double);			
extern double __ieee754_acosh(double);			
extern double __ieee754_log(double);			
extern double __ieee754_atanh(double);			
extern double __ieee754_asin(double);			
extern double __ieee754_atan2(double,double);			
extern double __ieee754_exp(double);
extern double __ieee754_cosh(double);
extern double __ieee754_fmod(double,double);
extern double __ieee754_pow(double,double);
extern double __ieee754_lgamma_r(double,int *);
extern double __ieee754_gamma_r(double,int *);
extern double __ieee754_lgamma(double);
extern double __ieee754_gamma(double);
extern double __ieee754_log10(double);
extern double __ieee754_sinh(double);
extern double __ieee754_hypot(double,double);
extern double __ieee754_j0(double);
extern double __ieee754_j1(double);
extern double __ieee754_y0(double);
extern double __ieee754_y1(double);
extern double __ieee754_jn(int,double);
extern double __ieee754_yn(int,double);
extern double __ieee754_remainder(double,double);
extern int    __ieee754_rem_pio2(double,double*);
extern double __ieee754_scalb(double,double);

/* fdlibm kernel function */
extern double __kernel_standard(double,double,int);	
extern double __kernel_sin(double,double,int);
extern double __kernel_cos(double,double);
extern double __kernel_tan(double,double,int);
extern int    __kernel_rem_pio2(double*,double*,int,int,int,const int*);


/* ieee style elementary float functions */
extern float __ieee754_sqrtf(float);			
extern float __ieee754_acosf(float);			
extern float __ieee754_acoshf(float);			
extern float __ieee754_logf(float);			
extern float __ieee754_atanhf(float);			
extern float __ieee754_asinf(float);			
extern float __ieee754_atan2f(float,float);			
extern float __ieee754_expf(float);
extern float __ieee754_coshf(float);
extern float __ieee754_fmodf(float,float);
extern float __ieee754_powf(float,float);
extern float __ieee754_lgammaf_r(float,int *);
extern float __ieee754_gammaf_r(float,int *);
extern float __ieee754_lgammaf(float);
extern float __ieee754_gammaf(float);
extern float __ieee754_log10f(float);
extern float __ieee754_sinhf(float);
extern float __ieee754_hypotf(float,float);
extern float __ieee754_j0f(float);
extern float __ieee754_j1f(float);
extern float __ieee754_y0f(float);
extern float __ieee754_y1f(float);
extern float __ieee754_jnf(int,float);
extern float __ieee754_ynf(int,float);
extern float __ieee754_remainderf(float,float);
extern int   __ieee754_rem_pio2f(float,float*);
extern float __ieee754_scalbf(float,float);

/* float versions of fdlibm kernel functions */
extern float __kernel_sinf(float,float,int);
extern float __kernel_cosf(float,float);
extern float __kernel_tanf(float,float,int);
extern int   __kernel_rem_pio2f(float*,float*,int,int,int,const int*);

#endif /* _MATH_PRIVATE_H_ */
#ifdef __STDC__
static const double one = 1.0;
#else
static double one = 1.0;
#endif

#ifdef __STDC__
	double modf(double x, double *iptr)
#else
	double modf(x, iptr)
	double x,*iptr;
#endif
{
	int32_t i0,i1,j0;
	u_int32_t i;
	EXTRACT_WORDS(i0,i1,x);
	j0 = ((i0>>20)&0x7ff)-0x3ff;	/* exponent of x */
	if(j0<20) {			/* integer part in high x */
	    if(j0<0) {			/* |x|<1 */
		INSERT_WORDS(*iptr,i0&0x80000000,0);	/* *iptr = +-0 */
		return x;
	    } else {
		i = (0x000fffff)>>j0;
		if(((i0&i)|i1)==0) {		/* x is integral */
		    u_int32_t high;
		    *iptr = x;
		    GET_HIGH_WORD(high,x);
		    INSERT_WORDS(x,high&0x80000000,0);	/* return +-0 */
		    return x;
		} else {
		    INSERT_WORDS(*iptr,i0&(~i),0);
		    return x - *iptr;
		}
	    }
	} else if (j0>51) {		/* no fraction part */
	    u_int32_t high;
	    *iptr = x*one;
	    GET_HIGH_WORD(high,x);
	    INSERT_WORDS(x,high&0x80000000,0);	/* return +-0 */
	    return x;
	} else {			/* fraction part in low x */
	    i = ((u_int32_t)(0xffffffff))>>(j0-20);
	    if((i1&i)==0) { 		/* x is integral */
		u_int32_t high;
		*iptr = x;
		GET_HIGH_WORD(high,x);
		INSERT_WORDS(x,high&0x80000000,0);	/* return +-0 */
		return x;
	    } else {
		INSERT_WORDS(*iptr,i0,i1&(~i));
		return x - *iptr;
	    }
	}
}

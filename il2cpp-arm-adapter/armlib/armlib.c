typedef __builtin_va_list va_list;
typedef unsigned long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned long uintptr_t;
typedef unsigned char uint8_t;
typedef unsigned long size_t;
typedef signed int int32_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;

#define va_start(l,a) __builtin_va_start(l,a)
#define va_end(l) __builtin_va_end(l)
#define va_arg(l,a) __builtin_va_arg(l,a)

#define NANOPRINTF_IMPLEMENTATION 1
#define NANOPRINTF_VISIBILITY_STATIC 1
#include "nanoprintf.h"

int vsprintf(char *dest, const char *format, va_list args) {
    return npf_vsnprintf(dest, (size_t)-1, format, args);
}

int vsnprintf(char *dest, size_t size, const char *format, va_list args) {
    return npf_vsnprintf(dest, size, format, args);
}

int sprintf(char *dest, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int result = vsprintf(dest, format, args);

    va_end(args);

    return result;
}

int snprintf(char *dest, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int result = vsnprintf(dest, size, format, args);

    va_end(args);

    return result;
}

typedef union
{
  double value;
  struct
  {
    u_int32_t lsw;
    u_int32_t msw;
  } parts;
  struct
  {
    u_int64_t w;
  } xparts;
} ieee_double_shape_type;

#define EXTRACT_WORDS(ix0,ix1,d)				\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0)
/* Get a 64-bit int from a double. */
#define EXTRACT_WORD64(ix,d)					\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix) = ew_u.xparts.w;						\
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
/* Set a double from a 64-bit int. */
#define INSERT_WORD64(d,ix)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.xparts.w = (ix);						\
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

/*
 * A union which permits us to convert between a float and a 32 bit
 * int.
 */
typedef union
{
  float value;
  /* FIXME: Assumes 32 bit int.  */
  unsigned int word;
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

#define	nan_mix(x, y)		(nan_mix_op((x), (y), +))
#define	nan_mix_op(x, y, op)	(((x) + 0.0L) op ((y) + 0))


/*
 * modf(double x, double *iptr)
 * return fraction part of x, and return x's integral part in *iptr.
 * Method:
 *	Bit twiddling.
 *
 * Exception:
 *	No exception.
 */

double
modf(double x, double *iptr)
{
static const double one = 1.0;
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
	    if (j0 == 0x400) {		/* inf/NaN */
		*iptr = x;
		return 0.0 / x;
	    }
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
float
fmodf(float x, float y)
{

static const float one = 1.0, Zero[] = {0.0, -0.0,};
	int32_t n,hx,hy,hz,ix,iy,sx,i;
	GET_FLOAT_WORD(hx,x);
	GET_FLOAT_WORD(hy,y);
	sx = hx&0x80000000;		/* sign of x */
	hx ^=sx;		/* |x| */
	hy &= 0x7fffffff;	/* |y| */
    /* purge off exception values */
	if(hy==0||(hx>=0x7f800000)||		/* y=0,or x not finite */
	   (hy>0x7f800000))			/* or y is NaN */
	    return nan_mix_op(x, y, *)/nan_mix_op(x, y, *);
	if(hx<hy) return x;			/* |x|<|y| return x */
	if(hx==hy)
	    return Zero[(u_int32_t)sx>>31];	/* |x|=|y| return x*0*/
    /* determine ix = ilogb(x) */
	if(hx<0x00800000) {	/* subnormal x */
	    for (ix = -126,i=(hx<<8); i>0; i<<=1) ix -=1;
	} else ix = (hx>>23)-127;
    /* determine iy = ilogb(y) */
	if(hy<0x00800000) {	/* subnormal y */
	    for (iy = -126,i=(hy<<8); i>=0; i<<=1) iy -=1;
	} else iy = (hy>>23)-127;
    /* set up {hx,lx}, {hy,ly} and align y to x */
	if(ix >= -126)
	    hx = 0x00800000|(0x007fffff&hx);
	else {		/* subnormal x, shift x to normal */
	    n = -126-ix;
	    hx = hx<<n;
	}
	if(iy >= -126)
	    hy = 0x00800000|(0x007fffff&hy);
	else {		/* subnormal y, shift y to normal */
	    n = -126-iy;
	    hy = hy<<n;
	}
    /* fix point fmod */
	n = ix - iy;
	while(n--) {
	    hz=hx-hy;
	    if(hz<0){hx = hx+hx;}
	    else {
	    	if(hz==0) 		/* return sign(x)*0 */
		    return Zero[(u_int32_t)sx>>31];
	    	hx = hz+hz;
	    }
	}
	hz=hx-hy;
	if(hz>=0) {hx=hz;}
    /* convert back to floating value and restore the sign */
	if(hx==0) 			/* return sign(x)*0 */
	    return Zero[(u_int32_t)sx>>31];
	while(hx<0x00800000) {		/* normalize x */
	    hx = hx+hx;
	    iy -= 1;
	}
	if(iy>= -126) {		/* normalize output */
	    hx = ((hx-0x00800000)|((iy+127)<<23));
	    SET_FLOAT_WORD(x,hx|sx);
	} else {		/* subnormal output */
	    n = -126 - iy;
	    hx >>= n;
	    SET_FLOAT_WORD(x,hx|sx);
	    x *= one;		/* create necessary signal */
	}
	return x;		/* exact output */
}

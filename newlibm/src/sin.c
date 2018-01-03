/* origin: FreeBSD /usr/src/lib/msun/src/s_sin.c */
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
/* sin(x)
 * Return sine function of x.
 *
 * kernel function:
 *      ____sin            ... sine function on [-pi/4,pi/4]
 *      ____cos            ... cose function on [-pi/4,pi/4]
 *      ____rem_pio2       ... argument reduction routine
 *
 * Method.
 *      Let S,C and T denote the sin, cos and tan respectively on
 *      [-PI/4, +PI/4]. Reduce the argument x to y1+y2 = x-k*pi/2
 *      in [-pi/4 , +pi/4], and let n = k mod 4.
 *      We have
 *
 *          n        sin(x)      cos(x)        tan(x)
 *     ----------------------------------------------------------
 *          0          S           C             T
 *          1          C          -S            -1/T
 *          2         -S          -C             T
 *          3         -C           S            -1/T
 *     ----------------------------------------------------------
 *
 * Special cases:
 *      Let trig be any of sin, cos, or tan.
 *      trig(+-INF)  is NaN, with signals;
 *      trig(NaN)    is that NaN;
 *
 * Accuracy:
 *      TRIG(x) returns trig(x) nearly rounded
 */

#include "libm.h"

double sin(double x)
{
	double y[2], z=0.0;
	int32_t n, ix;

	/* High word of x. */
	GET_HIGH_WORD(ix, x);

	/* |x| ~< pi/4 */
	ix &= 2147483647;
	if (ix <= 1072243195)
	{ 
		if (ix < 1045430272) {  /* |x| < 2**-26 */
			/* raise inexact if x != 0 */
			if ((int)x == 0)
				return x;
		}
		return ____sin(x, z, 0);
	}

	/* sin(Inf or NaN) is NaN */
	if (ix >=2146435072)
		return x - x;

	/* argument reduction needed */
	n = ____rem_pio2(x, y);
	switch (n&3)
	{
		/* 4 cases likely means that these are quadrants -- CM Graff */
		case 0: return  ____sin(y[0], y[1], 1);
		case 1: return  ____cos(y[0], y[1]);
		case 2: return -____sin(y[0], y[1], 1);
		default:
			return -____cos(y[0], y[1]);
	}
}

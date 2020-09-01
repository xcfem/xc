/* pltvwv.f -- translated by f2c (version 20160102).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "f2c.h"

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltvwv_(real *pll, real *pur, integer *n, integer *mask, 
	real *px, real *py, real *qx, real *qy)
{
    /* Initialized data */

    static integer izbit[32] = { 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,
	    8192,16384,32768,65536,131072,262144,524288,1048576,2097152,
	    4194304,8388608,16777216,33554432,67108864,134217728,268435456,
	    536870912,1073741824,2147483648 };

    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer j, k, m, j1;
    static real x1, y1, x2, y2, df;
    static integer jb;
    static real fp, fq;
    static integer km, jn;
    static real dx, dy, tn, pll1, pll2, pur1, pur2;

    /* Parameter adjustments */
    --qy;
    --qx;
    --py;
    --px;
    --mask;
    --pur;
    --pll;

    /* Function Body */
    pur1 = pur[1] + (float)1e-4;
    pur2 = pur[2] + (float)1e-4;
    pll1 = pll[1] - (float)1e-4;
    pll2 = pll[2] - (float)1e-4;
    dx = pur1 - pll1;
    dy = pur2 - pll2;
    j = 0;
    km = 0;
L10:
    if (j < *n) {
/* Computing MIN */
	i__1 = *n - j;
	jn = min(i__1,32);
	j1 = j;
	j += jn;
	++km;
	m = mask[km];
	if (m != 0) {
	    i__1 = jn;
	    for (k = 1; k <= i__1; ++k) {
		jb = izbit[k - 1];
		if ((m & jb) != 0) {
		    x1 = px[k + j1];
		    y1 = py[k + j1];
		    x2 = qx[k + j1];
		    y2 = qy[k + j1];
		    fp = x1 - pll1;
		    fq = x2 - pll1;
		    if (fp < (float)0. && fq < (float)0.) {
			m &= ~ jb;
		    } else if (fp > dx && fq > dx) {
			m &= ~ jb;
		    } else {
			df = fq - fp;
			if (df > (float)0.) {
			    tn = (y2 - y1) / df;
			    if (fp < (float)0.) {
				x1 = pll1;
				y1 -= fp * tn;
			    }
			    if (fq > dx) {
				x2 = pur1;
				y2 += (dx - fq) * tn;
			    }
			} else if (df < (float)0.) {
			    tn = (y2 - y1) / df;
			    if (fq < (float)0.) {
				x2 = pll1;
				y2 -= fq * tn;
			    }
			    if (fp > dx) {
				x1 = pur1;
				y1 += (dx - fp) * tn;
			    }
			}
			fp = y1 - pll2;
			fq = y2 - pll2;
			if (fp < (float)0. && fq < (float)0.) {
			    m &= ~ jb;
			} else if (fp > dy && fq > dy) {
			    m &= ~ jb;
			} else {
			    df = fq - fp;
			    if (df > (float)0.) {
				tn = (x2 - x1) / df;
				if (fp < (float)0.) {
				    y1 = pll2;
				    x1 -= fp * tn;
				}
				if (fq > dy) {
				    y2 = pur2;
				    x2 += (dy - fq) * tn;
				}
			    } else if (df < (float)0.) {
				tn = (x2 - x1) / df;
				if (fq < (float)0.) {
				    y2 = pll2;
				    x2 -= fq * tn;
				}
				if (fp > dy) {
				    y1 = pur2;
				    x1 += (dy - fp) * tn;
				}
			    }
			    px[k + j1] = x1;
			    py[k + j1] = y1;
			    qx[k + j1] = x2;
			    qy[k + j1] = y2;
			}
		    }
		}
/* L20: */
	    }
	    mask[km] = m;
	}
	goto L10;
    }
    return 0;
} /* pltvwv_ */

#ifdef __cplusplus
	}
#endif

/* pltcv2.f -- translated by f2c (version 20160102).
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
//#include "f2c.h"
#include "paving.h"

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltcv2_(integer *n, integer *mask, real *px, real *py, 
	real *qx, real *qy, real *ppx, real *ppy, real *qqx, real *qqy, real *
	c1, real *c2)
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
    static real x1, y1, x2, y2;
    static integer jb;
    static real fp, fq;
    static integer km, jn;
    static real cx, cy, dx, dy, xl;

    /* Parameter adjustments */
    --c2;
    --c1;
    --qqy;
    --qqx;
    --ppy;
    --ppx;
    --qy;
    --qx;
    --py;
    --px;
    --mask;

    /* Function Body */
    cx = c1[1];
    cy = c1[2];
    dx = c2[1] - cx;
    dy = c2[2] - cy;
    j = 0;
    km = 0;
L2100:
    if (! (j < *n)) {
	goto L2110;
    }
/* Computing MIN */
    i__1 = *n - j;
    jn = int_min(i__1,32);
    j1 = j;
    ++km;
    j += jn;
    m = mask[km];
    if (m == 0) {
	goto L2100;
    }
    i__1 = jn;
    for (k = 1; k <= i__1; ++k) {
	jb = izbit[k - 1];
	if ((jb & m) == 0) {
	    goto L2120;
	}
	x1 = px[j1 + k];
	y1 = py[j1 + k];
	x2 = qx[j1 + k];
	y2 = qy[j1 + k];
	fp = (y1 - cy) * dx - (x1 - cx) * dy;
	fq = (y2 - cy) * dx - (x2 - cx) * dy;
	if (fp < (float)0. && fq < (float)0.) {
	    m &= ~ jb;
	} else if (fp < (float)0.) {
	    xl = fq / (fq - fp);
	    x1 = x2 + xl * (x1 - x2);
	    y1 = y2 + xl * (y1 - y2);
	} else if (fq < (float)0.) {
	    xl = fp / (fp - fq);
	    x2 = x1 + xl * (x2 - x1);
	    y2 = y1 + xl * (y2 - y1);
	}
	ppx[k + j1] = x1;
	ppy[k + j1] = y1;
	qqx[k + j1] = x2;
	qqy[k + j1] = y2;
L2120:
	;
    }
    mask[km] = m;
    goto L2100;
L2110:
    return 0;
} /* pltcv2_ */

#ifdef __cplusplus
	}
#endif

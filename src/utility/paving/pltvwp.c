/* pltvwp.f -- translated by f2c (version 20160102).
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
#include "paving.h"

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltvwp_(real *pll, real *pur, integer *n, integer *mask, 
	real *px, real *py)
{
    /* Initialized data */

    static integer izbit[32] = { 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,
	    8192,16384,32768,65536,131072,262144,524288,1048576,2097152,
	    4194304,8388608,16777216,33554432,67108864,134217728,268435456,
	    536870912,1073741824,2147483648 };

    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer j, k, j1, km, jn;
    static real pll1, pll2, pur1, pur2;

    /* Parameter adjustments */
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
    j = 0;
    km = 0;
L2300:
    if (! (j < *n)) {
	goto L2310;
    }
/* Computing MIN */
    i__1 = *n - j;
    jn = int_min(i__1,32);
    j1 = j;
    j += jn;
    ++km;
    if (mask[km] == 0) {
	goto L2300;
    }
    i__1 = jn;
    for (k = 1; k <= i__1; ++k) {
	if ((mask[km] & izbit[k - 1]) == 0) {
	    goto L2320;
	}
	if (px[k + j1] < pll1 || px[k + j1] > pur1) {
	    mask[km] &= ~ izbit[k - 1];
	    goto L2320;
	}
	if (py[k + j1] < pll2 || py[k + j1] > pur2) {
	    mask[km] &= ~ izbit[k - 1];
	    goto L2320;
	}
L2320:
	;
    }
    goto L2300;
L2310:
    return 0;
} /* pltvwp_ */

#ifdef __cplusplus
	}
#endif

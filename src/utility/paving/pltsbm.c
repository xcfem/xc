/* pltsbm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int pltsbm_(integer *n, integer *mask, real *x, real *y, 
	char *symb, ftnlen symb_len)
{
    /* Initialized data */

    static integer izbit[32] = { 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,
	    8192,16384,32768,65536,131072,262144,524288,1048576,2097152,
	    4194304,8388608,16777216,33554432,67108864,134217728,268435456,
	    536870912,1073741824,2147483648 };

    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer j, k, m, j1, km, jn;
    extern /* Subroutine */ int pltxts_(real *, real *, char *, ftnlen);

    /* Parameter adjustments */
    --y;
    --x;
    --mask;

    /* Function Body */
    j = 0;
    km = 0;
L2170:
    if (! (j < *n)) {
	goto L2180;
    }
/* Computing MIN */
    i__1 = *n - j;
    jn = min(i__1,32);
    ++km;
    m = mask[km];
    j1 = j;
    j += jn;
    if (m == 0) {
	goto L2170;
    }
    i__1 = jn;
    for (k = 1; k <= i__1; ++k) {
	if ((m & izbit[k - 1]) != 0) {
	    pltxts_(&x[j1 + k], &y[j1 + k], symb, symb_len);
	}
/* L2190: */
    }
    goto L2170;
L2180:
    return 0;
} /* pltsbm_ */

#ifdef __cplusplus
	}
#endif

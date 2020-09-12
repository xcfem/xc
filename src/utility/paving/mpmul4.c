/* mpmul4.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int mpmul4_(integer *n, integer *mask, xc_float *arr1, xc_float *
	arr2, xc_float *arr3, xc_float *arr4, xc_float *mat, xc_float *res1, xc_float *res2, xc_float 
	*res3, xc_float *res4)
{
    /* Initialized data */

    static integer izbit[32] = { 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,
	    8192,16384,32768,65536,131072,262144,524288,1048576,2097152,
	    4194304,8388608,16777216,33554432,67108864,134217728,268435456,
	    536870912,1073741824,2147483648 };

    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

    /* Parameter adjustments */
    --res4;
    --res3;
    --res2;
    --res1;
    mat -= 5;
    --arr4;
    --arr3;
    --arr2;
    --arr1;

    /* Function Body */
    if (*mask == 0) {
	return 0;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((*mask & izbit[i__ - 1]) != 0) {
	    res1[i__] = mat[5] * arr1[i__] + mat[6] * arr2[i__] + mat[7] * 
		    arr3[i__] + mat[8] * arr4[i__];
	    res2[i__] = mat[9] * arr1[i__] + mat[10] * arr2[i__] + mat[11] * 
		    arr3[i__] + mat[12] * arr4[i__];
	    res3[i__] = mat[13] * arr1[i__] + mat[14] * arr2[i__] + mat[15] * 
		    arr3[i__] + mat[16] * arr4[i__];
	    res4[i__] = mat[17] * arr1[i__] + mat[18] * arr2[i__] + mat[19] * 
		    arr3[i__] + mat[20] * arr4[i__];
	}
/* L3160: */
    }
    return 0;
} /* mpmul4_ */

#ifdef __cplusplus
	}
#endif

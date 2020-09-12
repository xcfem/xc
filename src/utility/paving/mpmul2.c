/* mpmul2.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int mpmul2_(integer *n, real *x0, real *y0, real *mat, real *
	res1, real *res2, real *res3, real *res4)
{
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
    --y0;
    --x0;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	res1[i__] = mat[5] * x0[i__] + mat[6] * y0[i__] + mat[8];
	res2[i__] = mat[9] * x0[i__] + mat[10] * y0[i__] + mat[12];
	res3[i__] = mat[13] * x0[i__] + mat[14] * y0[i__] + mat[16];
	res4[i__] = mat[17] * x0[i__] + mat[18] * y0[i__] + mat[20];
/* L3040: */
    }
    return 0;
} /* mpmul2_ */

#ifdef __cplusplus
	}
#endif

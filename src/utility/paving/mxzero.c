/* mxzero.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int mxzero_(integer *n, real *mat)
{
    /* System generated locals */
    integer mat_dim1, mat_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j;

    /* Parameter adjustments */
    mat_dim1 = *n;
    mat_offset = 1 + mat_dim1;
    mat -= mat_offset;

    /* Function Body */
    if (*n == 4) {
	mat[mat_dim1 + 1] = (float)0.;
	mat[(mat_dim1 << 1) + 1] = (float)0.;
	mat[mat_dim1 * 3 + 1] = (float)0.;
	mat[(mat_dim1 << 2) + 1] = (float)0.;
	mat[mat_dim1 + 2] = (float)0.;
	mat[(mat_dim1 << 1) + 2] = (float)0.;
	mat[mat_dim1 * 3 + 2] = (float)0.;
	mat[(mat_dim1 << 2) + 2] = (float)0.;
	mat[mat_dim1 + 3] = (float)0.;
	mat[(mat_dim1 << 1) + 3] = (float)0.;
	mat[mat_dim1 * 3 + 3] = (float)0.;
	mat[(mat_dim1 << 2) + 3] = (float)0.;
	mat[mat_dim1 + 4] = (float)0.;
	mat[(mat_dim1 << 1) + 4] = (float)0.;
	mat[mat_dim1 * 3 + 4] = (float)0.;
	mat[(mat_dim1 << 2) + 4] = (float)0.;
    } else {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		mat[i__ + j * mat_dim1] = (float)0.;
/* L3050: */
	    }
/* L3030: */
	}
    }
    return 0;
} /* mxzero_ */

#ifdef __cplusplus
	}
#endif

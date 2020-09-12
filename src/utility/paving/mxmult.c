/* mxmult.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int mxmult_(integer *n, real *mat1, real *mat2, real *matr)
{
    /* System generated locals */
    integer mat1_dim1, mat1_offset, mat2_dim1, mat2_offset, matr_dim1, 
	    matr_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j, k;

    /* Parameter adjustments */
    matr_dim1 = *n;
    matr_offset = 1 + matr_dim1;
    matr -= matr_offset;
    mat2_dim1 = *n;
    mat2_offset = 1 + mat2_dim1;
    mat2 -= mat2_offset;
    mat1_dim1 = *n;
    mat1_offset = 1 + mat1_dim1;
    mat1 -= mat1_offset;

    /* Function Body */
    if (*n == 4) {
	matr[matr_dim1 + 1] = mat1[mat1_dim1 + 1] * mat2[mat2_dim1 + 1] + 
		mat1[(mat1_dim1 << 1) + 1] * mat2[mat2_dim1 + 2] + mat1[
		mat1_dim1 * 3 + 1] * mat2[mat2_dim1 + 3] + mat1[(mat1_dim1 << 
		2) + 1] * mat2[mat2_dim1 + 4];
	matr[(matr_dim1 << 1) + 1] = mat1[mat1_dim1 + 1] * mat2[(mat2_dim1 << 
		1) + 1] + mat1[(mat1_dim1 << 1) + 1] * mat2[(mat2_dim1 << 1) 
		+ 2] + mat1[mat1_dim1 * 3 + 1] * mat2[(mat2_dim1 << 1) + 3] + 
		mat1[(mat1_dim1 << 2) + 1] * mat2[(mat2_dim1 << 1) + 4];
	matr[matr_dim1 * 3 + 1] = mat1[mat1_dim1 + 1] * mat2[mat2_dim1 * 3 + 
		1] + mat1[(mat1_dim1 << 1) + 1] * mat2[mat2_dim1 * 3 + 2] + 
		mat1[mat1_dim1 * 3 + 1] * mat2[mat2_dim1 * 3 + 3] + mat1[(
		mat1_dim1 << 2) + 1] * mat2[mat2_dim1 * 3 + 4];
	matr[(matr_dim1 << 2) + 1] = mat1[mat1_dim1 + 1] * mat2[(mat2_dim1 << 
		2) + 1] + mat1[(mat1_dim1 << 1) + 1] * mat2[(mat2_dim1 << 2) 
		+ 2] + mat1[mat1_dim1 * 3 + 1] * mat2[(mat2_dim1 << 2) + 3] + 
		mat1[(mat1_dim1 << 2) + 1] * mat2[(mat2_dim1 << 2) + 4];
	matr[matr_dim1 + 2] = mat1[mat1_dim1 + 2] * mat2[mat2_dim1 + 1] + 
		mat1[(mat1_dim1 << 1) + 2] * mat2[mat2_dim1 + 2] + mat1[
		mat1_dim1 * 3 + 2] * mat2[mat2_dim1 + 3] + mat1[(mat1_dim1 << 
		2) + 2] * mat2[mat2_dim1 + 4];
	matr[(matr_dim1 << 1) + 2] = mat1[mat1_dim1 + 2] * mat2[(mat2_dim1 << 
		1) + 1] + mat1[(mat1_dim1 << 1) + 2] * mat2[(mat2_dim1 << 1) 
		+ 2] + mat1[mat1_dim1 * 3 + 2] * mat2[(mat2_dim1 << 1) + 3] + 
		mat1[(mat1_dim1 << 2) + 2] * mat2[(mat2_dim1 << 1) + 4];
	matr[matr_dim1 * 3 + 2] = mat1[mat1_dim1 + 2] * mat2[mat2_dim1 * 3 + 
		1] + mat1[(mat1_dim1 << 1) + 2] * mat2[mat2_dim1 * 3 + 2] + 
		mat1[mat1_dim1 * 3 + 2] * mat2[mat2_dim1 * 3 + 3] + mat1[(
		mat1_dim1 << 2) + 2] * mat2[mat2_dim1 * 3 + 4];
	matr[(matr_dim1 << 2) + 2] = mat1[mat1_dim1 + 2] * mat2[(mat2_dim1 << 
		2) + 1] + mat1[(mat1_dim1 << 1) + 2] * mat2[(mat2_dim1 << 2) 
		+ 2] + mat1[mat1_dim1 * 3 + 2] * mat2[(mat2_dim1 << 2) + 3] + 
		mat1[(mat1_dim1 << 2) + 2] * mat2[(mat2_dim1 << 2) + 4];
	matr[matr_dim1 + 3] = mat1[mat1_dim1 + 3] * mat2[mat2_dim1 + 1] + 
		mat1[(mat1_dim1 << 1) + 3] * mat2[mat2_dim1 + 2] + mat1[
		mat1_dim1 * 3 + 3] * mat2[mat2_dim1 + 3] + mat1[(mat1_dim1 << 
		2) + 3] * mat2[mat2_dim1 + 4];
	matr[(matr_dim1 << 1) + 3] = mat1[mat1_dim1 + 3] * mat2[(mat2_dim1 << 
		1) + 1] + mat1[(mat1_dim1 << 1) + 3] * mat2[(mat2_dim1 << 1) 
		+ 2] + mat1[mat1_dim1 * 3 + 3] * mat2[(mat2_dim1 << 1) + 3] + 
		mat1[(mat1_dim1 << 2) + 3] * mat2[(mat2_dim1 << 1) + 4];
	matr[matr_dim1 * 3 + 3] = mat1[mat1_dim1 + 3] * mat2[mat2_dim1 * 3 + 
		1] + mat1[(mat1_dim1 << 1) + 3] * mat2[mat2_dim1 * 3 + 2] + 
		mat1[mat1_dim1 * 3 + 3] * mat2[mat2_dim1 * 3 + 3] + mat1[(
		mat1_dim1 << 2) + 3] * mat2[mat2_dim1 * 3 + 4];
	matr[(matr_dim1 << 2) + 3] = mat1[mat1_dim1 + 3] * mat2[(mat2_dim1 << 
		2) + 1] + mat1[(mat1_dim1 << 1) + 3] * mat2[(mat2_dim1 << 2) 
		+ 2] + mat1[mat1_dim1 * 3 + 3] * mat2[(mat2_dim1 << 2) + 3] + 
		mat1[(mat1_dim1 << 2) + 3] * mat2[(mat2_dim1 << 2) + 4];
	matr[matr_dim1 + 4] = mat1[mat1_dim1 + 4] * mat2[mat2_dim1 + 1] + 
		mat1[(mat1_dim1 << 1) + 4] * mat2[mat2_dim1 + 2] + mat1[
		mat1_dim1 * 3 + 4] * mat2[mat2_dim1 + 3] + mat1[(mat1_dim1 << 
		2) + 4] * mat2[mat2_dim1 + 4];
	matr[(matr_dim1 << 1) + 4] = mat1[mat1_dim1 + 4] * mat2[(mat2_dim1 << 
		1) + 1] + mat1[(mat1_dim1 << 1) + 4] * mat2[(mat2_dim1 << 1) 
		+ 2] + mat1[mat1_dim1 * 3 + 4] * mat2[(mat2_dim1 << 1) + 3] + 
		mat1[(mat1_dim1 << 2) + 4] * mat2[(mat2_dim1 << 1) + 4];
	matr[matr_dim1 * 3 + 4] = mat1[mat1_dim1 + 4] * mat2[mat2_dim1 * 3 + 
		1] + mat1[(mat1_dim1 << 1) + 4] * mat2[mat2_dim1 * 3 + 2] + 
		mat1[mat1_dim1 * 3 + 4] * mat2[mat2_dim1 * 3 + 3] + mat1[(
		mat1_dim1 << 2) + 4] * mat2[mat2_dim1 * 3 + 4];
	matr[(matr_dim1 << 2) + 4] = mat1[mat1_dim1 + 4] * mat2[(mat2_dim1 << 
		2) + 1] + mat1[(mat1_dim1 << 1) + 4] * mat2[(mat2_dim1 << 2) 
		+ 2] + mat1[mat1_dim1 * 3 + 4] * mat2[(mat2_dim1 << 2) + 3] + 
		mat1[(mat1_dim1 << 2) + 4] * mat2[(mat2_dim1 << 2) + 4];
    } else {
	i__1 = *n;
	for (k = 1; k <= i__1; ++k) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		matr[k + j * matr_dim1] = (float)0.;
/* L200: */
	    }
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = *n;
		for (j = 1; j <= i__3; ++j) {
		    matr[k + j * matr_dim1] += mat1[k + i__ * mat1_dim1] * 
			    mat2[i__ + j * mat2_dim1];
/* L210: */
		}
/* L220: */
	    }
/* L230: */
	}
    }
    return 0;
} /* mxmult_ */

#ifdef __cplusplus
	}
#endif

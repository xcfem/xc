/* dstort.f -- translated by f2c (version 20160102).
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
#include "fastq_cpp.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int dstort_(real *x1, real *x2, real *x3, real *x4, real *y1,
	 real *y2, real *y3, real *y4, real *value)
{
    /* System generated locals */
    real r__1, r__2, r__3, r__4;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real c11, c12, c22, xj11, xj12, xj21, xj22, factor, determ;

/* *********************************************************************** */
/*  SUBROUTINE DSTORT = CALCULATES A DISTORTION METRIC FOR AN ELEMENT */
/*                    USING THE IDEAS IN THE PAPER BY ODDY, 1988. */
/* *********************************************************************** */
/*  SETUP THE JACOBIAN MATRIX */
    xj11 = *x1 * (float).125 + *x2 * (float).375 - *x3 * (float).375 - *x4 * (
	    float).125;
    xj12 = *y1 * (float).125 + *y2 * (float).375 - *y3 * (float).375 - *y4 * (
	    float).125;
    xj21 = -(*x1 * (float).375) + *x2 * (float).375 + *x3 * (float).125 - *x4 
	    * (float).125;
    xj22 = -(*y1 * (float).375) + *y2 * (float).375 + *y3 * (float).125 - *y4 
	    * (float).125;
/*  NORMALIZE THE JACOBIAN WITH RESPECT TO THE ELEMENT SIZE */
    determ = xj11 * xj22 - xj12 * xj21;
    if (determ <= (float)0.) {
	*value = (float)1e10;
	return 0;
    }
    factor = (float)1. / sqrt(determ);
    xj11 *= factor;
    xj12 *= factor;
    xj21 *= factor;
    xj22 *= factor;
/*  NOW USE THE SECOND INVARIANT OF GREEN'S STRAIN */
    c11 = xj11 * xj11 + xj21 * xj21;
    c12 = xj11 * xj12 + xj21 * xj22;
    c22 = xj12 * xj12 + xj22 * xj22;
/* Computing 2nd power */
    r__1 = c11;
/* Computing 2nd power */
    r__2 = c12;
/* Computing 2nd power */
    r__3 = c22;
/* Computing 2nd power */
    r__4 = c11 + c22;
    *value = r__1 * r__1 + r__2 * r__2 * (float)2. + r__3 * r__3 - r__4 * 
	    r__4 * (float).5;
    *value = dmax(*value,(float)0.);
    return 0;
} /* dstort_ */

#ifdef __cplusplus
	}
#endif

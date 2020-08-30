/* mnorm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int mnorm_(integer *mxnd, real *xn, real *yn, integer *nxl, 
	integer *lll, real *stdlen)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real d__;
    static integer l;
    static real s;
    static integer n1, n2;
    static real tol;
    static integer numl;

/* *********************************************************************** */
/*  SUBROUTINE MNORM = FINDS THE AVERAGE LENGTH OF THOSE LINES NOT MUCH */
/*                     LONGER THAN THE AVERAGE */
/* *********************************************************************** */
    /* Parameter adjustments */
    nxl -= 3;
    --yn;
    --xn;

    /* Function Body */
    *stdlen = (float)0.;
    numl = 0;
    s = (float)0.;
    i__1 = *lll;
    for (l = 1; l <= i__1; ++l) {
	n1 = nxl[(l << 1) + 1];
	n2 = nxl[(l << 1) + 2];
	if (n1 > 0) {
/* Computing 2nd power */
	    r__1 = xn[n1] - xn[n2];
/* Computing 2nd power */
	    r__2 = yn[n1] - yn[n2];
	    d__ = sqrt(r__1 * r__1 + r__2 * r__2);
	    s += d__;
	    ++numl;
	}
/* L100: */
    }
    if (numl <= 0) {
	return 0;
    }
    tol = s * (float)1.25 / (doublereal) numl;
    numl = 0;
    s = (float)0.;
    i__1 = *lll;
    for (l = 1; l <= i__1; ++l) {
	n1 = nxl[(l << 1) + 1];
	n2 = nxl[(l << 1) + 2];
	if (n1 > 0) {
/* Computing 2nd power */
	    r__1 = xn[n1] - xn[n2];
/* Computing 2nd power */
	    r__2 = yn[n1] - yn[n2];
	    d__ = sqrt(r__1 * r__1 + r__2 * r__2);
	    if (d__ < tol) {
		s += d__;
		++numl;
	    }
	}
/* L110: */
    }
    *stdlen = s / (doublereal) numl;
    return 0;
} /* mnorm_ */

#ifdef __cplusplus
	}
#endif

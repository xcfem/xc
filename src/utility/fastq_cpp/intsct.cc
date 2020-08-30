/* intsct.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int intsct_(real *x1, real *y1, real *x2, real *y2, real *x3,
	 real *y3, real *x4, real *y4, real *u, real *w, logical *lcross)
{
    static real xa, ya, xb, yb, xc, yc, xd, yd, denom;

/* *********************************************************************** */
/*  SUBROUTINE INTSCT = CHECKS TO SEE IF THE LINE FROM N1 TO N2 */
/*                      INTERSECTS THE LINE FROM N3 TO N4 */
/* *********************************************************************** */
/*  NOTE:  THIS INTERSECTION ROUTINE IS BASED ON AN ALGORITHM GIVEN */
/*         IN THE BOOK "GEOMETRIC MODELING" BY MICHAEL E. MORTENSON ON */
/*         PAGES 319 - 320. */
/* *********************************************************************** */
    *lcross = FALSE_;
    if (dmax(*x1,*x2) < dmin(*x3,*x4)) {
	return 0;
    }
    if (dmax(*y1,*y2) < dmin(*y3,*y4)) {
	return 0;
    }
    if (dmax(*x3,*x4) < dmin(*x1,*x2)) {
	return 0;
    }
    if (dmax(*y3,*y4) < dmin(*y1,*y2)) {
	return 0;
    }
/*  SET UP THE FIRST LINE'S VECTORS (A AND B) */
    xa = *x1;
    ya = *y1;
    xb = *x2 - *x1;
    yb = *y2 - *y1;
/*  SET UP THE SECOND LINE'S VECTORS (C AND D) */
    xc = *x3;
    yc = *y3;
    xd = *x4 - *x3;
    yd = *y4 - *y3;
/*  NOW USE THE VECTORS AND SOLVE FOR W. */
/*  W IS THE PROPORTION OF THE DISTANCE ALONG THE VECTOR D */
/*  WHERE THE INTERSECTION OCCURS.  LIKEWISE U IS THE PROPORTIONAL */
/*  DISTANCE ALONG THE VECTOR B FOR THE INTERSECTION.   IF THERE IS */
/*  AN INTERSECTION, BOTH U AND W MUST BE BETWEEN 0 AND 1. */
    denom = yb * xd - xb * yd;
/*  CHECK FOR SPECIAL PARALLEL CASE - THE DENOMINATOR IS EQUAL TO ZERO. */
    if (denom != (float)0.) {
/*  CHECK FOR INTERSECTION */
	*w = (yc * xb - xb * ya - xc * yb + yb * xa) / denom;
	if (*w < (float)1. && *w > (float)0.) {
/*  W INDICATES AN INTERSECTION HAS OCCURRED. */
/*  GET THE U VALUE AND CONFIRM. */
	    if (xb != (float)0.) {
		*u = (xc + *w * xd - xa) / xb;
	    } else {
		*u = (yc + *w * yd - ya) / yb;
	    }
	    if (*u < (float)1. && *u > (float)0.) {
		*lcross = TRUE_;
	    }
	}
    }
    return 0;
} /* intsct_ */

#ifdef __cplusplus
	}
#endif

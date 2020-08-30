/* tabint.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int tabint_(real *x1, real *x2, real *y1, real *y2, real *ct,
	 real *st, real *scale, real *xx1, real *yy1, real *xx2, real *yy2, 
	logical *drwtab)
{
    /* System generated locals */
    real r__1, r__2, r__3, r__4;

    /* Builtin functions */
    double atan2(doublereal, doublereal), cos(doublereal), sin(doublereal), 
	    sqrt(doublereal);

    /* Local variables */
    static real theta, scalex, scaley;

/* *********************************************************************** */
/*     SUBROUTINE TABINT = INITIALIZES THE TABLET TO THE PLOT LIMITS */
/* *********************************************************************** */
    if (*drwtab) {
	theta = atan2(*yy2 - *yy1, *xx2 - *xx1) - atan2(*y2 - *y1, *x2 - *x1);
	*ct = cos(theta);
	*st = sin(theta);
/* Computing 2nd power */
	r__1 = *x2 - *x1;
/* Computing 2nd power */
	r__2 = *y2 - *y1;
/* Computing 2nd power */
	r__3 = *xx2 - *xx1;
/* Computing 2nd power */
	r__4 = *yy2 - *yy1;
	*scale = sqrt((r__1 * r__1 + r__2 * r__2) / (r__3 * r__3 + r__4 * 
		r__4));
    } else {
	*ct = (float)1.;
	*st = (float)0.;
	*xx1 = (float)2e3;
	*xx2 = (float)1.5e4;
	*yy1 = (float)2e3;
	*yy2 = (float)1e4;
	scalex = (*x2 - *x1) / (*xx2 - *xx1);
	scaley = (*y2 - *y1) / (*yy2 - *yy1);
	if (scalex > scaley) {
	    *scale = scalex;
	    *yy1 = *yy2 - *yy1 - (*y2 - *y1) / *scale;
	} else {
	    *scale = scaley;
	    *xx1 = *xx2 - *xx1 - (*x2 - *x1) / *scale;
	}
    }
    return 0;
} /* tabint_ */

#ifdef __cplusplus
	}
#endif

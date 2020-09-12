/* dlpara.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int dlpara_(real *x1, real *y1, real *x2, real *y2, real *xm,
	 real *b, logical *bad)
{
    /* System generated locals */
    real r__1;

/* *********************************************************************** */
/*  SUBROUTINE DLPARA = DETERMINES LINE PARAMETERS FROM TWO POINTS */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     INREGN = INPUTS REGION CONNECTIVITIES */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     X1    = X VALUE OF POINT 1 */
/*     X2    = X VALUE OF POINT 2 */
/*     Y1    = Y VALUE OF POINT 1 */
/*     Y2    = Y VALUE OF POINT 2 */
/*     XM    = THE SLOPE OF A STRIGHT LINE BETWEEN POINT 1 AND 2 */
/*     B     = THE Y INTERCEPT OF THE STRAIGHT LINE BETWEEN POINT 1 AND 2 */
/* *********************************************************************** */
    if ((r__1 = *x2 - *x1, dbl_abs(r__1)) < (float)1e-6) {
	*bad = TRUE_;
	*b = *x1;
    } else {
	*bad = FALSE_;
	*xm = (*y2 - *y1) / (*x2 - *x1);
	*b = *y1 - *x1 * *xm;
    }
    return 0;
} /* dlpara_ */

#ifdef __cplusplus
	}
#endif

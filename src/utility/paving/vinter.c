/* vinter.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int vinter_(integer *mxnd, real *xn, real *yn, integer *n1, 
	integer *n2, integer *n3, real *xold, real *yold, real *xnew, real *
	ynew, logical *vcross)
{
    static real u, w, xa, ya, xb, yb, xc, yc, xd, yd, denom;

/* *********************************************************************** */
/*  SUBROUTINE VINTER = FINDS WHERE A VECTOR FROM N1 TO N2 */
/*                      INTERSECTS THE VECTOR FROM N3 TO (XOLD, YOLD) */
/* *********************************************************************** */
/*  NOTE:  THIS INTERSECTION ROUTINE IS BASED ON AN ALGORITHM GIVEN */
/*         IN THE BOOK "GEOMETRIC MODELING" BY MICHAEL E. MORTENSON ON */
/*         PAGES 319 - 320. */
/* *********************************************************************** */
    /* Parameter adjustments */
    --yn;
    --xn;

    /* Function Body */
    *vcross = FALSE_;
/*  SET UP THE FIRST LINE'S VECTORS (A AND B) */
    xa = xn[*n1];
    ya = yn[*n1];
    xb = xn[*n2] - xn[*n1];
    yb = yn[*n2] - yn[*n1];
/*  SET UP THE SECOND LINE'S VECTORS (C AND D) */
    xc = xn[*n3];
    yc = yn[*n3];
    xd = *xold - xn[*n3];
    yd = *yold - yn[*n3];
/*  NOW USE THE VECTORS AND SOLVE FOR W. */
/*  W IS THE PROPORTION OF THE DISTANCE ALONG THE VECTOR D */
/*  WHERE THE INTERSECTION OCCURS.  LIKEWISE U IS THE PROPORTIONAL */
/*  DISTANCE ALONG THE VECTOR B FOR THE INTERSECTION. */
    denom = yb * xd - xb * yd;
/*  CHECK FOR SPECIAL PARALLEL CASE - THE DENOMINATOR IS EQUAL TO ZERO. */
    if (denom != (float)0.) {
/*  GET INTERSECTION LOCATION */
	w = (yc * xb - xb * ya - xc * yb + yb * xa) / denom;
/*  GET THE U VALUE TO CONFIRM. */
	if (xb != (float)0.) {
	    u = (xc + w * xd - xa) / xb;
	} else {
	    u = (yc + w * yd - ya) / yb;
	}
/*  CALCULATE THE INTERSECTION POINT BASED ON SIMILAR TRIANGLES */
	*xnew = (xa + xb * u + (xc + xd * w)) * (float).5;
	*ynew = (ya + yb * u + (yc + yd * w)) * (float).5;
	*vcross = TRUE_;
    }
    return 0;
} /* vinter_ */

#ifdef __cplusplus
	}
#endif

/* extnd1.f -- translated by f2c (version 20160102).
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
//#include "f2c.h"
#include "paving.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int extnd1_(integer *mxnd, real *xn, real *yn, real *angle, 
	integer *n1, integer *n2, integer *n3, real *x, real *y, real *dist)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal), sin(doublereal), 
	    cos(doublereal);

    /* Local variables */
    static real ang, cang, dist1, dist2, adist;

/* *********************************************************************** */
/*  SUBROUTINE EXCORN = CALCULATES A POSITION AN AVERAGE LENGTH AWAY */
/*                      FROM A CORNER NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --angle;
    --yn;
    --xn;
    --x;
    --y;

    /* Function Body */
    cang = angle[*n2] * (float).5;
    ang = atan2(yn[*n1] - yn[*n2], xn[*n1] - xn[*n2]) - cang;
/* Computing 2nd power */
    r__1 = yn[*n2] - yn[*n1];
/* Computing 2nd power */
    r__2 = xn[*n2] - xn[*n1];
    dist1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = yn[*n3] - yn[*n2];
/* Computing 2nd power */
    r__2 = xn[*n3] - xn[*n2];
    dist2 = sqrt(r__1 * r__1 + r__2 * r__2);
    *dist = (dist1 + dist2) * (float).5;
    if (cang == (float)0.) {
	adist = *dist;
    } else {
	adist = *dist / sin(cang);
    }
    x[1] = adist * cos(ang) + xn[*n2];
    y[1] = adist * sin(ang) + yn[*n2];
    return 0;
} /* extnd1_ */

#ifdef __cplusplus
	}
#endif

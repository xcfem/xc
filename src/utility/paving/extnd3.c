/* extnd3.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int extnd3_(integer *mxnd, xc_float *xn, xc_float *yn, xc_float *angle, 
	integer *n1, integer *n2, integer *n3, xc_float *x, xc_float *y, xc_float *dist)
{
    /* System generated locals */
    xc_float r__1, r__2;

    /* Local variables */
    static xc_float ang, ang1, ang2, ang3, cang, dist1, dist2, adist;

/* *********************************************************************** */
/*  SUBROUTINE EXTND3 = CALCULATES TWO POSITIONS AN AVERAGE LENGTH AWAY */
/*                      FROM A CORNER NODE AND ONE AT 1/3 ANGLE INTERVALS */
/* *********************************************************************** */
    /* Parameter adjustments */
    --yn;
    --xn;
    --x;
    --y;

    /* Function Body */
    ang = atan2(yn[*n1] - yn[*n2], xn[*n1] - xn[*n2]);
    cang = *angle / (xc_float)3.;
    ang1 = ang - cang;
    ang2 = ang - *angle * (xc_float).5;
    ang3 = ang - cang * (xc_float)2.;
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
    *dist = (dist1 + dist2) * (xc_float).5;
    if (cang == (xc_float)0.) {
	adist = *dist;
    } else {
	adist = *dist / sin(cang);
    }
    x[1] = adist * cos(ang1) + xn[*n2];
    y[1] = adist * sin(ang1) + yn[*n2];
    x[2] = adist * (xc_float)1.4142 * cos(ang2) + xn[*n2];
    y[2] = adist * (xc_float)1.4142 * sin(ang2) + yn[*n2];
    x[3] = adist * cos(ang3) + xn[*n2];
    y[3] = adist * sin(ang3) + yn[*n2];
    return 0;
} /* extnd3_ */

#ifdef __cplusplus
	}
#endif

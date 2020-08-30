/* exside.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int exside_(integer *mxnd, real *xn, real *yn, integer *
	lnodes, real *angle, integer *n1, real *xnew, real *ynew)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal), cos(doublereal), 
	    sin(doublereal);

    /* Local variables */
    static integer n0, n2;
    static real ang, dist, dist1, dist2;

/* *********************************************************************** */
/*  SUBROUTINE EXSIDE = CALCULATES A POSITION AN AVERAGE LENGTH AWAY */
/*                      FROM A SIDE NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --angle;
    lnodes -= 8;
    --yn;
    --xn;

    /* Function Body */
    n0 = lnodes[*n1 * 7 + 2];
    n2 = lnodes[*n1 * 7 + 3];
    ang = atan2(yn[n2] - yn[*n1], xn[n2] - xn[*n1]) + angle[*n1] * (float).5;
/* Computing 2nd power */
    r__1 = yn[n2] - yn[*n1];
/* Computing 2nd power */
    r__2 = xn[n2] - xn[*n1];
    dist1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = yn[n0] - yn[*n1];
/* Computing 2nd power */
    r__2 = xn[n0] - xn[*n1];
    dist2 = sqrt(r__1 * r__1 + r__2 * r__2);
    dist = (dist1 + dist2) * (float).5;
    *xnew = dist * cos(ang) + xn[*n1];
    *ynew = dist * sin(ang) + yn[*n1];
    return 0;
} /* exside_ */

#ifdef __cplusplus
	}
#endif

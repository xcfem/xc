/* exdsct.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int exdsct_(integer *mxnd, real *xn, real *yn, integer *
	lnodes, real *angle, integer *n1, real *xnew, real *ynew)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal), cos(doublereal), 
	    sin(doublereal);

    /* Local variables */
    static integer n0, n2;
    static real ang, pid2, dist, dist1, dist2;

/* *********************************************************************** */
/*  SUBROUTINE EXCORN = CALCULATES A POSITION AN AVERAGE LENGTH AWAY */
/*                      FROM A DISSECTION NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --angle;
    lnodes -= 8;
    --yn;
    --xn;

    /* Function Body */
    n0 = lnodes[*n1 * 7 + 2];
    n2 = lnodes[*n1 * 7 + 3];
/*      XNEW = XN (N0) + XN (N2) - XN (N1) */
/*      YNEW = YN (N0) + YN (N2) - YN (N1) */
    pid2 = atan2((float)0., (float)-1.) * (float).5;
    ang = atan2(yn[n0] - yn[*n1], xn[n0] - xn[*n1]) - (angle[*n1] - pid2);
/*     &   (2. * ANGLE(N1) / 3.) */
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
} /* exdsct_ */

#ifdef __cplusplus
	}
#endif

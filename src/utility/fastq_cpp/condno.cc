/* condno.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int condno_(integer *mxnd, integer *nodes, real *qrat, real *
	srat, real *cond, real *sides, real *xn, real *yn, logical *lside)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer n1, n2, n3, n4;
    static real ss1, ss2, ss3, ss4, amin, bmin, amax, bmax;

/* *********************************************************************** */
/*  SUBROUTINE CONDNO = COMPUTES EVALUATOR FUNCTIONS FOR RESTRUCTURING */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     LSIDE = .TRUE. IF SIDES ARRAY IS TO BE FILLED */
/*     NODES = THE FOUR NODES OF THE ELEMENT */
/*     QRAT  = THE RATIO OF SMALLEST TO LARGEST ANGLE */
/*     SRAT  = THE RATIO OF SMALLEST TO LARGEST SIDE */
/*     COND  = SRAT*QRAT */
/* *********************************************************************** */
    /* Parameter adjustments */
    --yn;
    --xn;
    --nodes;
    --sides;

    /* Function Body */
    n1 = nodes[1];
    n2 = nodes[2];
    n3 = nodes[3];
    n4 = nodes[4];
/* Computing 2nd power */
    r__1 = xn[n1] - xn[n2];
/* Computing 2nd power */
    r__2 = yn[n1] - yn[n2];
    ss1 = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
    r__1 = xn[n2] - xn[n3];
/* Computing 2nd power */
    r__2 = yn[n2] - yn[n3];
    ss2 = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
    r__1 = xn[n3] - xn[n4];
/* Computing 2nd power */
    r__2 = yn[n3] - yn[n4];
    ss3 = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
    r__1 = xn[n4] - xn[n1];
/* Computing 2nd power */
    r__2 = yn[n4] - yn[n1];
    ss4 = r__1 * r__1 + r__2 * r__2;
    amax = dmax(ss1,ss3);
    amin = dmin(ss1,ss3);
    bmax = dmax(ss2,ss4);
    bmin = dmin(ss2,ss4);
    if (amin * bmin > (float)0.) {
	*srat = sqrt(sqrt(amax * bmax / (amin * bmin)));
    } else {
	*srat = (float)1e10;
    }
    *cond = *qrat * *srat;
    if (*lside) {
	sides[1] = sqrt(ss1);
	sides[2] = sqrt(ss2);
	sides[3] = sqrt(ss3);
	sides[4] = sqrt(ss4);
    }
    return 0;
} /* condno_ */

#ifdef __cplusplus
	}
#endif

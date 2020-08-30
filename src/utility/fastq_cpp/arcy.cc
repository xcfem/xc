/* arcy.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int arcy_(real *xcen, real *ycen, real *theta1, real *theta2,
	 real *xk, real *xa, real *x, real *ytry, logical *err)
{
    /* Builtin functions */
    double exp(doublereal), sin(doublereal);

    /* Local variables */
    static real f1, f2, ang, ang2, ytry2, theta;
    extern doublereal solve_(real *, real *, real *, real *, real *, real *, 
	    real *);
    static real anginc;
    extern doublereal spiral_(real *, real *, real *, real *, real *, real *);

/* *********************************************************************** */
/*  SUBROUTINE ARCY = ITERATIVELY SOLVES THE LOGARITHMIC SPIRAL PROBLEM */
/*                    TO DETERMINE A Y VALUE GIVEN AN X THAT INTERSECTS */
/*                    THE ARC */
/* *********************************************************************** */
/*  START WITH 10 INCREMENTS, EACH PASS INCREMENTS DECREASE TEN FOLD */
    anginc = (*theta2 - *theta1) * (float).05;
    ang = *theta1;
    f1 = spiral_(xa, xk, x, xcen, ycen, &ang);
    if (f1 == (float)0.) {
	theta = ang;
	goto L110;
    }
    ang2 = ang + anginc;
L100:
    f2 = spiral_(xa, xk, x, xcen, ycen, &ang2);
    if (f2 == (float)0.) {
	theta = ang2;
    } else if (f1 * f2 < (float)0.) {
	theta = solve_(xa, xk, x, xcen, ycen, &ang, &ang2);
    } else {
	ang = ang2;
	ang2 += anginc;
	if (ang2 <= *theta2) {
	    goto L100;
	}
	*err = TRUE_;
	goto L120;
    }
L110:
    *ytry = *xa * exp(*xk * theta) * sin(theta) + *ycen;
L120:
/*  FIND THE SECOND ROOT IF THE FIRST ONE HAS BEEN LOCATED */
    if (! (*err)) {
	ang = theta + anginc;
	f1 = spiral_(xa, xk, x, xcen, ycen, &ang);
	f2 = spiral_(xa, xk, x, xcen, ycen, theta2);
	if (f1 == (float)0.) {
	    theta = ang;
	} else if (f2 == (float)0.) {
	    theta = *theta2;
	} else if (f1 * f2 < (float)0.) {
	    theta = solve_(xa, xk, x, xcen, ycen, &ang, theta2);
	} else {
	    goto L130;
	}
    }
    ytry2 = *xa * exp(*xk * theta) * sin(theta) + *ycen;
    *ytry = dmax(*ytry,ytry2);
L130:
    return 0;
} /* arcy_ */

#ifdef __cplusplus
	}
#endif

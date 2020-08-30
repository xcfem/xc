/* qaaval.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int qaaval_(integer *mxnd, integer *nodes, real *angles, 
	real *qrat, real *area, real *xn, real *yn, logical *carea)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static integer i__, j, n1, n2, n3, n4;
    static real ag[4], pi, diff, qmin, qmax, twopi;

/* *********************************************************************** */
/*  SUBROUTINE QAAVAL = CALCULATES THE INTERIOR ANGLES OF A QUAD AND */
/*                      THE RATIO OF LARGEST TO SMALLEST ANGLE */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NODES  = THE FOUR NODES OF THE ELEMENT IN CCW ORDER */
/*     ANGLES = THE FOUR INTERIOR ANGLES IN THE ORDER 4-1, 1-2, 2-3, 3-4 */
/*     QRAT   = THE RATIO OF THE LARGEST TO THE SMALLEST ANGLE */
/*     CAREA  = .TRUE. IF THE AREA IS TO BE CALCULATED */
/*     AREA   = ELEMENT'S AREA */
/* *********************************************************************** */
    /* Parameter adjustments */
    --yn;
    --xn;
    --nodes;
    --angles;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    twopi = pi + pi;
    if (*carea) {
	n1 = nodes[1];
	n2 = nodes[2];
	n3 = nodes[3];
	n4 = nodes[4];
	*area = ((xn[n3] - xn[n1]) * (yn[n4] - yn[n2]) - (yn[n3] - yn[n1]) * (
		xn[n4] - xn[n2])) * (float).5;
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	if (i__ == 4) {
	    j = 1;
	} else {
	    j = i__ + 1;
	}
	n1 = nodes[i__];
	n2 = nodes[j];
	if (xn[n2] == xn[n1] && yn[n2] == yn[n1]) {
	    *qrat = (float)1e10;
	    return 0;
	} else {
	    ag[j - 1] = atan2(yn[n2] - yn[n1], xn[n2] - xn[n1]);
	}
/* L100: */
    }
    for (j = 1; j <= 4; ++j) {
	if (j == 1) {
	    i__ = 4;
	} else {
	    i__ = j - 1;
	}
	diff = ag[j - 1] - ag[i__ - 1];
	if (diff > pi) {
	    diff -= twopi;
	} else if (diff < -pi) {
	    diff += twopi;
	}
	angles[j] = pi - diff;
/* L110: */
    }
    qmin = angles[1];
    qmax = angles[1];
    for (i__ = 2; i__ <= 4; ++i__) {
/* Computing MIN */
	r__1 = qmin, r__2 = angles[i__];
	qmin = dmin(r__1,r__2);
/* Computing MAX */
	r__1 = qmax, r__2 = angles[i__];
	qmax = dmax(r__1,r__2);
/* L120: */
    }
    if (qmin > (float)0.) {
	*qrat = qmax / qmin;
    } else {
	*qrat = (float)1e10;
    }
    return 0;
} /* qaaval_ */

#ifdef __cplusplus
	}
#endif

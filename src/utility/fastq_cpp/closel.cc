/* closel.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int closel_(integer *mp, integer *ml, integer *n, real *coor,
	 integer *iline, integer *ltype, integer *lcon, integer *linkp, 
	integer *linkl, real *x, real *y, logical *bifind, integer *ifind, 
	logical *addcen, real *xchold, real *ychold)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal), exp(doublereal), 
	    cos(doublereal), sin(doublereal);

    /* Local variables */
    static integer i__;
    static real b1, b2;
    static integer i1, i2, i3, j1, j2, j3;
    static real r1, r2, x1, x2, y1, y2;
    static integer ii;
    static real pi, xa;
    static integer kt;
    static real xk, xm1, xm2;
    static logical bad;
    static integer icw;
    static logical err;
    static integer iccw;
    static real tang, xcen, ycen, dist, dtry, rtry, xtry, ytry, xhold, yhold, 
	    twopi, theta1, theta2;
    static logical addlnk;
    extern /* Subroutine */ int dlpara_(real *, real *, real *, real *, real *
	    , real *, logical *), arcpar_(integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, real *, real *, real *, real *, real *, logical *,
	     integer *, integer *, real *, real *);
    static real radius;
    extern /* Subroutine */ int linepr_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *);
    static real thetat;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE CLOSEL = FINDS CLOSEST PERPENDICULAR BISECTED LINE */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     BISECT   = */
/* *********************************************************************** */
/*  SUBROUTINES CALLED: */
/*     DLPARA = DETERMINES LINE PARAMETERS FROM TWO POINTS */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    linkl -= 3;
    lcon -= 4;
    --ltype;
    --iline;
    --n;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    twopi = pi + pi;
/*  FIND THE CLOSEST LINE ABOVE THE POINT INPUT */
    *bifind = FALSE_;
    addlnk = FALSE_;
    *ifind = 0;
    dist = (float)1e5;
    i__1 = n[19];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(ml, &linkl[3], &i__, &ii, &addlnk);
	if (ii > 0) {
	    kt = ltype[ii];
	    linepr_(ml, mp, &linkp[3], &lcon[4], &ii, &i1, &i2, &i3, &j1, &j2,
		     &j3);
	    if (j1 > 0 && j2 > 0) {
		if (kt == 1) {
/*  GET THE PARAMETERS FOR THE LINE */
		    dlpara_(&coor[(j1 << 1) + 1], &coor[(j1 << 1) + 2], &coor[
			    (j2 << 1) + 1], &coor[(j2 << 1) + 2], &xm1, &b1, &
			    bad);
/*  GET DISTANCE FOR VERTICAL LINE */
		    if (bad) {
			dtry = (r__1 = coor[(j1 << 1) + 1] - *x, dabs(r__1));
			xtry = coor[(j1 << 1) + 1];
			ytry = *y;
/*  GET DISTANCE FOR HORIZONTAL LINE */
		    } else if (dabs(xm1) < (float)1e-6) {
			dtry = (r__1 = coor[(j1 << 1) + 2] - *y, dabs(r__1));
			xtry = *x;
			ytry = coor[(j1 << 1) + 2];
/*  GET PERPENDICULAR DISTANCE TO ARBITRARY LINE */
		    } else {
			xm2 = (float)-1. / xm1;
			b2 = *y - xm2 * *x;
			xtry = (b2 - b1) / (xm1 - xm2);
			ytry = xm1 * xtry + b1;
/* Computing 2nd power */
			r__1 = *x - xtry;
/* Computing 2nd power */
			r__2 = *y - ytry;
			dtry = sqrt(r__1 * r__1 + r__2 * r__2);
		    }
		    if (dtry <= dist) {
/* Computing MIN */
			r__1 = coor[(j1 << 1) + 1], r__2 = coor[(j2 << 1) + 1]
				;
			x1 = dmin(r__1,r__2);
/* Computing MAX */
			r__1 = coor[(j1 << 1) + 1], r__2 = coor[(j2 << 1) + 1]
				;
			x2 = dmax(r__1,r__2);
/* Computing MIN */
			r__1 = coor[(j1 << 1) + 2], r__2 = coor[(j2 << 1) + 2]
				;
			y1 = dmin(r__1,r__2);
/* Computing MAX */
			r__1 = coor[(j1 << 1) + 2], r__2 = coor[(j2 << 1) + 2]
				;
			y2 = dmax(r__1,r__2);
			if (xtry >= x1 && xtry <= x2 && ytry >= y1 && ytry <= 
				y2) {
			    dist = dtry;
			    xhold = xtry;
			    yhold = ytry;
			    *ifind = i__;
			    *bifind = TRUE_;
			}
		    }
/*  CHECK DISTANCES TO CIRCULAR ARCS */
		} else if (kt == 3 || kt == 4 || kt == 6) {
/*  FIRST GET THETA1, THETA2, THETAT, R1, R2, AND RTRY */
		    arcpar_(mp, &kt, &iline[ii], &coor[3], &linkp[3], &j1, &
			    j2, &j3, &i3, &xcen, &ycen, &theta1, &theta2, &
			    tang, &r1, &r2, &err, &iccw, &icw, &xk, &xa);
		    if (*y == ycen && *x == xcen) {
			rtry = (float)0.;
			thetat = (theta1 + theta2) * (float).5;
		    } else {
			thetat = atan2(*y - ycen, *x - xcen);
/* Computing 2nd power */
			r__1 = *x - xcen;
/* Computing 2nd power */
			r__2 = *y - ycen;
			rtry = sqrt(r__1 * r__1 + r__2 * r__2);
		    }
/*  SEE IF THE POINT ANGLE IS WITHIN THE BEGINNING AND ENDING ANGLES */
		    if (thetat <= theta2 && thetat >= theta1 || thetat + 
			    twopi <= theta2 && thetat + twopi >= theta1) {
/*  CALCULATE THE ARC RADIUS AT THAT ANGLE */
			radius = xa * exp(xk * thetat);
			dtry = (r__1 = radius - rtry, dabs(r__1));
/*  CHECK TO SEE IF THE ARC IS THE CLOSEST */
			if (dtry <= dist) {
			    dist = dtry;
			    xhold = xcen + cos(thetat) * radius;
			    yhold = ycen + sin(thetat) * radius;
			    *ifind = i__;
			    if (kt == 4 || kt == 6) {
				*addcen = TRUE_;
				*xchold = xcen;
				*ychold = ycen;
			    } else {
				*addcen = FALSE_;
			    }
			    *bifind = TRUE_;
			}
		    }
		}
	    }
	}
/* L100: */
    }
    *x = xhold;
    *y = yhold;
    return 0;
} /* closel_ */

#ifdef __cplusplus
	}
#endif

/* labove.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int labove_(integer *mp, integer *ml, integer *n, integer *
	ipoint, real *coor, integer *iline, integer *ltype, integer *lcon, 
	integer *linkp, integer *linkl, real *x, real *y, real *y1, real *y2, 
	integer *ifind, integer *index, integer *ifirst, integer *inext)
{
    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal), exp(doublereal);

    /* Local variables */
    static integer i__;
    static real b1;
    static integer i1, i2, i3, j1, j2, j3;
    static real r1, r2;
    static integer ii;
    static real pi, xa, rp, xk;
    static logical up;
    static real xm1;
    static logical bad;
    static integer icw;
    static logical err;
    static integer iccw;
    static real tang, xcen, ycen;
    extern /* Subroutine */ int arcy_(real *, real *, real *, real *, real *, 
	    real *, real *, real *, logical *);
    static real dist, dtry, ytry, rtest, twopi, theta1, theta2;
    static logical addlnk;
    extern /* Subroutine */ int dlpara_(real *, real *, real *, real *, real *
	    , real *, logical *), arcpar_(integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, real *, real *, real *, real *, real *, logical *,
	     integer *, integer *, real *, real *);
    static real thetap;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE LABOVE = GETS THE CLOSEST LINE ABOVE A GIVEN X,Y */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     IFIND  = THE CLOSEST LINE FOUND ABOVE THE X,Y LOCATION */
/*     IFIRST = THE CCW END OF THE LINE (LEFT END) */
/*     INEXT  = THE CW END OF THE LINE (RIGHT END) */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    --ipoint;
    linkl -= 3;
    lcon -= 4;
    --ltype;
    --iline;
    --n;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    twopi = pi + pi;
    addlnk = FALSE_;
    dist = *y2 - *y1;
    i__1 = n[19];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(ml, &linkl[3], &i__, &ii, &addlnk);
	if (ii > 0) {
	    i1 = lcon[ii * 3 + 1];
	    i2 = lcon[ii * 3 + 2];
	    i3 = lcon[ii * 3 + 3];
	    ltsort_(mp, &linkp[3], &i1, &j1, &addlnk);
	    ltsort_(mp, &linkp[3], &i2, &j2, &addlnk);
	    if (i3 != 0) {
		i__2 = abs(i3);
		ltsort_(mp, &linkp[3], &i__2, &j3, &addlnk);
	    } else {
		j3 = 0;
	    }
/*  SEE IF LINE EXISTS */
	    if (j1 > 0 && j2 > 0) {
/*  CHECK A STRAIGHT LINE TO SEE IF IT SPANS THE POINT */
		if (ltype[ii] == 1 && (coor[(j1 << 1) + 1] <= *x && coor[(j2 
			<< 1) + 1] >= *x || coor[(j2 << 1) + 1] <= *x && coor[
			(j1 << 1) + 1] >= *x)) {
/*  SEE IF LINE IS ABOVE */
		    dlpara_(&coor[(j1 << 1) + 1], &coor[(j1 << 1) + 2], &coor[
			    (j2 << 1) + 1], &coor[(j2 << 1) + 2], &xm1, &b1, &
			    bad);
		    if (! bad) {
			ytry = xm1 * *x + b1;
			if (ytry > *y) {
/*  CHECK DISTANCE TO THE LINE - RECORD LINE NO. IF CLOSEST */
			    dtry = ytry - *y;
			    if (dtry <= dist || (r__1 = dist - dtry, dabs(
				    r__1)) < dist * (float).001) {
				dist = dtry;
				*ifind = i__;
				*index = ii;
				if (coor[(j1 << 1) + 1] > *x) {
				    *ifirst = i2;
				    *inext = i1;
				} else {
				    *ifirst = i1;
				    *inext = i2;
				}
			    }
			}
		    }
/*  CHECK AN ARC LINE */
		} else if ((ltype[ii] == 3 || ltype[ii] == 4 || ltype[ii] == 
			6) && j3 > 0) {
		    arcpar_(mp, &ltype[ii], &iline[ii], &coor[3], &linkp[3], &
			    j1, &j2, &j3, &i3, &xcen, &ycen, &theta1, &theta2,
			     &tang, &r1, &r2, &err, &iccw, &icw, &xk, &xa);
		    if (! err) {
/*  SET THE ARC AS EITHER AN UPWARD ARCH OR DOWNWARD ARCH */
/*  (A CLOSED CIRCLE IS ALWAYS AN UPWARD ARCH) */
			if (j1 == j2) {
			    up = TRUE_;
			} else if (coor[(iccw << 1) + 1] >= coor[(icw << 1) + 
				1]) {
			    up = TRUE_;
			} else {
			    up = FALSE_;
			}
/*  THE INPUT POINT IS RIGHT AT THE CENTER OF THE CIRCLE */
			if (*y == ycen && *x == xcen) {
			    rp = (float)0.;
			    thetap = (theta1 + theta2) * (float).5;
			} else {
			    thetap = atan2(*y - ycen, *x - xcen);
/* Computing 2nd power */
			    r__1 = *x - xcen;
/* Computing 2nd power */
			    r__2 = *y - ycen;
			    rp = sqrt(r__1 * r__1 + r__2 * r__2);
			}
/*  SEE IF THE POINT ANGLE IS WITHIN THE BEGINNING AND ENDING ANGLES */
			if (thetap <= theta2 && thetap >= theta1 || thetap + 
				twopi <= theta2 && thetap + twopi >= theta1 ||
				 thetap <= theta1 && thetap >= theta2 || 
				thetap + twopi <= theta1 && thetap + twopi >= 
				theta2) {
/*  SEE IF THE POINT TO CENTER DISTANCE IS LESS THAN THE */
/*  ARC RADIUS AT THAT ANGLE FOR AN UPWARD ARC OR GREATER FOR */
/*  A DOWNWARD ARC (BELOW THE ARC) */
			    rtest = xa * exp(xk * thetap);
			    if (up && rtest >= rp || ! up && rtest <= rp) {
/*  CHECK Y DISTANCE TO THE LINE - RECORD LINE NO. IF CLOSEST */
				arcy_(&xcen, &ycen, &theta1, &theta2, &xk, &
					xa, x, &ytry, &err);
				dtry = ytry - *y;
				if (! err && ytry > *y && (dtry <= dist || (
					r__1 = dist - dtry, dabs(r__1)) < 
					dist * (float).001)) {
				    dist = dtry;
				    *ifind = i__;
				    *index = ii;
				    if (up) {
					*ifirst = ipoint[icw];
					*inext = ipoint[iccw];
				    } else {
					*ifirst = ipoint[iccw];
					*inext = ipoint[icw];
				    }
				}
			    }
/*  THE ONLY OTHER ARC POSSIBILITY IS IF THE X FALLS IN THE SPAN */
/*  BETWEEN THE TWO ENDPOINTS OR BETWEEN END POINTS AND CENTER POINT */
			} else if ((*x - coor[(j1 << 1) + 1]) * (*x - coor[(
				j2 << 1) + 1]) < (float)0. || (*x - coor[(j1 
				<< 1) + 1]) * (*x - coor[(j3 << 1) + 1]) < (
				float)0. || (*x - coor[(j2 << 1) + 1]) * (*x 
				- coor[(j3 << 1) + 1]) < (float)0.) {
/*  CHECK Y DISTANCE TO THE LINE - RECORD LINE NO. IF CLOSEST */
			    arcy_(&xcen, &ycen, &theta1, &theta2, &xk, &xa, x,
				     &ytry, &err);
			    dtry = ytry - *y;
			    if (! err && ytry > *y && (dtry <= dist || (r__1 =
				     dist - dtry, dabs(r__1)) < dist * (float)
				    .001)) {
				dist = dtry;
				*ifind = i__;
				*index = ii;
/*  TREAT THE BETWEEN END POINTS DIFFERENTLY THAN BETWEEN */
/*  ENDPOINT AND CENTER POINT */
				if ((*x - coor[(j1 << 1) + 1]) * (*x - coor[(
					j2 << 1) + 1]) < (float)0.) {
				    if (up) {
					*ifirst = ipoint[icw];
					*inext = ipoint[iccw];
				    } else {
					*ifirst = ipoint[iccw];
					*inext = ipoint[icw];
				    }
				} else {
				    if (coor[(iccw << 1) + 2] > coor[(icw << 
					    1) + 2]) {
					*ifirst = ipoint[iccw];
					*inext = ipoint[icw];
				    } else {
					*ifirst = ipoint[icw];
					*inext = ipoint[iccw];
				    }
				}
			    }
			}
		    }
		}
	    }
	}
/* L100: */
    }
    return 0;
} /* labove_ */

#ifdef __cplusplus
	}
#endif

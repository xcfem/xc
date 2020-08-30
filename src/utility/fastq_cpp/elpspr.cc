/* elpspr.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int elpspr_(integer *mp, integer *kt, integer *knum, real *
	coor, integer *linkp, integer *ipntr1, integer *ipntr2, integer *
	ipntr3, integer *ip3, real *xcen, real *ycen, real *theta1, real *
	theta2, real *tang, integer *iccw, integer *icw, real *avalue, real *
	bvalue, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 CENTER POINT FOR LINE\002,i5,\002 LIES \
ON ONE OF\002,\002 THE ENDPOINTS\002)";
    static char fmt_10010[] = "(\002 POINTS GIVEN FOR LINE\002,i5,\002 DO NO\
T DEFINE AN ELIPSE\002)";

    /* System generated locals */
    integer i__1;
    real r__1, r__2, r__3;

    /* Builtin functions */
    double atan2(doublereal, doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    double sqrt(doublereal), acos(doublereal), sin(doublereal);

    /* Local variables */
    static real a2, d0, a7, a8, x1, y1, x2, y2, pi, vx, vy, theta, ratio, 
	    twopi;
    extern /* Subroutine */ int etheta_(real *, real *, real *, real *, real *
	    , real *, logical *);
    static real radmax, cvalue;
    extern /* Subroutine */ int pltflu_();

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___17 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE ELPSPR = THIS ROUTINE CALCULATES THE ELIPSE PARAMETERS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     TANG   = TOTAL ANGLE SCRIBED BY THE ARC */
/*     THETA1 = FIRST CCW ANGLE OF THE ARC */
/*     THETA2 = SECOND CCW ANGLE OF THE ARC */
/*     IPNTR1 = POINTER TO FIRST COORDINATE VALUE */
/*     IPNTR2 = POINTER TO SECOND COORDINATE VALUE */
/*     IPNTR3 = POINTER TO THIRD COORDINATE VALUE */
/*     IP3    = THE THIRD POINT NUMBER  (CAN BE NEGATED) */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    twopi = pi + pi;
    *err = FALSE_;
/*  ELIPSE GOES FROM 1ST POINT TO 2ND IN *COUNTER-CLOCKWISE* DIRECTION. */
    *xcen = coor[(*ipntr3 << 1) + 1];
    *ycen = coor[(*ipntr3 << 1) + 2];
/*  CHECK TO MAKE SURE THAT THE BEGINNING AND ENDING RADIUS EXIST */
    if (coor[(*ipntr1 << 1) + 1] == *xcen && coor[(*ipntr1 << 1) + 2] == *
	    ycen || coor[(*ipntr2 << 1) + 1] == *xcen && coor[(*ipntr2 << 1) 
	    + 2] == *ycen) {
	pltflu_();
	s_wsfe(&io___3);
	i__1 = abs(*knum);
	do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	e_wsfe();
	*err = TRUE_;
	goto L100;
    }
    *theta1 = atan2(coor[(*ipntr1 << 1) + 2] - *ycen, coor[(*ipntr1 << 1) + 1]
	     - *xcen);
    *theta2 = atan2(coor[(*ipntr2 << 1) + 2] - *ycen, coor[(*ipntr2 << 1) + 1]
	     - *xcen);
/*  NOW CALCULATE THE MAJOR AXIS (AVALUE) AND THE MINOR AXIS (BVALUE) */
    x1 = coor[(*ipntr1 << 1) + 1] - *xcen;
    y1 = coor[(*ipntr1 << 1) + 2] - *ycen;
    x2 = coor[(*ipntr2 << 1) + 1] - *xcen;
    y2 = coor[(*ipntr2 << 1) + 2] - *ycen;
/*  CHOOSE THE APPROPRIATE ELIPSE DEFINITION */
    if (y1 * y1 == y2 * y2) {
	*avalue = sqrt(x1 * x1 + y1 * y1);
	*bvalue = *avalue;
    } else if (y1 == (float)0. && x2 == (float)0.) {
	*avalue = x1;
	*bvalue = y2;
    } else if (y2 == (float)0. && x1 == (float)0.) {
	*avalue = x2;
	*bvalue = y1;
    } else {
	ratio = sqrt((r__1 = (x1 * x1 - x2 * x2) / (y1 * y1 - y2 * y2), dabs(
		r__1)));
	if (ratio == (float)0.) {
	    *avalue = sqrt(x1 * x1 + y1 * y1);
	    *bvalue = *avalue;
	} else if (ratio == (float)1.) {
	    *avalue = sqrt(x1 * x1 + y1 * y1);
	    *bvalue = *avalue;
	} else {
	    if (y2 == (float)0. || x2 == (float)0. && y1 != (float)0.) {
		if (ratio > (float)1.) {
		    vx = (float)1.;
/* Computing 2nd power */
		    r__1 = ratio;
		    vy = -((float)1. / (r__1 * r__1)) * (x1 / y1);
		} else {
		    vx = (float)1.;
/* Computing 2nd power */
		    r__1 = ratio;
		    vy = -(r__1 * r__1) * (x1 / y1);
		}
		d0 = sqrt(x1 * x1 + y1 * y1);
		a8 = acos((x1 * vx + y1 * vy) / (d0 * sqrt(vx * vx + vy * vy))
			);
		a7 = pi - a8;
		a2 = (r__1 = atan2(y1, x1), dabs(r__1));
		*theta1 = (r__1 = atan2(vy, (float)1.), dabs(r__1));
	    } else {
		if (ratio > (float)1.) {
		    vx = (float)1.;
/* Computing 2nd power */
		    r__1 = ratio;
		    vy = -((float)1. / (r__1 * r__1)) * (x2 / y2);
		} else {
		    vx = (float)1.;
/* Computing 2nd power */
		    r__1 = ratio;
		    vy = -(r__1 * r__1) * (x2 / y2);
		}
		d0 = sqrt(x2 * x2 + y2 * y2);
		a8 = acos((x2 * vx + y2 * vy) / (d0 * sqrt(vx * vx + vy * vy))
			);
		a7 = pi - a8;
		a2 = (r__1 = atan2(y2, x2), dabs(r__1));
		*theta1 = (r__1 = atan2(vy, (float)1.), dabs(r__1));
	    }
	    radmax = dmax(a7,a8);
	    etheta_(&a7, &a8, &a2, theta1, &radmax, &theta, err);
	    if (*err) {
		s_wsfe(&io___17);
		i__1 = abs(*knum);
		do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L100;
	    }
	    cvalue = d0 * sin(a8 - theta) / sin(a2 - a8 + theta);
/* Computing 2nd power */
	    r__2 = cvalue;
/* Computing 2nd power */
	    r__3 = ratio;
	    *bvalue = sqrt((r__1 = r__2 * r__2 / (r__3 * r__3 - 1), dabs(r__1)
		    ));
	    *avalue = *bvalue * ratio;
	}
    }
/*  NOW GET THE ANGLES GOING THE RIGHT WAY */
    *theta1 = atan2(coor[(*ipntr1 << 1) + 2] - *ycen, coor[(*ipntr1 << 1) + 1]
	     - *xcen);
    *theta2 = atan2(coor[(*ipntr2 << 1) + 2] - *ycen, coor[(*ipntr2 << 1) + 1]
	     - *xcen);
    if (*ipntr1 == *ipntr2) {
	*theta2 = *theta1 + twopi;
    } else if (*ip3 >= 0 && *theta2 <= *theta1) {
	*theta2 += twopi;
    } else if (*ip3 < 0 && *theta1 <= *theta2) {
	*theta1 += twopi;
    }
    *tang = *theta2 - *theta1;
    if (*ip3 < 0) {
	*iccw = *ipntr2;
	*icw = *ipntr1;
    } else {
	*iccw = *ipntr1;
	*icw = *ipntr2;
    }
L100:
    return 0;
} /* elpspr_ */

#ifdef __cplusplus
	}
#endif

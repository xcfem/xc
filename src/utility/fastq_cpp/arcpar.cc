/* arcpar.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int arcpar_(integer *mp, integer *kt, integer *knum, real *
	coor, integer *linkp, integer *ipntr1, integer *ipntr2, integer *
	ipntr3, integer *ip3, real *xcen, real *ycen, real *theta1, real *
	theta2, real *tang, real *r1, real *r2, logical *err, integer *iccw, 
	integer *icw, real *xk, real *xa)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 CENTER POINT FOR LINE\002,i5,\002 LIES \
ON ONE OF\002,\002 THE ENDPOINTS\002)";
    static char fmt_10010[] = "(\002 DEFINITION FOR ARC LINE\002,i5,\002 IS \
INVLAID\002)";

    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), cos(doublereal), sin(doublereal), 
	    sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    double log(doublereal), exp(doublereal);

    /* Local variables */
    static real r__, pi, x11, y11, x21, y21, dx, dy, det, arm, chord, divid, 
	    twopi, theta3;
    extern /* Subroutine */ int pltflu_();

    /* Fortran I/O blocks */
    static cilist io___13 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE ARCPAR = THIS ROUTINE CALCULATES THE ARC PARAMETERS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     TANG   = TOTAL ANGLE SCRIBED BY THE ARC */
/*     THETA1 = FIRST CCW ANGLE OF THE ARC */
/*     THETA2 = SECOND CCW ANGLE OF THE ARC */
/*     IPNTR1 = POINTER TO FIRST COORDINATE VALUE */
/*     IPNTR2 = POINTER TO SECOND COORDINATE VALUE */
/*     IPNTR3 = POINTER TO THIRD COORDINATE VALUE */
/*     IP3    = THE THIRD POINT NUMBER (CAN BE NEGATED) */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    twopi = pi + pi;
    *err = FALSE_;
/*  ARC WITH CENTER GIVEN */
/*  ARC GOES FROM 1ST POINT TO 2ND IN *COUNTER-CLOCKWISE* DIRECTION. */
    if (*kt == 3) {
	*xcen = coor[(*ipntr3 << 1) + 1];
	*ycen = coor[(*ipntr3 << 1) + 2];
/*  CIRCLE WITH THIRD POINT ON ARC */
    } else if (*kt == 4) {
	*theta1 = atan2(coor[(*ipntr3 << 1) + 2] - coor[(*ipntr1 << 1) + 2], 
		coor[(*ipntr3 << 1) + 1] - coor[(*ipntr1 << 1) + 1]) + pi / (
		float)2.;
	*theta2 = atan2(coor[(*ipntr3 << 1) + 2] - coor[(*ipntr2 << 1) + 2], 
		coor[(*ipntr3 << 1) + 1] - coor[(*ipntr2 << 1) + 1]) + pi / (
		float)2.;
	det = -cos(*theta1) * sin(*theta2) + cos(*theta2) * sin(*theta1);
	x11 = (coor[(*ipntr1 << 1) + 1] + coor[(*ipntr3 << 1) + 1]) * (float)
		.5;
	y11 = (coor[(*ipntr1 << 1) + 2] + coor[(*ipntr3 << 1) + 2]) * (float)
		.5;
	x21 = (coor[(*ipntr2 << 1) + 1] + coor[(*ipntr3 << 1) + 1]) * (float)
		.5;
	y21 = (coor[(*ipntr2 << 1) + 2] + coor[(*ipntr3 << 1) + 2]) * (float)
		.5;
	r__ = (-sin(*theta2) * (x21 - x11) + cos(*theta2) * (y21 - y11)) / 
		det;
	*xcen = x11 + r__ * cos(*theta1);
	*ycen = y11 + r__ * sin(*theta1);
/*     CIRCLE WITH RADIUS GIVEN */
    } else if (*kt == 6) {
	dx = (coor[(*ipntr2 << 1) + 1] - coor[(*ipntr1 << 1) + 1]) * (float)
		.5;
	dy = (coor[(*ipntr2 << 1) + 2] - coor[(*ipntr1 << 1) + 2]) * (float)
		.5;
	chord = sqrt(dx * dx + dy * dy);
	r__ = (r__1 = coor[(*ipntr3 << 1) + 1], dabs(r__1));
	if (r__ <= chord) {
	    *xcen = (coor[(*ipntr1 << 1) + 1] + coor[(*ipntr2 << 1) + 1]) * (
		    float).5;
	    *ycen = (coor[(*ipntr1 << 1) + 2] + coor[(*ipntr2 << 1) + 2]) * (
		    float).5;
	} else {
	    arm = sqrt(r__ * r__ - chord * chord);
	    if (*ip3 < 0) {
		*xcen = coor[(*ipntr1 << 1) + 1] + dx + arm * dy / chord;
		*ycen = coor[(*ipntr1 << 1) + 2] + dy - arm * dx / chord;
	    } else {
		*xcen = coor[(*ipntr1 << 1) + 1] + dx - arm * dy / chord;
		*ycen = coor[(*ipntr1 << 1) + 2] + dy + arm * dx / chord;
	    }
	}
    }
/*  CHECK TO MAKE SURE THAT THE BEGINNING AND ENDING RADIUS EXIST */
    if (coor[(*ipntr1 << 1) + 1] == *xcen && coor[(*ipntr1 << 1) + 2] == *
	    ycen || coor[(*ipntr2 << 1) + 1] == *xcen && coor[(*ipntr2 << 1) 
	    + 2] == *ycen) {
	pltflu_();
	s_wsfe(&io___13);
	i__1 = abs(*knum);
	do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	e_wsfe();
	*err = TRUE_;
	goto L100;
    }
/* Computing 2nd power */
    r__1 = coor[(*ipntr1 << 1) + 1] - *xcen;
/* Computing 2nd power */
    r__2 = coor[(*ipntr1 << 1) + 2] - *ycen;
    *r1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = coor[(*ipntr2 << 1) + 1] - *xcen;
/* Computing 2nd power */
    r__2 = coor[(*ipntr2 << 1) + 2] - *ycen;
    *r2 = sqrt(r__1 * r__1 + r__2 * r__2);
    *theta1 = atan2(coor[(*ipntr1 << 1) + 2] - *ycen, coor[(*ipntr1 << 1) + 1]
	     - *xcen);
    *theta2 = atan2(coor[(*ipntr2 << 1) + 2] - *ycen, coor[(*ipntr2 << 1) + 1]
	     - *xcen);
/*  ARC WITH THE CENTER GIVEN */
    if (*kt == 3) {
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
/*  CIRCULAR ARC WITH 3RD POINT ON ARC - CLOCKWISE OR COUNTER-CLOCKWISE */
    } else if (*kt == 4) {
	theta3 = atan2(coor[(*ipntr3 << 1) + 2] - *ycen, coor[(*ipntr3 << 1) 
		+ 1] - *xcen);
	if (*theta2 <= *theta1) {
	    *theta2 += twopi;
	}
	if (theta3 <= *theta1) {
	    theta3 += twopi;
	}
	*tang = *theta2 - *theta1;
	if (theta3 > *theta2) {
	    *tang = -(twopi - *tang);
	    *iccw = *ipntr2;
	    *icw = *ipntr1;
	} else {
	    *iccw = *ipntr1;
	    *icw = *ipntr2;
	}
/*     CIRRCULAR ARC WITH RADIUS GIVEN - CLOCKWISE OR COUNTER-CLOCKWISE */
    } else if (*kt == 6) {
	if (*ip3 >= 0 && *theta2 <= *theta1) {
	    *theta2 += twopi;
	}
	if (*ip3 < 0 && *theta1 <= *theta2) {
	    *theta1 += twopi;
	}
	*tang = *theta2 - *theta1;
	if (*ip3 >= 0) {
	    *iccw = *ipntr1;
	    *icw = *ipntr2;
	} else {
	    *iccw = *ipntr2;
	    *icw = *ipntr1;
	}
    }
/*  DEFINE THE SPIRAL PARAMETERS  (R = XA * EXP  (XK * THETA)) */
    *xk = log(*r2 / *r1) / (*theta2 - *theta1);
    divid = exp(*xk * *theta2);
    if (divid > (float)0.) {
	*xa = *r2 / divid;
    } else {
	s_wsfe(&io___16);
	i__1 = abs(*knum);
	do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	e_wsfe();
	*err = TRUE_;
	goto L100;
    }
L100:
    return 0;
} /* arcpar_ */

#ifdef __cplusplus
	}
#endif

/* eqlang.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int eqlang_(integer *mxnd, real *xn, real *yn, integer *lxn, 
	integer *node, integer *n0, integer *n2, integer *nfrom, real *dist, 
	real *vro, real *xdel, real *ydel)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal), sin(doublereal), 
	    asin(doublereal), cos(doublereal);

    /* Local variables */
    static real d0, d1, d2, d3, x0, y0, pi, ang, arg, ang1, ang2, ang3, ang12,
	     beta, ang3d, dist0, ang12d, twopi;
    static logical expand;

/* *********************************************************************** */
/*  SUBROUTINE EQLANG = CALCULATES A VECTOR SUM THAT ATTEMPTS TO */
/*                      MAINTAIN EQUAL ANGLES FOR A NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    --yn;
    --xn;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    twopi = pi * (float)2.;
    if (*nfrom > 0) {
/*  TEST FOR THE EXPANSION CASE */
	if ((lxn[(*nfrom << 2) + 4] != 0 && lxn[(*nfrom << 2) + 2] < 0 || lxn[
		(*nfrom << 2) + 4] < 0 && lxn[(*nfrom << 2) + 2] > 0) && (lxn[
		(*n0 << 2) + 3] == 0 || lxn[(*n2 << 2) + 3] == 0)) {
	    expand = TRUE_;
	} else {
	    expand = FALSE_;
	}
	ang1 = atan2(yn[*n2] - yn[*nfrom], xn[*n2] - xn[*nfrom]);
	if (ang1 < (float)0.) {
	    ang1 += twopi;
	}
	ang2 = atan2(yn[*n0] - yn[*nfrom], xn[*n0] - xn[*nfrom]);
	if (ang2 < (float)0.) {
	    ang2 += twopi;
	}
	ang3 = atan2(yn[*node] - yn[*nfrom], xn[*node] - xn[*nfrom]);
	if (ang3 < (float)0.) {
	    ang3 += twopi;
	}
/*  GET THE APPROPRIATE ANGLE BETWEEN ANGLE 1 AND 2 */
	ang12d = ang2 - ang1;
	if (ang12d < (float)0.) {
	    ang12d += twopi;
	}
/*  IF THIS IS AN EXPANSION, THEN ADJUST THE ANGLE ACCORDINGLY */
	if (expand) {
	    if (lxn[(*n2 << 2) + 3] == 0) {
		ang12 = ang1 + ang12d * (float).6;
	    } else if (lxn[(*n0 << 2) + 3] == 0) {
		ang12 = ang1 + ang12d * (float).4;
	    } else {
		ang12 = ang1 + ang12d * (float).5;
	    }
	} else {
	    ang12 = ang1 + ang12d * (float).5;
	}
	if (ang12 > twopi) {
	    ang12 -= twopi;
	}
/*  GET THE AVERAGE ANGLE BETWEEN ANGLE 12 AND 3 */
	if (ang12 > ang3) {
	    ang3d = ang12 - ang3;
	    if (ang3d > pi) {
		ang = ang12 + (twopi - ang3d) * (float).5;
	    } else {
		ang = ang12 - ang3d * (float).5;
	    }
	} else {
	    ang3d = ang3 - ang12;
	    if (ang3d > pi) {
		ang = ang3 + (twopi - ang3d) * (float).5;
	    } else {
		ang = ang3 - ang3d * (float).5;
	    }
	}
/*  GET THE DISTANCE TO MAKE THE OUTSIDE FLAT AT THIS ANGLE */
/* Computing 2nd power */
	r__1 = xn[*nfrom] - xn[*n0];
/* Computing 2nd power */
	r__2 = yn[*nfrom] - yn[*n0];
	d1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xn[*n2] - xn[*n0];
/* Computing 2nd power */
	r__2 = yn[*n2] - yn[*n0];
	d2 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xn[*nfrom] - xn[*n2];
/* Computing 2nd power */
	r__2 = yn[*nfrom] - yn[*n2];
	d3 = sqrt(r__1 * r__1 + r__2 * r__2);
	arg = sin(ang12d) * d1 / d2;
	if (arg > (float)1.) {
	    arg = (float)1.;
	}
	if (arg < (float)-1.) {
	    arg = (float)-1.;
	}
	beta = asin(arg);
	d0 = d3 * sin(beta) / sin(pi - beta - ang12d * (float).5);
	if (d0 > *dist) {
	    if (expand) {
		dist0 = d0;
	    } else {
		dist0 = (*dist + d0) * (float).5;
	    }
	} else {
	    dist0 = *dist;
	}
/*  CALCULATE THE NEW COORDINATES */
	x0 = xn[*nfrom] + cos(ang) * dist0;
	y0 = yn[*nfrom] + sin(ang) * dist0;
	*xdel = (x0 - xn[*node]) * *vro;
	*ydel = (y0 - yn[*node]) * *vro;
    } else {
	*xdel = (float)0.;
	*ydel = (float)0.;
    }
    return 0;
} /* eqlang_ */

#ifdef __cplusplus
	}
#endif

/* invmap.f -- translated by f2c (version 20160102).
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
#include "paving.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int invmap_(xc_float *x0, xc_float *y0, xc_float *x1, xc_float *y1, xc_float *x2,
	 xc_float *y2, xc_float *x3, xc_float *y3, xc_float *x4, xc_float *y4, xc_float *sxi, xc_float *
	seta, logical *inside)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    

    /* Local variables */
    static xc_float d1, d2;
    static doublereal ax, bx, cx, dx, ay, by, cy, dy, xi, xi1, xi2, rad, eta;
    static xc_float eps;
    static doublereal eta1, eta2;
    static xc_float eps2;
    static doublereal beta, gamma, alpha;

/* *********************************************************************** */
/*  THIS IS A TEST OF THE INVERTED MAPPING OF AN ELEMENT */
/* *********************************************************************** */
    eps = (xc_float).001;
    eps2 = (xc_float)1e-10;
/*  GET THE A, B, C, AND D VALUES FOR X AND Y. */
    ax = *x1 - *x0;
    bx = *x2 - *x1;
    cx = *x1 - *x2 + *x3 - *x4;
    dx = *x4 - *x1;
    ay = *y1 - *y0;
    by = *y2 - *y1;
    cy = *y1 - *y2 + *y3 - *y4;
    dy = *y4 - *y1;
/*  CALCULATE THE ALPHA, BETA, AND GAMMA VALUES. */
    alpha = cy * dx - cx * dy;
    beta = ax * cy - ay * cx + by * dx - bx * dy;
    gamma = ax * by - ay * bx;
/*  CALCULATE THE XI AND ETA VALUES. */
    if (alpha == (xc_float)0.) {
	eta = -gamma / beta;
	if (eta == 0. && bx == 0.) {
	    xi = (*y0 - *y1) / (*y2 - *y1);
	} else if (bx == -cx && eta == (xc_float)1.) {
	    xi = (*y0 - *y3) / (*y4 - *y3);
	} else if (bx == -cx * eta) {
	    xi = (xc_float)-1e3;
	} else {
	    xi = (-ax - dx * eta) / (bx + cx * eta);
	}
    } else {
/* Computing 2nd power */
	d__1 = beta;
	rad = d__1 * d__1 - alpha * (xc_float)4. * gamma;
	if (rad < (xc_float)0.) {
/* **               NEGATIVE RADICAL PROBLEM AS IT APPEARS THAT */
/* **               THIS MAY OCCUR - IT JUST MEANS THAT THE POINT */
/* **               TRULY IS NOT IN THE ELEMENT. */
/*            CALL MESAGE ('** ERROR - NEGATIVE RADICAL IN INVMAP **') */
	    *inside = FALSE_;
	    goto L100;
	}
	rad = sqrt(rad);
	eta1 = (-beta + rad) / (alpha * (xc_float)2.);
	eta2 = (-beta - rad) / (alpha * (xc_float)2.);
	if (abs(eta1) < eps2 && abs(bx) < eps2) {
	    xi1 = (*y0 - *y1) / (*y2 - *y1);
	} else if (bx == -cx && eta1 == (xc_float)1.) {
	    xi1 = (*y0 - *y3) / (*y4 - *y3);
	} else if (bx == -cx * eta1) {
	    xi1 = (xc_float)-1e3;
	} else {
	    xi1 = (-ax - dx * eta1) / (bx + cx * eta1);
	}
	if (abs(eta2) < eps2 && abs(bx) < eps2) {
	    xi2 = (*y0 - *y1) / (*y2 - *y1);
	} else if (bx == -cx && eta2 == (xc_float)1.) {
	    xi2 = (*y0 - *y3) / (*y4 - *y3);
	} else if (bx == -cx * eta2) {
	    xi2 = (xc_float)-1e3;
	} else {
	    xi2 = (-ax - dx * eta2) / (bx + cx * eta2);
	}
	d1 = sqrt(eta1 * eta1 + xi1 * xi1);
	d2 = sqrt(eta2 * eta2 + xi2 * xi2);
	if (d1 < d2) {
	    eta = eta1;
	    xi = xi1;
	} else {
	    eta = eta2;
	    xi = xi2;
	}
    }
/*  CHECK TO SEE IF ETA AND XI ARE WITHIN THE ELEMENT */
    if (! (eta <= eps + (xc_float)1. && eta >= (xc_float)0. - eps)) {
	*inside = FALSE_;
	goto L100;
    } else if (! (xi <= eps + (xc_float)1. && xi >= (xc_float)0. - eps)) {
	*inside = FALSE_;
	goto L100;
    } else {
	*inside = TRUE_;
    }
    *sxi = xi;
    *seta = eta;
L100:
    return 0;
} /* invmap_ */

#ifdef __cplusplus
	}
#endif

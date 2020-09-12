/* gnxka.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int gnxka_(integer *mxnd, xc_float *xn, xc_float *yn, integer *k, 
	integer *nodes, xc_float *area, integer *lxk, integer *nxl, logical *ccw)
{
    static integer i__, l, m1, m2, ntmp;

/* *********************************************************************** */
/*  SUBROUTINE GNXKA = GENERATES A LIST OF THE FOUR NODES ASSOCIATED WITH */
/*                     ELEMENT K */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     CCW    = .TRUE. IF LIST IS TO BE IN CCW ORDER AND AREA DEFINED */
/*    (Changed to always put in order and calculate area) */
/* *********************************************************************** */
    /* Parameter adjustments */
    nxl -= 3;
    lxk -= 5;
    --yn;
    --xn;
    --nodes;

    /* Function Body */
    *area = (xc_float)0.;
    for (i__ = 1; i__ <= 4; ++i__) {
	nodes[i__] = 0;
/* L10: */
    }
/* ... Let line 1 be the base line */
    l = lxk[(*k << 2) + 1];
/* ... Null List */
    if (l <= 0) {
	return 0;
    }
    nodes[1] = nxl[(l << 1) + 1];
    nodes[2] = nxl[(l << 1) + 2];
/* ... Find other ends of the two sides */
    for (i__ = 2; i__ <= 4; ++i__) {
	l = lxk[i__ + (*k << 2)];
	m1 = nxl[(l << 1) + 1];
	m2 = nxl[(l << 1) + 2];
	if (m1 == nodes[1]) {
	    nodes[4] = m2;
	} else if (m2 == nodes[1]) {
	    nodes[4] = m1;
	}
	if (m1 == nodes[2]) {
	    nodes[3] = m2;
	} else if (m2 == nodes[2]) {
	    nodes[3] = m1;
	}
/* L110: */
    }
/* ... Compute signed area */
    *area = ((xn[nodes[3]] - xn[nodes[1]]) * (yn[nodes[4]] - yn[nodes[2]]) - (
	    yn[nodes[3]] - yn[nodes[1]]) * (xn[nodes[4]] - xn[nodes[2]])) * (
	    float).5;
    if (*area < (xc_float)0.) {
/* ... Clockwise case  -  reverse the order */
	ntmp = nodes[2];
	nodes[2] = nodes[4];
	nodes[4] = ntmp;
	*area = -(*area);
    }
    return 0;
} /* gnxka_ */

#ifdef __cplusplus
	}
#endif

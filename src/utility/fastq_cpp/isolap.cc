/* isolap.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int isolap_(integer *mxnd, real *xn, real *yn, integer *lxk, 
	integer *kxl, integer *nxl, integer *lxn, integer *nnn, integer *
	nnnold, real *wfac, integer *nit, real *eps, real *ro)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1;

    /* Local variables */
    static integer j1, j2, j3, kk, kl, in, it, ks;
    static logical big, ccw, err;
    static real eps2, area;
    static integer node;
    static real xdel, ydel;
    extern /* Subroutine */ int gkxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *);
    static real sumx, sumy;
    static integer nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);
    static integer klist[20];

/* *********************************************************************** */
/*  SUBROUTINE ISOLAP = MESH SMOOTHING BY LAPLACE-S USING GAUSS-SEIDEL */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     WFAC = WEIGHT (0. = LAPLACIAN, 1. = ISOPARAMETRIC) */
/*     NIT  = THE MAX NUMBER OF ITERATIONS TO DO. */
/*     EPS  = MINIMUM DISTANCE NODES MUST MOVE TO CONTINUE ITERATIONS */
/*     RO   = AN UNDER- OR OVER-RELAXATION FACTOR (NORMALLY 1.0) */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --yn;
    --xn;

    /* Function Body */
    if (*ro < (float).01) {
	*ro = (float)1.;
    }
/* Computing 2nd power */
    r__1 = *eps * *ro;
    eps2 = r__1 * r__1;
/*  ITERATION LOOP */
    i__1 = *nit;
    for (it = 1; it <= i__1; ++it) {
	big = FALSE_;
/*  NODE LOOP */
	i__2 = *nnn;
	for (node = *nnnold + 1; node <= i__2; ++node) {
/*  SKIP CONTINUATION AND BOUNDARY LINES */
	    if (lxn[(node << 2) + 1] > 0 && lxn[(node << 2) + 2] > 0) {
/*  FIND ELEMENTS ATTACHED TO NODE */
		gkxn_(mxnd, &kxl[3], &lxn[5], &node, &ks, klist, &err);
		sumx = (float)0.;
		sumy = (float)0.;
		i__3 = ks;
		for (kl = 1; kl <= i__3; ++kl) {
		    ccw = FALSE_;
		    kk = klist[kl - 1];
		    gnxka_(mxnd, &xn[1], &yn[1], &kk, nodes, &area, &lxk[5], &
			    nxl[3], &ccw);
		    for (in = 1; in <= 4; ++in) {
			if (nodes[in - 1] == node) {
			    j1 = in + 1;
			    if (j1 > 4) {
				j1 = 1;
			    }
			    goto L110;
			}
/* L100: */
		    }
L110:
		    j2 = j1 + 1;
		    if (j2 > 4) {
			j2 = 1;
		    }
		    j3 = j2 + 1;
		    if (j3 > 4) {
			j3 = 1;
		    }
		    sumx = sumx + xn[nodes[j1 - 1]] + xn[nodes[j3 - 1]] - *
			    wfac * xn[nodes[j2 - 1]];
		    sumy = sumy + yn[nodes[j1 - 1]] + yn[nodes[j3 - 1]] - *
			    wfac * yn[nodes[j2 - 1]];
/* L120: */
		}
/*  REDEFINE THIS NODE-S COORDINATES */
		sumx /= (doublereal) ks * ((float)2. - *wfac);
		sumy /= (doublereal) ks * ((float)2. - *wfac);
		xdel = *ro * (sumx - xn[node]);
		ydel = *ro * (sumy - yn[node]);
		xn[node] += xdel;
		yn[node] += ydel;
/*  CHECK FOR CONVERGENCE */
		if (xdel * xdel + ydel * ydel > eps2) {
		    big = TRUE_;
		}
	    }
/* L130: */
	}
/*  IF NO SIGNIFICANT MOVEMENTS OCCURRED,  RETURN */
	if (! big) {
	    return 0;
	}
/* L140: */
    }
    return 0;
} /* isolap_ */

#ifdef __cplusplus
	}
#endif

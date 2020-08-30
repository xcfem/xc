/* ciapal.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int ciapal_(integer *mxnd, real *xn, real *yn, integer *lxk, 
	integer *kxl, integer *nxl, integer *lxn, integer *nnn, integer *
	nnnold, integer *nit, real *eps, real *ro, real *alpha)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, ik, kk, nn, it;
    static real fx, fy, dx, dy, al2;
    static logical big, ccw;
    static real arl;
    static logical err;
    static real sdx, sdy, eps2, area[20], beta;
    static integer node;
    static real delx, xcen[20], ycen[20], dely;
    extern /* Subroutine */ int gkxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *);
    static integer numk;
    static real sumw, xsum, ysum;
    static integer nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);
    static integer klist[20];
    static real rnumk, rsumw, weight;

/* *********************************************************************** */
/*  SUBROUTINE CIAPAL = CENTROID INVERSE AREA PUSH AND LAPLACIAN SMOOTH */
/* *********************************************************************** */
/*  NOTE: */
/*     IN THIS METHOD EACH CENTROID OF AN ELEMENT PUSHES OUT */
/*     ON THE SURROUNDING NODES WITH A FORCE INVERSELY PROPORTIONAL */
/*     TO THE AREA OF THE ELEMENT WHILE IT SIMULTANEOUSLY PULLS ON */
/*     EACH NODE WITH A FORCE PROPORTIONAL TO THE LENGTH OF THE LINE */
/*     CONNECTING THE CENTROID WITH EACH NODE. */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NIT   = MAX NUMBER OF ITERATIONS TO DO */
/*     EPS   = NODE MOVEMENT TOLERANCE FOR CONVERGENCE */
/*     RO    = UNDER OR OVER-RELAXATION FACTOR. */
/*     ALPHA = WEIGHT GIVEN TO AREA-PUSH VECTOR.  USUALLY=0.5. */
/*             WEIGHT GIVEN TO LAPLACIAN VECTOR = 1.-ALPHA. */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --yn;
    --xn;

    /* Function Body */
/* Computing 2nd power */
    r__1 = *eps * *ro;
    eps2 = r__1 * r__1;
    beta = (float)1. - *alpha;
/*  ITERATION LOOP */
    i__1 = *nit;
    for (it = 1; it <= i__1; ++it) {
	big = FALSE_;
/*  NODE LOOP */
	i__2 = *nnn;
	for (node = *nnnold + 1; node <= i__2; ++node) {
/*  SKIP CONTINUATION LINES AND BOUNDARY LINES */
	    if (lxn[(node << 2) + 1] > 0 && lxn[(node << 2) + 2] > 0) {
/*  GET ELEMENT LIST  (IGNORE ERR IF IT IS BECAUSE TOO MANY WERE FOUND) */
		gkxn_(mxnd, &kxl[3], &lxn[5], &node, &numk, klist, &err);
		if (err && numk < 20) {
		    return 0;
		}
/*  GET AREAS AND CENTROIDS */
		i__3 = numk;
		for (ik = 1; ik <= i__3; ++ik) {
		    kk = klist[ik - 1];
		    ccw = TRUE_;
		    gnxka_(mxnd, &xn[1], &yn[1], &kk, nodes, &area[ik - 1], &
			    lxk[5], &nxl[3], &ccw);
		    xsum = (float)0.;
		    ysum = (float)0.;
		    for (i__ = 1; i__ <= 4; ++i__) {
			nn = nodes[i__ - 1];
			xsum += xn[nn];
			ysum += yn[nn];
/* L100: */
		    }
		    xcen[ik - 1] = xsum * (float).25;
		    ycen[ik - 1] = ysum * (float).25;
/* L110: */
		}
/*  COMPUTE AND SUM THE FORCE VECTORS */
		fx = (float)0.;
		fy = (float)0.;
		sumw = (float)0.;
		sdx = (float)0.;
		sdy = (float)0.;
		i__3 = numk;
		for (ik = 1; ik <= i__3; ++ik) {
		    dx = xcen[ik - 1] - xn[node];
		    dy = ycen[ik - 1] - yn[node];
		    al2 = dx * dx + dy * dy;
		    arl = area[ik - 1] * sqrt(al2);
		    weight = (float)1e20;
		    if (arl > (float)0.) {
			weight = (float)1. / arl;
		    }
		    sumw += weight;
		    fx -= weight * dx;
		    fy -= weight * dy;
		    sdx += dx;
		    sdy += dy;
/* L120: */
		}
/*  MOVE THE NODE */
		rsumw = (float)1. / sumw;
		rnumk = (float)1. / (doublereal) numk;
		fx = *alpha * fx * rsumw + beta * sdx * rnumk;
		fy = *alpha * fy * rsumw + beta * sdy * rnumk;
		delx = *ro * fx;
		dely = *ro * fy;
		xn[node] += delx;
		yn[node] += dely;
/* Computing 2nd power */
		r__1 = delx;
/* Computing 2nd power */
		r__2 = dely;
		if (r__1 * r__1 + r__2 * r__2 > eps2) {
		    big = TRUE_;
		}
	    }
/* L130: */
	}
	if (! big) {
	    return 0;
	}
/* L140: */
    }
    return 0;
} /* ciapal_ */

#ifdef __cplusplus
	}
#endif

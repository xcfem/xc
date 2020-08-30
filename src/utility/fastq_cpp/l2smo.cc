/* l2smo.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int l2smo_(integer *mxnd, real *xn, real *yn, integer *nxl, 
	integer *lxn, integer *nnn, integer *nnnold, integer *nit, real *eps, 
	real *ro)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer l;
    static real al;
    static integer il;
    static real dx;
    static integer it;
    static real fx, fy, sl, dy, al2;
    static logical big, err;
    static real eps2;
    static integer nend, node;
    static real delx, dely;
    static integer lines[20], kount;
    extern /* Subroutine */ int getlxn_(integer *, integer *, integer *, 
	    integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE L2SMO = LAPLACIAN SQUARED METHOD OF MESH SMOOTHING */
/* *********************************************************************** */
/*  NOTE: */
/*     THIS METHOD IS LIKE LAPLACIAN SMOOTHING EXCEPT EACH VECTOR */
/*     POINTING TO A NEIGHBOR NODE HAS A LENGTH OF */
/*      (DISTANCE TO THAT NODE)**2. */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NIT = MAX NUMBER OF ITERATIONS TO DO */
/*     EPS = NODE MOVEMENT TOLERANCE FOR CONVERGENCE */
/*     RO  = UNDER OR OVER-RELAXATION FACTOR. */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    --yn;
    --xn;

    /* Function Body */
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
	    if (lxn[(node << 2) + 1] > 0 && lxn[(node << 2) + 2] > 0) {
		fx = (float)0.;
		fy = (float)0.;
		sl = (float)0.;
/*  LOOP THROUGH ALL LINES CONNECTED TO NODE */
		getlxn_(mxnd, &lxn[5], &node, lines, &kount, &err);
		if (err) {
		    return 0;
		}
		i__3 = kount;
		for (il = 1; il <= i__3; ++il) {
		    l = lines[il - 1];
		    nend = nxl[(l << 1) + 1] + nxl[(l << 1) + 2] - node;
		    dx = xn[nend] - xn[node];
		    dy = yn[nend] - yn[node];
		    al2 = dx * dx + dy * dy;
		    al = sqrt(al2);
		    fx += dx * al;
		    fy += dy * al;
		    sl += al;
/* L100: */
		}
/*  MOVE THE NODE */
		delx = *ro * fx / sl;
		dely = *ro * fy / sl;
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
/* L110: */
	}
	if (! big) {
	    return 0;
	}
/* L120: */
    }
    return 0;
} /* l2smo_ */

#ifdef __cplusplus
	}
#endif

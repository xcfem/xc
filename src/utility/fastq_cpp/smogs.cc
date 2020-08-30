/* smogs.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int smogs_(integer *mxnd, real *xn, real *yn, integer *nxl, 
	integer *lxn, integer *nnn, integer *nnnold, integer *nit, real *eps, 
	real *ro)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1;

    /* Local variables */
    static integer l, il, im, it;
    static logical big, err;
    static real eps2;
    static integer node;
    static real xdel, ydel, sumx, sumy;
    static integer lines[20], kount;
    extern /* Subroutine */ int getlxn_(integer *, integer *, integer *, 
	    integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE SMOGS  =  MESH SMOOTHING BY LAPLACE-S USING GAUSS-SEIDEL */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NIT   =  THE MAX NUMBER OF ITERATIONS TO DO. */
/*     EPS   =  MINIMUM DISTANCE NODES MUST MOVE TO CONTINUE ITERATIONS */
/*     RO    =  AN UNDER- OR OVER-RELAXATION FACTOR (NORMALLY 1.0) */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
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
/*  SUM COORDINATES OF ALL NEIGHBORING NODES */
		sumx = (float)0.;
		sumy = (float)0.;
		getlxn_(mxnd, &lxn[5], &node, lines, &kount, &err);
/*  IGNORE ERR BECAUSE IT IS ALREADY TAKEN CARE OF IN THE SKIP */
		i__3 = kount;
		for (il = 1; il <= i__3; ++il) {
		    l = lines[il - 1];
		    im = nxl[(l << 1) + 1] + nxl[(l << 1) + 2] - node;
		    sumx += xn[im];
		    sumy += yn[im];
/* L100: */
		}
/*  REDEFINE THIS NODE - S COORDINATES */
		sumx /= (doublereal) kount;
		sumy /= (doublereal) kount;
		xdel = *ro * (sumx - xn[node]);
		ydel = *ro * (sumy - yn[node]);
		xn[node] += xdel;
		yn[node] += ydel;
/*  CHECK FOR CONVERGENCE */
		if (xdel * xdel + ydel * ydel > eps2) {
		    big = TRUE_;
		}
	    }
/* L110: */
	}
/*  IF NO SIGNIFICANT MOVEMENTS OCCURRED,  RETURN */
	if (! big) {
	    return 0;
	}
/* L120: */
    }
    return 0;
} /* smogs_ */

#ifdef __cplusplus
	}
#endif

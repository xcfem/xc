/* apalsm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int apalsm_(integer *mxnd, real *xn, real *yn, integer *lxk, 
	integer *kxl, integer *nxl, integer *lxn, integer *nnn, integer *
	nnnold, integer *nit, real *tol, real *ro, real *alpha, logical *err)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1;

    /* Local variables */
    static integer i__, l, n1, n2, ik, il;
    static real xa, ya, dx;
    static integer it;
    static real xf, yf, dy, rk, xl, yl;
    static integer ns1[4], ns2[4];
    static logical big;
    static integer noe;
    static real dot;
    static integer nnt;
    static real tol2, alib[8], beta;
    static integer klib[8], nlib[32]	/* was [4][8] */, node, knum;
    static real xnet, ynet, area1, area2, dmin2, xcen1, ycen1, xcen2, ycen2, 
	    dist2;
    static integer idiag, ndiag;
    static real xclib[8], yclib[8];
    static integer lines[20], kount;
    extern /* Subroutine */ int apalib_(integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, real *, real *, real *, integer *
	    , integer *, integer *, real *, real *, real *), mesage_(char *, 
	    ftnlen);
    static real weight;
    extern /* Subroutine */ int getlxn_(integer *, integer *, integer *, 
	    integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE APALSM = AREA PULL AND LAPLACIAN MESH SMOOTHER */
/* *********************************************************************** */
/*  NOTE: */
/*     IN THIS SMOOTHER EACH NODE IS SUCCESSIVELY MOVED BY */
/*     AN AMOUNT GIVEN AS A WEIGHTED AVERAGE OF AN *AREA PULL* */
/*     VECTOR AND THE LAPLACIAN VECTOR (AVERAGE OF VECTORS POINTING */
/*     TO NEIGHBORS).  THE *AREA PULL* VECTOR IS OBTAINED BY LETTING */
/*     EACH ELEMENT PULL IN PERPENDICULARLY ON ITS SIDES WITH FORCE */
/*     PROPORTIONAL TO THE LENGTH OF THAT SIDE TIMES A FACTOR */
/*     INVOLVING THE AREAS OF THIS ELEMENT AND ITS NEIGHBOR SHARING */
/*     THAT SIDE. */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NIT   = MAX ITERATIONS TO DO */
/*     TOL   = PERCENT OF SMALLEST CONNECTING LINE TO USE AS NODE MOVEMENT */
/*             CONVERGENCE TOLERANCE. */
/*     RO    = UNDER OR OVER-RELAXATION FACTOR. */
/*     ALPHA = WEIGHT GIVEN TO AREA PULL VECTOR.  USUALLY = 0.5. */
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
    *err = FALSE_;
/* Computing 2nd power */
    r__1 = *tol;
    tol2 = r__1 * r__1;
    beta = (float)1. - *alpha;
/*  ITERATION LOOP */
    i__1 = *nit;
    for (it = 1; it <= i__1; ++it) {
	big = FALSE_;
/*  NODE LOOP */
	nnt = 0;
	i__2 = *nnn;
	for (node = *nnnold + 1; node <= i__2; ++node) {
/*  CHECK FOR CONTINUATIONS,  BOUNDARY,  OR RELAXED NODE */
	    if (lxn[(node << 2) + 3] >= 0 && lxn[(node << 2) + 2] > 0 && lxn[(
		    node << 2) + 1] > 0) {
		++nnt;
/*  INITIALIZE */
		knum = 0;
		xa = (float)0.;
		ya = (float)0.;
		xl = (float)0.;
		yl = (float)0.;
/*  PROCESS EACH LINE CONNECTED TO THIS NODE */
		getlxn_(mxnd, &lxn[5], &node, lines, &kount, err);
		if (*err) {
		    return 0;
		}
		i__3 = kount;
		for (il = 1; il <= i__3; ++il) {
		    l = lines[il - 1];
		    n1 = nxl[(l << 1) + 1];
		    n2 = nxl[(l << 1) + 2];
/*  FETCH ELEMENT DATA */
		    if (kxl[(l << 1) + 1] > 0) {
			apalib_(mxnd, &xn[1], &yn[1], &lxk[5], &nxl[3], &kxl[(
				l << 1) + 1], ns1, &area1, &xcen1, &ycen1, &
				knum, klib, nlib, alib, xclib, yclib);
		    }
		    if (kxl[(l << 1) + 2] > 0) {
			apalib_(mxnd, &xn[1], &yn[1], &lxk[5], &nxl[3], &kxl[(
				l << 1) + 2], ns2, &area2, &xcen2, &ycen2, &
				knum, klib, nlib, alib, xclib, yclib);
		    }
/*  GET FORCE DIRECTION MODULO PI RADIANS. */
/*  CORRECT FOR WRONG DIRECTION BY ALIGNING WITH THE VECTOR */
/*  FROM (XCEN1, YCEN1) TO (XCEN2, YCEN2). */
		    xf = -(yn[n2] - yn[n1]);
		    yf = xn[n2] - xn[n1];
		    dot = xf * (xcen2 - xcen1) + yf * (ycen2 - ycen1);
		    if (dot < (float)0.) {
			xf = -xf;
			yf = -yf;
		    }
/*  TAKE CARE OF ZERO AREAS */
		    if (area1 <= (float)0. || area2 <= (float)0.) {
			area1 = (float)1.;
			area2 = (float)1.;
		    }
/*  UPDATE AREA PULL VECTOR SUM */
		    weight = (area2 - area1) / (area2 + area1);
		    xa += weight * xf;
		    ya += weight * yf;
/*  UPDATE LAPLACIAN VECTOR SUM */
		    noe = n1 + n2 - node;
		    dx = xn[noe] - xn[node];
		    dy = yn[noe] - yn[node];
		    xl += dx;
		    yl += dy;
/*  UPDATE LEAST LENGTH */
		    dist2 = dx * dx + dy * dy;
		    if (il == 1) {
			dmin2 = dist2;
		    }
		    dmin2 = dmin(dmin2,dist2);
/* L100: */
		}
/*  COMPUTE NET MOVEMENT VECTOR */
		rk = (float)1. / (doublereal) kount;
		xnet = (*alpha * xa + beta * xl) * rk;
		ynet = (*alpha * ya + beta * yl) * rk;
/*  MOVE THE NODE */
		yn[node] += ynet * *ro;
		xn[node] += xnet * *ro;
/*  CHECK FOR SIGNIFICANT MOVEMENT */
		if (xnet * xnet + ynet * ynet > tol2 * dmin2) {
/*  SIGNIFICANT MOVEMENT - REMOVE RELAXATION FLAGS */
/*  FIRST FROM DIRECTLY CONNECTED NODES */
		    i__3 = kount;
		    for (il = 1; il <= i__3; ++il) {
			l = lines[il - 1];
			noe = nxl[(l << 1) + 1] + nxl[(l << 1) + 2] - node;
			lxn[(noe << 2) + 3] = (i__4 = lxn[(noe << 2) + 3], 
				abs(i__4));
/* L110: */
		    }
/*  NEXT FROM DIAGONALLY OPPOSITE NODES (MAX 8) */
		    i__3 = knum;
		    for (ik = 1; ik <= i__3; ++ik) {
			for (i__ = 1; i__ <= 4; ++i__) {
			    if (nlib[i__ + (ik << 2) - 5] == node) {
				idiag = i__ + 2;
				if (idiag >= 5) {
				    idiag += -4;
				}
				ndiag = nlib[idiag + (ik << 2) - 5];
				lxn[(ndiag << 2) + 3] = (i__4 = lxn[(ndiag << 
					2) + 3], abs(i__4));
				goto L130;
			    }
/* L120: */
			}
			mesage_("ERROR IN APALSM", (ftnlen)15);
			*err = TRUE_;
			return 0;
L130:
/* L140: */
			;
		    }
/*  INSIGNIFICANT MOVEMENT */
/*  INSERT RELAXATION FLAG */
		} else {
		    lxn[(node << 2) + 3] = -(i__3 = lxn[(node << 2) + 3], abs(
			    i__3));
		}
	    }
/* L150: */
	}
	if (! big) {
	    goto L170;
	}
/* L160: */
    }
    it = *nit;
/*  REMOVE ALL FLAGS */
L170:
    i__1 = *nnn;
    for (node = *nnnold + 1; node <= i__1; ++node) {
	lxn[(node << 2) + 3] = (i__2 = lxn[(node << 2) + 3], abs(i__2));
/* L180: */
    }
    return 0;
} /* apalsm_ */

#ifdef __cplusplus
	}
#endif

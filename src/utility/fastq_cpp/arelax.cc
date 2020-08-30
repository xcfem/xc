/* arelax.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int arelax_(integer *mxnd, real *xn, real *yn, integer *lxk, 
	integer *kxl, integer *nxl, integer *lll, real *arfact)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Local variables */
    static integer n1, n2;
    static logical ccw;
    static integer num, myl;
    static real dxdy, area1, area2;
    static integer nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);
    static real ratio, aspect, ratsum;

/* *********************************************************************** */
/*  SUBROUTINE ARELAX = CALCULATES UNDER - RELAXATION FACTOR FOR AREA PULL */
/*                      AND LAPLACIAN SMOOTHER */
/* *********************************************************************** */
/*  NOTE: */
/*     THE AREA PULL AND LAPLACIAN SMOOTHER WILL OVER - CORRECT */
/*     AND BECOME UNSTABLE WHEN TYPICAL MESH ELEMENTS ARE MUCH */
/*     LONGER THAN THEY ARE WIDE, SAY BY A FACTOR OF SIX OR MORE. */
/*     THIS ROUTINE COMPUTES AN APPROPRIATE UNDER - RELAXATION */
/*     FACTOR TO BE USED TO HELP CORRECT THIS PROBLEM.  ON REGIONS */
/*     WHICH HAVE GENERALLY NEAR SQUARE ELEMENTS WITH A SMALL */
/*     PERCENTAGE OF VERY LONG THIN ELEMENTS THIS FACTOR WILL */
/*     PROBABLY NOT ADEQUATELY HANDLE THE DIFFICULTY.  IN SUCH */
/*     SITUATIONS AN ALTERNATE SMOOTHER  (SUCH AS THE CENTROID - AREA - */
/*     PULL) SHOULD BE USED. */
/*     THE FACTOR RETURNED BY THIS ROUTINE MAY BE LARGER THAN ONE, */
/*     WHICH MEANS THAT OVER - RELAXATION IS APPROPRIATE. */
/* *********************************************************************** */
    /* Parameter adjustments */
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --yn;
    --xn;

    /* Function Body */
    *arfact = (float)1.;
    ratsum = (float)0.;
    num = 0;
    i__1 = *lll;
    for (myl = 1; myl <= i__1; ++myl) {
/*  SKIP BOUNDARY LINES */
	if (kxl[(myl << 1) + 1] > 0 && kxl[(myl << 1) + 2] > 0) {
	    ccw = TRUE_;
	    gnxka_(mxnd, &xn[1], &yn[1], &kxl[(myl << 1) + 1], nodes, &area1, 
		    &lxk[5], &nxl[3], &ccw);
	    gnxka_(mxnd, &xn[1], &yn[1], &kxl[(myl << 1) + 2], nodes, &area2, 
		    &lxk[5], &nxl[3], &ccw);
	    n1 = nxl[(myl << 1) + 1];
	    n2 = nxl[(myl << 1) + 2];
/* Computing 2nd power */
	    r__1 = xn[n2] - xn[n1];
/* Computing 2nd power */
	    r__2 = yn[n2] - yn[n1];
	    dxdy = r__1 * r__1 + r__2 * r__2;
	    if (area1 + area2 > (float)0.) {
		ratio = dxdy * (float)2. / (area1 + area2);
		if (ratio >= (float).99) {
		    ++num;
		    ratsum += ratio;
		}
	    }
	}
/* L100: */
    }
    if (num <= 0) {
	return 0;
    }
    aspect = ratsum / (doublereal) num;
/* Computing MIN */
    r__1 = (float)2. / aspect;
    *arfact = dmin(r__1,(float)1.5);
    return 0;
} /* arelax_ */

#ifdef __cplusplus
	}
#endif

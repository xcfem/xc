/* snappt.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int snappt_(integer *msnap, real *snapdx, integer *nsnap, 
	real *x, real *y)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__;
    static real xp, yp, xp1, xp2, yp1, yp2;
    static logical xmid, ymid;
    static real dist1, dist2, dist3, dist4;
    extern /* Subroutine */ int pltbel_();

/* *********************************************************************** */
/*  SUBROUTINE SNAPPT = GETS THE X, Y TO THE CLOSEST GRID POINT */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     SNAPDX = SNAP GRID LINE ARRAY */
/*     NSNAP  = ARRAY OF GRID LINE COUNTERS */
/*     MSNAP  = THE DIMENSION OF THE GRID LINE ARRAY */
/*     XMID   = .TRUE. IF THE X VALUE FALLS BETWEEN TWO X GRID LINES */
/*     YMID   = .TRUE. IF THE Y VALUE FALLS BETWEEN TWO Y GRID LINES */
/* *********************************************************************** */
    /* Parameter adjustments */
    snapdx -= 3;
    --nsnap;

    /* Function Body */
    xmid = FALSE_;
    ymid = FALSE_;
/*  GET THE BOUNDING X SNAP LINES */
    if (*x <= snapdx[3]) {
	xp = snapdx[3];
    } else if (*x >= snapdx[(nsnap[1] << 1) + 1]) {
	xp = snapdx[(nsnap[1] << 1) + 1];
    } else {
	xmid = TRUE_;
	i__1 = nsnap[1];
	for (i__ = 2; i__ <= i__1; ++i__) {
	    if (*x <= snapdx[(i__ << 1) + 1]) {
		xp1 = snapdx[(i__ - 1 << 1) + 1];
		xp2 = snapdx[(i__ << 1) + 1];
		goto L110;
	    }
/* L100: */
	}
L110:
	;
    }
/*  GET THE BOUNDING Y SNAP LINES */
    if (*y <= snapdx[4]) {
	yp = snapdx[4];
    } else if (*y >= snapdx[(nsnap[2] << 1) + 2]) {
	yp = snapdx[(nsnap[2] << 1) + 2];
    } else {
	ymid = TRUE_;
	i__1 = nsnap[2];
	for (i__ = 2; i__ <= i__1; ++i__) {
	    if (*y <= snapdx[(i__ << 1) + 2]) {
		yp1 = snapdx[(i__ - 1 << 1) + 2];
		yp2 = snapdx[(i__ << 1) + 2];
		goto L130;
	    }
/* L120: */
	}
L130:
	;
    }
/*  NOW GET THE APPROPRIATE COMBINATION OF XLOW, XHIGH, XMID, YLOW,  ETC. */
/*  FIRST THE MOST COMMON CASE OF FITTING BETWEEN X AND Y GRIDS */
    if (ymid && xmid) {
/*  GET THE SHORTEST DISTANCE TO THIS COMBINATION */
/* Computing 2nd power */
	r__1 = xp1 - *x;
/* Computing 2nd power */
	r__2 = yp1 - *y;
	dist1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xp1 - *x;
/* Computing 2nd power */
	r__2 = yp2 - *y;
	dist2 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xp2 - *x;
/* Computing 2nd power */
	r__2 = yp1 - *y;
	dist3 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xp2 - *x;
/* Computing 2nd power */
	r__2 = yp2 - *y;
	dist4 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing MIN */
	r__1 = min(dist2,dist3);
	if (dist1 <= dmin(r__1,dist4)) {
	    *x = xp1;
	    *y = yp1;
	} else /* if(complicated condition) */ {
/* Computing MIN */
	    r__1 = min(dist1,dist3);
	    if (dist2 <= dmin(r__1,dist4)) {
		*x = xp1;
		*y = yp2;
	    } else /* if(complicated condition) */ {
/* Computing MIN */
		r__1 = min(dist1,dist2);
		if (dist3 <= dmin(r__1,dist4)) {
		    *x = xp2;
		    *y = yp1;
		} else {
		    *x = xp2;
		    *y = yp2;
		}
	    }
	}
/*  NOW THE CORNER CASES OF XLOW,  XHIGH,  YLOW,  AND YHIGH COMBINATIONS */
    } else if (! xmid && ! ymid) {
	*x = xp;
	*y = yp;
/*  NOW THE EDGE CASES OF XLOW OR XHIGH AND YMID */
    } else if (! xmid) {
	*x = xp;
/* Computing 2nd power */
	r__1 = xp - *x;
/* Computing 2nd power */
	r__2 = yp1 - *y;
	dist1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xp - *x;
/* Computing 2nd power */
	r__2 = yp2 - *y;
	dist2 = sqrt(r__1 * r__1 + r__2 * r__2);
	if (dist1 < dist2) {
	    *y = yp1;
	} else {
	    *y = yp2;
	}
/*  NOW THE EDGE CASES OF XMID AND YHIGH OR YLOW */
    } else if (! ymid) {
	*y = yp;
/* Computing 2nd power */
	r__1 = xp1 - *x;
/* Computing 2nd power */
	r__2 = yp - *y;
	dist1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xp2 - *x;
/* Computing 2nd power */
	r__2 = yp - *y;
	dist2 = sqrt(r__1 * r__1 + r__2 * r__2);
	if (dist1 < dist2) {
	    *x = xp1;
	} else {
	    *x = xp2;
	}
/*  NOW A CHECK TO MAKE SURE THAT SOMETHING DIDN'T FALL THROUGH */
    } else {
	mesage_(" **  ERROR  -  IMPOSSIBLE CASE IN SNAPPT  ** ", (ftnlen)45);
	pltbel_();
    }
    return 0;
} /* snappt_ */

#ifdef __cplusplus
	}
#endif

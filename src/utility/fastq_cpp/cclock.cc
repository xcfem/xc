/* cclock.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int cclock_(real *x, real *y, integer *n, logical *ccw, 
	logical *err, logical *indetr)
{
    /* System generated locals */
    integer i__1;
    real r__1;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static integer i__;
    static real pi, diff;
    static integer next;
    static real agold, agnew, spiro, twopi;
    extern /* Subroutine */ int mesage_(char *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE CCLOCK = DETERMINES IF THE PERIMETER OF A REGION IS STATED */
/*                      IN COUNTER-CLOCKWISE FASHION */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     PERIM = GENERATES THE PERIMETER OF A REGION */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     CCW    = .TRUE. IF THE PERIMETER IS IN COUNTER-CLOCKWISE ORDER */
/*     ERR    = .TRUE. IF THE ORDER COULD NOT BE DETERMINED, OR IF AN */
/*              ERROR OCCURS CHECKING THE ORDER */
/*     N      = THE NUMBER OF NODES IN THE PERIMETER (MUST BE AT LEAST 3) */
/* *********************************************************************** */
    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    *err = TRUE_;
    *indetr = FALSE_;
    pi = atan2((float)0., (float)-1.);
    twopi = pi + pi;
    if (*n < 3) {
	mesage_("PERIMETER MUST CONTAIN MORE THAN 3 NODES", (ftnlen)40);
	goto L110;
    }
    spiro = (float)0.;
    if (x[1] == x[*n] && y[1] == y[*n]) {
	mesage_("PERIMETER CONTAINS DUPLICATE NODE LOCATIONS", (ftnlen)43);
	goto L110;
    }
    agold = atan2(y[1] - y[*n], x[1] - x[*n]);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	next = i__ + 1;
	if (next > *n) {
	    next = 1;
	}
	if (x[next] == x[i__] && y[next] == y[i__]) {
	    mesage_("PERIMETER CONTAINS DUPLICATE NODE LOCATIONS", (ftnlen)43)
		    ;
	    goto L110;
	}
	agnew = atan2(y[next] - y[i__], x[next] - x[i__]);
	diff = agnew - agold;
	if (diff > pi) {
	    diff -= twopi;
	}
	if (diff < -pi) {
	    diff += twopi;
	}
	if ((r__1 = dabs(diff) - pi, dabs(r__1)) < (float).001) {
	    mesage_("PERIMETER CONTAINS SWITCHBACKS", (ftnlen)30);
	    goto L110;
	}
	spiro += diff;
	agold = agnew;
/* L100: */
    }
    *ccw = TRUE_;
    if (spiro < (float)0.) {
	*ccw = FALSE_;
    }
    if (dabs(spiro) < pi || dabs(spiro) > pi * (float)3.) {
	*indetr = TRUE_;
	return 0;
    }
    *err = FALSE_;
    return 0;
/*  ERROR IN THE ROUTINE */
L110:
    return 0;
} /* cclock_ */

#ifdef __cplusplus
	}
#endif

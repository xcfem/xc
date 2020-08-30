/* endtan.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int endtan_(integer *mp, integer *ml, integer *n, integer *
	ipoint, real *coor, integer *ltype, integer *lcon, integer *linkp, 
	integer *linkl, integer *lnum, integer *lpntr, integer *np, real *
	theta, logical *err)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static integer i1, i2, i3, j1, j2, j3;
    static real r1, r2, pi, xa, xk;
    static integer icw, iccw;
    static real tang, xcen, ycen, vectx, vecty, twopi, theta1, theta2;
    static logical addlnk;
    extern /* Subroutine */ int arcpar_(integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, real *, real *, real *, 
	    real *, real *, logical *, integer *, integer *, real *, real *), 
	    ltsort_(integer *, integer *, integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE ENDTAN = GETS THE ANGLE FOR THE TANGENT AT THE END OF */
/*                      A LINE */
/* *********************************************************************** */
/*  VARIABLES USED:  LNUM   = LINE NUMBER */
/*                   LPNTR  = LINE POINTER */
/*                   VECTX  = X VECTOR */
/*                   VECTY     = Y VECTOR */
/*                   ICCW   = THE POINTER TO THE CCW END OF THE LINE */
/*                   ICW    = THE POINTER TO THE CW END OF THE LINE */
/*                   I1     = THE FIRST POINT NUMBER OF THE LINE */
/*                   I2     = THE SECOND POINT NUMBER OF THE LINE */
/*                   I3     = THE THIRD POINT NUMBER OF THE LINE */
/*                   J1     = THE FIRST POINT NUMBER INDEX */
/*                   J2     = THE SECOND POINT NUMBER INDEX */
/*                   J3     = THE THIRD POINT NUMBER INDEX */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    --ipoint;
    linkl -= 3;
    lcon -= 4;
    --ltype;
    --n;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    twopi = pi + pi;
    addlnk = FALSE_;
    *err = FALSE_;
/*  GET THE POINTERS TO THE POINTS THAT DEFINE THE LINE */
    i1 = lcon[*lpntr * 3 + 1];
    i2 = lcon[*lpntr * 3 + 2];
    i3 = lcon[*lpntr * 3 + 3];
    ltsort_(mp, &linkp[3], &i1, &j1, &addlnk);
    ltsort_(mp, &linkp[3], &i2, &j2, &addlnk);
    if (j1 <= 0 || j2 <= 0) {
	return 0;
    }
    if (i3 != 0) {
	i__1 = abs(i3);
	ltsort_(mp, &linkp[3], &i__1, &j3, &addlnk);
	if (j3 <= 0) {
	    return 0;
	}
    } else {
	j3 = 0;
    }
/*  STRAIGHT LINE END TANGENT */
    if (ltype[*lpntr] == 1) {
	if (i1 == *np) {
	    vectx = coor[(j2 << 1) + 1] - coor[(j1 << 1) + 1];
	    vecty = coor[(j2 << 1) + 2] - coor[(j1 << 1) + 2];
	} else if (i2 == *np) {
	    vectx = coor[(j1 << 1) + 1] - coor[(j2 << 1) + 1];
	    vecty = coor[(j1 << 1) + 2] - coor[(j2 << 1) + 2];
	} else {
	    *err = TRUE_;
	    return 0;
	}
	*theta = atan2(vecty, vectx);
/*  ARC LINE END TANGENT */
    } else if (ltype[*lpntr] != 5) {
	arcpar_(mp, &ltype[*lpntr], lnum, &coor[3], &linkp[3], &j1, &j2, &j3, 
		&i3, &xcen, &ycen, &theta1, &theta2, &tang, &r1, &r2, err, &
		iccw, &icw, &xk, &xa);
/*  CHECK FOR THE A CLOSED ARC */
	if (ipoint[iccw] == ipoint[icw]) {
	    *theta = theta2 + pi * (float).5;
	} else if (*np == ipoint[iccw]) {
	    *theta = theta1 + pi * (float).5 + xk;
	} else if (*np == ipoint[icw]) {
	    *theta = theta2 - (pi * (float).5 - xk);
	} else {
	    *err = TRUE_;
	    return 0;
	}
/*  NO OTHER LINES SUPPORTED */
    } else {
	*err = TRUE_;
	mesage_("UNSUPPORTED LINE TYPE IN ENDTAN", (ftnlen)31);
	return 0;
    }
/*  MAKE SURE THAT THETA IS BETWEEN 0 AND 2PI */
    if (*theta < (float)0.) {
	*theta += twopi;
    } else if (*theta > twopi) {
	*theta -= twopi;
    }
    return 0;
} /* endtan_ */

#ifdef __cplusplus
	}
#endif

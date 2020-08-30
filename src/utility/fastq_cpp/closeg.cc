/* closeg.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int closeg_(integer *msnap, real *snapdx, integer *nsnap, 
	real *x, real *y, integer *ii, integer *index, real *xbot, real *xtop,
	 real *ybot, real *ytop)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Local variables */
    static integer i__;
    static real xhold, yhold;
    extern /* Subroutine */ int snappt_(integer *, real *, integer *, real *, 
	    real *);

/* *********************************************************************** */
/*  SUBROUTINE CLOSEG = SUBROUTINE TO RETURN CLOSEST GRID LINE */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     DIGIT = A SUBROUTINE TO INPUT GEOMETRY */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     X      = THE X LOCATION IN USER COORDINATES */
/*     Y      = THE Y LOCATION IN USER COORDINATES */
/* *********************************************************************** */
/*  FIND CLOSEST GRID CROSSING IN X OR Y */
    /* Parameter adjustments */
    snapdx -= 3;
    --nsnap;

    /* Function Body */
    xhold = *x;
    yhold = *y;
    snappt_(msnap, &snapdx[3], &nsnap[1], &xhold, &yhold);
    if ((r__1 = xhold - *x, dabs(r__1)) < (r__2 = yhold - *y, dabs(r__2))) {
	*index = 1;
    } else {
	*index = 2;
	xhold = yhold;
    }
/*  FIND INDEX TO GRID LINE */
    i__1 = nsnap[*index];
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (snapdx[*index + (i__ << 1)] >= xhold) {
	    *ii = i__;
	    goto L110;
	}
/* L100: */
    }
    *ii = nsnap[*index];
L110:
/*  SET GRID LINE LIMITS */
    if (*index == 1) {
	*xbot = snapdx[(*ii << 1) + 1];
	*xtop = *xbot;
	*ybot = snapdx[4];
	*ytop = snapdx[(nsnap[2] << 1) + 2];
    } else {
	*xbot = snapdx[3];
	*xtop = snapdx[(nsnap[1] << 1) + 1];
	*ybot = snapdx[(*ii << 1) + 2];
	*ytop = *ybot;
    }
    return 0;
} /* closeg_ */

#ifdef __cplusplus
	}
#endif

/* closep.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int closep_(integer *mp, integer *n15, real *x, real *y, 
	integer *ipoint, real *coor, integer *linkp, integer *jj)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__;
    static real dmin__, dist;
    static integer ipntr;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE CLOSE = FINDS THE CLOSEST EXISTING POINT TO THE MOUSE */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     INPUT  = INPUTS MESH DEFINITIONS FROM THE LIGHT TABLE */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     X      = THE X LOCATION IN USER COORDINATES */
/*     Y      = THE Y LOCATION IN USER COORDINATES */
/*     POINT  = ARRAY OF VALUES DEFINING A POINT */
/*               (I, 1) = THE NUMBER OF THE POINT */
/*               (I, 2) = THE X COORDINATE OF THE POINT */
/*               (I, 3) = THE Y COORDINATE OF THE POINT */
/*               (I, 4) = THE BOUNDARY FLAG OF THE POINT */
/*     I      = THE NUMBER OF THE CLOSEST POINT FOUND */
/*     K      = THE NUMBER OF POINTS IN THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    --ipoint;

    /* Function Body */
    addlnk = FALSE_;
    dmin__ = (float)1e5;
    i__1 = *n15;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mp, &linkp[3], &i__, &ipntr, &addlnk);
	if (ipntr > 0) {
/* Computing 2nd power */
	    r__1 = coor[(ipntr << 1) + 1] - *x;
/* Computing 2nd power */
	    r__2 = coor[(ipntr << 1) + 2] - *y;
	    dist = sqrt(r__1 * r__1 + r__2 * r__2);
	    if (dist < dmin__) {
		dmin__ = dist;
		*jj = ipoint[ipntr];
	    }
	}
/* L100: */
    }
    return 0;
} /* closep_ */

#ifdef __cplusplus
	}
#endif

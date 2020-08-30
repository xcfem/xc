/* fq_rotate.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int fq_rotate__(integer *n, real *x, real *y, integer *nid, 
	integer *newf)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j, nlast;
    static real xlast, ylast;

/* *********************************************************************** */
/*  SUBROUTINE ROTATE = CIRCULARLY SHIFTS THE DATA IN X,  Y,  AND NID */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nid;
    --y;
    --x;

    /* Function Body */
    if (*newf <= 1 || *newf > *n) {
	return 0;
    }
/*  BUBBLE UP THROUGH THE ARRAYS AS MANY TIMES AS NEEDED */
    i__1 = *newf - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	xlast = x[1];
	ylast = y[1];
	nlast = nid[1];
	i__2 = *n - 1;
	for (j = 1; j <= i__2; ++j) {
	    x[j] = x[j + 1];
	    y[j] = y[j + 1];
	    nid[j] = nid[j + 1];
/* L100: */
	}
	x[*n] = xlast;
	y[*n] = ylast;
	nid[*n] = nlast;
/* L110: */
    }
    return 0;
} /* fq_rotate__ */

#ifdef __cplusplus
	}
#endif

/* bubble.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int bubble_(real *x, integer *karry, integer *nord, integer *
	n)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j, ii, jm1, kval;
    static real xval;
    static integer istart;

/* *********************************************************************** */
/*  SUBROUTINE BUBBLE=SORTS ALL VALUES X(I), KARRY(I) INTO DECREASING */
/*                      ORDER, ASSUMING THAT VALUES 1 TO NORD ARE SORTED */
/* *********************************************************************** */
    /* Parameter adjustments */
    --karry;
    --x;

    /* Function Body */
    if (*n <= 1) {
	return 0;
    }
/* Computing MAX */
    i__1 = *nord + 1;
    istart = max(i__1,2);
    if (istart > *n) {
	return 0;
    }
    i__1 = *n;
    for (j = istart; j <= i__1; ++j) {
	xval = x[j];
	kval = karry[j];
	jm1 = j - 1;
	i__ = j;
	i__2 = jm1;
	for (ii = 1; ii <= i__2; ++ii) {
	    if (xval <= x[i__ - 1]) {
		goto L110;
	    }
	    x[i__] = x[i__ - 1];
	    karry[i__] = karry[i__ - 1];
	    --i__;
/* L100: */
	}
L110:
	x[i__] = xval;
	karry[i__] = kval;
/* L120: */
    }
    return 0;
} /* bubble_ */

#ifdef __cplusplus
	}
#endif

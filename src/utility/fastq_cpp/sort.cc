/* sort.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int sort_(integer *n, integer *ix, integer *iy)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, j, l, m, m1, nn, keepx, keepy;

/* *********************************************************************** */
/*  SUBROUTINE SORT = SORT THE ARRAY IX,  CARRYING ALONG IY */
/* *********************************************************************** */
    /* Parameter adjustments */
    --iy;
    --ix;

    /* Function Body */
    nn = *n;
    m = nn;
L100:
    m = m * 9 / 16;
    if (m <= 0) {
	return 0;
    }
    m1 = m + 1;
    i__1 = nn;
    for (j = m1; j <= i__1; ++j) {
	l = j;
	i__ = j - m;
L110:
	if (ix[l] < ix[i__]) {
	    keepx = ix[i__];
	    keepy = iy[i__];
	    ix[i__] = ix[l];
	    iy[i__] = iy[l];
	    ix[l] = keepx;
	    iy[l] = keepy;
	    l = i__;
	    i__ -= m;
	    if (i__ >= 1) {
		goto L110;
	    }
	}
/* L120: */
    }
    goto L100;
} /* sort_ */

#ifdef __cplusplus
	}
#endif

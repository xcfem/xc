/* revers.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int revers_(real *x, integer *n)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m;
    static real xk;
    static integer nup;

/* *********************************************************************** */
/*  SUBROUTINE REVERS = REVERS THE REAL ARRAY OF X (I), I=1, N */
/* *********************************************************************** */
    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*n <= 1) {
	return 0;
    }
    nup = *n + 1;
    m = *n / 2;
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	--nup;
	xk = x[i__];
	x[i__] = x[nup];
	x[nup] = xk;
/* L100: */
    }
    return 0;
} /* revers_ */

#ifdef __cplusplus
	}
#endif

/* normnd.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int normnd_(integer *npnode, real *bmesur, real *rmax)
{
    /* System generated locals */
    integer i__1;
    real r__1;

    /* Local variables */
    static real bmin;
    static integer node;
    static real bmax;

/* *********************************************************************** */
/*  SUBROUTINE NORMND = NORMALIZES A NODE VARIABLE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --bmesur;

    /* Function Body */
    bmin = bmesur[1];
    bmax = bmesur[1];
    i__1 = *npnode;
    for (node = 2; node <= i__1; ++node) {
/* Computing MAX */
	r__1 = bmesur[node];
	bmax = dmax(r__1,bmax);
/* Computing MIN */
	r__1 = bmesur[node];
	bmin = dmin(r__1,bmin);
/* L100: */
    }
    bmax -= bmin;
    i__1 = *npnode;
    for (node = 1; node <= i__1; ++node) {
	bmesur[node] -= bmin;
/* L110: */
    }
/*  RMAX = MAXIMUM RATIO FOR PLATEAU VALUES */
    i__1 = *npnode;
    for (node = 1; node <= i__1; ++node) {
	if (bmesur[node] >= bmax * *rmax) {
	    bmesur[node] = (float)1.;
	} else {
	    bmesur[node] /= bmax * *rmax;
	}
/* L120: */
    }
    return 0;
} /* normnd_ */

#ifdef __cplusplus
	}
#endif

/* fq_minmax.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int minmax_fq__(integer *ndim, integer *n, real *x, real *y, 
	real *xmin, real *xmax, real *ymin, real *ymax)
{
    /* System generated locals */
    integer i__1;
    real r__1;

    /* Local variables */
    static integer i__;

/* *********************************************************************** */
/*  SUBROUTINE MINMAX_FQ = COMPARES AND RECORDS X AND Y EXTREMES */
/* *********************************************************************** */
    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    *xmin = x[1];
    *xmax = x[1];
    *ymin = y[1];
    *ymax = y[1];
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MIN */
	r__1 = x[i__];
	*xmin = dmin(r__1,*xmin);
/* Computing MAX */
	r__1 = x[i__];
	*xmax = dmax(r__1,*xmax);
/* Computing MIN */
	r__1 = y[i__];
	*ymin = dmin(r__1,*ymin);
/* Computing MAX */
	r__1 = y[i__];
	*ymax = dmax(r__1,*ymax);
/* L100: */
    }
    return 0;
} /* minmax_fq__ */

#ifdef __cplusplus
	}
#endif

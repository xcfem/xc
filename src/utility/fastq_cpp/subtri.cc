/* subtri.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int subtri_(integer *nper, integer *newper, real *x, real *y,
	 integer *nid, real *xsub, real *ysub, integer *nidsub, integer *m1, 
	integer *m2, integer *iadd, integer *itri, real *xcen, real *ycen)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, j;
    static real xdif, ydif, xint, yint;
    static integer itri2, kount;

/* *********************************************************************** */
/*  SUBROUTINE SUBPER = PUTS A SUBREGION'S PERIMETER INTO THE NPERIM */
/*                      ARRAYS */
/* *********************************************************************** */
/*  PUT SIDE ONE AND TWO INTO THE PERIMETER LIST */
    /* Parameter adjustments */
    --nidsub;
    --ysub;
    --xsub;
    --nid;
    --y;
    --x;

    /* Function Body */
    kount = 0;
    i__1 = *m1 + *m2 + 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++kount;
	j = i__ + *iadd;
	if (j > *nper) {
	    j -= *nper;
	}
	xsub[kount] = x[j];
	ysub[kount] = y[j];
	nidsub[kount] = nid[j];
/* L100: */
    }
/*  PUT SIDE THREE INTO THE LIST */
    xdif = *xcen - xsub[kount];
    ydif = *ycen - ysub[kount];
    xint = xdif / (doublereal) (*m1);
    yint = ydif / (doublereal) (*m1);
    i__1 = *m1 - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++kount;
	xsub[kount] = xsub[kount - 1] + xint;
	ysub[kount] = ysub[kount - 1] + yint;
	nidsub[kount] = *itri * 100000 + *m1 - i__ + 1;
/* L110: */
    }
/*  ENTER THE CENTER POINT */
    ++kount;
    xsub[kount] = *xcen;
    ysub[kount] = *ycen;
    nidsub[kount] = 100000;
/*  PUT SIDE FOUR INTO THE LIST */
    itri2 = *itri + 2;
    if (itri2 > 3) {
	itri2 += -3;
    }
    xdif = x[*iadd + 1] - *xcen;
    ydif = y[*iadd + 1] - *ycen;
    xint = xdif / (doublereal) (*m2);
    yint = ydif / (doublereal) (*m2);
    i__1 = *m2 - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++kount;
	xsub[kount] = xsub[kount - 1] + xint;
	ysub[kount] = ysub[kount - 1] + yint;
	nidsub[kount] = itri2 * 100000 + i__ + 1;
/* L120: */
    }
    *newper = kount;
    return 0;
} /* subtri_ */

#ifdef __cplusplus
	}
#endif

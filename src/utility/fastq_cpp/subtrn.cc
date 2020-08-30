/* subtrn.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int subtrn_(integer *nper, integer *newper, integer *ip, 
	real *x, real *y, integer *nid, real *xsub, real *ysub, integer *
	nidsub, integer *i1, integer *i2, integer *i3, integer *i4, integer *
	i5, integer *i6, integer *i7, integer *i8, real *xcen1, real *ycen1, 
	real *xcen2, real *ycen2, real *xmid1, real *ymid1, real *xmid2, real 
	*ymid2, real *ccw, real *err)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;
    static real xdif, ydif, xint, yint;
    static integer kount;

/* *********************************************************************** */
/*  SUBROUTINE SUBTRN = PUTS A TRANSITION'S SUBREGION'S PERIMETER INTO */
/*                      THE NPERIM ARRAYS */
/* *********************************************************************** */
/*  PUT THE CORRECT PORTION OF THE PERIMETER IN XSUB,  YSUB,  AND NIDSUB */
/*  BASED ON THE VALUE OF IP (WHICH OF THE 6 SUBREGIONS ARE NEEDED) */
    /* Parameter adjustments */
    --nidsub;
    --ysub;
    --xsub;
    --nid;
    --y;
    --x;

    /* Function Body */
    kount = 0;
/*  SUBREGION 1  -  SIDE 1 */
    if (*ip == 1) {
	i__1 = *i2;
	for (i__ = *i1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = x[i__];
	    ysub[kount] = y[i__];
	    nidsub[kount] = nid[i__];
/* L100: */
	}
/*  SUBREGION 1  -  SIDE 2 */
	xdif = *xcen2 - x[*i2];
	ydif = *ycen2 - y[*i2];
	xint = xdif / (doublereal) (*nper - *i8 + 1);
	yint = ydif / (doublereal) (*nper - *i8 + 1);
	i__1 = *nper - *i8;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = *nper + 300000 - *i8 - i__ + 2;
/* L110: */
	}
	++kount;
	xsub[kount] = *xcen2;
	ysub[kount] = *ycen2;
	nidsub[kount] = 200000;
/*  SUBREGION 1  -  SIDE 3 */
	xdif = x[*i8] - *xcen2;
	ydif = y[*i8] - *ycen2;
	xint = xdif / (doublereal) (*i2 - *i1);
	yint = ydif / (doublereal) (*i2 - *i1);
	i__1 = *i2 - *i1 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = i__ + 100001;
/* L120: */
	}
/*  SUBREGION 1  -  SIDE 4 */
	i__1 = *nper;
	for (i__ = *i8; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = x[i__];
	    ysub[kount] = y[i__];
	    nidsub[kount] = nid[i__];
/* L130: */
	}
/*  SUBREGION 2  -  SIDE 1 */
    } else if (*ip == 2) {
	i__1 = *i8;
	for (i__ = *i7; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = x[i__];
	    ysub[kount] = y[i__];
	    nidsub[kount] = nid[i__];
/* L140: */
	}
/*  SUBREGION 2  -  SIDE 2 */
	xdif = *xcen2 - x[*i8];
	ydif = *ycen2 - y[*i8];
	xint = xdif / (doublereal) (*i2 - *i1);
	yint = ydif / (doublereal) (*i2 - *i1);
	i__1 = *i2 - *i1 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = *i2 + 100000 - *i1 - i__ + 1;
/* L150: */
	}
	++kount;
	xsub[kount] = *xcen2;
	ysub[kount] = *ycen2;
	nidsub[kount] = 200000;
/*  SUBREGION 2  -  SIDE 3 */
	xdif = *xmid2 - *xcen2;
	ydif = *ymid2 - *ycen2;
	xint = xdif / (doublereal) (*i3 - *i2);
	yint = ydif / (doublereal) (*i3 - *i2);
	i__1 = *i3 - *i2 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = i__ + 200001;
/* L160: */
	}
	++kount;
	xsub[kount] = *xmid2;
	ysub[kount] = *ymid2;
	nidsub[kount] = *nper + 700000 - *i8 + 2;
/*  SUBREGION 2  -  SIDE 4 */
	xdif = x[*i7] - *xmid2;
	ydif = y[*i7] - *ymid2;
	xint = xdif / (doublereal) (*i2 - *i1);
	yint = ydif / (doublereal) (*i2 - *i1);
	i__1 = *i2 - *i1 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = nidsub[kount - 1] + 1;
/* L170: */
	}
/*  SUBREGION 3  -  SIDE 1 */
    } else if (*ip == 3) {
	++kount;
	xsub[kount] = x[*i3];
	ysub[kount] = y[*i3];
	nidsub[kount] = nid[*i3];
	xdif = *xmid2 - x[*i3];
	ydif = *ymid2 - y[*i3];
	xint = xdif / (doublereal) (*nper - *i8 + 1);
	yint = ydif / (doublereal) (*nper - *i8 + 1);
	i__1 = *nper - *i8 + 1;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = i__ + 700000;
/* L180: */
	}
	++kount;
	xsub[kount] = *xmid2;
	ysub[kount] = *ymid2;
	nidsub[kount] = *nper + 700000 - *i8 + 2;
/*  SUBREGION 3  -  SIDE 2 */
	xdif = *xcen2 - *xmid2;
	ydif = *ycen2 - *ymid2;
	xint = xdif / (doublereal) (*i3 - *i2);
	yint = ydif / (doublereal) (*i3 - *i2);
	i__1 = *i3 - *i2 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = *i3 + 200000 - *i2 - i__ + 1;
/* L190: */
	}
	++kount;
	xsub[kount] = *xcen2;
	ysub[kount] = *ycen2;
	nidsub[kount] = 200000;
/*  SUBREGION 3  -  SIDE 3 */
	xdif = x[*i2] - *xcen2;
	ydif = y[*i2] - *ycen2;
	xint = xdif / (doublereal) (*nper - *i8 + 1);
	yint = ydif / (doublereal) (*nper - *i8 + 1);
	i__1 = *nper - *i8;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = i__ + 300001;
/* L200: */
	}
/*  SUBREGION 3  -  SIDE 4 */
	i__1 = *i3 - 1;
	for (i__ = *i2; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = x[i__];
	    ysub[kount] = y[i__];
	    nidsub[kount] = nid[i__];
/* L210: */
	}
/*  SUBREGION 4  -  SIDE 1 AND 2 */
    } else if (*ip == 4) {
	i__1 = *i6;
	for (i__ = *i4; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = x[i__];
	    ysub[kount] = y[i__];
	    nidsub[kount] = nid[i__];
/* L220: */
	}
/*  SUBREGION 4  -  SIDE 3 */
	xdif = *xcen1 - x[*i6];
	ydif = *ycen1 - y[*i6];
	xint = xdif / (doublereal) (*i5 - *i4);
	yint = ydif / (doublereal) (*i5 - *i4);
	i__1 = *i5 - *i4 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = *i5 + 400000 - *i4 - i__ + 1;
/* L230: */
	}
	++kount;
	xsub[kount] = *xcen1;
	ysub[kount] = *ycen1;
	nidsub[kount] = 100000;
/*  SUBREGION 4  -  SIDE 4 */
	xdif = x[*i4] - *xcen1;
	ydif = y[*i4] - *ycen1;
	xint = xdif / (doublereal) (*i6 - *i5);
	yint = ydif / (doublereal) (*i6 - *i5);
	i__1 = *i6 - *i5 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = i__ + 600001;
/* L240: */
	}
/*  SUBREGION 5  -  SIDE 1 */
    } else if (*ip == 5) {
	i__1 = *i7;
	for (i__ = *i6; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = x[i__];
	    ysub[kount] = y[i__];
	    nidsub[kount] = nid[i__];
/* L250: */
	}
/*  SUBREGION 5  -  SIDE 2 */
	xdif = *xmid1 - x[*i7];
	ydif = *ymid1 - y[*i7];
	xint = xdif / (doublereal) (*i5 - *i4);
	yint = ydif / (doublereal) (*i5 - *i4);
	i__1 = *i5 - *i4 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = *i6 + 700000 - *i4 - i__ + 1;
/* L260: */
	}
	++kount;
	xsub[kount] = *xmid1;
	ysub[kount] = *ymid1;
	nidsub[kount] = *i6 + 700000 - *i5 + 1;
/*  SUBREGION 5  -  SIDE 3 */
	xdif = *xcen1 - *xmid1;
	ydif = *ycen1 - *ymid1;
	xint = xdif / (doublereal) (*i4 - *i3);
	yint = ydif / (doublereal) (*i4 - *i3);
	i__1 = *i4 - *i3 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = *i4 + 500000 - *i3 - i__ + 1;
/* L270: */
	}
	++kount;
	xsub[kount] = *xcen1;
	ysub[kount] = *ycen1;
	nidsub[kount] = 100000;
/*  SUBREGION 5  -  SIDE 4 */
	xdif = x[*i6] - *xcen1;
	ydif = y[*i6] - *ycen1;
	xint = xdif / (doublereal) (*i5 - *i4);
	yint = ydif / (doublereal) (*i5 - *i4);
	i__1 = *i5 - *i4 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = i__ + 400001;
/* L280: */
	}
/*  SUBREGION 6  -  SIDE 1 */
    } else if (*ip == 6) {
	++kount;
	xsub[kount] = *xmid1;
	ysub[kount] = *ymid1;
	nidsub[kount] = *i6 + 700000 - *i5 + 1;
	xdif = x[*i3] - *xmid1;
	ydif = y[*i3] - *ymid1;
	xint = xdif / (doublereal) (*i6 - *i5);
	yint = ydif / (doublereal) (*i6 - *i5);
	i__1 = *i6 - *i5 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = *i6 + 700000 - *i5 - i__ + 1;
/* L290: */
	}
/*  SUBREGION 6  -  SIDE 2 */
	i__1 = *i4;
	for (i__ = *i3; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = x[i__];
	    ysub[kount] = y[i__];
	    nidsub[kount] = nid[i__];
/* L300: */
	}
/*  SUBREGION 6  -  SIDE 3 */
	xdif = *xcen1 - x[*i4];
	ydif = *ycen1 - y[*i4];
	xint = xdif / (doublereal) (*i6 - *i5);
	yint = ydif / (doublereal) (*i6 - *i5);
	i__1 = *i6 - *i5 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = *i6 + 600000 - *i5 - i__ + 1;
/* L310: */
	}
	++kount;
	xsub[kount] = *xcen1;
	ysub[kount] = *ycen1;
	nidsub[kount] = 100000;
/*  SUBREGION 6  -  SIDE 4 */
	xdif = *xmid1 - *xcen1;
	ydif = *ymid1 - *ycen1;
	xint = xdif / (doublereal) (*i4 - *i3);
	yint = ydif / (doublereal) (*i4 - *i3);
	i__1 = *i4 - *i3 - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++kount;
	    xsub[kount] = xsub[kount - 1] + xint;
	    ysub[kount] = ysub[kount - 1] + yint;
	    nidsub[kount] = i__ + 500001;
/* L320: */
	}
    }
    *newper = kount;
    return 0;
} /* subtrn_ */

#ifdef __cplusplus
	}
#endif

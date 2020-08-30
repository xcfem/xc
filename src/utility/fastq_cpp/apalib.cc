/* apalib.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int apalib_(integer *mxnd, real *xn, real *yn, integer *lxk, 
	integer *nxl, integer *k, integer *nodes, real *area, real *xcen, 
	real *ycen, integer *knum, integer *klib, integer *nlib, real *alib, 
	real *xclib, real *yclib)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, j, n1, n2, n3, n4, ik;
    static logical ccw;
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE APALIB = LIBRARY OF ELEMENT DATA USED TO AVOID DUPLICATE */
/*                      COMPUTATIONS */
/* *********************************************************************** */
/*  SEARCH LIBRARY */
    /* Parameter adjustments */
    nxl -= 3;
    lxk -= 5;
    --yn;
    --xn;
    --nodes;
    --klib;
    nlib -= 5;
    --alib;
    --xclib;
    --yclib;

    /* Function Body */
    if (*knum > 0) {
	i__1 = *knum;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (*k - klib[i__] == 0) {
/*  FETCH FROM LIBRARY */
		ik = i__;
		for (j = 1; j <= 4; ++j) {
		    nodes[j] = nlib[j + (ik << 2)];
/* L100: */
		}
		*area = alib[ik];
		*xcen = xclib[ik];
		*ycen = yclib[ik];
		return 0;
	    }
/* L110: */
	}
    }
/*  COMPUTE NEW DATA */
    ccw = TRUE_;
    gnxka_(mxnd, &xn[1], &yn[1], k, &nodes[1], area, &lxk[5], &nxl[3], &ccw);
    n1 = nodes[1];
    n2 = nodes[2];
    n3 = nodes[3];
    n4 = nodes[4];
    *xcen = (xn[n1] + xn[n2] + xn[n3] + xn[n4]) * (float).25;
    *ycen = (yn[n1] + yn[n2] + yn[n3] + yn[n4]) * (float).25;
    if (*knum >= 8) {
	return 0;
    }
/*  FILE NEW DATA IN LIBRARY */
    ++(*knum);
    for (i__ = 1; i__ <= 4; ++i__) {
	nlib[i__ + (*knum << 2)] = nodes[i__];
/* L120: */
    }
    klib[*knum] = *k;
    alib[*knum] = *area;
    xclib[*knum] = *xcen;
    yclib[*knum] = *ycen;
    return 0;
} /* apalib_ */

#ifdef __cplusplus
	}
#endif

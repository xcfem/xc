/* mak2el.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int mak2el_(integer *mp, integer *mxnper, integer *mxnd, 
	integer *nnn0, integer *nnn, integer *kkk, real *x, real *y, integer *
	nid, real *xn, real *yn, integer *nuid, integer *lxk, real *coor, 
	integer *ip3)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 ** WARNING **  -  COLINEAR REFERENCE NO\
DE FOR ELEMENT:\002,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__;
    static real x1, y1, x2, y2;
    static integer kount;
    static real crossp;

    /* Fortran I/O blocks */
    static cilist io___8 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE MAK2EL = GENERATES  (ADDS) ELEMENT CONNECTIVITY FOR 2 NODES */
/* *********************************************************************** */
/*  PUT NODES AND NUID'S INTO THE PROPER LOCATIONS */
    /* Parameter adjustments */
    coor -= 3;
    --nid;
    --y;
    --x;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;

    /* Function Body */
    kount = 0;
    i__1 = *nnn - 1;
    for (i__ = *nnn0; i__ <= i__1; ++i__) {
	++kount;
	++(*kkk);
	xn[i__] = x[kount];
	yn[i__] = y[kount];
	nuid[i__] = nid[kount];
	lxk[(*kkk << 2) + 1] = i__;
	lxk[(*kkk << 2) + 2] = i__ + 1;
	lxk[(*kkk << 2) + 3] = 0;
	lxk[(*kkk << 2) + 4] = 0;
	if (*ip3 > 0) {
	    x1 = x[i__ + 1] - x[i__];
	    y1 = y[i__ + 1] - y[i__];
	    x2 = x[i__ + 1] - coor[(*ip3 << 1) + 1];
	    y2 = y[i__ + 1] - coor[(*ip3 << 1) + 2];
	    crossp = x1 * y2 - y1 * x2;
	    if (crossp > (float)0.) {
		lxk[(*kkk << 2) + 1] = i__ + 1;
		lxk[(*kkk << 2) + 2] = i__;
	    }
	    if (crossp * crossp < (x1 * x1 + y1 * y1) * (float).01 * (x2 * x2 
		    + y2 * y2)) {
		s_wsfe(&io___8);
		do_fio(&c__1, (char *)&(*kkk), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
/* L100: */
    }
    xn[*nnn] = x[kount + 1];
    yn[*nnn] = y[kount + 1];
    nuid[*nnn] = nid[kount + 1];
    return 0;
} /* mak2el_ */

#ifdef __cplusplus
	}
#endif

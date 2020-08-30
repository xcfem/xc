/* tmschm.f -- translated by f2c (version 20160102).
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
static integer c__2 = 2;
static integer c__3 = 3;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int tmschm_(integer *nper, integer *kkk, integer *lll, 
	integer *nnn, integer *ml, integer *ms, integer *nspr, integer *
	islist, integer *nint, integer *ifline, integer *nlps, integer *
	illist, integer *linkl, integer *links, integer *mxnper, integer *
	maxprm, integer *max3, integer *mxnd, real *x, real *y, integer *nid, 
	integer *nnps, real *angle, real *xn, real *yn, integer *nuid, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, real *xsub, 
	real *ysub, integer *nidsub, integer *indx, integer *iavail, integer *
	navail, logical *ccw, logical *err)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer m1a, m1b, m2a, m2b, m3a, m3b;
    static real xcen, ycen;
    extern /* Subroutine */ int getm3_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, integer *, real *
	    , integer *, integer *, integer *, integer *, integer *, integer *
	    , integer *, real *, real *, logical *, logical *);
    static logical final;
    extern /* Subroutine */ int rmesh_(integer *, integer *, real *, real *, 
	    integer *, real *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *);
    static integer kkkold, lllold, nnnold, newper;
    extern /* Subroutine */ int fixsub_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *), subtri_(integer *, integer *, 
	    real *, real *, integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *);

/* *********************************************************************** */
/*  TMSCHM - "T" MESH SCHEME; CALCULATE A "TRIANGULAR" MAPPED MESH */
/*           (3 RECTANGULAR SUBREGIONS) */
/* *********************************************************************** */
/*  SET UP THE TRIANGLE DIVISIONS, AND FIND THE CENTER POINT */
    /* Parameter adjustments */
    linkl -= 3;
    --nint;
    links -= 3;
    --illist;
    --nlps;
    --ifline;
    --islist;
    --nidsub;
    --ysub;
    --xsub;
    --angle;
    --y;
    --x;
    --nid;
    --nnps;
    --indx;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;

    /* Function Body */
    getm3_(ml, ms, max3, nspr, &islist[1], &nint[1], &ifline[1], &nlps[1], &
	    illist[1], &linkl[3], &links[3], &x[1], &y[1], &nid[1], &nnps[1], 
	    &angle[1], nper, &m1a, &m1b, &m2a, &m2b, &m3a, &m3b, &xcen, &ycen,
	     ccw, err);
    final = FALSE_;
/*  SET UP THE FIRST SUBREGION, AND SEND IT OFF TO BE GENERATED */
    if (! (*err)) {
	subtri_(nper, &newper, &x[1], &y[1], &nid[1], &xsub[1], &ysub[1], &
		nidsub[1], &m1b, &m2a, &m1a, &c__1, &xcen, &ycen);
	nnnold = *nnn;
	kkkold = *kkk;
	lllold = *lll;
	rmesh_(&newper, mxnd, &xsub[1], &ysub[1], &nidsub[1], &xn[1], &yn[1], 
		&nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &m1b, &m2a, kkk,
		 &kkkold, nnn, &nnnold, lll, &lllold, iavail, navail, err);
    }
/*  SET UP THE SECOND SUBREGION, AND SEND IT OFF TO BE GENERATED */
    if (! (*err)) {
	i__1 = m1a + m1b + m2a;
	subtri_(nper, &newper, &x[1], &y[1], &nid[1], &xsub[1], &ysub[1], &
		nidsub[1], &m2b, &m3a, &i__1, &c__2, &xcen, &ycen);
	nnnold = *nnn;
	kkkold = *kkk;
	lllold = *lll;
	rmesh_(&newper, mxnd, &xsub[1], &ysub[1], &nidsub[1], &xn[1], &yn[1], 
		&nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &m2b, &m3a, kkk,
		 &kkkold, nnn, &nnnold, lll, &lllold, iavail, navail, err);
	fixsub_(mxnd, &nnnold, nnn, &lllold, lll, &kkkold, kkk, &xn[1], &yn[1]
		, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &indx[1], 
		iavail, navail, &final);
    }
/*  SET UP THE THIRD SUBREGION, AND SEND IT OFF TO BE GENERATED */
    if (! (*err)) {
	i__1 = m1a + m1b + m2a + m2b + m3a;
	subtri_(nper, &newper, &x[1], &y[1], &nid[1], &xsub[1], &ysub[1], &
		nidsub[1], &m3b, &m1a, &i__1, &c__3, &xcen, &ycen);
	nnnold = *nnn;
	kkkold = *kkk;
	lllold = *lll;
	rmesh_(&newper, mxnd, &xsub[1], &ysub[1], &nidsub[1], &xn[1], &yn[1], 
		&nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &m3b, &m1a, kkk,
		 &kkkold, nnn, &nnnold, lll, &lllold, iavail, navail, err);
	final = TRUE_;
	fixsub_(mxnd, &nnnold, nnn, &lllold, lll, &kkkold, kkk, &xn[1], &yn[1]
		, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &indx[1], 
		iavail, navail, &final);
    }
    return 0;
} /* tmschm_ */

#ifdef __cplusplus
	}
#endif

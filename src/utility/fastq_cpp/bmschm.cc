/* bmschm.f -- translated by f2c (version 20160102).
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
static integer c__4 = 4;
static integer c__5 = 5;
static integer c__6 = 6;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int bmschm_(integer *nper, integer *kkk, integer *lll, 
	integer *nnn, integer *ml, integer *ms, integer *nspr, integer *
	islist, integer *nint, integer *ifline, integer *nlps, integer *
	illist, integer *linkl, integer *links, integer *mxnper, integer *
	maxprm, integer *max3, integer *mxnd, real *x, real *y, integer *nid, 
	integer *nnps, real *angle, real *xn, real *yn, integer *nuid, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, real *xsub, 
	real *ysub, integer *nidsub, integer *indx, integer *iavail, integer *
	navail, logical *ccw, logical *halfc, logical *err)
{
    static integer i1, i2, i3, i4, i5, i6, i7, i8, m1, m2;
    static real xcen1, ycen1, xcen2, ycen2, xmid1, ymid1, xmid2, ymid2;
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
	    integer *, integer *, logical *), gettrn_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, logical *,
	     logical *, logical *), subtrn_(integer *, integer *, integer *, 
	    real *, real *, integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, real *, real *, real *, real *, real *, real *,
	     real *, logical *, logical *);

/* *********************************************************************** */
/*  BMSCHM - "B" MESH SCHEME; CALCULATE A "TRANSITION" MAPPED MESH */
/*      (2 TRIANGULAR SUBREGIONS WITH 3 RECTANGULAR SUBREGIONS/TRIANGLE) */
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
    gettrn_(ml, ms, max3, nspr, &islist[1], &nint[1], &ifline[1], &nlps[1], &
	    illist[1], &linkl[3], &links[3], &x[1], &y[1], &nid[1], &nnps[1], 
	    &angle[1], nper, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &xcen1, &
	    ycen1, &xcen2, &ycen2, &xmid1, &ymid1, &xmid2, &ymid2, ccw, halfc,
	     err);
    final = FALSE_;
/*  SET UP THE FIRST SUBREGION, AND SEND IT OFF TO BE GENERATED */
    if (! (*err)) {
	subtrn_(nper, &newper, &c__1, &x[1], &y[1], &nid[1], &xsub[1], &ysub[
		1], &nidsub[1], &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &
		xcen1, &ycen1, &xcen2, &ycen2, &xmid1, &ymid1, &xmid2, &ymid2,
		 ccw, err);
	nnnold = *nnn;
	kkkold = *kkk;
	lllold = *lll;
	m1 = i2 - i1;
	m2 = *nper - i8 + 1;
	rmesh_(&newper, mxnd, &xsub[1], &ysub[1], &nidsub[1], &xn[1], &yn[1], 
		&nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &m1, &m2, kkk, &
		kkkold, nnn, &nnnold, lll, &lllold, iavail, navail, err);
    }
/*  SET UP THE SECOND SUBREGION, AND SEND IT OFF TO BE GENERATED */
    if (! (*err)) {
	subtrn_(nper, &newper, &c__2, &x[1], &y[1], &nid[1], &xsub[1], &ysub[
		1], &nidsub[1], &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &
		xcen1, &ycen1, &xcen2, &ycen2, &xmid1, &ymid1, &xmid2, &ymid2,
		 ccw, err);
	nnnold = *nnn;
	kkkold = *kkk;
	lllold = *lll;
	m1 = i8 - i7;
	m2 = i2 - i1;
	rmesh_(&newper, mxnd, &xsub[1], &ysub[1], &nidsub[1], &xn[1], &yn[1], 
		&nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &m1, &m2, kkk, &
		kkkold, nnn, &nnnold, lll, &lllold, iavail, navail, err);
	fixsub_(mxnd, &nnnold, nnn, &lllold, lll, &kkkold, kkk, &xn[1], &yn[1]
		, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &indx[1], 
		iavail, navail, &final);
    }
/*  SET UP THE THIRD SUBREGION, AND SEND IT OFF TO BE GENERATED */
    if (! (*err)) {
	subtrn_(nper, &newper, &c__3, &x[1], &y[1], &nid[1], &xsub[1], &ysub[
		1], &nidsub[1], &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &
		xcen1, &ycen1, &xcen2, &ycen2, &xmid1, &ymid1, &xmid2, &ymid2,
		 ccw, err);
	nnnold = *nnn;
	kkkold = *kkk;
	lllold = *lll;
	m1 = *nper - i8 + 1;
	m2 = i3 - i2;
	rmesh_(&newper, mxnd, &xsub[1], &ysub[1], &nidsub[1], &xn[1], &yn[1], 
		&nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &m1, &m2, kkk, &
		kkkold, nnn, &nnnold, lll, &lllold, iavail, navail, err);
	fixsub_(mxnd, &nnnold, nnn, &lllold, lll, &kkkold, kkk, &xn[1], &yn[1]
		, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &indx[1], 
		iavail, navail, &final);
    }
/*  SET UP THE FOURTH SUBREGION, AND SEND IT OFF TO BE GENERATED */
    if (! (*err)) {
	subtrn_(nper, &newper, &c__4, &x[1], &y[1], &nid[1], &xsub[1], &ysub[
		1], &nidsub[1], &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &
		xcen1, &ycen1, &xcen2, &ycen2, &xmid1, &ymid1, &xmid2, &ymid2,
		 ccw, err);
	nnnold = *nnn;
	kkkold = *kkk;
	lllold = *lll;
	m1 = i5 - i4;
	m2 = i6 - i5;
	rmesh_(&newper, mxnd, &xsub[1], &ysub[1], &nidsub[1], &xn[1], &yn[1], 
		&nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &m1, &m2, kkk, &
		kkkold, nnn, &nnnold, lll, &lllold, iavail, navail, err);
	fixsub_(mxnd, &nnnold, nnn, &lllold, lll, &kkkold, kkk, &xn[1], &yn[1]
		, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &indx[1], 
		iavail, navail, &final);
    }
/*  SET UP THE FIFTH SUBREGION, AND SEND IT OFF TO BE GENERATED */
    if (! (*err)) {
	subtrn_(nper, &newper, &c__5, &x[1], &y[1], &nid[1], &xsub[1], &ysub[
		1], &nidsub[1], &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &
		xcen1, &ycen1, &xcen2, &ycen2, &xmid1, &ymid1, &xmid2, &ymid2,
		 ccw, err);
	nnnold = *nnn;
	kkkold = *kkk;
	lllold = *lll;
	m1 = i7 - i6;
	m2 = i5 - i4;
	rmesh_(&newper, mxnd, &xsub[1], &ysub[1], &nidsub[1], &xn[1], &yn[1], 
		&nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &m1, &m2, kkk, &
		kkkold, nnn, &nnnold, lll, &lllold, iavail, navail, err);
	fixsub_(mxnd, &nnnold, nnn, &lllold, lll, &kkkold, kkk, &xn[1], &yn[1]
		, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &indx[1], 
		iavail, navail, &final);
    }
/*  SET UP THE SIXTH SUBREGION, AND SEND IT OFF TO BE GENERATED */
    if (! (*err)) {
	subtrn_(nper, &newper, &c__6, &x[1], &y[1], &nid[1], &xsub[1], &ysub[
		1], &nidsub[1], &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &
		xcen1, &ycen1, &xcen2, &ycen2, &xmid1, &ymid1, &xmid2, &ymid2,
		 ccw, err);
	nnnold = *nnn;
	kkkold = *kkk;
	lllold = *lll;
	m1 = i6 - i5;
	m2 = i4 - i3;
	rmesh_(&newper, mxnd, &xsub[1], &ysub[1], &nidsub[1], &xn[1], &yn[1], 
		&nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &m1, &m2, kkk, &
		kkkold, nnn, &nnnold, lll, &lllold, iavail, navail, err);
	final = TRUE_;
	fixsub_(mxnd, &nnnold, nnn, &lllold, lll, &kkkold, kkk, &xn[1], &yn[1]
		, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &indx[1], 
		iavail, navail, &final);
    }
    return 0;
} /* bmschm_ */

#ifdef __cplusplus
	}
#endif

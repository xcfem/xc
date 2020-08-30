/* addrow.f -- translated by f2c (version 20160102).
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

/* Common Block Declarations */

/* Table of constant values */

static integer c__1 = 1;
static integer c__2 = 2;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int addrow_(integer *mxnd, integer *mxcorn, integer *mxloop, 
	integer *mln, integer *maxprm, integer *nuid, real *xn, real *yn, 
	real *zn, integer *lxk, integer *kxl, integer *nxl, integer *lxn, 
	real *angle, real *bnsize, integer *lnodes, integer *nbegin, integer *
	nend, integer *nloop, integer *nextn1, integer *linkpr, integer *
	kperim, integer *kkkold, integer *lllold, integer *nnnold, integer *
	iavail, integer *navail, real *xmin, real *xmax, real *ymin, real *
	ymax, real *zmin, real *zmax, char *dev1, integer *lll, integer *kkk, 
	integer *nnn, integer *nnn2, integer *nadj1, integer *nadj2, integer *
	lcorn, integer *kloop, logical *graph, logical *video, integer *kreg, 
	logical *done, logical *sizeit, logical *noroom, logical *err, real *
	xnold, real *ynold, integer *nxkold, integer *linkeg, integer *listeg,
	 real *bmesur, integer *mlink, integer *npnold, integer *npeold, 
	integer *nnxk, real *remesh, real *rexmin, real *rexmax, real *reymin,
	 real *reymax, integer *idivis, real *sizmin, real *emax, real *emin, 
	ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, nxkold_dim1, nxkold_offset, i__1, 
	    i__2;

    /* Local variables */
    static real x[5], y[5];
    static integer n0, n1, n2, n3, new1, new2, new3, new4, idum;
    static real dist;
    static integer idum1, idum2;
    static real time1, time2, dist1, dist2;
    static logical ambig;
    static real ahold;
    extern /* Subroutine */ int pinch_(integer *, integer *, integer *, 
	    integer *, real *, real *, real *, integer *, integer *, integer *
	    , integer *, real *, integer *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    real *, real *, real *, real *, real *, real *, char *, integer *,
	     integer *, integer *, integer *, integer *, integer *, integer *,
	     logical *, logical *, integer *, logical *, logical *, ftnlen);
    static integer nlold, ncorn;
    extern /* Subroutine */ int add2nd_(integer *, integer *, real *, real *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    real *, real *, real *, real *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *, logical *, real *, real *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    real *, real *, real *, real *, real *, integer *, real *, real *,
	     real *), add3nd_(integer *, integer *, real *, real *, integer *,
	     integer *, integer *, integer *, real *, integer *, real *, real 
	    *, real *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, logical *, logical *, real *, real *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, real *, real *, real *, real *, real *, integer *, 
	    real *, real *, real *);
    static integer kount;
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
	    integer *), extnd1_(integer *, real *, real *, real *, integer *, 
	    integer *, integer *, real *, real *, real *), extnd3_(integer *, 
	    real *, real *, real *, integer *, integer *, integer *, real *, 
	    real *, real *), extnd5_(integer *, real *, real *, real *, 
	    integer *, integer *, integer *, real *, real *, real *), addnod_(
	    integer *, integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, real *, real *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, integer *, logical *, logical *, logical *, 
	    real *, real *, integer *, integer *, integer *, real *, integer *
	    , integer *, integer *, integer *, real *, real *, real *, real *,
	     real *, integer *, real *, real *, real *);
    static integer lambig;
    extern /* Subroutine */ int getime_(real *), 
	    lupang_(integer *, integer *, real *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, integer *, real *, real *, real *, real *, real *, 
	    real *, char *, integer *, logical *, ftnlen), marksm_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *), snapit_(integer *), sflush_(), fixlxn_(
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, logical *),
	     filsmo_(integer *, integer *, real *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    real *, real *, char *, integer *, ftnlen), rplotl_(integer *, 
	    real *, real *, real *, integer *, real *, real *, real *, real *,
	     real *, real *, integer *, char *, integer *, ftnlen), colaps_(
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, integer *, integer *, integer *, integer *, real *
	    , integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, real *, real *, real *, real *, 
	    real *, real *, char *, integer *, integer *, integer *, integer *
	    , integer *, integer *, integer *, integer *, logical *, logical *
	    , integer *, logical *, logical *, ftnlen), pcross_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, integer *, integer *, integer *, integer *, real *
	    , integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, real *, real *, real *, real *, real *, real *, char *,
	     integer *, integer *, integer *, integer *, integer *, integer *,
	     integer *, integer *, logical *, logical *, integer *, logical *,
	     logical *, ftnlen), connod_(integer *, integer *, real *, real *,
	     integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, logical *, logical *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE ADDROW = ADDS A ROW OF ELEMENTS BETWEEN TWO CORNERS */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --zn;
    --yn;
    --xn;
    --nuid;
    --lcorn;
    --nextn1;
    --nloop;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    linkpr -= 4;
    linkeg -= 3;
    --bmesur;
    --ynold;
    --xnold;
    --listeg;
    nxkold_dim1 = *nnxk;
    nxkold_offset = 1 + nxkold_dim1;
    nxkold -= nxkold_offset;

    /* Function Body */
    timing_ timing_1;
    nlold = 0;
    idum1 = 0;
    idum2 = 0;
    getime_(&time1);
    *err = FALSE_;
    ambig = FALSE_;
    *nnnold = min(*nnn,*nnnold);
    *nnn2 = *nnnold;
/*  IN THE LNODES ARRAY, */
/*  THE CORNER STATUS IS STORED IN LNODES (1, N1): */
/*      0 = NOT DECIDED */
/*      1 = ROW END */
/*      3 = ROW SIDE */
/*      5 = ROW CORNER */
/*      7 = ROW REVERSAL */
/*  THE PRECEDING NODE IN LNODES (2, N1), */
/*  THE NEXT NODE IN LNODES (3, N1), */
/*  THE INTERIOR/EXTERIOR STATUS OF NODE IS IN LNODES (4, N1). */
/*      1 = EXTERIOR OR ON THE BOUNDARY OF THE MESH */
/*          (NEGATED FOR SMOOTHING) */
/*      2 = INTERIOR TO THE MESH (NEGATED FOR SMOOTHING) */
/*  THE NEXT COUNTERCLOCKWISE LINE IS STORED IN LNODES (5, N1). */
/*  THE ANGLE STATUS OF LNODES IS STORED IN (6, N1), */
/*      1 = ROW END ONLY */
/*      2 = ROW END OR SIDE */
/*      3 = ROW SIDE ONLY */
/*      4 = ROW SIDE OR ROW CORNER */
/*      5 = ROW CORNER ONLY */
/*      6 = ROW CORNER OR REVERSAL */
/*      7 = ROW REVERSAL ONLY */
/*  THE NUMBER OF NODES TO THE NEXT CORNER IS STORED IN (7, N1). */
/*  THE DEPTH OF THE ROW OF THIS NODE IS STORED IN (8, N1) */
/*  START AT THE FIRST BEGINNING (CORNER) NODE */
/*  AND GO TO THE END (CORNER) NODE */
    n1 = *nbegin;
    n0 = lnodes[n1 * lnodes_dim1 + 2];
    *nadj1 = n0;
    n2 = lnodes[n1 * lnodes_dim1 + 3];
    n3 = lnodes[n2 * lnodes_dim1 + 3];
/*  SET UP FOR SMOOTHING AROUND THE BEGINNING AND ENDING OF THE ROW */
    lnodes[*nbegin * lnodes_dim1 + 4] = -(i__1 = lnodes[*nbegin * lnodes_dim1 
	    + 4], abs(i__1));
    lnodes[*nend * lnodes_dim1 + 4] = -(i__1 = lnodes[*nend * lnodes_dim1 + 4]
	    , abs(i__1));
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], nbegin, err);
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], nend, err);
/*  FIRST CHECK TO SEE IF THE ROW IS CIRCULAR - BEGINS AND ENDS AT N1 */
    if (*nend == *nbegin) {
/*  A CIRCLUAR ROW THAT ENDS AT A ROW END (TEAR DROP SHAPE) NEEDS STARTED */
	if (lnodes[n1 * lnodes_dim1 + 1] == 1) {
	    *nend = n0;
	    *nadj1 = *nnn + 1;
/*  A CIRCLUAR ROW THAT HAS NO END, CORNER, OR REVERSAL (CIRCLE SHAPE) */
/*  NEEDS STARTED */
	} else if (lnodes[n1 * lnodes_dim1 + 1] == 3) {
	    extnd1_(mxnd, &xn[1], &yn[1], &angle[1], &n0, &n1, &n2, x, y, &
		    dist1);
	    extnd1_(mxnd, &xn[1], &yn[1], &angle[1], &n1, &n2, &n3, &x[1], &y[
		    1], &dist2);
	    add2nd_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &
		    lxn[5], &bnsize[3], &lnodes[lnodes_offset], x, y, &x[1], &
		    y[1], &dist1, &dist2, nnn, lll, kkk, &n1, &n2, &nloop[1], 
		    sizeit, err, noroom, &xnold[1], &ynold[1], &nxkold[
		    nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], mlink,
		     npnold, npeold, nnxk, remesh, rexmin, rexmax, reymin, 
		    reymax, idivis, sizmin, emax, emin);
	    if (*err || *noroom) {
		goto L120;
	    }
	    n0 = *nnn;
	    *nadj1 = *nnn - 1;
	    if (*graph || *video) {
		i__1 = *nnn - 1;
		d2node_(mxnd, &xn[1], &yn[1], &n1, &i__1);
		i__1 = *nnn - 1;
		d2node_(mxnd, &xn[1], &yn[1], &i__1, nnn);
		d2node_(mxnd, &xn[1], &yn[1], nnn, &n2);
		sflush_();
		if (*video) {
		    snapit_(&c__1);
		}
	    }
	    n1 = n2;
/*  A CIRCLUAR ROW THAT ENDS AT A ROW CORNER NEEDS STARTED */
	} else if (lnodes[n1 * lnodes_dim1 + 1] == 5) {
	    ambig = TRUE_;
	    lambig = lnodes[n1 * lnodes_dim1 + 5];
	    extnd3_(mxnd, &xn[1], &yn[1], &angle[n1], &n0, &n1, &n2, x, y, &
		    dist);
	    add3nd_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &
		    lxn[5], &bnsize[3], &lnodes[lnodes_offset], x, y, &dist, 
		    nnn, lll, kkk, &n1, &nloop[1], sizeit, err, noroom, &
		    xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], &
		    listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
		    remesh, rexmin, rexmax, reymin, reymax, idivis, sizmin, 
		    emax, emin);
	    if (*err || *noroom) {
		goto L120;
	    }
	    *nadj1 = *nnn - 2;
	    lnodes[n1 * lnodes_dim1 + 1] = 1;
	    if (*graph || *video) {
		i__1 = *nnn - 2;
		d2node_(mxnd, &xn[1], &yn[1], &n1, &i__1);
		i__1 = *nnn - 2;
		i__2 = *nnn - 1;
		d2node_(mxnd, &xn[1], &yn[1], &i__1, &i__2);
		i__1 = *nnn - 1;
		d2node_(mxnd, &xn[1], &yn[1], &i__1, nnn);
		d2node_(mxnd, &xn[1], &yn[1], nnn, &n1);
		sflush_();
		if (*video) {
		    snapit_(&c__1);
		}
	    }
	    n0 = *nnn;
	    n3 = lnodes[n2 * lnodes_dim1 + 2];
	    goto L110;
/*  A CIRCLUAR ROW THAT ENDS AT A ROW REVERSAL NEEDS STARTED */
	} else if (lnodes[n1 * lnodes_dim1 + 1] == 7) {
	    extnd5_(mxnd, &xn[1], &yn[1], &angle[n2], &n1, &n2, &n3, x, y, &
		    dist);
	    add3nd_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &
		    lxn[5], &bnsize[3], &lnodes[lnodes_offset], x, y, &dist, 
		    nnn, lll, kkk, &n1, &nloop[1], sizeit, err, noroom, &
		    xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], &
		    listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
		    remesh, rexmin, rexmax, reymin, reymax, idivis, sizmin, 
		    emax, emin);
	    if (*err || *noroom) {
		goto L120;
	    }
	    if (*graph || *video) {
		i__1 = *nnn - 2;
		d2node_(mxnd, &xn[1], &yn[1], &n1, &i__1);
		i__1 = *nnn - 2;
		i__2 = *nnn - 1;
		d2node_(mxnd, &xn[1], &yn[1], &i__1, &i__2);
		i__1 = *nnn - 1;
		d2node_(mxnd, &xn[1], &yn[1], &i__1, nnn);
		d2node_(mxnd, &xn[1], &yn[1], nnn, &n1);
		sflush_();
		if (*video) {
		    snapit_(&c__1);
		}
	    }
	    n0 = *nnn;
	    add2nd_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &
		    lxn[5], &bnsize[3], &lnodes[lnodes_offset], &x[3], &y[3], 
		    &x[4], &y[4], &dist, &dist, nnn, lll, kkk, nnn, &n1, &
		    nloop[1], sizeit, err, noroom, &xnold[1], &ynold[1], &
		    nxkold[nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1],
		     mlink, npnold, npeold, nnxk, remesh, rexmin, rexmax, 
		    reymin, reymax, idivis, sizmin, emax, emin);
	    if (*err || *noroom) {
		goto L120;
	    }
	    lnodes[n1 * lnodes_dim1 + 1] = 1;
	    if (*graph || *video) {
		i__1 = *nnn - 1;
		d2node_(mxnd, &xn[1], &yn[1], &n0, &i__1);
		i__1 = *nnn - 1;
		d2node_(mxnd, &xn[1], &yn[1], &i__1, nnn);
		d2node_(mxnd, &xn[1], &yn[1], nnn, &n1);
		sflush_();
		if (*video) {
		    snapit_(&c__1);
		}
	    }
	} else {
	    mesage_("PROBLEMS IN ADDROW - THE CIRCLUAR ROW", (ftnlen)37);
	    mesage_("HAS NO END POINT CLASSIFICATION", (ftnlen)31);
	    *err = TRUE_;
	    goto L120;
	}
    }
    kount = 0;
L100:
    ++kount;
    n0 = lnodes[n1 * lnodes_dim1 + 2];
    n2 = lnodes[n1 * lnodes_dim1 + 3];
    n3 = lnodes[n2 * lnodes_dim1 + 3];
L110:
/*  NOW ADD THE NEXT ELEMENT(S) TO THE ROW */
    if (n2 != *nend) {
/*  IF N2 IS A ROW SIDE NODE, THEN PROJECT AT THE MIDLINE ANGLE */
	if (lnodes[n2 * lnodes_dim1 + 1] == 3) {
	    extnd1_(mxnd, &xn[1], &yn[1], &angle[1], &n1, &n2, &n3, x, y, &
		    dist);
	    addnod_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &
		    lxn[5], &angle[1], &bnsize[3], &lnodes[lnodes_offset], x, 
		    y, &dist, nnn, kkk, lll, &n0, &n1, &n2, &ambig, &lambig, 
		    sizeit, err, noroom, &xnold[1], &ynold[1], &nxkold[
		    nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], mlink,
		     npnold, npeold, nnxk, remesh, rexmin, rexmax, reymin, 
		    reymax, idivis, sizmin, emax, emin);
	    if (*err || *noroom) {
		goto L120;
	    }
	    ambig = FALSE_;
	    if (*graph || *video) {
		d2node_(mxnd, &xn[1], &yn[1], &n0, nnn);
		d2node_(mxnd, &xn[1], &yn[1], &n2, nnn);
		sflush_();
		if (*video) {
		    snapit_(&c__1);
		}
	    }
	    n1 = n2;
	    goto L100;
/*  IF N2 IS A ROW CORNER NODE, */
/*  THEN PROJECT TWO NODES (1 & 3) AT AVERAGE ANGLES AND ANOTHER (2) */
/*  AS AN ISOPARAMETRIC TYPE */
	} else if (lnodes[n2 * lnodes_dim1 + 1] == 5) {
	    ahold = angle[n2];
	    if (*nadj1 == n1) {
		*nadj1 = n0;
	    }
	    extnd3_(mxnd, &xn[1], &yn[1], &ahold, &n1, &n2, &n3, x, y, &dist);
	    addnod_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &
		    lxn[5], &angle[1], &bnsize[3], &lnodes[lnodes_offset], x, 
		    y, &dist, nnn, kkk, lll, &n0, &n1, &n2, &ambig, &lambig, 
		    sizeit, err, noroom, &xnold[1], &ynold[1], &nxkold[
		    nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], mlink,
		     npnold, npeold, nnxk, remesh, rexmin, rexmax, reymin, 
		    reymax, idivis, sizmin, emax, emin);
	    if (*err || *noroom) {
		goto L120;
	    }
	    ambig = FALSE_;
	    new1 = *nnn;
	    if (*graph || *video) {
		d2node_(mxnd, &xn[1], &yn[1], &n0, &new1);
		d2node_(mxnd, &xn[1], &yn[1], &n2, &new1);
		sflush_();
		if (*video) {
		    snapit_(&c__1);
		}
	    }
/*  TRY A PINCH TO MAKE SURE THAT CONTINUING DOES NOT OVERLAY */
/*  AN ALREADY GOOD CLOSURE */
	    fixlxn_(mxnd, &lxn[5], &nxl[3], &nuid[1], navail, iavail, nnn, 
		    lll, nnnold, lllold, err, noroom);
	    if (*err || *noroom) {
		goto L120;
	    }
	    *lllold = *lll;
	    *nnnold = *nnn;
	    getime_(&time2);
	    timing_1.timea = timing_1.timea + time2 - time1;
	    filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[
		    3], &lxn[5], lll, nnn, nnn2, &lnodes[lnodes_offset], &
		    bnsize[3], &nloop[1], xmin, xmax, ymin, ymax, zmin, zmax, 
		    dev1, kreg, (ftnlen)3);
	    getime_(&time1);
	    if (*graph || *video) {
		rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, 
			ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
		if (*video) {
		    snapit_(&c__2);
		}
	    }
	    lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[
		    3], &lxn[5], &nloop[1], &angle[1], &lnodes[lnodes_offset],
		     nadj1, lll, xmin, xmax, ymin, ymax, zmin, zmax, dev1, 
		    kreg, err, (ftnlen)3);
	    if (*err) {
		goto L120;
	    }
	    getime_(&time2);
	    timing_1.timea = timing_1.timea + time2 - time1;
	    pinch_(mxnd, mxcorn, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[
		    5], &kxl[3], &nxl[3], &lxn[5], &angle[1], &lnodes[
		    lnodes_offset], &bnsize[3], nadj1, &nloop[1], kkkold, 
		    lllold, nnnold, iavail, navail, done, xmin, xmax, ymin, 
		    ymax, zmin, zmax, dev1, lll, kkk, nnn, &lcorn[1], &ncorn, 
		    &idum1, &idum2, graph, video, kreg, noroom, err, (ftnlen)
		    3);
	    if (*noroom || *err) {
		goto L120;
	    }
	    if (*done) {
		goto L120;
	    }
	    getime_(&time1);
/*  TRY A COLAPS TO MAKE SURE THAT CONTINUING DOES NOT COMPLICATE */
/*  AN ALREADY OVERLAPPED SIDE */
	    if (nloop[1] > 6) {
		nlold = nloop[1];
		getime_(&time2);
		timing_1.timea = timing_1.timea + time2 - time1;
		colaps_(mxnd, mxcorn, mln, mxloop, &nuid[1], &xn[1], &yn[1], &
			zn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &angle[1], 
			&lnodes[lnodes_offset], &bnsize[3], nadj1, kkkold, 
			lllold, nnnold, iavail, navail, done, xmin, xmax, 
			ymin, ymax, zmin, zmax, dev1, lll, kkk, nnn, &lcorn[1]
			, &ncorn, &nloop[1], &nextn1[1], kloop, graph, video, 
			kreg, noroom, err, (ftnlen)3);
		if (*noroom || *err) {
		    goto L120;
		}
		if (*done) {
		    goto L120;
		}
		getime_(&time1);
	    }
/*  CHECK TO SEE IF ANY OF THE CONCURRENT PERIMETERS OVERLAP */
	    if (linkpr[*kperim * 3 + 2] != 0) {
		linkpr[*kperim * 3 + 3] = nloop[1];
		getime_(&time2);
		timing_1.timea = timing_1.timea + time2 - time1;
		idum = *nadj1;
		pcross_(mxnd, mxcorn, mln, mxloop, maxprm, &nuid[1], &xn[1], &
			yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
			angle[1], &lnodes[lnodes_offset], &bnsize[3], &linkpr[
			4], kperim, &idum, nadj1, &new1, kkkold, lllold, 
			nnnold, iavail, navail, done, xmin, xmax, ymin, ymax, 
			zmin, zmax, dev1, lll, kkk, nnn, &lcorn[1], &ncorn, &
			nloop[1], &nextn1[1], kloop, graph, video, kreg, 
			noroom, err, (ftnlen)3);
		if (*noroom || *err) {
		    goto L120;
		}
		getime_(&time1);
	    }
/*  THE ROW CAN BE CONTINUED IF THE NEW NODE AND THE ENDING NODE */
/*  IS STILL ON THE BOUNDARY AFTER THE PINCH, COLAPS AND PCROSS */
	    if (nlold == nloop[1] && (i__1 = lnodes[new1 * lnodes_dim1 + 4], 
		    abs(i__1)) == 1 && (i__2 = lnodes[*nend * lnodes_dim1 + 4]
		    , abs(i__2)) == 1 && lnodes[n2 * lnodes_dim1 + 2] == new1 
		    && lxn[(new1 << 2) + 1] > 0 && angle[n2] > (float)
		    2.3561945) {
		extnd3_(mxnd, &xn[1], &yn[1], &ahold, &n1, &n2, &n3, x, y, &
			dist);
		add2nd_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], 
			&lxn[5], &bnsize[3], &lnodes[lnodes_offset], &x[1], &
			y[1], &x[2], &y[2], &dist, &dist, nnn, lll, kkk, &
			new1, &n2, &nloop[1], sizeit, err, noroom, &xnold[1], 
			&ynold[1], &nxkold[nxkold_offset], &linkeg[3], &
			listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
			remesh, rexmin, rexmax, reymin, reymax, idivis, 
			sizmin, emax, emin);
		if (*err || *noroom) {
		    goto L120;
		}
		new2 = *nnn - 1;
		new3 = *nnn;
		if (*graph || *video) {
		    d2node_(mxnd, &xn[1], &yn[1], &new1, &new2);
		    d2node_(mxnd, &xn[1], &yn[1], &new2, &new3);
		    d2node_(mxnd, &xn[1], &yn[1], &n2, &new3);
		    sflush_();
		    if (*video) {
			snapit_(&c__1);
		    }
		}
		n1 = n2;
		goto L100;
	    } else {
		*nbegin = *nadj1;
		*nadj1 = 0;
		*nadj2 = 0;
		goto L120;
	    }
/*  IF N2 IS A ROW REVERSAL NODE, */
/*  THEN PROJECT THREE NODES (1, 3, & 5) AT AVERAGE ANGLES AND */
/*  TWO OTHERS (2 & 4) AS AN ISOPARAMETRIC TYPE */
	} else if (lnodes[n2 * lnodes_dim1 + 1] == 7) {
	    ahold = angle[n2];
	    extnd5_(mxnd, &xn[1], &yn[1], &ahold, &n1, &n2, &n3, x, y, &dist);
	    addnod_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &
		    lxn[5], &angle[1], &bnsize[3], &lnodes[lnodes_offset], x, 
		    y, &dist, nnn, kkk, lll, &n0, &n1, &n2, &ambig, &lambig, 
		    sizeit, err, noroom, &xnold[1], &ynold[1], &nxkold[
		    nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], mlink,
		     npnold, npeold, nnxk, remesh, rexmin, rexmax, reymin, 
		    reymax, idivis, sizmin, emax, emin);
	    if (*err || *noroom) {
		goto L120;
	    }
	    ambig = FALSE_;
	    new1 = *nnn;
	    if (*graph || *video) {
		d2node_(mxnd, &xn[1], &yn[1], &n0, &new1);
		d2node_(mxnd, &xn[1], &yn[1], &n2, &new1);
		sflush_();
		if (*video) {
		    snapit_(&c__1);
		}
	    }
/*  TRY A PINCH TO MAKE SURE THAT CONTINUING DOES NOT OVERLAY */
/*  AN ALREADY GOOD CLOSURE */
	    fixlxn_(mxnd, &lxn[5], &nxl[3], &nuid[1], navail, iavail, nnn, 
		    lll, nnnold, lllold, err, noroom);
	    if (*noroom || *err) {
		goto L120;
	    }
	    *lllold = *lll;
	    *nnnold = *nnn;
	    lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[
		    3], &lxn[5], &nloop[1], &angle[1], &lnodes[lnodes_offset],
		     nadj1, lll, xmin, xmax, ymin, ymax, zmin, zmax, dev1, 
		    kreg, err, (ftnlen)3);
	    if (*err) {
		goto L120;
	    }
	    getime_(&time2);
	    timing_1.timea = timing_1.timea + time2 - time1;
	    filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[
		    3], &lxn[5], lll, nnn, nnn2, &lnodes[lnodes_offset], &
		    bnsize[3], &nloop[1], xmin, xmax, ymin, ymax, zmin, zmax, 
		    dev1, kreg, (ftnlen)3);
	    getime_(&time1);
	    if (*graph || *video) {
		rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, 
			ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
		if (*video) {
		    snapit_(&c__2);
		}
	    }
	    getime_(&time2);
	    timing_1.timea = timing_1.timea + time2 - time1;
	    pinch_(mxnd, mxcorn, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[
		    5], &kxl[3], &nxl[3], &lxn[5], &angle[1], &lnodes[
		    lnodes_offset], &bnsize[3], nadj1, &nloop[1], kkkold, 
		    lllold, nnnold, iavail, navail, done, xmin, xmax, ymin, 
		    ymax, zmin, zmax, dev1, lll, kkk, nnn, &lcorn[1], &ncorn, 
		    &idum1, &idum2, graph, video, kreg, noroom, err, (ftnlen)
		    3);
	    if (*noroom || *err) {
		goto L120;
	    }
	    if (*done) {
		goto L120;
	    }
	    getime_(&time1);
/*  TRY A COLAPS TO MAKE SURE THAT CONTINUING DOES NOT COMPLICATE */
/*  AN ALREADY OVERLAPPED SIDE */
	    if (nloop[1] > 6) {
		nlold = nloop[1];
		getime_(&time2);
		timing_1.timea = timing_1.timea + time2 - time1;
		colaps_(mxnd, mxcorn, mln, mxloop, &nuid[1], &xn[1], &yn[1], &
			zn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &angle[1], 
			&lnodes[lnodes_offset], &bnsize[3], nadj1, kkkold, 
			lllold, nnnold, iavail, navail, done, xmin, xmax, 
			ymin, ymax, zmin, zmax, dev1, lll, kkk, nnn, &lcorn[1]
			, &ncorn, &nloop[1], &nextn1[1], kloop, graph, video, 
			kreg, noroom, err, (ftnlen)3);
		if (*noroom || *err) {
		    goto L120;
		}
		if (*done) {
		    goto L120;
		}
		getime_(&time1);
	    }
/*  CHECK TO SEE IF ANY OF THE CONCURRENT PERIMETERS OVERLAP */
	    if (linkpr[*kperim * 3 + 2] != 0) {
		linkpr[*kperim * 3 + 3] = nloop[1];
		getime_(&time2);
		timing_1.timea = timing_1.timea + time2 - time1;
		idum = *nadj1;
		pcross_(mxnd, mxcorn, mln, mxloop, maxprm, &nuid[1], &xn[1], &
			yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
			angle[1], &lnodes[lnodes_offset], &bnsize[3], &linkpr[
			4], kperim, &idum, nadj1, &new1, kkkold, lllold, 
			nnnold, iavail, navail, done, xmin, xmax, ymin, ymax, 
			zmin, zmax, dev1, lll, kkk, nnn, &lcorn[1], &ncorn, &
			nloop[1], &nextn1[1], kloop, graph, video, kreg, 
			noroom, err, (ftnlen)3);
		if (*noroom || *err) {
		    goto L120;
		}
		getime_(&time1);
	    }
/*  THE ROW CAN BE CONTINUED IF THE NEW NODE AND THE ENDING NODE */
/*  IS STILL ON THE BOUNDARY AFTER THE PINCH, COLAPS AND PCROSS */
/*  AND IF THE ANGLE AT THE NODE IS SOMEWHAT AS IT SHOULD BE */
	    if (nlold == nloop[1] && (i__1 = lnodes[new1 * lnodes_dim1 + 4], 
		    abs(i__1)) == 1 && lnodes[n2 * lnodes_dim1 + 2] == new1 &&
		     (i__2 = lnodes[*nend * lnodes_dim1 + 4], abs(i__2)) == 1 
		    && lxn[(new1 << 2) + 1] > 0 && angle[n2] > (float)
		    3.9269908) {
		extnd5_(mxnd, &xn[1], &yn[1], &ahold, &n1, &n2, &n3, x, y, &
			dist);
		add2nd_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], 
			&lxn[5], &bnsize[3], &lnodes[lnodes_offset], &x[1], &
			y[1], &x[2], &y[2], &dist, &dist, nnn, lll, kkk, &
			new1, &n2, &nloop[1], sizeit, err, noroom, &xnold[1], 
			&ynold[1], &nxkold[nxkold_offset], &linkeg[3], &
			listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
			remesh, rexmin, rexmax, reymin, reymax, idivis, 
			sizmin, emax, emin);
		if (*err || *noroom) {
		    goto L120;
		}
		new2 = *nnn - 1;
		new3 = *nnn;
		if (*graph || *video) {
		    d2node_(mxnd, &xn[1], &yn[1], &new1, &new2);
		    d2node_(mxnd, &xn[1], &yn[1], &new2, &new3);
		    d2node_(mxnd, &xn[1], &yn[1], &n2, &new3);
		    sflush_();
		    if (*video) {
			snapit_(&c__1);
		    }
		}
	    } else {
		*nbegin = *nadj1;
		*nadj1 = 0;
		*nadj2 = 0;
		goto L120;
	    }
/*  TRY A PINCH TO MAKE SURE THAT CONTINUING DOES NOT OVERLAY */
/*  AN ALREADY GOOD CLOSURE */
	    fixlxn_(mxnd, &lxn[5], &nxl[3], &nuid[1], navail, iavail, nnn, 
		    lll, nnnold, lllold, err, noroom);
	    if (*noroom || *err) {
		goto L120;
	    }
	    *lllold = *lll;
	    *nnnold = *nnn;
	    lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[
		    3], &lxn[5], &nloop[1], &angle[1], &lnodes[lnodes_offset],
		     nadj1, lll, xmin, xmax, ymin, ymax, zmin, zmax, dev1, 
		    kreg, err, (ftnlen)3);
	    if (*err) {
		goto L120;
	    }
	    getime_(&time2);
	    timing_1.timea = timing_1.timea + time2 - time1;
	    filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[
		    3], &lxn[5], lll, nnn, nnn2, &lnodes[lnodes_offset], &
		    bnsize[3], &nloop[1], xmin, xmax, ymin, ymax, zmin, zmax, 
		    dev1, kreg, (ftnlen)3);
	    getime_(&time1);
	    if (*graph || *video) {
		rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, 
			ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
		if (*video) {
		    snapit_(&c__2);
		}
	    }
	    getime_(&time2);
	    timing_1.timea = timing_1.timea + time2 - time1;
	    pinch_(mxnd, mxcorn, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[
		    5], &kxl[3], &nxl[3], &lxn[5], &angle[1], &lnodes[
		    lnodes_offset], &bnsize[3], nadj1, &nloop[1], kkkold, 
		    lllold, nnnold, iavail, navail, done, xmin, xmax, ymin, 
		    ymax, zmin, zmax, dev1, lll, kkk, nnn, &lcorn[1], &ncorn, 
		    &idum1, &idum2, graph, video, kreg, noroom, err, (ftnlen)
		    3);
	    if (*noroom || *err) {
		goto L120;
	    }
	    if (*done) {
		goto L120;
	    }
	    getime_(&time1);
/*  TRY A COLAPS TO MAKE SURE THAT CONTINUING DOES NOT COMPLICATE */
/*  AN ALREADY OVERLAPPED SIDE */
	    if (nloop[1] > 6) {
		nlold = nloop[1];
		getime_(&time2);
		timing_1.timea = timing_1.timea + time2 - time1;
		colaps_(mxnd, mxcorn, mln, mxloop, &nuid[1], &xn[1], &yn[1], &
			zn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &angle[1], 
			&lnodes[lnodes_offset], &bnsize[3], nadj1, kkkold, 
			lllold, nnnold, iavail, navail, done, xmin, xmax, 
			ymin, ymax, zmin, zmax, dev1, lll, kkk, nnn, &lcorn[1]
			, &ncorn, &nloop[1], &nextn1[1], kloop, graph, video, 
			kreg, noroom, err, (ftnlen)3);
		if (*noroom || *err) {
		    goto L120;
		}
		if (*done) {
		    goto L120;
		}
		getime_(&time1);
	    }
/*  CHECK TO SEE IF ANY OF THE CONCURRENT PERIMETERS OVERLAP */
	    if (linkpr[*kperim * 3 + 2] != 0) {
		linkpr[*kperim * 3 + 3] = nloop[1];
		getime_(&time2);
		timing_1.timea = timing_1.timea + time2 - time1;
		idum = *nadj1;
		pcross_(mxnd, mxcorn, mln, mxloop, maxprm, &nuid[1], &xn[1], &
			yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
			angle[1], &lnodes[lnodes_offset], &bnsize[3], &linkpr[
			4], kperim, &idum, nadj1, &new3, kkkold, lllold, 
			nnnold, iavail, navail, done, xmin, xmax, ymin, ymax, 
			zmin, zmax, dev1, lll, kkk, nnn, &lcorn[1], &ncorn, &
			nloop[1], &nextn1[1], kloop, graph, video, kreg, 
			noroom, err, (ftnlen)3);
		if (*noroom || *err) {
		    goto L120;
		}
		getime_(&time1);
	    }
/*  THE ROW CAN BE CONTINUED IF THE NEW NODE AND THE ENDING NODE */
/*  IS STILL ON THE BOUNDARY AFTER THE PINCH, COLAPS AND PCROSS */
	    if (nlold == nloop[1] && (i__1 = lnodes[new3 * lnodes_dim1 + 4], 
		    abs(i__1)) == 1 && lnodes[n2 * lnodes_dim1 + 2] == new3 &&
		     (i__2 = lnodes[*nend * lnodes_dim1 + 4], abs(i__2)) == 1 
		    && lxn[(new3 << 2) + 1] > 0 && angle[n2] > (float)
		    2.3561945) {
		extnd5_(mxnd, &xn[1], &yn[1], &ahold, &n1, &n2, &n3, x, y, &
			dist);
		add2nd_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], 
			&lxn[5], &bnsize[3], &lnodes[lnodes_offset], &x[3], &
			y[3], &x[4], &y[4], &dist, &dist, nnn, lll, kkk, &
			new3, &n2, &nloop[1], sizeit, err, noroom, &xnold[1], 
			&ynold[1], &nxkold[nxkold_offset], &linkeg[3], &
			listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
			remesh, rexmin, rexmax, reymin, reymax, idivis, 
			sizmin, emax, emin);
		if (*err || *noroom) {
		    goto L120;
		}
		new4 = *nnn - 1;
		if (*graph || *video) {
		    d2node_(mxnd, &xn[1], &yn[1], &new3, &new4);
		    d2node_(mxnd, &xn[1], &yn[1], &new4, nnn);
		    d2node_(mxnd, &xn[1], &yn[1], &n2, nnn);
		    sflush_();
		    if (*video) {
			snapit_(&c__1);
		    }
		}
	    } else {
		*nbegin = *nadj1;
		*nadj1 = 0;
		*nadj2 = 0;
		goto L120;
	    }
	    n1 = n2;
	    goto L100;
	} else {
	    mesage_("PROBLEMS IN ADDROW - THE NEXT NODE IN THE", (ftnlen)41);
	    mesage_("ROW DOES NOT HAVE THE RIGHT CLASSIFICATION", (ftnlen)42);
	    *err = TRUE_;
	    goto L120;
	}
	if (kount > nloop[1]) {
	    mesage_("PROBLEMS IN ADDROW - THE ROW DOESN'T STOP", (ftnlen)41);
	    *err = TRUE_;
	    goto L120;
	}
/*  CLOSE OFF THE END OF THE ROW - THE END NODE IS THE CONNECTION */
    } else {
	*nadj2 = lnodes[n2 * lnodes_dim1 + 3];
	connod_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3]
		, &lxn[5], &angle[1], &lnodes[lnodes_offset], nnn, kkk, lll, 
		nnnold, lllold, &n0, &n1, &n2, &lnodes[n2 * lnodes_dim1 + 3], 
		&n1, &nloop[1], iavail, navail, graph, video, noroom, err);
	if (*noroom || *err) {
	    goto L120;
	}
/*  NOW SMOOTH THE CURRENT ROW INTO THE MESH */
	getime_(&time2);
	timing_1.timea = timing_1.timea + time2 - time1;
	filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], 
		&lxn[5], lll, nnn, nnn2, &lnodes[lnodes_offset], &bnsize[3], &
		nloop[1], xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, (
		ftnlen)3);
	getime_(&time1);
/*  CALCULATE NEW ANGLES */
	lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], 
		&lxn[5], &nloop[1], &angle[1], &lnodes[lnodes_offset], &n3, 
		lll, xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, err, (
		ftnlen)3);
	if (*err) {
	    goto L120;
	}
/*  PLOT THE NEW MESH */
	if (*video) {
	    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, 
		    ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
	    snapit_(&c__2);
	}
    }
    *nbegin = n3;
L120:
    getime_(&time2);
    timing_1.timea = timing_1.timea + time2 - time1;
    return 0;
} /* addrow_ */

#ifdef __cplusplus
	}
#endif

/* add1cn.f -- translated by f2c (version 20160102).
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
#include "paving.h"

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int add1cn_(integer *mxnd, integer *mln, xc_float *xn, xc_float *yn, 
	integer *nuid, integer *lxk, integer *kxl, integer *nxl, integer *lxn,
	 xc_float *angle, xc_float *bnsize, integer *lnodes, integer *nnn, integer *
	kkk, integer *lll, integer *nnnold, integer *lllold, integer *nloop, 
	xc_float *xnew, xc_float *ynew, integer *i1, integer *iavail, integer *navail,
	 logical *graph, logical *video, logical *sizeit, logical *noroom, 
	logical *err, xc_float *xnold, xc_float *ynold, integer *nxkold, integer *
	linkeg, integer *listeg, xc_float *bmesur, integer *mlink, integer *
	npnold, integer *npeold, integer *nnxk, xc_float *rexmin, 
	xc_float *rexmax, xc_float *reymin, xc_float *reymax, integer *idivis, xc_float *
	sizmin, xc_float *emax, xc_float *emin)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, nxkold_dim1, nxkold_offset;

    /* Local variables */
    static integer i2, i3, i4, i5, i6, i7, idum;
    static xc_float zero;
    static logical ambig;
    extern /* Subroutine */ int d2node_(integer *, xc_float *, xc_float *, integer *, 
	    integer *), close4_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
				integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE ADD1CN = ADDS ONE CENTER NODE IN THE MIDDLE OF 6 CLOSING */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    linkeg -= 3;
    --bmesur;
    --ynold;
    --xnold;
    --listeg;
    nxkold_dim1 = *nnxk;
    nxkold_offset = 1 + nxkold_dim1;
    nxkold -= nxkold_offset;

    /* Function Body */
    ambig = FALSE_;
    *err = FALSE_;
    i2 = lnodes[*i1 * lnodes_dim1 + 3];
    i3 = lnodes[i2 * lnodes_dim1 + 3];
    i4 = lnodes[i3 * lnodes_dim1 + 3];
    i5 = lnodes[i4 * lnodes_dim1 + 3];
    i6 = lnodes[i5 * lnodes_dim1 + 3];
    zero = (xc_float)0.;
    addnod_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
	    angle[1], &bnsize[3], &lnodes[lnodes_offset], xnew, ynew, &zero, 
	    nnn, kkk, lll, &i6, i1, &i2, &ambig, &idum, sizeit, err, noroom, &
	    xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], &listeg[
	    1], &bmesur[1], mlink, npnold, npeold, nnxk, rexmin, 
	    rexmax, reymin, reymax, idivis, sizmin, emax, emin);
    if (*err || *noroom) {
	goto L100;
    }
    i7 = *nnn;
    lnodes[i7 * lnodes_dim1 + 4] = -2;
    if (*graph || *video) {
	d2node_(mxnd, &xn[1], &yn[1], &i6, &i7);
	d2node_(mxnd, &xn[1], &yn[1], &i2, &i7);
	sflush_();
	if (*video) {
	    snapit_(&c__1);
	}
    }
    connod_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &
	    lxn[5], &angle[1], &lnodes[lnodes_offset], nnn, kkk, lll, nnnold, 
	    lllold, &i7, &i2, &i3, &i4, &idum, nloop, iavail, navail, graph, 
	    video, noroom, err);
    if (*noroom || *err) {
	goto L100;
    }
    close4_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], &i7, &i4, &i5, &i6, kkk, err);
L100:
    return 0;
} /* add1cn_ */

#ifdef __cplusplus
	}
#endif

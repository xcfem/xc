/* addnod.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int addnod_(integer *mxnd, integer *mln, real *xn, real *yn, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, real *angle, 
	real *bnsize, integer *lnodes, real *xnew, real *ynew, real *dist, 
	integer *nnn, integer *kkk, integer *lll, integer *n0, integer *n1, 
	integer *n2, logical *ambig, integer *lambig, logical *sizeit, 
	logical *err, logical *noroom, real *xnold, real *ynold, integer *
	nxkold, integer *linkeg, integer *listeg, real *bmesur, integer *
	mlink, integer *npnold, integer *npeold, integer *nnxk, real *remesh, 
	real *rexmin, real *rexmax, real *reymin, real *reymax, integer *
	idivis, real *sizmin, real *emax, real *emin)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, nxkold_dim1, nxkold_offset, i__1;

    /* Local variables */
    static integer i__;
    static real size1, size2;
    extern /* Subroutine */ int addkxl_(integer *, integer *, integer *, 
	    integer *), marksm_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *), getsiz_(
	    real *, real *, integer *, integer *, integer *, real *, integer *
	    , integer *, integer *, integer *, real *, real *, real *, real *,
	     real *, integer *, real *, real *, real *, real *, real *, real *
	    );
    static real siznew;

/* *********************************************************************** */
/*  SUBROUTINE ADDNOD = ADDS A NEW ELEMENT TO A NEW NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
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
    ++(*nnn);
    if (*nnn > *mxnd) {
	*noroom = TRUE_;
	goto L110;
    }
    xn[*nnn] = *xnew;
    yn[*nnn] = *ynew;
/*  PUT THE BEGINNING BOUNDARY DISTANCE IN PLACE */
    if (lxn[(*n2 << 2) + 2] < 0) {
	bnsize[(*nnn << 1) + 1] = *dist;
	bnsize[(*nnn << 1) + 2] = (float)1.;
    } else {
	if (*sizeit) {
/* **               LOCATION SIZE AND PROJECTING FROM LOCATION SIZE. */
	    getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], 
		    &listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
		    remesh, rexmin, rexmax, reymin, reymax, idivis, sizmin, 
		    emax, emin, xnew, ynew, &size1);
	    getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], 
		    &listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
		    remesh, rexmin, rexmax, reymin, reymax, idivis, sizmin, 
		    emax, emin, &xn[*n2], &yn[*n2], &size2);
	    siznew = dmin(size1,size2);
	} else {
	    siznew = bnsize[(*n2 << 1) + 1];
	}
	bnsize[(*nnn << 1) + 1] = siznew;
	if (bnsize[(*n2 << 1) + 1] == (float)0. || *sizeit) {
	    bnsize[(*nnn << 1) + 2] = (float)1.;
	} else {
	    bnsize[(*nnn << 1) + 2] = *dist / siznew;
	}
    }
/*  MAKE LXN, NXL, KXL, AND LXK ARRAYS */
/*  FIRST, ADD THE NEW NODE'S LINES */
    ++(*lll);
    nxl[(*lll << 1) + 1] = *nnn;
    nxl[(*lll << 1) + 2] = *n0;
    ++(*lll);
    nxl[(*lll << 1) + 1] = *nnn;
    nxl[(*lll << 1) + 2] = *n2;
    for (i__ = 1; i__ <= 4; ++i__) {
	lxn[i__ + (*nnn << 2)] = 0;
/* L100: */
    }
/*  MAKE THE NEW ELEMENT */
    ++(*kkk);
    lxk[(*kkk << 2) + 1] = lnodes[*n0 * lnodes_dim1 + 5];
    if (*ambig) {
	lxk[(*kkk << 2) + 2] = *lambig;
    } else {
	lxk[(*kkk << 2) + 2] = lnodes[*n1 * lnodes_dim1 + 5];
    }
    lxk[(*kkk << 2) + 3] = *lll;
    lxk[(*kkk << 2) + 4] = *lll - 1;
    addkxl_(mxnd, &kxl[3], kkk, lll);
    i__1 = *lll - 1;
    addkxl_(mxnd, &kxl[3], kkk, &i__1);
    addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n0 * lnodes_dim1 + 5]);
    if (*ambig) {
	addkxl_(mxnd, &kxl[3], kkk, lambig);
    } else {
	addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n1 * lnodes_dim1 + 5]);
    }
/*  REDO THE LNODES ARRAY */
    lnodes[*n2 * lnodes_dim1 + 2] = *nnn;
    lnodes[*n0 * lnodes_dim1 + 3] = *nnn;
    lnodes[*n0 * lnodes_dim1 + 1] = 0;
    lnodes[*nnn * lnodes_dim1 + 1] = 0;
    lnodes[*n2 * lnodes_dim1 + 1] = 0;
    lnodes[*nnn * lnodes_dim1 + 2] = *n0;
    lnodes[*nnn * lnodes_dim1 + 3] = *n2;
    lnodes[*nnn * lnodes_dim1 + 5] = *lll;
    lnodes[*n0 * lnodes_dim1 + 5] = *lll - 1;
    lnodes[*nnn * lnodes_dim1 + 4] = -1;
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n1, err);
    if (*err) {
	goto L110;
    }
    if (! (*ambig)) {
	lnodes[*n1 * lnodes_dim1 + 4] = -2;
    }
    lnodes[*nnn * lnodes_dim1 + 8] = lnodes[*n2 * lnodes_dim1 + 8] + 1;
L110:
    return 0;
} /* addnod_ */

#ifdef __cplusplus
	}
#endif

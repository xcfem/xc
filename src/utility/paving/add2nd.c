/* add2nd.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int add2nd_(integer *mxnd, integer *mln, real *xn, real *yn, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, real *bnsize, 
	integer *lnodes, real *x1, real *y1, real *x2, real *y2, real *dist1, 
	real *dist2, integer *nnn, integer *lll, integer *kkk, integer *n1, 
	integer *n2, integer *nloop, logical *sizeit, logical *err, logical *
	noroom, real *xnold, real *ynold, integer *nxkold, integer *linkeg, 
	integer *listeg, real *bmesur, integer *mlink, integer *npnold, 
	integer *npeold, integer *nnxk, real *rexmin, real *
	rexmax, real *reymin, real *reymax, integer *idivis, real *sizmin, 
	real *emax, real *emin)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, nxkold_dim1, nxkold_offset;

    /* Local variables */
    static integer i__, l1, l2, l3, node1, node2;
    static real size1, size2;
    extern /* Subroutine */ int addkxl_(integer *, integer *, integer *, 
	    integer *), marksm_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *);
    static real siznew;

/* *********************************************************************** */
/*  SUBROUTINE ADD2ND = ADDS A NEW ELEMENT JUTTING OUT FROM AN EXISTING */
/*                      LINE */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
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
    xn[*nnn] = *x1;
    yn[*nnn] = *y1;
    node1 = *nnn;
    ++(*nnn);
    if (*nnn > *mxnd) {
	*noroom = TRUE_;
	goto L110;
    }
    xn[*nnn] = *x2;
    yn[*nnn] = *y2;
    node2 = *nnn;
/*  PUT THE BEGINNING BOUNDARY DISTANCE IN PLACE */
    if (lxn[(*n1 << 2) + 2] < 0) {
	bnsize[(node1 << 1) + 1] = *dist1;
	bnsize[(node1 << 1) + 2] = (float)1.;
    } else {
	if (*sizeit) {
/* **               LOCATION SIZE AND PROJECTING FROM LOCATION SIZE. */
	    getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], 
		    &listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
		    rexmin, rexmax, reymin, reymax, idivis, sizmin, 
		    emax, emin, x1, y1, &size1);
	    getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], 
		    &listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
		    rexmin, rexmax, reymin, reymax, idivis, sizmin, 
		    emax, emin, &xn[*n1], &yn[*n1], &size2);
	    siznew = dbl_min(size1,size2);
	} else {
	    siznew = bnsize[(*n1 << 1) + 1];
	}
	bnsize[(node1 << 1) + 1] = siznew;
	if (bnsize[(*n1 << 1) + 1] == (float)0. || *sizeit) {
	    bnsize[(node1 << 1) + 2] = (float)1.;
	} else {
	    bnsize[(node1 << 1) + 2] = *dist1 / siznew;
	}
    }
    if (lxn[(*n2 << 2) + 2] < 0) {
	bnsize[(node2 << 1) + 1] = *dist2;
	bnsize[(node2 << 1) + 2] = (float)1.;
    } else {
	if (*sizeit) {
	    getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], 
		    &listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
		    rexmin, rexmax, reymin, reymax, idivis, sizmin, 
		    emax, emin, x2, y2, &siznew);
	} else {
	    siznew = bnsize[(*n2 << 1) + 1];
	}
	bnsize[(node2 << 1) + 1] = siznew;
	if (bnsize[(*n2 << 1) + 1] == (float)0. || *sizeit) {
	    bnsize[(node2 << 1) + 2] = (float)1.;
	} else {
	    bnsize[(node2 << 1) + 2] = *dist2 / siznew;
	}
    }
/*  MAKE NXL ARRAY */
/*  ADD THE THREE NEW LINES */
    ++(*lll);
    l1 = *lll;
    nxl[(l1 << 1) + 1] = *n1;
    nxl[(l1 << 1) + 2] = node1;
    ++(*lll);
    l2 = *lll;
    nxl[(l2 << 1) + 1] = node1;
    nxl[(l2 << 1) + 2] = node2;
    ++(*lll);
    l3 = *lll;
    nxl[(l3 << 1) + 1] = node2;
    nxl[(l3 << 1) + 2] = *n2;
/*  MAKE THE NEW ELEMENT */
    ++(*kkk);
    lxk[(*kkk << 2) + 1] = lnodes[*n1 * lnodes_dim1 + 5];
    lxk[(*kkk << 2) + 2] = l3;
    lxk[(*kkk << 2) + 3] = l2;
    lxk[(*kkk << 2) + 4] = l1;
    addkxl_(mxnd, &kxl[3], kkk, &l1);
    addkxl_(mxnd, &kxl[3], kkk, &l2);
    addkxl_(mxnd, &kxl[3], kkk, &l3);
    addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n1 * lnodes_dim1 + 5]);
/*  ZERO OUT THE LXN ARRAY */
    for (i__ = 1; i__ <= 4; ++i__) {
	lxn[i__ + (node1 << 2)] = 0;
	lxn[i__ + (node2 << 2)] = 0;
/* L100: */
    }
/*  REDO THE LNODES ARRAY */
    lnodes[node1 * lnodes_dim1 + 1] = 0;
    lnodes[node2 * lnodes_dim1 + 1] = 0;
    lnodes[*n1 * lnodes_dim1 + 1] = 0;
    lnodes[*n2 * lnodes_dim1 + 1] = 0;
    lnodes[node1 * lnodes_dim1 + 2] = *n1;
    lnodes[node2 * lnodes_dim1 + 2] = node1;
    lnodes[*n2 * lnodes_dim1 + 2] = node2;
    lnodes[*n1 * lnodes_dim1 + 3] = node1;
    lnodes[node1 * lnodes_dim1 + 3] = node2;
    lnodes[node2 * lnodes_dim1 + 3] = *n2;
    lnodes[node1 * lnodes_dim1 + 4] = -1;
    lnodes[node2 * lnodes_dim1 + 4] = -1;
    lnodes[*n1 * lnodes_dim1 + 5] = l1;
    lnodes[node1 * lnodes_dim1 + 5] = l2;
    lnodes[node2 * lnodes_dim1 + 5] = l3;
    lnodes[node1 * lnodes_dim1 + 8] = lnodes[*n1 * lnodes_dim1 + 8] + 1;
    lnodes[node2 * lnodes_dim1 + 8] = lnodes[*n2 * lnodes_dim1 + 8] + 1;
    *nloop += 2;
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n1, err);
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n2, err);
L110:
    return 0;
} /* add2nd_ */

#ifdef __cplusplus
	}
#endif

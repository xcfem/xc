/* invert.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int invert_fq__(integer *mxnd, integer *mln, real *xn, real *
	yn, real *zn, integer *lxk, integer *kxl, integer *nxl, integer *lxn, 
	integer *lll, integer *lnodes, real *xmin, real *xmax, real *ymin, 
	real *ymax, real *zmin, real *zmax, char *dev1, integer *kreg, 
	integer *node, real *xdel, real *ydel, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static integer n0, n1, n2, n3, n4;
    static logical err;
    static real xold, yold, xnew, ynew, ang1a, ang2a, ang3a, ang2b, ang1b, 
	    ang3b;
    extern /* Subroutine */ int getang_(integer *, integer *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, logical *), vinter_(integer *, real 
	    *, real *, integer *, integer *, integer *, real *, real *, real *
	    , real *, logical *);
    static logical vcross;

/* *********************************************************************** */
/*  SUBROUTINE INVERT = CHECKS FOR AN INVERSION OR CROSSING OF A BOUNDARY */
/*                      UPON ITSELF AND CORRECTS IT WHERE NECESSARY */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --zn;
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    err = FALSE_;
    xold = xn[*node];
    yold = yn[*node];
    n2 = *node;
    n3 = lnodes[n2 * lnodes_dim1 + 3];
    n4 = lnodes[n3 * lnodes_dim1 + 3];
    n1 = lnodes[n2 * lnodes_dim1 + 2];
    n0 = lnodes[n1 * lnodes_dim1 + 2];
/*  GET THE ANGLES BEFORE MOVEMENT */
    if (lxn[(n1 << 2) + 4] == 0) {
	getang_(mxnd, mln, &xn[1], &yn[1], &lnodes[lnodes_offset], &lxk[5], &
		kxl[3], &nxl[3], &lxn[5], &n0, &n1, &n2, &ang1a, &err);
    }
    if (lxn[(n2 << 2) + 4] == 0) {
	getang_(mxnd, mln, &xn[1], &yn[1], &lnodes[lnodes_offset], &lxk[5], &
		kxl[3], &nxl[3], &lxn[5], &n1, &n2, &n3, &ang2a, &err);
    }
    if (lxn[(n3 << 2) + 4] == 0) {
	getang_(mxnd, mln, &xn[1], &yn[1], &lnodes[lnodes_offset], &lxk[5], &
		kxl[3], &nxl[3], &lxn[5], &n2, &n3, &n4, &ang3a, &err);
    }
/*  NOW PLACE THE NODE TEMPORARILY AT THE NEW PROPOSED LOCATION */
    xn[*node] += *xdel;
    yn[*node] += *ydel;
/*  GET THE ANGLE BEING ADJUSTED AT THE NODE ITSELF */
    if (lxn[(n2 << 2) + 4] == 0 && ang2a > (float)0.) {
	getang_(mxnd, mln, &xn[1], &yn[1], &lnodes[lnodes_offset], &lxk[5], &
		kxl[3], &nxl[3], &lxn[5], &n1, &n2, &n3, &ang2b, &err);
/*  ADJUST THE NODE LOCATION IF NECESSARY */
	if (ang2b < (float)0.) {
	    vinter_(mxnd, &xn[1], &yn[1], &n1, &n3, &n2, &xold, &yold, &xnew, 
		    &ynew, &vcross);
	    if (vcross) {
		xn[*node] = xnew;
		yn[*node] = ynew;
	    }
	}
    }
/*  GET THE ANGLE BEING ADJUSTED ON THE CCW SIDE OF THIS NODE */
    if (lxn[(n1 << 2) + 4] == 0 && ang1a > (float)0.) {
	getang_(mxnd, mln, &xn[1], &yn[1], &lnodes[lnodes_offset], &lxk[5], &
		kxl[3], &nxl[3], &lxn[5], &n0, &n1, &n2, &ang1b, &err);
/*  ADJUST THE NODE LOCATION IF NECESSARY */
	if (ang1b < (float)0.) {
	    vinter_(mxnd, &xn[1], &yn[1], &n1, &n0, &n2, &xold, &yold, &xnew, 
		    &ynew, &vcross);
	    if (vcross) {
		xn[*node] = xnew;
		yn[*node] = ynew;
	    }
	}
    }
/*  GET THE ANGLE BEING ADJUSTED ON THE CW SIDE OF THIS NODE */
    if (lxn[(n3 << 2) + 4] == 0 && ang3a > (float)0.) {
	getang_(mxnd, mln, &xn[1], &yn[1], &lnodes[lnodes_offset], &lxk[5], &
		kxl[3], &nxl[3], &lxn[5], &n2, &n3, &n4, &ang3b, &err);
/*  ADJUST THE NODE LOCATION IF NECESSARY */
	if (ang3b < (float)0.) {
	    vinter_(mxnd, &xn[1], &yn[1], &n3, &n4, &n2, &xold, &yold, &xnew, 
		    &ynew, &vcross);
	    if (vcross) {
		xn[*node] = xnew;
		yn[*node] = ynew;
	    }
	}
    }
/*  RESTORE THE OLD LOCATION AND THE XDEL AND YDEL TO THE CORRECTED */
/*  VALUES */
    *xdel = xn[*node] - xold;
    *ydel = yn[*node] - yold;
    xn[*node] = xold;
    yn[*node] = yold;
    return 0;
} /* invert_fq__ */

#ifdef __cplusplus
	}
#endif

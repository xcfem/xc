/* add2el.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int add2el_(integer *mxnd, integer *mln, real *xn, real *yn, 
	integer *nuid, integer *lxk, integer *kxl, integer *nxl, integer *lxn,
	 real *angle, integer *lnodes, integer *nnn, integer *kkk, integer *
	lll, integer *nnnold, integer *lllold, integer *nloop, integer *i1, 
	integer *i4, integer *iavail, integer *navail, logical *graph, 
	logical *video, logical *noroom, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static integer i2, i3, i5, i6, idum;
    extern /* Subroutine */ int close4_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *), connod_(integer *, 
	    integer *, real *, real *, integer *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *, logical *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE ADD2EL = CLOSES A SIX SIDED REGION BY FORMING 2 ELEMENTS */
/* *********************************************************************** */
    /* Parameter adjustments */
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

    /* Function Body */
    *err = FALSE_;
    i2 = lnodes[*i1 * lnodes_dim1 + 3];
    i3 = lnodes[i2 * lnodes_dim1 + 3];
    i5 = lnodes[*i4 * lnodes_dim1 + 3];
    i6 = lnodes[i5 * lnodes_dim1 + 3];
    connod_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &
	    lxn[5], &angle[1], &lnodes[lnodes_offset], nnn, kkk, lll, nnnold, 
	    lllold, i1, &i2, &i3, i4, &idum, nloop, iavail, navail, graph, 
	    video, noroom, err);
    if (*noroom || *err) {
	goto L100;
    }
    close4_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], i1, i4, &i5, &i6, kkk, err);
L100:
    return 0;
} /* add2el_ */

#ifdef __cplusplus
	}
#endif

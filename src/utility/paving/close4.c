/* close4.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int close4_(integer *mxnd, integer *mln, integer *lxk, 
	integer *kxl, integer *nxl, integer *lxn, integer *lnodes, integer *
	n0, integer *n1, integer *n2, integer *n3, integer *kkk, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    extern /* Subroutine */ int addkxl_(integer *, integer *, integer *, 
	    integer *), marksm_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE CLOSE4 = CLOSES THE AREA AROUND A FOUR NODE ELEMENT */
/* *********************************************************************** */
/*  SET ALL THE LOOP NODES TO BE INTERIOR */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    lnodes[*n0 * lnodes_dim1 + 4] = -2;
    lnodes[*n1 * lnodes_dim1 + 4] = -2;
    lnodes[*n2 * lnodes_dim1 + 4] = -2;
    lnodes[*n3 * lnodes_dim1 + 4] = -2;
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n0, err);
    if (*err) {
	goto L100;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n1, err);
    if (*err) {
	goto L100;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n2, err);
    if (*err) {
	goto L100;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n3, err);
    if (*err) {
	goto L100;
    }
/*  MAKE THE LXK AND THE KXL ARRAY */
    ++(*kkk);
    lxk[(*kkk << 2) + 1] = lnodes[*n0 * lnodes_dim1 + 5];
    lxk[(*kkk << 2) + 2] = lnodes[*n1 * lnodes_dim1 + 5];
    lxk[(*kkk << 2) + 3] = lnodes[*n2 * lnodes_dim1 + 5];
    lxk[(*kkk << 2) + 4] = lnodes[*n3 * lnodes_dim1 + 5];
    addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n0 * lnodes_dim1 + 5]);
    addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n1 * lnodes_dim1 + 5]);
    addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n2 * lnodes_dim1 + 5]);
    addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n3 * lnodes_dim1 + 5]);
L100:
    return 0;
} /* close4_ */

#ifdef __cplusplus
	}
#endif

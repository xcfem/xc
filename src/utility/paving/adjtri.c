/* adjtri.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int adjtri_(integer *mxnd, integer *mln, integer *lnodes, 
	real *xn, real *yn, real *zn, integer *nuid, integer *lxk, integer *
	kxl, integer *nxl, integer *lxn, integer *nnn, integer *navail, 
	integer *iavail, integer *node, integer *kelem, real *ang, real *
	toler1, real *toler2, integer *n1, integer *n2, integer *n3, integer *
	kreg, real *xmin, real *xmax, real *ymin, real *ymax, real *zmin, 
	real *zmax, integer *kkk, integer *lll, char *dev1, logical *done, 
	logical *check, logical *graph, logical *video, logical *noroom, 
	logical *err, integer *kkkadd, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static integer node1, node3;
    extern /* Subroutine */ int delem_(integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *, logical *, logical *), d2node_(integer *, real *, real 
	    *, integer *, integer *), longel_(integer *, integer *, integer *,
	     real *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, integer *, integer *, integer *, real *, real *, 
	    real *, real *, integer *, integer *, logical *, logical *, 
	    logical *, logical *, logical *, integer *), lcolor_(char *, 
	    ftnlen), sflush_(), rplotl_(integer *, real *, real *, real *, 
	    integer *, real *, real *, real *, real *, real *, real *, 
	    integer *, char *, integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE ADJTRI = ADJUSTS A TRIANGULAR SHAPED ELEMENT WHERE */
/*                      POSSIBLE */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY TRIDEL */
/* *********************************************************************** */
/*  THERE ARE THREE POSSIBILITIES FOR CHANGE: */
/*     1) ANYTHING OVER TOLER1 GETS THE CORRESPONDING ELEMENT */
/*        DELETED */
/*     2) ANYTHING OVER TOLER2 AND HOOKED TO ANOTHER 3-LINE NODE GETS */
/*        THE CORRESPONDING ELEMENT DELETED */
/*     3) AN ELONGATED ELEMENT OVER 150 DEGREES GETS A 3 ELEMENT */
/*        REPLACEMENT FOR THE TWO ELEMENTS THERE */
/* *********************************************************************** */
/*  CHECK FOR CASE 1 */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --zn;
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    if (*ang > *toler1) {
	if (*graph) {
	    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, 
		    ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
	    lcolor_("PINK ", (ftnlen)5);
	    d2node_(mxnd, &xn[1], &yn[1], node, n1);
	    d2node_(mxnd, &xn[1], &yn[1], node, n2);
	    lcolor_("WHITE", (ftnlen)5);
	    sflush_();
	}
L100:
	delem_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &
		lxn[5], nnn, navail, iavail, node, kelem, &node1, &node3, 
		done, check, noroom, err);
	if (*noroom || *err) {
	    goto L120;
	}
	if (lxn[(node1 << 2) + 3] <= 0) {
	    *node = node1;
	    *kelem = kxl[(lxn[(*node << 2) + 1] << 1) + 1];
	    *check = FALSE_;
	    goto L100;
	} else if (lxn[(node3 << 2) + 3] <= 0) {
	    *node = node3;
	    *kelem = kxl[(lxn[(*node << 2) + 1] << 1) + 1];
	    *check = FALSE_;
	    goto L100;
	}
	*check = TRUE_;
	if (*err || *done) {
	    goto L120;
	}
    }
/*  CHECK FOR CASE 2 */
    if (*ang > *toler2 && lxn[(*n3 << 2) + 4] == 0 && lxn[(*n3 << 2) + 3] > 0 
	    && lxn[(*n3 << 2) + 2] > 0) {
	if (*graph) {
	    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, 
		    ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
	    lcolor_("PINK ", (ftnlen)5);
	    d2node_(mxnd, &xn[1], &yn[1], node, n1);
	    d2node_(mxnd, &xn[1], &yn[1], node, n2);
	    lcolor_("WHITE", (ftnlen)5);
	    sflush_();
	}
L110:
	delem_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &
		lxn[5], nnn, navail, iavail, node, kelem, &node1, &node3, 
		done, check, noroom, err);
	if (*noroom || *err) {
	    goto L120;
	}
	if (lxn[(node1 << 2) + 3] <= 0) {
	    *node = node1;
	    *kelem = kxl[(lxn[(*node << 2) + 1] << 1) + 1];
	    *check = FALSE_;
	    goto L110;
	} else if (lxn[(node3 << 2) + 3] <= 0) {
	    *node = node3;
	    *kelem = kxl[(lxn[(*node << 2) + 1] << 1) + 1];
	    *check = FALSE_;
	    goto L110;
	}
	*check = TRUE_;
	if (*err || *done) {
	    goto L120;
	}
    }
/*  CHECK FOR CASE 3 */
    longel_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &nuid[1], &lxk[
	    5], &kxl[3], &nxl[3], &lxn[5], nnn, navail, iavail, node, kelem, 
	    ang, toler2, n1, n2, kreg, xmin, xmax, ymin, ymax, kkk, lll, done,
	     graph, video, noroom, err, kkkadd);
L120:
    return 0;
} /* adjtri_ */

#ifdef __cplusplus
	}
#endif

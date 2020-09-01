/* undelm.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__3 = 3;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int undelm_(integer *mxnd, integer *mln, integer *lnodes, 
	real *xn, real *yn, integer *nuid, integer *lxk, integer *kxl, 
	integer *nxl, integer *lxn, integer *nnn, integer *lll, integer *kkk, 
	integer *navail, integer *iavail, integer *n0, integer *n1, integer *
	n2, integer *n3, integer *l1, integer *l2, integer *l3, integer *k1, 
	integer *k2, logical *noroom, logical *err, logical *graph, logical *
	video)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static integer i__, l4, l5, nnew;
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
	    integer *), mesage_(char *, ftnlen), addlxn_(integer *, integer *,
	     integer *, integer *, integer *, integer *, integer *, integer *,
	     logical *, logical *), dellxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *), lcolor_(char *, ftnlen), marksm_(integer *, integer *,
	     integer *, integer *, integer *, integer *, integer *, integer *,
	     logical *), snapit_(integer *), sflush_(), fixlxn_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE UNDELM = UNDELETES AN ELEMENT BY EXPANDING N1 INTO A */
/*                      NEW ELEMENT */
/* *********************************************************************** */
    /* Parameter adjustments */
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
/*  MAKE SURE THAT N2 HAS AT LEAST FOUR LINES ATTACHED TO IT */
    if (lxn[(*n2 << 2) + 4] == 0) {
	*err = TRUE_;
	mesage_("** N2 IN UNDELM CANNOT BE USED TO EXPAND AN ELEMENT **", (
		ftnlen)54);
	goto L140;
    }
/*  ERASE THE LINE L3 */
    if (*graph || *video) {
	lcolor_("BLACK", (ftnlen)5);
	d2node_(mxnd, &xn[1], &yn[1], n0, n2);
	if (*graph) {
	    lcolor_("WHITE", (ftnlen)5);
	} else {
	    lcolor_("YELOW", (ftnlen)5);
	}
	sflush_();
	if (*video) {
	    snapit_(&c__3);
	}
    }
/*  DEFINE THE NEW NODE AND THE TWO NEW LINES */
    ++(*nnn);
    if (*nnn > *mxnd) {
	*noroom = TRUE_;
	goto L140;
    }
    nnew = *nnn;
    xn[nnew] = (xn[*n0] + xn[*n2]) * (float).5;
    yn[nnew] = (yn[*n0] + yn[*n2]) * (float).5;
    *lll += 2;
    l4 = *lll - 1;
    l5 = *lll;
    nxl[(l4 << 1) + 1] = *n1;
    nxl[(l4 << 1) + 2] = nnew;
    nxl[(l5 << 1) + 1] = *n3;
    nxl[(l5 << 1) + 2] = nnew;
/*  NOW CHANGE LINE L3'S END POINT FROM N2 TO NNEW */
    if (nxl[(*l3 << 1) + 1] == *n2) {
	nxl[(*l3 << 1) + 1] = nnew;
    } else if (nxl[(*l3 << 1) + 2] == *n2) {
	nxl[(*l3 << 1) + 2] = nnew;
    } else {
	mesage_("** PROBLEMS IN UNDLEM WITH L3'S END POINT **", (ftnlen)44);
	*err = TRUE_;
	goto L140;
    }
/*  NOW UPDATE THE LXN ARRAYS */
    lxn[(nnew << 2) + 1] = *l3;
    lxn[(nnew << 2) + 2] = l4;
    lxn[(nnew << 2) + 3] = l5;
    lxn[(nnew << 2) + 4] = 0;
    fixlxn_(mxnd, &lxn[5], &nxl[3], &nuid[1], navail, iavail, nnn, lll, nnn, 
	    lll, err, noroom);
    if (*noroom || *err) {
	goto L140;
    }
/*  REMOVE L3 FROM THE LIST OF LINES FOR N2 */
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, n2, l3, nnn, err, noroom)
	    ;
    if (*noroom || *err) {
	mesage_("** PROBLEMS IN UNDELM UNHOOKING L3 FROM N2 **", (ftnlen)45);
	goto L140;
    }
/*  ADD LINE L4 TO N1 */
    addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, n1, &l4, nnn, err, 
	    noroom);
    if (*noroom || *err) {
	mesage_("** PROBLEMS IN UNDELM HOOKING L4 TO N1 **", (ftnlen)41);
	goto L140;
    }
/*  ADD LINE L5 TO N3 */
    addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, n3, &l5, nnn, err, 
	    noroom);
    if (*noroom || *err) {
	mesage_("** PROBLEMS IN UNDELM HOOKING L5 TO N3 **", (ftnlen)41);
	goto L140;
    }
/*  NOW ADD THE NEW ELEMENT */
    ++(*kkk);
    lxk[(*kkk << 2) + 1] = *l1;
    lxk[(*kkk << 2) + 2] = *l2;
    lxk[(*kkk << 2) + 3] = l5;
    lxk[(*kkk << 2) + 4] = l4;
/*  NOW FIX THE KXL ARRAY FOR LINE L1 */
    if (kxl[(*l1 << 1) + 1] == *k2) {
	kxl[(*l1 << 1) + 1] = *kkk;
    } else if (kxl[(*l1 << 1) + 2] == *k2) {
	kxl[(*l1 << 1) + 2] = *kkk;
    } else {
	mesage_("** PROBLEMS IN UNDELM REPLACING K2 FOR L1 **", (ftnlen)44);
	*err = TRUE_;
	goto L140;
    }
/*  NOW FIX THE KXL ARRAY FOR LINE L2 */
    if (kxl[(*l2 << 1) + 1] == *k1) {
	kxl[(*l2 << 1) + 1] = *kkk;
    } else if (kxl[(*l2 << 1) + 2] == *k1) {
	kxl[(*l2 << 1) + 2] = *kkk;
    } else {
	mesage_("** PROBLEMS IN UNDELM REPLACING K1 FOR L2 **", (ftnlen)44);
	*err = TRUE_;
	goto L140;
    }
/*  ADD THE KXL ENTRIES FOR THE NEW LINES */
    kxl[(l4 << 1) + 1] = *k2;
    kxl[(l4 << 1) + 2] = *kkk;
    kxl[(l5 << 1) + 1] = *k1;
    kxl[(l5 << 1) + 2] = *kkk;
/*  NOW FIX THE LXK ARRAY FOR THE ELEMENT K1 */
    for (i__ = 1; i__ <= 4; ++i__) {
	if (lxk[i__ + (*k1 << 2)] == *l2) {
	    lxk[i__ + (*k1 << 2)] = l5;
	    goto L110;
	}
/* L100: */
    }
    mesage_("** PROBLEMS IN UNDELM REPLACING L2 WITH L5 IN K1 **", (ftnlen)51)
	    ;
    *err = TRUE_;
    goto L140;
L110:
/*  NOW FIX THE LXK ARRAY FOR THE ELEMENT K2 */
    for (i__ = 1; i__ <= 4; ++i__) {
	if (lxk[i__ + (*k2 << 2)] == *l1) {
	    lxk[i__ + (*k2 << 2)] = l4;
	    goto L130;
	}
/* L120: */
    }
    mesage_("** PROBLEMS IN UNDELM REPLACING L1 WITH L4 IN K2 **", (ftnlen)51)
	    ;
    *err = TRUE_;
    goto L140;
L130:
/*  NOW REDRAW THE LINES */
    if (*graph || *video) {
	d2node_(mxnd, &xn[1], &yn[1], n0, &nnew);
	d2node_(mxnd, &xn[1], &yn[1], n1, &nnew);
	d2node_(mxnd, &xn[1], &yn[1], n3, &nnew);
	sflush_();
	if (*video) {
	    snapit_(&c__3);
	}
    }
    lnodes[nnew * lnodes_dim1 + 4] = 2;
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], &nnew, err);
    if (*err) {
	goto L140;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n0, err);
    if (*err) {
	goto L140;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n1, err);
    if (*err) {
	goto L140;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n2, err);
    if (*err) {
	goto L140;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], n3, err);
    if (*err) {
	goto L140;
    }
L140:
    return 0;
} /* undelm_ */

#ifdef __cplusplus
	}
#endif

/* addwdg.f -- translated by f2c (version 20160102).
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

static integer c__2 = 2;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int addwdg_(integer *mxnd, integer *mln, integer *nuid, xc_float 
	*xn, xc_float *yn, xc_float *zn, integer *lxk, integer *kxl, integer *nxl, 
	integer *lxn, integer *lnodes, xc_float *angle, xc_float *bnsize, integer *
	nloop, integer *iavail, integer *navail, integer *lll, integer *kkk, 
	integer *nnn, integer *lllold, integer *nnnold, xc_float *tang, integer *
	kang, integer *nstart, integer *nend, xc_float *xmin, xc_float *xmax, xc_float *
	ymin, xc_float *ymax, xc_float *zmin, xc_float *zmax, logical *graph, logical *
	video, char *dev1, integer *kreg, logical *noroom, logical *err, 
	ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;

    /* Local variables */
    static integer i__, idum;
    extern /* Subroutine */ int wedge_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, xc_float *, xc_float *, 
	    integer *, xc_float *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, logical *, logical *, logical *, logical *);
    static integer inode[4], nnode, nwant;
    static logical pwedge;
    extern /* Subroutine */ int marksm_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     snapit_(integer *), nsplit_(integer *, integer *, integer *, 
	    xc_float *, integer *, integer *, integer *, integer *, integer *, 
	    logical *);
    static logical maxsiz;
    extern /* Subroutine */ int rplotl_(integer *, xc_float *, xc_float *, xc_float *, 
	    integer *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, 
	    integer *, char *, integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE ADDWDG = ADDS WEDGES IN A ROW */
/* *********************************************************************** */
/*  ADD WEDGES BASED ON THE TOTAL TURNED ANGLE: */
/*      FOR TURNING ANGLES LESS THAN 135 DEGREES - 1 WEDGE */
/*      FOR TURNING ANGLES BETWEEN 135 AND 225 DEGREES - TRY 2 WEDGES */
/*      FOR TURNING ANGLES BETWEEN 225 AND 315 DEGREES - TRY 3 WEDGES */
/*      FOR TURNING ANGLES GREATER THAN 315 DEGREES - TRY 4 WEDGES */
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
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    maxsiz = TRUE_;
    *err = FALSE_;
    pwedge = FALSE_;
    if (*tang < (xc_float)2.3561945) {
	nwant = 1;
    } else if (*tang < (xc_float)3.9269908) {
	nwant = 2;
    } else if (*tang < (xc_float)5.4977871) {
	nwant = 3;
    } else {
	nwant = 4;
    }
    nsplit_(mxnd, mln, &lnodes[lnodes_offset], &angle[1], nstart, kang, inode,
	     &nnode, &nwant, &maxsiz);
    i__1 = nnode;
    for (i__ = 1; i__ <= i__1; ++i__) {
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[inode[i__ - 1] * lnodes_dim1 + 2], 
		err);
	if (*err) {
	    goto L110;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[lnodes[inode[i__ - 1] * lnodes_dim1 + 
		2] * lnodes_dim1 + 2], err);
	if (*err) {
	    goto L110;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[inode[i__ - 1] * lnodes_dim1 + 3], 
		err);
	if (*err) {
	    goto L110;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[lnodes[inode[i__ - 1] * lnodes_dim1 + 
		3] * lnodes_dim1 + 3], err);
	if (*err) {
	    goto L110;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &inode[i__ - 1], err);
	if (*err) {
	    goto L110;
	}
	wedge_(mxnd, mln, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &xn[1]
		, &yn[1], &lnodes[lnodes_offset], &bnsize[3], iavail, navail, 
		lll, kkk, nnn, lllold, nnnold, &inode[i__ - 1], &idum, nloop, 
		&pwedge, graph, video, noroom, err);
	if (*noroom || *err) {
	    goto L110;
	}
	if (*video) {
	    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, 
		    ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
	    snapit_(&c__2);
	}
/* L100: */
    }
L110:
    return 0;
} /* addwdg_ */

#ifdef __cplusplus
	}
#endif

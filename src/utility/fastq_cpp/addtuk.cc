/* addtuk.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int addtuk_(integer *mxnd, integer *mln, integer *nuid, real 
	*xn, real *yn, real *zn, integer *lxk, integer *kxl, integer *nxl, 
	integer *lxn, integer *lnodes, real *angle, integer *nloop, integer *
	iavail, integer *navail, integer *lll, integer *kkk, integer *nnn, 
	real *tang, integer *kang, integer *nstart, integer *nend, integer *
	node, real *xmin, real *xmax, real *ymin, real *ymax, real *zmin, 
	real *zmax, logical *graph, logical *video, char *dev1, logical *
	noroom, logical *err, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;

    /* Local variables */
    static integer i__, kreg;
    extern /* Subroutine */ int tuck_(integer *, integer *, integer *, real *,
	     real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *, logical *);
    static integer inode[4], nnode, nwant;
    extern /* Subroutine */ int marksm_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     snapit_(integer *), nsplit_(integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    logical *);
    static logical maxsiz;
    extern /* Subroutine */ int rplotl_(integer *, real *, real *, real *, 
	    integer *, real *, real *, real *, real *, real *, real *, 
	    integer *, char *, integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE ADDTUK = ADDS TUCKS IN A ROW */
/* *********************************************************************** */
/*  ADD TUCKS BASED ON THE TOTAL TURNED ANGLE: */
/*      FOR TURNING ANGLES LESS THAN 135 DEGREES - 1 TUCK */
/*      FOR TURNING ANGLES BETWEEN 135 AND 225 DEGREES - TRY 2 TUCKS */
/*      FOR TURNING ANGLES BETWEEN 225 AND 315 DEGREES - TRY 3 TUCKS */
/*      FOR TURNING ANGLES GREATER THAN 315 DEGREES - TRY 4 TUCKS */
/* *********************************************************************** */
    /* Parameter adjustments */
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
    *err = FALSE_;
    maxsiz = FALSE_;
    if (*tang < (float)2.3561945) {
	nwant = 1;
    } else if (*tang < (float)3.9269908) {
	if (*kang > 2) {
	    nwant = 2;
	} else {
	    nwant = 1;
	}
    } else if (*tang < (float)5.4977871) {
	if (*kang > 4) {
	    nwant = 3;
	} else if (*kang > 2) {
	    nwant = 2;
	} else {
	    nwant = 1;
	}
    } else {
	if (*kang > 6) {
	    nwant = 4;
	} else if (*kang > 4) {
	    nwant = 3;
	} else if (*kang > 2) {
	    nwant = 2;
	} else {
	    nwant = 1;
	}
    }
    nsplit_(mxnd, mln, &lnodes[lnodes_offset], &angle[1], nstart, kang, inode,
	     &nnode, &nwant, &maxsiz);
    i__1 = nnode;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (lxn[(i__ << 2) + 1] > 0) {
/*  MARK THE SMOOTHING */
	    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		    lnodes_offset], &lnodes[inode[i__ - 1] * lnodes_dim1 + 2],
		     err);
	    if (*err) {
		goto L110;
	    }
	    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		    lnodes_offset], &lnodes[lnodes[inode[i__ - 1] * 
		    lnodes_dim1 + 2] * lnodes_dim1 + 2], err);
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
		    lnodes_offset], &lnodes[lnodes[inode[i__ - 1] * 
		    lnodes_dim1 + 3] * lnodes_dim1 + 3], err);
	    if (*err) {
		goto L110;
	    }
	    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		    lnodes_offset], &lnodes[lnodes[lnodes[inode[i__ - 1] * 
		    lnodes_dim1 + 3] * lnodes_dim1 + 3] * lnodes_dim1 + 3], 
		    err);
	    if (*err) {
		goto L110;
	    }
/*  MAKE SURE THAT THE NSTART, NEND, AND NODE GET UPDATED IF THEY ARE TO */
/*  BE DELETED */
	    if (inode[i__ - 1] == *nstart || lnodes[inode[i__ - 1] * 
		    lnodes_dim1 + 3] == *nstart) {
		*nstart = lnodes[lnodes[inode[i__ - 1] * lnodes_dim1 + 3] * 
			lnodes_dim1 + 3];
	    }
	    if (inode[i__ - 1] == *nend || lnodes[inode[i__ - 1] * 
		    lnodes_dim1 + 3] == *nend) {
		*nend = lnodes[lnodes[inode[i__ - 1] * lnodes_dim1 + 3] * 
			lnodes_dim1 + 3];
	    }
	    if (inode[i__ - 1] == *node || lnodes[inode[i__ - 1] * 
		    lnodes_dim1 + 3] == *node) {
		*node = lnodes[lnodes[inode[i__ - 1] * lnodes_dim1 + 3] * 
			lnodes_dim1 + 3];
	    }
/*  TAKE THE TUCK */
	    tuck_(mxnd, mln, &nuid[1], &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[
		    3], &lxn[5], &lnodes[lnodes_offset], iavail, navail, lll, 
		    kkk, nnn, &inode[i__ - 1], nloop, graph, noroom, err);
	    if (*noroom || *err) {
		goto L110;
	    }
	    if (*video) {
		rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, 
			ymin, ymax, zmin, zmax, lll, dev1, &kreg, (ftnlen)3);
		snapit_(&c__1);
	    }
	}
/* L100: */
    }
L110:
    return 0;
} /* addtuk_ */

#ifdef __cplusplus
	}
#endif

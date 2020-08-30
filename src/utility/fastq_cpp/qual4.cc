/* qual4.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int qual4_(integer *mxnd, integer *mxcorn, integer *mln, 
	integer *ncorn, integer *lcorn, integer *lnodes, integer *icomb, real 
	*angle, integer *lxn, integer *itest, integer *ltest, real *qual, 
	logical *posbl4, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;

    /* Local variables */
    static integer i__, node, ilen;
    extern doublereal nickc_(real *, integer *), nicks_(real *, integer *);
    extern /* Subroutine */ int spaced_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROTINE QUAL4 = CHECKS THE QUALITY OF A RECTANGLE INTERPRETATION */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    --angle;
    --icomb;
    --lcorn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    --itest;
    --ltest;

    /* Function Body */
    *err = FALSE_;
/*  ASSUME PERFECT QUALITY */
    *qual = (float)0.;
    *posbl4 = FALSE_;
/*  FIRST GET THE INTERVAL LENGTHS TO THE CHOSEN CORNERS */
    ilen = 4;
    spaced_(mxnd, mxcorn, mln, &ilen, ncorn, &lcorn[1], &lnodes[lnodes_offset]
	    , &icomb[1], &itest[1], &ltest[1], err);
    if (*err) {
	goto L110;
    }
/*  GET SEE IF A RECTANGLE INTERPRETATION IS POSSIBLE WITH */
/*  THESE INTERVALS */
    if (ltest[1] == ltest[3] && ltest[2] == ltest[4]) {
	*posbl4 = TRUE_;
    } else {
	return 0;
    }
/*  NOT ADD UP THE NICKS FOR BAD ANGLES */
    i__1 = *ncorn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	node = lcorn[i__];
	if (icomb[i__] == 1) {
	    *qual += nickc_(&angle[node], &lxn[(node << 2) + 1]) * (float).8;
	} else {
	    *qual += nicks_(&angle[node], &lxn[(node << 2) + 1]) * (float).8;
	}
/* L100: */
    }
L110:
    return 0;
} /* qual4_ */

#ifdef __cplusplus
	}
#endif

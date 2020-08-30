/* qual2.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int qual2_(integer *mxnd, integer *mxcorn, integer *mln, 
	integer *ncorn, integer *lcorn, integer *lnodes, integer *icomb, real 
	*bnsize, real *angle, integer *lxn, integer *itest, integer *ltest, 
	real *qual, logical *posbl2, logical *posbl3, logical *rowchn, 
	integer *istart, integer *iend)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1, i__2;

    /* Local variables */
    static integer i__, i1, i2, l1, l2;
    static logical err;
    static integer ldif, node, ilen, nchg1, nchg2;
    extern doublereal nickc_(real *, integer *);
    static real qual13;
    extern doublereal nicks_(real *, integer *);
    static real qual23;
    static integer istep;
    extern /* Subroutine */ int spaced_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *);
    extern integer jumplp_(integer *, integer *, integer *, integer *, 
	    integer *);
    extern logical shrunk_(real *, integer *);

/* *********************************************************************** */
/*  SUBROTINE QUAL2 = CHECKS THE QUALITY OF A SEMICIRCLE INTERPRETATION */
/* *********************************************************************** */
/* ... See note below regarding bug... */
    /* Parameter adjustments */
    lxn -= 5;
    --angle;
    bnsize -= 3;
    --icomb;
    --lcorn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    --itest;
    --ltest;

    /* Function Body */
    istep = 0;
/*  ASSUME PERFECT QUALITY */
    *qual = (float)0.;
    *posbl2 = FALSE_;
    *posbl3 = FALSE_;
    *rowchn = FALSE_;
/*  FIRST GET THE INTERVAL LENGTHS TO THE CHOSEN CORNERS */
    ilen = 2;
    spaced_(mxnd, mxcorn, mln, &ilen, ncorn, &lcorn[1], &lnodes[lnodes_offset]
	    , &icomb[1], &itest[1], &ltest[1], &err);
    if (err) {
	return 0;
    }
/*  SEE IF A SEMICIRCLE INTERPRETATION IS POSSIBLE WITH */
/*  THESE INTERVALS */
    if (ltest[1] >= 2 && ltest[2] >= 2) {
	*posbl2 = TRUE_;
    }
/*  NOT ADD UP THE NICKS FOR BAD ANGLES */
    i__1 = *ncorn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	node = lcorn[i__];
	if (icomb[i__] == 1) {
	    *qual += nickc_(&angle[node], &lxn[(node << 2) + 1]);
	} else {
	    *qual += nicks_(&angle[node], &lxn[(node << 2) + 1]);
	}
/* L100: */
    }
/*  NOW SEE IF A TRIANGLE INTERPRETATION IS WARRANTED */
    if (ltest[1] > ltest[2]) {
	i1 = itest[1];
	l1 = ltest[1];
	i2 = itest[2];
	l2 = ltest[2];
    } else {
	i1 = itest[2];
	l1 = ltest[2];
	i2 = itest[1];
	l2 = ltest[1];
    }
    ldif = (l1 - l2) / 2;
    if (ldif > l1 / 2) {
	ldif = l1 - ldif;
    }
/*  THIS TESTS THE FORCED TRIANGLE - THE NEW ROW MUST BE */
/*  ENDED AT A CURRENT SIDE NODE */
    if (l1 == l2) {
	nchg1 = jumplp_(mxnd, mln, &lnodes[lnodes_offset], &i1, &ldif);
	nchg2 = jumplp_(mxnd, mln, &lnodes[lnodes_offset], &i2, &ldif);
	qual13 = *qual + nickc_(&angle[nchg1], &lxn[(nchg1 << 2) + 1]);
	qual23 = *qual + nickc_(&angle[nchg2], &lxn[(nchg2 << 2) + 1]);
	if (shrunk_(&bnsize[(nchg1 << 1) + 2], &lnodes[nchg1 * lnodes_dim1 + 
		8]) && shrunk_(&bnsize[(nchg2 << 1) + 2], &lnodes[nchg2 * 
		lnodes_dim1 + 8])) {
	    *posbl3 = TRUE_;
	    *posbl2 = FALSE_;
	    if (qual13 < qual23) {
		*iend = nchg1;
		*istart = i1;
	    } else {
		*iend = nchg2;
		*istart = i2;
	    }
	} else if (shrunk_(&bnsize[(nchg1 << 1) + 2], &lnodes[nchg1 * 
		lnodes_dim1 + 8])) {
	    *posbl3 = TRUE_;
	    *posbl2 = FALSE_;
	    *iend = nchg1;
	    *istart = i1;
	} else if (shrunk_(&bnsize[(nchg2 << 1) + 2], &lnodes[nchg2 * 
		lnodes_dim1 + 8])) {
	    *posbl3 = TRUE_;
	    *posbl2 = FALSE_;
	    *iend = nchg2;
	    *istart = i2;
	} else {
	    *posbl3 = FALSE_;
	}
/*  SEE IF THE ROW NEEDS ADJUSTED SO THAT A RECTANGLE REMAINS POSSIBLE */
/*  WITH A SIGNIFICANTLY REDUCED ELEMENT SIZE ON THE LONG SIDE */
    } else {
/* ... There is a bug here since ISTEP is not defined */
/*     Since it has been 'kindof' working for several years, */
/*     we assume that ISTEP=0 will work the first time through */
/*     and we add that initialization to the beginning of this routine */
	nchg1 = jumplp_(mxnd, mln, &lnodes[lnodes_offset], &i1, &istep);
	i__1 = l1 - istep;
	nchg2 = jumplp_(mxnd, mln, &lnodes[lnodes_offset], &i1, &i__1);
	if (shrunk_(&bnsize[(nchg1 << 1) + 2], &lnodes[nchg1 * lnodes_dim1 + 
		8]) && shrunk_(&bnsize[(nchg2 << 1) + 2], &lnodes[nchg2 * 
		lnodes_dim1 + 8])) {
	    *rowchn = TRUE_;
	    *istart = i2;
	    *iend = i1;
/*  CHECK THE SIZE REDUCTIONS AND TRIANGLE INTERPRETATION */
	} else {
	    i__1 = l1 / 2 - 1;
	    for (istep = ldif + 1; istep <= i__1; ++istep) {
		nchg1 = jumplp_(mxnd, mln, &lnodes[lnodes_offset], &i1, &
			istep);
		i__2 = l1 - istep;
		nchg2 = jumplp_(mxnd, mln, &lnodes[lnodes_offset], &i1, &i__2)
			;
		if (shrunk_(&bnsize[(nchg1 << 1) + 2], &lnodes[nchg1 * 
			lnodes_dim1 + 8]) && shrunk_(&bnsize[(nchg2 << 1) + 2]
			, &lnodes[nchg2 * lnodes_dim1 + 8])) {
		    *rowchn = TRUE_;
		    *istart = i2;
		    *iend = i1;
		    goto L120;
		}
/* L110: */
	    }
L120:
	    ;
	}
    }
    return 0;
} /* qual2_ */

#ifdef __cplusplus
	}
#endif

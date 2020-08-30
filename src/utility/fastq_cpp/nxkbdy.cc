/* nxkbdy.f -- translated by f2c (version 20160102).
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

static integer c_n1 = -1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int nxkbdy_(integer *mdim, integer *nnxk, integer *npelem, 
	integer *nxk, integer *kkk, integer *list, logical *three, logical *
	eight, logical *nine)
{
    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2, i__3;

    /* Builtin functions */
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer i_sign(integer *, integer *);

    /* Local variables */
    static integer i__, k, n, n2, nend;
    static real hold;
    static integer lval, node1, node2, lhash, lkeep;
    static logical itsok;
    extern /* Subroutine */ int mesage_(char *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE NXKBDY = FLAGS ALL SIDES OF ELEMENTS ONLY ONCE BY USE OF */
/*                      A HASH SCHEME */
/* *********************************************************************** */
/*     NXK    = THE NODES PER ELEMENT ARRAY  (CONNECTIVITY) */
/*               (A NEGATIVE VALUE WILL INDICATE UNIQUENESS OF FOLLOWING */
/*              SIDE) */
/* *********************************************************************** */
    /* Parameter adjustments */
    --list;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;

    /* Function Body */
    i__1 = *mdim;
    for (i__ = 1; i__ <= i__1; ++i__) {
	list[i__] = 0;
/* L100: */
    }
    i__1 = *kkk;
    for (k = 1; k <= i__1; ++k) {
	if (nxk[k * nxk_dim1 + 3] == 0 && *three) {
	    itsok = TRUE_;
	    nend = 1;
	} else if (nxk[k * nxk_dim1 + 3] != 0 && (*eight || *nine)) {
	    itsok = TRUE_;
	    nend = 4;
	} else {
	    itsok = FALSE_;
	}
	if (itsok) {
	    i__2 = nend;
	    for (n = 1; n <= i__2; ++n) {
/*  COMPUTE HASH CODE FOR LINE */
		n2 = n + 1;
		if (n >= 4) {
		    n2 = 1;
		}
		node1 = nxk[n + k * nxk_dim1];
		node2 = (i__3 = nxk[n2 + k * nxk_dim1], abs(i__3));
		if (node2 > node1) {
		    lval = node1 * 100000 + node2;
		} else {
		    lval = node2 * 100000 + node1;
		}
/*  CALCULATE THE BEGINNING HASH VALUE */
		hold = (doublereal) (node1 + node2) * (float)3.1830989;
		lhash = (integer) ((hold - (integer) hold) * (doublereal) (*
			mdim) + 1);
		lkeep = lhash;
/*  FIRST-TIMERS CLAIM THE NODE */
L110:
		if (list[lhash] != 0) {
		    if (list[lhash] == lval) {
			goto L120;
		    }
		    ++lhash;
		    if (lhash == *mdim) {
			lhash = 1;
		    }
		    if (lhash != lkeep) {
			goto L110;
		    }
		    mesage_("HASH SCHEME OVERFLOW IN KXNBDY", (ftnlen)30);
		    s_stop("", (ftnlen)0);
		}
		list[lhash] = lval;
		nxk[n + k * nxk_dim1] = i_sign(&nxk[n + k * nxk_dim1], &c_n1);
L120:
/* L130: */
		;
	    }
	}
/* L140: */
    }
    return 0;
} /* nxkbdy_ */

#ifdef __cplusplus
	}
#endif

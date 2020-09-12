/* cntcrn.f -- translated by f2c (version 20160102).
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
//#include "f2c.h"
#include "paving.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int cntcrn_(integer *mxnd, integer *mxcorn, integer *mln, 
	integer *lnodes, integer *lcorn, integer *ncorn, integer *nloop, 
	integer *n1, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static integer kkc, node, nold, lastc, kount;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static integer kountc;

/* *********************************************************************** */
/*  SUBROUTINE CNTCRN = COUNTS THE CURRENT DESIGNATED CORNER LENGTHS */
/* *********************************************************************** */
    /* Parameter adjustments */
    --lcorn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *err = FALSE_;
/*  COUNT THE CURRENT CORNERS STARTING AT THE I COUNTER */
    node = *n1;
    nold = *n1;
    kount = 0;
    *ncorn = 0;
    kkc = 0;
    kountc = 0;
    lastc = 0;
L100:
    ++kount;
    if (kount > *nloop) {
	mesage_("PROBLEM IN CNTCRN WITH UNCLOSED LOOP", (ftnlen)36);
	*err = TRUE_;
	goto L110;
    }
/*  A NEW CORNER NODE HAS BEEN FOUND */
    if (lnodes[node * lnodes_dim1 + 1] == 1) {
/*  ADD UP THE NUMBER OF NODES FROM THE LAST "NON-SIDE" */
	++(*ncorn);
	if (*ncorn <= *mxcorn) {
	    lcorn[*ncorn] = node;
	}
	if (*ncorn > 1) {
	    lnodes[lastc * lnodes_dim1 + 7] = kountc + 1;
	} else {
	    kkc = kountc + 1;
	}
	lastc = node;
	kountc = 0;
/*  THIS IS A SIDE - JUST CONTINUE */
    } else {
	++kountc;
    }
/*  CHECK FOR COMPLETION OF THE LOOP */
    node = lnodes[node * lnodes_dim1 + 3];
    if (node != nold) {
	goto L100;
    }
/*  GET THE FIRST CORNER'S DISTANCE FROM PREVIOUS CORNER CORRECT */
    lnodes[lastc * lnodes_dim1 + 7] = kkc + kountc;
L110:
    return 0;
} /* cntcrn_ */

#ifdef __cplusplus
	}
#endif

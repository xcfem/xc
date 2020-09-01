/* ch3to4.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int ch3to4_(integer *mxnd, integer *mxcorn, integer *mln, 
	integer *ncorn, integer *lcorn, integer *lnodes, integer *icomb, real 
	*angle, integer *itest, integer *ltest, real *qual, logical *posbl4, 
	integer *ichang)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;

    /* Local variables */
    static integer mmax;
    extern integer jumplp_(integer *, integer *, integer *, integer *, 
	    integer *);

/* *********************************************************************** */
/*  SUBROTINE CH3TO4 = CHECKS THE FEASIBILITY OF A */
/*                     RECTANGLE FROM A TRIANGLE */
/* *********************************************************************** */
/*  ASSUME PERFECT QUALITY */
/*      QUAL = 0. */
    /* Parameter adjustments */
    --angle;
    --icomb;
    --lcorn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    --itest;
    --ltest;

    /* Function Body */
    *posbl4 = TRUE_;
/*  FIND THE POSSIBLE RECTANGLE (THIS ALREADY ASSUMES THAT THE */
/*  SUM OF THE SMALLER TWO IS EQUAL TO THE LARGEST ONE) */
/* Computing MAX */
    i__1 = max(ltest[1],ltest[2]);
    mmax = max(i__1,ltest[3]);
    if (ltest[1] == mmax) {
	*ichang = jumplp_(mxnd, mln, &lnodes[lnodes_offset], &itest[1], &
		ltest[2]);
    } else if (ltest[2] == mmax) {
	*ichang = jumplp_(mxnd, mln, &lnodes[lnodes_offset], &itest[2], &
		ltest[3]);
    } else {
	*ichang = jumplp_(mxnd, mln, &lnodes[lnodes_offset], &itest[3], &
		ltest[1]);
    }
/*  TEST THE POSSIBLE RECTANGLE FOR GOODNESS */
/*  ADD UP THE NICKS FOR BAD ANGLES AT THE GIVEN CORNERS */
/*      DO 100 I = 1, NCORN */
/*         IF (ICOMB (I) .EQ. 1) THEN */
/*            QUAL = QUAL + (.8 * NICKC (ANGLE (LCORN (I)) )) */
/*         ELSE */
/*            QUAL = QUAL + (.8 * NICKS (ANGLE (LCORN (I)) )) */
/*         ENDIF */
/*  100 CONTINUE */
/*  ADD UP THE NICKS FOR THE NEW CORNER */
/*      QUAL = QUAL + (.8 * NICKS (ANGLE (ICHANG)) ) */
    return 0;
} /* ch3to4_ */

#ifdef __cplusplus
	}
#endif

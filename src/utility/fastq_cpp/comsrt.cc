/* comsrt.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int comsrt_(integer *mxnd, integer *mxcorn, integer *mxpick, 
	integer *mln, integer *lnodes, integer *lcorn, integer *ncorn, 
	integer *icomb, integer *itype, integer *npick)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, icomb_dim1, icomb_offset, i__1, i__2, 
	    i__3;

    /* Local variables */
    static integer i__, j, k, itest;

/* *********************************************************************** */
/*  SUBROUTINE COMSRT = THIS SUBROUTINE GETS ALL THE COMBINATIONS */
/*                      POSSIBLE OF CORNERS, SIDES, AND DISSECION */
/*                      NODES */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     ICOMB  = THE DIFFERENT VALID PRIMITIVE COMBINATIONS */
/*                 ICOMB (I, J) WHERE I IS THE COMBINATION NUMBER */
/*                              AND J IS THE CORNER NUMBER. */
/*                 THE VALUE OF ICOMB (I,J) IS 1 FOR CORNER AND 0 FOR */
/*                 FOR A SIDE INTERPRETATION */
/*     ITYPE  = THE TYPE OF PRIMITIVE OR NUMBER OF CORNERS IN THIS */
/*              COMBINATION. */
/*                  = 0 FOR A CIRCLE */
/*                  = 1 FOR A TEARDROP */
/*                  = 2 FOR A SEMICIRCLE */
/*                  = 3 FOR A TRIANGLE */
/*                  = 4 FOR A RECTANGLE */
/*                  = >4 OTHERWISE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --lcorn;
    --itype;
    icomb_dim1 = *mxcorn;
    icomb_offset = 1 + icomb_dim1;
    icomb -= icomb_offset;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *npick = 1;
    i__1 = *mxpick;
    for (i__ = 1; i__ <= i__1; ++i__) {
	itype[i__] = 0;
/* L100: */
    }
    i__1 = *ncorn;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*  PUT PURE CORNER AND CORNER/SIDE DESIGNATIONS IN ORDER */
	itest = lnodes[lcorn[i__] * lnodes_dim1 + 6];
	if (itest <= 2) {
	    i__2 = *npick;
	    for (j = 1; j <= i__2; ++j) {
		icomb[i__ + j * icomb_dim1] = 1;
		++itype[j];
/* L110: */
	    }
	    if (itest == 2) {
		i__2 = *npick << 1;
		for (j = *npick + 1; j <= i__2; ++j) {
		    i__3 = i__ - 1;
		    for (k = 1; k <= i__3; ++k) {
			icomb[k + j * icomb_dim1] = icomb[k + (j - *npick) * 
				icomb_dim1];
/* L120: */
		    }
		    itype[j] = itype[j - *npick] - 1;
		    icomb[i__ + j * icomb_dim1] = 0;
/* L130: */
		}
		*npick <<= 1;
	    }
/*  PUT PURE SIDE AND SIDE/DISSECTIONS DESIGNATIONS IN ORDER */
	} else if (itest <= 4) {
	    i__2 = *npick;
	    for (j = 1; j <= i__2; ++j) {
		icomb[i__ + j * icomb_dim1] = 0;
/* L140: */
	    }
	}
/* L150: */
    }
    return 0;
} /* comsrt_ */

#ifdef __cplusplus
	}
#endif

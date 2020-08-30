/* ltnew.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int ltnew_(integer *mdim, integer *link)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/* *********************************************************************** */
/*  SUBROUTINE LTNEW = LOOKUP TABLE CLEARING FOR DATA POINTER ARRAYS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     MDIM   = DIMENSION OF LINK ARRAY, AND BASE FOR LOOKUP START */
/*     LINK   = LOOKUP TABLE ARRAY OF ID'S AND POINTERS */
/*              LINK(1,I) = ID VALUE STORED IN I'TH ROW (0 IF EMPTY) */
/*              LINK(2,I) = DATA POINTER ASSOCIATED W/THIS I'TH ID VALUE */
/* *********************************************************************** */
/*  ZERO OUT THE ID ROW ONLY */
    /* Parameter adjustments */
    link -= 3;

    /* Function Body */
    i__1 = *mdim;
    for (i__ = 1; i__ <= i__1; ++i__) {
	link[(i__ << 1) + 1] = 0;
/* L100: */
    }
    return 0;
} /* ltnew_ */

#ifdef __cplusplus
	}
#endif

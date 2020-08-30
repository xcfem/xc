/* ltadd.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int ltadd_(integer *mdim, integer *mdimo, integer *n, 
	integer *link, integer *ihold)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE LTADD = LOOKUP TABLE REINSURTION FOR DATA POINTER ARRAYS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     MDIM   = DIMENSION OF LINK ARRAY, AND BASE FOR LOOKUP START */
/*     LINK   = LOOKUP TABLE ARRAY OF ID'S AND POINTERS */
/*              LINK(1,I) = ID VALUE STORED IN I'TH ROW (0 IF EMPTY) */
/*              LINK(2,I) = DATA POINTER ASSOCIATED W/THIS I'TH ID VALUE */
/*     IHOLD  = TEMPORARY LINK ARRAY TO BE USED FOR REINSURRTION */
/* *********************************************************************** */
    /* Parameter adjustments */
    ihold -= 3;
    link -= 3;

    /* Function Body */
    addlnk = TRUE_;
/*  SORT THROUGH THE ORIGINAL LINK LIST AND LINK VALID ENTRIES IN THE */
/*  IHOLD LIST */
    i__1 = *mdimo;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (link[(i__ << 1) + 1] != 0 && link[(i__ << 1) + 2] <= *n) {
	    i__3 = (i__2 = link[(i__ << 1) + 1], abs(i__2));
	    ltsort_(mdim, &ihold[3], &i__3, &link[(i__ << 1) + 2], &addlnk);
	}
/* L100: */
    }
/*  TRANSFER THE IHOLD LIST BACK TO THE LINK LIST */
    i__1 = *mdim;
    for (i__ = 1; i__ <= i__1; ++i__) {
	for (j = 1; j <= 2; ++j) {
	    link[j + (i__ << 1)] = ihold[j + (i__ << 1)];
/* L110: */
	}
/* L120: */
    }
    return 0;
} /* ltadd_ */

#ifdef __cplusplus
	}
#endif

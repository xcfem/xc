/* flagd.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int flagd_(integer *mdim, integer *n, integer *link, integer 
	*inum, logical *flag__)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, ii;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE FLAGD = FLAGS THE DATA TO BE PLOTTED */
/* *********************************************************************** */
    /* Parameter adjustments */
    --inum;
    link -= 3;

    /* Function Body */
    addlnk = FALSE_;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mdim, &link[3], &i__, &ii, &addlnk);
	if (ii > 0) {
	    if (*flag__) {
		inum[ii] = -(i__2 = inum[ii], abs(i__2));
	    } else {
		inum[ii] = (i__2 = inum[ii], abs(i__2));
	    }
	}
/* L100: */
    }
    return 0;
} /* flagd_ */

#ifdef __cplusplus
	}
#endif

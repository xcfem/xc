/* snapxy.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int snapxy_(integer *mp, integer *msnap, integer *n, integer 
	*ipoint, real *coor, integer *linkp, real *snapdx, integer *nsnap)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    static integer i__, ii;
    static logical err;
    static integer index;
    static logical addlnk;
    extern /* Subroutine */ int addsnp_(integer *, real *, integer *, integer 
	    *, real *, logical *), ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE SNAPXY = ADDS SNAP GRIDS AT EVERY X, Y LOCATION */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    --ipoint;
    snapdx -= 3;
    --nsnap;

    /* Function Body */
    addlnk = FALSE_;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__3 = (i__2 = ipoint[i__], abs(i__2));
	ltsort_(mp, &linkp[3], &i__3, &ii, &addlnk);
	if (ii > 0) {
	    index = 1;
	    addsnp_(msnap, &snapdx[3], &nsnap[1], &index, &coor[(ii << 1) + 1]
		    , &err);
	    if (err) {
		return 0;
	    }
	    index = 2;
	    addsnp_(msnap, &snapdx[3], &nsnap[1], &index, &coor[(ii << 1) + 2]
		    , &err);
	    if (err) {
		return 0;
	    }
	}
/* L100: */
    }
    return 0;
} /* snapxy_ */

#ifdef __cplusplus
	}
#endif

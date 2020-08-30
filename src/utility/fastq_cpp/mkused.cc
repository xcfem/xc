/* mkused.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int mkused_(integer *mxnl, integer *mp, integer *ml, integer 
	*listl, integer *ipoint, integer *nint, integer *linkp, integer *
	linkl, integer *lcon, integer *nl)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, il, ip1, ip2;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE MKUSED = MARKS ALL LINES AND POINTS USED IN THE PERIMETER */
/* *********************************************************************** */
/*  NOTE: */
/*     THE MESH TABLES ARE EFFECTIVELY DESTROYED BY THIS ROUTINE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --listl;
    linkp -= 3;
    --ipoint;
    lcon -= 4;
    linkl -= 3;
    --nint;

    /* Function Body */
    addlnk = FALSE_;
/*  FLAG ALL LINES AND POINTS AS BEING USED */
    i__1 = *nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(ml, &linkl[3], &listl[i__], &il, &addlnk);
	nint[il] = -(i__2 = nint[il], abs(i__2));
	ltsort_(mp, &linkp[3], &lcon[il * 3 + 1], &ip1, &addlnk);
	ltsort_(mp, &linkp[3], &lcon[il * 3 + 2], &ip2, &addlnk);
	ipoint[ip1] = -(i__2 = ipoint[ip1], abs(i__2));
	ipoint[ip2] = -(i__2 = ipoint[ip2], abs(i__2));
/* L100: */
    }
    return 0;
} /* mkused_ */

#ifdef __cplusplus
	}
#endif

/* nps.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int nps_(integer *ml, integer *ms, integer *mnnps, integer *
	ns, integer *islist, integer *nint, integer *ifline, integer *nlps, 
	integer *illist, integer *linkl, integer *links, integer *nnps, 
	logical *err)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j, j1, j2, il, ks, ipntr;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE NPS = GIVES A LIST OF THE NUMBER OF PERIMETER NODES */
/*                   ON EACH OF A REGION'S SIDES */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     QMESH = GENERATES QUAD ELEMENTS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NNPS  = ARRAY OF NUMBER OF NODES PER SIDE */
/*     CCW   = .TRUE. IF THE SIDE IS ORIENTED CCW */
/*     KS    = COUNTER OF THE NUMBER OF SIDES */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkl -= 3;
    --nint;
    links -= 3;
    --illist;
    --nlps;
    --ifline;
    --nnps;
    --islist;

    /* Function Body */
    *err = TRUE_;
    addlnk = FALSE_;
    ks = 0;
    i__1 = *ns;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (islist[i__] < 0) {
	    ++ks;
	    il = (i__2 = islist[i__], abs(i__2));
	    ltsort_(ml, &linkl[3], &il, &ipntr, &addlnk);
	    nnps[ks] = (i__2 = nint[ipntr], abs(i__2)) + 1;
	} else if (islist[i__] > 0) {
	    ltsort_(ms, &links[3], &islist[i__], &ipntr, &addlnk);
	    j1 = ifline[ipntr];
	    j2 = j1 + nlps[ipntr] - 1;
	    ++ks;
	    nnps[ks] = 0;
	    i__2 = j2;
	    for (j = j1; j <= i__2; ++j) {
		il = illist[j];
		ltsort_(ml, &linkl[3], &il, &ipntr, &addlnk);
		nnps[ks] += (i__3 = nint[ipntr], abs(i__3));
/* L100: */
	    }
	    ++nnps[ks];
	} else {
	    return 0;
	}
/* L110: */
    }
    *err = FALSE_;
    return 0;
} /* nps_ */

#ifdef __cplusplus
	}
#endif

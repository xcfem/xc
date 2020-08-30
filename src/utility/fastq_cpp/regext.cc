/* regext.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int regext_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *n, integer *ii, real *coor, integer *iline, integer *
	ltype, integer *lcon, integer *nlps, integer *ifline, integer *illist,
	 integer *nspr, integer *ifside, integer *islist, integer *linkp, 
	integer *linkl, integer *links, integer *linkr, real *xmin, real *
	xmax, real *ymin, real *ymax)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;

    /* Local variables */
    static integer j, k;
    static real x1, y1;
    static integer jj, kk, ipnt;
    static logical test;
    extern /* Subroutine */ int dline_(integer *, integer *, real *, integer *
	    , integer *, integer *, integer *, integer *, integer *, logical *
	    , real *, real *, logical *, logical *, real *, real *, real *, 
	    real *);
    static logical found;
    static integer ipntr;
    static logical addlnk, getmax, numplt;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE REGEXT = GETS THE REGION EXTREMES */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    linkl -= 3;
    lcon -= 4;
    --ltype;
    --iline;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    linkr -= 3;
    --islist;
    --ifside;
    --nspr;
    --n;

    /* Function Body */
    addlnk = FALSE_;
    getmax = TRUE_;
    found = FALSE_;
    i__1 = ifside[*ii] + nspr[*ii] - 1;
    for (j = ifside[*ii]; j <= i__1; ++j) {
/*  GET SIDE EXTREMES */
	if (islist[j] > 0) {
	    ltsort_(ms, &links[3], &islist[j], &ipntr, &addlnk);
	    if (ipntr > 0) {
		i__2 = ifline[ipntr] + nlps[ipntr] - 1;
		for (k = ifline[ipntr]; k <= i__2; ++k) {
		    ltsort_(ml, &linkl[3], &illist[k], &kk, &addlnk);
		    if (kk > 0) {
			if (! found) {
			    i__4 = (i__3 = lcon[kk * 3 + 1], abs(i__3));
			    ltsort_(mp, &linkp[3], &i__4, &ipnt, &addlnk);
			    if (ipnt > 0) {
				*xmax = coor[(ipnt << 1) + 1];
				*xmin = coor[(ipnt << 1) + 1];
				*ymax = coor[(ipnt << 1) + 2];
				*ymin = coor[(ipnt << 1) + 2];
				found = TRUE_;
			    }
			}
			if (found) {
			    dline_(mp, ml, &coor[3], &linkp[3], &iline[kk], &
				    ltype[kk], &lcon[kk * 3 + 1], &lcon[kk * 
				    3 + 2], &lcon[kk * 3 + 3], &numplt, &x1, &
				    y1, &test, &getmax, xmin, xmax, ymin, 
				    ymax);
			}
		    }
/* L100: */
		}
	    }
/*  GET LINE EXTREMES */
	} else if (islist[j] < 0) {
	    jj = (i__2 = islist[j], abs(i__2));
	    ltsort_(ml, &linkl[3], &jj, &kk, &addlnk);
	    if (kk > 0) {
		if (! found) {
		    i__3 = (i__2 = lcon[kk * 3 + 1], abs(i__2));
		    ltsort_(mp, &linkp[3], &i__3, &ipnt, &addlnk);
		    if (ipnt > 0) {
			*xmax = coor[(ipnt << 1) + 1];
			*xmin = coor[(ipnt << 1) + 1];
			*ymax = coor[(ipnt << 1) + 2];
			*ymin = coor[(ipnt << 1) + 2];
			found = TRUE_;
		    }
		}
		if (found) {
		    dline_(mp, ml, &coor[3], &linkp[3], &iline[kk], &ltype[kk]
			    , &lcon[kk * 3 + 1], &lcon[kk * 3 + 2], &lcon[kk *
			     3 + 3], &numplt, &x1, &y1, &test, &getmax, xmin, 
			    xmax, ymin, ymax);
		}
	    }
	}
/* L110: */
    }
    return 0;
} /* regext_ */

#ifdef __cplusplus
	}
#endif

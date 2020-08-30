/* getext.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getext_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *n, integer *ipoint, real *coor, integer *iline, integer *
	ltype, integer *lcon, integer *nlps, integer *ifline, integer *illist,
	 integer *nspr, integer *ifside, integer *islist, integer *linkp, 
	integer *linkl, integer *links, integer *linkr, real *rextrm, real *
	bxmin, real *bxmax, real *bymin, real *bymax)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1;

    /* Local variables */
    static integer i__;
    static real x1, y1;
    static integer ii;
    static real xmin, xmax, ymin;
    static logical test;
    static real ymax;
    extern /* Subroutine */ int dline_(integer *, integer *, real *, integer *
	    , integer *, integer *, integer *, integer *, integer *, logical *
	    , real *, real *, logical *, logical *, real *, real *, real *, 
	    real *);
    static logical found, addlnk, getmax;
    extern /* Subroutine */ int regext_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, real *);
    static logical numplt;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE GETEXT = GETS THE REGION AND BODY EXTREMES */
/* *********************************************************************** */
/*  GET THE POINTS EXTREMES */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    --ipoint;
    linkl -= 3;
    lcon -= 4;
    --ltype;
    --iline;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    rextrm -= 5;
    linkr -= 3;
    --islist;
    --ifside;
    --nspr;
    --n;

    /* Function Body */
    addlnk = FALSE_;
    getmax = TRUE_;
    found = FALSE_;
    i__1 = n[1];
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__3 = (i__2 = ipoint[i__], abs(i__2));
	ltsort_(mp, &linkp[3], &i__3, &ii, &addlnk);
	if (ii > 0) {
	    if (found) {
/* Computing MAX */
		r__1 = coor[(ii << 1) + 1];
		*bxmax = dmax(r__1,*bxmax);
/* Computing MAX */
		r__1 = coor[(ii << 1) + 2];
		*bymax = dmax(r__1,*bymax);
/* Computing MIN */
		r__1 = coor[(ii << 1) + 1];
		*bxmin = dmin(r__1,*bxmin);
/* Computing MIN */
		r__1 = coor[(ii << 1) + 2];
		*bymin = dmin(r__1,*bymin);
	    } else {
		*bxmax = coor[(ii << 1) + 1];
		*bxmin = coor[(ii << 1) + 1];
		*bymax = coor[(ii << 1) + 2];
		*bymin = coor[(ii << 1) + 2];
		found = TRUE_;
	    }
	}
/* L100: */
    }
/*  GET ALL THE LINES EXTREMES */
    if (found) {
	i__1 = n[2];
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__3 = (i__2 = iline[i__], abs(i__2));
	    ltsort_(ml, &linkl[3], &i__3, &ii, &addlnk);
	    if (ii > 0) {
		dline_(mp, ml, &coor[3], &linkp[3], &iline[ii], &ltype[ii], &
			lcon[ii * 3 + 1], &lcon[ii * 3 + 2], &lcon[ii * 3 + 3]
			, &numplt, &x1, &y1, &test, &getmax, bxmin, bxmax, 
			bymin, bymax);
	    }
/* L110: */
	}
    } else {
	*bxmin = (float)0.;
	*bxmax = (float)20.;
	*bymin = (float)0.;
	*bymax = (float)15.;
    }
/*  CALCULATE THE EXTREMES FOR EACH REGION */
    i__1 = n[22];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mr, &linkr[3], &i__, &ii, &addlnk);
	if (ii > 0) {
	    regext_(mp, ml, ms, mr, &n[1], &ii, &coor[3], &iline[1], &ltype[1]
		    , &lcon[4], &nlps[1], &ifline[1], &illist[1], &nspr[1], &
		    ifside[1], &islist[1], &linkp[3], &linkl[3], &links[3], &
		    linkr[3], &xmin, &xmax, &ymin, &ymax);
	    rextrm[(ii << 2) + 1] = xmin;
	    rextrm[(ii << 2) + 2] = xmax;
	    rextrm[(ii << 2) + 3] = ymin;
	    rextrm[(ii << 2) + 4] = ymax;
	}
/* L120: */
    }
    return 0;
} /* getext_ */

#ifdef __cplusplus
	}
#endif

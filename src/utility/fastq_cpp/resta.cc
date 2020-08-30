/* resta.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int resta_(integer *mxnd, real *xn, real *yn, integer *nuid, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, integer *kkk, 
	integer *kkkold, integer *navail, integer *iavail, integer *nnn, 
	integer *limit, integer *irest, real *tilt, logical *err, logical *
	noroom)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, k, k2, ik, ik1, ik2;
    static real cnd[26];
    static logical ccw;
    static real area;
    static integer kcnd[26];
    static real cond;
    static integer ntab;
    static logical done;
    static real asum, qrat, srat;
    static integer nsum, ntry;
    static real cond1, cond2;
    static logical carea, lside;
    static real sides[4];
    static integer nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);
    static real dummy;
    extern /* Subroutine */ int bubble_(real *, integer *, integer *, integer 
	    *), mesage_(char *, ftnlen), qaaval_(integer *, integer *, real *,
	     real *, real *, real *, real *, logical *);
    static real angles[4];
    static integer maxtab;
    extern /* Subroutine */ int condno_(integer *, integer *, real *, real *, 
	    real *, real *, real *, real *, logical *);
    static real cndtol;
    extern /* Subroutine */ int restry_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, real *, 
	    integer *, integer *, integer *, integer *, logical *, logical *, 
	    logical *);

/* ************************************************************************ */
/*  SUBROUTINE RESTA  =  RESTRUCTURES THE MESH TO ELIMINATE WORST ELELMENTS */
/* *********************************************************************** */
/*  NOTE: */
/*     A RECORD IS KEPT OF UP TO 25 OF THE CURRENT WORST CONDITION NUMBERS */
/*     AND THE WORST ELEMENT POSSIBLE IS RESTRUCTURED */
/*     UNTIL NO FURTHER RESTRUCTURING CAN BE DONE. */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;

    /* Function Body */
    *err = FALSE_;
/*  CHECK FOR IMPENDING OVERFLOW */
    if (*navail <= 1) {
	*noroom = TRUE_;
	mesage_("INSUFFICIENT STORAGE AVAILABLE IN RESTA", (ftnlen)39);
	return 0;
    }
/*  INITIALIZE */
    ntab = 0;
    maxtab = 25;
    cndtol = (float)2.;
    asum = (float)0.;
    nsum = 0;
    ccw = TRUE_;
    carea = FALSE_;
    *irest = 0;
    i__1 = *kkk;
    for (k = *kkkold + 1; k <= i__1; ++k) {
	if (lxk[(k << 2) + 1] > 0) {
	    lside = FALSE_;
/*  GET THE ELEMENTS COND VALUE (BASED ON ANGLE AND SIDE LENGTH) */
	    gnxka_(mxnd, &xn[1], &yn[1], &k, nodes, &area, &lxk[5], &nxl[3], &
		    ccw);
	    qaaval_(mxnd, nodes, angles, &qrat, &dummy, &xn[1], &yn[1], &
		    carea);
	    condno_(mxnd, nodes, &qrat, &srat, &cond, sides, &xn[1], &yn[1], &
		    lside);
/*  ADD UP THE NUMBER OF ANGLES < PI/2 */
	    for (i__ = 1; i__ <= 4; ++i__) {
		if (angles[i__ - 1] <= (float)1.58) {
		    asum += angles[i__ - 1];
		    ++nsum;
		}
/* L100: */
	    }
/*  ADD BAD ELEMENTS TO THE LIST */
	    if (cond >= cndtol) {
		cnd[ntab] = cond;
		kcnd[ntab] = k;
		i__2 = ntab + 1;
		bubble_(cnd, kcnd, &ntab, &i__2);
/* Computing MIN */
		i__2 = ntab + 1;
		ntab = min(i__2,maxtab);
	    }
	}
/* L110: */
    }
/*  TILT IS THE AVERAGE VALUE IN DEGREES OF ANGLES < PI/2 */
    if (nsum > 0) {
	*tilt = asum / (doublereal) nsum * (float)57.2957795;
    } else {
	*tilt = (float)90.;
    }
    if (*limit <= 0 || ntab <= 0) {
	return 0;
    }
    cndtol = cnd[ntab - 1];
/*  TRY TO RESTRUCTURE ON THE 10 WORST ELEMENTS ONLY */
L120:
    ntry = min(ntab,10);
    i__1 = ntry;
    for (ik = 1; ik <= i__1; ++ik) {
	ik1 = ik;
	restry_(mxnd, &kcnd[ik - 1], &k2, &lxk[5], &nxl[3], &kxl[3], &lxn[5], 
		&xn[1], &yn[1], &nuid[1], navail, iavail, nnn, &done, err, 
		noroom);
	if (*err) {
	    return 0;
	}
	if (done) {
	    goto L140;
	}
/* L130: */
    }
    return 0;
L140:
    ++(*irest);
    if (*irest >= *limit) {
	return 0;
    }
/*  UPDATE THE TABLE (AFTER 1 RESTRUCTURE) */
    gnxka_(mxnd, &xn[1], &yn[1], &kcnd[ik1 - 1], nodes, &area, &lxk[5], &nxl[
	    3], &ccw);
    qaaval_(mxnd, nodes, angles, &qrat, &dummy, &xn[1], &yn[1], &carea);
    condno_(mxnd, nodes, &qrat, &srat, &cond1, sides, &xn[1], &yn[1], &lside);
    cnd[ik1 - 1] = cond1;
    i__1 = ntab;
    for (ik = 1; ik <= i__1; ++ik) {
	ik2 = ik;
	if (kcnd[ik - 1] == k2) {
	    goto L160;
	}
/* L150: */
    }
    ik2 = ntab + 1;
    ++ntab;
L160:
    gnxka_(mxnd, &xn[1], &yn[1], &k2, nodes, &area, &lxk[5], &nxl[3], &ccw);
    qaaval_(mxnd, nodes, angles, &qrat, &dummy, &xn[1], &yn[1], &carea);
    condno_(mxnd, nodes, &qrat, &srat, &cond2, sides, &xn[1], &yn[1], &lside);
    cnd[ik2 - 1] = cond2;
    kcnd[ik2 - 1] = k2;
/*  RE-SORT AND PRUNE */
    bubble_(cnd, kcnd, &c__1, &ntab);
    for (i__ = 1; i__ <= 2; ++i__) {
	if (cnd[ntab - 1] < cndtol) {
	    --ntab;
	}
/* L170: */
    }
    ntab = min(ntab,maxtab);
    if (ntab <= 0) {
	return 0;
    }
    cndtol = cnd[ntab - 1];
    goto L120;
} /* resta_ */

#ifdef __cplusplus
	}
#endif

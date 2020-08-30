/* boxit.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int boxit_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *n, integer *ipoint, real *coor, integer *iline, integer *
	ltype, integer *lcon, integer *iregn, integer *imat, integer *nspr, 
	integer *ifside, integer *islist, integer *linkp, integer *linkl, 
	integer *linkr, integer *linkm, integer *nholdr, integer *iholdr, 
	integer *nholdm, integer *iholdm, integer *irgflg, real *x, real *y, 
	real *y1, real *y2, logical *boxed, logical *merge, logical *noroom)
{
    /* System generated locals */
    integer i__1, i__2;
    real r__1;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static integer i__, j, j1, j2, jj;
    static real pi;
    static integer np;
    static logical err;
    static integer ifind, ihold[100]	/* was [50][2] */, jhold[50], lastp, 
	    imtrl;
    static real sumth, twopi;
    static integer jfind1, jfind2;
    static real theta1, theta2;
    static logical addlnk, circle;
    static integer ifhold;
    extern /* Subroutine */ int labove_(integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, real *, real *, real *, real *, integer *, integer *, 
	    integer *, integer *), endtan_(integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, logical *), inregn_(
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, logical *);
    static integer ifound;
    extern /* Subroutine */ int irever_(integer *, integer *);
    static integer istart;
    static real thetmx;
    static integer jkount;
    static real testth;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE BOXIT = BOXES IN A REGION SURROUNDING A POINT */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     INPUT  = INPUTS MESH DEFINITIONS FROM THE LIGHT TABLE */
/* *********************************************************************** */
/*  SUBROUTINES CALLED: */
/*     DLPARA = DETERMINES LINE PARAMETERS FROM TWO POINTS */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    --ipoint;
    linkl -= 3;
    lcon -= 4;
    --ltype;
    --iline;
    --irgflg;
    iholdm -= 3;
    iholdr -= 3;
    linkm -= 3;
    linkr -= 3;
    --islist;
    --ifside;
    --nspr;
    --imat;
    --iregn;
    --n;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    twopi = pi + pi;
    *boxed = FALSE_;
    *noroom = FALSE_;
    addlnk = FALSE_;
    circle = FALSE_;
    ifind = 0;
    sumth = (float)0.;
    thetmx = (float)0.;
/*  FIND THE CLOSEST LINE ABOVE THE POINT INPUT */
    labove_(mp, ml, &n[1], &ipoint[1], &coor[3], &iline[1], &ltype[1], &lcon[
	    4], &linkp[3], &linkl[3], x, y, y1, y2, &ifind, &jfind1, &istart, 
	    &np);
    if (ifind <= 0) {
	return 0;
    }
    ifhold = ifind;
/*  SET UP REGION CHECKING CONNECTIVITY */
    for (i__ = 1; i__ <= 50; ++i__) {
	jhold[i__ - 1] = 0;
	ihold[i__ - 1] = 0;
	ihold[i__ + 49] = 0;
/* L100: */
    }
    ifound = 0;
    lastp = istart;
    ltsort_(ml, &linkl[3], &ifind, &jfind1, &addlnk);
    jhold[0] = ifind;
    i__1 = n[2] + 2;
    for (i__ = 1; i__ <= i__1; ++i__) {
	jkount = 0;
/*  GET ALL LINES CONTAINING THE NEW POINT "NP" */
	i__2 = n[19];
	for (j = 1; j <= i__2; ++j) {
	    ltsort_(ml, &linkl[3], &j, &jj, &addlnk);
	    if (jj > 0) {
		j1 = lcon[jj * 3 + 1];
		j2 = lcon[jj * 3 + 2];
		if ((j1 == np || j2 == np) && jj != jfind1) {
		    ++jkount;
		    ihold[jkount - 1] = jj;
		    if (j1 == np) {
			ihold[jkount + 49] = 2;
		    } else {
			ihold[jkount + 49] = 1;
		    }
		}
	    }
/* L110: */
	}
/*  CHECK FOR A CLOSED CIRCLE WITH NO LINES ATTACHED */
	if (jkount == 0 && np == lastp) {
	    ifound = 1;
	    goto L140;
/*  CHECK FOR NO ADDITIONAL LINES ATTACHED */
	} else if (jkount == 0) {
	    return 0;
/*  CHECK FOR A CLOSED CIRCLE ATTACHED TO NP */
	} else if (jkount == 1 && lcon[ihold[0] * 3 + 1] == lcon[ihold[0] * 3 
		+ 2]) {
	    jfind1 = ihold[0];
	    sumth += thetmx;
	    lastp = np;
	    np = lcon[jfind1 * 3 + 1];
	    ifind = iline[jfind1];
	    jhold[i__] = iline[jfind1];
	    ++ifound;
/*  CHECK FOR CLOSING OF THE REGION */
	    if (ifind == ifhold) {
		if (lastp == istart && ifind == ifhold && i__ != 1) {
		    goto L140;
		}
	    }
/*  SET THE FLAG THAT WE ARE RETURNING FROM THIS CLOSED CIRCLE */
	    circle = TRUE_;
/*  USING THE NP COORDINATES AS A NEW CENTER */
/*  CHECK TO SEE WHICH LINE HAS THE SMALLEST INTERIOR ANGLE */
/*   (ASSUMES WE ARE PROGRESSING IN CLOCKWISE ORDER) */
	} else {
	    endtan_(mp, ml, &n[1], &ipoint[1], &coor[3], &ltype[1], &lcon[4], 
		    &linkp[3], &linkl[3], &ifind, &jfind1, &np, &theta1, &err)
		    ;
/*  SET THE SPECIAL CASE OF A CLOSED CIRCLE WITH ATTACHED LINES */
/*  AND NOT A RETURN FROM A CIRCLE AS HAVING A THETMX OF PI. */
/*  A CLOSED CIRCLE RETURN GETS THE ENDTANGENT FLIPPED TO GET THE */
/*  RIGHT INTERIOR ANGLE. */
	    if (np == lastp) {
		if (circle) {
		    theta1 -= pi;
		    if (theta1 < (float)0.) {
			theta1 += twopi;
		    } else if (theta1 > twopi) {
			theta1 -= twopi;
		    }
		    thetmx = twopi * (float)2.;
		} else {
		    thetmx = pi;
		}
	    } else {
		thetmx = twopi * (float)2.;
	    }
	    i__2 = jkount;
	    for (j = 1; j <= i__2; ++j) {
/*  JJ = THE POINTER TO THE ATTACHED POINT OF THE LINE BEING TESTED */
		endtan_(mp, ml, &n[1], &ipoint[1], &coor[3], &ltype[1], &lcon[
			4], &linkp[3], &linkl[3], &iline[ihold[j - 1]], &
			ihold[j - 1], &np, &theta2, &err);
		testth = theta2 - theta1;
		if (testth < (float)0.) {
		    testth += twopi;
		} else if (testth > twopi) {
		    testth -= twopi;
		}
		if (! err && (testth <= thetmx || (r__1 = thetmx - testth, 
			dabs(r__1)) < (float)1e-4)) {
		    thetmx = testth;
		    jfind1 = ihold[j - 1];
		    jfind2 = ihold[j + 49];
		}
/* L120: */
	    }
/*  CHECK FOR CLOSING OF THE REGION */
	    if (ifind == ifhold) {
/*  FIRST THE SINGLE LINE REGION WITH LINES ATTACHED OUTSIDE THE CIRCLE */
		if (np == lastp && ifind == iline[jfind1]) {
		    ifound = 1;
		    goto L140;
/*  SECOND TEST FOR THE NORMAL CLOSING */
		} else if (lastp == istart && ifind == ifhold && i__ != 1) {
		    goto L140;
		}
	    }
	    sumth += thetmx;
	    lastp = np;
	    np = lcon[jfind2 + jfind1 * 3];
	    ifind = iline[jfind1];
	    jhold[i__] = iline[jfind1];
	    ++ifound;
	    circle = FALSE_;
	}
/* L130: */
    }
    return 0;
/*  CHECK TO MAKE SURE THE REGION CLOSED CORRECTLY */
/*      AVETH = SUMTH / DBLE(IFOUND) */
/*      IF (AVETH .GT. 180.) THEN */
/*         CALL VDBELL */
/*         CALL VDBUFL */
/*         RETURN */
/*      ENDIF */
/*  INPUT THE REGION */
L140:
    jj = n[22] + 1;
    imtrl = 0;
    irever_(jhold, &ifound);
    i__1 = ifound;
    for (j = 1; j <= i__1; ++j) {
	jhold[j - 1] = -jhold[j - 1];
/* L150: */
    }
    inregn_(ms, mr, &n[7], &n[8], &n[22], &n[23], &jj, &imtrl, jhold, &ifound,
	     &iregn[1], &imat[1], &nspr[1], &ifside[1], &islist[1], &linkr[3],
	     &linkm[3], nholdr, &iholdr[3], nholdm, &iholdm[3], &irgflg[1], 
	    merge, noroom);
    if (*noroom) {
	return 0;
    }
    *boxed = TRUE_;
    return 0;
} /* boxit_ */

#ifdef __cplusplus
	}
#endif

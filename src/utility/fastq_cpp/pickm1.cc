/* pickm1.f -- translated by f2c (version 20160102).
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


/*     C* FILE: [.QMESH]PICKM1.FOR */
/*     C* MODIFIED BY: TED BLACKER */
/*     C* MODIFICATION DATE: 7/6/90 */
/*     C* MODIFICATION: COMPLETED HEADER INFORMATION */

/* Subroutine */ int pickm1_(integer *n, real *x, real *y, real *angle, 
	integer *m1, integer *ifirst, logical *real__)
{
    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2;
    doublereal d__1;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static integer i__, j, m, i1, i2, i3, i4, m2, ii;
    static real pi;
    static integer isa, nsa;
    static real afac, efac, diff, gval, erat, atol;
    static integer mmax;
    static real temp;
    static integer next;
    static real agold, agnew, gbest, smang[7];
    static integer index[7], itemp;
    static real twopi, badang;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static real amaxel;

/* *********************************************************************** */

/*     SUBROUTINE PICKM1 = DETERMINES A REASONABLE SHAPE FOR A LOGICAL */
/*     RECTANGLE WITH PERIMETER GIVEN IN X AND Y */

/* *********************************************************************** */



/*     FORM THE LIST OF SMALLEST ANGLES */

    /* Parameter adjustments */
    --angle;
    --y;
    --x;

    /* Function Body */
    nsa = 6;
    i__1 = nsa;
    for (i__ = 1; i__ <= i__1; ++i__) {
	smang[i__ - 1] = (float)10.;
	index[i__ - 1] = 0;
/* L100: */
    }

    pi = atan2((float)0., (float)-1.);
    twopi = pi + pi;
    agold = atan2(y[1] - y[*n], x[1] - x[*n]);

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {

/*     GET THE ANGLE FORMED BY THIS SET OF POINTS */

	next = j + 1;
	if (next > *n) {
	    next = 1;
	}
	agnew = atan2(y[next] - y[j], x[next] - x[j]);
	diff = agnew - agold;
	if (diff > pi) {
	    diff -= twopi;
	}
	if (diff < -pi) {
	    diff += twopi;
	}
	angle[j] = pi - diff;
	agold = agnew;

/*     SORT THIS ANGLE AGAINST PREVIOUS ANGLES TO SEE IF IT IS ONE OF */
/*     THE SMALLEST */

	smang[nsa] = angle[j];
	index[nsa] = j;
	i__2 = nsa;
	for (ii = 1; ii <= i__2; ++ii) {
	    i__ = nsa + 1 - ii;
	    if (smang[i__] >= smang[i__ - 1]) {
		goto L120;
	    }
	    temp = smang[i__ - 1];
	    itemp = index[i__ - 1];
	    smang[i__ - 1] = smang[i__];
	    index[i__ - 1] = index[i__];
	    smang[i__] = temp;
	    index[i__] = itemp;
	}
L120:

/* L130: */
	;
    }

/*     DETERMINE OPTIMUM ORIGIN / SHAPE COMBINATION */

    atol = pi * (float)150. / (float)180.;
    *ifirst = 1;
    *m1 = *n / 4;
    m2 = *n / 2 - *m1;
    i2 = *m1 + 1;
    i3 = i2 + m2;
    i4 = i3 + *m1;
    gbest = angle[1] + angle[i2] + angle[i3] + angle[i4];
/* Computing MAX */
    r__1 = angle[1], r__2 = angle[i2], r__1 = max(r__1,r__2), r__2 = angle[i3]
	    , r__1 = max(r__1,r__2), r__2 = angle[i4];
    badang = dmax(r__1,r__2);

    mmax = *n / 2 - 1;
    amaxel = (doublereal) (*n / 4) * (doublereal) ((*n + 2) / 4);
    i__1 = nsa;
    for (isa = 1; isa <= i__1; ++isa) {
	if (smang[isa - 1] <= atol) {
	    i1 = index[isa - 1];
	    i__2 = mmax;
	    for (m = 1; m <= i__2; ++m) {
		m2 = *n / 2 - m;
		i2 = i1 + m;
		if (i2 > *n) {
		    i2 -= *n;
		}
		i3 = i2 + m2;
		if (i3 > *n) {
		    i3 -= *n;
		}
		i4 = i3 + m;
		if (i4 > *n) {
		    i4 -= *n;
		}
		afac = angle[i1] + angle[i2] + angle[i3] + angle[i4];
/* Computing MIN */
		d__1 = amaxel / (doublereal) (m * m2);
		erat = (real) min(d__1,5.);
		efac = (erat + (float)15.) / (float)16.;
		gval = afac * efac;
		if (gval < gbest) {
/* Computing MAX */
		    r__1 = angle[i1], r__2 = angle[i2], r__1 = max(r__1,r__2),
			     r__2 = angle[i3], r__1 = max(r__1,r__2), r__2 = 
			    angle[i4];
		    badang = dmax(r__1,r__2);
		    *ifirst = i1;
		    *m1 = m;
		    gbest = gval;
		}
/* L140: */
	    }
	}
/* L150: */
    }
    if (*real__ && badang > (float)2.62) {
	mesage_(" **  WARNING: CORNER (S) OF THE REGION HAVE  **", (ftnlen)47)
		;
	mesage_(" **           LARGE ANGLES  (> 150 DEGREES.) **", (ftnlen)47)
		;
	mesage_(" **           POORLY FORMED MESH MAY RESULT  **", (ftnlen)47)
		;
    }

    return 0;
} /* pickm1_ */

#ifdef __cplusplus
	}
#endif

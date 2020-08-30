/* pickm3.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int pickm3_(integer *n, real *x, real *y, real *angle, 
	integer *m1, integer *m2, integer *ifirst)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static integer i__, j, i1, i2, i3, n1, n2, ii;
    static real pi;
    static integer mm1, mm2, mm3, isa, nsa, max__;
    static real sum1, sum2, diff, gval, atol;
    static integer mmax;
    static real temp;
    static integer itmp, next;
    static real agold, agnew, gbest, smang[7];
    static integer index[7], itemp, isort[3];
    static real twopi;

/* *********************************************************************** */
/*  SUBROUTINE PICKM3 = DETERMINES A REASONABLE SHAPE FOR A LOGICAL */
/*                      TRIANGLE WITH PERIMETER GIVEN IN X AND Y */
/* *********************************************************************** */
/*  FORM THE LIST OF SMALLEST ANGLES */
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
/*  GET THE ANGLE FORMED BY THIS SET OF POINTS */
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
/*  SORT THIS ANGLE AGAINST PREVIOUS ANGLES TO SEE IF IT IS ONE OF */
/*  THE SMALLEST */
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
/*  DETERMINE OPTIMUM ORIGIN / SHAPE COMBINATION FOR A TRIANGLE */
    atol = pi * (float)150. / (float)180.;
/*  FIND SIDE DIVISION USING 5 SMALLEST ANGLES AND CHECK CONDITION */
    for (i__ = 1; i__ <= 3; ++i__) {
	isort[i__ - 1] = index[i__ - 1];
/* L140: */
    }
    for (i__ = 1; i__ <= 2; ++i__) {
	for (j = i__ + 1; j <= 3; ++j) {
	    if (isort[i__ - 1] > isort[j - 1]) {
		itmp = isort[i__ - 1];
		isort[i__ - 1] = isort[j - 1];
		isort[j - 1] = itmp;
	    }
/* L150: */
	}
/* L160: */
    }
    i1 = isort[0];
    i2 = isort[1];
    i3 = isort[2];
    mm1 = i2 - i1;
    if (mm1 < 0) {
	mm1 = *n + mm1;
    }
    mm2 = i3 - i2;
    if (mm2 < 0) {
	mm2 = *n + mm2;
    }
    mm3 = *n - mm1 - mm2;
/* Computing MAX */
    i__1 = max(mm1,mm2);
    max__ = max(i__1,mm3);
    if (max__ <= *n - max__ - 2) {
/*  ADD UP ASSIGNED ANGLES */
	*ifirst = i1;
	*m1 = mm1;
	*m2 = mm2;
	gbest = angle[i1] + angle[i2] + angle[i3];
    } else {
	*ifirst = 1;
	gbest = (float)1e5;
    }
/*  LIMIT THE SIZE OF ANY SIDE */
    mmax = (*n - 2) / 2;
/*  GO AROUND THE PERIMETER USING THE 10 SMALLEST ANGLES AS POSSIBLE */
/*  STARTING POINTS,  AND THEN FIND THE BEST COMBINATION OF SIDE LENGTHS */
    i__1 = nsa;
    for (isa = 1; isa <= i__1; ++isa) {
	if (smang[isa - 1] <= atol) {
	    i1 = index[isa - 1];
	    sum1 = angle[i1];
	    if (sum1 >= gbest) {
		goto L190;
	    }
/*  ASSIGN A TRIAL SECOND NODE */
	    i__2 = mmax;
	    for (n1 = 2; n1 <= i__2; ++n1) {
		i2 = i1 + n1;
		if (i2 > *n) {
		    i2 -= *n;
		}
		sum2 = sum1 + angle[i2];
		if (sum2 >= gbest) {
		    goto L180;
		}
/*  ASSIGN A TRIAL THIRD NODE */
		i__3 = *n - n1 - 2;
		for (n2 = 2; n2 <= i__3; ++n2) {
		    i3 = i2 + n2;
		    if (i3 > *n) {
			i3 -= *n;
		    }
		    gval = sum2 + angle[i3];
		    if (gval >= gbest) {
			goto L170;
		    }
/*  FIND SIDE DIVISION AND CHECK CONDITION */
		    mm1 = i2 - i1;
		    if (mm1 < 0) {
			mm1 = *n + mm1;
		    }
		    mm2 = i3 - i2;
		    if (mm2 < 0) {
			mm2 = *n + mm2;
		    }
/* ... Guess by GDS, MM1 substituted for MM? */
		    mm3 = *n - mm1 - mm2;
/* Computing MAX */
		    i__4 = max(mm1,mm2);
		    max__ = max(i__4,mm3);
		    if (max__ <= *n - max__ - 2) {
/*  ADD UP ASSIGNED ANGLES AND COMPARE TO PREVIOUS TRIALS */
			*ifirst = i1;
			*m1 = mm1;
			*m2 = mm2;
			gbest = gval;
		    }
L170:
		    ;
		}
L180:
		;
	    }
	}
L190:
	;
    }
    return 0;
} /* pickm3_ */

#ifdef __cplusplus
	}
#endif

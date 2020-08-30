/* csmang.f -- translated by f2c (version 20160102).
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


/*     C* FILE: [.QMESH]CSMANG.FOR */
/*     C* MODIFIED BY: TED BLACKER */
/*     C* MODIFICATION DATE: 7/6/90 */
/*     C* MODIFICATION: COMPLETED HEADER INFORMATION */

/* Subroutine */ int csmang_(integer *n, real *x, real *y, real *angle, 
	integer *nsang, real *smang, integer *index)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static integer i__, j, ii;
    static real pi;
    static integer nsa;
    static real diff, temp;
    static integer next;
    static real agold, agnew;
    static integer itemp;
    static real twopi;

/* *********************************************************************** */

/*     SUBROUTINE CSMANG = CALCULATES THE "NSANG" SMALLEST ANGLES */
/*     AND PLACES THEM IN THE SMANG ARRAY WITH */
/*     THE INDICES BEING PLACED IN THE INDEX ARRAY */

/* *********************************************************************** */

/*     OBSERVATION: - IT DOES NOT MATTER THE ANGLE ORIENTATION. */
/*     - THE ANGLES ARE STORE IN THE ANGLE ARRAY AS THEY */
/*     APPEAR. */
/*     - THE SMALLEST ANGLES ARE IN ASCENDING ORDER. */
/*     - THE INDEX ARRAY RETURNS THE SMALLEST ANGLE POSITION */
/*     IN ASCENDING ORDER. */


/*     MODIFIED BY : HORACIO RECALDE             DATE:JAN 1988 */
/* *********************************************************************** */

    /* Parameter adjustments */
    --angle;
    --y;
    --x;
    --index;
    --smang;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    twopi = pi * (float)2.;

/*     FORM THE LIST OF SMALLEST ANGLES */

    nsa = *nsang;
    i__1 = nsa;
    for (i__ = 1; i__ <= i__1; ++i__) {
	smang[i__] = (float)10.;
	index[i__] = 0;
    }

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

	smang[nsa + 1] = angle[j];
	index[nsa + 1] = j;
	i__2 = nsa;
	for (ii = 1; ii <= i__2; ++ii) {
	    i__ = nsa + 1 - ii;
	    if (smang[i__ + 1] >= smang[i__]) {
		goto L120;
	    }
	    temp = smang[i__];
	    itemp = index[i__];
	    smang[i__] = smang[i__ + 1];
	    index[i__] = index[i__ + 1];
	    smang[i__ + 1] = temp;
	    index[i__ + 1] = itemp;
	}
L120:

	;
    }

    return 0;

} /* csmang_ */

#ifdef __cplusplus
	}
#endif

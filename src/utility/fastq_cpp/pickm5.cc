/* pickm5.f -- translated by f2c (version 20160102).
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

static integer c__10 = 10;
static integer c__12 = 12;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int pickm5_(integer *n, real *x, real *y, real *angle, 
	integer *ist2, integer *ist3, integer *ist4, integer *ist5, integer *
	indst, integer *m1, integer *n1, integer *n2, integer *n3, integer *
	n4)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static integer i__, m2, m3, m4, m5, n5;
    static real pi;
    static integer mm1, mm2, nn1, nn2, mm3, nn3, mm4, nn4;
    static real tol;
    static integer ist1[12];
    static real sum1, sum2, sum3, sum4;
    static integer mmax;
    static logical check;
    static real smang[11];
    static integer index[11];
    extern /* Subroutine */ int skpop_(integer *, integer *, integer *, 
	    integer *);
    static integer m1hold, n1hold, n2hold, n3hold, n4hold;
    extern /* Subroutine */ int chcond_(integer *, integer *, real *, integer 
	    *, integer *, integer *, integer *, integer *, integer *, logical 
	    *);
    static integer irange;
    extern /* Subroutine */ int csmang_(integer *, real *, real *, real *, 
	    integer *, real *, integer *);
    static real sumang;
    extern /* Subroutine */ int skinit_(integer *, integer *, integer *, 
	    integer *);
    static integer ierror;
    extern /* Subroutine */ int skpush_(integer *, integer *, integer *, 
	    integer *), sortst_(integer *, real *, integer *, integer *, 
	    integer *, integer *);

/* *********************************************************************** */
/*  SUBROUTINE PICKM5 = DETERMINES A REASONABLE SHAPE FOR A LOGICAL */
/*                      PENTAGON WITH PERIMETER GIVEN IN X AND Y */
/* *********************************************************************** */
/*  VARIABLES IN: N....... NUMBER OF POINTS */
/*                X, Y.... ARRAY OF COORDINATES */
/*                ANGLE... ARRAY OF ANGLES */
/*           OUT: Ni...... POINTERS TO THE CHOSEN CORNERS */
/*  WRITTEN BY: HORACIO RECALDE                       DATE: JAN, 1988 */
/* ********************************************************************** */
/* --- SET THE MAXIMUM SIDE AND TOLERANCE */
    /* Parameter adjustments */
    --indst;
    --ist5;
    --ist4;
    --ist3;
    --ist2;
    --angle;
    --y;
    --x;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    mmax = *n / 2 - 1;
    tol = pi * (float).83333333333333337;
/* ---GET THE 10 SMALLEST ANGLES */
    csmang_(n, &x[1], &y[1], &angle[1], &c__10, smang, index);
    if (angle[index[0]] > tol) {
	*m1 = -1;
	return 0;
    }
/* ---CHECK CONDITIONS ON THE 5 SMALLEST ANGLES */
    chcond_(n, &c__10, smang, index, m1, n1, n2, n3, n4, &check);
    if (check) {
	return 0;
    }
    sumang = (float)99999.;
/* ---PUSH ANGLE INDEX ON FIRST STACK */
    skinit_(ist1, &c__12, &c__10, &ierror);
    for (i__ = 10; i__ >= 1; --i__) {
	skpush_(ist1, &c__12, &index[i__ - 1], &ierror);
/* L100: */
    }
/* ---POP TOP OF FIRST STACK */
L110:
    skpop_(ist1, &c__12, m1, &ierror);
    if (ierror == 2) {
	goto L200;
    }
    if (angle[*m1] > sumang) {
	goto L200;
    }
    sum1 = angle[*m1];
/* ---SELECT SECOND TRIAL */
    mm1 = *m1 + 2;
    if (mm1 > *n) {
	mm1 -= *n;
    }
/* Computing MIN */
    i__1 = *m1 + mmax, i__2 = *m1 + *n - 8;
    nn1 = min(i__1,i__2);
    if (nn1 > *n) {
	nn1 -= *n;
    }
    sortst_(n, &angle[1], &mm1, &nn1, &irange, &indst[1]);
/* ---PUSH ANGLE INDEX ON SECOND STACK IN DESCENDING ORDER */
    i__1 = *n - 2;
    skinit_(&ist2[1], n, &i__1, &ierror);
    for (i__ = min(10,irange); i__ >= 1; --i__) {
	skpush_(&ist2[1], n, &indst[i__], &ierror);
/* L120: */
    }
/* ---POP TOP OF SECOND STACK */
L130:
    skpop_(&ist2[1], n, &m2, &ierror);
    if (ierror == 2) {
	goto L110;
    }
    if (sum1 + angle[m2] >= sumang) {
	goto L110;
    }
    sum2 = sum1 + angle[m2];
    *n1 = m2 - *m1;
    if (*n1 < 0) {
	*n1 = *n + *n1;
    }
/* ---SELECT THIRD TRIAL NODE AND SORT IN ASCENDING ORDER */
    mm2 = m2 + 2;
    if (mm2 > *n) {
	mm2 -= *n;
    }
/* Computing MIN */
    i__1 = *m1 + *n1 + mmax, i__2 = *m1 + *n - 6;
    nn2 = min(i__1,i__2);
    if (nn2 > *n) {
	nn2 -= *n;
    }
    sortst_(n, &angle[1], &mm2, &nn2, &irange, &indst[1]);
/* ---PUSH ANGLE INDEX ON THIRD STACK IN DESCENDING ORDER */
    i__1 = *n - 2;
    skinit_(&ist3[1], n, &i__1, &ierror);
    for (i__ = min(10,irange); i__ >= 1; --i__) {
	skpush_(&ist3[1], n, &indst[i__], &ierror);
/* L140: */
    }
/* ---POP TOP OF THIRD STACK */
L150:
    skpop_(&ist3[1], n, &m3, &ierror);
    if (ierror == 2) {
	goto L130;
    }
    if (sum2 + angle[m3] >= sumang) {
	goto L130;
    }
    sum3 = sum2 + angle[m3];
    *n2 = m3 - m2;
    if (*n2 < 0) {
	*n2 = *n + *n2;
    }
    if (*n1 + *n2 > *n / 2 - 1) {
	goto L150;
    }
/* ---SELECT FOURTH TRIAL AND SORT IN ASCENDING ORDER */
    mm3 = m3 + 2;
    if (mm3 > *n) {
	mm3 -= *n;
    }
/* Computing MIN */
    i__1 = *m1 + *n1 + *n2 + mmax, i__2 = *m1 + *n - 4;
    nn3 = min(i__1,i__2);
    if (nn3 > *n) {
	nn3 -= *n;
    }
    sortst_(n, &angle[1], &mm3, &nn3, &irange, &indst[1]);
/* ---PUSH ANGLE INDEX ON FOURTH STACK IN DESCENDING ORDER */
    i__1 = *n - 2;
    skinit_(&ist4[1], n, &i__1, &ierror);
    for (i__ = min(10,irange); i__ >= 1; --i__) {
	skpush_(&ist4[1], n, &indst[i__], &ierror);
/* L160: */
    }
/* ---POP TOP OF FOURTH STACK */
L170:
    skpop_(&ist4[1], n, &m4, &ierror);
    if (ierror == 2) {
	goto L150;
    }
    if (sum3 + angle[m4] >= sumang) {
	goto L150;
    }
    sum4 = sum3 + angle[m4];
    *n3 = m4 - m3;
    if (*n3 < 0) {
	*n3 = *n + *n3;
    }
    if (*n2 + *n3 > *n / 2 - 1) {
	goto L170;
    }
/* ---SELECT FIFTH TRIAL AND SORT IN ASCENDING ORDER */
    mm4 = m4 + 2;
    if (mm4 > *n) {
	mm4 -= *n;
    }
/* Computing MIN */
    i__1 = *m1 + *n1 + *n2 + *n3 + mmax, i__2 = *m1 + *n - 2;
    nn4 = min(i__1,i__2);
    if (nn4 > *n) {
	nn4 -= *n;
    }
    sortst_(n, &angle[1], &mm4, &nn4, &irange, &indst[1]);
/* ---PUSH ANGLE INDEX ON FIFTH STACK IN DESCENDING ORDER */
    i__1 = *n - 2;
    skinit_(&ist5[1], n, &i__1, &ierror);
    for (i__ = min(10,irange); i__ >= 1; --i__) {
	skpush_(&ist5[1], n, &indst[i__], &ierror);
/* L180: */
    }
/* ---POP TOP OF FIFTH STACK */
L190:
    skpop_(&ist5[1], n, &m5, &ierror);
    if (ierror == 2) {
	goto L170;
    }
    if (sum4 + angle[m5] > sumang) {
	goto L170;
    }
    *n4 = m5 - m4;
    if (*n4 < 0) {
	*n4 = *n + *n4;
    }
    n5 = *m1 - m5;
    if (n5 < 0) {
	n5 = *n + n5;
    }
/* --- CHECK COMPATIBILITY EQUATIONS */
    if (*n1 + *n2 + *n3 < *n4 + n5 + 2 || *n2 + *n3 + *n4 < n5 + *n1 + 2 || *
	    n3 + *n4 + n5 < *n1 + *n2 + 2 || *n4 + n5 + *n1 < *n2 + *n3 + 2 ||
	     n5 + *n1 + *n2 < *n3 + *n4 + 2) {
    } else {
	sumang = sum4 + angle[m5];
	m1hold = *m1;
	n1hold = *n1;
	n2hold = *n2;
	n3hold = *n3;
	n4hold = *n4;
    }
    goto L190;
L200:
    if (sumang == (float)99999.) {
	*m1 = 0;
    } else {
	*m1 = m1hold;
	*n1 = n1hold;
	*n2 = n2hold;
	*n3 = n3hold;
	*n4 = n4hold;
    }
    return 0;
} /* pickm5_ */

#ifdef __cplusplus
	}
#endif

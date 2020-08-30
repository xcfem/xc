/* getm3.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getm3_(integer *ml, integer *ms, integer *mnnps, integer 
	*ns, integer *islist, integer *nint, integer *ifline, integer *nlps, 
	integer *illist, integer *linkl, integer *links, real *x, real *y, 
	integer *nid, integer *nnps, real *angle, integer *nper, integer *m1a,
	 integer *m1b, integer *m2a, integer *m2b, integer *m3a, integer *m3b,
	 real *xcen, real *ycen, logical *ccw, logical *err)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real d1, d2, d3;
    static integer i1, i2, i3, j1, m1, m2, m3, j2, j3;
    extern /* Subroutine */ int fq_rotate__(integer *, real *, real *, 
	    integer *, integer *);
    static real x1, y1, x2, y2, x3, y3, d1a, d1b, d2a, d2b, d3a, d3b;
    extern /* Subroutine */ int nps_(integer *, integer *, integer *, integer 
	    *, integer *, integer *, integer *, integer *, integer *, integer 
	    *, integer *, integer *, logical *);
    static real pro1, pro2, pro3;
    static integer mmax, mhold;
    extern /* Subroutine */ int pickm3_(integer *, real *, real *, real *, 
	    integer *, integer *, integer *), mesage_(char *, ftnlen), 
	    irever_(integer *, integer *);
    static integer ifirst;

/* *********************************************************************** */
/*  SUBROUTINE GETM3 = GETS THE APPROPRIATE SIDE LENGTHS AND DIVISIONS */
/*                     FOR A TRIANGULAR REGION */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     QMESH = GENERATES QUAD ELEMENTS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NNPS  = ARRAY OF NUMBER OF NODES PER SIDE */
/*     CCW   = .TRUE. IF THE SIDE IS ORIENTED CCW */
/*     NORM  = .TRUE. IF THE FIRST SIDE IS TO BE TRIED AS THE BASE */
/* *********************************************************************** */
/*  CALCULATE THE NUMBER OF NODES PER SIDE */
    /* Parameter adjustments */
    linkl -= 3;
    --nint;
    --links;
    --illist;
    --nlps;
    --ifline;
    --nnps;
    --islist;
    --angle;
    --nid;
    --y;
    --x;

    /* Function Body */
    nps_(ml, ms, mnnps, ns, &islist[1], &nint[1], &ifline[1], &nlps[1], &
	    illist[1], &linkl[3], &links[1], &nnps[1], err);
    if (*err) {
	return 0;
    }
    if (! (*ccw)) {
	irever_(&nnps[1], ns);
    }
/*  FIND THE BEST CORNER NODES IN THE LIST */
    pickm3_(nper, &x[1], &y[1], &angle[1], &m1, &m2, &ifirst);
    if (ifirst != 1) {
	fq_rotate__(nper, &x[1], &y[1], &nid[1], &ifirst);
    }
/*  NOW SORT THE LIST SO THE LONGEST SIDE IS FIRST */
    m3 = *nper - m1 - m2;
/* Computing MAX */
    i__1 = max(m1,m2);
    mmax = max(i__1,m3);
    if (m1 == mmax) {
    } else if (m2 == mmax) {
	i__1 = m1 + 1;
	fq_rotate__(nper, &x[1], &y[1], &nid[1], &i__1);
	mhold = m1;
	m1 = m2;
	m2 = m3;
	m3 = mhold;
    } else if (m3 == mmax) {
	i__1 = m1 + m2 + 1;
	fq_rotate__(nper, &x[1], &y[1], &nid[1], &i__1);
	mhold = m1;
	m1 = m3;
	m3 = m2;
	m2 = mhold;
    }
/*  SPLIT THE SIDES INTO LOGICAL DIVISIONS */
/*      IF (M2 .EQ. M3)THEN */
/*         M1A = (.5 * DBLE(M1)) + .001 */
/*         M1B = M1A */
/*      ELSEIF (M2 .LT. M3)THEN */
/*         FACT = DBLE(M2 - 1) / DBLE(M2 + M3 - 2) */
/*         M1A = FACT * M1 + .001 */
/*         M1A = MAX0 (M1A, 1) */
/*         M1A = MIN0 (M1A, M1-1) */
/*         M1B = M1 - M1A */
/*      ELSE */
/*         FACT = DBLE(M3 - 1) / DBLE(M2 + M3 - 2) */
/*         M1B = FACT * M1 + .001 */
/*         M1B = MAX0 (M1B, 1) */
/*         M1B = MIN0 (M1B, M1-1) */
/*         M1A = M1-M1B */
/*      ENDIF */
    *m1a = (m1 + m2 - m3) / 2;
    *m1b = m1 - *m1a;
    *m2a = m2 - *m1a;
    *m2b = *m1a;
    *m3a = *m1b;
    *m3b = m3 - *m3a;
    *err = TRUE_;
    if (*m3b != *m2a) {
	mesage_("ERROR GENERATING TRIANGLE DIVISION POINT", (ftnlen)40);
	return 0;
    }
/*  DEFINE THE MIDDLE POINT AS THE AVERAGE OF PROPORIONAL DIVISIONS */
/*  OF SIDE DIVISION POINT TO OPPOSITE TRIANGLE CORNER LINES */
    i1 = 1;
    i2 = m1 + 1;
    i3 = m1 + m2 + 1;
    j1 = i1 + *m1a;
    j2 = i2 + *m2a;
    j3 = i3 + *m3a;
/*  FIND DISTANCES FROM CORNER TO CORNER,  AND CORNERS TO SIDE DIVISIONS */
/* Computing 2nd power */
    r__1 = x[i2] - x[i1];
/* Computing 2nd power */
    r__2 = y[i2] - y[i1];
    d1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[i3] - x[i2];
/* Computing 2nd power */
    r__2 = y[i3] - y[i2];
    d2 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[i1] - x[i3];
/* Computing 2nd power */
    r__2 = y[i1] - y[i3];
    d3 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[j1] - x[i1];
/* Computing 2nd power */
    r__2 = y[j1] - y[i1];
    d1a = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[i2] - x[j1];
/* Computing 2nd power */
    r__2 = y[i2] - y[j1];
    d1b = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[j2] - x[i2];
/* Computing 2nd power */
    r__2 = y[j2] - y[i2];
    d2a = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[i3] - x[j2];
/* Computing 2nd power */
    r__2 = y[i3] - y[j2];
    d2b = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[j3] - x[i3];
/* Computing 2nd power */
    r__2 = y[j3] - y[i3];
    d3a = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[i1] - x[j3];
/* Computing 2nd power */
    r__2 = y[i1] - y[j3];
    d3b = sqrt(r__1 * r__1 + r__2 * r__2);
/*  GET MIDPOINT TRIALS 1,  2,  AND 3 AS PROPORTIONS */
    pro1 = (d3a / d3 + d1b / d1) * (float).5;
    x1 = x[j2] - pro1 * (x[j2] - x[i1]);
    y1 = y[j2] - pro1 * (y[j2] - y[i1]);
    pro2 = (d2b / d2 + d1a / d1) * (float).5;
    x2 = x[j3] - pro2 * (x[j3] - x[i2]);
    y2 = y[j3] - pro2 * (y[j3] - y[i2]);
    pro3 = (d2a / d2 + d3b / d3) * (float).5;
    x3 = x[j1] - pro3 * (x[j1] - x[i3]);
    y3 = y[j1] - pro3 * (y[j1] - y[i3]);
/*  AVERAGE POINTS TO GET THE CENTER */
    *xcen = (x1 + x2 + x3) / (float)3.;
    *ycen = (y1 + y2 + y3) / (float)3.;
    *err = FALSE_;
    return 0;
} /* getm3_ */

#ifdef __cplusplus
	}
#endif

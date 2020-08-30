/* getm5.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getm5_(integer *ia, integer *ml, integer *ms, integer *
	mnnps, integer *ns, integer *islist, integer *nint, integer *ifline, 
	integer *nlps, integer *illist, integer *linkl, integer *links, real *
	x, real *y, integer *nid, integer *nnps, real *angle, integer *nper, 
	integer *m1a, integer *m1b, integer *m2, integer *m3a, integer *m3b, 
	integer *m4a, integer *m4b, integer *m5, integer *mc, real *xcen, 
	real *ycen, logical *ccw, logical *err)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real d1, d2, d3;
    static integer i1, i2, i3, m1, m3, m4;
    extern /* Subroutine */ int fq_rotate__(integer *, real *, real *, 
	    integer *, integer *);
    static real x1, y1, x2, y2, x3, y3;
    static integer kk;
    static real xj[3], yj[3], d1a, d1b, d2a, d2b, d3a, d3b;
    static integer ip2, ip3, ip4, ip5;
    extern /* Subroutine */ int nps_(integer *, integer *, integer *, integer 
	    *, integer *, integer *, integer *, integer *, integer *, integer 
	    *, integer *, integer *, logical *);
    static real pro1, pro2, pro3;
    static integer indp, mmax, knum;
    extern /* Subroutine */ int mddel_(char *, ftnlen);
    static integer mhold;
    extern /* Subroutine */ int pickm5_(integer *, real *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *), mesage_(char *, 
	    ftnlen), irever_(integer *, integer *);
    static integer ifirst;
    extern /* Subroutine */ int mdrsrv_(char *, integer *, integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE GETM5 = GETS THE APPROPRIATE SIDE LENGTHS AND DIVISIONS */
/*                      FOR A PENTAGON REGION */
/*  WRITTEN BY: HORACIO RECALDE                   DATE: JAN 1988 */
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
    --ia;
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
/*   RESERVE MEMORY FOR THE STACKS */
    mdrsrv_("IST2", &ip2, nper, (ftnlen)4);
    mdrsrv_("IST3", &ip3, nper, (ftnlen)4);
    mdrsrv_("IST4", &ip4, nper, (ftnlen)4);
    mdrsrv_("IST5", &ip5, nper, (ftnlen)4);
    mdrsrv_("INDST", &indp, nper, (ftnlen)5);
/*  FIND THE BEST CORNER NODES IN THE LIST */
    pickm5_(nper, &x[1], &y[1], &angle[1], &ia[ip2], &ia[ip3], &ia[ip4], &ia[
	    ip5], &ia[indp], &ifirst, &m1, m2, &m3, &m4);
    if (ifirst == 0) {
	*err = TRUE_;
	mesage_("ERROR FITTING LOGICAL PENTAGON TO DATA", (ftnlen)38);
	return 0;
    } else if (ifirst == -1) {
	*err = TRUE_;
	mesage_("TOLERANCE EXCEEDED", (ftnlen)18);
	return 0;
    } else if (ifirst != 1) {
	fq_rotate__(nper, &x[1], &y[1], &nid[1], &ifirst);
    }
/*  DELETE THE STACKS */
    mddel_("IST2", (ftnlen)4);
    mddel_("IST3", (ftnlen)4);
    mddel_("IST4", (ftnlen)4);
    mddel_("IST5", (ftnlen)4);
    mddel_("INDST", (ftnlen)5);
/*  NOW SORT THE LIST SO THE LONGEST SIDE IS FIRST */
    *m5 = *nper - m1 - *m2 - m3 - m4;
/* Computing MAX */
    i__1 = max(m1,*m2), i__1 = max(i__1,m3), i__1 = max(i__1,m4);
    mmax = max(i__1,*m5);
    if (m1 == mmax) {
	knum = 0;
    } else if (*m2 == mmax) {
	i__1 = m1 + 1;
	fq_rotate__(nper, &x[1], &y[1], &nid[1], &i__1);
	knum = 1;
    } else if (m3 == mmax) {
	i__1 = m1 + *m2 + 1;
	fq_rotate__(nper, &x[1], &y[1], &nid[1], &i__1);
	knum = 2;
    } else if (m4 == mmax) {
	i__1 = m1 + *m2 + m3 + 1;
	fq_rotate__(nper, &x[1], &y[1], &nid[1], &i__1);
	knum = 3;
    } else if (*m5 == mmax) {
	i__1 = m1 + *m2 + m3 + m4 + 1;
	fq_rotate__(nper, &x[1], &y[1], &nid[1], &i__1);
	knum = 4;
    }
    i__1 = knum;
    for (kk = 1; kk <= i__1; ++kk) {
	mhold = m1;
	m1 = *m2;
	*m2 = m3;
	m3 = m4;
	m4 = *m5;
	*m5 = mhold;
/* L100: */
    }
/*  SPLIT THE SIDES INTO LOGICAL DIVISIONS */
    *m1a = (m1 + m4 + *m5 - *m2 - m3) / 2;
    *m1b = (m1 + *m2 + m3 - m4 - *m5) / 2;
    *m3a = *m1b;
    *m3b = (m3 + m4 + *m5 - m1 - *m2) / 2;
    *m4a = (*m2 + m3 + m4 - m1 - *m5) / 2;
    *m4b = *m1a;
    *mc = (m1 + *m2 + *m5 - m3 - m4) / 2;
/*  DEFINE THE MIDDLE POINT AS THE AVERAGE OF PROPORIONAL DIVISIONS */
/*  OF SIDE DIVISION POINT TO OPPOSITE TRIANGLE CORNER LINES */
    i1 = *m1a + 1;
    i2 = i1 + *m1b + *m2 + *m3a;
    i3 = i2 + *m3b + *m4a;
/*  FIND DISTANCES FROM CORNER TO CORNER, AND CORNERS TO SIDE DIVISIONS */
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
    d1a = (doublereal) (*m4b) * d1 / (doublereal) m4;
    d1b = d1 - d1a;
    d2a = (doublereal) (*m1b) * d2 / (doublereal) m1;
    d2b = d2 - d1a;
    d3a = (doublereal) (*m3b) * d3 / (doublereal) m3;
    d3b = d3 - d3a;
    xj[0] = x[i1] + (x[i2] - x[i1]) * d1a / d1;
    yj[0] = y[i1] + (y[i2] - y[i1]) * d1a / d1;
    xj[1] = x[i2] + (x[i3] - x[i2]) * d2a / d2;
    yj[1] = y[i2] + (y[i3] - y[i2]) * d2a / d2;
    xj[2] = x[i3] + (x[i1] - x[i3]) * d3a / d3;
    yj[2] = y[i3] + (y[i1] - y[i3]) * d3a / d3;
/*  GET MIDPOINT TRIALS 1, 2, AND 3 AS PROPORTIONS */
    pro1 = (d3a / d3 + d1b / d1) * (float).5;
    x1 = xj[1] - pro1 * (xj[1] - x[i1]);
    y1 = yj[1] - pro1 * (yj[1] - y[i1]);
    pro2 = (d2b / d2 + d1a / d1) * (float).5;
    x2 = xj[2] - pro2 * (xj[2] - x[i2]);
    y2 = yj[2] - pro2 * (yj[2] - y[i2]);
    pro3 = (d2a / d2 + d3b / d3) * (float).5;
    x3 = xj[0] - pro3 * (xj[0] - x[i3]);
    y3 = yj[0] - pro3 * (yj[0] - y[i3]);
/*  AVERAGE POINTS TO GET THE CENTER */
    *xcen = (x1 + x2 + x3) / (float)3.;
    *ycen = (y1 + y2 + y3) / (float)3.;
    *err = FALSE_;
    return 0;
} /* getm5_ */

#ifdef __cplusplus
	}
#endif

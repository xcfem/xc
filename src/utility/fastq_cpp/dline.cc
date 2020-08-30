/* dline.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int dline_(integer *mp, integer *ml, real *coor, integer *
	linkp, integer *knum, integer *kt, integer *ip1, integer *ip2, 
	integer *ip3, logical *numplt, real *x1, real *y1, logical *test, 
	logical *getmax, real *xmin, real *xmax, real *ymin, real *ymax)
{
    /* Format strings */
    static char fmt_10000[] = "(a2,i6,a1)";
    static char fmt_10010[] = "(a5,i10,a1,i10,a6,i10,a1,i10,a1)";
    static char fmt_10040[] = "(\002 POINTS GIVEN FOR LINE\002,i5,\002 DO NO\
T DEFINE A PARABOLA\002)";
    static char fmt_10020[] = "(\002 ZERO LINE LENGTH ENCOUNTERED FOR LIN\
E\002,i5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;
    real r__1, r__2, r__3, r__4, r__5, r__6;

    /* Builtin functions */
    double atan2(doublereal, doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    double log(doublereal), exp(doublereal), cos(doublereal), sin(doublereal),
	     sqrt(doublereal);

    /* Local variables */
    static integer i__, n;
    static real r1, r2, x2, y2, fl, fm, fr, pi, xa, xk, xl, yk, xr, fac, dff, 
	    del, ang;
    static integer inc, len;
    static real dot;
    static logical err;
    static real darc, frac, coef;
    static integer iend;
    extern doublereal parc_(real *, real *);
    static real tang;
    static integer mask;
    static real delx, xcen, ycen, xmid, ymid, perp, cost, sint, rotx, roty;
    static integer idum1, idum2;
    extern /* Subroutine */ int mp2pt_(integer *, real *, real *, real *, 
	    real *, integer *);
    static real tcoef, halfw, theta, pleft, cross;
    static char dummy[72];
    static integer kount;
    static real theta1, theta2;
    extern /* Subroutine */ int mpd2vc_(integer *, real *, real *, real *, 
	    real *);
    static integer ipntr1, ipntr2, ipntr3;
    static real arcdel;
    static logical addlnk;
    static real arcold;
    extern /* Subroutine */ int arcpar_(integer *, integer *, integer *, real 
	    *, integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, real *, real *, real *, real *, logical *, 
	    integer *, integer *, real *, real *);
    static real height, avalue, bvalue;
    extern /* Subroutine */ int getdum_(integer *, char *, integer *, ftnlen);
    static real radius, arcnow, arctot, arcnxt;
    extern /* Subroutine */ int elpspr_(integer *, integer *, integer *, real 
	    *, integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, real *, real *, integer *, integer *, real *, 
	    real *, logical *), pltflu_(), pltxth_(real *, real *, char *, 
	    ftnlen), ltsort_(integer *, integer *, integer *, integer *, 
	    logical *);

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 12, 0, fmt_10000, 0 };
    static cilist io___11 = { 0, 12, 0, fmt_10010, 0 };
    static cilist io___31 = { 0, 12, 0, fmt_10010, 0 };
    static cilist io___34 = { 0, 12, 0, fmt_10010, 0 };
    static cilist io___40 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___42 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___67 = { 0, 12, 0, fmt_10010, 0 };
    static cilist io___68 = { 0, 12, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE DLINE = DRAWS A LINE ACCORDING TO THE CURRENT DEFINITION */
/*                     OR SIMPLY GETS THE MAX/MIN FOR LINES GETMAX=.TRUE. */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     IP1   = POINTER FOR THE FIRST POINT */
/*     IP2   = POINTER FOR THE SECOND POINT */
/*     IP3   = POINTER FOR THE THIRD POINT */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    if (*test) {
	s_wsfe(&io___2);
	do_fio(&c__1, "SP", (ftnlen)2);
	do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	do_fio(&c__1, ";", (ftnlen)1);
	e_wsfe();
    }
    addlnk = FALSE_;
/*  DEFINE FIRST POINT EXACTLY AND BRANCH */
    ltsort_(mp, &linkp[3], ip1, &ipntr1, &addlnk);
    ltsort_(mp, &linkp[3], ip2, &ipntr2, &addlnk);
    if (ipntr1 <= 0 || ipntr2 <= 0) {
	goto L140;
    }
    if (*ip3 != 0) {
	i__1 = abs(*ip3);
	ltsort_(mp, &linkp[3], &i__1, &ipntr3, &addlnk);
	if (ipntr3 <= 0) {
	    goto L140;
	}
    } else {
	ipntr3 = 0;
    }
    *x1 = coor[(ipntr1 << 1) + 1];
    *y1 = coor[(ipntr1 << 1) + 2];
/*  STRAIGHT LINE GENERATION */
    if (*kt == 1) {
	x2 = coor[(ipntr2 << 1) + 1];
	y2 = coor[(ipntr2 << 1) + 2];
	if (*getmax) {
/* Computing MAX */
	    r__1 = max(*x1,x2);
	    *xmax = dmax(r__1,*xmax);
/* Computing MAX */
	    r__1 = max(*y1,y2);
	    *ymax = dmax(r__1,*ymax);
/* Computing MIN */
	    r__1 = min(*x1,x2);
	    *xmin = dmin(r__1,*xmin);
/* Computing MIN */
	    r__1 = min(*y1,y2);
	    *ymin = dmin(r__1,*ymin);
	    goto L140;
	}
	xmid = (*x1 + x2) * (float).5;
	ymid = (*y1 + y2) * (float).5;
/*  CORNER GENERATION */
    } else if (*kt == 2) {
	x2 = coor[(ipntr3 << 1) + 1];
	y2 = coor[(ipntr3 << 1) + 2];
	if (*getmax) {
/* Computing MAX */
	    r__1 = max(*x1,x2), r__2 = coor[(ipntr2 << 1) + 1], r__1 = max(
		    r__1,r__2);
	    *xmax = dmax(r__1,*xmax);
/* Computing MAX */
	    r__1 = max(*y1,y2), r__2 = coor[(ipntr2 << 1) + 2], r__1 = max(
		    r__1,r__2);
	    *ymax = dmax(r__1,*ymax);
/* Computing MIN */
	    r__1 = min(*x1,x2), r__2 = coor[(ipntr2 << 1) + 1], r__1 = min(
		    r__1,r__2);
	    *xmin = dmin(r__1,*xmin);
/* Computing MIN */
	    r__1 = min(*y1,y2), r__2 = coor[(ipntr2 << 1) + 2], r__1 = min(
		    r__1,r__2);
	    *ymin = dmin(r__1,*ymin);
	    goto L140;
	}
	if (*test) {
	    s_wsfe(&io___11);
	    do_fio(&c__1, "PU;PA", (ftnlen)5);
	    i__1 = (integer) (*x1 * (float)1e3);
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, ", ", (ftnlen)2);
	    i__2 = (integer) (*y1 * (float)1e3);
	    do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
	    do_fio(&c__1, ";PD;PA", (ftnlen)6);
	    i__3 = (integer) (x2 * (float)1e3);
	    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
	    do_fio(&c__1, ", ", (ftnlen)2);
	    i__4 = (integer) (y2 * (float)1e3);
	    do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
	    do_fio(&c__1, ";", (ftnlen)1);
	    e_wsfe();
	}
	mpd2vc_(&c__1, x1, y1, &x2, &y2);
	xmid = *x1 + (x2 - *x1) * (float).25;
	ymid = *y1 + (y2 - *y1) * (float).25;
	*x1 = x2;
	*y1 = y2;
	x2 = coor[(ipntr2 << 1) + 1];
	y2 = coor[(ipntr2 << 1) + 2];
/*  CIRCULAR ARC GENERATION */
    } else if (*kt == 3 || *kt == 4 || *kt == 6) {
	arcpar_(mp, kt, knum, &coor[3], &linkp[3], &ipntr1, &ipntr2, &ipntr3, 
		ip3, &xcen, &ycen, &theta1, &theta2, &tang, &r1, &r2, &err, &
		idum1, &idum2, &xk, &xa);
	if (err) {
	    goto L140;
	}
/*  GENERATE THE CIRCLE */
	ang = theta1;
	darc = (float).1;
	inc = (integer) (dabs(tang) / darc) + 1;
	if (inc <= 6) {
	    inc = 6;
	}
	del = tang * ((float)1. / (doublereal) inc);
	iend = inc - 1;
	xk = log(r2 / r1) / (theta2 - theta1);
	xa = r2 / exp(xk * theta2);
	if (*getmax) {
	    *xmax = dmax(*x1,*xmax);
	    *ymax = dmax(*y1,*ymax);
	    *xmin = dmin(*x1,*xmin);
	    *ymin = dmin(*y1,*ymin);
	}
	i__1 = iend;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ang += del;
	    radius = xa * exp(xk * ang);
	    x2 = xcen + cos(ang) * radius;
	    y2 = ycen + sin(ang) * radius;
	    if (*getmax) {
		*xmax = dmax(x2,*xmax);
		*ymax = dmax(y2,*ymax);
		*xmin = dmin(x2,*xmin);
		*ymin = dmin(y2,*ymin);
	    } else {
		mpd2vc_(&c__1, x1, y1, &x2, &y2);
	    }
	    if (*test) {
		s_wsfe(&io___31);
		do_fio(&c__1, "PU;PA", (ftnlen)5);
		i__2 = (integer) (*x1 * (float)1e3);
		do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
		do_fio(&c__1, ", ", (ftnlen)2);
		i__3 = (integer) (*y1 * (float)1e3);
		do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		do_fio(&c__1, ";PD;PA", (ftnlen)6);
		i__4 = (integer) (x2 * (float)1e3);
		do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
		do_fio(&c__1, ", ", (ftnlen)2);
		i__5 = (integer) (y2 * (float)1e3);
		do_fio(&c__1, (char *)&i__5, (ftnlen)sizeof(integer));
		do_fio(&c__1, ";", (ftnlen)1);
		e_wsfe();
	    }
	    *x1 = x2;
	    *y1 = y2;
	    if (i__ == inc / 2) {
		xmid = *x1;
		ymid = *y1;
	    }
/* L100: */
	}
/*  ELIPSE GENERATION */
    } else if (*kt == 7) {
	elpspr_(mp, kt, knum, &coor[3], &linkp[3], &ipntr1, &ipntr2, &ipntr3, 
		ip3, &xcen, &ycen, &theta1, &theta2, &tang, &idum1, &idum2, &
		avalue, &bvalue, &err);
	if (err) {
	    goto L140;
	}
/*  GENERATE THE ELIPSE */
	if (*getmax) {
	    *xmax = dmax(*x1,*xmax);
	    *ymax = dmax(*y1,*ymax);
	    *xmin = dmin(*x1,*xmin);
	    *ymin = dmin(*y1,*ymin);
	}
	darc = (float).1;
/* Computing MAX */
	i__1 = (integer) (dabs(tang) / darc) + 1;
	inc = max(i__1,15);
	del = tang * ((float)1. / (doublereal) inc);
	iend = inc - 1;
	ang = theta1;
	i__1 = iend;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ang += del;
/* Computing 2nd power */
	    r__1 = avalue;
/* Computing 2nd power */
	    r__2 = bvalue;
/* Computing 2nd power */
	    r__3 = bvalue;
/* Computing 2nd power */
	    r__4 = cos(ang);
/* Computing 2nd power */
	    r__5 = avalue;
/* Computing 2nd power */
	    r__6 = sin(ang);
	    radius = sqrt(r__1 * r__1 * (r__2 * r__2) / (r__3 * r__3 * (r__4 *
		     r__4) + r__5 * r__5 * (r__6 * r__6)));
	    x2 = xcen + cos(ang) * radius;
	    y2 = ycen + sin(ang) * radius;
	    if (*getmax) {
		*xmax = dmax(x2,*xmax);
		*ymax = dmax(y2,*ymax);
		*xmin = dmin(x2,*xmin);
		*ymin = dmin(y2,*ymin);
	    } else {
		mpd2vc_(&c__1, x1, y1, &x2, &y2);
	    }
	    if (*test) {
		s_wsfe(&io___34);
		do_fio(&c__1, "PU;PA", (ftnlen)5);
		i__2 = (integer) (*x1 * (float)1e3);
		do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
		do_fio(&c__1, ", ", (ftnlen)2);
		i__3 = (integer) (*y1 * (float)1e3);
		do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		do_fio(&c__1, ";PD;PA", (ftnlen)6);
		i__4 = (integer) (x2 * (float)1e3);
		do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
		do_fio(&c__1, ", ", (ftnlen)2);
		i__5 = (integer) (y2 * (float)1e3);
		do_fio(&c__1, (char *)&i__5, (ftnlen)sizeof(integer));
		do_fio(&c__1, ";", (ftnlen)1);
		e_wsfe();
	    }
	    *x1 = x2;
	    *y1 = y2;
	    if (i__ == inc / 2) {
		xmid = *x1;
		ymid = *y1;
	    }
/* L110: */
	}
/*  PARABOLA */
    } else if (*kt == 5) {
	n = 50;
	fac = (float)1.;
	dff = (float).02;
/*  CHECK LEGITIMACY OF DATA */
	xmid = (coor[(ipntr1 << 1) + 1] + coor[(ipntr2 << 1) + 1]) * (float)
		.5;
	ymid = (coor[(ipntr1 << 1) + 2] + coor[(ipntr2 << 1) + 2]) * (float)
		.5;
	dot = (coor[(ipntr2 << 1) + 1] - coor[(ipntr1 << 1) + 1]) * (coor[(
		ipntr3 << 1) + 1] - xmid) + (coor[(ipntr2 << 1) + 2] - coor[(
		ipntr1 << 1) + 2]) * (coor[(ipntr3 << 1) + 2] - ymid);
/* Computing 2nd power */
	r__1 = coor[(ipntr2 << 1) + 1] - coor[(ipntr1 << 1) + 1];
/* Computing 2nd power */
	r__2 = coor[(ipntr2 << 1) + 2] - coor[(ipntr1 << 1) + 2];
/* Computing 2nd power */
	r__3 = coor[(ipntr3 << 1) + 1] - xmid;
/* Computing 2nd power */
	r__4 = coor[(ipntr3 << 1) + 2] - ymid;
	perp = sqrt(r__1 * r__1 + r__2 * r__2) * sqrt(r__3 * r__3 + r__4 * 
		r__4);
	if (dot >= perp * (float).05) {
	    pltflu_();
	    s_wsfe(&io___40);
	    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	    e_wsfe();
	    goto L140;
	}
	if (*getmax) {
	    *xmax = dmax(*x1,*xmax);
	    *ymax = dmax(*y1,*ymax);
	    *xmin = dmin(*x1,*xmin);
	    *ymin = dmin(*y1,*ymin);
	}
/*  GET ARC LENGTH */
/* Computing 2nd power */
	r__1 = coor[(ipntr2 << 1) + 1] - coor[(ipntr1 << 1) + 1];
/* Computing 2nd power */
	r__2 = coor[(ipntr2 << 1) + 2] - coor[(ipntr1 << 1) + 2];
	halfw = sqrt(r__1 * r__1 + r__2 * r__2) * (float).5;
	if (halfw == (float)0.) {
	    pltflu_();
	    s_wsfe(&io___42);
	    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	    e_wsfe();
	    goto L140;
	}
/* Computing 2nd power */
	r__1 = xmid - coor[(ipntr3 << 1) + 1];
/* Computing 2nd power */
	r__2 = ymid - coor[(ipntr3 << 1) + 2];
	height = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = halfw;
	coef = height / (r__1 * r__1);
	tcoef = coef * (float)2.;
/*  PARC IS A STATEMENT FUNCTION */
	r__1 = -tcoef * halfw;
	pleft = parc_(&r__1, &tcoef);
	r__1 = tcoef * halfw;
	arctot = parc_(&r__1, &tcoef) * (float)2.;
	arcdel = dff * arctot;
	arcnxt = arcdel;
	arcnow = (float)0.;
	theta = atan2(coor[(ipntr2 << 1) + 2] - coor[(ipntr1 << 1) + 2], coor[
		(ipntr2 << 1) + 1] - coor[(ipntr1 << 1) + 1]);
/*  CORRECT FOR ORIENTATION */
	cross = (coor[(ipntr3 << 1) + 1] - xmid) * (coor[(ipntr2 << 1) + 2] - 
		coor[(ipntr1 << 1) + 2]) - (coor[(ipntr3 << 1) + 2] - ymid) * 
		(coor[(ipntr2 << 1) + 1] - coor[(ipntr1 << 1) + 1]);
	if (cross < (float)0.) {
	    theta += pi;
	}
	sint = sin(theta);
	cost = cos(theta);
/*  FIND POINTS APPROXIMATELY BY INTEGRATION */
	xl = -halfw;
/* Computing 2nd power */
	r__1 = tcoef * xl;
	fl = sqrt(r__1 * r__1 + (float)1.);
	kount = 1;
	delx = halfw * (float)2. / (float)200.;
	for (i__ = 1; i__ <= 100; ++i__) {
/* Computing 2nd power */
	    r__1 = tcoef * (xl + delx);
	    fm = sqrt(r__1 * r__1 + (float)1.);
	    xr = -halfw + (doublereal) i__ * (float)2. * delx;
/* Computing 2nd power */
	    r__1 = tcoef * xr;
	    fr = sqrt(r__1 * r__1 + (float)1.);
	    arcold = arcnow;
	    arcnow += delx * (fl + fm * (float)4. + fr) / (float)3.;
	    if (arcnow >= arcnxt) {
/*  COMPUTE POSITION IN LOCAL COORDINATE SYSTEM */
		frac = (arcnxt - arcold) / (arcnow - arcold);
		xk = xl + frac * (float)2. * delx;
/* Computing 2nd power */
		r__1 = xk;
		yk = coef * (r__1 * r__1);
/*  CORRECT FOR ORIENTATION PROBLEM */
		if (cross < (float)0.) {
		    xk = -xk;
		}
/*  ROTATE IN LINE WITH GLOBAL COORDINATE SYSTEM */
		rotx = xk * cost - yk * sint;
		roty = yk * cost + xk * sint;
/*  RESTORE XK */
		if (cross < (float)0.) {
		    xk = -xk;
		}
/*  TRANSLATE */
		++kount;
		x2 = rotx + coor[(ipntr3 << 1) + 1];
		y2 = roty + coor[(ipntr3 << 1) + 2];
		if (*test) {
		    s_wsfe(&io___67);
		    do_fio(&c__1, "PU;PA", (ftnlen)5);
		    i__1 = (integer) (*x1 * (float)1e3);
		    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
		    do_fio(&c__1, ", ", (ftnlen)2);
		    i__2 = (integer) (*y1 * (float)1e3);
		    do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
		    do_fio(&c__1, ";PD;PA", (ftnlen)6);
		    i__3 = (integer) (x2 * (float)1e3);
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    do_fio(&c__1, ", ", (ftnlen)2);
		    i__4 = (integer) (y2 * (float)1e3);
		    do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
		    do_fio(&c__1, ";", (ftnlen)1);
		    e_wsfe();
		}
		if (*getmax) {
		    *xmax = dmax(x2,*xmax);
		    *ymax = dmax(y2,*ymax);
		    *xmin = dmin(x2,*xmin);
		    *ymin = dmin(y2,*ymin);
		} else {
		    mpd2vc_(&c__1, x1, y1, &x2, &y2);
		}
		*x1 = x2;
		*y1 = y2;
/*  PREPARE FOR NEXT POINT */
		if (kount >= n - 1) {
		    goto L130;
		}
		arcdel *= fac;
		arcnxt += arcdel;
/*  RESTART INTEGRATION */
		xr = xk;
/* Computing 2nd power */
		r__1 = tcoef * xr;
		fr = sqrt(r__1 * r__1 + (float)1.);
/*  CORRECT FOR INTEGRATION ERROR */
		r__1 = tcoef * xr;
		arcnow = parc_(&r__1, &tcoef) - pleft;
	    }
	    xl = xr;
	    fl = fr;
/* L120: */
	}
L130:
	xmid = coor[(ipntr3 << 1) + 1];
	ymid = coor[(ipntr3 << 1) + 2];
    }
/*     NORMAL EXIT */
/*     DEFINE LAST POINT EXACTLY */
    x2 = coor[(ipntr2 << 1) + 1];
    y2 = coor[(ipntr2 << 1) + 2];
    if (*getmax) {
	*xmax = dmax(x2,*xmax);
	*ymax = dmax(y2,*ymax);
	*xmin = dmin(x2,*xmin);
	*ymin = dmin(y2,*ymin);
	goto L140;
    }
    if (*test) {
	s_wsfe(&io___68);
	do_fio(&c__1, "PU;PA", (ftnlen)5);
	i__1 = (integer) (*x1 * (float)1e3);
	do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	do_fio(&c__1, ", ", (ftnlen)2);
	i__2 = (integer) (*y1 * (float)1e3);
	do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
	do_fio(&c__1, ";PD;PA", (ftnlen)6);
	i__3 = (integer) (x2 * (float)1e3);
	do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
	do_fio(&c__1, ", ", (ftnlen)2);
	i__4 = (integer) (y2 * (float)1e3);
	do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
	do_fio(&c__1, ";", (ftnlen)1);
	e_wsfe();
    }
    mpd2vc_(&c__1, x1, y1, &x2, &y2);
    pltflu_();
/*  PLOT THE LINE NUMBER IF DESIRED */
    if (*knum > 0) {
	mp2pt_(&c__1, &xmid, &ymid, x1, y1, &mask);
	if (mask % 2 != 0 && *numplt) {
	    getdum_(knum, dummy, &len, (ftnlen)72);
	    pltxth_(x1, y1, dummy, len);
	}
    }
L140:
    return 0;
} /* dline_ */

#ifdef __cplusplus
	}
#endif

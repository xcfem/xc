/* linlen.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int linlen_(integer *mp, real *coor, integer *linkp, integer 
	*knum, integer *lnum, integer *kt, integer *i3, integer *j1, integer *
	j2, integer *j3, real *dist, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 ZERO LINE LENGTH ENCOUNTERED FOR LIN\
E\002,i5)";
    static char fmt_10030[] = "(\002 POINTS GIVEN FOR LINE\002,i5,\002 DO NO\
T DEFINE A PARABOLA\002)";

    /* System generated locals */
    real r__1, r__2, r__3, r__4, r__5, r__6;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    double exp(doublereal), cos(doublereal), sin(doublereal);

    /* Local variables */
    static integer i__;
    static real r1, r2, da, db, fl, fm, fr, pi, xa, xk, xl, yk, xr, del, ang, 
	    xda, yda, xdb, ydb;
    static integer icw;
    static real dot, frac, coef;
    static integer iccw;
    extern doublereal parc_(real *, real *);
    static real tang, delx, xcen, ycen, xend, yend, xmid, ymid, perp, cost, 
	    sint, rotx, roty;
    static integer idum1, idum2;
    static real xdiff, ydiff, halfw, tcoef, theta, pleft, cross;
    static integer kount;
    static real theta1, theta2, arcdel, arcold;
    extern /* Subroutine */ int arcpar_(integer *, integer *, integer *, real 
	    *, integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, real *, real *, real *, real *, logical *, 
	    integer *, integer *, real *, real *);
    static real height, avalue, bvalue, radius, arcnow, arctot, arcnxt;
    extern /* Subroutine */ int elpspr_(integer *, integer *, integer *, real 
	    *, integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, real *, real *, integer *, integer *, real *, 
	    real *, logical *);
    static real xstart, ystart;

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___11 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___39 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___41 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE LINLEN = CALCULATES THE LENGTH OF A GIVEN LINE */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NID   = AN ARRAY OF UNIQUE NODE IDENTIFIERS. */
/*     REAL  = .TRUE. FOR AN ACTUAL GENERATION */
/*           = .FALSE. FOR A TRIAL GENERATION */
/*     ERR   = .TRUE. IF AN ERROR WAS ENCOUNTERED */
/*     J1    = POINTER FOR THE FIRST POINT */
/*     J2    = POINTER FOR THE SECOND POINT */
/*     J3    = POINTER FOR THE THIRD POINT */
/*     MAXNP = MAXIMUM NUMBER OF NODES ON THE PERIMETER */
/*             NOTE: MAXNP MUST BE ADJUSTED FOR THE CURRENT */
/*                   LOCATION IN X, Y, & NID */
/*     KT    = THE LINE TYPE: */
/*           = 1 FOR STRAIGHT LINES */
/*           = 2 FOR CORNER LINES */
/*           = 3 FOR ARC WITH CENTER GIVEN */
/*           = 4 FOR ARC WITH THIRD POINT ON THE ARC */
/*           = 5 FOR PARABOLA */
/*           = 6 FOR ARC WITH RADIUS GIVEN */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    *dist = (float)0.;
    *err = TRUE_;
/*  STRAIGHT LINE GENERATION */
    if (*kt == 1) {
	ydiff = coor[(*j2 << 1) + 2] - coor[(*j1 << 1) + 2];
	xdiff = coor[(*j2 << 1) + 1] - coor[(*j1 << 1) + 1];
/* Computing 2nd power */
	r__1 = ydiff;
/* Computing 2nd power */
	r__2 = xdiff;
	*dist = sqrt(r__1 * r__1 + r__2 * r__2);
	if (*dist == (float)0.) {
	    s_wsfe(&io___4);
	    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
/*  CORNER GENERATION */
    } else if (*kt == 2) {
	xda = coor[(*j3 << 1) + 1] - coor[(*j1 << 1) + 1];
	yda = coor[(*j3 << 1) + 2] - coor[(*j1 << 1) + 2];
	xdb = coor[(*j2 << 1) + 1] - coor[(*j3 << 1) + 1];
	ydb = coor[(*j2 << 1) + 2] - coor[(*j3 << 1) + 2];
/* Computing 2nd power */
	r__1 = xda;
/* Computing 2nd power */
	r__2 = yda;
	da = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xdb;
/* Computing 2nd power */
	r__2 = ydb;
	db = sqrt(r__1 * r__1 + r__2 * r__2);
	if (da == (float)0. || db == (float)0.) {
	    s_wsfe(&io___11);
	    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
	*dist = da + db;
/*  CIRCULAR ARC */
    } else if (*kt == 3 || *kt == 4 || *kt == 6) {
	xstart = coor[(*j1 << 1) + 1];
	ystart = coor[(*j1 << 1) + 2];
	arcpar_(mp, kt, knum, &coor[3], &linkp[3], j1, j2, j3, i3, &xcen, &
		ycen, &theta1, &theta2, &tang, &r1, &r2, err, &iccw, &icw, &
		xk, &xa);
/*  GENERATE THE CIRCLE */
	ang = theta1;
	del = tang / 30;
	for (i__ = 2; i__ <= 29; ++i__) {
	    ang += del;
	    radius = xa * exp(xk * ang);
	    xend = xcen + cos(ang) * radius;
	    yend = ycen + sin(ang) * radius;
/* Computing 2nd power */
	    r__1 = xend - xstart;
/* Computing 2nd power */
	    r__2 = yend - ystart;
	    *dist += sqrt(r__1 * r__1 + r__2 * r__2);
	    xstart = xend;
	    ystart = yend;
/* L100: */
	}
	xend = coor[(*j2 << 1) + 1];
	yend = coor[(*j2 << 1) + 2];
/* Computing 2nd power */
	r__1 = xend - xstart;
/* Computing 2nd power */
	r__2 = yend - ystart;
	*dist += sqrt(r__1 * r__1 + r__2 * r__2);
/*  ELIPSE */
    } else if (*kt == 7) {
	xstart = coor[(*j1 << 1) + 1];
	ystart = coor[(*j1 << 1) + 2];
	elpspr_(mp, kt, knum, &coor[3], &linkp[3], j1, j2, j3, i3, &xcen, &
		ycen, &theta1, &theta2, &tang, &idum1, &idum2, &avalue, &
		bvalue, err);
/*  GENERATE THE ELIPSE */
	ang = theta1;
	del = tang / 30;
	for (i__ = 2; i__ <= 29; ++i__) {
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
	    xend = xcen + cos(ang) * radius;
	    yend = ycen + sin(ang) * radius;
/* Computing 2nd power */
	    r__1 = xend - xstart;
/* Computing 2nd power */
	    r__2 = yend - ystart;
	    *dist += sqrt(r__1 * r__1 + r__2 * r__2);
	    xstart = xend;
	    ystart = yend;
/* L110: */
	}
	xend = coor[(*j2 << 1) + 1];
	yend = coor[(*j2 << 1) + 2];
/* Computing 2nd power */
	r__1 = xend - xstart;
/* Computing 2nd power */
	r__2 = yend - ystart;
	*dist += sqrt(r__1 * r__1 + r__2 * r__2);
/*     PARABOLA */
    } else if (*kt == 5) {
/*  CHECK LEGITIMACY OF DATA */
	xmid = (coor[(*j1 << 1) + 1] + coor[(*j2 << 1) + 1]) * (float).5;
	ymid = (coor[(*j1 << 1) + 2] + coor[(*j2 << 1) + 2]) * (float).5;
	dot = (coor[(*j2 << 1) + 1] - coor[(*j1 << 1) + 1]) * (coor[(*j3 << 1)
		 + 1] - xmid) + (coor[(*j2 << 1) + 2] - coor[(*j1 << 1) + 2]) 
		* (coor[(*j3 << 1) + 2] - ymid);
/* Computing 2nd power */
	r__1 = coor[(*j2 << 1) + 1] - coor[(*j1 << 1) + 1];
/* Computing 2nd power */
	r__2 = coor[(*j2 << 1) + 2] - coor[(*j1 << 1) + 2];
/* Computing 2nd power */
	r__3 = coor[(*j3 << 1) + 1] - xmid;
/* Computing 2nd power */
	r__4 = coor[(*j3 << 1) + 2] - ymid;
	perp = sqrt(r__1 * r__1 + r__2 * r__2) * sqrt(r__3 * r__3 + r__4 * 
		r__4);
	if (dot >= perp * (float).05) {
	    s_wsfe(&io___39);
	    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
/*  GETARC LENGTH */
/* Computing 2nd power */
	r__1 = coor[(*j2 << 1) + 1] - coor[(*j1 << 1) + 1];
/* Computing 2nd power */
	r__2 = coor[(*j2 << 1) + 2] - coor[(*j1 << 1) + 2];
	halfw = sqrt(r__1 * r__1 + r__2 * r__2) * (float).5;
	if (halfw == (float)0.) {
	    s_wsfe(&io___41);
	    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
/* Computing 2nd power */
	r__1 = xmid - coor[(*j3 << 1) + 1];
/* Computing 2nd power */
	r__2 = ymid - coor[(*j3 << 1) + 2];
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
	arcdel = arctot / 30;
	arcnxt = arcdel;
	arcnow = (float)0.;
	theta = atan2(coor[(*j2 << 1) + 2] - coor[(*j1 << 1) + 2], coor[(*j2 
		<< 1) + 1] - coor[(*j1 << 1) + 1]);
/*  CORRECT FOR ORIENTATION */
	cross = (coor[(*j3 << 1) + 1] - xmid) * (coor[(*j2 << 1) + 2] - coor[(
		*j1 << 1) + 2]) - (coor[(*j3 << 1) + 2] - ymid) * (coor[(*j2 
		<< 1) + 1] - coor[(*j1 << 1) + 1]);
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
	xstart = coor[(*j1 << 1) + 1];
	ystart = coor[(*j1 << 1) + 2];
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
		xend = rotx + coor[(*j3 << 1) + 1];
		yend = roty + coor[(*j3 << 1) + 2];
/* Computing 2nd power */
		r__1 = xend - xstart;
/* Computing 2nd power */
		r__2 = yend - ystart;
		*dist += sqrt(r__1 * r__1 + r__2 * r__2);
		++kount;
		xstart = xend;
		ystart = yend;
/*  PREPARE FOR NEXT POINT */
		if (kount >= 29) {
		    goto L130;
		}
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
	xend = coor[(*j2 << 1) + 1];
	yend = coor[(*j2 << 1) + 2];
/* Computing 2nd power */
	r__1 = xend - xstart;
/* Computing 2nd power */
	r__2 = yend - ystart;
	*dist += sqrt(r__1 * r__1 + r__2 * r__2);
    }
/*     NORMAL EXIT */
    *err = FALSE_;
    return 0;
} /* linlen_ */

#ifdef __cplusplus
	}
#endif

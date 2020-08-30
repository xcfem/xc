/* pline.f -- translated by f2c (version 20160102).
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
static integer c__9 = 9;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int pline_(integer *mp, integer *ml, integer *maxnp, integer 
	*maxnbc, integer *maxsbc, integer *ipoint, real *coor, integer *linkp,
	 integer *knum, integer *kt, integer *nint, real *fac, integer *ip1, 
	integer *ip2, integer *ip3, real *x, real *y, integer *nid, integer *
	ipbc1, integer *ipbc2, integer *ilbc, integer *isbc, integer *linkpb, 
	integer *nppf, integer *ifpb, integer *listpb, integer *linklb, 
	integer *nlpf, integer *iflb, integer *listlb, integer *linksb, 
	integer *nspf, integer *ifsb, integer *listsb, integer *lstnbc, 
	integer *knbc, integer *ksbc, logical *err, logical *test, logical *
	real__, logical *count, logical *noroom, real *amesur, real *xnold, 
	real *ynold, integer *nxkold, integer *mmpold, integer *linkeg, 
	integer *listeg, real *bmesur, integer *mlink, integer *nprold, 
	integer *npnold, integer *npeold, integer *nnxk, logical *remesh, 
	real *rexmin, real *rexmax, real *reymin, real *reymax, integer *
	idivis, real *sizmin, real *emax, real *emin, logical *graph, real *
	dxmax)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 ZERO NUMBER OF INTERVALS FOR LINE\002,i\
5)";
    static char fmt_10010[] = "(\002 ZERO LINE LENGTH ENCOUNTERED FOR LIN\
E\002,i5)";
    static char fmt_10020[] = "(\002 POINTS GIVEN FOR LINE\002,i5,\002 DO NO\
T DEFINE AN ARC\002)";
    static char fmt_10030[] = "(\002 MORE THAN 250 POINTS FOR PARABOLA NOT A\
LLOWED - LINE\002,i5)";
    static char fmt_10040[] = "(\002 POINTS GIVEN FOR LINE\002,i5,\002 DO NO\
T DEFINE A PARABOLA\002)";
    static char fmt_10060[] = "(\002 POINT BOUNDARY FLAG\002,i5,\002 IS NOT \
PROPERLY LINKED\002)";
    static char fmt_10050[] = "(\002 LINE BOUNDARY FLAG\002,i5,\002 IS NOT P\
ROPERLY LINKED\002)";
    static char fmt_10070[] = "(\002 SIDE BOUNDARY FLAG\002,i5,\002 IS NOT P\
ROPERLY LINKED\002)";

    /* System generated locals */
    integer nxkold_dim1, nxkold_offset, i__1, i__2, i__3;
    real r__1, r__2, r__3, r__4, r__5, r__6;

    /* Builtin functions */
    double atan2(doublereal, doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(), 
	    s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle();
    /* Subroutine */ int s_stop(char *, ftnlen);
    double pow_ri(real *, integer *), sqrt(doublereal), cos(doublereal), sin(
	    doublereal), log(doublereal), exp(doublereal);

    /* Local variables */
    static real d__;
    static integer i__, j, k, n;
    static real r__, r1, r2, x1, y1, x2, y2, aa, bb, da, db;
    static integer na, nb;
    static real fl, fm;
    static integer ki;
    static real fr, pi, dx, dy, xk, xl, yk, xr, dff, del, ang, xda, yda, xdb, 
	    ydb;
    static integer nap;
    static real det, arm;
    static integer icw;
    static real dot, eps, sum, frac, coef;
    static integer iccw;
    extern doublereal parc_(real *, real *);
    static real tang, delx, xcen, ycen, xmid, ymid, part, perp, cost, sint, 
	    rotx, roty;
    static integer iflag;
    static real chord, xdiff, ydiff, halfw, tcoef, theta, pleft;
    static integer lpart;
    static real cross;
    static integer kount;
    static real twopi, theta1, theta2, theta3;
    static integer ipntr1;
    static real arcdel;
    static logical addlnk;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static real arcold, height, avalue, bvalue;
    extern /* Subroutine */ int crcsiz_(integer *, real *, real *, integer *, 
	    integer *, real *, real *, real *, real *, real *, real *, real *,
	     real *, real *, logical *, logical *, real *, real *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, logical *, real *, real *, real *, real *, integer *, 
	    real *, real *, real *, logical *, real *);
    static real radius;
    extern /* Subroutine */ int elpspr_(integer *, integer *, integer *, real 
	    *, integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, real *, real *, integer *, integer *, real *, 
	    real *, logical *);
    static real arctot, arcnxt, arcnow;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *), strsiz_(integer *, real *, real *, integer 
	    *, integer *, real *, real *, real *, real *, real *, logical *, 
	    logical *, real *, real *, integer *, integer *, integer *, real *
	    , integer *, integer *, integer *, integer *, logical *, real *, 
	    real *, real *, real *, integer *, real *, real *, real *, 
	    logical *, real *);

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___7 = { 0, 6, 0, 0, 0 };
    static cilist io___12 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___22 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___44 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___55 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___60 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___62 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___88 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___93 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___94 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___96 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___97 = { 0, 6, 0, fmt_10070, 0 };


/* *********************************************************************** */
/*  SUBROUTINE PLINE = PRODUCES A NODE STRING FOR THE K'TH LINE IN THE */
/*                     LINE TABLE */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NID   = AN ARRAY OF UNIQUE NODE IDENTIFIERS. */
/*     REAL  = .TRUE. FOR AN ACTUAL GENERATION */
/*           = .FALSE. FOR A TRIAL GENERATION */
/*     ERR   = .TRUE. IF AN ERROR WAS ENCOUNTERED */
/*     IP1   = POINTER FOR THE FIRST POINT */
/*     IP2   = POINTER FOR THE SECOND POINT */
/*     IP3   = POINTER FOR THE THIRD POINT */
/*     MAXNP = MAXIMUM NUMBER OF NODES ON THE PERIMETER */
/*     NOTE: MAXNP MUST BE ADJUSTED FOR THE CURRENT LOCATION */
/*           IN X, Y, &NID */
/*     KT    = THE LINE TYPE: */
/*           = 1 FOR STRAIGHT LINES */
/*           = 2 FOR CORNER LINES */
/*           = 3 FOR ARC WITH CENTER GIVEN */
/*           = 4 FOR ARC WITH THIRD POINT ON THE ARC */
/*           = 5 FOR PARABOLA */
/*           = 6 FOR ARC WITH RADIUS GIVEN */
/* *********************************************************************** */
    /* Parameter adjustments */
    listpb -= 3;
    --ifpb;
    --nppf;
    linkpb -= 3;
    linkp -= 3;
    coor -= 3;
    --ipoint;
    listsb -= 3;
    --ifsb;
    --nspf;
    linksb -= 3;
    listlb -= 3;
    --iflb;
    --nlpf;
    linklb -= 3;
    --nid;
    --y;
    --x;
    --lstnbc;
    linkeg -= 3;
    mmpold -= 4;
    --bmesur;
    --ynold;
    --xnold;
    --listeg;
    --amesur;
    nxkold_dim1 = *nnxk;
    nxkold_offset = 1 + nxkold_dim1;
    nxkold -= nxkold_offset;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    eps = (float).01;
    *err = FALSE_;
    addlnk = FALSE_;
    *noroom = FALSE_;
    twopi = pi + pi;
/*     COMPUTE FRACTION OF TOTAL LENGTH FOR FIRST INTERVAL */
    n = abs(*nint) + 1;
    if (n <= 1) {
	s_wsfe(&io___6);
	do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	e_wsfe();
	*err = TRUE_;
	goto L340;
    }
    if (n > *maxnp) {
	s_wsle(&io___7);
	do_lio(&c__9, &c__1, "INTERNAL ERROR: Intervals exceed space", (
		ftnlen)38);
	e_wsle();
	s_stop("", (ftnlen)0);
    }
    dff = (float)1. / (doublereal) (n - 1);
    if ((r__1 = (float)1. - *fac, dabs(r__1)) > (float)1e-6) {
	i__1 = n - 1;
	dff = (*fac - (float)1.) / (pow_ri(fac, &i__1) - (float)1.);
    }
/*  DEFINE FIRST POINT EXACTLY AND BRANCH */
    x[1] = coor[(*ip1 << 1) + 1];
    y[1] = coor[(*ip1 << 1) + 2];
    if (n > 2) {
/*  STRAIGHT LINE GENERATION */
	if (*kt == 1) {
	    ydiff = coor[(*ip2 << 1) + 2] - coor[(*ip1 << 1) + 2];
	    xdiff = coor[(*ip2 << 1) + 1] - coor[(*ip1 << 1) + 1];
/* Computing 2nd power */
	    r__1 = ydiff;
/* Computing 2nd power */
	    r__2 = xdiff;
	    d__ = sqrt(r__1 * r__1 + r__2 * r__2);
	    if (d__ == (float)0.) {
		s_wsfe(&io___12);
		do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		e_wsfe();
		*err = TRUE_;
		goto L340;
	    }
	    if (*remesh) {
		strsiz_(maxnp, &x[1], &y[1], nint, &n, &coor[(*ip2 << 1) + 1],
			 &coor[(*ip2 << 1) + 2], &xdiff, &ydiff, &d__, err, 
			test, &xnold[1], &ynold[1], &nxkold[nxkold_offset], &
			linkeg[3], &listeg[1], &bmesur[1], mlink, npnold, 
			npeold, nnxk, remesh, rexmin, rexmax, reymin, reymax, 
			idivis, sizmin, emax, emin, graph, dxmax);
		if (*err) {
		    goto L340;
		}
	    } else {
		del = d__ * dff;
		i__1 = n - 1;
		for (i__ = 2; i__ <= i__1; ++i__) {
		    part = del / d__;
		    x[i__] = x[i__ - 1] + part * xdiff;
		    y[i__] = y[i__ - 1] + part * ydiff;
		    del *= *fac;
/* L100: */
		}
	    }
/*  CORNER GENERATION */
	} else if (*kt == 2) {
	    xda = coor[(*ip3 << 1) + 1] - coor[(*ip1 << 1) + 1];
	    yda = coor[(*ip3 << 1) + 2] - coor[(*ip1 << 1) + 2];
	    xdb = coor[(*ip2 << 1) + 1] - coor[(*ip3 << 1) + 1];
	    ydb = coor[(*ip2 << 1) + 2] - coor[(*ip3 << 1) + 2];
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
		s_wsfe(&io___22);
		do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		e_wsfe();
		*err = TRUE_;
		goto L340;
	    }
	    d__ = da + db;
	    del = d__ * dff;
	    sum = (float)0.;
/*  BREAK N INTO TWO PARTS APPROPRIATELY */
	    i__1 = n - 1;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		ki = i__;
		sum += del;
		if (sum + del * (float).5 > da) {
		    goto L120;
		}
		del *= *fac;
/* L110: */
	    }
/*  GENERATE FIRST SIDE OF CORNER */
L120:
	    na = ki;
	    dff = (float)1. / (doublereal) (na - 1);
	    if ((r__1 = (float)1. - *fac, dabs(r__1)) > (float)1e-6) {
		i__1 = na - 1;
		dff = (*fac - (float)1.) / (pow_ri(fac, &i__1) - (float)1.);
	    }
	    del = da * dff;
	    i__1 = na;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		part = del / da;
		x[i__] = x[i__ - 1] + part * xda;
		y[i__] = y[i__ - 1] + part * yda;
		del *= *fac;
/* L130: */
	    }
/*  GENERATE SECOND SIDE OF CORNER */
	    nb = n - ki + 1;
	    dff = (float)1. / (doublereal) (nb - 1);
	    if ((r__1 = (float)1. - *fac, dabs(r__1)) > (float)1e-6) {
		i__1 = nb - 1;
		dff = (*fac - (float)1.) / (pow_ri(fac, &i__1) - (float)1.);
	    }
	    del = db * dff;
	    nap = na + 1;
	    i__1 = n - 1;
	    for (i__ = nap; i__ <= i__1; ++i__) {
		part = del / db;
		x[i__] = x[i__ - 1] + part * xdb;
		y[i__] = y[i__ - 1] + part * ydb;
		del *= *fac;
/* L140: */
	    }
/*  CIRCULAR ARC GENERATION */
	} else if (*kt == 3 || *kt == 4 || *kt == 6) {
/*  ARC WITH CENTER GIVEN */
/*  ARC GOES FROM 1ST POINT TO 2ND IN *COUNTER-CLOCKWISE* DIRECTION. */
	    if (*kt == 3) {
		xcen = coor[(abs(*ip3) << 1) + 1];
		ycen = coor[(abs(*ip3) << 1) + 2];
/*  CIRCLE WITH THIRD POINT ON ARC. */
	    } else if (*kt == 4) {
		theta1 = atan2(coor[(*ip3 << 1) + 2] - coor[(*ip1 << 1) + 2], 
			coor[(*ip3 << 1) + 1] - coor[(*ip1 << 1) + 1]) + pi / 
			(float)2.;
		theta2 = atan2(coor[(*ip3 << 1) + 2] - coor[(*ip2 << 1) + 2], 
			coor[(*ip3 << 1) + 1] - coor[(*ip2 << 1) + 1]) + pi / 
			(float)2.;
		det = -cos(theta1) * sin(theta2) + cos(theta2) * sin(theta1);
		x1 = (coor[(*ip1 << 1) + 1] + coor[(*ip3 << 1) + 1]) * (float)
			.5;
		y1 = (coor[(*ip1 << 1) + 2] + coor[(*ip3 << 1) + 2]) * (float)
			.5;
		x2 = (coor[(*ip2 << 1) + 1] + coor[(*ip3 << 1) + 1]) * (float)
			.5;
		y2 = (coor[(*ip2 << 1) + 2] + coor[(*ip3 << 1) + 2]) * (float)
			.5;
		r__ = (-sin(theta2) * (x2 - x1) + cos(theta2) * (y2 - y1)) / 
			det;
		xcen = x1 + r__ * cos(theta1);
		ycen = y1 + r__ * sin(theta1);
/*     CIRCLE WITH RADIUS GIVEN */
	    } else if (*kt == 6) {
		dx = (coor[(*ip2 << 1) + 1] - coor[(*ip1 << 1) + 1]) * (float)
			.5;
		dy = (coor[(*ip2 << 1) + 2] - coor[(*ip1 << 1) + 2]) * (float)
			.5;
		chord = sqrt(dx * dx + dy * dy);
		r__ = (r__1 = coor[(abs(*ip3) << 1) + 1], dabs(r__1));
		if (r__ <= chord) {
		    xcen = (coor[(*ip1 << 1) + 1] + coor[(*ip2 << 1) + 1]) * (
			    float).5;
		    ycen = (coor[(*ip1 << 1) + 2] + coor[(*ip2 << 1) + 2]) * (
			    float).5;
		} else {
		    arm = sqrt(r__ * r__ - chord * chord);
		    if (*ip3 < 0) {
			xcen = coor[(*ip1 << 1) + 1] + dx + arm * dy / chord;
			ycen = coor[(*ip1 << 1) + 2] + dy - arm * dx / chord;
		    } else {
			xcen = coor[(*ip1 << 1) + 1] + dx - arm * dy / chord;
			ycen = coor[(*ip1 << 1) + 2] + dy + arm * dx / chord;
		    }
		}
	    }
/* Computing 2nd power */
	    r__1 = coor[(*ip1 << 1) + 1] - xcen;
/* Computing 2nd power */
	    r__2 = coor[(*ip1 << 1) + 2] - ycen;
	    r1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	    r__1 = coor[(*ip2 << 1) + 1] - xcen;
/* Computing 2nd power */
	    r__2 = coor[(*ip2 << 1) + 2] - ycen;
	    r2 = sqrt(r__1 * r__1 + r__2 * r__2);
	    if (r1 == (float)0. || r2 == (float)0.) {
		s_wsfe(&io___44);
		do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		e_wsfe();
		*err = TRUE_;
		goto L340;
	    }
	    theta1 = atan2(coor[(*ip1 << 1) + 2] - ycen, coor[(*ip1 << 1) + 1]
		     - xcen);
	    theta2 = atan2(coor[(*ip2 << 1) + 2] - ycen, coor[(*ip2 << 1) + 1]
		     - xcen);
/*  ARC WITH THE CENTER GIVEN */
	    if (*kt == 3) {
		if (*ip3 >= 0 && theta2 <= theta1) {
		    theta2 += twopi;
		}
		if (*ip3 < 0 && theta1 <= theta2) {
		    theta1 += twopi;
		}
		tang = theta2 - theta1;
/*  CIRCULAR ARC WITH 3RD POINT ON ARC - CLOCKWISE OR COUNTER-CLOCKWISE */
	    } else if (*kt == 4) {
		theta3 = atan2(coor[(*ip3 << 1) + 2] - ycen, coor[(*ip3 << 1) 
			+ 1] - xcen);
		if (theta2 <= theta1) {
		    theta2 += twopi;
		}
		if (theta3 <= theta1) {
		    theta3 += twopi;
		}
		tang = theta2 - theta1;
		if (theta3 > theta2) {
		    tang = -(twopi - tang);
		}
/*     CIRRCULAR ARC WITH RADIUS GIVEN - CLOCKWISE OR COUNTER-CLOCKWISE */
	    } else if (*kt == 6) {
		if (*ip3 >= 0 && theta2 <= theta1) {
		    theta2 += twopi;
		}
		if (*ip3 < 0 && theta1 <= theta2) {
		    theta1 += twopi;
		}
		tang = theta2 - theta1;
	    }
/*  GENERATE THE CIRCLE */
	    ang = theta1;
	    del = tang * dff;
	    aa = log(r2 / r1) / (theta2 - theta1);
	    bb = r2 / exp(aa * theta2);
	    if (*remesh) {
		crcsiz_(maxnp, &x[1], &y[1], nint, &n, &coor[(*ip2 << 1) + 1],
			 &coor[(*ip2 << 1) + 2], &xcen, &ycen, &theta1, &
			theta2, &tang, &aa, &bb, err, test, &xnold[1], &ynold[
			1], &nxkold[nxkold_offset], &linkeg[3], &listeg[1], &
			bmesur[1], mlink, npnold, npeold, nnxk, remesh, 
			rexmin, rexmax, reymin, reymax, idivis, sizmin, emax, 
			emin, graph, dxmax);
		if (*err) {
		    goto L340;
		}
	    } else {
		i__1 = n - 1;
		for (i__ = 2; i__ <= i__1; ++i__) {
		    ang += del;
		    radius = bb * exp(aa * ang);
		    x[i__] = xcen + cos(ang) * radius;
		    y[i__] = ycen + sin(ang) * radius;
		    del *= *fac;
/* L150: */
		}
	    }
/*     ELIPSE */
	} else if (*kt == 7) {
/*  GET THE ELIPSE PARAMETERS */
	    i__1 = abs(*ip3);
	    elpspr_(mp, kt, knum, &coor[3], &linkp[3], ip1, ip2, &i__1, ip3, &
		    xcen, &ycen, &theta1, &theta2, &tang, &iccw, &icw, &
		    avalue, &bvalue, err);
	    if (*err) {
		goto L340;
	    }
/*  GENERATE THE ELIPSE */
	    ang = theta1;
	    del = tang * dff;
	    i__1 = n - 1;
	    for (i__ = 2; i__ <= i__1; ++i__) {
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
		radius = sqrt(r__1 * r__1 * (r__2 * r__2) / (r__3 * r__3 * (
			r__4 * r__4) + r__5 * r__5 * (r__6 * r__6)));
		x[i__] = xcen + cos(ang) * radius;
		y[i__] = ycen + sin(ang) * radius;
		del *= *fac;
/* L160: */
	    }
/*     PARABOLA */
	} else if (*kt == 5) {
	    if (n > 250) {
		s_wsfe(&io___55);
		do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		e_wsfe();
		*err = TRUE_;
		goto L340;
	    }
/*  CHECK LEGITIMACY OF DATA */
	    xmid = (coor[(*ip1 << 1) + 1] + coor[(*ip2 << 1) + 1]) * (float)
		    .5;
	    ymid = (coor[(*ip1 << 1) + 2] + coor[(*ip2 << 1) + 2]) * (float)
		    .5;
	    dot = (coor[(*ip2 << 1) + 1] - coor[(*ip1 << 1) + 1]) * (coor[(*
		    ip3 << 1) + 1] - xmid) + (coor[(*ip2 << 1) + 2] - coor[(*
		    ip1 << 1) + 2]) * (coor[(*ip3 << 1) + 2] - ymid);
/* Computing 2nd power */
	    r__1 = coor[(*ip2 << 1) + 1] - coor[(*ip1 << 1) + 1];
/* Computing 2nd power */
	    r__2 = coor[(*ip2 << 1) + 2] - coor[(*ip1 << 1) + 2];
/* Computing 2nd power */
	    r__3 = coor[(*ip3 << 1) + 1] - xmid;
/* Computing 2nd power */
	    r__4 = coor[(*ip3 << 1) + 2] - ymid;
	    perp = sqrt(r__1 * r__1 + r__2 * r__2) * sqrt(r__3 * r__3 + r__4 *
		     r__4);
	    if (dot >= perp * (float).05) {
		s_wsfe(&io___60);
		do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		e_wsfe();
		*err = TRUE_;
		goto L340;
	    }
/*  GET ARC LENGTH */
/* Computing 2nd power */
	    r__1 = coor[(*ip2 << 1) + 1] - coor[(*ip1 << 1) + 1];
/* Computing 2nd power */
	    r__2 = coor[(*ip2 << 1) + 2] - coor[(*ip1 << 1) + 2];
	    halfw = sqrt(r__1 * r__1 + r__2 * r__2) * (float).5;
	    if (halfw == (float)0.) {
		s_wsfe(&io___62);
		do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		e_wsfe();
		*err = TRUE_;
		goto L340;
	    }
/* Computing 2nd power */
	    r__1 = xmid - coor[(*ip3 << 1) + 1];
/* Computing 2nd power */
	    r__2 = ymid - coor[(*ip3 << 1) + 2];
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
	    theta = atan2(coor[(*ip2 << 1) + 2] - coor[(*ip1 << 1) + 2], coor[
		    (*ip2 << 1) + 1] - coor[(*ip1 << 1) + 1]);
/*  CORRECT FOR ORIENTATION */
	    cross = (coor[(*ip3 << 1) + 1] - xmid) * (coor[(*ip2 << 1) + 2] - 
		    coor[(*ip1 << 1) + 2]) - (coor[(*ip3 << 1) + 2] - ymid) * 
		    (coor[(*ip2 << 1) + 1] - coor[(*ip1 << 1) + 1]);
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
		    x[kount] = rotx + coor[(*ip3 << 1) + 1];
		    y[kount] = roty + coor[(*ip3 << 1) + 2];
/*  PREPARE FOR NEXT POINT */
		    if (kount >= n - 1) {
			goto L180;
		    }
		    arcdel *= *fac;
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
/* L170: */
	    }
L180:
	    ;
	}
/*  SEE IF THE 1 INTERVAL LINE HAS SOME LENGTH TO IT */
    } else {
	if (coor[(*ip1 << 1) + 1] == coor[(*ip2 << 1) + 1] && coor[(*ip1 << 1)
		 + 2] == coor[(*ip2 << 1) + 2]) {
	    s_wsfe(&io___88);
	    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    goto L340;
	}
    }
/*     NORMAL EXIT */
/*     DEFINE LAST POINT EXACTLY */
    x[n] = coor[(*ip2 << 1) + 1];
    y[n] = coor[(*ip2 << 1) + 2];
    if (*test) {
	goto L340;
    }
/*  DEFINE UNIQUE  (IN THE WHOLE BODY) NODE NUMBERS */
    lpart = *knum * 100000 + 1000000000;
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	nid[i__] = lpart + i__;
/* L190: */
    }
    nid[1] = ipoint[*ip1];
    nid[n] = ipoint[*ip2];
/*  FLAG PREVIOUSLY USED POINTS WITH NEGATIVES */
    if (*nint < 0) {
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nid[i__] = -nid[i__];
/* L200: */
	}
    }
    if (ipoint[*ip1] < 0) {
	nid[1] = -abs(nid[1]);
    }
    if (ipoint[*ip2] < 0) {
	nid[n] = -(i__1 = nid[n], abs(i__1));
    }
    if (*ip1 == *ip2) {
	nid[n] = -(i__1 = nid[n], abs(i__1));
    }
/*  WRITE OUT NODAL BOUNDARY CONDITIONS FOR THE FIRST POINT */
/*  IF THE POINT HAS NOT BEEN USED BEFORE */
    if (ipoint[*ip1] > 0) {
	if (*ipbc1 > 0) {
	    ltsort_(mp, &linkpb[3], ipbc1, &k, &addlnk);
	    iflag = *ipbc1;
L210:
	    i__1 = nppf[k] + ifpb[k] - 1;
	    for (i__ = ifpb[k]; i__ <= i__1; ++i__) {
		ltsort_(mp, &linkp[3], &listpb[(i__ << 1) + 1], &ipntr1, &
			addlnk);
		if (ipntr1 == *ip1) {
		    *knbc += 2;
		    if (*knbc <= *maxnbc) {
			if (*real__) {
			    lstnbc[*knbc - 1] = -iflag;
			    lstnbc[*knbc] = abs(nid[1]);
			}
		    } else {
			*noroom = TRUE_;
		    }
		    if (listpb[(i__ << 1) + 2] > 0) {
			ltsort_(mp, &linkpb[3], &listpb[(i__ << 1) + 2], &k, &
				addlnk);
			iflag = listpb[(i__ << 1) + 2];
			goto L210;
		    } else {
			goto L230;
		    }
		}
/* L220: */
	    }
	    s_wsfe(&io___93);
	    do_fio(&c__1, (char *)&iflag, (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    goto L340;
	}
L230:
	;
    }
/*  WRITE OUT NODAL BOUNDARY CONDITIONS FOR THE SECOND POINT */
/*  IF THE POINT HAS NOT BEEN USED BEFORE */
    if (ipoint[*ip2] > 0) {
	if (*ipbc2 > 0) {
	    ltsort_(mp, &linkpb[3], ipbc2, &k, &addlnk);
	    iflag = *ipbc2;
L240:
	    i__1 = nppf[k] + ifpb[k] - 1;
	    for (i__ = ifpb[k]; i__ <= i__1; ++i__) {
		ltsort_(mp, &linkp[3], &listpb[(i__ << 1) + 1], &ipntr1, &
			addlnk);
		if (ipntr1 == *ip2) {
		    *knbc += 2;
		    if (*knbc <= *maxnbc) {
			if (*real__) {
			    lstnbc[*knbc - 1] = -iflag;
			    lstnbc[*knbc] = (i__2 = nid[n], abs(i__2));
			}
		    } else {
			*noroom = TRUE_;
		    }
		    if (listpb[(i__ << 1) + 2] > 0) {
			ltsort_(mp, &linkpb[3], &listpb[(i__ << 1) + 2], &k, &
				addlnk);
			iflag = listpb[(i__ << 1) + 2];
			goto L240;
		    } else {
			goto L260;
		    }
		}
/* L250: */
	    }
	    s_wsfe(&io___94);
	    do_fio(&c__1, (char *)&iflag, (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    goto L340;
	}
L260:
	;
    }
/*  WRITE OUT NODAL BOUNDARY CONDITIONS FOR THE LINE */
/*  IF THE LINE HAS NOT BEEN USED BEFORE */
    if (*nint > 0) {
	if (*ilbc > 0) {
	    ltsort_(ml, &linklb[3], ilbc, &k, &addlnk);
	    iflag = *ilbc;
L270:
	    i__1 = nlpf[k] + iflb[k] - 1;
	    for (i__ = iflb[k]; i__ <= i__1; ++i__) {
		if (listlb[(i__ << 1) + 1] <= 0) {
		    mesage_("PROBLEMS WITH SIDES IN FLAG LIST IN PLINE", (
			    ftnlen)41);
		} else {
		    if (listlb[(i__ << 1) + 1] == *knum) {
			++(*knbc);
			if (*knbc <= *maxnbc) {
			    if (*real__) {
				lstnbc[*knbc] = -iflag;
			    }
			} else {
			    *noroom = TRUE_;
			}
			i__2 = n;
			for (j = 1; j <= i__2; ++j) {
			    ++(*knbc);
			    if (*knbc <= *maxnbc) {
				if (*real__) {
				    lstnbc[*knbc] = (i__3 = nid[j], abs(i__3))
					    ;
				}
			    } else {
				*noroom = TRUE_;
			    }
/* L280: */
			}
			if (listlb[(i__ << 1) + 2] > 0) {
			    ltsort_(ml, &linklb[3], &listlb[(i__ << 1) + 2], &
				    k, &addlnk);
			    iflag = listlb[(i__ << 1) + 2];
			    goto L270;
			} else {
			    goto L300;
			}
		    }
		}
/* L290: */
	    }
	    s_wsfe(&io___96);
	    do_fio(&c__1, (char *)&iflag, (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    goto L340;
	}
L300:
	;
    }
/*  IF COUNT, THEN COUNT THE SIDE BOUNDARY CONDITIONS FOR THE LINE */
/*  NOTE: IT DOES NOT MATTER IF THE LINE HAS BEEN USED BEFORE */
    if (*count && *isbc > 0) {
	ltsort_(ml, &linksb[3], isbc, &k, &addlnk);
	iflag = *isbc;
L310:
	i__1 = nspf[k] + ifsb[k] - 1;
	for (i__ = ifsb[k]; i__ <= i__1; ++i__) {
	    if (listsb[(i__ << 1) + 1] < 0) {
		mesage_("PROBLEMS WITH SIDES IN FLAG LIST IN PLINE", (ftnlen)
			41);
	    } else {
		if (listsb[(i__ << 1) + 1] == *knum) {
		    *ksbc += (n - 1) * 3;
		    if (*ksbc > *maxsbc) {
			*noroom = TRUE_;
		    }
		    if (listsb[(i__ << 1) + 2] > 0) {
			ltsort_(ml, &linksb[3], &listsb[(i__ << 1) + 2], &k, &
				addlnk);
			iflag = listsb[(i__ << 1) + 2];
			goto L310;
		    } else {
			goto L330;
		    }
		}
	    }
/* L320: */
	}
	s_wsfe(&io___97);
	do_fio(&c__1, (char *)&iflag, (ftnlen)sizeof(integer));
	e_wsfe();
	*err = TRUE_;
	goto L340;
    }
L330:
/*  NORMAL COMPLETION */
L340:
    return 0;
} /* pline_ */

#ifdef __cplusplus
	}
#endif

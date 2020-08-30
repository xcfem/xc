/* strsiz.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int strsiz_(integer *maxnp, real *x, real *y, integer *nint, 
	integer *n, real *xend, real *yend, real *xdiff, real *ydiff, real *
	d__, logical *err, logical *test, real *xnold, real *ynold, integer *
	nxkold, integer *linkeg, integer *listeg, real *bmesur, integer *
	mlink, integer *npnold, integer *npeold, integer *nnxk, logical *
	remesh, real *rexmin, real *rexmax, real *reymin, real *reymax, 
	integer *idivis, real *sizmin, real *emax, real *emin, logical *graph,
	 real *dxmax)
{
    /* System generated locals */
    integer nxkold_dim1, nxkold_offset, i__1, i__2;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, j;
    static real s1, s2, s3, dt, dx1, dx2, eps, dist1, dist2, size1, size2, 
	    size3, xnew1, ynew1, xnew2, ynew2;
    static logical moved;
    static real ratio;
    extern /* Subroutine */ int mpd2vc_(integer *, real *, real *, real *, 
	    real *), mesage_(char *, ftnlen);
    static real dratio;
    static logical sgraph;
    static integer iterat;
    extern /* Subroutine */ int lcolor_(char *, ftnlen);
    static real dtotal;
    extern /* Subroutine */ int symbol_(integer *, real *, real *, char *, 
	    ftnlen), pltflu_(), getsiz_(real *, real *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    logical *, real *, real *, real *, real *, integer *, real *, 
	    real *, real *, real *, real *, real *);
    static real tratio, dstnow;
    static integer intnow;
    static real siznow;

/* *********************************************************************** */
/*  SUBROUTINE STRSIZ = GETS INTERVALS ON A SRAIGHT LINE BASED ON ERROR */
/*                      SIZE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --y;
    --x;
    linkeg -= 3;
    --bmesur;
    --ynold;
    --xnold;
    --listeg;
    nxkold_dim1 = *nnxk;
    nxkold_offset = 1 + nxkold_dim1;
    nxkold -= nxkold_offset;

    /* Function Body */
    if (*graph) {
	sgraph = TRUE_;
    } else {
	sgraph = FALSE_;
    }
    iterat = 100;
    eps = (float).01;
    dstnow = (float)0.;
    intnow = 0;
    siznow = (float)0.;
    if (*graph) {
	symbol_(&c__1, &x[1], &y[1], "DIAMND", (ftnlen)6);
	pltflu_();
    }
L100:
    ++intnow;
    if (dstnow + siznow * (float)1.3 > *d__) {
/*  THE END OF THE LINE (OR CLOSE ENOUGH) HAS BEEN REACHED */
/*  IF WE ARE TESTING OR THE INTERVALS MATCH, THEN SIMPLY FINISH THE */
/*  LINE. */
	if (*test || intnow == *nint) {
	    *nint = intnow;
	    *n = *nint + 1;
	    x[*n] = *xend;
	    y[*n] = *yend;
	    if (*graph) {
		symbol_(&c__1, &x[intnow], &y[intnow], "DIAMND", (ftnlen)6);
		mpd2vc_(&c__1, &x[intnow], &y[intnow], &x[*n], &y[*n]);
		symbol_(&c__1, &x[*n], &y[*n], "DIAMND", (ftnlen)6);
		pltflu_();
	    }
	} else {
/*  OTHERWISE, MAKE SURE THE INTERVALS ARE ALRIGHT AND ADD THE EXTRA ONE */
	    eps = (float).001;
	    if (intnow + 1 != *nint) {
		mesage_("** PROBLEMS WITH INTNOW IN PLINE **", (ftnlen)35);
		*err = TRUE_;
		goto L160;
	    }
	    x[intnow + 1] = (x[intnow] + *xend) * (float).5;
	    y[intnow + 1] = (y[intnow] + *yend) * (float).5;
	    *n = *nint + 1;
	    x[*n] = *xend;
	    y[*n] = *yend;
	    if (*graph) {
		symbol_(&c__1, &x[intnow + 1], &y[intnow + 1], "DIAMND", (
			ftnlen)6);
		symbol_(&c__1, &x[*n], &y[*n], "DIAMND", (ftnlen)6);
		mpd2vc_(&c__1, &x[intnow], &y[intnow], &x[intnow + 1], &y[
			intnow + 1]);
		mpd2vc_(&c__1, &x[intnow + 1], &y[intnow + 1], &x[*n], &y[*n])
			;
		pltflu_();
	    }
	}
    } else {
/*  NOT TO THE END YET */
	getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], &
		listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, remesh, 
		rexmin, rexmax, reymin, reymax, idivis, sizmin, emax, emin, &
		x[intnow], &y[intnow], &s1);
	xnew1 = x[1] + (dstnow + s1) / *d__ * *xdiff;
	ynew1 = y[1] + (dstnow + s1) / *d__ * *ydiff;
	getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], &
		listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, remesh, 
		rexmin, rexmax, reymin, reymax, idivis, sizmin, emax, emin, &
		xnew1, &ynew1, &s2);
	xnew2 = x[1] + (dstnow + (s1 + s2) * (float).5) / *d__ * *xdiff;
	ynew2 = y[1] + (dstnow + (s1 + s2) * (float).5) / *d__ * *ydiff;
	getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], &
		listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, remesh, 
		rexmin, rexmax, reymin, reymax, idivis, sizmin, emax, emin, &
		xnew2, &ynew2, &s3);
	siznow = ((s1 + s2) * (float).5 + s3) * (float).5;
	dstnow += siznow;
	x[intnow + 1] = x[1] + dstnow / *d__ * *xdiff;
	y[intnow + 1] = y[1] + dstnow / *d__ * *ydiff;
	if (*graph) {
	    symbol_(&c__1, &x[intnow + 1], &y[intnow + 1], "DIAMND", (ftnlen)
		    6);
	    mpd2vc_(&c__1, &x[intnow], &y[intnow], &x[intnow + 1], &y[intnow 
		    + 1]);
	    pltflu_();
	}
	goto L100;
    }
/*  ERASE THE NODES FOR REPLOTTING IF NEEDED */
    if (! sgraph && *graph) {
	i__1 = *nint;
	for (j = 2; j <= i__1; ++j) {
	    lcolor_("BLACK", (ftnlen)5);
	    mpd2vc_(&c__1, &x[j], &y[j], &x[j + 1], &y[j + 1]);
	    mpd2vc_(&c__1, &x[j], &y[j], &x[j - 1], &y[j - 1]);
	    symbol_(&c__1, &x[j], &y[j], "DIAMND", (ftnlen)6);
	    lcolor_("WHITE", (ftnlen)5);
	    pltflu_();
/* L110: */
	}
    }
/*  NOW SMOOTH THE NODES ALONG THE LINE */
    i__1 = iterat;
    for (i__ = 1; i__ <= i__1; ++i__) {
	moved = FALSE_;
	i__2 = *nint;
	for (j = 2; j <= i__2; ++j) {
	    getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], 
		    &listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
		    remesh, rexmin, rexmax, reymin, reymax, idivis, sizmin, 
		    emax, emin, &x[j - 1], &y[j - 1], &size1);
	    getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], 
		    &listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
		    remesh, rexmin, rexmax, reymin, reymax, idivis, sizmin, 
		    emax, emin, &x[j], &y[j], &size2);
	    getsiz_(&xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], 
		    &listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, 
		    remesh, rexmin, rexmax, reymin, reymax, idivis, sizmin, 
		    emax, emin, &x[j + 1], &y[j + 1], &size3);
/* Computing 2nd power */
	    r__1 = x[j - 1] - x[j];
/* Computing 2nd power */
	    r__2 = y[j - 1] - y[j];
	    dist1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	    r__1 = x[j] - x[j + 1];
/* Computing 2nd power */
	    r__2 = y[j] - y[j + 1];
	    dist2 = sqrt(r__1 * r__1 + r__2 * r__2);
	    dtotal = dist1 + dist2;
	    ratio = dist1 / dtotal;
	    dratio = (size1 + size2) * (float).5 / ((size1 + size2) * (float)
		    .5 + (size2 + size3) * (float).5);
	    tratio = (ratio + dratio) * (float).5;
	    if (sgraph) {
		lcolor_("BLACK", (ftnlen)5);
		mpd2vc_(&c__1, &x[j], &y[j], &x[j + 1], &y[j + 1]);
		mpd2vc_(&c__1, &x[j], &y[j], &x[j - 1], &y[j - 1]);
		symbol_(&c__1, &x[j], &y[j], "DIAMND", (ftnlen)6);
		lcolor_("WHITE", (ftnlen)5);
		pltflu_();
	    }
	    x[j] = tratio * (x[j + 1] - x[j - 1]) + x[j - 1];
	    y[j] = tratio * (y[j + 1] - y[j - 1]) + y[j - 1];
	    if (sgraph) {
		mpd2vc_(&c__1, &x[j], &y[j], &x[j + 1], &y[j + 1]);
		mpd2vc_(&c__1, &x[j], &y[j], &x[j - 1], &y[j - 1]);
		symbol_(&c__1, &x[j], &y[j], "DIAMND", (ftnlen)6);
		pltflu_();
	    }
	    dx1 = dist1 / ((size1 + size2) * (float).5);
	    dx2 = dist2 / ((size2 + size3) * (float).5);
	    if (j == 2) {
		*dxmax = dmax(dx1,dx2);
	    } else {
/* Computing MAX */
		r__1 = max(*dxmax,dx1);
		*dxmax = dmax(r__1,dx2);
	    }
	    dt = (r__1 = tratio * dtotal - dist1, dabs(r__1));
	    if (dt / dtotal > eps) {
		moved = TRUE_;
	    }
/* L120: */
	}
	if (! moved) {
	    goto L140;
	}
/* L130: */
    }
L140:
    if (! sgraph && *graph) {
	i__1 = *nint;
	for (j = 2; j <= i__1; ++j) {
	    mpd2vc_(&c__1, &x[j], &y[j], &x[j + 1], &y[j + 1]);
	    mpd2vc_(&c__1, &x[j], &y[j], &x[j - 1], &y[j - 1]);
	    symbol_(&c__1, &x[j], &y[j], "DIAMND", (ftnlen)6);
	    pltflu_();
/* L150: */
	}
    }
L160:
    return 0;
} /* strsiz_ */

#ifdef __cplusplus
	}
#endif

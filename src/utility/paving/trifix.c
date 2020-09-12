/* trifix.f -- translated by f2c (version 20160102).
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
#include "paving.h"

/* Table of constant values */

static integer c__3 = 3;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int trifix_(integer *mxnd, integer *mln, xc_float *xn, xc_float *yn, 
	xc_float *zn, integer *nuid, integer *lxk, integer *kxl, integer *nxl, 
	integer *lxn, integer *nnn, integer *lll, integer *kkk, integer *
	navail, integer *iavail, xc_float *angle, integer *lnodes, xc_float *bnsize, 
	integer *nloop, char *dev1, integer *kreg, xc_float *xmin, xc_float *xmax, 
	xc_float *ymin, xc_float *ymax, xc_float *zmin, xc_float *zmaxz, logical *graph, 
	logical *video, logical *noroom, logical *err, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;
    xc_float r__1, r__2, r__3, r__4, r__5, r__6;

    /* Local variables */
    static integer i__, i0, i1, i2;
    static xc_float pi, ang;
    static logical ccw;
    static xc_float ang1, ang2, area;
    static logical done, redo;
    static integer kmax;
    static xc_float dist, zmax;
    static integer kelem, nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, xc_float *, xc_float *, integer *, 
	    integer *, xc_float *, integer *, integer *, logical *);
    static xc_float toler;
    static integer kount;
    static xc_float twopi;
    static integer kkkadd;
    extern /* Subroutine */ int longel_(integer *, integer *, integer *, xc_float 
	    *, xc_float *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, xc_float *, 
	    xc_float *, integer *, integer *, integer *, xc_float *, xc_float *, xc_float *, 
	    xc_float *, integer *, integer *, logical *, logical *, logical *, 
	    logical *, logical *, integer *), filsmo_(integer *, integer *, 
	    xc_float *, xc_float *, xc_float *, integer *, integer *, integer *, integer *
	    , integer *, integer *, integer *, integer *, xc_float *, integer *, 
	    xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, char *, integer *,
	     ftnlen), snapit_(integer *), rplotl_(integer *, xc_float *, xc_float *, 
	    xc_float *, integer *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *,
	     integer *, char *, integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE TRIFIX = CHECKS ALL ELEMENTS FOR ANY TRIANGULAR SHAPED */
/*                      LONG ELEMENT AND DELETES THEM WHEN */
/*                      FOUND AND POSSIBLE */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --zn;
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    pi = M_PI;
    twopi = pi * (xc_float)2.;
    *err = FALSE_;
    done = FALSE_;
    ccw = TRUE_;
    kmax = 30;
    kount = 0;
/*  TOLERANCE IS SET AT 150 DEGREES */
    toler = (xc_float)2.6179939;
L100:
    ++kount;
    if (kount > kmax) {
	goto L140;
    }
    redo = FALSE_;
    i__1 = *kkk;
    for (kelem = 1; kelem <= i__1; ++kelem) {
	if (lxk[(kelem << 2) + 1] > 0) {
	    gnxka_(mxnd, &xn[1], &yn[1], &kelem, nodes, &area, &lxk[5], &nxl[
		    3], &ccw);
	    for (i__ = 1; i__ <= 4; ++i__) {
		i1 = nodes[i__ - 1];
		if (i__ == 1) {
		    i0 = nodes[3];
		    i2 = nodes[1];
		} else if (i__ == 4) {
		    i0 = nodes[2];
		    i2 = nodes[0];
		} else {
		    i0 = nodes[i__ - 2];
		    i2 = nodes[i__];
		}
		ang1 = atan2(yn[i0] - yn[i1], xn[i0] - xn[i1]);
		if (ang1 < (xc_float)0.) {
		    ang1 += twopi;
		}
		ang2 = atan2(yn[i2] - yn[i1], xn[i2] - xn[i1]);
		if (ang2 < (xc_float)0.) {
		    ang2 += twopi;
		}
		ang = ang1 - ang2;
		if (ang < (xc_float)0.) {
		    ang += twopi;
		}
		longel_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &
			nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], nnn, 
			navail, iavail, &i1, &kelem, &ang, &toler, &i0, &i2, 
			kreg, xmin, xmax, ymin, ymax, kkk, lll, &done, graph, 
			video, noroom, err, &kkkadd);
		if (*noroom || *err) {
		    goto L140;
		}
		if (done) {
		    if (*graph && ! (*video)) {
/* Computing MAX */
			r__5 = (r__1 = xn[i0] - xn[i1], dbl_abs(r__1)), r__6 = (
				r__2 = xn[i2] - xn[i1], dbl_abs(r__2)), r__5 = 
				dbl_max(r__5,r__6), r__6 = (r__3 = yn[i0] - yn[i1]
				, dbl_abs(r__3)), r__5 = dbl_max(r__5,r__6), r__6 = (
				r__4 = yn[i2] - yn[i1], dbl_abs(r__4));
			dist = dbl_max(r__5,r__6) * (xc_float)3.;
			*xmin = xn[i1] - dist;
			*xmax = xn[i1] + dist;
			*ymin = yn[i1] - dist;
			*ymax = yn[i1] + dist;
		    }
		    if (*video) {
			rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, 
				xmax, ymin, ymax, zmin, &zmax, lll, dev1, 
				kreg, (ftnlen)3);
			snapit_(&c__3);
		    }
		    filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[
			    3], &nxl[3], &lxn[5], lll, nnn, nnn, &lnodes[
			    lnodes_offset], &bnsize[3], nloop, xmin, xmax, 
			    ymin, ymax, zmin, &zmax, dev1, kreg, (ftnlen)3);
		    if (*graph || *video) {
			rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, 
				xmax, ymin, ymax, zmin, &zmax, lll, dev1, 
				kreg, (ftnlen)3);
			if (*video) {
			    snapit_(&c__3);
			}
		    }
		    done = FALSE_;
		    redo = TRUE_;
		    goto L120;
		}
/* L110: */
	    }
L120:
	    ;
	}
/* L130: */
    }
    if (redo) {
	goto L100;
    }
L140:
    return 0;
} /* trifix_ */

#ifdef __cplusplus
	}
#endif

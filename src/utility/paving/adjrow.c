/* adjrow.f -- translated by f2c (version 20160102).
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
//#include "f2c.h"
#include "paving.h"

/* Common Block Declarations */

struct {
    real timea, timep, timec, timepc, timeaj, times;
} timing_;

#define timing_1 timing_

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int adjrow_(integer *mxnd, integer *mln, integer *nuid, real 
	*xn, real *yn, real *zn, integer *lxk, integer *kxl, integer *nxl, 
	integer *lxn, real *angle, real *bnsize, integer *lnodes, integer *
	nloop, integer *iavail, integer *navail, real *xmin, real *xmax, real 
	*ymin, real *ymax, real *zmin, real *zmax, char *dev1, integer *lll, 
	integer *kkk, integer *nnn, integer *lllold, integer *nnnold, integer 
	*node, integer *nadj1, integer *nadj2, integer *nnn2, logical *graph, 
	logical *video, integer *kreg, real *defsiz, logical *adjted, logical 
	*noroom, logical *err, ftnlen dev1_len)
{
    /* Initialized data */

    static real tmin1 = (float).8;
    static real tmin2 = (float).3;
    static real wmin1 = (float)1.25;
    static real wmin2 = (float)1.35;

    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1, i__2;
    real r__1, r__2, r__3, r__4;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal);

    /* Local variables */
    static integer n0, n1, n2, n11;
    static real pi, eps;
    static integer kang;
    static real tang, dist, teps, time1, time2;
    static integer n11old;
    static real ratio;
    static integer kount;
    extern /* Subroutine */ int addwdg_(integer *, integer *, integer *, real 
	    *, real *, real *, integer *, integer *, integer *, integer *, 
	    integer *, real *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, real *, real *, real *, real *, 
	    real *, real *, logical *, logical *, char *, integer *, logical *
	    , logical *, ftnlen), mesage_(char *, ftnlen), getime_(real *), 
	    addtuk_(integer *, integer *, integer *, real *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, real *, real *
	    , real *, real *, real *, real *, logical *, logical *, char *, 
	    logical *, logical *, ftnlen);
    static integer idepth;
    extern /* Subroutine */ int lupang_(integer *, integer *, real *, real *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, real *, real *, real *, 
	    real *, real *, real *, char *, integer *, logical *, ftnlen), 
	    filsmo_(integer *, integer *, real *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    real *, real *, char *, integer *, ftnlen), snapit_(integer *), 
	    rplotl_(integer *, real *, real *, real *, integer *, real *, 
	    real *, real *, real *, real *, real *, integer *, char *, 
	    integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE ADJROW = ADJUSTS A ROW OF ELEMENTS BETWEEN TWO CORNERS */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --zn;
    --yn;
    --xn;
    --nuid;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    getime_(&time1);
    *err = FALSE_;
    eps = (float).0523599;
/*  START BY SETTING UP THE LIMITS OF THE SEARCH */
    if (*nadj1 == *nadj2) {
	n2 = lnodes[*nadj1 * lnodes_dim1 + 3];
	kount = 0;
L100:
	++kount;
	if (angle[n2] >= pi - eps && angle[n2] <= pi + eps) {
	    *nadj1 = n2;
	    *nadj2 = n2;
	    teps = (pi - (doublereal) (*nloop - 2) * pi / (doublereal) (*
		    nloop)) * (float).95;
	    if (teps <= eps) {
		eps = teps;
	    }
	    goto L110;
	} else if (n2 == *nadj2) {
	    teps = (pi - (doublereal) (*nloop - 2) * pi / (doublereal) (*
		    nloop)) * (float).95;
	    if (teps <= eps) {
		eps = teps;
	    }
	    goto L110;
	} else if (kount > *nloop) {
	    mesage_("** PROBLEMS IN ADJROW WITH LOOP NOT CLOSING **", (ftnlen)
		    46);
	    *err = TRUE_;
	    goto L160;
	} else {
	    n2 = lnodes[n2 * lnodes_dim1 + 3];
	    goto L100;
	}
    }
L110:
    n1 = lnodes[*nadj1 * lnodes_dim1 + 3];
    *adjted = FALSE_;
L120:
    if (n1 == *nadj2) {
	goto L150;
    }
/*  CHECK A STRING OF CONCAVE (< PI) INTERIOR ANGLES FOR NEEDING A */
/*  TUCK INSERTED SOMEWHERE */
    if (angle[n1] < pi - eps && lnodes[n1 * lnodes_dim1 + 8] > 1 && lxn[(n1 <<
	     2) + 4] == 0 && lxn[(n1 << 2) + 3] > 0) {
/*  ADDED UP THE TURNING ANGLE AND THE AVERAGE SIZE REDUCTION */
	tang = (float)0.;
	kang = 0;
	ratio = (float)0.;
	n11 = n1;
L130:
	tang += pi - angle[n11];
	++kang;
	n0 = lnodes[n11 * lnodes_dim1 + 2];
	n2 = lnodes[n11 * lnodes_dim1 + 3];
/* Computing 2nd power */
	r__1 = xn[n0] - xn[n11];
/* Computing 2nd power */
	r__2 = yn[n0] - yn[n11];
/* Computing 2nd power */
	r__3 = xn[n2] - xn[n11];
/* Computing 2nd power */
	r__4 = yn[n2] - yn[n11];
	dist = (sqrt(r__1 * r__1 + r__2 * r__2) + sqrt(r__3 * r__3 + r__4 * 
		r__4)) * (float).5;
	if (*defsiz > (float)0.) {
	    if (dist < *defsiz) {
		ratio += dist / bnsize[(n11 << 1) + 1];
	    } else {
		ratio += dist / *defsiz;
	    }
	} else {
	    ratio += dist / bnsize[(n11 << 1) + 1];
	}
	n11 = lnodes[n11 * lnodes_dim1 + 3];
	if (n11 != *nadj2 && angle[n11] < pi - eps && lxn[(n11 << 2) + 4] == 
		0 && lxn[(n11 << 2) + 3] > 0 && lnodes[n11 * lnodes_dim1 + 8] 
		> 1) {
	    goto L130;
	}
	kang = kang;
/*  NOW SEE IF THIS PORTION OF THE ROW NEEDS ADJUSTED WITH A TUCK(S) */
	if (kang >= 1) {
	    ratio /= (doublereal) kang;
/* **               THIS CRITERIA SHOULD HELP ALLEVIATE THE LONG SKINNY */
/* **               ELEMENT FORMATIONS WHEN TRANSITIONING. */
	    if (ratio < tmin1 && tang > (float)1.2217 || ratio < tmin2 && 
		    tang > (float).9) {
		if (*graph || *video) {
		    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax,
			     ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)
			    3);
		    if (*video) {
			snapit_(&c__1);
		    }
		}
		n11old = n11;
		addtuk_(mxnd, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[5], 
			&kxl[3], &nxl[3], &lxn[5], &lnodes[lnodes_offset], &
			angle[1], nloop, iavail, navail, lll, kkk, nnn, &tang,
			 &kang, &n1, &n11, node, xmin, xmax, ymin, ymax, zmin,
			 zmax, graph, video, dev1, noroom, err, (ftnlen)3);
		if (*noroom || *err) {
		    goto L160;
		}
/*  MAKE SURE THAT THE TUCK DOES NOT ELIMINATE THE END NODES FOR THE LOOP */
		if (n11 != n11old) {
		    if (*nadj2 == n11old) {
			*nadj2 = n11;
		    }
		    if (*node == n11old) {
			*node = n11;
		    }
		}
		*nnnold = *nnn;
		*lllold = *lll;
		*adjted = TRUE_;
	    }
	}
	n1 = n11;
	goto L120;
/*  CHECK A STRING OF CONVEX (> PI) INTERIOR ANGLES FOR NEEDING A */
/*  WEDGE INSERTED SOMEWHERE */
    } else if (angle[n1] >= pi + eps && lxn[(n1 << 2) + 3] > 0 && lxn[(n1 << 
	    2) + 4] == 0) {
/*  ADD UP THE TURNING ANGLE AND THE AVERAGE SIZE REDUCTION */
	tang = (float)0.;
	kang = 0;
	ratio = (float)0.;
	idepth = 0;
	n11 = n1;
L140:
	tang += angle[n11] - pi;
	++kang;
	n0 = lnodes[n11 * lnodes_dim1 + 2];
	n2 = lnodes[n11 * lnodes_dim1 + 3];
/* Computing 2nd power */
	r__1 = xn[n0] - xn[n11];
/* Computing 2nd power */
	r__2 = yn[n0] - yn[n11];
/* Computing 2nd power */
	r__3 = xn[n2] - xn[n11];
/* Computing 2nd power */
	r__4 = yn[n2] - yn[n11];
	dist = (sqrt(r__1 * r__1 + r__2 * r__2) + sqrt(r__3 * r__3 + r__4 * 
		r__4)) * (float).5;
	if (*defsiz > (float)0.) {
	    if (dist > *defsiz) {
		ratio += dist / bnsize[(n11 << 1) + 1];
	    } else {
		ratio += dist / *defsiz;
	    }
	} else {
	    ratio += dist / bnsize[(n11 << 1) + 1];
	}
	n11 = lnodes[n11 * lnodes_dim1 + 3];
/* Computing MAX */
	i__1 = idepth, i__2 = lnodes[n11 * lnodes_dim1 + 8];
	idepth = int_max(i__1,i__2);
	if (n11 != *nadj2 && angle[n11] >= pi + eps && lxn[(n11 << 2) + 4] == 
		0 && lxn[(n11 << 2) + 3] > 0) {
	    goto L140;
	}
/*  NOW SEE IF THIS PORTION OF THE ROW NEEDS ADJUSTED WITH A WEDGE(S) */
	if (kang >= 1) {
	    ratio /= (doublereal) kang;
	    if ((ratio > wmin1 && idepth > 1 || ratio > wmin2 && idepth == 1) 
		    && tang > (float)1.2217) {
		if (*graph || *video) {
		    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax,
			     ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)
			    3);
		    if (*video) {
			snapit_(&c__1);
		    }
		}
		addwdg_(mxnd, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[5], 
			&kxl[3], &nxl[3], &lxn[5], &lnodes[lnodes_offset], &
			angle[1], &bnsize[3], nloop, iavail, navail, lll, kkk,
			 nnn, lllold, nnnold, &tang, &kang, &n1, &n11, xmin, 
			xmax, ymin, ymax, zmin, zmax, graph, video, dev1, 
			kreg, noroom, err, (ftnlen)3);
		if (*noroom || *err) {
		    goto L160;
		}
		*nnnold = *nnn;
		*lllold = *lll;
		*adjted = TRUE_;
	    }
	}
	n1 = n11;
	goto L120;
    } else {
	n1 = lnodes[n1 * lnodes_dim1 + 3];
	goto L120;
    }
/*  NOW SMOOTH, CALCULATE THE NEW ANGLES, AND PLOT IF NEEDED */
L150:
    if (*adjted) {
	getime_(&time2);
	timing_1.timeaj = timing_1.timeaj + time2 - time1;
	filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], 
		&lxn[5], lll, nnn, nnn2, &lnodes[lnodes_offset], &bnsize[3], 
		nloop, xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, (
		ftnlen)3);
	getime_(&time1);
	lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], 
		&lxn[5], nloop, &angle[1], &lnodes[lnodes_offset], &n1, lll, 
		xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, err, (ftnlen)
		3);
	if (*err) {
	    goto L160;
	}
	if (*graph || *video) {
	    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, 
		    ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
	    if (*video) {
		snapit_(&c__1);
	    }
	}
    }
L160:
    getime_(&time2);
    timing_1.timeaj = timing_1.timeaj + time2 - time1;
    return 0;
} /* adjrow_ */

#ifdef __cplusplus
	}
#endif

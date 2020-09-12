/* filsmo.f -- translated by f2c (version 20160102).
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

/* Common Block Declarations */

struct {
    xc_float timea, timep, timec, timepc, timeaj, times;
} timing_;

#define timing_1 timing_

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int filsmo_(integer *mxnd, integer *mln, xc_float *xn, xc_float *yn, 
	xc_float *zn, integer *lxk, integer *kxl, integer *nxl, integer *lxn, 
	integer *lll, integer *nnn, integer *nnn2, integer *lnodes, xc_float *
	bnsize, integer *nloop, xc_float *xmin, xc_float *xmax, xc_float *ymin, xc_float *
	ymax, xc_float *zmin, xc_float *zmax, char *dev1, integer *kreg, ftnlen 
	dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1, i__2, i__3;
    xc_float r__1, r__2;

    /* Builtin functions */
    

    /* Local variables */
    static integer i__, l, n0, n2;
    static xc_float al;
    static integer ii, il;
    static xc_float dx;
    static integer it;
    static xc_float fx, fy, sl, ro, vl, dy, wt;
    static logical big;
    static xc_float eps;
    static logical err;
    static integer nit;
    static xc_float tol, vro, eps2, wfac;
    static integer nend;
    static logical done;
    static integer node;
    static xc_float delx, dely, wfac2;
    static integer node1, node2;
    static xc_float time1, time2;
    static logical graph;
    static integer idraw, lines[20];
    extern /* Subroutine */ int setn02_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *);
    static xc_float vxdel, vydel;
    extern /* Subroutine */ int mnorm_(integer *, xc_float *, xc_float *, integer *, 
	    integer *, xc_float *);
    static integer kount;
    extern /* Subroutine */ int d2node_(integer *, xc_float *, xc_float *, integer *, 
	    integer *), eqlang_(integer *, xc_float *, xc_float *, integer *, integer 
	    *, integer *, integer *, integer *, xc_float *, xc_float *, xc_float *, xc_float *
	    ), getime_(xc_float *);
    static xc_float stdlen;
    extern /* Subroutine */ int lcolor_(char *, ftnlen), getlxn_(integer *, 
	    integer *, integer *, integer *, integer *, logical *), sflush_(),
	     rowsmo_(integer *, integer *, xc_float *, xc_float *, xc_float *, integer *, 
	    integer *, integer *, integer *, integer *, xc_float *, xc_float *, 
	    integer *, xc_float *, xc_float *, integer *, integer *, xc_float *, integer *
	    , logical *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, char 
	    *, integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE FILSMO = MESH SMOOTHING DONE BY ISOPARAMETRIC/EQUAL */
/*                      ANGULAR SMOOTHING OF THE ADDED INTERIOR (FREE) */
/*                      BOUNDARY ROW AND THEN A LENGTH-WEIGHTED/EQUAL */
/*                      ANGULAR BOUNDARY LAPLACIAN OF THE INTERIOR NODES. */
/*                      THE FREE BOUNDARY IS FINALLY SMOOTHED AGAIN. */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     WFAC = WEIGHT (0. = LAPLACIAN, 1. = ISOPARAMETRIC) */
/*     NIT  = THE MAX NUMBER OF ITERATIONS TO DO. */
/*     EPS  = MINIMUM DISTANCE NODES MUST MOVE TO CONTINUE ITERATIONS */
/*     RO   = AN UNDER- OR OVER-RELAXATION FACTOR (NORMALLY 1.0) */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --zn;
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    getime_(&time1);
    graph = FALSE_;
    done = FALSE_;
    wt = (xc_float)10.;
/* Computing MAX */
    i__1 = *nloop * 5;
    nit = int_max(i__1,40);
    tol = (xc_float).03;
    vro = (xc_float)1.;
    ro = (xc_float)1.;
    wfac = (xc_float)1.;
    wfac2 = (xc_float).5;
    mnorm_(mxnd, &xn[1], &yn[1], &nxl[3], lll, &stdlen);
    eps = tol * stdlen;
    if (ro < (xc_float).01) {
	ro = (xc_float)1.;
    }
/* Computing 2nd power */
    r__1 = eps * ro;
    eps2 = r__1 * r__1;
/*  FIRST SMOOTH THE ADDED ROW */
    if (*nloop > 0) {
	rowsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], 
		&lxn[5], nnn, &wfac, &wfac2, &nit, &eps, &ro, nnn2, &lnodes[
		lnodes_offset], &bnsize[3], lll, &graph, xmin, xmax, ymin, 
		ymax, zmin, zmax, dev1, kreg, (ftnlen)3);
    }
/*  NOW SMOOTH THE INTERIOR NODES */
/*  ITERATION LOOP */
    i__1 = nit;
    for (it = 1; it <= i__1; ++it) {
	big = FALSE_;
/*  NODE LOOP */
	i__2 = *nnn;
	for (node = 1; node <= i__2; ++node) {
	    if (lxn[(node << 2) + 1] > 0 && lxn[(node << 2) + 2] > 0 && 
		    lnodes[node * lnodes_dim1 + 4] == -2) {
		done = TRUE_;
		fx = (xc_float)0.;
		fy = (xc_float)0.;
		sl = (xc_float)0.;
		vl = (xc_float)0.;
/*  LOOP THROUGH ALL LINES CONNECTED TO NODE */
		getlxn_(mxnd, &lxn[5], &node, lines, &kount, &err);
		if (err) {
		    goto L150;
		}
		i__3 = kount;
		for (il = 1; il <= i__3; ++il) {
		    l = lines[il - 1];
		    nend = nxl[(l << 1) + 1] + nxl[(l << 1) + 2] - node;
		    dx = xn[nend] - xn[node];
		    dy = yn[nend] - yn[node];
		    al = sqrt(dx * dx + dy * dy);
/*  CHECK FOR A BOUNDARY NODE AT THE OTHER END */
/*  OF THE LINE - TRY TO AVERAGE ANGULAR ERRORS WITH THE BOUNDARY WHERE */
/*  POSSIBLE - THIS MEANS ADDING IN AN EXTRA VECTOR TO PULL THE NODE */
/*  BACK TO WHERE IT OUGHT TO BE TO BE AT EQUAL ANGLES */
		    if (lxn[(nend << 2) + 2] < 0) {
			setn02_(mxnd, &nxl[3], &lxk[5], &kxl[3], &l, &nend, &
				node, &n0, &n2);
			eqlang_(mxnd, &xn[1], &yn[1], &lxn[5], &node, &n0, &
				n2, &nend, &al, &vro, &vxdel, &vydel);
			vl = sqrt(vxdel * vxdel + vydel * vydel);
			fx += vxdel * wt * vl;
			fy += vydel * wt * vl;
			sl += vl * wt;
		    }
		    fx += dx * al;
		    fy += dy * al;
		    sl += al;
/* L100: */
		}
/*  MOVE THE NODE */
		delx = ro * fx / sl;
		dely = ro * fy / sl;
/*  ERASE THE NODE'S LINES IF GRAPH IS ON */
		if (graph) {
		    lcolor_("BLACK", (ftnlen)5);
		    i__3 = kount;
		    for (ii = 1; ii <= i__3; ++ii) {
			idraw = lines[ii - 1];
			node1 = nxl[(idraw << 1) + 1];
			node2 = nxl[(idraw << 1) + 2];
			d2node_(mxnd, &xn[1], &yn[1], &node1, &node2);
/* L110: */
		    }
		    lcolor_("WHITE", (ftnlen)5);
		}
		xn[node] += delx;
		yn[node] += dely;
/*  REPLOT THE NODE'S LINES IF GRAPH IS ON */
		if (graph) {
		    i__3 = kount;
		    for (ii = 1; ii <= i__3; ++ii) {
			idraw = lines[ii - 1];
			node1 = nxl[(idraw << 1) + 1];
			node2 = nxl[(idraw << 1) + 2];
			d2node_(mxnd, &xn[1], &yn[1], &node1, &node2);
/* L120: */
		    }
		    sflush_();
		}
/* Computing 2nd power */
		r__1 = delx;
/* Computing 2nd power */
		r__2 = dely;
		if (r__1 * r__1 + r__2 * r__2 > eps2) {
		    big = TRUE_;
		}
	    }
/* L130: */
	}
	if (! big) {
	    goto L150;
	}
/* L140: */
    }
L150:
/*  NOW RESMOOTH THE ADDED ROW IF THE MESH HAS CHANGED INTERNALLY */
    if (*nloop > 0 && done) {
	rowsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], 
		&lxn[5], nnn, &wfac, &wfac2, &nit, &eps, &ro, nnn2, &lnodes[
		lnodes_offset], &bnsize[3], lll, &graph, xmin, xmax, ymin, 
		ymax, zmin, zmax, dev1, kreg, (ftnlen)3);
    }
/*  NOW RESET ALL THE NODES AS BEING SMOOTHED */
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	lnodes[i__ * lnodes_dim1 + 4] = (i__2 = lnodes[i__ * lnodes_dim1 + 4],
		 abs(i__2));
/* L160: */
    }
    getime_(&time2);
    timing_1.times = timing_1.times + time2 - time1;
    return 0;
} /* filsmo_ */

#ifdef __cplusplus
	}
#endif

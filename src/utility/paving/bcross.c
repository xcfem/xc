/* bcross.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int bcross_(integer *mxnd, integer *mln, real *xn, real *yn, 
	real *zn, integer *lxk, integer *kxl, integer *nxl, integer *lxn, 
	integer *lnodes, integer *i1, integer *i2, integer *j1, integer *j2, 
	integer *nloop, logical *bok, integer *lll, real *xmin, real *xmax, 
	real *ymin, real *ymax, real *zmin, real *zmax, char *dev1, integer *
	kreg, logical *err, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static real u, w;
    static integer j0, j3, node1, node2;
    static real angle;
    static integer kount;
    extern /* Subroutine */ int getang_(integer *, integer *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, logical *), intsct_(real *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, real *, 
	    logical *);
    static logical lcross;

/* *********************************************************************** */
/*  SUBROUTINE BCROSS = CHECKS TO MAKE SURE THAT A BOUNDARY IS NOT */
/*                      BECOMING A PERMANENT CROSS */
/* *********************************************************************** */
    /* Parameter adjustments */
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
    *bok = TRUE_;
    *err = FALSE_;
    j0 = lnodes[*j1 * lnodes_dim1 + 2];
    j3 = lnodes[*j2 * lnodes_dim1 + 3];
/*  IF J0 TO I2, OR J3 TO I1 IS TO BECOME A BOUNDARY LINE, */
/*  THEN TEST THE CONNECTION TO SEE IF IT INTERSECTS ANY OTHER */
/*  BOUNDARY LINES */
    kount = 0;
/*  FIRST TEST THE J0 TO I2 LINE */
    if (lxn[(j0 << 2) + 2] < 0 && lxn[(*i2 << 2) + 2] < 0) {
	node1 = *i1;
	node2 = *i2;
L100:
	node1 = node2;
	node2 = lnodes[node2 * lnodes_dim1 + 3];
	if (lxn[(node1 << 2) + 2] < 0 && lxn[(node2 << 2) + 2] < 0) {
	    if (node2 == j0) {
		getang_(mxnd, mln, &xn[1], &yn[1], &lnodes[lnodes_offset], &
			lxk[5], &kxl[3], &nxl[3], &lxn[5], &node1, &j0, j1, &
			angle, err);
		if (angle < (float)0.) {
		    *bok = FALSE_;
		    goto L130;
		} else {
		    goto L110;
		}
	    } else {
		intsct_(&xn[node1], &yn[node1], &xn[node2], &yn[node2], &xn[
			j0], &yn[j0], &xn[*i2], &yn[*i2], &u, &w, &lcross);
		if (lcross) {
		    *bok = FALSE_;
		    goto L130;
		}
	    }
	}
	++kount;
	if (kount < *nloop) {
	    goto L100;
	} else {
	    *err = TRUE_;
	    goto L130;
	}
    }
L110:
/*  NEXT TEST THE J3 TO I1 LINE */
    kount = 0;
    if (lxn[(j3 << 2) + 2] < 0 && lxn[(*i1 << 2) + 2] < 0) {
	node1 = j3;
	node2 = lnodes[j3 * lnodes_dim1 + 3];
L120:
	node1 = node2;
	node2 = lnodes[node2 * lnodes_dim1 + 3];
	if (lxn[(node1 << 2) + 2] < 0 && lxn[(node2 << 2) + 2] < 0) {
	    if (node2 == *i1) {
		getang_(mxnd, mln, &xn[1], &yn[1], &lnodes[lnodes_offset], &
			lxk[5], &kxl[3], &nxl[3], &lxn[5], &node1, i1, i2, &
			angle, err);
		if (angle < (float)0.) {
		    *bok = FALSE_;
		    goto L130;
		} else {
		    goto L130;
		}
	    } else {
		intsct_(&xn[node1], &yn[node1], &xn[node2], &yn[node2], &xn[
			j3], &yn[j3], &xn[*i1], &yn[*i1], &u, &w, &lcross);
		if (lcross) {
		    *bok = FALSE_;
		    goto L130;
		}
	    }
	}
	++kount;
	if (kount < *nloop) {
	    goto L120;
	} else {
	    *err = TRUE_;
	    goto L130;
	}
    }
L130:
    return 0;
} /* bcross_ */

#ifdef __cplusplus
	}
#endif

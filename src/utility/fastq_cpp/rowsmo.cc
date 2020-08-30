/* rowsmo.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int rowsmo_(integer *mxnd, integer *mln, real *xn, real *yn, 
	real *zn, integer *lxk, integer *kxl, integer *nxl, integer *lxn, 
	integer *nnn, real *wfac, real *wfac2, integer *nit, real *eps, real *
	ro, integer *nnn2, integer *lnodes, real *bnsize, integer *lll, 
	logical *graph, real *xmin, real *xmax, real *ymin, real *ymax, real *
	zmin, real *zmax, char *dev1, integer *kreg, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1, i__2, i__3;
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), cos(doublereal), sin(doublereal), 
	    sqrt(doublereal);

    /* Local variables */
    static integer j;
    static real r__;
    static integer j1, j2, j3;
    extern /* Subroutine */ int invert_fq__(integer *, integer *, real *, 
	    real *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, real *, real *, real *, 
	    real *, char *, integer *, integer *, real *, real *, ftnlen);
    static integer ii, kk, kl, in;
    static real pi;
    static integer nl;
    static real x11, y11;
    static integer it, ks;
    static real x21, y21, vx, vy;
    static logical big, ccw;
    static real det, dro;
    static logical err;
    static real vro, eps2, area[20];
    static integer nend, node;
    static real xdel;
    static logical aver;
    static real ydel;
    extern /* Subroutine */ int gkxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *);
    static logical test;
    static real xnew, ynew, sumx, sumy;
    static integer node1, node2;
    static real dist0, dist1, dfact;
    static integer idraw, nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);
    static integer lines[20], nfrom, klist[20];
    static real xdist, ydist;
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
	    integer *);
    static logical near2l;
    static real theta1, theta2;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static integer nbegin;
    extern /* Subroutine */ int eqlang_(integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, real *, real *, real *
	    , real *), getfrm_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *), lcolor_(char *, 
	    ftnlen), getlxn_(integer *, integer *, integer *, integer *, 
	    integer *, logical *), sflush_();

/* *********************************************************************** */
/*  SUBROUTINE ROWSMO = SMOOTHS AN ADDED ROW DURING FILLING USING THE */
/*                      ISOPARAMETRIC SMOOTHER WITH SPECIAL CONSIDERATION */
/*                      GIVEN TO THE 2-LINE NODES (ROW CORNERS) */
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
    pi = atan2((float)0., (float)-1.);
    nodes[0] = -1;
    nodes[1] = -1;
    nodes[2] = -1;
    nodes[3] = -1;
    if (*ro < (float).01) {
	*ro = (float)1.;
    }
    dro = (float)1.;
    vro = (float)1.;
    eps2 = *eps * *ro;
    test = FALSE_;
    aver = TRUE_;
/*  ITERATION LOOP */
    i__1 = *nit;
    for (it = 1; it <= i__1; ++it) {
	if (it == *nit) {
	    mesage_("THE ROWSMO ROUTINE IS NOT CONVERGING", (ftnlen)36);
	}
	big = FALSE_;
/*  NODE LOOP */
/* Computing MAX */
	i__2 = *nnn2 - 1;
	nbegin = max(i__2,1);
	nend = *nnn + 1;
	i__2 = nend;
	for (j = nbegin; j <= i__2; ++j) {
	    if (j == nend) {
		node = lnodes[*nnn * lnodes_dim1 + 3];
	    } else if (j == nbegin) {
		node = lnodes[nbegin * lnodes_dim1 + 2];
	    } else {
		node = j;
	    }
/*  SKIP CONTINUATION LINES, EXTERIOR BOUNDARY LINES, AND NODES */
/*  THAT ARE ON THE INTERIOR */
	    if (node > 0) {
		if (lxn[(node << 2) + 1] > 0 && lxn[(node << 2) + 2] > 0 && 
			lnodes[node * lnodes_dim1 + 4] == -1) {
/*  FIND ELEMENTS AND LINES ATTACHED TO NODE */
		    gkxn_(mxnd, &kxl[3], &lxn[5], &node, &ks, klist, &err);
		    getlxn_(mxnd, &lxn[5], &node, lines, &nl, &err);
		    sumx = (float)0.;
		    sumy = (float)0.;
/*  PERFORM AN AREA PULL AND LAPLACIAN */
/*  ON ANY NODE ATTACHED TO A 2-LINE NODE */
/*               TWOL = .FALSE. */
/*               NEAR2L = .FALSE. */
/*               IF (LXN (3, NODE) . EQ. 0) THEN */
/*                  TWOL = .TRUE. */
/*                  NFROM = 0 */
/*               ELSE */
/*                  IF (NL .EQ. 3) THEN */
/*                     DO 100 IL = 1, NL */
/*                        ILL = LINES (IL) */
/*                        IF (NXL (1, ILL) .EQ. NODE) THEN */
/*                           NTEST = NXL (2, ILL) */
/*                        ELSEIF (NXL (2, ILL) .EQ. NODE) THEN */
/*                           NTEST = NXL (1, ILL) */
/*                        ELSE */
/*                           CALL MESAGE ('** PROBLEMS IN ROWSMO **') */
/*                           GOTO 110 */
/*                        ENDIF */
/*                        NODES(IL) = NTEST */
/*                        IF (LXN (3, NTEST) .EQ. 0) THEN */
/*  MAKE SURE THAT THE OTHER END OF THE 2-LINE NODE HAS ONLY 3 LINES */
/*                           IF (LXN (1, NTEST) .EQ. ILL) THEN */
/*                              LTEST = LXN (2, NTEST) */
/*                           ELSEIF (LXN (2, NTEST) .EQ. ILL) THEN */
/*                              LTEST = LXN (1, NTEST) */
/*                           ELSE */
/*                              CALL MESAGE ('** PROBLEMS IN ROWSMO **') */
/*                              GOTO 110 */
/*                           ENDIF */
/*                           IF (NXL (1, LTEST) .EQ. NTEST) THEN */
/*                              NTEST = NXL (2, LTEST) */
/*                           ELSEIF (NXL (2, LTEST) .EQ. NTEST) THEN */
/*                              NTEST = NXL (1, LTEST) */
/*                           ELSE */
/*                              CALL MESAGE ('** PROBLEMS IN ROWSMO **') */
/*                              GOTO 110 */
/*                           ENDIF */
/*                           IF ((LXN (3, NTEST) .GT. 0) .AND. */
/*     &                        (LXN (4, NTEST) .EQ. 0)) THEN */
/*                              NEAR2L = .TRUE. */
/*                           ENDIF */
/*                        ENDIF */
/*  100                CONTINUE */
/*  110                CONTINUE */
/*                     NFROM = NODES(2) */
/*                  ELSE */
/*                     NFROM = 0 */
/*                  ENDIF */
/*               ENDIF */
		    near2l = FALSE_;
		    if (near2l || test) {
			theta1 = atan2(yn[nodes[2]] - yn[nodes[0]], xn[nodes[
				2]] - xn[nodes[0]]) + pi / (float)2.;
			theta2 = atan2(yn[nodes[2]] - yn[nodes[1]], xn[nodes[
				2]] - xn[nodes[1]]) + pi / (float)2.;
			det = -cos(theta1) * sin(theta2) + cos(theta2) * sin(
				theta1);
			x11 = (xn[nodes[0]] + xn[nodes[2]]) * (float).5;
			y11 = (yn[nodes[0]] + yn[nodes[2]]) * (float).5;
			x21 = (xn[nodes[1]] + xn[nodes[2]]) * (float).5;
			y21 = (yn[nodes[1]] + yn[nodes[2]]) * (float).5;
			r__ = (-sin(theta2) * (x21 - x11) + cos(theta2) * (
				y21 - y11)) / det;
			xnew = x11 + r__ * cos(theta1);
			ynew = y11 + r__ * sin(theta1);
			xdel = xnew - xn[node];
			ydel = ynew - yn[node];
/*  PERFORM AN ISOPARAMETRIC SMOOTH ON OTHER NODES */
		    } else {
			i__3 = ks;
			for (kl = 1; kl <= i__3; ++kl) {
			    ccw = FALSE_;
			    kk = klist[kl - 1];
			    gnxka_(mxnd, &xn[1], &yn[1], &kk, nodes, &area[kl 
				    - 1], &lxk[5], &nxl[3], &ccw);
			    for (in = 1; in <= 4; ++in) {
				if (nodes[in - 1] == node) {
				    j1 = in + 1;
				    if (j1 > 4) {
					j1 = 1;
				    }
				    goto L110;
				}
/* L100: */
			    }
L110:
			    j2 = j1 + 1;
			    if (j2 > 4) {
				j2 = 1;
			    }
			    j3 = j2 + 1;
			    if (j3 > 4) {
				j3 = 1;
			    }
			    sumx = sumx + xn[nodes[j1 - 1]] + xn[nodes[j3 - 1]
				    ] - *wfac * xn[nodes[j2 - 1]];
			    sumy = sumy + yn[nodes[j1 - 1]] + yn[nodes[j3 - 1]
				    ] - *wfac * yn[nodes[j2 - 1]];
/* L120: */
			}
			sumx /= (doublereal) ks * ((float)2. - *wfac);
			sumy /= (doublereal) ks * ((float)2. - *wfac);
			xdel = *ro * (sumx - xn[node]);
			ydel = *ro * (sumy - yn[node]);
			getfrm_(mxnd, lines, &nl, &nxl[3], &node, &lnodes[
				node * lnodes_dim1 + 2], &lnodes[node * 
				lnodes_dim1 + 3], &nfrom);
			if (nfrom > 0) {
/*  FACTOR IN THE LENGTH CONSTANT (GENERATED LENGTH) OF THE NODE */
			    dist0 = bnsize[(node << 1) + 1] * bnsize[(node << 
				    1) + 2];
			    xdist = xdel + xn[node] - xn[nfrom];
			    ydist = ydel + yn[node] - yn[nfrom];
/* Computing 2nd power */
			    r__1 = xdist;
/* Computing 2nd power */
			    r__2 = ydist;
			    dist1 = sqrt(r__1 * r__1 + r__2 * r__2);
			    dfact = dist0 / dist1 * dro;
			    sumx = xn[nfrom] + xdist * dfact;
			    sumy = yn[nfrom] + ydist * dfact;
			    xdel = sumx - xn[node];
			    ydel = sumy - yn[node];
/*  FACTOR IN THE EQUAL ANGLE VECTORS */
			    if (lnodes[node * lnodes_dim1 + 2] != lnodes[node 
				    * lnodes_dim1 + 3]) {
				eqlang_(mxnd, &xn[1], &yn[1], &lxn[5], &node, 
					&lnodes[node * lnodes_dim1 + 2], &
					lnodes[node * lnodes_dim1 + 3], &
					nfrom, &dist0, &vro, &vx, &vy);
				if (aver) {
				    xdel = (xdel + vx) * (float).5;
				    ydel = (ydel + vy) * (float).5;
				} else {
				    xdel = vx;
				    ydel = vy;
				}
			    }
			}
		    }
/*  NOW CHECK THAT THE ROW IS NOT BENDING OVER ON ITSELF WITH THIS SMOOTH */
		    if (lxn[(node << 2) + 4] == 0) {
			invert_fq__(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5]
				, &kxl[3], &nxl[3], &lxn[5], lll, &lnodes[
				lnodes_offset], xmin, xmax, ymin, ymax, zmin, 
				zmax, dev1, kreg, &node, &xdel, &ydel, (
				ftnlen)3);
		    }
/*  REDEFINE THIS NODE'S COORDINATES */
/*  AND PLOT THE NEW NODE AND LINES */
		    if (xdel * xdel + ydel * ydel > eps2) {
			big = TRUE_;
		    }
		    if (*graph) {
			lcolor_("BLACK", (ftnlen)5);
			i__3 = nl;
			for (ii = 1; ii <= i__3; ++ii) {
			    idraw = lines[ii - 1];
			    node1 = nxl[(idraw << 1) + 1];
			    node2 = nxl[(idraw << 1) + 2];
			    d2node_(mxnd, &xn[1], &yn[1], &node1, &node2);
/* L130: */
			}
			lcolor_("WHITE", (ftnlen)5);
			sflush_();
		    }
		    xn[node] += xdel;
		    yn[node] += ydel;
		    if (*graph) {
			i__3 = nl;
			for (ii = 1; ii <= i__3; ++ii) {
			    idraw = lines[ii - 1];
			    node1 = nxl[(idraw << 1) + 1];
			    node2 = nxl[(idraw << 1) + 2];
			    d2node_(mxnd, &xn[1], &yn[1], &node1, &node2);
/* L140: */
			}
			sflush_();
		    }
/*  CHECK FOR CONVERGENCE */
		}
	    }
/* L150: */
	}
/*  IF NO SIGNIFICANT MOVEMENTS OCCURRED,  RETURN */
	if (! big) {
	    return 0;
	}
/* L160: */
    }
/*  NOW SMOOTH THE INTERIOR */
    return 0;
} /* rowsmo_ */

#ifdef __cplusplus
	}
#endif

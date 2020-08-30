/* zhole.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int zhole_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *ns, integer *maxnl, integer *maxnp, integer *maxprm, 
	integer *nprm, integer *maxnbc, integer *maxsbc, integer *knbc, 
	integer *ksbc, integer *knum, integer *ipoint, real *coor, integer *
	ipboun, integer *iline, integer *ltype, integer *nint, real *factor, 
	integer *lcon, integer *ilboun, integer *isboun, integer *iside, 
	integer *nlps, integer *ifline, integer *illist, integer *islist, 
	integer *indxh, integer *nppf, integer *ifpb, integer *listpb, 
	integer *nlpf, integer *iflb, integer *listlb, integer *nspf, integer 
	*ifsb, integer *listsb, integer *linkp, integer *linkl, integer *
	links, integer *linkpb, integer *linklb, integer *linksb, real *x, 
	real *y, integer *nid, integer *listl, integer *marked, integer *nl, 
	integer *lstnbc, integer *mxnd, real *xn, real *yn, integer *nuid, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, integer *nxh, 
	integer *nperim, integer *nnn, integer *nnnold, integer *kkk, integer 
	*lll, integer *iavail, integer *navail, integer *jhole, integer *
	inside, real *eps, logical *noroom, logical *err, real *amesur, real *
	xnold, real *ynold, integer *nxkold, integer *mmpold, integer *linkeg,
	 integer *listeg, real *bmesur, integer *mlink, integer *nprold, 
	integer *npnold, integer *npeold, integer *nnxk, logical *remesh, 
	real *rexmin, real *rexmax, real *reymin, real *reymax, integer *
	idivis, real *sizmin, real *emax, real *emin)
{
    /* System generated locals */
    integer nid_dim1, nid_offset, nxkold_dim1, nxkold_offset, i__1, i__2, 
	    i__3;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal), acos(doublereal), atan2(doublereal, doublereal);

    /* Local variables */
    static integer i__, j, k;
    static real r__;
    static integer i1, i2, j1, i3, j2, j3, nh;
    static real pi;
    static integer nl1, ks1;
    static logical ccw;
    static integer lin;
    static real sum;
    static integer nlp1;
    static real area, diff;
    static logical ldel;
    static integer near__;
    static logical even;
    static real xcen, ycen, dist;
    extern /* Subroutine */ int gkxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *);
    static integer lnum;
    static real xmin, xmax, ymin, ymax;
    static integer npnt, numl, next, nsum, nnnx;
    static real agold;
    static logical lreal;
    static real agnew;
    static integer nodes[4], lines[20];
    extern /* Subroutine */ int perim_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, logical *, 
	    logical *, logical *, logical *, logical *, real *, real *, real *
	    , integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, real *, 
	    real *, real *, real *, integer *, real *, real *, real *);
    static real small;
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);
    static logical count;
    static integer itemp, nperv;
    static real tdist, spiro, twopi;
    static integer klist1[20];
    static logical addlnk;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static logical delete__;
    extern /* Subroutine */ int delhol_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, logical *);
    static logical lcircl;
    extern /* Subroutine */ int linlen_(integer *, real *, integer *, integer 
	    *, integer *, integer *, integer *, integer *, integer *, integer 
	    *, real *, logical *), innerh_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, logical *);
    extern logical lpntin_(integer *, real *, real *, integer *, real *, real 
	    *);
    static real radius;
    extern /* Subroutine */ int getlxn_(integer *, integer *, integer *, 
	    integer *, integer *, logical *);
    static integer istart;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE ZHOLE  =  REMESHES AROUND HOLE IN REGION */
/* *********************************************************************** */
/*  CHECK FOR INPUT ERRORS */
    /* Parameter adjustments */
    linkpb -= 3;
    linkp -= 3;
    listpb -= 3;
    --ifpb;
    --nppf;
    --ipboun;
    coor -= 3;
    --ipoint;
    linksb -= 3;
    linklb -= 3;
    linkl -= 3;
    listsb -= 3;
    --ifsb;
    --nspf;
    listlb -= 3;
    --iflb;
    --nlpf;
    --isboun;
    --ilboun;
    lcon -= 4;
    --factor;
    --nint;
    --ltype;
    --iline;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --iside;
    --islist;
    --listl;
    --y;
    --x;
    --nperim;
    nid_dim1 = *maxnp;
    nid_offset = 1 + nid_dim1;
    nid -= nid_offset;
    --lstnbc;
    --nxh;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;
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
    *err = FALSE_;
    if (*nnn - *nnnold <= 0) {
	mesage_("NO NODES DEFINED IN REGION", (ftnlen)26);
	*err = TRUE_;
/*  GOOD INPUT */
    } else {
	lnum = (i__1 = islist[*indxh], abs(i__1));
	addlnk = FALSE_;
	ltsort_(ml, &linkl[3], &lnum, &lin, &addlnk);
	lcircl = *ns == 1 && ltype[lin] == 3;
/*  CIRCULAR HOLE */
	if (lcircl) {
	    ltsort_(mp, &linkp[3], &lcon[lin * 3 + 1], &i1, &addlnk);
	    ltsort_(mp, &linkp[3], &lcon[lin * 3 + 2], &i2, &addlnk);
	    if (i1 != i2) {
		mesage_("CIRCULAR HOLE DOES NOT CLOSE", (ftnlen)28);
		*err = TRUE_;
		goto L380;
	    }
	    ltsort_(mp, &linkp[3], &lcon[lin * 3 + 3], &i2, &addlnk);
	    xcen = coor[(i2 << 1) + 1];
	    ycen = coor[(i2 << 1) + 2];
/* Computing 2nd power */
	    r__1 = coor[(i1 << 1) + 1] - xcen;
/* Computing 2nd power */
	    r__2 = coor[(i1 << 1) + 2] - ycen;
	    radius = r__1 * r__1 + r__2 * r__2;
	    if (radius <= (float)0.) {
		mesage_("RADIUS HAS ZERO LENGTH", (ftnlen)22);
		*err = TRUE_;
		goto L380;
	    }
	    xmin = xcen - sqrt(radius);
	    xmax = xcen + sqrt(radius);
	    ymin = ycen - sqrt(radius);
	    ymax = ycen + sqrt(radius);
	    nperv = nint[lin];
/*  NON-CIRCULAR HOLE */
	} else {
	    nlp1 = *nl + 1;
	    ccw = TRUE_;
	    count = FALSE_;
	    even = FALSE_;
	    lreal = FALSE_;
	    perim_(mp, ml, ms, ns, maxnl, maxnp, maxnbc, maxsbc, knbc, ksbc, 
		    knum, &ipoint[1], &coor[3], &ipboun[1], &iline[1], &ltype[
		    1], &nint[1], &factor[1], &lcon[4], &ilboun[1], &isboun[1]
		    , &iside[1], &nlps[1], &ifline[1], &illist[1], &islist[*
		    indxh], &nppf[1], &ifpb[1], &listpb[3], &nlpf[1], &iflb[1]
		    , &listlb[3], &nspf[1], &ifsb[1], &listsb[3], &linkp[3], &
		    linkl[3], &links[3], &linkpb[3], &linklb[3], &linksb[3], &
		    x[1], &y[1], &nid[*nprm * nid_dim1 + 1], &nperim[*nprm], &
		    listl[nlp1], &nl1, &lstnbc[1], marked, &even, &lreal, err,
		     &ccw, &count, noroom, &amesur[1], &xnold[1], &ynold[1], &
		    nxkold[nxkold_offset], &mmpold[4], &linkeg[3], &listeg[1],
		     &bmesur[1], mlink, nprold, npnold, npeold, nnxk, remesh, 
		    rexmin, rexmax, reymin, reymax, idivis, sizmin, emax, 
		    emin);
	    if (*err || *noroom) {
		goto L380;
	    }
	    nperv = nperim[*nprm];
	    npnt = nperv;
	    xmin = x[1];
	    xmax = xmin;
	    ymin = y[1];
	    ymax = ymin;
	    i__1 = npnt;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		xcen += x[i__];
		ycen += y[i__];
/* Computing MIN */
		r__1 = xmin, r__2 = x[i__];
		xmin = dmin(r__1,r__2);
/* Computing MAX */
		r__1 = xmax, r__2 = x[i__];
		xmax = dmax(r__1,r__2);
/* Computing MIN */
		r__1 = ymin, r__2 = y[i__];
		ymin = dmin(r__1,r__2);
/* Computing MAX */
		r__1 = ymax, r__2 = y[i__];
		ymax = dmax(r__1,r__2);
/* L100: */
	    }
	    xcen /= (doublereal) npnt;
	    ycen /= (doublereal) npnt;
/* Computing 2nd power */
	    r__1 = xcen - x[1];
/* Computing 2nd power */
	    r__2 = ycen - y[1];
	    radius = sqrt(r__1 * r__1 + r__2 * r__2);
	    i__1 = npnt;
	    for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing 2nd power */
		r__1 = xcen - x[i__];
/* Computing 2nd power */
		r__2 = ycen - y[i__];
		r__ = sqrt(r__1 * r__1 + r__2 * r__2);
		radius = dmin(radius,r__);
/* L110: */
	    }
	}
/*  INITIALIZE NODES PER (ON) HOLE */
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nxh[i__] = 0;
/* L120: */
	}
/*  DELETE EVERYTHING ATTACHED TO NODES WITHIN HOLE */
	near__ = 0;
	small = (float)0.;
	i__1 = *nnn;
	for (i__ = *nnnold + 1; i__ <= i__1; ++i__) {
	    if (xn[i__] > xmin && xn[i__] < xmax && yn[i__] > ymin && yn[i__] 
		    < ymax) {
/* Computing 2nd power */
		r__1 = xn[i__] - xcen;
/* Computing 2nd power */
		r__2 = yn[i__] - ycen;
		dist = r__1 * r__1 + r__2 * r__2;
		if (lcircl) {
		    ldel = dist < radius;
		} else {
		    ldel = lpntin_(maxnp, &x[1], &y[1], &npnt, &xn[i__], &yn[
			    i__]);
		}
		if (dist < small || near__ == 0) {
		    near__ = i__;
		    small = dist;
		}
		if (ldel) {
		    if (nuid[i__] == 0) {
			delhol_(&i__, mxnd, &lxk[5], &kxl[3], &nxl[3], &lxn[5]
				, &nxh[1], &nuid[1], nnn, iavail, navail, 
				noroom, err);
			if (*noroom || *err) {
			    goto L380;
			}
/*  CANNOT DELETE BOUNDARY NODES */
		    } else {
			mesage_("HOLE CROSSES FIXED BOUNDARY", (ftnlen)27);
			*err = TRUE_;
			goto L380;
		    }
		}
	    }
/* L130: */
	}
/*  PROCESS SMALL CIRCLES (I.E. SMALLER THAN AN ELEMENT) */
	if (small > radius) {
	    ccw = TRUE_;
	    gkxn_(mxnd, &kxl[3], &lxn[5], &near__, &ks1, klist1, err);
	    i__1 = ks1;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		gnxka_(mxnd, &xn[1], &yn[1], &klist1[i__ - 1], nodes, &area, &
			lxk[5], &nxl[3], &ccw);
		sum = (float)0.;
		for (j = 1; j <= 4; ++j) {
		    j1 = j + 1;
		    if (j1 > 4) {
			j1 = 1;
		    }
		    sum += (r__1 = (xn[nodes[j1 - 1]] - xn[nodes[j - 1]]) * (
			    ycen - yn[nodes[j - 1]]) - (xcen - xn[nodes[j - 1]
			    ]) * (yn[nodes[j1 - 1]] - yn[nodes[j - 1]]), dabs(
			    r__1));
/* L140: */
		}
		sum /= (float)2.;
		if ((r__1 = (area - sum) / area, dabs(r__1)) < (float)1e-4) {
		    goto L160;
		}
/* L150: */
	    }
	    mesage_("FAILED TO FIND ELEMENT SURROUNDING SMALL HOLE", (ftnlen)
		    45);
	    *err = TRUE_;
	    goto L380;
L160:
	    for (i__ = 1; i__ <= 4; ++i__) {
		if (nuid[nodes[i__ - 1]] == 0) {
		    delhol_(&nodes[i__ - 1], mxnd, &lxk[5], &kxl[3], &nxl[3], 
			    &lxn[5], &nxh[1], &nuid[1], nnn, iavail, navail, 
			    noroom, err);
		    if (*noroom || *err) {
			goto L380;
		    }
		}
/* L170: */
	    }
	}
/*  SQUARE UP BOUNDARY (DELETE INTERIOR NODES WITH ONLY TWO LINES) */
L180:
	delete__ = FALSE_;
	i__1 = *nnn;
	for (i__ = *nnnold + 1; i__ <= i__1; ++i__) {
	    if (nxh[i__] == 1 && nuid[i__] == 0) {
		getlxn_(mxnd, &lxn[5], &i__, lines, &numl, err);
		if (numl == 2 && nuid[i__] == 0) {
		    delhol_(&i__, mxnd, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
			    nxh[1], &nuid[1], nnn, iavail, navail, noroom, 
			    err);
		    if (*noroom || *err) {
			goto L380;
		    }
		    delete__ = TRUE_;
		}
	    }
/* L190: */
	}
	if (delete__) {
	    goto L180;
	}
/*  GENERATE DELETED ELEMENT BOUNDARY NODE LIST */
	nh = 0;
	i__1 = *nnn;
	for (i__ = *nnnold + 1; i__ <= i__1; ++i__) {
	    if (nxh[i__] > 0) {
		++nh;
		nxh[nh] = i__;
	    }
/* L200: */
	}
/*  ENSURE THAT THERE ARE A MINIMUM OF MIN(12, NPERV) INTERVALS */
/*     AROUND HOLE */
	if (nh < max(12,nperv)) {
	    i__1 = *mxnd;
	    for (i__ = nh + 1; i__ <= i__1; ++i__) {
		nxh[i__] = 0;
/* L210: */
	    }
	    i__1 = nh;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (nuid[nxh[i__]] == 0) {
		    i__2 = *mxnd - nh;
		    delhol_(&nxh[i__], &i__2, &lxk[5], &kxl[3], &nxl[3], &lxn[
			    5], &nxh[nh + 1], &nuid[1], nnn, iavail, navail, 
			    noroom, err);
		    if (*noroom || *err) {
			goto L380;
		    }
		    delete__ = TRUE_;
		} else {
		    i1 = nxh[i__] + nh;
		    nxh[i1] = 1;
		}
/* L220: */
	    }
	    if (delete__) {
		i1 = 0;
		i__1 = *mxnd;
		for (i__ = nh + 1; i__ <= i__1; ++i__) {
		    ++i1;
		    nxh[i1] = nxh[i__];
/* L230: */
		}
		goto L180;
	    } else {
		mesage_("INTERVAL MISMATCH BETWEEN HOLE AND BOUNDARY.", (
			ftnlen)44);
		mesage_("ALL ELEMENTS DELETED.", (ftnlen)21);
		*err = TRUE_;
		goto L380;
	    }
	}
/*  ORDER THE INTERIOR NODE LIST */
	i__1 = nh - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    getlxn_(mxnd, &lxn[5], &nxh[i__], lines, &numl, err);
	    i__2 = numl;
	    for (j = 1; j <= i__2; ++j) {
		j1 = nxl[(lines[j - 1] << 1) + 2] + nxl[(lines[j - 1] << 1) + 
			1] - nxh[i__];
		i__3 = nh;
		for (k = i__ + 1; k <= i__3; ++k) {
		    if (nxh[k] == j1) {
			nxh[k] = nxh[i__ + 1];
			nxh[i__ + 1] = j1;
			goto L260;
		    }
/* L240: */
		}
/* L250: */
	    }
L260:
	    ;
	}
/*  MAKE SURE LOOP CLOSES */
	getlxn_(mxnd, &lxn[5], &nxh[nh], lines, &numl, err);
	i__1 = numl;
	for (j = 1; j <= i__1; ++j) {
	    j1 = nxl[(lines[j - 1] << 1) + 2] + nxl[(lines[j - 1] << 1) + 1] 
		    - nxh[nh];
	    if (nxh[1] == j1) {
		goto L280;
	    }
/* L270: */
	}
	mesage_("HOLE PERIMETER DOES NOT CLOSE", (ftnlen)29);
	*err = TRUE_;
	goto L380;
L280:
/*  MAKE SURE HOLE PERIMETER IS DEFINED COUNTER-CLOCKWISE */
	pi = acos((float)-1.);
	twopi = pi + pi;
	spiro = (float)0.;
	agold = atan2(yn[nxh[1]] - yn[nxh[nh]], xn[nxh[1]] - xn[nxh[nh]]);
	i__1 = nh;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (i__ == nh) {
		next = 1;
	    } else {
		next = i__ + 1;
	    }
	    agnew = atan2(yn[nxh[next]] - yn[nxh[i__]], xn[nxh[next]] - xn[
		    nxh[i__]]);
	    diff = agnew - agold;
	    if (diff > pi) {
		diff -= twopi;
	    } else if (diff < -pi) {
		diff += twopi;
	    }
	    if ((r__1 = dabs(diff) - pi, dabs(r__1)) < (float).001) {
		mesage_("PERIMETER CONTAINS SWITCHBACKS", (ftnlen)30);
		*err = TRUE_;
		goto L380;
	    }
	    spiro += diff;
	    agold = agnew;
/* L290: */
	}
	if (spiro < (float)0.) {
	    i__1 = nh / 2;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		itemp = nxh[i__];
		nxh[i__] = nxh[nh - i__ + 1];
		nxh[nh - i__ + 1] = itemp;
/* L300: */
	    }
	} else if (dabs(spiro) < pi || dabs(spiro) > pi * (float)3.) {
	    mesage_("UNABLE TO DETERMINE CW OR CCW SENSE OF HOLE", (ftnlen)43)
		    ;
	    *err = TRUE_;
	    goto L380;
	}
/*  FIND THE BEST STARTING POINT ON THE CIRCULAR HOLE */
	if (*nnn + nh > *mxnd) {
	    *noroom = TRUE_;
	    goto L380;
	}
/*  GENERATE THE PERIMETER OF THE HOLE */
	even = TRUE_;
	ccw = TRUE_;
	lreal = TRUE_;
	count = TRUE_;
	if (nperv != nh) {
	    if (lcircl) {
		nint[lin] = nh;
	    } else {
		tdist = (float)0.;
		i__1 = nl1;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    lnum = listl[*nl + i__];
		    ltsort_(ml, &linkl[3], &lnum, &lin, &addlnk);
		    i1 = lcon[lin * 3 + 1];
		    i2 = lcon[lin * 3 + 2];
		    i3 = lcon[lin * 3 + 3];
		    ltsort_(mp, &linkp[3], &i1, &j1, &addlnk);
		    ltsort_(mp, &linkp[3], &i2, &j2, &addlnk);
		    if (i3 != 0) {
			i__2 = abs(i3);
			ltsort_(mp, &linkp[3], &i__2, &j3, &addlnk);
			if (i3 < 0) {
			    j3 = -j3;
			}
		    } else {
			j3 = 0;
		    }
		    linlen_(mp, &coor[3], &linkp[3], jhole, &iline[lin], &
			    ltype[lin], &i3, &j1, &j2, &j3, &dist, err);
		    if (*err) {
			goto L380;
		    }
		    tdist += dist;
/* L310: */
		}
		nsum = 0;
		i__1 = nl1 - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    lnum = listl[*nl + i__];
		    ltsort_(ml, &linkl[3], &lnum, &lin, &addlnk);
		    i1 = lcon[lin * 3 + 1];
		    i2 = lcon[lin * 3 + 2];
		    i3 = lcon[lin * 3 + 3];
		    ltsort_(mp, &linkp[3], &i1, &j1, &addlnk);
		    ltsort_(mp, &linkp[3], &i2, &j2, &addlnk);
		    if (i3 != 0) {
			i__2 = abs(i3);
			ltsort_(mp, &linkp[3], &i__2, &j3, &addlnk);
			if (i3 < 0) {
			    j3 = -j3;
			}
		    } else {
			j3 = 0;
		    }
		    linlen_(mp, &coor[3], &linkp[3], jhole, &iline[lin], &
			    ltype[lin], &i3, &j1, &j2, &j3, &dist, err);
		    if (*err) {
			goto L380;
		    }
		    nint[lin] = (integer) (nh * dist / tdist + (float).5);
		    nsum += nint[lin];
/* L320: */
		}
		lnum = listl[*nl + nl1];
		ltsort_(ml, &linkl[3], &lnum, &lin, &addlnk);
		nint[lin] = nh - nsum;
	    }
	    mesage_("INTERVALS MODIFIED FOR HOLE", (ftnlen)27);
	}
	nlp1 = *nl + 1;
	perim_(mp, ml, ms, ns, maxnl, maxnp, maxnbc, maxsbc, knbc, ksbc, knum,
		 &ipoint[1], &coor[3], &ipboun[1], &iline[1], &ltype[1], &
		nint[1], &factor[1], &lcon[4], &ilboun[1], &isboun[1], &iside[
		1], &nlps[1], &ifline[1], &illist[1], &islist[*indxh], &nppf[
		1], &ifpb[1], &listpb[3], &nlpf[1], &iflb[1], &listlb[3], &
		nspf[1], &ifsb[1], &listsb[3], &linkp[3], &linkl[3], &links[3]
		, &linkpb[3], &linklb[3], &linksb[3], &x[1], &y[1], &nid[*
		nprm * nid_dim1 + 1], &nperim[*nprm], &listl[nlp1], &nl1, &
		lstnbc[1], marked, &even, &lreal, err, &ccw, &count, noroom, &
		amesur[1], &xnold[1], &ynold[1], &nxkold[nxkold_offset], &
		mmpold[4], &linkeg[3], &listeg[1], &bmesur[1], mlink, nprold, 
		npnold, npeold, nnxk, remesh, rexmin, rexmax, reymin, reymax, 
		idivis, sizmin, emax, emin);
	if (*noroom || *err) {
	    goto L380;
	}
	if (nl1 >= 0 && *nl + nl1 <= *maxnl) {
	    *nl += nl1;
	} else {
	    mesage_("UNABLE TO ADD HOLE LINES TO REGION LINE LIST", (ftnlen)
		    44);
	    *err = TRUE_;
	    goto L380;
	}
/*  TACK THE HOLE LINE LIST ONTO THE BOUNDARY LINE LIST */
	if (nperim[*nprm] != nh) {
	    mesage_("INTERVAL MISMATCH ON HOLE PERIMETER", (ftnlen)35);
	    *err = TRUE_;
	    goto L380;
	}
	istart = 0;
	dist = (float)0.;
	i__1 = nh;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    sum = (float)0.;
	    i1 = i__ - 1;
	    i__2 = nh;
	    for (j = 1; j <= i__2; ++j) {
		++i1;
		if (i1 > nh) {
		    i1 = 1;
		}
		i2 = nxh[i1];
/* Computing 2nd power */
		r__1 = xn[i2] - x[j];
/* Computing 2nd power */
		r__2 = yn[i2] - y[j];
		sum = sum + r__1 * r__1 + r__2 * r__2;
/* L330: */
	    }
	    if (sum < dist || istart == 0) {
		dist = sum;
		istart = i__;
	    }
/* L340: */
	}
	nnnx = *nnn;
	i__1 = nh;
	for (j = 1; j <= i__1; ++j) {
	    ++(*nnn);
	    xn[*nnn] = x[j];
	    yn[*nnn] = y[j];
	    nuid[*nnn] = nid[j + *nprm * nid_dim1];
/* L350: */
	}
/*  FIRST ROW OF ELEMENTS */
	innerh_(mxnd, &nxh[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], 
		kkk, lll, nnn, &nnnx, &nh, &istart, iavail, navail, noroom, 
		err);
	if (*noroom || *err) {
	    goto L380;
	}
/*  INSERT INNER NECKLACE OF ELEMENTS */
	istart = 1;
	i__1 = *inside;
	for (j = 1; j <= i__1; ++j) {
	    nnnx = *nnn;
	    i__2 = nh;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		++(*nnn);
		nxh[i__] = nnnx - nh + i__;
		xn[*nnn] = xn[nxh[i__]];
		yn[*nnn] = yn[nxh[i__]];
		nuid[*nnn] = nuid[nxh[i__]];
		nuid[nxh[i__]] = 0;
		lxn[(nxh[i__] << 2) + 2] = (i__3 = lxn[(nxh[i__] << 2) + 2], 
			abs(i__3));
/* L360: */
	    }
	    innerh_(mxnd, &nxh[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[
		    5], kkk, lll, nnn, &nnnx, &nh, &istart, iavail, navail, 
		    noroom, err);
	    if (*noroom || *err) {
		goto L380;
	    }
/* L370: */
	}
    }
L380:
    return 0;
} /* zhole_ */

#ifdef __cplusplus
	}
#endif

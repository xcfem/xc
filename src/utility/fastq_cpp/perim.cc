/* perim.f -- translated by f2c (version 20160102).
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

static real c_b2 = (float)0.;
static integer c__0 = 0;
static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int perim_(integer *mp, integer *ml, integer *ms, integer *
	ns, integer *maxnl, integer *maxnp, integer *maxnbc, integer *maxsbc, 
	integer *knbc, integer *ksbc, integer *knum, integer *ipoint, real *
	coor, integer *ipboun, integer *iline, integer *ltype, integer *nint, 
	real *factor, integer *lcon, integer *ilboun, integer *isboun, 
	integer *iside, integer *nlps, integer *ifline, integer *illist, 
	integer *islist, integer *nppf, integer *ifpb, integer *listpb, 
	integer *nlpf, integer *iflb, integer *listlb, integer *nspf, integer 
	*ifsb, integer *listsb, integer *linkp, integer *linkl, integer *
	links, integer *linkpb, integer *linklb, integer *linksb, real *x, 
	real *y, integer *nid, integer *n, integer *listl, integer *nl, 
	integer *lstnbc, integer *marked, logical *even, logical *real__, 
	logical *err, logical *ccw, logical *count, logical *noroom, real *
	amesur, real *xnold, real *ynold, integer *nxkold, integer *mmpold, 
	integer *linkeg, integer *listeg, real *bmesur, integer *mlink, 
	integer *nprold, integer *npnold, integer *npeold, integer *nnxk, 
	logical *remesh, real *rexmin, real *rexmax, real *reymin, real *
	reymax, integer *idivis, real *sizmin, real *emax, real *emin)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 LINE\002,i5,\002 DOES NOT CONNECT TO \
THE\002,/,\002 PREVIOUS SECTION OF THE PERIMETER\002)";
    static char fmt_10010[] = "(\002 IN REGION\002,i5,\002 NO LINE IS ALTERA\
BLE TO ENFORCE\002,/,\002 AN EVEN NUMBER OF PERIMETER POINTS\002)";
    static char fmt_10020[] = "(\002 NO. OF INTERVALS ON LINE\002,i5,\002 WA\
S INCREASED BY 1\002)";

    /* System generated locals */
    integer nxkold_dim1, nxkold_offset, i__1, i__2, i__3;
    real r__1, r__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    double sqrt(doublereal);
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    static real d__;
    static integer i__, j1, j2, k1, k2, il, kp;
    static real dx;
    static integer ix, il1, il2, ip1, ip2, ip3;
    static real xx1, xx2, yy1, yy2;
    static integer ili;
    static logical test;
    static real xrat, yrat;
    static logical graph;
    static real xdimd, ydimd;
    extern /* Subroutine */ int pline_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    real *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, logical *, 
	    logical *, logical *, logical *, real *, real *, real *, integer *
	    , integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, real *, real *, real *
	    , real *, integer *, real *, real *, real *, logical *, real *);
    static real dxmax, xdimr, ydimr;
    extern /* Subroutine */ int llist_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *);
    static integer ipntr;
    extern /* Subroutine */ int mport2_(real *, real *, real *, real *);
    static logical addlnk;
    extern /* Subroutine */ int cclock_(real *, real *, integer *, logical *, 
	    logical *, logical *), mesage_(char *, ftnlen);
    static logical itried;
    static integer nidsav;
    static logical indetr;
    extern /* Subroutine */ int pltbgn_(), mpview_(real *, real *, real *, 
	    real *), pltfrm_(integer *);
    static integer numint, imaxnp;
    extern /* Subroutine */ int revers_(real *, integer *), irever_(integer *,
	     integer *), ltsort_(integer *, integer *, integer *, integer *, 
	    logical *);

    /* Fortran I/O blocks */
    static cilist io___22 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___35 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___36 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___37 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___38 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___41 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE PERIM = GENERATES THE PERIMETER OF A REGION */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     QMESH = GENERATES THE QUADRILATERAL MESH */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     X   = THE X VALUES OF THE PERIMETER LIST */
/*     Y   = THE Y VALUES OF THE PERIMETER LIST */
/*     NID = AN ARRAY OF UNIQUE NODE IDENTIFIERS WITHIN THE BODY. */
/*           IF 00000YYYYY,  YYYYY IS AN INDEX INTO THE POINT TABLE. */
/*           IF 1XXXXYYYYY,  XXXX IS AN INDEX INTO THE LINE TABLE. */
/*     N   = THE NUMBER OF NODES ON THE PERIMETER */
/*     ERR = .TRUE. IF ERRORS WERE ENCOUNTERED */
/* *********************************************************************** */
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
    marked -= 4;
    --listl;
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
    *n = 0;
    addlnk = FALSE_;
    graph = FALSE_;
    itried = FALSE_;
    if (graph) {
	xdimd = (float)1.;
	ydimd = (float).75;
	pltbgn_();
	xdimr = *rexmax - *rexmin;
	ydimr = *reymax - *reymin;
	mpview_(&c_b2, &xdimd, &c_b2, &ydimd);
	xrat = xdimr / xdimd;
	yrat = ydimr / ydimd;
	if (xrat < yrat) {
	    xdimr = xdimd * yrat;
	    xx1 = (*rexmin + *rexmax - xdimr) * (float).5;
	    xx2 = (*rexmin + *rexmax + xdimr) * (float).5;
	    xdimr = xx2 - xx1;
	    yy1 = *reymin;
	    yy2 = *reymax;
	} else {
	    ydimr = ydimd * xrat;
	    yy1 = (*reymin + *reymax - ydimr) * (float).5;
	    yy2 = (*reymin + *reymax + ydimr) * (float).5;
	    ydimr = yy2 - yy1;
	    xx1 = *rexmin;
	    xx2 = *rexmax;
	}
	xx1 -= xdimr * (float).1;
	xx2 += xdimr * (float).1;
	yy1 -= ydimr * (float).1;
	yy2 += ydimr * (float).1;
	mport2_(&xx1, &xx2, &yy1, &yy2);
	pltfrm_(&c__0);
    }
/*  GET LIST OF LINES */
    llist_(ms, ml, maxnl, ns, nl, knum, &listl[1], &iline[1], &iside[1], &
	    nlps[1], &ifline[1], &illist[1], &lcon[4], &islist[1], &links[3], 
	    &linkl[3], err);
    if (*err) {
	return 0;
    }
    *err = TRUE_;
/*  DEFINE VALUE OF KP,  THE BEGINNING CONNECTIVITY POINT */
    if (*nl < 2) {
	ltsort_(ml, &linkl[3], &listl[1], &ili, &addlnk);
	kp = lcon[ili * 3 + 1];
    } else {
	ltsort_(ml, &linkl[3], &listl[1], &il1, &addlnk);
	ltsort_(ml, &linkl[3], &listl[2], &il2, &addlnk);
	j1 = lcon[il1 * 3 + 1];
	j2 = lcon[il1 * 3 + 2];
	k1 = lcon[il2 * 3 + 1];
	k2 = lcon[il2 * 3 + 2];
	kp = 0;
	if (j1 == k1 || j1 == k2) {
	    kp = j2;
	}
	if (j2 == k1 || j2 == k2) {
	    kp = j1;
	}
	ili = il2;
	if (kp == 0) {
	    s_wsfe(&io___22);
	    do_fio(&c__1, (char *)&iline[il2], (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
    }
/*  IF PERIMETER HAS ODD NUMBER OF POINTS, */
/*  DO A TRIAL PERIMETER GENERATION TO SEE WHERE TO INSERT A NODE. */
    if (*even) {
	numint = 0;
	i__1 = *nl;
	for (il = 1; il <= i__1; ++il) {
	    ltsort_(ml, &linkl[3], &listl[il], &ili, &addlnk);
	    numint += (i__2 = nint[ili], abs(i__2));
/* L100: */
	}
	if (numint % 2 == 1 || *remesh) {
	    dx = (float)0.;
	    ix = 0;
	    i__1 = *nl;
	    for (il = 1; il <= i__1; ++il) {
		ltsort_(ml, &linkl[3], &listl[il], &ili, &addlnk);
/*  SKIP PREVIOUSLY USED LINES */
/*  SKIP LINES USED TWICE IN THIS BOUNDARY  (RECALL ANNULUS) */
		if (nint[ili] > 0) {
		    i__2 = *nl;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			ltsort_(ml, &linkl[3], &listl[i__], &ipntr, &addlnk);
			if (i__ != il && ipntr == ili) {
			    goto L130;
			}
/* L110: */
		    }
		    ltsort_(mp, &linkp[3], &lcon[ili * 3 + 1], &ip1, &addlnk);
		    ltsort_(mp, &linkp[3], &lcon[ili * 3 + 2], &ip2, &addlnk);
		    if (lcon[ili * 3 + 3] > 0) {
			ltsort_(mp, &linkp[3], &lcon[ili * 3 + 3], &ip3, &
				addlnk);
		    } else if (lcon[ili * 3 + 3] < 0) {
			i__3 = (i__2 = lcon[ili * 3 + 3], abs(i__2));
			ltsort_(mp, &linkp[3], &i__3, &ip3, &addlnk);
			ip3 = -ip3;
		    } else {
			ip3 = 0;
		    }
		    test = TRUE_;
		    pline_(mp, ml, maxnp, maxnbc, maxsbc, &ipoint[1], &coor[3]
			    , &linkp[3], &iline[ili], &ltype[ili], &nint[ili],
			     &factor[ili], &ip1, &ip2, &ip3, &x[1], &y[1], &
			    nid[1], &ipboun[ip1], &ipboun[ip2], &ilboun[ili], 
			    &isboun[ili], &linkpb[3], &nppf[1], &ifpb[1], &
			    listpb[3], &linklb[3], &nlpf[1], &iflb[1], &
			    listlb[3], &linksb[3], &nspf[1], &ifsb[1], &
			    listsb[3], &lstnbc[1], knbc, ksbc, err, &test, 
			    real__, count, noroom, &amesur[1], &xnold[1], &
			    ynold[1], &nxkold[nxkold_offset], &mmpold[4], &
			    linkeg[3], &listeg[1], &bmesur[1], mlink, nprold, 
			    npnold, npeold, nnxk, remesh, rexmin, rexmax, 
			    reymin, reymax, idivis, sizmin, emax, emin, &
			    graph, &dxmax);
		    if (*err) {
			return 0;
		    }
		    if (*remesh) {
			if (il == 1) {
			    ix = ili;
			    dx = dxmax;
			} else {
			    if (dxmax > dx) {
				ix = ili;
				dx = dxmax;
			    }
			}
		    } else {
			i__3 = (i__2 = nint[ili], abs(i__2)) + 1;
			for (i__ = 2; i__ <= i__3; ++i__) {
/* Computing 2nd power */
			    r__1 = x[i__] - x[i__ - 1];
/* Computing 2nd power */
			    r__2 = y[i__] - y[i__ - 1];
			    d__ = sqrt(r__1 * r__1 + r__2 * r__2);
			    if (d__ > dx) {
				dx = d__;
				ix = ili;
			    }
/* L120: */
			}
		    }
		}
L130:
/* L140: */
		;
	    }
	    if (ix == 0) {
		*err = TRUE_;
		s_wsfe(&io___35);
		do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		e_wsfe();
		return 0;
	    }
/*  RECALCULATE THE NUMBER OF INTERVALS IF REMESHING */
	    if (*remesh) {
		numint = 0;
		i__1 = *nl;
		for (il = 1; il <= i__1; ++il) {
		    ltsort_(ml, &linkl[3], &listl[il], &ili, &addlnk);
		    numint += (i__3 = nint[ili], abs(i__3));
/* L150: */
		}
		if (numint % 2 == 1) {
		    ++nint[ix];
		    s_wsfe(&io___36);
		    do_fio(&c__1, (char *)&iline[ix], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		}
	    } else {
		++nint[ix];
		s_wsfe(&io___37);
		do_fio(&c__1, (char *)&iline[ix], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
    }
/*  NOW LOOP THROUGH THE LINES TO GENERATE THE PERIMETER */
    if (graph) {
	pltbgn_();
	mpview_(&c_b2, &xdimd, &c_b2, &ydimd);
	mport2_(&xx1, &xx2, &yy1, &yy2);
	pltfrm_(&c__0);
    }
L160:
    j1 = kp;
    *n = 0;
    i__1 = *nl;
    for (il = 1; il <= i__1; ++il) {
	ltsort_(ml, &linkl[3], &listl[il], &ili, &addlnk);
	k1 = lcon[ili * 3 + 1];
	k2 = lcon[ili * 3 + 2];
	if (k1 != kp && k2 != kp) {
	    s_wsfe(&io___38);
	    do_fio(&c__1, (char *)&iline[ili], (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
	if (*n > 0) {
	    nidsav = nid[*n + 1];
	}
	ltsort_(mp, &linkp[3], &lcon[ili * 3 + 1], &ip1, &addlnk);
	ltsort_(mp, &linkp[3], &lcon[ili * 3 + 2], &ip2, &addlnk);
	if (lcon[ili * 3 + 3] > 0) {
	    ltsort_(mp, &linkp[3], &lcon[ili * 3 + 3], &ip3, &addlnk);
	} else if (lcon[ili * 3 + 3] < 0) {
	    i__2 = (i__3 = lcon[ili * 3 + 3], abs(i__3));
	    ltsort_(mp, &linkp[3], &i__2, &ip3, &addlnk);
	    ip3 = -ip3;
	} else {
	    ip3 = 0;
	}
	imaxnp = *maxnp - *n;
	test = FALSE_;
	if (imaxnp < nint[ili]) {
	    s_stop("ERROR: Intervals larger than space", (ftnlen)34);
	}
	pline_(mp, ml, &imaxnp, maxnbc, maxsbc, &ipoint[1], &coor[3], &linkp[
		3], &iline[ili], &ltype[ili], &nint[ili], &factor[ili], &ip1, 
		&ip2, &ip3, &x[*n + 1], &y[*n + 1], &nid[*n + 1], &ipboun[ip1]
		, &ipboun[ip2], &ilboun[ili], &isboun[ili], &linkpb[3], &nppf[
		1], &ifpb[1], &listpb[3], &linklb[3], &nlpf[1], &iflb[1], &
		listlb[3], &linksb[3], &nspf[1], &ifsb[1], &listsb[3], &
		lstnbc[1], knbc, ksbc, err, &test, real__, count, noroom, &
		amesur[1], &xnold[1], &ynold[1], &nxkold[nxkold_offset], &
		mmpold[4], &linkeg[3], &listeg[1], &bmesur[1], mlink, nprold, 
		npnold, npeold, nnxk, remesh, rexmin, rexmax, reymin, reymax, 
		idivis, sizmin, emax, emin, &graph, &dxmax);
	if (*err) {
	    return 0;
	}
	*err = TRUE_;
/*  REVERSE LINE IF NECESSARY */
	if (k1 != kp) {
	    i__2 = (i__3 = nint[ili], abs(i__3)) + 1;
	    revers_(&x[*n + 1], &i__2);
	    i__2 = (i__3 = nint[ili], abs(i__3)) + 1;
	    revers_(&y[*n + 1], &i__2);
	    i__2 = (i__3 = nint[ili], abs(i__3)) + 1;
	    irever_(&nid[*n + 1], &i__2);
	}
	if (*n > 0) {
	    nid[*n + 1] = nidsav;
	}
/*  FINISH UP WITH THIS LINE */
/*  KP IS THE POINT ON THE FAR END OF THE LINE */
/*  DON'T INCLUDE THE LAST POINT ON THIS LINE IN THE LIST */
	kp = k1 + k2 - kp;
	*n += (i__3 = nint[ili], abs(i__3));
/*  MARK ALL THE LINES AS USED AND */
/*  REMEMBER WHICH HAVE JUST BEEN MARKED */
	if (ipoint[ip1] > 0) {
	    marked[il * 3 + 1] = ip1;
	    ipoint[ip1] = -(i__3 = ipoint[ip1], abs(i__3));
	} else {
	    marked[il * 3 + 1] = 0;
	}
	if (ipoint[ip2] > 0) {
	    marked[il * 3 + 2] = ip2;
	    ipoint[ip2] = -(i__3 = ipoint[ip2], abs(i__3));
	} else {
	    marked[il * 3 + 2] = 0;
	}
	if (nint[ili] > 0) {
	    marked[il * 3 + 3] = ili;
	    nint[ili] = -(i__3 = nint[ili], abs(i__3));
	} else {
	    marked[il * 3 + 3] = 0;
	}
/* L170: */
    }
/*  LINES ARE EXHAUSTED  -  CHECK FOR CIRCULARITY */
    ltsort_(ml, &linkl[3], &listl[1], &ili, &addlnk);
    if (kp != j1) {
	s_wsfe(&io___41);
	do_fio(&c__1, (char *)&iline[ili], (ftnlen)sizeof(integer));
	e_wsfe();
	return 0;
    }
/*  RESET THE JUST MARKED LINES */
    i__1 = *nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (marked[i__ * 3 + 1] > 0) {
	    ipoint[marked[i__ * 3 + 1]] = (i__3 = ipoint[marked[i__ * 3 + 1]],
		     abs(i__3));
	}
	if (marked[i__ * 3 + 2] > 0) {
	    ipoint[marked[i__ * 3 + 2]] = (i__3 = ipoint[marked[i__ * 3 + 2]],
		     abs(i__3));
	}
	if (marked[i__ * 3 + 3] > 0) {
	    nint[marked[i__ * 3 + 3]] = (i__3 = nint[marked[i__ * 3 + 3]], 
		    abs(i__3));
	}
/* L180: */
    }
/*  PERIMETER COMPLETED */
/*  INSURE PROPER ORIENTATION  (COUNTER-CLOCKWISE) */
    cclock_(&x[1], &y[1], n, ccw, err, &indetr);
/*  THE LINE ORIENTATION MAY BE BAD - TRY A SIMPLE FIX */
    if (indetr && ! itried) {
	i__1 = *nl;
	for (il = 1; il <= i__1; ++il) {
	    ltsort_(ml, &linkl[3], &listl[il], &ili, &addlnk);
	    if (lcon[ili * 3 + 1] == lcon[ili * 3 + 2]) {
		lcon[ili * 3 + 3] = -lcon[ili * 3 + 3];
		itried = TRUE_;
		goto L160;
	    }
/* L190: */
	}
    } else if (indetr && itried) {
	mesage_("ORIENTATION OF PERIMETER CANNOT BE DETERMINED", (ftnlen)45);
    }
    if (*err) {
	return 0;
    }
    *err = TRUE_;
    if (! (*ccw)) {
	i__1 = *n - 1;
	revers_(&x[2], &i__1);
	i__1 = *n - 1;
	revers_(&y[2], &i__1);
	i__1 = *n - 1;
	irever_(&nid[2], &i__1);
    }
/*  EXIT */
    *err = FALSE_;
    return 0;
} /* perim_ */

#ifdef __cplusplus
	}
#endif

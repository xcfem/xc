/* spring.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int spring_(integer *mp, integer *ml, integer *ms, integer *
	mxnper, integer *mxnd, integer *maxnbc, integer *maxsbc, integer *l, 
	integer *ipoint, real *coor, integer *ipboun, integer *linkp, integer 
	*iline, integer *ltype, integer *nint, real *factor, integer *lcon, 
	integer *ilboun, integer *isboun, integer *linkl, integer *nlpb, 
	integer *jfline, integer *jllist, integer *linkpb, integer *nppf, 
	integer *ifpb, integer *listpb, integer *linklb, integer *nlpf, 
	integer *iflb, integer *listlb, integer *linksb, integer *nspf, 
	integer *ifsb, integer *listsb, integer *lstnbc, real *x, real *y, 
	integer *nid, real *xn, real *yn, integer *nuid, integer *lxk, 
	integer *nnn, integer *kkk, integer *lll, integer *knbc, integer *
	ksbc, logical *err, logical *addlnk, logical *count, logical *noroom, 
	real *amesur, real *xnold, real *ynold, integer *nxkold, integer *
	mmpold, integer *linkeg, integer *listeg, real *bmesur, integer *
	mlink, integer *nprold, integer *npnold, integer *npeold, integer *
	nnxk, real *remesh, real *rexmin, real *rexmax, real *reymin, real *
	reymax, integer *idivis, real *sizmin, real *emax, real *emin, 
	logical *graph)
{
    /* System generated locals */
    integer nxkold_dim1, nxkold_offset, i__1, i__2;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, j1, j2, nk, kk1, kk2, ip1, ip2, ip3, nnn0, nnn1;
    static logical real__, test;
    static real dist1, dist2;
    extern /* Subroutine */ int pline_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    real *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, logical *, 
	    logical *, logical *, logical *, real *, real *, real *, integer *
	    , integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, real *, real *, 
	    real *, integer *, real *, real *, real *, logical *, real *);
    static real dxmax;
    static integer kount;
    extern /* Subroutine */ int ltsort_(integer *, 
	    integer *, integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE SPRING = GENERATES SPRING ELEMENTS FROM A BARSET */
/* *********************************************************************** */
    /* Parameter adjustments */
    listpb -= 3;
    --ifpb;
    --nppf;
    linkpb -= 3;
    linkp -= 3;
    --ipboun;
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
    linkl -= 3;
    --isboun;
    --ilboun;
    lcon -= 4;
    --factor;
    --nint;
    --ltype;
    --iline;
    --jllist;
    --jfline;
    --nlpb;
    --nid;
    --y;
    --x;
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
    real__ = TRUE_;
    test = FALSE_;
    *kkk = 0;
    *nnn = 0;
    *knbc = 0;
    *ksbc = 0;
    *lll = 1;
/*  CHECK THAT THE BARSET HAS ONLY TWO SIDES AND THAT THE SIDES HAVE */
/*  EQUAL INTERVALS */
    if (nlpb[*l] != 2) {
	*err = TRUE_;
	mesage_(" SPRINGS CAN ONLY BE GENERATED FOR 2 LINE BAR SETS", (ftnlen)
		50);
	goto L130;
    } else {
	j1 = jfline[*l];
	j2 = jfline[*l] + 1;
	ltsort_(ml, &linkl[3], &jllist[j1], &kk1, addlnk);
	ltsort_(ml, &linkl[3], &jllist[j2], &kk2, addlnk);
	if (nint[kk1] != nint[kk2]) {
	    mesage_("SPRING BAR SETS MUST CONTAIN EQUAL INTERVALS ON OPPOSIN\
G SIDES", (ftnlen)62);
	    *err = TRUE_;
	    goto L130;
	}
    }
/*  NOW GENERATE THE NODES FOR THE FIRST LINE */
    ltsort_(mp, &linkp[3], &lcon[kk1 * 3 + 1], &ip1, addlnk);
    ltsort_(mp, &linkp[3], &lcon[kk1 * 3 + 2], &ip2, addlnk);
    if (lcon[kk1 * 3 + 3] > 0) {
	ltsort_(mp, &linkp[3], &lcon[kk1 * 3 + 3], &ip3, addlnk);
    } else if (lcon[kk1 * 3 + 3] < 0) {
	i__2 = (i__1 = lcon[kk1 * 3 + 3], abs(i__1));
	ltsort_(mp, &linkp[3], &i__2, &ip3, addlnk);
	ip3 = -ip3;
    } else {
	ip3 = 0;
    }
    pline_(mp, ml, mxnper, maxnbc, maxsbc, &ipoint[1], &coor[3], &linkp[3], &
	    iline[kk1], &ltype[kk1], &nint[kk1], &factor[kk1], &ip1, &ip2, &
	    ip3, &x[1], &y[1], &nid[1], &ipboun[ip1], &ipboun[ip2], &ilboun[
	    kk1], &isboun[kk1], &linkpb[3], &nppf[1], &ifpb[1], &listpb[3], &
	    linklb[3], &nlpf[1], &iflb[1], &listlb[3], &linksb[3], &nspf[1], &
	    ifsb[1], &listsb[3], lstnbc, knbc, ksbc, err, &test, &real__, 
	    count, noroom, &amesur[1], &xnold[1], &ynold[1], &nxkold[
	    nxkold_offset], &mmpold[4], &linkeg[3], &listeg[1], &bmesur[1], 
	    mlink, nprold, npnold, npeold, nnxk, remesh, rexmin, rexmax, 
	    reymin, reymax, idivis, sizmin, emax, emin, graph, &dxmax);
    if (*err) {
	mesage_("PROBLEMS GENERATING NODES FOR FIRST SPRING LINE", (ftnlen)47)
		;
	goto L130;
    }
/*  ADD THESE NODES TO THE CURRENT LIST */
    nnn0 = *nnn + 1;
    *nnn = *nnn + (i__1 = nint[kk1], abs(i__1)) + 1;
    kount = 0;
    i__1 = *nnn;
    for (i__ = nnn0; i__ <= i__1; ++i__) {
	++kount;
	xn[i__] = x[kount];
	yn[i__] = y[kount];
	nuid[i__] = nid[kount];
/* L100: */
    }
/*  MARK THESE POINTS AND THE LINE AS BEING USED */
    nint[kk1] = -(i__1 = nint[kk1], abs(i__1));
    ipoint[ip1] = -(i__1 = ipoint[ip1], abs(i__1));
    ipoint[ip2] = -(i__1 = ipoint[ip2], abs(i__1));
/*  NOW GENERATE THE NODES FOR THE SECOND LINE */
    ltsort_(mp, &linkp[3], &lcon[kk2 * 3 + 1], &ip1, addlnk);
    ltsort_(mp, &linkp[3], &lcon[kk2 * 3 + 2], &ip2, addlnk);
    if (lcon[kk2 * 3 + 3] > 0) {
	ltsort_(mp, &linkp[3], &lcon[kk2 * 3 + 3], &ip3, addlnk);
    } else if (lcon[kk2 * 3 + 3] < 0) {
	i__2 = (i__1 = lcon[kk2 * 3 + 3], abs(i__1));
	ltsort_(mp, &linkp[3], &i__2, &ip3, addlnk);
	ip3 = -ip3;
    } else {
	ip3 = 0;
    }
    pline_(mp, ml, mxnper, maxnbc, maxsbc, &ipoint[1], &coor[3], &linkp[3], &
	    iline[kk2], &ltype[kk2], &nint[kk2], &factor[kk2], &ip1, &ip2, &
	    ip3, &x[1], &y[1], &nid[1], &ipboun[ip1], &ipboun[ip2], &ilboun[
	    kk2], &isboun[kk2], &linkpb[3], &nppf[1], &ifpb[1], &listpb[3], &
	    linklb[3], &nlpf[1], &iflb[1], &listlb[3], &linksb[3], &nspf[1], &
	    ifsb[1], &listsb[3], lstnbc, knbc, ksbc, err, &test, &real__, 
	    count, noroom, &amesur[1], &xnold[1], &ynold[1], &nxkold[
	    nxkold_offset], &mmpold[4], &linkeg[3], &listeg[1], &bmesur[1], 
	    mlink, nprold, npnold, npeold, nnxk, remesh, rexmin, rexmax, 
	    reymin, reymax, idivis, sizmin, emax, emin, graph, &dxmax);
    if (*err) {
	mesage_("PROBLEMS GENERATING NODES FOR FIRST SPRING LINE", (ftnlen)47)
		;
	goto L130;
    }
/*  ADD THESE NODES TO THE CURRENT LIST */
    nnn1 = *nnn + 1;
    *nnn = *nnn + (i__1 = nint[kk2], abs(i__1)) + 1;
    kount = 0;
    i__1 = *nnn;
    for (i__ = nnn1; i__ <= i__1; ++i__) {
	++kount;
	xn[i__] = x[kount];
	yn[i__] = y[kount];
	nuid[i__] = nid[kount];
/* L110: */
    }
/*  MARK THESE POINTS AND THE LINE AS BEING USED */
    nint[kk2] = -(i__1 = nint[kk2], abs(i__1));
    ipoint[ip1] = -(i__1 = ipoint[ip1], abs(i__1));
    ipoint[ip2] = -(i__1 = ipoint[ip2], abs(i__1));
/*  NOW GENERATE THE ELEMENTS BY SEEING WHICH END OF THE LINE MATCHES */
/*  UP THE CLOSEST. */
/* Computing 2nd power */
    r__1 = xn[nnn0] - xn[nnn1];
/* Computing 2nd power */
    r__2 = yn[nnn0] - yn[nnn1];
    dist1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = xn[nnn0] - xn[*nnn];
/* Computing 2nd power */
    r__2 = yn[nnn0] - yn[*nnn];
    dist2 = sqrt(r__1 * r__1 + r__2 * r__2);
    nk = (i__1 = nint[kk1], abs(i__1)) + 1;
    i__1 = nk;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++(*kkk);
	lxk[(*kkk << 2) + 1] = i__;
	if (dist1 <= dist2) {
	    lxk[(*kkk << 2) + 2] = i__ + nk;
	} else {
	    lxk[(*kkk << 2) + 2] = *nnn - i__ + 1;
	}
	lxk[(*kkk << 2) + 3] = 0;
	lxk[(*kkk << 2) + 4] = 0;
/* L120: */
    }
L130:
    return 0;
} /* spring_ */

#ifdef __cplusplus
	}
#endif

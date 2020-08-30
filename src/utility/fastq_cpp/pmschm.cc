/* pmschm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int pmschm_(integer *nper, integer *nprm, integer *mxnd, 
	integer *mln, integer *mp, integer *ml, integer *ms, integer *mr, 
	integer *nl, integer *maxnl, integer *maxnp, integer *maxprm, integer 
	*maxnb, integer *maxnbc, integer *maxsbc, integer *knbc, integer *
	ksbc, integer *knum, integer *ipoint, real *coor, integer *ipboun, 
	integer *iline, integer *ltype, integer *nint, real *factor, integer *
	lcon, integer *ilboun, integer *isboun, integer *iside, integer *nlps,
	 integer *ifline, integer *illist, integer *islist, integer *iregn, 
	integer *nppf, integer *ifpb, integer *listpb, integer *nlpf, integer 
	*iflb, integer *listlb, integer *nspf, integer *ifsb, integer *listsb,
	 integer *linkp, integer *linkl, integer *links, integer *linkr, 
	integer *linkpb, integer *linklb, integer *linksb, integer *nspr, 
	integer *ifside, real *rsize, integer *ifhole, integer *nhpr, integer 
	*ihlist, real *x, real *y, integer *nid, integer *listl, real *xn, 
	real *yn, real *zn, integer *nuid, integer *lxk, integer *kxl, 
	integer *nxl, integer *lxn, integer *lstnbc, integer *nperim, real *
	angle, real *bnsize, integer *lnodes, integer *linkpr, integer *
	marked, integer *iptper, integer *numper, integer *lperim, integer *
	kkk, integer *nnn, integer *lll, integer *iavail, integer *navail, 
	char *dev1, integer *kreg, integer *ipntrg, logical *batch, logical *
	noroom, logical *err, real *amesur, real *xnold, real *ynold, integer 
	*nxkold, integer *mmpold, integer *linkeg, integer *listeg, real *
	bmesur, integer *mlink, integer *nprold, integer *npnold, integer *
	npeold, integer *nnxk, logical *remesh, real *rexmin, real *rexmax, 
	real *reymin, real *reymax, integer *idivis, real *sizmin, real *emax,
	 real *emin, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, nid_dim1, nid_offset, nxkold_dim1, 
	    nxkold_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j, ir, nl1;
    static logical ccw;
    static integer nlp1, node;
    static logical even, lreal, graph, video;
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
    static logical timer, count;
    static integer ipntr, jpntr;
    static logical addlnk;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static integer nbnode;
    static logical errchk;
    static real defsiz;
    static logical sizeit;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *), dataok_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    logical *, logical *), revers_(real *, integer *), irever_(
	    integer *, integer *), paving_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, real *, integer *
	    , integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *, logical *, real *, logical *, char *, integer *, 
	    logical *, logical *, logical *, real *, real *, real *, integer *
	    , integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, real *, real *, real *
	    , real *, integer *, real *, real *, real *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE FMSCHM = GENERATES AN INITIAL PAVED MESH */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     X     = X VALUES AROUND THE PERIMETER */
/*     Y     = Y VALUES AROUND THE PERIMETER */
/*     NID   = PERIMETER NODE UNIQUE ID'S */
/*     NPER  = NUMBER OF PERIMETER NODES */
/*     ERR   = .TRUE. IF ERRORS WERE ENCOUNTERED */
/*     XN    = GLOBAL X VALUES OF NODES */
/*     YN    = GLOBAL Y VALUES OF NODES */
/*     NUID  = GLOBAL NODE UNIQUE IDENTIFIERS */
/*     LXK   = LINES PER ELEMENT */
/*     KXL   = ELEMENTS PER LINE */
/*     NXL   = NODES PER LINE */
/*     LXN   = LINES PER NODE */
/*  NOTE: */
/*     FOR *XN TABLES A NEGATIVE FLAG IN THE FOURTH COLUMN MEANS */
/*     GO TO THAT ROW FOR A CONTINUATION OF THE LIST.  IN THAT ROW */
/*     THE FIRST ELEMENT WILL BE NEGATED TO INDICATE THAT THIS IS */
/*     A CONTINUATION ROW. */
/*     A NEGATIVE FLAG IN THE SECOND COLUMN OF THE LXN ARRAY MEANS */
/*     THAT THIS NODE IS AN EXTERIOR BOUNDARY NODE. */
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
    --ihlist;
    --nhpr;
    --ifhole;
    --rsize;
    --ifside;
    --nspr;
    linkr -= 3;
    --iregn;
    --islist;
    marked -= 4;
    --listl;
    --y;
    --x;
    --numper;
    --iptper;
    linkpr -= 4;
    --nperim;
    nid_dim1 = *maxnp;
    nid_offset = 1 + nid_dim1;
    nid -= nid_offset;
    --lperim;
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
    addlnk = FALSE_;
    *err = FALSE_;
    graph = FALSE_;
    timer = FALSE_;
    sizeit = FALSE_;
    errchk = TRUE_;
    defsiz = rsize[*ipntrg];
    video = FALSE_;
/*  PUT ALL THE NODES IN THE ORIGINAL PERIMETER INTO THE GLOBAL */
/*  PERIMETER ARRAYS FOR THE PAVING ROUTINE */
    i__1 = *nper;
    for (i__ = 1; i__ <= i__1; ++i__) {
	xn[i__] = x[i__];
	yn[i__] = y[i__];
	nuid[i__] = nid[i__ + nid_dim1];
	lperim[i__] = i__;
/* L100: */
    }
    iptper[1] = 1;
    numper[1] = *nper;
    nbnode = *nper;
/*  NOW GENERATE ALL THE HOLE PERIMETERS */
    ccw = TRUE_;
    count = TRUE_;
    even = TRUE_;
    lreal = TRUE_;
    ltsort_(mr, &linkr[3], kreg, &ipntr, &addlnk);
    i__1 = ifhole[ipntr] + nhpr[ipntr] - 1;
    for (ir = ifhole[ipntr]; ir <= i__1; ++ir) {
	ltsort_(mr, &linkr[3], &ihlist[ir], &jpntr, &addlnk);
	if (jpntr > 0) {
	    ++(*nprm);
	    i__3 = (i__2 = iregn[ir], abs(i__2));
	    dataok_(mp, ml, ms, mr, &jpntr, &i__3, &coor[3], &iline[1], &
		    ltype[1], &nint[1], &lcon[4], &nlps[1], &ifline[1], &
		    illist[1], &nspr[1], &ifside[1], &islist[1], &linkp[3], &
		    linkl[3], &links[3], &rsize[jpntr], &errchk, err);
	    if (*err) {
		goto L130;
	    }
	    nlp1 = *nl + 1;
	    perim_(mp, ml, ms, &nspr[jpntr], maxnl, maxnp, maxnbc, maxsbc, 
		    knbc, ksbc, knum, &ipoint[1], &coor[3], &ipboun[1], &
		    iline[1], &ltype[1], &nint[1], &factor[1], &lcon[4], &
		    ilboun[1], &isboun[1], &iside[1], &nlps[1], &ifline[1], &
		    illist[1], &islist[ifside[jpntr]], &nppf[1], &ifpb[1], &
		    listpb[3], &nlpf[1], &iflb[1], &listlb[3], &nspf[1], &
		    ifsb[1], &listsb[3], &linkp[3], &linkl[3], &links[3], &
		    linkpb[3], &linklb[3], &linksb[3], &x[1], &y[1], &nid[*
		    nprm * nid_dim1 + 1], &nperim[*nprm], &listl[nlp1], &nl1, 
		    &lstnbc[1], &marked[4], &even, &lreal, err, &ccw, &count, 
		    noroom, &amesur[1], &xnold[1], &ynold[1], &nxkold[
		    nxkold_offset], &mmpold[4], &linkeg[3], &listeg[1], &
		    bmesur[1], mlink, nprold, npnold, npeold, nnxk, remesh, 
		    rexmin, rexmax, reymin, reymax, idivis, sizmin, emax, 
		    emin);
	    if (*noroom || *err) {
		goto L130;
	    }
	    if (nl1 >= 0 && *nl + nl1 <= *maxnl) {
		*nl += nl1;
	    } else {
		mesage_("** PROBLEMS IN FMSCHM ADDING HOLE LINES TO REGION L\
INE LIST **", (ftnlen)62);
		*err = TRUE_;
		goto L130;
	    }
/*  NOW LINK THIS HOLE'S PERIMETER NODES TOGETHER INTO THE GLOBAL */
/*  PERIMETER LIST */
	    i__2 = nperim[*nprm] - 1;
	    revers_(&x[2], &i__2);
	    i__2 = nperim[*nprm] - 1;
	    revers_(&y[2], &i__2);
	    i__2 = nperim[*nprm] - 1;
	    irever_(&nid[*nprm * nid_dim1 + 2], &i__2);
	    iptper[*nprm] = nbnode + 1;
	    numper[*nprm] = nperim[*nprm];
	    nbnode += nperim[*nprm];
	    i__2 = nperim[*nprm];
	    for (j = 1; j <= i__2; ++j) {
		node = j + iptper[*nprm] - 1;
		xn[node] = x[j];
		yn[node] = y[j];
		zn[node] = (float)0.;
		nuid[node] = nid[j + *nprm * nid_dim1];
		lperim[node] = node;
/* L110: */
	    }
	}
/* L120: */
    }
/*  NOW MESH THE BOUNDARIES WITH PAVING */
    *nnn = nbnode;
    paving_(&nbnode, nprm, mln, &iptper[1], &numper[1], &lperim[1], &xn[1], &
	    yn[1], &zn[1], &lxk[5], &nxl[3], nnn, lll, kkk, mxnd, &angle[1], &
	    bnsize[3], &lnodes[lnodes_offset], &linkpr[4], &nperim[1], &lxk[5]
	    , &kxl[3], &nxl[3], &lxn[5], &nuid[1], iavail, navail, &graph, &
	    timer, &video, &defsiz, &sizeit, dev1, kreg, batch, noroom, err, &
	    amesur[1], &xnold[1], &ynold[1], &nxkold[nxkold_offset], &mmpold[
	    4], &linkeg[3], &listeg[1], &bmesur[1], mlink, nprold, npnold, 
	    npeold, nnxk, remesh, rexmin, rexmax, reymin, reymax, idivis, 
	    sizmin, emax, emin, (ftnlen)3);
    *err = FALSE_;
    return 0;
/*  ERROR DURING GENERATION */
L130:
    return 0;
} /* pmschm_ */

#ifdef __cplusplus
	}
#endif

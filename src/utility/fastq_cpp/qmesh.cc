/* qmesh.f -- translated by f2c (version 20160102).
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

static integer c__6 = 6;
static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int qmesh_(real *a, integer *ia, integer *mp, integer *ml, 
	integer *ms, integer *mr, integer *msc, integer *mcom, integer *icom, 
	integer *jcom, char *cin, real *rin, integer *iin, integer *kin, 
	integer *iunit, integer *idump, integer *n, integer *ipoint, real *
	coor, integer *ipboun, integer *iline, integer *ltype, integer *nint, 
	real *factor, integer *lcon, integer *ilboun, integer *isboun, 
	integer *iside, integer *nlps, integer *ifline, integer *illist, 
	integer *ibarst, integer *jmat, integer *jcent, integer *nlpb, 
	integer *jfline, integer *jllist, integer *iregn, integer *imat, 
	integer *nspr, integer *ifside, integer *islist, integer *irpb, 
	integer *ipbf, integer *nppf, integer *ifpb, integer *listpb, integer 
	*ilbf, integer *nlpf, integer *iflb, integer *listlb, integer *isbf, 
	integer *nspf, integer *ifsb, integer *listsb, integer *linkp, 
	integer *linkl, integer *links, integer *linkb, integer *linkr, 
	integer *linksc, integer *linkpb, integer *linklb, integer *linksb, 
	real *rsize, integer *ifhole, integer *nhpr, integer *ihlist, integer 
	*irgflg, integer *ischm, char *scheme, char *defsch, real *defsiz, 
	integer *npregn, integer *npnbc, integer *npsbc, integer *npnode, 
	integer *npelem, integer *maxkxn, logical *step, char *dev1, logical *
	three, logical *eight, logical *nine, logical *lgroup, logical *batch,
	 real *amesur, real *xnold, real *ynold, integer *nxkold, integer *
	mmpold, integer *linkeg, integer *listeg, real *bmesur, integer *
	mlink, integer *nprold, integer *npnold, integer *npeold, integer *
	nnxk, logical *remesh, real *rexmin, real *rexmax, real *reymin, real 
	*reymax, integer *idivis, real *sizmin, real *emax, real *emin, 
	ftnlen cin_len, ftnlen scheme_len, ftnlen defsch_len, ftnlen dev1_len)
{
    /* Initialized data */

    static integer iexit = 1;
    static integer iover = 2;
    static integer iquit = 3;

    /* Format strings */
    static char fmt_10000[] = "(\002 INITIAL CHECK BEGUN FOR REGION:\002,i5)";
    static char fmt_10010[] = "(\002 INITIAL CHECK BEGUN FOR GROUP:\002,i5)";
    static char fmt_10020[] = "(\002 ...INITIAL CHECK BEGUN FOR REGION:\002,\
i5)";
    static char fmt_10030[] = "(\002 ** ERROR - REGION\002,i5,\002 IN BODY L\
IST IS INVALID **\002)";
    static char fmt_10040[] = "(\002 INITIAL CHECK BEGUN FOR BAR SET:\002,i5)"
	    ;
    static char fmt_10050[] = "(\002 ** ERROR - LINE GENERATION ERRORS FOR B\
AR  SET\002,i5,\002 **\002)";
    static char fmt_10060[] = "(\002 ** ERROR - BAR SET\002,i5,\002 IN BODY \
LIST IS INVALID **\002)";
    static char fmt_10070[] = "(\002 NOW PROCESSING GROUP: \002,i5)";
    static char fmt_10080[] = "(\002 ...NOW PROCESSING REGION:\002,i5)";
    static char fmt_10100[] = "(\002 GROUP\002,i5,\002 SUCCESSFULLY COMPLETE\
D AND SAVED\002)";
    static char fmt_10090[] = "(\002 NOW PROCESSING REGION:\002,i5)";
    static char fmt_10110[] = "(\002 REGION\002,i5,\002 SUCCESSFULLY COMPLET\
ED AND SAVED\002)";
    static char fmt_10130[] = "(\002 NOW PROCESSING SPRING BAR SET:\002,i5)";
    static char fmt_10120[] = "(\002 NOW PROCESSING BAR SET:\002,i5)";
    static char fmt_10140[] = "(\002 BAR SET\002,i5,\002 SUCCESSFULLY COMPLE\
TED AND SAVED\002)";

    /* System generated locals */
    integer nxkold_dim1, nxkold_offset, i__1, i__2, i__3, i__4, i__5;

    /* Builtin functions */
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, k[30], l, m, i1, j1, j2, i2, m1;
    extern /* Subroutine */ int minmax_fq__(integer *, integer *, real *, 
	    real *, real *, real *, real *, real *);
    static integer kk, ll, nl, iz, ip1, ip2, ip3;
    static logical bar, ccw;
    static integer kkk, lll, mln;
    static logical err;
    static integer nnn, max1, max2, max3, max4, nnn0, knbc, ksbc;
    static logical real__, fill, even, ians, rect;
    static integer mxnd;
    static logical test;
    static integer mxnl;
    static real size;
    static integer nerr, nper, igrp, nprm, knum;
    static real xmin, xmax, ymin, ymax;
    static logical halfc;
    extern /* Subroutine */ int check_(integer *, integer *, integer *);
    static integer icode;
    extern /* Subroutine */ int mddel_(char *, ftnlen);
    static logical final;
    extern /* Subroutine */ int geti12_(integer *, integer *, integer *, char 
	    *, integer *, integer *, integer *, integer *, integer *, ftnlen);
    static integer kksbc;
    static logical graph;
    static integer ihole;
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
    static integer maxnl, maxnp;
    static logical count;
    static integer mused, ipntr;
    static real dxmax;
    static integer maxnb;
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
    static integer iplus, mxrxg;
    extern /* Subroutine */ int mak2el_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, integer *, real *
	    , real *, integer *, integer *, real *, integer *);
    static integer ipntr1, ipntr2;
    static logical addlnk;
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen), dataok_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, logical *, logical *);
    static integer maxnbc, maxsbc, idummy[1];
    static char schstr[72];
    static logical noroom, pentag, triang, trnsit, errchk;
    static integer mxnper;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);
    static integer ipntrh, maxprm;
    extern /* Subroutine */ int mdrsrv_(char *, integer *, integer *, ftnlen),
	     mdstat_(integer *, integer *), mderor_(integer *), chkrgn_(
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, char *, char *
	    , integer *, integer *, integer *, integer *, integer *, integer *
	    , real *, real *, integer *, integer *, integer *, real *, real *,
	     integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, integer *, integer *, integer *, integer *, real *
	    , integer *, integer *, integer *, integer *, integer *, logical *
	    , real *, real *, real *, real *, integer *, real *, real *, real 
	    *, logical *, logical *, logical *, ftnlen, ftnlen);
    static integer iminus, iostat, ifound, igpntr, nnnold, kkkold, lllold, 
	    jjhole;
    extern /* Subroutine */ int rgnsch_(integer *, integer *, integer *, char 
	    *, integer *, real *, integer *, logical *, integer *, integer *, 
	    integer *, integer *, char *, char *, char *, integer *, integer *
	    , logical *, logical *, logical *, logical *, logical *, integer *
	    , logical *, ftnlen, ftnlen, ftnlen, ftnlen);
    static integer lensch;
    extern /* Subroutine */ int bmschm_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, integer *, 
	    integer *, real *, real *, real *, integer *, integer *, integer *
	    , integer *, integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, logical *, logical *, logical *);
    static integer iavail, navail;
    extern /* Subroutine */ int tmschm_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, integer *, 
	    integer *, real *, real *, real *, integer *, integer *, integer *
	    , integer *, integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, logical *, logical *), umschm_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, integer *, integer *, real *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, integer *, integer *, integer *, integer *, logical *, 
	    logical *), pmschm_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, real *, real *, integer *
	    , integer *, real *, real *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    char *, integer *, integer *, logical *, logical *, logical *, 
	    real *, real *, real *, integer *, integer *, integer *, integer *
	    , real *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, real *, real *, real *, real *, integer *, real *, 
	    real *, real *, ftnlen), mdlong_(char *, integer *, integer *, 
	    ftnlen), mmschm_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, integer *, real *
	    , real *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, logical *, char *, 
	    integer *, logical *, ftnlen), pltbel_(), pltflu_(), pschem_(
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, integer *, integer *, real *, real *, integer *, integer *
	    , integer *, integer *, integer *, integer *, integer *, integer *
	    , integer *, integer *, integer *, integer *, integer *, integer *
	    , integer *, integer *, integer *, integer *, char *, char *, 
	    logical *, integer *, integer *, integer *, integer *, char *, 
	    logical *, logical *, logical *, integer *, integer *, integer *, 
	    char *, integer *, real *, integer *, integer *, integer *, real *
	    , real *, real *, real *, integer *, logical *, logical *, real *,
	     real *, real *, integer *, integer *, integer *, integer *, real 
	    *, integer *, integer *, integer *, integer *, integer *, logical 
	    *, real *, real *, real *, real *, integer *, real *, real *, 
	    real *, ftnlen, ftnlen, ftnlen, ftnlen);
    static integer inside;
    extern /* Subroutine */ int intrup_(char *, logical *, integer *, integer 
	    *, integer *, char *, integer *, real *, integer *, ftnlen, 
	    ftnlen), fixsub_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *), fxnuid_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, logical *),
	     chkkxl_(integer *, integer *, integer *, integer *, logical *), 
	    getsbc_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, logical *),
	     mkused_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), savreg_(
	    integer *, integer *, integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    integer *), spring_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *, logical *, logical *, real *, 
	    real *, real *, integer *, integer *, integer *, integer *, real *
	    , integer *, integer *, integer *, integer *, integer *, logical *
	    , real *, real *, real *, real *, integer *, real *, real *, real 
	    *, logical *);
    static integer lstnbc;

    /* Fortran I/O blocks */
    static cilist io___46 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___48 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___49 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___50 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___52 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___62 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___63 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___72 = { 0, 6, 0, fmt_10070, 0 };
    static cilist io___79 = { 0, 6, 0, fmt_10080, 0 };
    static cilist io___101 = { 0, 6, 0, fmt_10100, 0 };
    static cilist io___102 = { 0, 6, 0, fmt_10100, 0 };
    static cilist io___103 = { 0, 6, 0, fmt_10090, 0 };
    static cilist io___104 = { 0, 6, 0, fmt_10110, 0 };
    static cilist io___105 = { 0, 6, 0, fmt_10130, 0 };
    static cilist io___108 = { 0, 6, 0, fmt_10120, 0 };
    static cilist io___110 = { 0, 6, 0, fmt_10140, 0 };


/* *********************************************************************** */
/*  QMESH: A QUADRILATERAL MESH GENERATION PROGRAM */
/* *********************************************************************** */
/*  ORIGINALLY WRITTEN BY: */
/*     RONDALL E JONES  DIV 2642  SANDIA LABORATORIES  ALBUQUERQUE */
/*  REWRITTEN AND UPDATED BY: */
/*     TEDDY D. BLACKER  DIV 1522 SANDIA LABORATORIES  ALBUQUERQUE */
/*     DECEMBER 1985 */
/* *********************************************************************** */
    /* Parameter adjustments */
    --a;
    --ia;
    linkpb -= 3;
    linkp -= 3;
    listpb -= 3;
    --ifpb;
    --nppf;
    --ipbf;
    --ipboun;
    coor -= 3;
    --ipoint;
    linksb -= 3;
    linklb -= 3;
    linkl -= 3;
    listsb -= 3;
    --ifsb;
    --nspf;
    --isbf;
    listlb -= 3;
    --iflb;
    --nlpf;
    --ilbf;
    --isboun;
    --ilboun;
    lcon -= 4;
    --factor;
    --nint;
    --ltype;
    --iline;
    linkb -= 3;
    links -= 3;
    --jllist;
    --jfline;
    --nlpb;
    --jcent;
    --jmat;
    --ibarst;
    --illist;
    --ifline;
    --nlps;
    --iside;
    --irgflg;
    --ihlist;
    --nhpr;
    --ifhole;
    --rsize;
    linksc -= 3;
    linkr -= 3;
    --irpb;
    --islist;
    --ifside;
    --nspr;
    --imat;
    --iregn;
    scheme -= 72;
    --ischm;
    --kin;
    --iin;
    --rin;
    cin -= 72;
    --n;
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
/*  INITIALIZE */
    iz = 0;
    ipntr = 0;
    ipntr1 = 0;
    ipntr2 = 0;
    *npregn = 0;
    *npelem = 1;
    *npnode = 1;
    *npnbc = 1;
    *npsbc = 1;
    *maxkxn = 1;
    mxnd = 1;
    mxnl = 1;
    mxrxg = 1;
    mxnper = 1;
    maxsbc = 1;
    maxnbc = 1;
    ksbc = 1;
    kksbc = 1;
    max1 = 1;
    max2 = 1;
    max3 = 1;
    max4 = 1;
    even = TRUE_;
    count = TRUE_;
    addlnk = FALSE_;
    pentag = FALSE_;
    triang = FALSE_;
    trnsit = FALSE_;
    fill = FALSE_;
    graph = FALSE_;
/*  HEADER */
    mesage_(" ", (ftnlen)1);
    mesage_("MESH PROCESSING BEGUN", (ftnlen)21);
    mesage_(" ", (ftnlen)1);
/*  FILL IN ANY MISSING INTERVALS ACCORDING TO SIZE AND CHECK THE */
/*  VALIDITY OF REGION DATA */
    errchk = FALSE_;
    i__1 = n[9];
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__3 = (i__2 = irpb[i__], abs(i__2));
	ltsort_(mr, &linkr[3], &i__3, &ipntr1, &addlnk);
	i__3 = (i__2 = irpb[i__], abs(i__2));
	ltsort_(ms, &linkb[3], &i__3, &ipntr2, &addlnk);
	if (irpb[i__] > 0 && irpb[i__] <= n[22] && ipntr1 > 0) {
	    if (irgflg[ipntr1] <= -1) {
		l = ipntr1;
		if (rsize[l] <= (float)0.) {
		    size = *defsiz;
		} else {
		    size = rsize[l];
		}
		dataok_(mp, ml, ms, mr, &l, &iregn[l], &coor[3], &iline[1], &
			ltype[1], &nint[1], &lcon[4], &nlps[1], &ifline[1], &
			illist[1], &nspr[1], &ifside[1], &islist[1], &linkp[3]
			, &linkl[3], &links[3], &size, &errchk, &err);
		if (nhpr[l] > 0) {
		    i__2 = ifhole[l] + nhpr[l] - 1;
		    for (ihole = ifhole[l]; ihole <= i__2; ++ihole) {
			i__4 = (i__3 = ihlist[ihole], abs(i__3));
			ltsort_(mr, &linkr[3], &i__4, &ipntrh, &addlnk);
			if (ipntrh > 0) {
			    ll = ipntrh;
			    if (rsize[ll] <= (float)0.) {
				size = *defsiz;
			    } else {
				size = rsize[ll];
			    }
			    dataok_(mp, ml, ms, mr, &ll, &iregn[ll], &coor[3],
				     &iline[1], &ltype[1], &nint[1], &lcon[4],
				     &nlps[1], &ifline[1], &illist[1], &nspr[
				    1], &ifside[1], &islist[1], &linkp[3], &
				    linkl[3], &links[3], &size, &errchk, &err)
				    ;
			}
/* L100: */
		    }
		}
	    } else if (irgflg[ipntr1] >= 1) {
		j1 = ifside[ipntr1];
		j2 = j1 + nspr[ipntr1] - 1;
		iregn[ipntr1] = -(i__2 = iregn[ipntr1], abs(i__2));
/* Computing MAX */
		i__2 = mxrxg, i__3 = nspr[ipntr1];
		mxrxg = max(i__2,i__3);
		i__2 = j2;
		for (j = j1; j <= i__2; ++j) {
		    i__4 = (i__3 = islist[j], abs(i__3));
		    ltsort_(mr, &linkr[3], &i__4, &ipntr1, &addlnk);
		    if (ipntr1 > 0) {
			l = ipntr1;
			if (rsize[l] <= (float)0.) {
			    size = *defsiz;
			} else {
			    size = rsize[l];
			}
			dataok_(mp, ml, ms, mr, &l, &iregn[l], &coor[3], &
				iline[1], &ltype[1], &nint[1], &lcon[4], &
				nlps[1], &ifline[1], &illist[1], &nspr[1], &
				ifside[1], &islist[1], &linkp[3], &linkl[3], &
				links[3], &size, &errchk, &err);
			if (nhpr[l] > 0) {
			    i__3 = ifhole[l] + nhpr[l] - 1;
			    for (ihole = ifhole[l]; ihole <= i__3; ++ihole) {
				i__5 = (i__4 = ihlist[ihole], abs(i__4));
				ltsort_(mr, &linkr[3], &i__5, &ipntrh, &
					addlnk);
				if (ipntrh > 0) {
				    ll = ipntrh;
				    if (rsize[ll] <= (float)0.) {
					size = *defsiz;
				    } else {
					size = rsize[ll];
				    }
				    dataok_(mp, ml, ms, mr, &ll, &iregn[ll], &
					    coor[3], &iline[1], &ltype[1], &
					    nint[1], &lcon[4], &nlps[1], &
					    ifline[1], &illist[1], &nspr[1], &
					    ifside[1], &islist[1], &linkp[3], 
					    &linkl[3], &links[3], &size, &
					    errchk, &err);
				}
/* L110: */
			    }
			}
		    }
/* L120: */
		}
	    }
	}
/* L130: */
    }
    errchk = TRUE_;
/*  FIND THE MAXIMUM NUMBER OF LINES/REGION, PERIMETER POINTS/REGION, */
/*  AND HOLES/REGION */
    i__1 = n[2];
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	i__2 = nint[i__];
	max1 = max(i__2,max1);
/* L140: */
    }
    i__1 = n[3];
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	i__2 = nlps[i__];
	max2 = max(i__2,max2);
/* L150: */
    }
    i__1 = n[5];
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	i__2 = nlpb[i__];
	max2 = max(i__2,max2);
/* L160: */
    }
    i__1 = n[7];
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	i__2 = nspr[i__];
	max3 = max(i__2,max3);
/* Computing MAX */
	i__2 = nhpr[i__];
	max4 = max(i__2,max4);
/* L170: */
    }
    if (*remesh) {
	max1 *= 20;
    }
    maxnl = max2 * (max3 + max4 * max3) + 1;
    maxnp = max1 * maxnl + 1;
    maxprm = max4 + 1;
    ++max3;
/*  GET INITIAL SPACE IN ARRAY A FOR PERIMETER GENERATION */
/*  K(1) = X ARRAY OF THE PERIMETER */
/*  K(2) = Y ARRAY OF THE PERIMETER */
/*  K(3) = NID ARRAY OF THE PERIMETER */
/*  K(4) = LINE LIST */
/*  K(5) = NO OF NODES PER SIDE LIST */
/*  K(6) = WORK ARRAY FOR M1 GENERATION */
    mdrsrv_("X", k, &maxnp, (ftnlen)1);
    mdrsrv_("Y", &k[1], &maxnp, (ftnlen)1);
    mdrsrv_("NID", &k[2], &maxnp, (ftnlen)3);
    mdrsrv_("LISTL", &k[3], &maxnl, (ftnlen)5);
    mdrsrv_("NNPS", &k[4], &max3, (ftnlen)4);
    mdrsrv_("ANGLE", &k[5], &maxnp, (ftnlen)5);
    i__1 = maxnl * 3;
    mdrsrv_("MARKED", &k[25], &i__1, (ftnlen)6);
    mdstat_(&nerr, &mused);
    if (nerr > 0) {
	mderor_(&c__6);
	s_stop(" ", (ftnlen)1);
    }
/*  LOOP THROUGH THE GROUPS/REGIONS AND BAR SETS IN THE BODY LIST */
/*  CHECK CONNECTIVITY AND CALCULATE THE DIMENSIONS NEEDED FOR MESHING */
/*  NO PERIMETER INFORMATION IS SAVED THIS TIME THROUGH */
    real__ = FALSE_;
    count = TRUE_;
    i__1 = n[9];
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__3 = (i__2 = irpb[i__], abs(i__2));
	ltsort_(mr, &linkr[3], &i__3, &ipntr1, &addlnk);
	i__3 = (i__2 = irpb[i__], abs(i__2));
	ltsort_(ms, &linkb[3], &i__3, &ipntr2, &addlnk);
/*  CHECK A REGION OR GROUP */
	if (irpb[i__] > 0 && irpb[i__] <= n[22] && ipntr1 > 0) {
	    if (irgflg[ipntr1] <= -1) {
		s_wsfe(&io___46);
		do_fio(&c__1, (char *)&irpb[i__], (ftnlen)sizeof(integer));
		e_wsfe();
		l = ipntr1;
		chkrgn_(&ia[1], &l, mp, ml, ms, mr, msc, &n[24], &ipoint[1], &
			coor[3], &ipboun[1], &iline[1], &ltype[1], &nint[1], &
			factor[1], &lcon[4], &ilboun[1], &isboun[1], &iside[1]
			, &nlps[1], &ifline[1], &illist[1], &iregn[1], &nspr[
			1], &ifside[1], &islist[1], &nppf[1], &ifpb[1], &
			listpb[3], &nlpf[1], &iflb[1], &listlb[3], &nspf[1], &
			ifsb[1], &listsb[3], &ifhole[1], &nhpr[1], &ihlist[1],
			 &linkp[3], &linkl[3], &links[3], &linkr[3], &linksc[
			3], &linkpb[3], &linklb[3], &linksb[3], &rsize[1], 
			scheme + 72, defsch, npregn, npsbc, npnode, &maxnp, &
			maxnl, &max3, &a[k[0]], &a[k[1]], &ia[k[2]], &ia[k[3]]
			, &ia[k[4]], &a[k[5]], &a[k[25]], &mxnd, &mxnper, &
			mxnl, &maxnbc, &maxsbc, &amesur[1], &xnold[1], &ynold[
			1], &nxkold[nxkold_offset], &mmpold[4], &linkeg[3], &
			listeg[1], &bmesur[1], mlink, nprold, npnold, npeold, 
			nnxk, remesh, rexmin, rexmax, reymin, reymax, idivis, 
			sizmin, emax, emin, &noroom, &errchk, &err, (ftnlen)
			72, (ftnlen)72);
	    } else if (irgflg[ipntr1] >= 1) {
		s_wsfe(&io___48);
		do_fio(&c__1, (char *)&irpb[i__], (ftnlen)sizeof(integer));
		e_wsfe();
		j1 = ifside[ipntr1];
		j2 = j1 + nspr[ipntr1] - 1;
		iregn[ipntr1] = -(i__2 = iregn[ipntr1], abs(i__2));
/* Computing MAX */
		i__2 = mxrxg, i__3 = nspr[ipntr1];
		mxrxg = max(i__2,i__3);
		i__2 = j2;
		for (j = j1; j <= i__2; ++j) {
		    i__4 = (i__3 = islist[j], abs(i__3));
		    ltsort_(mr, &linkr[3], &i__4, &ipntr1, &addlnk);
		    if (ipntr1 > 0) {
			s_wsfe(&io___49);
			i__4 = (i__3 = islist[j], abs(i__3));
			do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
			e_wsfe();
			l = ipntr1;
			chkrgn_(&ia[1], &l, mp, ml, ms, mr, msc, &n[24], &
				ipoint[1], &coor[3], &ipboun[1], &iline[1], &
				ltype[1], &nint[1], &factor[1], &lcon[4], &
				ilboun[1], &isboun[1], &iside[1], &nlps[1], &
				ifline[1], &illist[1], &iregn[1], &nspr[1], &
				ifside[1], &islist[1], &nppf[1], &ifpb[1], &
				listpb[3], &nlpf[1], &iflb[1], &listlb[3], &
				nspf[1], &ifsb[1], &listsb[3], &ifhole[1], &
				nhpr[1], &ihlist[1], &linkp[3], &linkl[3], &
				links[3], &linkr[3], &linksc[3], &linkpb[3], &
				linklb[3], &linksb[3], &rsize[1], scheme + 72,
				 defsch, npregn, npsbc, npnode, &maxnp, &
				maxnl, &max3, &a[k[0]], &a[k[1]], &ia[k[2]], &
				ia[k[3]], &ia[k[4]], &a[k[5]], &a[k[25]], &
				mxnd, &mxnper, &mxnl, &maxnbc, &maxsbc, &
				amesur[1], &xnold[1], &ynold[1], &nxkold[
				nxkold_offset], &mmpold[4], &linkeg[3], &
				listeg[1], &bmesur[1], mlink, nprold, npnold, 
				npeold, nnxk, remesh, rexmin, rexmax, reymin, 
				reymax, idivis, sizmin, emax, emin, &noroom, &
				errchk, &err, (ftnlen)72, (ftnlen)72);
		    }
/* L180: */
		}
	    }
/*  WRITE AN ERROR FOR THIS REGION IN THE BODY LIST */
	} else if (irpb[i__] > 0) {
	    s_wsfe(&io___50);
	    do_fio(&c__1, (char *)&irpb[i__], (ftnlen)sizeof(integer));
	    e_wsfe();
	    i__3 = (i__2 = irpb[i__], abs(i__2));
	    ltsort_(mr, &linkr[3], &i__3, &ipntr, &addlnk);
	    addlnk = TRUE_;
	    iminus = -ipntr;
	    i__3 = (i__2 = irpb[i__], abs(i__2));
	    ltsort_(mr, &linkr[3], &i__3, &iminus, &addlnk);
	    addlnk = FALSE_;
/*  CHECK A BAR SET */
	} else if (irpb[i__] < 0 && (i__2 = irpb[i__], abs(i__2)) <= n[21] && 
		ipntr2 > 0) {
	    l = ipntr2;
	    s_wsfe(&io___52);
	    i__3 = (i__2 = irpb[i__], abs(i__2));
	    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
	    e_wsfe();
	    real__ = FALSE_;
	    count = TRUE_;
	    test = FALSE_;
	    nper = 1;
	    knbc = 1;
	    ksbc = 1;
	    i__2 = jfline[ipntr2] + nlpb[ipntr2] - 1;
	    for (j = jfline[ipntr2]; j <= i__2; ++j) {
		ltsort_(ml, &linkl[3], &jllist[j], &kk, &addlnk);
		ltsort_(mp, &linkp[3], &lcon[kk * 3 + 1], &ip1, &addlnk);
		ltsort_(mp, &linkp[3], &lcon[kk * 3 + 2], &ip2, &addlnk);
		if (lcon[kk * 3 + 3] > 0) {
		    ltsort_(mp, &linkp[3], &lcon[kk * 3 + 3], &ip3, &addlnk);
		} else if (lcon[kk * 3 + 3] < 0) {
		    i__3 = -lcon[kk * 3 + 3];
		    ltsort_(mp, &linkp[3], &i__3, &ip3, &addlnk);
		    ip3 = -ip3;
		} else {
		    ip3 = 0;
		}
		pline_(mp, ml, &maxnp, &c__1, &c__1, &ipoint[1], &coor[3], &
			linkp[3], &iline[kk], &ltype[kk], &nint[kk], &factor[
			kk], &ip1, &ip2, &ip3, &a[k[0]], &a[k[1]], &ia[k[2]], 
			&ipboun[ip1], &ipboun[ip2], &ilboun[kk], &isboun[kk], 
			&linkpb[3], &nppf[1], &ifpb[1], &listpb[3], &linklb[3]
			, &nlpf[1], &iflb[1], &listlb[3], &linksb[3], &nspf[1]
			, &ifsb[1], &listsb[3], idummy, &knbc, &ksbc, &err, &
			test, &real__, &count, &noroom, &amesur[1], &xnold[1],
			 &ynold[1], &nxkold[nxkold_offset], &mmpold[4], &
			linkeg[3], &listeg[1], &bmesur[1], mlink, nprold, 
			npnold, npeold, nnxk, remesh, rexmin, rexmax, reymin, 
			reymax, idivis, sizmin, emax, emin, &graph, &dxmax);
		if (err) {
		    s_wsfe(&io___62);
		    do_fio(&c__1, (char *)&ibarst[l], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    addlnk = FALSE_;
		    ltsort_(ms, &linkb[3], &irpb[i__], &ipntr, &addlnk);
		    addlnk = TRUE_;
		    iminus = -ipntr;
		    ltsort_(ms, &linkb[3], &irpb[i__], &iminus, &addlnk);
		    addlnk = FALSE_;
		    goto L200;
		}
		nper = nper + nint[kk] + 1;
/* L190: */
	    }
	    ibarst[l] = -ibarst[l];
/*  WHEN CHECKING THE MAXIMUMS - ADD ENOUGH FOR ONE MORE INTERVAL */
/*  ON THE LINE AS THIS LINE MAY BE INCREMENTED BY ONE IF THE */
/*  PERIMETER IS ODD */
/* Computing MAX */
	    i__2 = maxnbc, i__3 = knbc + 3;
	    maxnbc = max(i__2,i__3);
/* Computing MAX */
	    i__2 = maxsbc, i__3 = ksbc + 3;
	    maxsbc = max(i__2,i__3);
	    mxnd = max(mxnd,nper);
/* Computing MAX */
	    i__2 = mxnper, i__3 = nper + 2;
	    mxnper = max(i__2,i__3);
/*  WRITE AN ERROR FOR THIS BAR SET IN THE BODY LIST */
	} else {
	    s_wsfe(&io___63);
	    i__3 = (i__2 = irpb[i__], abs(i__2));
	    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
	    e_wsfe();
	    addlnk = FALSE_;
	    ltsort_(ms, &linkb[3], &irpb[i__], &ipntr, &addlnk);
	    addlnk = TRUE_;
	    iminus = -ipntr;
	    ltsort_(ms, &linkb[3], &irpb[i__], &iminus, &addlnk);
	    addlnk = FALSE_;
	}
L200:
/* L210: */
	;
    }
/*  RESET ALL USED POINTS AND LINES */
    i__1 = n[1];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ipoint[i__] = (i__2 = ipoint[i__], abs(i__2));
/* L220: */
    }
    i__1 = n[2];
    for (i__ = 1; i__ <= i__1; ++i__) {
	nint[i__] = (i__2 = nint[i__], abs(i__2));
/* L230: */
    }
/*  RELEASE THE OLD ARRAYS, AND THEN */
/*  DIMENSION BASED ON THE MAXIMUMS CALCULATED */
    mddel_("X", (ftnlen)1);
    mddel_("Y", (ftnlen)1);
    mddel_("NID", (ftnlen)3);
    mddel_("LISTL", (ftnlen)5);
    mddel_("NNPS", (ftnlen)4);
    mddel_("ANGLE", (ftnlen)5);
    mddel_("MARKED", (ftnlen)6);
    mdstat_(&nerr, &mused);
    if (nerr > 0) {
	mderor_(&c__6);
	s_stop(" ", (ftnlen)1);
    }
/*  K(1) = X ARRAY OF THE PERIMETER */
/*  K(2) = Y ARRAY OF THE PERIMETER */
/*  K(3) = NID ARRAY(S) OF THE PERIMETER(S) [HOLES CAUSE MULTIPLE PERIMS] */
/*  K(4) = LINE LIST */
/*  K(5) = NO OF NODES PER SIDE LIST */
/*  K(6) = WORK ARRAY FOR M1 GENERATION */
/*  K(7) = XN (GLOBAL NODAL X VALUES) */
/*  K(8) = YN (GLOBAL NODAL Y VALUES) */
/*  K(9) = NUID (GLOBAL NODE UNIQUE ID'S) */
/*  K(10) = LXK (LINES PER ELEMENT) */
/*  K(11) = KXL (ELEMENTS PER LINE) */
/*  K(12) = NXL (NODES PER LINE) */
/*  K(13) = LXN (LINES PER NODE) */
/*  K(14) = LSTNBC (LIST OF NODAL BOUNDARY FLAGS AND NODES) */
/*  K(15) = LSTSBC (LIST OF SIDE BOUNDARY FLAGS AND NODES) */
/*  K(16) = XSUB ARRAY OF THE PERIMETER OF A SUBREGION */
/*  K(17) = YSUB ARRAY OF THE PERIMETER OF A SUBREGION */
/*  K(18) = NIDSUB ARRAY OF THE PERIMETER OF A SUBREGION */
/*  K(19) = NODE NUMBERS AROUND HOLE VOID */
/*  K(20) = NUMBER OF NODES ON PERIMETERS (REGION + HOLE) */
/*  K(21) = INDEX ARRAY FOR COMBINING SUB-REGIONS AND REGIONS */
/*  K(22) = FANGLE ARRAY FOR INTERIOR ANGLE IN FILL ROUTINES */
/*  K(23) = BNSIZE ARRAY FOR SIZE DIFFERENTIAL IN FILL ROUTINES */
/*  K(24) = LNODES ARRAY FOR CONNECTIVITY OF THE INSIDE PERIMETER */
/*          NODES IN FILL ROUTINES */
/*  NOTE: LINES IN THIS CONTEXT REFERS TO CONNECTIONS OF ELEMENT NODES */
/*  MAKE ROOM IN LINE LIST FOR HOLES */
    mxnd = (integer) (mxnd * mxrxg * (float)1.2);
    mxnl += (mxrxg + max4) * max2 * max3;
    mln = 8;
    maxnb = mxnper * maxprm;
    mdrsrv_("X", k, &mxnper, (ftnlen)1);
    mdrsrv_("Y", &k[1], &mxnper, (ftnlen)1);
    i__1 = mxnper * maxprm;
    mdrsrv_("NID", &k[2], &i__1, (ftnlen)3);
    mdrsrv_("LISTL", &k[3], &mxnl, (ftnlen)5);
    mdrsrv_("NNPS", &k[4], &max3, (ftnlen)4);
    mdrsrv_("ANGLE", &k[5], &mxnper, (ftnlen)5);
    mdrsrv_("XN", &k[6], &mxnd, (ftnlen)2);
    mdrsrv_("YN", &k[7], &mxnd, (ftnlen)2);
    mdrsrv_("NUID", &k[8], &mxnd, (ftnlen)4);
    i__1 = mxnd << 2;
    mdrsrv_("LXK", &k[9], &i__1, (ftnlen)3);
    i__1 = mxnd * 6;
    mdrsrv_("KXL", &k[10], &i__1, (ftnlen)3);
    i__1 = mxnd * 6;
    mdrsrv_("NXL", &k[11], &i__1, (ftnlen)3);
    i__1 = mxnd << 2;
    mdrsrv_("LXN", &k[12], &i__1, (ftnlen)3);
    mdrsrv_("LSTNBC", &k[13], &maxnbc, (ftnlen)6);
    i__1 = maxsbc << 1;
    mdrsrv_("LSTSBC", &k[14], &i__1, (ftnlen)6);
    mdrsrv_("XSUB", &k[15], &mxnper, (ftnlen)4);
    mdrsrv_("YSUB", &k[16], &mxnper, (ftnlen)4);
    mdrsrv_("NIDSUB", &k[17], &mxnper, (ftnlen)6);
    mdrsrv_("NXH", &k[18], &mxnd, (ftnlen)3);
    mdrsrv_("NPERIM", &k[19], &maxprm, (ftnlen)6);
    mdrsrv_("INDX", &k[20], &mxnd, (ftnlen)4);
    mdrsrv_("FANGLE", &k[21], &mxnd, (ftnlen)6);
    i__1 = mxnd << 1;
    mdrsrv_("BNSIZE", &k[22], &i__1, (ftnlen)6);
    i__1 = mxnd * mln;
    mdrsrv_("LNODES", &k[23], &i__1, (ftnlen)6);
    i__1 = maxprm * 3;
    mdrsrv_("PRLINK", &k[24], &i__1, (ftnlen)6);
    i__1 = mxnl * 3;
    mdrsrv_("MARKED", &k[25], &i__1, (ftnlen)6);
    mdrsrv_("IPTPER", &k[26], &maxprm, (ftnlen)6);
    mdrsrv_("NUMPER", &k[27], &maxprm, (ftnlen)6);
    mdrsrv_("LPERIM", &k[28], &maxnb, (ftnlen)6);
    mdrsrv_("ZN", &k[29], &mxnd, (ftnlen)2);
    mdstat_(&nerr, &mused);
    if (nerr > 0) {
	mderor_(&c__6);
	s_stop(" ", (ftnlen)1);
    }
/*  SET UP THE LOOP FOR PROCESSING GROUPS */
    if (*lgroup) {
L240:
	if (*step && n[22] > 0) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("STEP PROCESS GROUPS I1 THROUGH I2", (ftnlen)33);
	    if (*icom > *jcom) {
		frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72,
			 &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		*icom = 1;
	    }
	    geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		    ifound, (ftnlen)72);
	    if (ifound > 0) {
		check_(&i1, &i2, &n[22]);
	    } else {
		goto L320;
	    }
	} else {
	    i1 = 1;
	    i2 = n[22];
	}
/*  BEGIN PROCESSING GROUPS */
	real__ = TRUE_;
	count = FALSE_;
	i__1 = i2;
	for (igrp = i1; igrp <= i__1; ++igrp) {
L250:
	    ltsort_(mr, &linkr[3], &igrp, &igpntr, &addlnk);
	    if (igpntr > 0 && irgflg[igpntr] >= 1 && iregn[igpntr] < 0) {
		s_wsfe(&io___72);
		do_fio(&c__1, (char *)&igrp, (ftnlen)sizeof(integer));
		e_wsfe();
		j1 = ifside[igpntr];
		j2 = j1 + nspr[igpntr] - 1;
		nnn = 0;
		kkk = 0;
		lll = 0;
		nnnold = 0;
		kkkold = 0;
		lllold = 0;
		i__2 = j2;
		for (j = j1; j <= i__2; ++j) {
		    i__4 = (i__3 = islist[j], abs(i__3));
		    ltsort_(mr, &linkr[3], &i__4, &ipntr2, &addlnk);
		    if (ipntr2 > 0 && iregn[ipntr2] < 0) {
			l = ipntr2;
			noroom = FALSE_;
			mesage_(" ", (ftnlen)1);
			s_wsfe(&io___79);
			i__4 = (i__3 = iregn[l], abs(i__3));
			do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
			e_wsfe();
/*  CALCULATE THE PERIMETER OF THE REGION */
L260:
			nprm = 1;
			jjhole = 0;
			knbc = 0;
			i__4 = (i__3 = iregn[l], abs(i__3));
			perim_(mp, ml, ms, &nspr[l], &mxnl, &mxnper, &maxnbc, 
				&maxsbc, &knbc, &ksbc, &i__4, &ipoint[1], &
				coor[3], &ipboun[1], &iline[1], &ltype[1], &
				nint[1], &factor[1], &lcon[4], &ilboun[1], &
				isboun[1], &iside[1], &nlps[1], &ifline[1], &
				illist[1], &islist[ifside[l]], &nppf[1], &
				ifpb[1], &listpb[3], &nlpf[1], &iflb[1], &
				listlb[3], &nspf[1], &ifsb[1], &listsb[3], &
				linkp[3], &linkl[3], &links[3], &linkpb[3], &
				linklb[3], &linksb[3], &a[k[0]], &a[k[1]], &
				ia[k[2]], &nper, &ia[k[3]], &nl, &ia[k[13]], &
				ia[k[25]], &even, &real__, &err, &ccw, &count,
				 &noroom, &amesur[1], &xnold[1], &ynold[1], &
				nxkold[nxkold_offset], &mmpold[4], &linkeg[3],
				 &listeg[1], &bmesur[1], mlink, nprold, 
				npnold, npeold, nnxk, remesh, rexmin, rexmax, 
				reymin, reymax, idivis, sizmin, emax, emin);
/*  GET THE REGION SCHEME */
			i__4 = (i__3 = iregn[l], abs(i__3));
			ltsort_(mr, &linksc[3], &i__4, &ipntr, &addlnk);
			rgnsch_(mcom, icom, jcom, cin + 72, &iin[1], &rin[1], 
				&kin[1], step, &iregn[l], &ipntr, &n[24], msc,
				 scheme + 72, defsch, schstr, &lensch, &nper, 
				&pentag, &triang, &trnsit, &halfc, &fill, &
				icode, remesh, (ftnlen)72, (ftnlen)72, (
				ftnlen)72, (ftnlen)72);
			if (icode == iexit) {
			    goto L270;
			} else if (icode == iover) {
			    goto L260;
			} else if (icode == iquit) {
			    goto L270;
/*  GENERATE INITIAL GRID */
/*  CALCULATE A "TRANSITION" MAPPED MESH */
			} else if (trnsit) {
			    bmschm_(&nper, &kkk, &lll, &nnn, ml, ms, &nspr[l],
				     &islist[ifside[l]], &nint[1], &ifline[1],
				     &nlps[1], &illist[1], &linkl[3], &links[
				    3], &mxnper, &maxprm, &max3, &mxnd, &a[k[
				    0]], &a[k[1]], &ia[k[2]], &ia[k[4]], &a[k[
				    5]], &a[k[6]], &a[k[7]], &ia[k[8]], &ia[k[
				    9]], &ia[k[10]], &ia[k[11]], &ia[k[12]], &
				    a[k[15]], &a[k[16]], &ia[k[17]], &ia[k[20]
				    ], &iavail, &navail, &ccw, &halfc, &err);
/*  CALCULATE A "TRIANGULAR" MAPPED MESH */
			} else if (triang) {
			    tmschm_(&nper, &kkk, &lll, &nnn, ml, ms, &nspr[l],
				     &islist[ifside[l]], &nint[1], &ifline[1],
				     &nlps[1], &illist[1], &linkl[3], &links[
				    3], &mxnper, &maxprm, &max3, &mxnd, &a[k[
				    0]], &a[k[1]], &ia[k[2]], &ia[k[4]], &a[k[
				    5]], &a[k[6]], &a[k[7]], &ia[k[8]], &ia[k[
				    9]], &ia[k[10]], &ia[k[11]], &ia[k[12]], &
				    a[k[15]], &a[k[16]], &ia[k[17]], &ia[k[20]
				    ], &iavail, &navail, &ccw, &err);
/*  CALCULATE A "PENTAGON" MAPPED MESH */
			} else if (pentag) {
			    umschm_(&ia[1], &nper, &kkk, &lll, &nnn, ml, ms, &
				    nspr[l], &islist[ifside[l]], &nint[1], &
				    ifline[1], &nlps[1], &illist[1], &linkl[3]
				    , &links[3], &mxnper, &maxprm, &max3, &
				    mxnd, &a[k[0]], &a[k[1]], &ia[k[2]], &ia[
				    k[4]], &a[k[5]], &a[k[6]], &a[k[7]], &ia[
				    k[8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], 
				    &ia[k[12]], &a[k[15]], &a[k[16]], &ia[k[
				    17]], &ia[k[20]], &iavail, &navail, &ccw, 
				    &err);
/*  USE THE PAVING TECHNIQUE TO FILL THE INITIAL REGION */
			} else if (fill) {
			    i__4 = (i__3 = iregn[l], abs(i__3));
			    pmschm_(&nper, &nprm, &mxnd, &mln, mp, ml, ms, mr,
				     &nl, &mxnl, &mxnper, &maxprm, &maxnb, &
				    maxnbc, &maxsbc, &knbc, &ksbc, &knum, &
				    ipoint[1], &coor[3], &ipboun[1], &iline[1]
				    , &ltype[1], &nint[1], &factor[1], &lcon[
				    4], &ilboun[1], &isboun[1], &iside[1], &
				    nlps[1], &ifline[1], &illist[1], &islist[
				    1], &iregn[1], &nppf[1], &ifpb[1], &
				    listpb[3], &nlpf[1], &iflb[1], &listlb[3],
				     &nspf[1], &ifsb[1], &listsb[3], &linkp[3]
				    , &linkl[3], &links[3], &linkr[3], &
				    linkpb[3], &linklb[3], &linksb[3], &nspr[
				    1], &ifside[1], &rsize[1], &ifhole[1], &
				    nhpr[1], &ihlist[1], &a[k[0]], &a[k[1]], &
				    ia[k[2]], &ia[k[3]], &a[k[6]], &a[k[7]], &
				    a[k[29]], &ia[k[8]], &ia[k[9]], &ia[k[10]]
				    , &ia[k[11]], &ia[k[12]], &ia[k[13]], &ia[
				    k[19]], &a[k[21]], &a[k[22]], &ia[k[23]], 
				    &ia[k[24]], &ia[k[25]], &ia[k[26]], &ia[k[
				    27]], &ia[k[28]], &kkk, &nnn, &lll, &
				    iavail, &navail, dev1, &i__4, &l, batch, &
				    noroom, &err, &amesur[1], &xnold[1], &
				    ynold[1], &nxkold[nxkold_offset], &mmpold[
				    4], &linkeg[3], &listeg[1], &bmesur[1], 
				    mlink, nprold, npnold, npeold, nnxk, 
				    remesh, rexmin, rexmax, reymin, reymax, 
				    idivis, sizmin, emax, emin, (ftnlen)3);
			    if (noroom) {
				mxnd = (integer) (mxnd * (float)1.5 + 1);
				maxnbc = max(maxnbc,knbc);
				maxsbc = max(maxsbc,ksbc);
				ksbc = kksbc;
				mdlong_("XN", &k[6], &mxnd, (ftnlen)2);
				mdlong_("YN", &k[7], &mxnd, (ftnlen)2);
				mdlong_("ZN", &k[29], &mxnd, (ftnlen)2);
				mdlong_("NUID", &k[8], &mxnd, (ftnlen)4);
				i__3 = mxnd << 2;
				mdlong_("LXK", &k[9], &i__3, (ftnlen)3);
				i__3 = mxnd * 6;
				mdlong_("KXL", &k[10], &i__3, (ftnlen)3);
				i__3 = mxnd * 6;
				mdlong_("NXL", &k[11], &i__3, (ftnlen)3);
				i__3 = mxnd << 2;
				mdlong_("LXN", &k[12], &i__3, (ftnlen)3);
				mdlong_("LSTNBC", &k[13], &maxnbc, (ftnlen)6);
				mdlong_("LSTSBC", &k[14], &maxsbc, (ftnlen)6);
				mdlong_("NXH", &k[18], &mxnd, (ftnlen)3);
				mdlong_("INDX", &k[20], &mxnd, (ftnlen)4);
				mdlong_("FANGLE", &k[21], &mxnd, (ftnlen)6);
				i__3 = mxnd << 1;
				mdlong_("BNSIZE", &k[22], &i__3, (ftnlen)6);
				i__3 = mxnd * mln;
				mdlong_("LNODES", &k[23], &i__3, (ftnlen)6);
				mdstat_(&nerr, &mused);
				if (nerr > 0) {
				    mderor_(&c__6);
				    s_stop(" ", (ftnlen)1);
				}
				mesage_("REDIMENSIONING NEEDED - PLEASE WAIT",
					 (ftnlen)35);
				if (*step) {
				    mesage_("CURRENT PROCESSING SCHEME IS SA\
VED", (ftnlen)34);
				} else {
				    mesage_("CURRENT SCHEME WILL BE REPEATED",
					     (ftnlen)31);
				}
				goto L260;
			    }
/*  PROCESS A "NORMAL" REGION */
			} else {
			    mmschm_(&nper, &kkk, &lll, &nnn, ml, ms, &nspr[l],
				     &islist[ifside[l]], &nint[1], &ifline[1],
				     &nlps[1], &illist[1], &linkl[3], &links[
				    3], &mxnper, &maxprm, &max3, &mxnd, &a[k[
				    0]], &a[k[1]], &ia[k[2]], &ia[k[4]], &a[k[
				    5]], &a[k[6]], &a[k[7]], &ia[k[8]], &ia[k[
				    9]], &ia[k[10]], &ia[k[11]], &ia[k[12]], &
				    iavail, &navail, &ccw, &real__, schstr, &
				    m1, &err, (ftnlen)72);
			}
/*  FLAG THE REGION IF AN ERROR HAS OCCURRED */
			if (err) {
			    mesage_("ERROR IN INITIAL GRID GENERATION", (
				    ftnlen)32);
			    mesage_("** REGION PROCESSING ABORTED **", (
				    ftnlen)31);
			    pltbel_();
			    pltflu_();
			    mesage_(" ", (ftnlen)1);
			    iregn[l] = (i__3 = iregn[l], abs(i__3));
			    if (islist[j] == iregn[l]) {
				addlnk = TRUE_;
				iminus = -l;
				ltsort_(mr, &linkr[3], &iregn[l], &iminus, &
					addlnk);
				addlnk = FALSE_;
			    }
			    goto L270;
			}
/*  BEGIN FULL SCHEME CONTROL FOR A GROUP SUB-REGION */
			rect = ! (pentag || triang || trnsit || fill);
			if (*step) {
			    minmax_fq__(&mxnper, &nper, &a[k[0]], &a[k[1]], &
				    xmin, &xmax, &ymin, &ymax);
			}
			pschem_(mp, ml, ms, mr, &n[1], &ipoint[1], &coor[3], &
				ipboun[1], &iline[1], &ltype[1], &nint[1], &
				factor[1], &lcon[4], &ilboun[1], &isboun[1], &
				iside[1], &nlps[1], &ifline[1], &illist[1], &
				iregn[1], &nspr[1], &ifside[1], &islist[1], &
				nppf[1], &ifpb[1], &listpb[3], &nlpf[1], &
				iflb[1], &listlb[3], &nspf[1], &ifsb[1], &
				listsb[3], &linkp[3], &linkl[3], &links[3], &
				linkr[3], &linksc[3], &linkpb[3], &linklb[3], 
				&linksb[3], &ifhole[1], &nhpr[1], &ihlist[1], 
				&maxnbc, &knbc, &maxsbc, &ksbc, &mxnd, &nnn, &
				nnnold, &kkk, &kkkold, &lll, &a[k[0]], &a[k[1]
				], &ia[k[2]], &ia[k[3]], &a[k[6]], &a[k[7]], &
				ia[k[8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &
				ia[k[12]], &ia[k[13]], &ia[k[18]], &ia[k[19]],
				 &ia[k[25]], &iavail, &navail, &mxnl, &mxnper,
				 &nper, &maxprm, &nprm, msc, &ischm[1], 
				scheme + 72, schstr, &rect, &m1, &inside, &
				jjhole, &kksbc, dev1, eight, nine, step, &l, &
				nl, mcom, cin + 72, &iin[1], &rin[1], &kin[1],
				 icom, jcom, &xmin, &xmax, &ymin, &ymax, &
				icode, &noroom, &err, &amesur[1], &xnold[1], &
				ynold[1], &nxkold[nxkold_offset], &mmpold[4], 
				&linkeg[3], &listeg[1], &bmesur[1], mlink, 
				nprold, npnold, npeold, nnxk, remesh, rexmin, 
				rexmax, reymin, reymax, idivis, sizmin, emax, 
				emin, (ftnlen)72, (ftnlen)72, (ftnlen)3, (
				ftnlen)72);
			if (noroom) {
			    mxnd = (integer) (mxnd * (float)1.2 + 1);
			    maxnbc = max(maxnbc,knbc);
			    maxsbc = max(maxsbc,ksbc);
			    ksbc = kksbc;
			    mdlong_("XN", &k[6], &mxnd, (ftnlen)2);
			    mdlong_("YN", &k[7], &mxnd, (ftnlen)2);
			    mdlong_("ZN", &k[29], &mxnd, (ftnlen)2);
			    mdlong_("NUID", &k[8], &mxnd, (ftnlen)4);
			    i__3 = mxnd << 2;
			    mdlong_("LXK", &k[9], &i__3, (ftnlen)3);
			    i__3 = mxnd * 6;
			    mdlong_("KXL", &k[10], &i__3, (ftnlen)3);
			    i__3 = mxnd * 6;
			    mdlong_("NXL", &k[11], &i__3, (ftnlen)3);
			    i__3 = mxnd << 2;
			    mdlong_("LXN", &k[12], &i__3, (ftnlen)3);
			    mdlong_("LSTNBC", &k[13], &maxnbc, (ftnlen)6);
			    mdlong_("LSTSBC", &k[14], &maxsbc, (ftnlen)6);
			    mdlong_("NXH", &k[18], &mxnd, (ftnlen)3);
			    mdlong_("INDX", &k[20], &mxnd, (ftnlen)4);
			    mdlong_("FANGLE", &k[21], &mxnd, (ftnlen)6);
			    i__3 = mxnd << 1;
			    mdlong_("BNSIZE", &k[22], &i__3, (ftnlen)6);
			    i__3 = mxnd * mln;
			    mdlong_("LNODES", &k[23], &i__3, (ftnlen)6);
			    mdstat_(&nerr, &mused);
			    if (nerr > 0) {
				mderor_(&c__6);
				s_stop(" ", (ftnlen)1);
			    }
			    mesage_("REDIMENSIONING NEEDED - PLEASE WAIT", (
				    ftnlen)35);
			    if (*step) {
				mesage_("CURRENT PROCESSING SCHEME IS SAVED", 
					(ftnlen)34);
			    } else {
				mesage_("CURRENT SCHEME WILL BE REPEATED", (
					ftnlen)31);
			    }
			    goto L260;
			} else if (err) {
			    if (*step) {
				intrup_("WOULD YOU LIKE TO REPROCESS REGION", 
					&ians, mcom, icom, jcom, cin + 72, &
					iin[1], &rin[1], &kin[1], (ftnlen)34, 
					(ftnlen)72);
				nnn = nnnold;
				kkk = kkkold;
				lll = lllold;
				if (ians) {
				    goto L260;
				}
			    }
			    mesage_("REGION PROCESSING ABORTED", (ftnlen)25);
			    goto L270;
			} else if (icode == iexit) {
			    final = j == j2;
			    if (j > j1) {
				fixsub_(&mxnd, &nnnold, &nnn, &lllold, &lll, &
					kkkold, &kkk, &a[k[6]], &a[k[7]], &ia[
					k[8]], &ia[k[9]], &ia[k[10]], &ia[k[
					11]], &ia[k[12]], &ia[k[20]], &iavail,
					 &navail, &final);
			    }
			    nnnold = nnn;
			    lllold = lll;
			    kkkold = kkk;
			    if (final) {
				fxnuid_(&nspr[igpntr], &islist[ifside[igpntr]]
					, mr, ms, ml, &nspr[1], &iline[1], &
					iside[1], &nlps[1], &ifline[1], &
					illist[1], &lcon[4], &islist[1], &
					ifside[1], &linkr[3], &links[3], &
					linkl[3], &nnn, &mxnl, &mxnd, &ia[k[3]
					], &ia[k[8]], &ia[k[11]], &ia[k[12]], 
					&ia[k[20]], &noroom, &err);
			    }
			    if (err) {
				mesage_("GROUP SCHEME PROCESSING NOT POSSIBLE"
					, (ftnlen)36);
				mesage_("GROUP PROCESSING ABORTED", (ftnlen)
					24);
				goto L300;
			    }
			} else if (icode == iover) {
			    nnn = nnnold;
			    kkk = kkkold;
			    lll = lllold;
			    goto L260;
			} else if (icode == iquit) {
			    nnn = nnnold;
			    kkk = kkkold;
			    lll = lllold;
			    goto L270;
			}
		    }
L270:
		    ;
		}
/*  BEGIN FULL SCHEME CONTROL FOR A GROUP REGION */
		nnnold = 0;
		kkkold = 0;
		rect = FALSE_;
		mesage_(" ", (ftnlen)1);
		mesage_("GROUP SCHEME PROCESSING BEGUN", (ftnlen)29);
		i__3 = (i__2 = iregn[igpntr], abs(i__2));
		ltsort_(mr, &linksc[3], &i__3, &ipntr, &addlnk);
		rgnsch_(mcom, icom, jcom, cin + 72, &iin[1], &rin[1], &kin[1],
			 step, &iregn[igpntr], &ipntr, &n[24], msc, scheme + 
			72, defsch, schstr, &lensch, &nper, &pentag, &triang, 
			&trnsit, &halfc, &fill, &icode, remesh, (ftnlen)72, (
			ftnlen)72, (ftnlen)72, (ftnlen)72);
		if (icode == iexit) {
		    chkkxl_(&mxnd, &ia[k[9]], &ia[k[10]], &lll, &err);
		    if (err) {
			mesage_("ERROR IN CHECK OF KXL ARRAY", (ftnlen)27);
			if (*step) {
			    intrup_("WOULD YOU LIKE TO REPROCESS GROUP", &
				    ians, mcom, icom, jcom, cin + 72, &iin[1],
				     &rin[1], &kin[1], (ftnlen)33, (ftnlen)72)
				    ;
			    if (ians) {
				goto L250;
			    }
			}
			mesage_("GROUP PROCESSING ABORTED", (ftnlen)24);
			goto L300;
		    }
		    bar = FALSE_;
		    ksbc = 0;
		    getsbc_(&mxnd, &mxnper, &nper, &nl, ml, &maxsbc, &maxprm, 
			    &nprm, &ia[k[2]], &ia[k[3]], &a[k[6]], &a[k[7]], &
			    ia[k[8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[
			    k[14]], &ia[k[19]], &ksbc, &lcon[4], &isboun[1], &
			    linkl[3], &nspf[1], &ifsb[1], &listsb[3], &linksb[
			    3], &lll, &bar, &err);
		    if (err) {
			mesage_("ERROR IN SORTING SIDE BOUNDARIES", (ftnlen)
				32);
			if (*step) {
			    intrup_("WOULD YOU LIKE TO REPROCESS GROUP", &
				    ians, mcom, icom, jcom, cin + 72, &iin[1],
				     &rin[1], &kin[1], (ftnlen)33, (ftnlen)72)
				    ;
			    if (ians) {
				goto L250;
			    }
			}
			mesage_("GROUP PROCESSING ABORTED", (ftnlen)24);
			goto L300;
		    }
		    mkused_(&mxnl, mp, ml, &ia[k[3]], &ipoint[1], &nint[1], &
			    linkp[3], &linkl[3], &lcon[4], &nl);
		    savreg_(&mxnd, &maxnbc, &maxsbc, &a[k[6]], &a[k[7]], &ia[
			    k[8]], &ia[k[9]], &ia[k[11]], &ia[k[12]], &ia[k[
			    13]], &ia[k[14]], &knbc, &ksbc, &nnn, &kkk, &igrp,
			     iunit, &bar, &m1);
		    i__2 = j2;
		    for (j = j1; j <= i__2; ++j) {
			i__4 = (i__3 = islist[j], abs(i__3));
			ltsort_(mr, &linkr[3], &i__4, &ipntr2, &addlnk);
			iregn[ipntr2] = (i__3 = iregn[ipntr2], abs(i__3));
/* L280: */
		    }
		    ++(*npregn);
		    *npelem += kkk;
		    *npnode += nnn;
		    if (*eight || *nine) {
			*npnode += lll;
		    }
		    if (*nine) {
			*npnode += kkk;
		    }
		    *npnbc += knbc;
		    if (*three || *eight || *nine) {
			*npnbc += knbc;
		    }
		    *npsbc += ksbc;
		    if (*three || *eight || *nine) {
			*npsbc += ksbc;
		    }
		    *maxkxn += lll;
		    iregn[igpntr] = (i__2 = iregn[igpntr], abs(i__2));
		    s_wsfe(&io___101);
		    do_fio(&c__1, (char *)&iregn[igpntr], (ftnlen)sizeof(
			    integer));
		    e_wsfe();
		    goto L300;
		} else if (icode == iover) {
		    goto L250;
		} else if (icode == iquit) {
		    goto L300;
		}
		if (*step) {
		    minmax_fq__(&mxnd, &nnn, &a[k[6]], &a[k[7]], &xmin, &xmax,
			     &ymin, &ymax);
		}
		pschem_(mp, ml, ms, mr, &n[1], &ipoint[1], &coor[3], &ipboun[
			1], &iline[1], &ltype[1], &nint[1], &factor[1], &lcon[
			4], &ilboun[1], &isboun[1], &iside[1], &nlps[1], &
			ifline[1], &illist[1], &iregn[1], &nspr[1], &ifside[1]
			, &islist[1], &nppf[1], &ifpb[1], &listpb[3], &nlpf[1]
			, &iflb[1], &listlb[3], &nspf[1], &ifsb[1], &listsb[3]
			, &linkp[3], &linkl[3], &links[3], &linkr[3], &linksc[
			3], &linkpb[3], &linklb[3], &linksb[3], &ifhole[1], &
			nhpr[1], &ihlist[1], &maxnbc, &knbc, &maxsbc, &ksbc, &
			mxnd, &nnn, &nnnold, &kkk, &kkkold, &lll, &a[k[0]], &
			a[k[1]], &ia[k[2]], &ia[k[3]], &a[k[6]], &a[k[7]], &
			ia[k[8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[k[12]
			], &ia[k[13]], &ia[k[18]], &ia[k[19]], &ia[k[25]], &
			iavail, &navail, &mxnl, &mxnper, &nper, &maxprm, &
			nprm, msc, &ischm[1], scheme + 72, schstr, &rect, &m1,
			 &inside, &jjhole, &kksbc, dev1, eight, nine, step, &
			igpntr, &nl, mcom, cin + 72, &iin[1], &rin[1], &kin[1]
			, icom, jcom, &xmin, &xmax, &ymin, &ymax, &icode, &
			noroom, &err, &amesur[1], &xnold[1], &ynold[1], &
			nxkold[nxkold_offset], &mmpold[4], &linkeg[3], &
			listeg[1], &bmesur[1], mlink, nprold, npnold, npeold, 
			nnxk, remesh, rexmin, rexmax, reymin, reymax, idivis, 
			sizmin, emax, emin, (ftnlen)72, (ftnlen)72, (ftnlen)3,
			 (ftnlen)72);
		if (noroom) {
		    mxnd = (integer) (mxnd * (float)1.2 + 1);
		    maxnbc = max(maxnbc,knbc);
		    maxsbc = max(maxsbc,ksbc);
		    ksbc = kksbc;
		    mdlong_("XN", &k[6], &mxnd, (ftnlen)2);
		    mdlong_("YN", &k[7], &mxnd, (ftnlen)2);
		    mdlong_("ZN", &k[29], &mxnd, (ftnlen)2);
		    mdlong_("NUID", &k[8], &mxnd, (ftnlen)4);
		    i__2 = mxnd << 2;
		    mdlong_("LXK", &k[9], &i__2, (ftnlen)3);
		    i__2 = mxnd * 6;
		    mdlong_("KXL", &k[10], &i__2, (ftnlen)3);
		    i__2 = mxnd * 6;
		    mdlong_("NXL", &k[11], &i__2, (ftnlen)3);
		    i__2 = mxnd << 2;
		    mdlong_("LXN", &k[12], &i__2, (ftnlen)3);
		    mdlong_("LSTNBC", &k[13], &maxnbc, (ftnlen)6);
		    mdlong_("LSTSBC", &k[14], &maxsbc, (ftnlen)6);
		    mdlong_("NXH", &k[18], &mxnd, (ftnlen)3);
		    mdlong_("INDX", &k[20], &mxnd, (ftnlen)4);
		    mdlong_("FANGLE", &k[21], &mxnd, (ftnlen)6);
		    i__2 = mxnd << 1;
		    mdlong_("BNSIZE", &k[22], &i__2, (ftnlen)6);
		    i__2 = mxnd * mln;
		    mdlong_("LNODES", &k[23], &i__2, (ftnlen)6);
		    mdstat_(&nerr, &mused);
		    if (nerr > 0) {
			mderor_(&c__6);
			s_stop(" ", (ftnlen)1);
		    }
		    mesage_("REDIMENSIONING NEEDED - PLEASE WAIT", (ftnlen)35)
			    ;
		    if (*step) {
			mesage_("CURRENT PROCESSING SCHEME IS SAVED", (ftnlen)
				34);
		    } else {
			mesage_("CURRENT SCHEME WILL BE REPEATED", (ftnlen)31)
				;
		    }
		    goto L250;
		} else if (err) {
		    if (*step) {
			intrup_("WOULD YOU LIKE TO REPROCESS GROUP", &ians, 
				mcom, icom, jcom, cin + 72, &iin[1], &rin[1], 
				&kin[1], (ftnlen)33, (ftnlen)72);
			if (ians) {
			    goto L250;
			}
		    }
		    mesage_("GROUP PROCESSING ABORTED", (ftnlen)24);
		    goto L300;
		} else if (icode == iexit) {
		    chkkxl_(&mxnd, &ia[k[9]], &ia[k[10]], &lll, &err);
		    if (err) {
			mesage_("ERROR IN CHECK OF KXL ARRAY", (ftnlen)27);
			if (*step) {
			    intrup_("WOULD YOU LIKE TO REPROCESS GROUP", &
				    ians, mcom, icom, jcom, cin + 72, &iin[1],
				     &rin[1], &kin[1], (ftnlen)33, (ftnlen)72)
				    ;
			    if (ians) {
				goto L250;
			    }
			}
			mesage_("GROUP PROCESSING ABORTED", (ftnlen)24);
			goto L300;
		    }
		    bar = FALSE_;
		    ksbc = 0;
		    getsbc_(&mxnd, &mxnper, &nper, &nl, ml, &maxsbc, &maxprm, 
			    &nprm, &ia[k[2]], &ia[k[3]], &a[k[6]], &a[k[7]], &
			    ia[k[8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[
			    k[14]], &ia[k[19]], &ksbc, &lcon[4], &isboun[1], &
			    linkl[3], &nspf[1], &ifsb[1], &listsb[3], &linksb[
			    3], &lll, &bar, &err);
		    if (err) {
			mesage_("ERROR IN SORTING SIDE BOUNDARIES", (ftnlen)
				32);
			if (*step) {
			    intrup_("WOULD YOU LIKE TO REPROCESS GROUP", &
				    ians, mcom, icom, jcom, cin + 72, &iin[1],
				     &rin[1], &kin[1], (ftnlen)33, (ftnlen)72)
				    ;
			    if (ians) {
				goto L250;
			    }
			}
			mesage_("GROUP PROCESSING ABORTED", (ftnlen)24);
			goto L300;
		    }
		    mkused_(&mxnl, mp, ml, &ia[k[3]], &ipoint[1], &nint[1], &
			    linkp[3], &linkl[3], &lcon[4], &nl);
		    savreg_(&mxnd, &maxnbc, &maxsbc, &a[k[6]], &a[k[7]], &ia[
			    k[8]], &ia[k[9]], &ia[k[11]], &ia[k[12]], &ia[k[
			    13]], &ia[k[14]], &knbc, &ksbc, &nnn, &kkk, &igrp,
			     iunit, &bar, &m1);
		    i__2 = j2;
		    for (j = j1; j <= i__2; ++j) {
			i__4 = (i__3 = islist[j], abs(i__3));
			ltsort_(mr, &linkr[3], &i__4, &ipntr2, &addlnk);
			iregn[ipntr2] = (i__3 = iregn[ipntr2], abs(i__3));
/* L290: */
		    }
		    ++(*npregn);
		    *npelem += kkk;
		    *npnode += nnn;
		    if (*eight || *nine) {
			*npnode += lll;
		    }
		    if (*nine) {
			*npnode += kkk;
		    }
		    *npnbc += knbc;
		    if (*three || *eight || *nine) {
			*npnbc += knbc;
		    }
		    *npsbc += ksbc;
		    if (*three || *eight || *nine) {
			*npsbc += ksbc;
		    }
		    *maxkxn += lll;
		    iregn[igpntr] = (i__2 = iregn[igpntr], abs(i__2));
		    s_wsfe(&io___102);
		    do_fio(&c__1, (char *)&iregn[igpntr], (ftnlen)sizeof(
			    integer));
		    e_wsfe();
		} else if (icode == iover) {
		    goto L250;
		} else if (icode == iquit) {
		    goto L300;
		}
	    }
L300:
/* L310: */
	    ;
	}
/*  END OF THIS SET OF GROUPS */
/*     IF STEPPING THROUGH, SEE IF ANY MORE GROUPS */
/*        ARE TO BE PROCESSED */
	if (*step) {
	    intrup_("PROCESS ADDITIONAL GROUPS", &ians, mcom, icom, jcom, cin 
		    + 72, &iin[1], &rin[1], &kin[1], (ftnlen)25, (ftnlen)72);
	    if (ians) {
		goto L240;
	    }
	}
    }
/*  SET UP THE LOOP FOR PROCESSING REGIONS */
L320:
    if (*step && n[22] > 0) {
	mesage_(" ", (ftnlen)1);
	mesage_("STEP PROCESS REGIONS I1 THROUGH I2", (ftnlen)34);
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    check_(&i1, &i2, &n[22]);
	} else {
	    goto L370;
	}
    } else {
	i1 = 1;
	i2 = n[22];
    }
/*  BEGIN PROCESSING REGIONS */
    real__ = TRUE_;
    count = FALSE_;
    i__1 = i2;
    for (i__ = i1; i__ <= i__1; ++i__) {
	ltsort_(mr, &linkr[3], &i__, &l, &addlnk);
	if (l > 0) {
	    if (iregn[l] < 0) {
		noroom = FALSE_;
		mesage_(" ", (ftnlen)1);
		s_wsfe(&io___103);
		i__3 = (i__2 = iregn[l], abs(i__2));
		do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		e_wsfe();
/*  CALCULATE THE PERIMETER OF THE REGION */
L330:
		nnn = 0;
		kkk = 0;
		lll = 0;
		nprm = 1;
		jjhole = 0;
		knbc = 0;
		i__3 = (i__2 = iregn[l], abs(i__2));
		perim_(mp, ml, ms, &nspr[l], &mxnl, &mxnper, &maxnbc, &maxsbc,
			 &knbc, &ksbc, &i__3, &ipoint[1], &coor[3], &ipboun[1]
			, &iline[1], &ltype[1], &nint[1], &factor[1], &lcon[4]
			, &ilboun[1], &isboun[1], &iside[1], &nlps[1], &
			ifline[1], &illist[1], &islist[ifside[l]], &nppf[1], &
			ifpb[1], &listpb[3], &nlpf[1], &iflb[1], &listlb[3], &
			nspf[1], &ifsb[1], &listsb[3], &linkp[3], &linkl[3], &
			links[3], &linkpb[3], &linklb[3], &linksb[3], &a[k[0]]
			, &a[k[1]], &ia[k[2]], &nper, &ia[k[3]], &nl, &ia[k[
			13]], &ia[k[25]], &even, &real__, &err, &ccw, &count, 
			&noroom, &amesur[1], &xnold[1], &ynold[1], &nxkold[
			nxkold_offset], &mmpold[4], &linkeg[3], &listeg[1], &
			bmesur[1], mlink, nprold, npnold, npeold, nnxk, 
			remesh, rexmin, rexmax, reymin, reymax, idivis, 
			sizmin, emax, emin);
/*  GET THE REGION SCHEME */
		i__3 = (i__2 = iregn[l], abs(i__2));
		ltsort_(mr, &linksc[3], &i__3, &ipntr, &addlnk);
		rgnsch_(mcom, icom, jcom, cin + 72, &iin[1], &rin[1], &kin[1],
			 step, &iregn[l], &ipntr, &n[24], msc, scheme + 72, 
			defsch, schstr, &lensch, &nper, &pentag, &triang, &
			trnsit, &halfc, &fill, &icode, remesh, (ftnlen)72, (
			ftnlen)72, (ftnlen)72, (ftnlen)72);
		if (icode == iexit) {
		    goto L360;
		} else if (icode == iover) {
		    goto L330;
		} else if (icode == iquit) {
		    goto L350;
/*  GENERATE INITIAL GRID */
/*  CALCULATE A "TRANSITION" MAPPED MESH */
		} else if (trnsit) {
		    bmschm_(&nper, &kkk, &lll, &nnn, ml, ms, &nspr[l], &
			    islist[ifside[l]], &nint[1], &ifline[1], &nlps[1],
			     &illist[1], &linkl[3], &links[3], &mxnper, &
			    maxprm, &max3, &mxnd, &a[k[0]], &a[k[1]], &ia[k[2]
			    ], &ia[k[4]], &a[k[5]], &a[k[6]], &a[k[7]], &ia[k[
			    8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[k[12]]
			    , &a[k[15]], &a[k[16]], &ia[k[17]], &ia[k[20]], &
			    iavail, &navail, &ccw, &halfc, &err);
/*  CALCULATE A "TRIANGULAR" MAPPED MESH */
		} else if (triang) {
		    tmschm_(&nper, &kkk, &lll, &nnn, ml, ms, &nspr[l], &
			    islist[ifside[l]], &nint[1], &ifline[1], &nlps[1],
			     &illist[1], &linkl[3], &links[3], &mxnper, &
			    maxprm, &max3, &mxnd, &a[k[0]], &a[k[1]], &ia[k[2]
			    ], &ia[k[4]], &a[k[5]], &a[k[6]], &a[k[7]], &ia[k[
			    8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[k[12]]
			    , &a[k[15]], &a[k[16]], &ia[k[17]], &ia[k[20]], &
			    iavail, &navail, &ccw, &err);
/*  CALCULATE A "PENTAGON" MAPPED MESH */
		} else if (pentag) {
		    umschm_(&ia[1], &nper, &kkk, &lll, &nnn, ml, ms, &nspr[l],
			     &islist[ifside[l]], &nint[1], &ifline[1], &nlps[
			    1], &illist[1], &linkl[3], &links[3], &mxnper, &
			    maxprm, &max3, &mxnd, &a[k[0]], &a[k[1]], &ia[k[2]
			    ], &ia[k[4]], &a[k[5]], &a[k[6]], &a[k[7]], &ia[k[
			    8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[k[12]]
			    , &a[k[15]], &a[k[16]], &ia[k[17]], &ia[k[20]], &
			    iavail, &navail, &ccw, &err);
/*  USE THE PAVING TECHNIQUE TO FILL THE INITIAL REGION */
		} else if (fill) {
		    i__3 = (i__2 = iregn[l], abs(i__2));
		    pmschm_(&nper, &nprm, &mxnd, &mln, mp, ml, ms, mr, &nl, &
			    mxnl, &mxnper, &maxprm, &maxnb, &maxnbc, &maxsbc, 
			    &knbc, &ksbc, &knum, &ipoint[1], &coor[3], &
			    ipboun[1], &iline[1], &ltype[1], &nint[1], &
			    factor[1], &lcon[4], &ilboun[1], &isboun[1], &
			    iside[1], &nlps[1], &ifline[1], &illist[1], &
			    islist[1], &iregn[1], &nppf[1], &ifpb[1], &listpb[
			    3], &nlpf[1], &iflb[1], &listlb[3], &nspf[1], &
			    ifsb[1], &listsb[3], &linkp[3], &linkl[3], &links[
			    3], &linkr[3], &linkpb[3], &linklb[3], &linksb[3],
			     &nspr[1], &ifside[1], &rsize[1], &ifhole[1], &
			    nhpr[1], &ihlist[1], &a[k[0]], &a[k[1]], &ia[k[2]]
			    , &ia[k[3]], &a[k[6]], &a[k[7]], &a[k[29]], &ia[k[
			    8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[k[12]]
			    , &ia[k[13]], &ia[k[19]], &a[k[21]], &a[k[22]], &
			    ia[k[23]], &ia[k[24]], &ia[k[25]], &ia[k[26]], &
			    ia[k[27]], &ia[k[28]], &kkk, &nnn, &lll, &iavail, 
			    &navail, dev1, &i__3, &l, batch, &noroom, &err, &
			    amesur[1], &xnold[1], &ynold[1], &nxkold[
			    nxkold_offset], &mmpold[4], &linkeg[3], &listeg[1]
			    , &bmesur[1], mlink, nprold, npnold, npeold, nnxk,
			     remesh, rexmin, rexmax, reymin, reymax, idivis, 
			    sizmin, emax, emin, (ftnlen)3);
		    if (noroom) {
			mxnd = (integer) (mxnd * (float)1.5 + 1);
			maxnbc = max(maxnbc,knbc);
			maxsbc = max(maxsbc,ksbc);
			ksbc = kksbc;
			mdlong_("XN", &k[6], &mxnd, (ftnlen)2);
			mdlong_("YN", &k[7], &mxnd, (ftnlen)2);
			mdlong_("ZN", &k[29], &mxnd, (ftnlen)2);
			mdlong_("NUID", &k[8], &mxnd, (ftnlen)4);
			i__2 = mxnd << 2;
			mdlong_("LXK", &k[9], &i__2, (ftnlen)3);
			i__2 = mxnd * 6;
			mdlong_("KXL", &k[10], &i__2, (ftnlen)3);
			i__2 = mxnd * 6;
			mdlong_("NXL", &k[11], &i__2, (ftnlen)3);
			i__2 = mxnd << 2;
			mdlong_("LXN", &k[12], &i__2, (ftnlen)3);
			mdlong_("LSTNBC", &k[13], &maxnbc, (ftnlen)6);
			mdlong_("LSTSBC", &k[14], &maxsbc, (ftnlen)6);
			mdlong_("NXH", &k[18], &mxnd, (ftnlen)3);
			mdlong_("INDX", &k[20], &mxnd, (ftnlen)4);
			mdlong_("FANGLE", &k[21], &mxnd, (ftnlen)6);
			i__2 = mxnd << 1;
			mdlong_("BNSIZE", &k[22], &i__2, (ftnlen)6);
			i__2 = mxnd * mln;
			mdlong_("LNODES", &k[23], &i__2, (ftnlen)6);
			mdstat_(&nerr, &mused);
			if (nerr > 0) {
			    mderor_(&c__6);
			    s_stop(" ", (ftnlen)1);
			}
			mesage_("REDIMENSIONING NEEDED - PLEASE WAIT", (
				ftnlen)35);
			if (*step) {
			    mesage_("CURRENT PROCESSING SCHEME IS SAVED", (
				    ftnlen)34);
			} else {
			    mesage_("CURRENT SCHEME WILL BE REPEATED", (
				    ftnlen)31);
			}
			goto L330;
		    }
/*  PROCESS A "NORMAL" REGION */
		} else {
		    mmschm_(&nper, &kkk, &lll, &nnn, ml, ms, &nspr[l], &
			    islist[ifside[l]], &nint[1], &ifline[1], &nlps[1],
			     &illist[1], &linkl[3], &links[3], &mxnper, &
			    maxprm, &max3, &mxnd, &a[k[0]], &a[k[1]], &ia[k[2]
			    ], &ia[k[4]], &a[k[5]], &a[k[6]], &a[k[7]], &ia[k[
			    8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[k[12]]
			    , &iavail, &navail, &ccw, &real__, schstr, &m1, &
			    err, (ftnlen)72);
		}
/*  FLAG THE REGION IF AN ERROR HAS OCCURRED */
		if (err) {
		    mesage_("ERROR IN INITIAL GRID GENERATION", (ftnlen)32);
		    mesage_("** REGION PROCESSING ABORTED **", (ftnlen)31);
		    mesage_(" ", (ftnlen)1);
		    pltbel_();
		    pltflu_();
		    iregn[l] = (i__2 = iregn[l], abs(i__2));
		    i__2 = n[9];
		    for (j = 1; j <= i__2; ++j) {
			if (irpb[j] == iregn[l]) {
			    addlnk = TRUE_;
			    iminus = -l;
			    ltsort_(mr, &linkr[3], &iregn[l], &iminus, &
				    addlnk);
			    addlnk = FALSE_;
			}
/* L340: */
		    }
		    goto L350;
		}
/*  BEGIN FULL SCHEME CONTROL */
		rect = ! (pentag || triang || trnsit || fill);
		if (*step) {
		    minmax_fq__(&mxnper, &nper, &a[k[0]], &a[k[1]], &xmin, &
			    xmax, &ymin, &ymax);
		}
		nnnold = 0;
		kkkold = 0;
		pschem_(mp, ml, ms, mr, &n[1], &ipoint[1], &coor[3], &ipboun[
			1], &iline[1], &ltype[1], &nint[1], &factor[1], &lcon[
			4], &ilboun[1], &isboun[1], &iside[1], &nlps[1], &
			ifline[1], &illist[1], &iregn[1], &nspr[1], &ifside[1]
			, &islist[1], &nppf[1], &ifpb[1], &listpb[3], &nlpf[1]
			, &iflb[1], &listlb[3], &nspf[1], &ifsb[1], &listsb[3]
			, &linkp[3], &linkl[3], &links[3], &linkr[3], &linksc[
			3], &linkpb[3], &linklb[3], &linksb[3], &ifhole[1], &
			nhpr[1], &ihlist[1], &maxnbc, &knbc, &maxsbc, &ksbc, &
			mxnd, &nnn, &nnnold, &kkk, &kkkold, &lll, &a[k[0]], &
			a[k[1]], &ia[k[2]], &ia[k[3]], &a[k[6]], &a[k[7]], &
			ia[k[8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[k[12]
			], &ia[k[13]], &ia[k[18]], &ia[k[19]], &ia[k[25]], &
			iavail, &navail, &mxnl, &mxnper, &nper, &maxprm, &
			nprm, msc, &ischm[1], scheme + 72, schstr, &rect, &m1,
			 &inside, &jjhole, &kksbc, dev1, eight, nine, step, &
			l, &nl, mcom, cin + 72, &iin[1], &rin[1], &kin[1], 
			icom, jcom, &xmin, &xmax, &ymin, &ymax, &icode, &
			noroom, &err, &amesur[1], &xnold[1], &ynold[1], &
			nxkold[nxkold_offset], &mmpold[4], &linkeg[3], &
			listeg[1], &bmesur[1], mlink, nprold, npnold, npeold, 
			nnxk, remesh, rexmin, rexmax, reymin, reymax, idivis, 
			sizmin, emax, emin, (ftnlen)72, (ftnlen)72, (ftnlen)3,
			 (ftnlen)72);
		if (noroom) {
		    mxnd = (integer) (mxnd * (float)1.2 + 1);
		    maxnbc = max(maxnbc,knbc) << 1;
		    maxsbc = max(maxsbc,ksbc) << 1;
		    ksbc = kksbc;
		    mdlong_("XN", &k[6], &mxnd, (ftnlen)2);
		    mdlong_("YN", &k[7], &mxnd, (ftnlen)2);
		    mdlong_("ZN", &k[29], &mxnd, (ftnlen)2);
		    mdlong_("NUID", &k[8], &mxnd, (ftnlen)4);
		    i__2 = mxnd << 2;
		    mdlong_("LXK", &k[9], &i__2, (ftnlen)3);
		    i__2 = mxnd * 6;
		    mdlong_("KXL", &k[10], &i__2, (ftnlen)3);
		    i__2 = mxnd * 6;
		    mdlong_("NXL", &k[11], &i__2, (ftnlen)3);
		    i__2 = mxnd << 2;
		    mdlong_("LXN", &k[12], &i__2, (ftnlen)3);
		    mdlong_("LSTNBC", &k[13], &maxnbc, (ftnlen)6);
		    mdlong_("LSTSBC", &k[14], &maxsbc, (ftnlen)6);
		    mdlong_("NXH", &k[18], &mxnd, (ftnlen)3);
		    mdlong_("INDX", &k[20], &mxnd, (ftnlen)4);
		    mdlong_("FANGLE", &k[21], &mxnd, (ftnlen)6);
		    i__2 = mxnd << 1;
		    mdlong_("BNSIZE", &k[22], &i__2, (ftnlen)6);
		    i__2 = mxnd * mln;
		    mdlong_("LNODES", &k[23], &i__2, (ftnlen)6);
		    mdstat_(&nerr, &mused);
		    if (nerr > 0) {
			mderor_(&c__6);
			s_stop(" ", (ftnlen)1);
		    }
		    mesage_("REDIMENSIONING NEEDED - PLEASE WAIT", (ftnlen)35)
			    ;
		    if (*step) {
			mesage_("CURRENT PROCESSING SCHEME IS SAVED", (ftnlen)
				34);
		    } else {
			mesage_("CURRENT SCHEME WILL BE REPEATED", (ftnlen)31)
				;
		    }
		    goto L330;
		} else if (err) {
		    if (*step) {
			intrup_("WOULD YOU LIKE TO REPROCESS REGION", &ians, 
				mcom, icom, jcom, cin + 72, &iin[1], &rin[1], 
				&kin[1], (ftnlen)34, (ftnlen)72);
			if (ians) {
			    goto L330;
			}
		    }
		    mesage_("REGION PROCESSING ABORTED", (ftnlen)25);
		    goto L350;
		} else if (icode == iexit) {
		    chkkxl_(&mxnd, &ia[k[9]], &ia[k[10]], &lll, &err);
		    if (err) {
			mesage_("ERROR IN CHECK OF KXL ARRAY", (ftnlen)27);
			if (*step) {
			    intrup_("WOULD YOU LIKE TO REPROCESS REGION", &
				    ians, mcom, icom, jcom, cin + 72, &iin[1],
				     &rin[1], &kin[1], (ftnlen)34, (ftnlen)72)
				    ;
			    if (ians) {
				goto L330;
			    }
			}
			mesage_("REGION PROCESSING ABORTED", (ftnlen)25);
			goto L350;
		    }
		    bar = FALSE_;
		    ksbc = 0;
		    getsbc_(&mxnd, &mxnper, &nper, &nl, ml, &maxsbc, &maxprm, 
			    &nprm, &ia[k[2]], &ia[k[3]], &a[k[6]], &a[k[7]], &
			    ia[k[8]], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[
			    k[14]], &ia[k[19]], &ksbc, &lcon[4], &isboun[1], &
			    linkl[3], &nspf[1], &ifsb[1], &listsb[3], &linksb[
			    3], &lll, &bar, &err);
		    if (err) {
			mesage_("ERROR IN SORTING SIDE BOUNDARIES", (ftnlen)
				32);
			if (*step) {
			    intrup_("WOULD YOU LIKE TO REPROCESS REGION", &
				    ians, mcom, icom, jcom, cin + 72, &iin[1],
				     &rin[1], &kin[1], (ftnlen)34, (ftnlen)72)
				    ;
			    if (ians) {
				goto L330;
			    }
			}
			mesage_("REGION PROCESSING ABORTED", (ftnlen)25);
			goto L350;
		    }
		    mkused_(&mxnl, mp, ml, &ia[k[3]], &ipoint[1], &nint[1], &
			    linkp[3], &linkl[3], &lcon[4], &nl);
		    i__3 = (i__2 = iregn[l], abs(i__2));
		    savreg_(&mxnd, &maxnbc, &maxsbc, &a[k[6]], &a[k[7]], &ia[
			    k[8]], &ia[k[9]], &ia[k[11]], &ia[k[12]], &ia[k[
			    13]], &ia[k[14]], &knbc, &ksbc, &nnn, &kkk, &i__3,
			     iunit, &bar, &m1);
		    iregn[l] = (i__2 = iregn[l], abs(i__2));
		    ++(*npregn);
		    *npelem += kkk;
		    *npnode += nnn;
		    if (*eight || *nine) {
			*npnode += lll;
		    }
		    if (*nine) {
			*npnode += kkk;
		    }
		    *npnbc += knbc;
		    if (*three || *eight || *nine) {
			*npnbc += knbc;
		    }
		    *npsbc += ksbc;
		    if (*three || *eight || *nine) {
			*npsbc += ksbc;
		    }
		    *maxkxn += lll;
		    iregn[l] = (i__2 = iregn[l], abs(i__2));
		    s_wsfe(&io___104);
		    do_fio(&c__1, (char *)&iregn[l], (ftnlen)sizeof(integer));
		    e_wsfe();
		} else if (icode == iover) {
		    goto L330;
		} else if (icode == iquit) {
		    goto L350;
		}
	    }
	}
L350:
	;
    }
/*  END OF THIS SET OF REGIONS */
/*     IF STEPPING THROUGH, SEE IF ANY MORE REGIONS */
/*        ARE TO BE PROCESSED */
L360:
    if (*step) {
	intrup_("PROCESS ADDITIONAL REGIONS", &ians, mcom, icom, jcom, cin + 
		72, &iin[1], &rin[1], &kin[1], (ftnlen)26, (ftnlen)72);
	if (ians) {
	    goto L320;
	}
    }
/*  SET UP THE LOOP FOR PROCESSING BAR SETS */
L370:
L380:
    if (*step && n[21] > 0) {
	mesage_("STEP PROCESS BAR SETS I1 THROUGH I2", (ftnlen)35);
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    check_(&i1, &i2, &n[21]);
	} else {
	    goto L450;
	}
    } else {
	i1 = 1;
	i2 = n[21];
    }
/*  BEGIN PROCESSING BAR SETS */
    real__ = TRUE_;
    count = FALSE_;
    i__1 = i2;
    for (i__ = i1; i__ <= i__1; ++i__) {
	ltsort_(ms, &linkb[3], &i__, &ipntr, &addlnk);
/*  SEE IF THIS BAR SET IS FOR SPRINGS */
	if (ipntr > 0 && ibarst[ipntr] < 0 && jmat[ipntr] < 0) {
	    l = ipntr;
	    s_wsfe(&io___105);
	    i__3 = (i__2 = ibarst[l], abs(i__2));
	    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
	    e_wsfe();
	    spring_(mp, ml, ms, &mxnper, &mxnd, &maxnbc, &maxsbc, &l, &ipoint[
		    1], &coor[3], &ipboun[1], &linkp[3], &iline[1], &ltype[1],
		     &nint[1], &factor[1], &lcon[4], &ilboun[1], &isboun[1], &
		    linkl[3], &nlpb[1], &jfline[1], &jllist[1], &linkpb[3], &
		    nppf[1], &ifpb[1], &listpb[3], &linklb[3], &nlpf[1], &
		    iflb[1], &listlb[3], &linksb[3], &nspf[1], &ifsb[1], &
		    listsb[3], &lstnbc, &a[k[0]], &a[k[1]], &ia[k[2]], &a[k[6]
		    ], &a[k[7]], &ia[k[8]], &ia[k[9]], &nnn, &kkk, &lll, &
		    knbc, &ksbc, &err, &addlnk, &count, &noroom, &amesur[1], &
		    xnold[1], &ynold[1], &nxkold[nxkold_offset], &mmpold[4], &
		    linkeg[3], &listeg[1], &bmesur[1], mlink, nprold, npnold, 
		    npeold, nnxk, remesh, rexmin, rexmax, reymin, reymax, 
		    idivis, sizmin, emax, emin, &graph);
	    if (err) {
		mesage_("ERROR IN 2-NODE SPRING ELEMENT GENERATION", (ftnlen)
			41);
		mesage_("BAR SET PROCESSING ABORTED", (ftnlen)26);
		mesage_(" ", (ftnlen)1);
		pltbel_();
		pltflu_();
		ibarst[l] = (i__2 = ibarst[l], abs(i__2));
		i__2 = n[9];
		for (m = 1; m <= i__2; ++m) {
		    if ((i__3 = irpb[m], abs(i__3)) == ibarst[l]) {
			ltsort_(ms, &linkb[3], &irpb[m], &ipntr, &addlnk);
			addlnk = TRUE_;
			iminus = -ipntr;
			ltsort_(ms, &linkb[3], &irpb[m], &iminus, &addlnk);
			addlnk = FALSE_;
		    }
/* L390: */
		}
		goto L430;
	    }
/*  PROCESS A REGULAR BARSET */
	} else if (ipntr > 0 && ibarst[ipntr] < 0) {
	    l = ipntr;
	    s_wsfe(&io___108);
	    i__3 = (i__2 = ibarst[l], abs(i__2));
	    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
	    e_wsfe();
	    real__ = TRUE_;
	    test = FALSE_;
	    kkk = 0;
	    nnn = 0;
	    knbc = 0;
	    ksbc = 0;
	    lll = 1;
/*  LOOP THROUGH ALL THE LINES IN THE BAR SETS */
	    i__2 = jfline[l] + nlpb[l] - 1;
	    for (j = jfline[l]; j <= i__2; ++j) {
		ltsort_(ml, &linkl[3], &jllist[j], &kk, &addlnk);
		ltsort_(mp, &linkp[3], &lcon[kk * 3 + 1], &ip1, &addlnk);
		ltsort_(mp, &linkp[3], &lcon[kk * 3 + 2], &ip2, &addlnk);
		if (lcon[kk * 3 + 3] > 0) {
		    ltsort_(mp, &linkp[3], &lcon[kk * 3 + 3], &ip3, &addlnk);
		} else if (lcon[kk * 3 + 3] < 0) {
		    i__4 = (i__3 = lcon[kk * 3 + 3], abs(i__3));
		    ltsort_(mp, &linkp[3], &i__4, &ip3, &addlnk);
		    ip3 = -ip3;
		} else {
		    ip3 = 0;
		}
/*  CALCULATE NODES IN THE BAR SET LINE */
		pline_(mp, ml, &mxnper, &maxnbc, &maxsbc, &ipoint[1], &coor[3]
			, &linkp[3], &iline[kk], &ltype[kk], &nint[kk], &
			factor[kk], &ip1, &ip2, &ip3, &a[k[0]], &a[k[1]], &ia[
			k[2]], &ipboun[ip1], &ipboun[ip2], &ilboun[kk], &
			isboun[kk], &linkpb[3], &nppf[1], &ifpb[1], &listpb[3]
			, &linklb[3], &nlpf[1], &iflb[1], &listlb[3], &linksb[
			3], &nspf[1], &ifsb[1], &listsb[3], &ia[k[13]], &knbc,
			 &ksbc, &err, &test, &real__, &count, &noroom, &
			amesur[1], &xnold[1], &ynold[1], &nxkold[
			nxkold_offset], &mmpold[4], &linkeg[3], &listeg[1], &
			bmesur[1], mlink, nprold, npnold, npeold, nnxk, 
			remesh, rexmin, rexmax, reymin, reymax, idivis, 
			sizmin, emax, emin, &graph, &dxmax);
		if (err) {
		    mesage_("ERROR IN 2-NODE ELEMENT GENERATION", (ftnlen)34);
		    mesage_("BAR SET PROCESSING ABORTED", (ftnlen)26);
		    mesage_(" ", (ftnlen)1);
		    pltbel_();
		    pltflu_();
		    ibarst[l] = (i__3 = ibarst[l], abs(i__3));
		    i__3 = n[9];
		    for (m = 1; m <= i__3; ++m) {
			if ((i__4 = irpb[m], abs(i__4)) == ibarst[l]) {
			    ltsort_(ms, &linkb[3], &irpb[m], &ipntr, &addlnk);
			    addlnk = TRUE_;
			    iminus = -ipntr;
			    ltsort_(ms, &linkb[3], &irpb[m], &iminus, &addlnk)
				    ;
			    addlnk = FALSE_;
			}
/* L400: */
		    }
		    goto L430;
		}
/*  ADD THESE NODES AND ELEMENTS TO THE CURRENT LIST */
		nnn0 = nnn + 1;
		nnn = nnn + (i__3 = nint[kk], abs(i__3)) + 1;
		if (jcent[l] > 0) {
		    ltsort_(mp, &linkp[3], &jcent[l], &ip3, &addlnk);
		} else {
		    ip3 = 0;
		}
		mak2el_(mp, &mxnper, &mxnd, &nnn0, &nnn, &kkk, &a[k[0]], &a[k[
			1]], &ia[k[2]], &a[k[6]], &a[k[7]], &ia[k[8]], &ia[k[
			9]], &coor[3], &ip3);
/*  MARK THESE POINTS AND THE LINE AS BEING USED */
		nint[kk] = -(i__3 = nint[kk], abs(i__3));
		ipoint[ip1] = -(i__3 = ipoint[ip1], abs(i__3));
		ipoint[ip2] = -(i__3 = ipoint[ip2], abs(i__3));
/* L410: */
	    }
	}
/*  WRITE OUT THE BAR SET ELEMENTS AND BOUNDARY CONDITIONS */
	if (ipntr > 0 && ibarst[ipntr] < 0) {
	    bar = TRUE_;
	    ksbc = 0;
	    getsbc_(&mxnd, &nnn, &nnn, &nlpb[l], ml, &maxsbc, &c__1, &c__1, &
		    ia[k[2]], &jllist[jfline[l]], &a[k[6]], &a[k[7]], &ia[k[8]
		    ], &ia[k[9]], &ia[k[10]], &ia[k[11]], &ia[k[14]], &ia[k[
		    19]], &ksbc, &lcon[4], &isboun[1], &linkl[3], &nspf[1], &
		    ifsb[1], &listsb[3], &linksb[3], &kkk, &bar, &err);
	    if (err) {
		mesage_("ERROR IN SORTING SIDE BOUNDARIES", (ftnlen)32);
		mesage_("BAR SET PROCESSING ABORTED", (ftnlen)26);
		mesage_(" ", (ftnlen)1);
		ibarst[l] = (i__2 = ibarst[l], abs(i__2));
		i__2 = n[9];
		for (m = 1; m <= i__2; ++m) {
		    if ((i__3 = irpb[m], abs(i__3)) == ibarst[l]) {
			ltsort_(ms, &linkb[3], &irpb[m], &ipntr, &addlnk);
			addlnk = TRUE_;
			iminus = -ipntr;
			ltsort_(ms, &linkb[3], &irpb[m], &iminus, &addlnk);
			addlnk = FALSE_;
		    }
/* L420: */
		}
		goto L430;
	    }
	    i__3 = (i__2 = ibarst[l], abs(i__2));
	    savreg_(&mxnd, &maxnbc, &maxsbc, &a[k[6]], &a[k[7]], &ia[k[8]], &
		    ia[k[9]], &ia[k[11]], &ia[k[12]], &ia[k[13]], &ia[k[14]], 
		    &knbc, &ksbc, &nnn, &kkk, &i__3, iunit, &bar, &m1);
	    ++(*npregn);
	    *npelem += kkk;
	    *npnode += nnn;
	    if (*three) {
/* Computing MAX */
		i__2 = nnn, i__3 = kkk + 1;
		*npnode += max(i__2,i__3);
	    }
	    *npnbc += knbc;
	    if (*three) {
		*npnbc += knbc;
	    }
	    *npsbc += ksbc;
	    if (*three) {
		*npsbc += ksbc;
	    }
	    *maxkxn = *maxkxn + kkk + nlpb[l];
	    ibarst[l] = (i__2 = ibarst[l], abs(i__2));
	    s_wsfe(&io___110);
	    do_fio(&c__1, (char *)&ibarst[l], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
/*  END OF THIS BAR SET */
L430:
/* L440: */
	;
    }
/*  END OF THIS GROUP OF BAR SETS */
/*  IF STEPPING THROUGH, SEE IF ANY MORE BAR SETS ARE TO BE PROCESSED */
    if (*step && n[21] > 0) {
	if (*icom <= *jcom && (*(unsigned char *)&cin[*icom * 72] == 'Y' || *(
		unsigned char *)&cin[*icom * 72] == 'y')) {
	    ians = TRUE_;
	    ++(*icom);
	} else if (*icom <= *jcom && (*(unsigned char *)&cin[*icom * 72] == 
		'N' || *(unsigned char *)&cin[*icom * 72] == 'n')) {
	    ians = TRUE_;
	    ++(*icom);
	} else {
	    if (*icom <= *jcom && (*(unsigned char *)&cin[*icom * 72] == 'Y' 
		    || *(unsigned char *)&cin[*icom * 72] == 'y')) {
		ians = TRUE_;
		++(*icom);
	    } else if (*icom <= *jcom && (*(unsigned char *)&cin[*icom * 72] 
		    == 'N' || *(unsigned char *)&cin[*icom * 72] == 'n')) {
		ians = FALSE_;
		++(*icom);
	    } else {
		mesage_(" ", (ftnlen)1);
		intrup_("PROCESS ADDITIONAL BAR SETS", &ians, mcom, icom, 
			jcom, cin + 72, &iin[1], &rin[1], &kin[1], (ftnlen)27,
			 (ftnlen)72);
	    }
	}
	if (ians) {
	    goto L380;
	}
    }
/*  RESTORE THE DATA BASE TO ITS INITIAL CONDITION */
L450:
    i__1 = n[1];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ipoint[i__] = (i__2 = ipoint[i__], abs(i__2));
/* L460: */
    }
    i__1 = n[2];
    for (i__ = 1; i__ <= i__1; ++i__) {
	nint[i__] = (i__2 = nint[i__], abs(i__2));
/* L470: */
    }
    i__1 = n[5];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ibarst[i__] = (i__2 = ibarst[i__], abs(i__2));
/* L480: */
    }
    i__1 = n[7];
    for (i__ = 1; i__ <= i__1; ++i__) {
	iregn[i__] = (i__2 = iregn[i__], abs(i__2));
/* L490: */
    }
    i__1 = n[5];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(ms, &linkb[3], &ibarst[i__], &ipntr, &addlnk);
	addlnk = TRUE_;
	iplus = abs(ipntr);
	ltsort_(ms, &linkb[3], &ibarst[i__], &iplus, &addlnk);
	addlnk = FALSE_;
/* L500: */
    }
    i__1 = n[7];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mr, &linkr[3], &iregn[i__], &ipntr, &addlnk);
	addlnk = TRUE_;
	iplus = abs(ipntr);
	ltsort_(mr, &linkr[3], &iregn[i__], &iplus, &addlnk);
	addlnk = FALSE_;
/* L510: */
    }
    mddel_("X", (ftnlen)1);
    mddel_("Y", (ftnlen)1);
    mddel_("NID", (ftnlen)3);
    mddel_("LISTL", (ftnlen)5);
    mddel_("NNPS", (ftnlen)4);
    mddel_("ANGLE", (ftnlen)5);
    mddel_("XN", (ftnlen)2);
    mddel_("YN", (ftnlen)2);
    mddel_("NUID", (ftnlen)4);
    mddel_("LXK", (ftnlen)3);
    mddel_("KXL", (ftnlen)3);
    mddel_("NXL", (ftnlen)3);
    mddel_("LXN", (ftnlen)3);
    mddel_("LSTNBC", (ftnlen)6);
    mddel_("LSTSBC", (ftnlen)6);
    mddel_("XSUB", (ftnlen)4);
    mddel_("YSUB", (ftnlen)4);
    mddel_("NIDSUB", (ftnlen)6);
    mddel_("NXH", (ftnlen)3);
    mddel_("NPERIM", (ftnlen)6);
    mddel_("INDX", (ftnlen)4);
    mddel_("FANGLE", (ftnlen)6);
    mddel_("BNSIZE", (ftnlen)6);
    mddel_("LNODES", (ftnlen)6);
    mddel_("PRLINK", (ftnlen)6);
    mddel_("MARKED", (ftnlen)6);
    mddel_("IPTPER", (ftnlen)6);
    mddel_("NUMPER", (ftnlen)6);
    mddel_("LPERIM", (ftnlen)6);
    mddel_("ZN", (ftnlen)2);
    mdstat_(&nerr, &mused);
    if (nerr > 0) {
	mderor_(&c__6);
	s_stop(" ", (ftnlen)1);
    }
    return 0;
} /* qmesh_ */

#ifdef __cplusplus
	}
#endif

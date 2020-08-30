/* chkrgn.f -- translated by f2c (version 20160102).
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

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int chkrgn_(integer *ia, integer *l, integer *mp, integer *
	ml, integer *ms, integer *mr, integer *msc, integer *n24, integer *
	ipoint, real *coor, integer *ipboun, integer *iline, integer *ltype, 
	integer *nint, real *factor, integer *lcon, integer *ilboun, integer *
	isboun, integer *iside, integer *nlps, integer *ifline, integer *
	illist, integer *iregn, integer *nspr, integer *ifside, integer *
	islist, integer *nppf, integer *ifpb, integer *listpb, integer *nlpf, 
	integer *iflb, integer *listlb, integer *nspf, integer *ifsb, integer 
	*listsb, integer *ifhole, integer *nhpr, integer *ihlist, integer *
	linkp, integer *linkl, integer *links, integer *linkr, integer *
	linksc, integer *linkpb, integer *linklb, integer *linksb, real *
	rsize, char *scheme, char *defsch, integer *npregn, integer *npsbc, 
	integer *npnode, integer *maxnp, integer *maxnl, integer *max3, real *
	x, real *y, integer *nid, integer *listl, integer *nnps, real *angle, 
	integer *marked, integer *mxnd, integer *mxnper, integer *mxnl, 
	integer *maxnbc, integer *maxsbc, real *amesur, real *xnold, real *
	ynold, integer *nxkold, integer *mmpold, integer *linkeg, integer *
	listeg, real *bmesur, integer *mlink, integer *nprold, integer *
	npnold, integer *npeold, integer *nnxk, logical *remesh, real *rexmin,
	 real *rexmax, real *reymin, real *reymax, integer *idivis, real *
	sizmin, real *emax, real *emin, logical *noroom, logical *errchk, 
	logical *err, ftnlen scheme_len, ftnlen defsch_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 ** ERROR - DATA PROBLEMS FOR REGION:\
\002,i5,\002 **\002)";
    static char fmt_10010[] = "(\002 ** ERROR - PERIMETER GENERATION ERRORS \
FOR REGION:\002,i5,\002 **\002)";
    static char fmt_10020[] = "(\002 ** ERROR - MAPPING BASE GENERATION ERRO\
RS FOR REGION:\002,i5,\002 **\002)";

    /* System generated locals */
    integer nxkold_dim1, nxkold_offset, i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer j, i1, i2, i3, i4, i5, i6, i7, i8, m2, m5, m1, mc, nl, m1a,
	     m1b, m2a, m2b, m3a, m3b, m4a, m4b;
    static logical ccw;
    static integer knbc, ksbc;
    static logical real__, even;
    static real xcen, ycen;
    static integer nper;
    static logical norm;
    extern /* Subroutine */ int getm1_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, integer *, real *
	    , integer *, char *, integer *, logical *, logical *, logical *, 
	    logical *, ftnlen);
    static real xcen1;
    extern /* Subroutine */ int getm3_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, integer *, real *
	    , integer *, integer *, integer *, integer *, integer *, integer *
	    , integer *, real *, real *, logical *, logical *);
    static real ycen1, xcen2, ycen2, xmid1, ymid1, xmid2, ymid2;
    extern /* Subroutine */ int getm5_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, logical *, logical *);
    static logical halfc;
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
    static logical count;
    static integer ipntr;
    static logical addlnk;
    extern /* Subroutine */ int dataok_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, logical *, logical *);
    static logical pentag;
    static integer idummy[1];
    static char schstr[72];
    static logical triang, trnsit;
    static integer iminus;
    extern /* Subroutine */ int strcut_(char *, ftnlen), ltsort_(integer *, 
	    integer *, integer *, integer *, logical *), strlng_(char *, 
	    integer *, ftnlen);
    static integer lensch, mxtest;
    extern /* Subroutine */ int gettrn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, integer *, real *
	    , integer *, integer *, integer *, integer *, integer *, integer *
	    , integer *, integer *, integer *, real *, real *, real *, real *,
	     real *, real *, real *, real *, logical *, logical *, logical *),
	     mkused_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), chkhol_(
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, real *, integer *, integer *
	    , integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, real *, real *, real *, real *, 
	    integer *, real *, real *, real *, logical *, logical *, logical *
	    );

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___14 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___54 = { 0, 6, 0, fmt_10020, 0 };


/* *********************************************************************** */
/*  SUBROUTINE CHKRGN - CHECK THAT A REGION MAY BE MESHED */
/* *********************************************************************** */
    /* Parameter adjustments */
    --ia;
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
    --rsize;
    linksc -= 3;
    linkr -= 3;
    --ihlist;
    --nhpr;
    --ifhole;
    --islist;
    --ifside;
    --nspr;
    --iregn;
    scheme -= 72;
    --angle;
    --nid;
    --y;
    --x;
    marked -= 4;
    --listl;
    --nnps;
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
    ipntr = 0;
    addlnk = FALSE_;
    count = TRUE_;
    if (*remesh) {
	even = TRUE_;
    } else {
	even = FALSE_;
    }
    real__ = FALSE_;
/*  CHECK TO MAKE SURE CONNECTING DATA FOR THE REGION EXISTS */
/*  AND FILL IN ANY BLANK INTERVALS ACCORDING TO THE GIVEN SIZE */
/*  FOR THE REGION AND THE LINE'S LENGTH */
    dataok_(mp, ml, ms, mr, l, &iregn[*l], &coor[3], &iline[1], &ltype[1], &
	    nint[1], &lcon[4], &nlps[1], &ifline[1], &illist[1], &nspr[1], &
	    ifside[1], &islist[1], &linkp[3], &linkl[3], &links[3], &rsize[*l]
	    , errchk, err);
    if (*err) {
	s_wsfe(&io___6);
	do_fio(&c__1, (char *)&iregn[*l], (ftnlen)sizeof(integer));
	e_wsfe();
	addlnk = TRUE_;
	iminus = -(*l);
	ltsort_(mr, &linkr[3], &iregn[*l], &iminus, &addlnk);
	addlnk = FALSE_;
/*  CALCULATE THE PERIMETER OF THE REGION */
    } else {
	knbc = 0;
	ksbc = 0;
	perim_(mp, ml, ms, &nspr[*l], maxnl, maxnp, &c__1, &c__1, &knbc, &
		ksbc, &iregn[*l], &ipoint[1], &coor[3], &ipboun[1], &iline[1],
		 &ltype[1], &nint[1], &factor[1], &lcon[4], &ilboun[1], &
		isboun[1], &iside[1], &nlps[1], &ifline[1], &illist[1], &
		islist[ifside[*l]], &nppf[1], &ifpb[1], &listpb[3], &nlpf[1], 
		&iflb[1], &listlb[3], &nspf[1], &ifsb[1], &listsb[3], &linkp[
		3], &linkl[3], &links[3], &linkpb[3], &linklb[3], &linksb[3], 
		&x[1], &y[1], &nid[1], &nper, &listl[1], &nl, idummy, &marked[
		4], &even, &real__, err, &ccw, &count, noroom, &amesur[1], &
		xnold[1], &ynold[1], &nxkold[nxkold_offset], &mmpold[4], &
		linkeg[3], &listeg[1], &bmesur[1], mlink, nprold, npnold, 
		npeold, nnxk, remesh, rexmin, rexmax, reymin, reymax, idivis, 
		sizmin, emax, emin);
	if (nper <= 0 || *err) {
	    s_wsfe(&io___14);
	    do_fio(&c__1, (char *)&iregn[*l], (ftnlen)sizeof(integer));
	    e_wsfe();
	    addlnk = TRUE_;
	    iminus = -(*l);
	    ltsort_(mr, &linkr[3], &iregn[*l], &iminus, &addlnk);
	    addlnk = FALSE_;
	    goto L120;
	}
/*  WHEN CHECKING THE MAXIMUMS - ADD ENOUGH FOR ONE MORE INTERVAL */
/*  ON THE LINE AS THIS LINE MAY BE INCREMENTED BY ONE IF THE */
/*  PERIMETER IS ODD */
/* Computing MAX */
	i__1 = *maxnbc, i__2 = knbc + 3;
	*maxnbc = max(i__1,i__2);
/* Computing MAX */
	i__1 = *maxsbc, i__2 = ksbc + 3;
	*maxsbc = max(i__1,i__2);
	*mxnl = max(*mxnl,nl);
/*  GET THE REGION SCHEME */
	i__2 = (i__1 = iregn[*l], abs(i__1));
	ltsort_(mr, &linksc[3], &i__2, &ipntr, &addlnk);
	if (iregn[*l] <= *n24 && ipntr > 0) {
	    s_copy(schstr, scheme + ipntr * 72, (ftnlen)72, (ftnlen)72);
	} else {
	    s_copy(schstr, defsch, (ftnlen)72, (ftnlen)72);
	}
/*  SEE IF A TRIANGULAR, PENTAGON, SEMICIRCLE, OR A TRANSITION */
/*  REGION HAS BEEN FLAGGED */
	pentag = FALSE_;
	trnsit = FALSE_;
	triang = FALSE_;
	strcut_(schstr, (ftnlen)72);
	strlng_(schstr, &lensch, (ftnlen)72);
	i__1 = lensch;
	for (j = 1; j <= i__1; ++j) {
	    if (*(unsigned char *)&schstr[j - 1] == 'T' || *(unsigned char *)&
		    schstr[j - 1] == 't') {
		if (nper >= 6) {
		    triang = TRUE_;
		} else {
		    mesage_("TRIANGULAR REGION MESH NOT", (ftnlen)26);
		    mesage_("POSSIBLE WITH PERIMETER < 6", (ftnlen)27);
		    mesage_("REGULAR PROCESSING ASSUMED", (ftnlen)26);
		}
		goto L110;
	    } else if (*(unsigned char *)&schstr[j - 1] == 'U' || *(unsigned 
		    char *)&schstr[j - 1] == 'u') {
		if (nper >= 10) {
		    pentag = TRUE_;
		} else {
		    mesage_("PENTAGON REGION MESH NOT", (ftnlen)24);
		    mesage_("POSSIBLE WITH PERIMETER < 10", (ftnlen)28);
		    mesage_("REGULAR PROCESSING ASSUMED", (ftnlen)26);
		}
		goto L110;
	    } else if (*(unsigned char *)&schstr[j - 1] == 'B' || *(unsigned 
		    char *)&schstr[j - 1] == 'b') {
		if (nper >= 8) {
		    trnsit = TRUE_;
		    halfc = FALSE_;
		} else {
		    mesage_("TRANSITION REGION GENERATION NOT", (ftnlen)32);
		    mesage_("POSSIBLE WITH NO. IN PERIMETER < 8", (ftnlen)34);
		    mesage_("REGULAR PROCESSING WILL BE ATTEMPTED", (ftnlen)
			    36);
		}
		goto L110;
	    } else if (*(unsigned char *)&schstr[j - 1] == 'C' || *(unsigned 
		    char *)&schstr[j - 1] == 'c') {
		if (nper >= 8) {
		    trnsit = TRUE_;
		    halfc = TRUE_;
		} else {
		    mesage_("SEMICIRCLE REGION GENERATION NOT", (ftnlen)32);
		    mesage_("POSSIBLE WITH NO. IN PERIMETER < 8", (ftnlen)34);
		    mesage_("REGULAR PROCESSING WILL BE ATTEMPTED", (ftnlen)
			    36);
		}
		goto L110;
	    }
/* L100: */
	}
L110:
/*  SET UP THE TRIANGLE DIVISIONS, AND FIND THE CENTER POINT */
	if (triang) {
	    getm3_(ml, ms, max3, &nspr[*l], &islist[ifside[*l]], &nint[1], &
		    ifline[1], &nlps[1], &illist[1], &linkl[3], &links[3], &x[
		    1], &y[1], &nid[1], &nnps[1], &angle[1], &nper, &m1a, &
		    m1b, &m2a, &m2b, &m3a, &m3b, &xcen, &ycen, &ccw, err);
/*  CHECK FOR MAXIMUM DIMENSIONS NEEDED FOR EACH REGION */
/*  ASSUMING THAT 10 NECKLACES WILL BE ADEQUATE */
	    mxtest = (m1a + 1) * (m3b + 1) + (m1b + 1) * (m2a + 1) + (m2b + 1)
		     * (m3a + 1) + (nper + 1) * 10 + (nper << 1);
	    *mxnd = max(mxtest,*mxnd);
/* Computing MAX */
	    i__1 = *mxnper, i__2 = nper + 2 << 1;
	    *mxnper = max(i__1,i__2);
/*  SET UP THE TRANSITION DIVISIONS, AND FIND THE CENTER POINT */
	} else if (trnsit) {
	    gettrn_(ml, ms, max3, &nspr[*l], &islist[ifside[*l]], &nint[1], &
		    ifline[1], &nlps[1], &illist[1], &linkl[3], &links[3], &x[
		    1], &y[1], &nid[1], &nnps[1], &angle[1], &nper, &i1, &i2, 
		    &i3, &i4, &i5, &i6, &i7, &i8, &xcen1, &ycen1, &xcen2, &
		    ycen2, &xmid1, &ymid1, &xmid2, &ymid2, &ccw, &halfc, err);
/*  CHECK FOR MAXIMUM DIMENSIONS NEEDED FOR EACH REGION */
/*  ASSUMING THAT 10 NECKLACES WILL BE ADEQUATE */
	    mxtest = (i2 - i1) * (nper - i8) + (i3 - i2) * (nper - i8) + (i4 
		    - i3) * (i7 - i6) + (i5 - i4) * (i6 - i5) + (i5 - i4) * (
		    i7 - i6) + (nper + 1) * 10 + (nper << 1);
	    *mxnd = max(mxtest,*mxnd);
/* Computing MAX */
	    i__1 = *mxnper, i__2 = nper + 2 << 1;
	    *mxnper = max(i__1,i__2);
/*  SET UP THE PENTAGON DIVISIONS, AND FIND THE CENTER POINT */
	} else if (pentag) {
	    getm5_(&ia[1], ml, ms, max3, &nspr[*l], &islist[ifside[*l]], &
		    nint[1], &ifline[1], &nlps[1], &illist[1], &linkl[3], &
		    links[3], &x[1], &y[1], &nid[1], &nnps[1], &angle[1], &
		    nper, &m1a, &m1b, &m2, &m3a, &m3b, &m4a, &m4b, &m5, &mc, &
		    xcen, &ycen, &ccw, err);
/*  CHECK FOR MAXIMUM DIMENSIONS NEEDED FOR THE REGION */
/*  ASSUMING THAT 10 NECKLACES WILL BE ADEQUATE */
	    mxtest = m1b * m2 + m4a * m3b + m4b * m5 + (nper + 1) * 10;
	    *mxnd = max(mxtest,*mxnd);
/* Computing MAX */
	    i__1 = *mxnper, i__2 = nper + 2 << 1;
	    *mxnper = max(i__1,i__2);
/*  CALCULATE THE BASE OF THE RECTANGLE FOR THE REGION */
	} else {
	    getm1_(ml, ms, max3, &nspr[*l], &islist[ifside[*l]], &nint[1], &
		    ifline[1], &nlps[1], &illist[1], &linkl[3], &links[3], &x[
		    1], &y[1], &nid[1], &nnps[1], &angle[1], &nper, schstr, &
		    m1, &ccw, &norm, &real__, err, (ftnlen)72);
	    if (*err) {
		s_wsfe(&io___54);
		do_fio(&c__1, (char *)&iregn[*l], (ftnlen)sizeof(integer));
		e_wsfe();
		addlnk = TRUE_;
		iminus = -(*l);
		ltsort_(mr, &linkr[3], &iregn[*l], &iminus, &addlnk);
		addlnk = FALSE_;
		goto L120;
	    }
	    m2 = nper / 2 - m1;
/*  CHECK FOR MAXIMUM DIMENSIONS NEEDED FOR EACH REGION */
/*  ASSUMING THAT 10 NECKLACES WILL BE ADEQUATE */
	    mxtest = (m1 + 1) * (m2 + 1) + (m1 + m2 + 2) * 10;
	    *mxnd = max(mxtest,*mxnd);
/* Computing MAX */
	    i__1 = *mxnper, i__2 = nper + 4;
	    *mxnper = max(i__1,i__2);
	}
/*  FLAG THE REGION AS BEING PROCESSABLE */
	iregn[*l] = -iregn[*l];
/*  MARK THE LINES AND POINTS IN THE REGION AS BEING USED */
	mkused_(maxnl, mp, ml, &listl[1], &ipoint[1], &nint[1], &linkp[3], &
		linkl[3], &lcon[4], &nl);
/*  CHECK ALL THE HOLES IN THE REGION */
	chkhol_(&ia[1], l, mp, ml, ms, mr, msc, &ipoint[1], &coor[3], &ipboun[
		1], &iline[1], &ltype[1], &nint[1], &factor[1], &lcon[4], &
		ilboun[1], &isboun[1], &iside[1], &nlps[1], &ifline[1], &
		illist[1], &iregn[1], &nspr[1], &ifside[1], &islist[1], &nppf[
		1], &ifpb[1], &listpb[3], &nlpf[1], &iflb[1], &listlb[3], &
		nspf[1], &ifsb[1], &listsb[3], &ifhole[1], &nhpr[1], &ihlist[
		1], &linkp[3], &linkl[3], &links[3], &linkr[3], &linksc[3], &
		linkpb[3], &linklb[3], &linksb[3], &rsize[1], npregn, npsbc, 
		npnode, maxnp, maxnl, mxnper, &knbc, &ksbc, &x[1], &y[1], &
		nid[1], &listl[1], &marked[4], mxnl, maxnbc, maxsbc, &amesur[
		1], &xnold[1], &ynold[1], &nxkold[nxkold_offset], &mmpold[4], 
		&linkeg[3], &listeg[1], &bmesur[1], mlink, nprold, npnold, 
		npeold, nnxk, remesh, rexmin, rexmax, reymin, reymax, idivis, 
		sizmin, emax, emin, noroom, errchk, err);
    }
L120:
    return 0;
} /* chkrgn_ */

#ifdef __cplusplus
	}
#endif

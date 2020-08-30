/* pschem.f -- translated by f2c (version 20160102).
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
static integer c__3 = 3;
static real c_b74 = (float).5;
static integer c__13 = 13;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* **               SIDE BOUNDARY FLAG WOULD REDIMENSION FOREVER. */
/* **               THE KKSBC VARIABLE WAS CHANGED TO BE SET AT THE */
/* **               BEGINNING OF THE ROUTINE INSTEAD OF RIGHT BEFORE THE */
/* **               ZHOLE PROCESSING WAS STARTED. */
/* Subroutine */ int pschem_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *n, integer *ipoint, real *coor, integer *ipboun, integer 
	*iline, integer *ltype, integer *nint, real *factor, integer *lcon, 
	integer *ilboun, integer *isboun, integer *iside, integer *nlps, 
	integer *ifline, integer *illist, integer *iregn, integer *nspr, 
	integer *ifside, integer *islist, integer *nppf, integer *ifpb, 
	integer *listpb, integer *nlpf, integer *iflb, integer *listlb, 
	integer *nspf, integer *ifsb, integer *listsb, integer *linkp, 
	integer *linkl, integer *links, integer *linkr, integer *linksc, 
	integer *linkpb, integer *linklb, integer *linksb, integer *ifhole, 
	integer *nhpr, integer *ihlist, integer *maxnbc, integer *knbc, 
	integer *maxsbc, integer *ksbc, integer *mxnd, integer *nnn, integer *
	nnnold, integer *kkk, integer *kkkold, integer *lll, real *x, real *y,
	 integer *nid, integer *listl, real *xn, real *yn, integer *nuid, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, integer *
	lstnbc, integer *nxh, integer *nperim, integer *marked, integer *
	iavail, integer *navail, integer *mxnl, integer *mxnper, integer *
	nper, integer *maxprm, integer *nprm, integer *msc, integer *ischm, 
	char *scheme, char *schstr, logical *rect, integer *m1, integer *
	inside, integer *jjhole, integer *kksbc, char *dev1, logical *eight, 
	logical *nine, logical *step, integer *l, integer *nl, integer *mcom, 
	char *cin, integer *iin, real *rin, integer *kin, integer *icom, 
	integer *jcom, real *xmin, real *xmax, real *ymin, real *ymax, 
	integer *icode, logical *noroom, logical *err, real *amesur, real *
	xnold, real *ynold, integer *nxkold, integer *mmpold, integer *linkeg,
	 integer *listeg, real *bmesur, integer *mlink, integer *nprold, 
	integer *npnold, integer *npeold, integer *nnxk, logical *remesh, 
	real *rexmin, real *rexmax, real *reymin, real *reymax, integer *
	idivis, real *sizmin, real *emax, real *emin, ftnlen scheme_len, 
	ftnlen schstr_len, ftnlen dev1_len, ftnlen cin_len)
{
    /* Initialized data */

    static integer iexit = 1;
    static integer iover = 2;
    static integer iquit = 3;

    /* Format strings */
    static char fmt_10010[] = "(\002 ALPHA SMOOTHING PARAMETER FOR EQUAL ARE\
AS SET TO:\002,f6.3)";
    static char fmt_10020[] = "(\002 SCHEME: \002,a,/,\002 FOR REGION:\002,i\
5,/,\002 CANNOT BE SAVED HERE DUE TO DIMENSIONING CONSTRAINTS\002)";
    static char fmt_10030[] = "(\002 RO SMOOTHING PARAMETER FOR RELAXATION S\
ET TO:\002,f6.3)";
    static char fmt_10050[] = "(\002 NO OF SMOOTHING ITERATIONS SET TO:\002,\
i5)";
    static char fmt_10060[] = "(\002 SMOOTHING TOLERANCE SET TO:\002,g14.7\
,/,\002 SMOOTHING EPSILON SET TO:\002,g14.7)";
    static char fmt_10070[] = "(\002 SMALLEST ANGLE OF ELEMENT TO BE DELETED\
 SET TO:\002,f6.3)";
    static char fmt_10040[] = "(\002 WF SMOOTHING PARAMETER FOR ISOPARAMETIC\
 SET TO:\002,f6.3)";
    static char fmt_10000[] = "(\002 \002,/,\002 INITIAL MESH DEFINED USING \
THIS HOLE SCHEME:\002/,\002    \002,a)";
    static char fmt_10080[] = "(\002 ILLEGAL SCHEME COMMAND: \002,a1)";

    /* System generated locals */
    integer nxkold_dim1, nxkold_offset, i__1, i__2;
    real r__1, r__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_cmp(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer j, m, i1, jj;
    static real wf, ro, eps;
    static integer nit;
    static real tol;
    static integer nit3, lena;
    static logical lact[10], done;
    static integer ilpc[10];
    static logical ians;
    static real rone, tilt;
    static integer ltry;
    extern /* Subroutine */ int l2smo_(integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, real *, real *);
    static real alpha;
    static integer nold10, nold24;
    static real rodel;
    extern /* Subroutine */ int casmo_(integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *);
    static real wfdel;
    static integer jhole, limit;
    extern /* Subroutine */ int resta_(integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    logical *, logical *);
    static real atilt;
    extern /* Subroutine */ int mnorm_(integer *, real *, real *, integer *, 
	    integer *, real *);
    static integer irest;
    extern /* Subroutine */ int plotl_(integer *, real *, real *, integer *, 
	    integer *, real *, real *, real *, real *, integer *, char *, 
	    ftnlen), smogs_(integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *);
    static integer ipntr;
    extern /* Subroutine */ int zhole_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, logical *, logical *, real *, real *, real *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, real *, 
	    real *, real *, real *, integer *, real *, real *, real *), 
	    ciapal_(integer *, real *, real *, integer *, integer *, integer *
	    , integer *, integer *, integer *, integer *, real *, real *, 
	    real *);
    static integer nacall;
    static char defsch[72];
    static logical addlnk;
    extern /* Subroutine */ int ncklce_(integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, logical *, 
	    logical *);
    static real arfact;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static char addstr[72], schole[72];
    static logical dolink, active, dosmoo, dotilt;
    static real asmall;
    static integer istype, newsgn, nleftp;
    static real stdlen;
    extern /* Subroutine */ int stripb_(char *, integer *, integer *, ftnlen);
    static integer lensch, iisign;
    extern /* Subroutine */ int squash_(integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, logical *, logical *, 
	    logical *), strlng_(char *, integer *, ftnlen), inschm_(integer *,
	     integer *, integer *, integer *, integer *, char *, integer *, 
	    char *, integer *, char *, logical *, logical *, ftnlen, ftnlen, 
	    ftnlen), repsmo_(integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *), arelax_(integer 
	    *, real *, real *, integer *, integer *, integer *, integer *, 
	    real *), apalsm_(integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, logical *), isolap_(integer *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, real *, real *);
    static integer mxhole;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *), strcut_(char *, ftnlen);
    static integer lenhol;
    extern /* Subroutine */ int intrup_(char *, logical *, integer *, integer 
	    *, integer *, char *, integer *, real *, integer *, ftnlen, 
	    ftnlen), inqstr_(char *, char *, ftnlen, ftnlen), help_fq__(
	    integer *);

    /* Fortran I/O blocks */
    static cilist io___22 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___35 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___37 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___38 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___39 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___41 = { 0, 6, 0, fmt_10070, 0 };
    static cilist io___43 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___50 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___57 = { 0, 6, 0, fmt_10080, 0 };


/* *********************************************************************** */
/*  PSCHEM - PROCESS A COMPLETE SCHEME */
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
    --ihlist;
    --nhpr;
    --ifhole;
    linksc -= 3;
    linkr -= 3;
    --islist;
    --ifside;
    --nspr;
    --iregn;
    --n;
    --lstnbc;
    --nxh;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;
    marked -= 4;
    --listl;
    --y;
    --x;
    --nperim;
    --nid;
    scheme -= 72;
    --ischm;
    --kin;
    --rin;
    --iin;
    cin -= 72;
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
    alpha = (float).7;
    asmall = (float)45.;
    ro = (float)1.;
    tol = (float).03;
    wf = (float).7;
    *icode = 0;
    istype = 1;
    j = 1;
    nacall = 0;
    newsgn = 0;
    dotilt = TRUE_;
    dosmoo = TRUE_;
    *err = FALSE_;
    *noroom = FALSE_;
    *kksbc = *ksbc;
    nleftp = 0;
    nit = *nper * 5 / 2;
    mnorm_(mxnd, &xn[1], &yn[1], &nxl[3], lll, &stdlen);
    eps = stdlen * (float).03;
    stripb_(schstr, &i1, &lensch, (ftnlen)72);
L100:
    iisign = newsgn;
    newsgn = 0;
/*  ACT ON NEXT COMMAND */
/*  A - ALPHA CONTROL FOR APALSM */
    if (*(unsigned char *)&schstr[j - 1] == 'A' || *(unsigned char *)&schstr[
	    j - 1] == 'a') {
	if (iisign >= 0) {
/* Computing MIN */
	    r__1 = alpha + (float).1;
	    alpha = dmin(r__1,(float)1.);
	} else {
/* Computing MAX */
	    r__1 = alpha - (float).1;
	    alpha = dmax(r__1,(float)0.);
	}
	if (istype <= 2) {
	    dosmoo = TRUE_;
	}
	s_wsfe(&io___22);
	do_fio(&c__1, (char *)&alpha, (ftnlen)sizeof(real));
	e_wsfe();
/*  B - BLACKER TRANSITION REGION TEST */
    } else if (*(unsigned char *)&schstr[j - 1] == 'B' || *(unsigned char *)&
	    schstr[j - 1] == 'b') {
/*  C - SEMI-CIRCLE REGION TEST */
    } else if (*(unsigned char *)&schstr[j - 1] == 'C' || *(unsigned char *)&
	    schstr[j - 1] == 'c') {
/*  D - DELETE WORST RHOMBUS */
    } else if (*(unsigned char *)&schstr[j - 1] == 'D' || *(unsigned char *)&
	    schstr[j - 1] == 'd') {
	limit = 0;
	if (dotilt) {
	    resta_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], 
		    &lxn[5], kkk, kkkold, navail, iavail, nnn, &limit, &irest,
		     &tilt, err, noroom);
	    if (*noroom) {
		goto L140;
	    } else if (*err) {
		mesage_("ERROR DURING SHAPE SORTING OF ELEMENTS", (ftnlen)38);
		goto L140;
	    }
	    dotilt = FALSE_;
	}
/* Computing MIN */
	r__1 = (float)45., r__2 = tilt * (float).667;
	atilt = dmin(r__1,r__2);
	atilt = asmall / (float)45. * atilt;
	squash_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &
		lxn[5], kkk, kkkold, nnn, navail, iavail, &atilt, &done, 
		noroom, err);
	if (*noroom) {
	    goto L140;
	} else if (*err) {
	    mesage_("ERROR DURING DELETION OF ELEMENT", (ftnlen)32);
	    goto L140;
	}
	if (done) {
	    dosmoo = TRUE_;
	    *rect = FALSE_;
	    active = TRUE_;
	    mesage_("ELEMENT DELETED", (ftnlen)15);
	} else {
	    mesage_("NO ELEMENT(S) DELETED", (ftnlen)21);
	}
/*  E - EXIT SAVING SCHEME AND REGION */
    } else if (*(unsigned char *)&schstr[j - 1] == 'E' || *(unsigned char *)&
	    schstr[j - 1] == 'e') {
/*  SAVE THE SCHEME USED IF STEPPING THROUGH */
	if (*step) {
	    dolink = TRUE_;
	    nold10 = n[10];
	    nold24 = n[24];
	    jj = (i__1 = iregn[*l], abs(i__1));
	    strlng_(schstr, &ltry, (ftnlen)72);
	    *(unsigned char *)&schstr[ltry - 1] = ' ';
	    inschm_(mr, msc, &n[10], &n[24], &jj, schstr, &ischm[1], scheme + 
		    72, &linksc[3], defsch, noroom, &dolink, (ftnlen)72, (
		    ftnlen)72, (ftnlen)72);
	    if (*noroom) {
		n[10] = nold10;
		n[24] = nold24;
		s_wsfe(&io___35);
		do_fio(&c__1, schstr, lensch);
		i__2 = (i__1 = iregn[*l], abs(i__1));
		do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L140;
	    }
	}
	*icode = iexit;
	goto L140;
/*  F - CONTROL UNDER- OR OVER-RELAXATION */
    } else if (*(unsigned char *)&schstr[j - 1] == 'F' || *(unsigned char *)&
	    schstr[j - 1] == 'f') {
	rodel = (float).25;
	if (iisign >= 0) {
	    ro += rodel;
	} else {
/* Computing MAX */
	    r__1 = ro - rodel;
	    ro = dmax(r__1,rodel);
	}
	dosmoo = TRUE_;
	s_wsfe(&io___37);
	do_fio(&c__1, (char *)&ro, (ftnlen)sizeof(real));
	e_wsfe();
/*  H - INDICATES A HELP MESSAGE RESPONSE IF STEPPING */
    } else if (*(unsigned char *)&schstr[j - 1] == 'H' || *(unsigned char *)&
	    schstr[j - 1] == 'h') {
	if (*step) {
	    help_fq__(&c__3);
	}
/*  I - CHANGE MAX SMOOTHING ITERATIONS */
    } else if (*(unsigned char *)&schstr[j - 1] == 'I' || *(unsigned char *)&
	    schstr[j - 1] == 'i') {
	if (iisign >= 0) {
	    nit = (integer) ((doublereal) nit * (float)1.5 + (float).51);
	} else {
	    nit = (integer) ((doublereal) nit * (float).6667 + (float).51);
	}
	dosmoo = TRUE_;
	s_wsfe(&io___38);
	do_fio(&c__1, (char *)&nit, (ftnlen)sizeof(integer));
	e_wsfe();
/*  J - CONTROL SMOOTHING TOLERANCE */
    } else if (*(unsigned char *)&schstr[j - 1] == 'J' || *(unsigned char *)&
	    schstr[j - 1] == 'j') {
	if (iisign >= 0) {
	    tol *= (float)1.259921;
	    eps *= (float)1.259921;
	} else {
	    tol /= (float)1.259921;
	    eps /= (float)1.259921;
	}
	dosmoo = TRUE_;
	s_wsfe(&io___39);
	do_fio(&c__1, (char *)&tol, (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(real));
	e_wsfe();
/*  L - INSERT ROW OF ELEMENTS AROUND A HOLE (TOO LATE NOW) */
    } else if (*(unsigned char *)&schstr[j - 1] == 'L' || *(unsigned char *)&
	    schstr[j - 1] == 'l') {
/*  M - LOGICAL MESH SIDES CHOSEN BY QMESH (TOO LATE NOW) */
    } else if (*(unsigned char *)&schstr[j - 1] == 'M' || *(unsigned char *)&
	    schstr[j - 1] == 'm') {
/*  N - NECKLACE */
    } else if (*(unsigned char *)&schstr[j - 1] == 'N' || *(unsigned char *)&
	    schstr[j - 1] == 'n') {
	ncklce_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &
		lxn[5], kkk, nnn, nnnold, lll, navail, iavail, &eps, noroom, 
		err);
	if (*noroom) {
	    goto L140;
	} else if (*err) {
	    mesage_("ERROR DURING NECKLACING OF REGION", (ftnlen)33);
	    goto L140;
	}
	active = TRUE_;
	*rect = FALSE_;
	dosmoo = TRUE_;
	mesage_("NECKLACE INSTALLED", (ftnlen)18);
/*  O - ORIGINATE THE MESH AGAIN. */
    } else if (*(unsigned char *)&schstr[j - 1] == 'O' || *(unsigned char *)&
	    schstr[j - 1] == 'o') {
	mesage_("PROCESSING RETURNED TO ORIGINAL", (ftnlen)31);
	s_copy(schstr, " ", (ftnlen)72, (ftnlen)1);
	*icode = iover;
	goto L140;
/*  P - PLOT */
    } else if (*(unsigned char *)&schstr[j - 1] == 'P' || *(unsigned char *)&
	    schstr[j - 1] == 'p') {
	if (*step) {
	    i__2 = (i__1 = iregn[*l], abs(i__1));
	    plotl_(mxnd, &xn[1], &yn[1], &nxl[3], &i__2, xmin, xmax, ymin, 
		    ymax, lll, dev1, (ftnlen)3);
	} else {
	    mesage_("PLOTTING AVAILABLE ONLY IN INTERACTIVE STEP PROCESSING", 
		    (ftnlen)54);
	}
/*  Q - QUIT STEP PROCESSING WITHOUT SAVING MESH */
    } else if (*(unsigned char *)&schstr[j - 1] == 'Q' || *(unsigned char *)&
	    schstr[j - 1] == 'q') {
	mesage_("REGION PROCESSING ABORTED WITH \"QUIT\"", (ftnlen)37);
	*icode = iquit;
	goto L140;
/*  R - RESTRUCTURE */
    } else if (*(unsigned char *)&schstr[j - 1] == 'R' || *(unsigned char *)&
	    schstr[j - 1] == 'r') {
	limit = *mxnd;
	resta_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &
		lxn[5], kkk, kkkold, navail, iavail, nnn, &limit, &irest, &
		tilt, err, noroom);
	if (*noroom) {
	    goto L140;
	} else if (*err) {
	    mesage_("ERROR DURING RESTRUCTURE OF REGION", (ftnlen)34);
	    goto L140;
	}
	dotilt = FALSE_;
	if (irest >= 1) {
	    dosmoo = TRUE_;
	    *rect = FALSE_;
	    active = TRUE_;
	    mesage_("RESTRUCTURE COMPLETED", (ftnlen)21);
	}
/*  S - SMOOTH */
    } else if (*(unsigned char *)&schstr[j - 1] == 'S' || *(unsigned char *)&
	    schstr[j - 1] == 's') {
	if (dosmoo) {
	    if (istype == 1) {
		if (*rect) {
		    repsmo_(mxnd, &xn[1], &yn[1], &lxn[5], nnn, nnnold, &nit, 
			    &eps, &ro, m1);
		    mesage_("EQUIPOTENTIAL SMOOTHING COMPLETED", (ftnlen)33);
		} else {
		    if (nacall / 5 * 5 == nacall) {
			arelax_(mxnd, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[
				3], lll, &arfact);
		    }
		    r__1 = ro * arfact;
		    apalsm_(mxnd, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &
			    lxn[5], nnn, nnnold, &nit, &tol, &r__1, &alpha, 
			    err);
		    if (*err) {
			mesage_("ERROR DURING AREA PULL & LAPLACIAN SMOOTHING"
				, (ftnlen)44);
			goto L140;
		    }
		    mesage_("AREA PULL AND LAPLACIAN SMOOTHING COMPLETED", (
			    ftnlen)43);
		    ++nacall;
		}
	    } else if (istype == 2) {
		if (nacall / 5 * 5 == nacall) {
		    arelax_(mxnd, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], 
			    lll, &arfact);
		}
		r__1 = ro * arfact;
		apalsm_(mxnd, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[
			5], nnn, nnnold, &nit, &tol, &r__1, &alpha, err);
		if (*err) {
		    mesage_("ERROR DURING AREA PULL & LAPLACIAN SMOOTHING", (
			    ftnlen)44);
		    goto L140;
		}
		mesage_("AREA PULL AND LAPLACIAN SMOOTHING COMPLETED", (
			ftnlen)43);
		++nacall;
	    } else if (istype == 3) {
		ciapal_(mxnd, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[
			5], nnn, nnnold, &nit, &eps, &ro, &c_b74);
		mesage_("CENTROID INVERSE PUSH AND LAPLACIAN SMOOTHING COMPL\
ETED", (ftnlen)55);
	    } else if (istype == 4) {
		casmo_(mxnd, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[
			5], nnn, nnnold, &nit, &eps, &ro);
		mesage_("CENTROID AREA PULL SMOOTHING COMPLETED", (ftnlen)38);
	    } else if (istype == 5) {
		if (*rect) {
		    repsmo_(mxnd, &xn[1], &yn[1], &lxn[5], nnn, nnnold, &nit, 
			    &eps, &ro, m1);
		    mesage_("EQUIPOTENTIAL SMOOTHING COMPLETED", (ftnlen)33);
		} else {
		    smogs_(mxnd, &xn[1], &yn[1], &nxl[3], &lxn[5], nnn, 
			    nnnold, &nit, &eps, &ro);
		    mesage_("LAPLACIAN SMOOTHING COMPLETED", (ftnlen)29);
		}
	    } else if (istype == 6) {
		l2smo_(mxnd, &xn[1], &yn[1], &nxl[3], &lxn[5], nnn, nnnold, &
			nit, &eps, &ro);
		mesage_("LENGTH WEIGHTED LAPLACIAN SMOOTHING COMPLETED", (
			ftnlen)45);
	    } else if (istype == 7) {
		isolap_(mxnd, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[
			5], nnn, nnnold, &wf, &nit, &eps, &ro);
		mesage_("LAPLACIAN-ISOPARAMETRIC SMOOTHING COMPLETED", (
			ftnlen)43);
	    }
	    dosmoo = FALSE_;
	    active = TRUE_;
	} else {
	    mesage_("MESH AND/OR SMOOTHING PARAMETERS HAVE", (ftnlen)37);
	    mesage_("NOT CHANGED - NO SMOOTHING ATTEMPTED", (ftnlen)36);
	}
/*  T - TRIANGULAR REGION - QUAD MESH GENERATION */
    } else if (*(unsigned char *)&schstr[j - 1] == 'T' || *(unsigned char *)&
	    schstr[j - 1] == 't') {
/*  U - PENTAGON REGION - QUAD MESH GENERATION */
    } else if (*(unsigned char *)&schstr[j - 1] == 'U' || *(unsigned char *)&
	    schstr[j - 1] == 'u') {
/*  V - CHANGE ASMALL FOR SQUASH (D) */
    } else if (*(unsigned char *)&schstr[j - 1] == 'V' || *(unsigned char *)&
	    schstr[j - 1] == 'v') {
	if (iisign >= 0) {
/* Computing MIN */
	    r__1 = asmall + (float)2.5;
	    asmall = dmin(r__1,(float)80.);
	} else {
/* Computing MAX */
	    r__1 = asmall - (float)2.5;
	    asmall = dmax(r__1,(float)10.);
	}
	s_wsfe(&io___41);
	do_fio(&c__1, (char *)&asmall, (ftnlen)sizeof(real));
	e_wsfe();
/*  W - RESTRUCTURE WORST POSSIBLE ELEMENT ONLY */
    } else if (*(unsigned char *)&schstr[j - 1] == 'W' || *(unsigned char *)&
	    schstr[j - 1] == 'w') {
	limit = 1;
	resta_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &
		lxn[5], kkk, kkkold, navail, iavail, nnn, &limit, &irest, &
		tilt, err, noroom);
	if (*noroom) {
	    goto L140;
	} else if (*err) {
	    mesage_("ERROR DURING WORST ELEMENT RESTRUCTURE", (ftnlen)38);
	    goto L140;
	}
	dotilt = FALSE_;
	if (irest >= 1) {
	    dosmoo = TRUE_;
	    *rect = FALSE_;
	    active = TRUE_;
	    mesage_("WORST ELEMENT RESTRUCTURED", (ftnlen)26);
	}
/*  X - PAVING REGION - QUAD MESH GENERATION */
    } else if (*(unsigned char *)&schstr[j - 1] == 'X' || *(unsigned char *)&
	    schstr[j - 1] == 'x') {
/*  Y - CONTROL UNDER- OR OVER-RELAXATION */
    } else if (*(unsigned char *)&schstr[j - 1] == 'Y' || *(unsigned char *)&
	    schstr[j - 1] == 'y') {
	wfdel = (float).1;
	if (iisign >= 0) {
	    wf += wfdel;
	} else {
/* Computing MAX */
	    r__1 = wf - wfdel;
	    wf = dmax(r__1,wfdel);
	}
	dosmoo = TRUE_;
	s_wsfe(&io___43);
	do_fio(&c__1, (char *)&wf, (ftnlen)sizeof(real));
	e_wsfe();
/*  Z - PROCESS REGION WITH HOLES */
    } else if (*(unsigned char *)&schstr[j - 1] == 'Z' || *(unsigned char *)&
	    schstr[j - 1] == 'z') {
	if (nhpr[*l] == 0) {
	    mesage_("NO HOLES DEFINED IN THIS REGION", (ftnlen)31);
	    goto L130;
	} else if (*jjhole == 0) {
	    *jjhole = ifhole[*l];
	    mxhole = *jjhole + nhpr[*l] - 1;
	} else {
	    ++(*jjhole);
	}
	if (*jjhole > mxhole) {
	    mesage_("ALL HOLES PROCESSED FOR REGION", (ftnlen)30);
	    goto L130;
	}
	addlnk = FALSE_;
	ltsort_(mr, &linkr[3], &ihlist[*jjhole], &jhole, &addlnk);
/*  JHOLE IS NEGATIVE FOR REGIONS ON BODY CARD WITH LESS THAN THREE INTERVALS */
	jhole = abs(jhole);
	i__2 = (i__1 = iregn[jhole], abs(i__1));
	ltsort_(mr, &linksc[3], &i__2, &ipntr, &addlnk);
	if ((i__1 = iregn[jhole], abs(i__1)) <= n[24] && ipntr > 0) {
	    s_copy(schole, scheme + ipntr * 72, (ftnlen)72, (ftnlen)72);
	} else {
	    s_copy(schole, " ", (ftnlen)72, (ftnlen)1);
	}
	if (*step) {
	    strcut_(schole, (ftnlen)72);
	    strlng_(schole, &lenhol, (ftnlen)72);
	    s_wsfe(&io___50);
	    do_fio(&c__1, schole, lenhol);
	    e_wsfe();
	    intrup_("USE CURRENT HOLE SCHEME", &ians, mcom, icom, jcom, cin + 
		    72, &iin[1], &rin[1], &kin[1], (ftnlen)23, (ftnlen)72);
	    if (! ians) {
L110:
		if (*icom <= *jcom) {
		    s_copy(schole, cin + *icom * 72, (ftnlen)72, (ftnlen)72);
		    ++(*icom);
		    ians = TRUE_;
		} else {
		    inqstr_("ENTER HOLE PROCESSING SCHEME: ", schole, (ftnlen)
			    30, (ftnlen)72);
		}
		if (*(unsigned char *)schole == 'H' || *(unsigned char *)
			schole == 'h') {
		    mesage_(" ", (ftnlen)1);
		    help_fq__(&c__13);
		    mesage_(" ", (ftnlen)1);
		    goto L110;
		}
	    }
	}
	strcut_(schole, (ftnlen)72);
	strlng_(schole, &lenhol, (ftnlen)72);
	*inside = 0;
	i__1 = lenhol;
	for (m = 1; m <= i__1; ++m) {
	    if (*(unsigned char *)&schole[m - 1] == 'L' || *(unsigned char *)&
		    schole[m - 1] == 'l') {
		++(*inside);
	    }
/* L120: */
	}
	++(*nprm);
	zhole_(mp, ml, ms, mr, &nspr[jhole], mxnl, mxnper, maxprm, nprm, 
		maxnbc, maxsbc, knbc, ksbc, &iregn[jhole], &ipoint[1], &coor[
		3], &ipboun[1], &iline[1], &ltype[1], &nint[1], &factor[1], &
		lcon[4], &ilboun[1], &isboun[1], &iside[1], &nlps[1], &ifline[
		1], &illist[1], &islist[1], &ifside[jhole], &nppf[1], &ifpb[1]
		, &listpb[3], &nlpf[1], &iflb[1], &listlb[3], &nspf[1], &ifsb[
		1], &listsb[3], &linkp[3], &linkl[3], &links[3], &linkpb[3], &
		linklb[3], &linksb[3], &x[1], &y[1], &nid[1], &listl[1], &
		marked[4], nl, &lstnbc[1], mxnd, &xn[1], &yn[1], &nuid[1], &
		lxk[5], &kxl[3], &nxl[3], &lxn[5], &nxh[1], &nperim[1], nnn, 
		nnnold, kkk, lll, iavail, navail, &jhole, inside, &eps, 
		noroom, err, &amesur[1], &xnold[1], &ynold[1], &nxkold[
		nxkold_offset], &mmpold[4], &linkeg[3], &listeg[1], &bmesur[1]
		, mlink, nprold, npnold, npeold, nnxk, remesh, rexmin, rexmax,
		 reymin, reymax, idivis, sizmin, emax, emin);
	if (*noroom) {
	    goto L140;
	} else if (*err) {
	    mesage_("HOLE PROCESSING FAILED", (ftnlen)22);
	    goto L140;
	} else {
	    dosmoo = TRUE_;
	    *rect = FALSE_;
	    active = TRUE_;
	    if (*inside > 0) {
		nit3 = 3;
		rone = (float)1.;
		smogs_(mxnd, &xn[1], &yn[1], &nxl[3], &lxn[5], nnn, nnnold, &
			nit3, &eps, &rone);
	    }
	    if (ipntr <= 0) {
		addlnk = TRUE_;
		i__2 = (i__1 = iregn[jhole], abs(i__1));
		ltsort_(mr, &linksc[3], &i__2, &ipntr, &addlnk);
		addlnk = FALSE_;
	    }
	    if (ipntr > 0) {
		s_copy(scheme + ipntr * 72, schole, (ftnlen)72, (ftnlen)72);
	    }
	    mesage_("HOLE PROCESSING COMPLETED", (ftnlen)25);
	}
L130:
/*  ( - START LOOP */
	;
    } else if (*(unsigned char *)&schstr[j - 1] == '(') {
	if (nleftp >= 10) {
	    mesage_("TOO MANY NESTED LOOPS IN THE SCHEME", (ftnlen)35);
	    goto L140;
	}
	++nleftp;
	lact[nleftp - 1] = active;
	ilpc[nleftp - 1] = j;
	active = FALSE_;
/*  ) - END OF LOOP - CHECK FOR ACTIVITY */
    } else if (*(unsigned char *)&schstr[j - 1] == ')') {
	if (nleftp <= 0) {
	    mesage_("THERE IS NO LEFT PARENTHESIS TO", (ftnlen)31);
	    mesage_("MATCH THE RIGHT PARENTHESIS", (ftnlen)27);
	    mesage_("\")\" IS THUS IGNORED", (ftnlen)19);
	} else {
/*  LOOP BACK */
	    if (active) {
		active = FALSE_;
		j = ilpc[nleftp - 1];
		lact[nleftp - 1] = TRUE_;
/*  LOOP IS COMPLETED */
	    } else {
		active = lact[nleftp - 1];
		--nleftp;
	    }
	}
/*  + SIGN */
    } else if (*(unsigned char *)&schstr[j - 1] == '+') {
	newsgn = 1;
/*  - SIGN */
    } else if (*(unsigned char *)&schstr[j - 1] == '-') {
	newsgn = -1;
/*  1, 2, ..., 6  SMOOTHING TYPE DECLARATION */
    } else if (*(unsigned char *)&schstr[j - 1] == '1') {
	if (istype != 1) {
	    istype = 1;
	    dosmoo = TRUE_;
	}
	mesage_("SMOOTHING TYPE SET TO \"EQUIPOTENTIAL\"", (ftnlen)37);
    } else if (*(unsigned char *)&schstr[j - 1] == '2') {
	if (istype != 2) {
	    istype = 2;
	    dosmoo = TRUE_;
	}
	mesage_("SMOOTHING TYPE SET TO \"AREA PULL & LAPLACIAN\"", (ftnlen)45)
		;
    } else if (*(unsigned char *)&schstr[j - 1] == '3') {
	if (istype != 3) {
	    istype = 3;
	    dosmoo = TRUE_;
	}
	mesage_("SMOOTHING TYPE SET TO \"CENTROID INVERSE AREA PUSH & LAPLAC\
IAN\"", (ftnlen)62);
    } else if (*(unsigned char *)&schstr[j - 1] == '4') {
	if (istype != 4) {
	    istype = 4;
	    dosmoo = TRUE_;
	}
	mesage_("SMOOTHING TYPE SET TO \"CENTROID AREA PULL\"", (ftnlen)42);
    } else if (*(unsigned char *)&schstr[j - 1] == '5') {
	if (istype != 5) {
	    istype = 5;
	    dosmoo = TRUE_;
	}
	mesage_("SMOOTHING TYPE SET TO \"LAPLACIAN\"", (ftnlen)33);
    } else if (*(unsigned char *)&schstr[j - 1] == '6') {
	if (istype != 6) {
	    istype = 6;
	    dosmoo = TRUE_;
	}
	mesage_("SMOOTHING TYPE SET TO \"LENGTH WEIGHTED LAPLACIAN\"", (
		ftnlen)49);
    } else if (*(unsigned char *)&schstr[j - 1] == '7') {
	if (istype != 7) {
	    istype = 7;
	    dosmoo = TRUE_;
	}
	mesage_("SMOOTHING TYPE SET TO \"LAPLACIAN-ISOPARAMETRIC\"", (ftnlen)
		47);
/*  BLANK SCHEME FLAG */
    } else if (*(unsigned char *)&schstr[j - 1] == ' ') {
	if (j != 1) {
	    mesage_("BLANK SCHEME COMMAND IGNORED", (ftnlen)28);
	}
/*  ILLEGAL SCHEME FLAG */
    } else {
	s_wsfe(&io___57);
	do_fio(&c__1, schstr + (j - 1), (ftnlen)1);
	e_wsfe();
    }
/*  GET NEXT SCHEME COMMAND */
    ++j;
    if (j <= lensch) {
	goto L100;
    } else if (*step) {
	mesage_("------------------------", (ftnlen)24);
	if (*icom <= *jcom) {
	    s_copy(addstr, cin + *icom * 72, (ftnlen)72, (ftnlen)72);
	    ++(*icom);
	} else {
	    inqstr_("FURTHER PROCESSING STEPS: ", addstr, (ftnlen)26, (ftnlen)
		    72);
	}
	strcut_(addstr, (ftnlen)72);
	strlng_(addstr, &lena, (ftnlen)72);
	i__1 = j - 2;
	if (lensch == 1 && s_cmp(schstr + i__1, " ", j - 1 - i__1, (ftnlen)1) 
		== 0) {
	    --lensch;
	}
	if (lensch + lena > 72) {
	    mesage_("ERROR - SCHEME TOO LONG", (ftnlen)23);
	    goto L140;
	}
	i__1 = lensch;
	s_copy(schstr + i__1, addstr, lensch + lena - i__1, lena);
	strcut_(schstr, (ftnlen)72);
	j = lensch + 1;
	strlng_(schstr, &lensch, (ftnlen)72);
	goto L100;
    }
    *icode = iexit;
/*  END OF THIS REGION */
L140:
    return 0;
} /* pschem_ */

#ifdef __cplusplus
	}
#endif

/* pdata.f -- translated by f2c (version 20160102).
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

static integer c__2 = 2;
static real c_b5 = (float)160.;
static real c_b6 = (float)0.;
static integer c__1 = 1;
static integer c__0 = 0;
static logical c_true = TRUE_;
static real c_b41 = (float)1.;
static real c_b43 = (float)5.;
static real c_b48 = (float)7.;
static real c_b69 = (float)2.;
static real c_b72 = (float)3.;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int pdata_(integer *mp, integer *ml, integer *mr, integer *
	msc, integer *ipoint, real *coor, integer *ipboun, integer *iline, 
	integer *ltype, integer *nint, integer *lcon, real *factor, integer *
	ilboun, integer *isboun, integer *iregn, integer *imat, integer *
	linkp, integer *linkl, integer *linkr, integer *linksc, real *rsize, 
	char *scheme, char *defsch, real *defsiz, real *rextrm, integer *n, 
	logical *labp, logical *labl, logical *labr, logical *full, logical *
	labmd, logical *labi, logical *labf, logical *labpb, logical *lablb, 
	logical *labsbd, logical *labsc, logical *labsz, logical *axisd, char 
	*title, real *xmin, real *xmax, real *ymin, real *ymax, real *xx1, 
	real *yy1, real *xx2, real *yy2, char *dev1, char *versn, ftnlen 
	scheme_len, ftnlen defsch_len, ftnlen title_len, ftnlen dev1_len, 
	ftnlen versn_len)
{
    /* Format strings */
    static char fmt_10000[] = "(a)";
    static char fmt_10010[] = "(a2,i10,a1,i10,a1,i10,a1,i10,a1)";
    static char fmt_10020[] = "(a5,i10,a1,i10,a3,a,a1)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1, r__2;
    olist o__1;

    /* Builtin functions */
    integer f_open(olist *), s_wsfe(cilist *), do_fio(integer *, char *, 
	    ftnlen), e_wsfe();
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__;
    static real x1, y1;
    static integer ii, lt;
    static real xr, yr;
    static integer ip1, ip2, ip3;
    static logical add;
    static integer len;
    static real dum1, dum2, dum3, dum4;
    static char date[8];
    static integer mask;
    static char time[8];
    static real xmid, ymid, xlen;
    static integer inum, knum;
    static real xdum[2], ydum[2];
    static logical test;
    static real xrat, yrat;
    extern /* Subroutine */ int mp2pt_(integer *, real *, real *, real *, 
	    real *, integer *), dline_(integer *, integer *, real *, integer *
	    , integer *, integer *, integer *, integer *, integer *, logical *
	    , real *, real *, logical *, logical *, real *, real *, real *, 
	    real *);
    static real xdimd, ydimd, xdimr, ydimr;
    static char dummy[72];
    static integer ipntr;
    extern /* Subroutine */ int pltd2g_(real *, real *, real *, real *), 
	    pltg2d_(real *, real *, real *, real *);
    static integer ipntr1, ipntr2, ipntr3;
    extern /* Subroutine */ int mport2_(real *, real *, real *, real *);
    static logical addlnk;
    extern logical cpuifc_(logical *);
    extern /* Subroutine */ int exdate_(char *, ftnlen);
    static real xbegin;
    extern /* Subroutine */ int pltbel_();
    static logical getmax;
    extern /* Subroutine */ int pltbgn_(), getdum_(integer *, char *, integer 
	    *, ftnlen), extime_(char *, ftnlen);
    static real shrink;
    extern /* Subroutine */ int pltfrm_(integer *), pltxhe_(real *, real *), 
	    pltflu_(), mpview_(real *, real *, real *, real *), setaxs_(real *
	    , real *), strlng_(char *, integer *, ftnlen), pltstd_(integer *, 
	    real *), pltxhl_(char *, real *, ftnlen), gtxdum_(real *, char *, 
	    integer *, ftnlen), pltxth_(real *, real *, char *, ftnlen), 
	    ltsort_(integer *, integer *, integer *, integer *, logical *), 
	    pltstv_(integer *, real *);

    /* Fortran I/O blocks */
    static cilist io___10 = { 0, 12, 0, fmt_10000, 0 };
    static cilist io___11 = { 0, 12, 0, fmt_10010, 0 };
    static cilist io___42 = { 0, 12, 0, fmt_10020, 0 };
    static cilist io___46 = { 0, 12, 0, fmt_10020, 0 };
    static cilist io___47 = { 0, 12, 0, fmt_10020, 0 };


/* *********************************************************************** */
/*  SUBROUTINE PDATA = PLOTS FLAGGED POINTS,  LINES,  AND REGIONS */
/* *********************************************************************** */
/*  INITIALIZE THE PLOTTING SURFACE */
    /* Parameter adjustments */
    linkp -= 3;
    --ipboun;
    coor -= 3;
    --ipoint;
    linkl -= 3;
    --isboun;
    --ilboun;
    --factor;
    lcon -= 4;
    --nint;
    --ltype;
    --iline;
    rextrm -= 5;
    --rsize;
    linksc -= 3;
    linkr -= 3;
    --imat;
    --iregn;
    scheme -= 72;
    --n;

    /* Function Body */
    test = FALSE_;
    getmax = FALSE_;
    if (test) {
	o__1.oerr = 0;
	o__1.ounit = 12;
	o__1.ofnmlen = 10;
	o__1.ofnm = "HP7580.DAT";
	o__1.orl = 0;
	o__1.osta = "NEW";
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
    }
    addlnk = FALSE_;
    pltbgn_();
    pltstv_(&c__2, &c_b5);
    xdimr = (r__1 = *xmax - *xmin, dabs(r__1));
    ydimr = (r__1 = *ymax - *ymin, dabs(r__1));
    xdimd = (float)1.;
    ydimd = (float).75;
    mpview_(&c_b6, &xdimd, &c_b6, &ydimd);
    xrat = xdimr / xdimd;
    yrat = ydimr / ydimd;
    if (xrat < yrat) {
	xdimr = xdimd * yrat;
	*xx1 = (*xmin + *xmax - xdimr) * (float).5;
	*xx2 = (*xmin + *xmax + xdimr) * (float).5;
	xdimr = *xx2 - *xx1;
	*yy1 = *ymin;
	*yy2 = *ymax;
    } else {
	ydimr = ydimd * xrat;
	*yy1 = (*ymin + *ymax - ydimr) * (float).5;
	*yy2 = (*ymin + *ymax + ydimr) * (float).5;
	ydimr = *yy2 - *yy1;
	*xx1 = *xmin;
	*xx2 = *xmax;
    }
/*  SET UP SCALING EXTREMES FOR AXIS */
    if (test) {
	s_wsfe(&io___10);
	do_fio(&c__1, "IN;SP6;;IP - 5710, -10060, 15710, 10060;", (ftnlen)40);
	e_wsfe();
	s_wsfe(&io___11);
	do_fio(&c__1, "SC", (ftnlen)2);
	i__1 = (integer) (*xx1 * 1000);
	do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	do_fio(&c__1, ", ", (ftnlen)2);
	i__2 = (integer) (*yy1 * 1000);
	do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
	do_fio(&c__1, ", ", (ftnlen)2);
	i__3 = (integer) (*xx2 * 1000);
	do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
	do_fio(&c__1, ", ", (ftnlen)2);
	i__4 = (integer) (*yy2 * 1000);
	do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
	do_fio(&c__1, ";", (ftnlen)1);
	e_wsfe();
    }
    if (*axisd) {
	xdum[0] = *xx1 - xdimr * (float).05;
	xdum[1] = *xx2 + xdimr * (float).05;
	ydum[0] = *yy1 - ydimr * (float).05;
	ydum[1] = *yy2 + ydimr * (float).05;
	shrink = (float).2;
    } else {
	shrink = (float).1;
    }
/*  SHRINK TO FIT A BORDER ON THE PLOT */
    *xx1 -= xdimr * shrink;
    *xx2 += xdimr * shrink;
    *yy1 -= ydimr * shrink;
    *yy2 += ydimr * shrink;
    mport2_(xx1, xx2, yy1, yy2);
    pltfrm_(&c__0);
/*  PLOT THE TITLE AND THE TRACE */
    strlng_(title, &len, (ftnlen)72);
    if (len > 1 || *(unsigned char *)title != ' ') {
	pltxhl_(title, &xlen, len);
/* Computing MAX */
	r__1 = (float)0., r__2 = xdimd * (float).5 - xlen * (float).5;
	xbegin = dmax(r__1,r__2);
	r__1 = ydimd * (float).95;
	pltxth_(&xbegin, &r__1, title, len);
    }
    s_copy(dummy, " DRAWN BY ", (ftnlen)10, (ftnlen)10);
    s_copy(dummy + 10, versn, (ftnlen)10, (ftnlen)10);
    s_copy(dummy + 20, "  ", (ftnlen)2, (ftnlen)2);
    exdate_(date, (ftnlen)8);
    s_copy(dummy + 22, date, (ftnlen)8, (ftnlen)8);
    s_copy(dummy + 30, "  ", (ftnlen)2, (ftnlen)2);
    extime_(time, (ftnlen)8);
    s_copy(dummy + 32, time, (ftnlen)8, (ftnlen)8);
    pltxth_(&c_b6, &c_b6, dummy, (ftnlen)40);
/*  DRAW THE AXIS IF REQUIRED,  AND SET CLIPPING WITHIN AXIS */
    if (*axisd) {
	setaxs_(xdum, ydum);
    }
    if (cpuifc_(&c_true)) {
	goto L130;
    }
/*  PLOT THE POINTS FLAGGED */
    if (*labp || *labpb) {
	i__1 = n[18];
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (cpuifc_(&c_true)) {
		goto L130;
	    }
	    ltsort_(mp, &linkp[3], &i__, &ii, &addlnk);
	    if (ii > 0) {
		if (ipoint[ii] < 0) {
		    inum = -ipoint[ii];
		    mp2pt_(&c__1, &coor[(ii << 1) + 1], &coor[(ii << 1) + 2], 
			    &x1, &y1, &mask);
		    if (mask % 2 != 0) {
/*  PLOT THE POINT LABELS */
			if (*labp) {
			    pltstd_(&c__1, &c_b41);
			    getdum_(&inum, dummy, &len, (ftnlen)72);
			    pltxth_(&x1, &y1, dummy, len);
			    pltxhe_(&x1, &y1);
			}
/*  PLOT THE POINBC FLAGS */
			if ((*labpb || *full && *labp) && ipboun[ii] > 0) {
			    pltstd_(&c__1, &c_b43);
			    if (*labp) {
				pltxth_(&x1, &y1, "/", (ftnlen)1);
				pltxhe_(&x1, &y1);
			    }
			    getdum_(&ipboun[ii], dummy, &len, (ftnlen)72);
			    pltxth_(&x1, &y1, dummy, len);
			}
		    }
		}
	    }
/* L100: */
	}
    }
/*  PLOT ALL LINES THAT HAVE BEEN FLAGGED */
    i__1 = n[19];
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (cpuifc_(&c_true)) {
	    goto L130;
	}
	ltsort_(ml, &linkl[3], &i__, &ii, &addlnk);
	if (ii > 0) {
	    if (*labl) {
		add = TRUE_;
	    } else {
		add = FALSE_;
	    }
	    pltstd_(&c__1, &c_b48);
	    if (iline[ii] < 0) {
		if (*labl || *lablb || *labsbd || *labf || *labi) {
		    knum = -iline[ii];
		} else {
		    knum = 0;
		}
		lt = ltype[ii];
		ip1 = lcon[ii * 3 + 1];
		ip2 = lcon[ii * 3 + 2];
		ip3 = lcon[ii * 3 + 3];
		ltsort_(mp, &linkp[3], &ip1, &ipntr1, &addlnk);
		ltsort_(mp, &linkp[3], &ip2, &ipntr2, &addlnk);
		if (ip3 != 0) {
		    i__2 = abs(ip3);
		    ltsort_(mp, &linkp[3], &i__2, &ipntr3, &addlnk);
		} else {
		    ipntr3 = 0;
		}
		if (ipntr1 > 0 && ipntr2 > 0 && (lt == 1 || ipntr3 > 0)) {
		    dline_(mp, ml, &coor[3], &linkp[3], &knum, &lt, &ip1, &
			    ip2, &ip3, labl, &x1, &y1, &test, &getmax, &dum1, 
			    &dum2, &dum3, &dum4);
/*  PLOT INTERVAL NUMBERS */
		    if (*full && *labl || *labi) {
			pltstd_(&c__1, &c_b43);
			if (add) {
			    pltxhe_(&x1, &y1);
			    pltxth_(&x1, &y1, "/", (ftnlen)1);
			    pltxhe_(&x1, &y1);
			}
			getdum_(&nint[ii], dummy, &len, (ftnlen)72);
			if (test) {
			    pltd2g_(&x1, &y1, &xr, &yr);
			    pltg2d_(&x1, &y1, &xr, &yr);
			    s_wsfe(&io___42);
			    do_fio(&c__1, "PU;PA", (ftnlen)5);
			    i__2 = (integer) (xr * (float)1e3);
			    do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, ", ", (ftnlen)2);
			    i__3 = (integer) (yr * (float)1e3);
			    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, ";LB", (ftnlen)3);
			    do_fio(&c__1, dummy, len);
			    do_fio(&c__1, "\003", (ftnlen)1);
			    e_wsfe();
			}
			pltxth_(&x1, &y1, dummy, len);
			add = TRUE_;
		    }
/*  PLOT THE LINE FACTOR */
		    if (*full && *labl || *labf) {
			if (add) {
			    pltstd_(&c__1, &c_b41);
			    pltxhe_(&x1, &y1);
			    pltxth_(&x1, &y1, "/", (ftnlen)1);
			    pltxhe_(&x1, &y1);
			}
			gtxdum_(&factor[ii], dummy, &len, (ftnlen)72);
			pltxth_(&x1, &y1, dummy, len);
			add = TRUE_;
		    }
/*  PLOT THE LINEBC FLAGS */
		    if ((*full && *labl || *lablb) && ilboun[ii] > 0) {
			pltstd_(&c__1, &c_b69);
			if (add) {
			    pltxhe_(&x1, &y1);
			    pltxth_(&x1, &y1, "/", (ftnlen)1);
			    pltxhe_(&x1, &y1);
			}
			getdum_(&ilboun[ii], dummy, &len, (ftnlen)72);
			pltxth_(&x1, &y1, dummy, len);
			add = TRUE_;
		    }
/*  PLOT THE SIDEBC FLAGS */
		    if ((*full && *labl || *labsbd) && isboun[ii] > 0) {
			pltstd_(&c__1, &c_b72);
			if (add) {
			    pltxhe_(&x1, &y1);
			    pltxth_(&x1, &y1, "/", (ftnlen)1);
			    pltxhe_(&x1, &y1);
			}
			getdum_(&isboun[ii], dummy, &len, (ftnlen)72);
			pltxth_(&x1, &y1, dummy, len);
		    }
		}
	    }
	}
/* L110: */
    }
/*  PLOT ALL REGIONS FLAGGED */
    if (*labr || *labmd || *labsc || *labsz) {
	if (cpuifc_(&c_true)) {
	    goto L130;
	}
	i__1 = n[22];
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ltsort_(mr, &linkr[3], &i__, &ii, &addlnk);
	    if (ii > 0) {
		if (iregn[ii] < 0) {
		    add = FALSE_;
		    inum = -iregn[ii];
		    xmid = (rextrm[(ii << 2) + 1] + rextrm[(ii << 2) + 2]) / (
			    float)2.;
		    ymid = (rextrm[(ii << 2) + 3] + rextrm[(ii << 2) + 4]) / (
			    float)2.;
		    mp2pt_(&c__1, &xmid, &ymid, &x1, &y1, &mask);
		    if (mask % 2 != 0) {
/*  PLOT THE REGION NUMBER */
			if (*labr) {
			    pltstd_(&c__1, &c_b69);
			    getdum_(&inum, dummy, &len, (ftnlen)72);
			    pltxth_(&x1, &y1, dummy, len);
			    add = TRUE_;
			}
/*  PLOT OUT THE MATERIAL NUMBER */
			if (*full && *labr || *labmd) {
			    pltstd_(&c__1, &c_b41);
			    if (add) {
				pltxhe_(&x1, &y1);
				pltxth_(&x1, &y1, "/", (ftnlen)1);
				pltxhe_(&x1, &y1);
			    }
			    add = TRUE_;
			    getdum_(&imat[ii], dummy, &len, (ftnlen)72);
			    pltxth_(&x1, &y1, dummy, len);
			}
/*  PLOT OUT THE SIZE NUMBER FOR THE REGION */
			if (*full && *labr || *labsz) {
			    pltstd_(&c__1, &c_b41);
			    if (add) {
				pltxhe_(&x1, &y1);
				pltxth_(&x1, &y1, "/", (ftnlen)1);
				pltxhe_(&x1, &y1);
			    }
			    add = TRUE_;
			    gtxdum_(&rsize[ii], dummy, &len, (ftnlen)72);
			    pltxth_(&x1, &y1, dummy, len);
			}
/*  PLOT OUT THE SCHEME */
			if (*full && *labr || *labsc) {
			    pltstd_(&c__1, &c_b48);
			    if (add) {
				pltxhe_(&x1, &y1);
				pltxth_(&x1, &y1, "/", (ftnlen)1);
				pltxhe_(&x1, &y1);
			    }
			    ltsort_(mr, &linksc[3], &inum, &ipntr, &addlnk);
			    if (inum <= n[24] && ipntr > 0) {
				strlng_(scheme + ipntr * 72, &len, (ftnlen)72)
					;
				if (test) {
				    pltd2g_(&x1, &y1, &xr, &yr);
				    s_wsfe(&io___46);
				    do_fio(&c__1, "PU;PA", (ftnlen)5);
				    i__2 = (integer) (xr * (float)1e3);
				    do_fio(&c__1, (char *)&i__2, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, ", ", (ftnlen)2);
				    i__3 = (integer) (yr * (float)1e3);
				    do_fio(&c__1, (char *)&i__3, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, ";LB", (ftnlen)3);
				    do_fio(&c__1, scheme + ipntr * 72, len);
				    do_fio(&c__1, "\003", (ftnlen)1);
				    e_wsfe();
				}
				pltxth_(&x1, &y1, scheme + ipntr * 72, len);
			    } else {
				strlng_(defsch, &len, (ftnlen)72);
				if (test) {
				    pltd2g_(&x1, &y1, &xr, &yr);
				    s_wsfe(&io___47);
				    do_fio(&c__1, "PU;PA", (ftnlen)5);
				    i__2 = (integer) (xr * (float)1e3);
				    do_fio(&c__1, (char *)&i__2, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, ", ", (ftnlen)2);
				    i__3 = (integer) (yr * (float)1e3);
				    do_fio(&c__1, (char *)&i__3, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, ";LB", (ftnlen)3);
				    do_fio(&c__1, defsch, len);
				    do_fio(&c__1, "\003", (ftnlen)1);
				    e_wsfe();
				}
				pltxth_(&x1, &y1, defsch, len);
			    }
			}
		    }
		}
	    }
/* L120: */
	}
    }
L130:
    pltstd_(&c__1, &c_b48);
    pltbel_();
    pltflu_();
    return 0;
} /* pdata_ */

#ifdef __cplusplus
	}
#endif

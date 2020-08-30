/* rplotl.f -- translated by f2c (version 20160102).
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

static integer c__10002 = 10002;
static integer c__0 = 0;
static real c_b9 = (float)0.;
static integer c__1 = 1;
static integer c__10001 = 10001;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int rplotl_(integer *mxnd, real *xn, real *yn, real *zn, 
	integer *nxl, real *xmin, real *xmax, real *ymin, real *ymax, real *
	zmin, real *zmax, integer *lll, char *dev1, integer *kreg, ftnlen 
	dev1_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 POINT \002,i6,2x,2(1pe14.7,2x))";
    static char fmt_10010[] = "(\002 LINE  \002,i6,2x,\002STR \002,i6,2x,i6)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1, r__2;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer f_open(olist *);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(), 
	    f_clos(cllist *);

    /* Local variables */
    static integer i__;
    static real x[2], y[2], xx1, xx2, yy1, yy2;
    static integer len;
    static logical hard;
    static char hold[72];
    static integer idum;
    static real xrat, yrat, xdum, ydum, xdimd, ydimd, xdimr, ydimr;
    static integer iunit;
    static char dummy[72];
    static integer kaval2;
    extern /* Subroutine */ int mpd2vc_(integer *, real *, real *, real *, 
	    real *), mport2_(real *, real *, real *, real *);
    static logical figure;
    extern /* Subroutine */ int vdescp_(integer *, integer *, integer *), 
	    getdum_(integer *, char *, integer *, ftnlen), pltbgn_(), vdiqes_(
	    integer *, integer *), pltfrm_(integer *), pltflu_(), mpview_(
	    real *, real *, real *, real *), pltxth_(real *, real *, char *, 
	    ftnlen);

    /* Fortran I/O blocks */
    static cilist io___24 = { 0, 0, 0, fmt_10000, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_10000, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE RPLOTL = REPLOTS THE CURRENT MESH FROM THE NXL ARRAY */
/* *********************************************************************** */
    /* Parameter adjustments */
    nxl -= 3;
    --zn;
    --yn;
    --xn;

    /* Function Body */
    hard = FALSE_;
    figure = FALSE_;
/*  INITIALIZE THE PLOTTING SURFACE */
    xdimd = (float)1.;
    ydimd = (float).75;
/*  TURN ON THE HARDCOPY IF NEEDED */
    if (hard) {
	vdiqes_(&c__10002, &kaval2);
	if (kaval2 != 1) {
	    goto L110;
	}
	vdescp_(&c__10002, &c__0, &c__0);
    }
/*  OPEN A FIGURE FILE IF NEEDED */
    if (figure) {
	iunit = 98;
	o__1.oerr = 1;
	o__1.ounit = iunit;
	o__1.ofnmlen = 8;
	o__1.ofnm = "DATA.FIG";
	o__1.orl = 0;
	o__1.osta = "NEW";
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	i__1 = f_open(&o__1);
	if (i__1 != 0) {
	    goto L110;
	}
    }
    pltbgn_();
    xdimr = *xmax - *xmin;
    ydimr = *ymax - *ymin;
    mpview_(&c_b9, &xdimd, &c_b9, &ydimd);
    xrat = xdimr / xdimd;
    yrat = ydimr / ydimd;
    if (xrat < yrat) {
	xdimr = xdimd * yrat;
	xx1 = (*xmin + *xmax - xdimr) * (float).5;
	xx2 = (*xmin + *xmax + xdimr) * (float).5;
	xdimr = xx2 - xx1;
	yy1 = *ymin;
	yy2 = *ymax;
    } else {
	ydimr = ydimd * xrat;
	yy1 = (*ymin + *ymax - ydimr) * (float).5;
	yy2 = (*ymin + *ymax + ydimr) * (float).5;
	ydimr = yy2 - yy1;
	xx1 = *xmin;
	xx2 = *xmax;
    }
    xx1 -= xdimr * (float).1;
    xx2 += xdimr * (float).1;
    yy1 -= ydimr * (float).1;
    yy2 += ydimr * (float).1;
    mport2_(&xx1, &xx2, &yy1, &yy2);
    pltfrm_(&c__0);
    getdum_(kreg, hold, &len, (ftnlen)72);
    s_copy(dummy, " ", (ftnlen)72, (ftnlen)1);
    s_copy(dummy + 7, hold, len, len);
    s_copy(dummy, "REGION ", (ftnlen)7, (ftnlen)7);
    len += 7;
    r__1 = xdimd * (float).05;
    r__2 = ydimd * (float).95;
    pltxth_(&r__1, &r__2, dummy, len);
/*  PLOT THE LINES IN NXL ARRAY,  SKIPPING DELETIONS */
    if (figure) {
	idum = 0;
	xdum = (float)0.;
	ydum = (float)0.;
    }
    i__1 = *lll;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nxl[(i__ << 1) + 1] > 0) {
	    x[1] = xn[nxl[(i__ << 1) + 2]];
	    y[1] = yn[nxl[(i__ << 1) + 2]];
	    x[0] = xn[nxl[(i__ << 1) + 1]];
	    y[0] = yn[nxl[(i__ << 1) + 1]];
	    mpd2vc_(&c__1, x, y, &x[1], &y[1]);
	    pltflu_();
	    if (figure && (x[0] < xx2 && x[0] > xx1 && y[0] < yy2 && y[0] > 
		    yy1 || x[1] < xx2 && x[1] > xx1 && y[1] < yy2 && y[1] > 
		    yy1)) {
		io___24.ciunit = iunit;
		s_wsfe(&io___24);
		i__2 = nxl[(i__ << 1) + 1] + idum;
		do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
		r__1 = x[0] + xdum;
		do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
		r__2 = y[0] + ydum;
		do_fio(&c__1, (char *)&r__2, (ftnlen)sizeof(real));
		e_wsfe();
		io___25.ciunit = iunit;
		s_wsfe(&io___25);
		i__2 = nxl[(i__ << 1) + 2] + idum;
		do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
		r__1 = x[1] + xdum;
		do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
		r__2 = y[1] + ydum;
		do_fio(&c__1, (char *)&r__2, (ftnlen)sizeof(real));
		e_wsfe();
		io___26.ciunit = iunit;
		s_wsfe(&io___26);
		i__2 = i__ + idum;
		do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
		i__3 = nxl[(i__ << 1) + 1] + idum;
		do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		i__4 = nxl[(i__ << 1) + 2] + idum;
		do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
/* L100: */
    }
    pltflu_();
    if (hard) {
	pltflu_();
	vdescp_(&c__10001, &c__0, &c__0);
    }
L110:
    if (figure) {
	cl__1.cerr = 0;
	cl__1.cunit = iunit;
	cl__1.csta = 0;
	f_clos(&cl__1);
    }
    return 0;
} /* rplotl_ */

#ifdef __cplusplus
	}
#endif

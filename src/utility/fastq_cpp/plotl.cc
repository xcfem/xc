/* plotl.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int plotl_(integer *mxnd, real *xn, real *yn, integer *nxl, 
	integer *kreg, real *xmin, real *xmax, real *ymin, real *ymax, 
	integer *lll, char *dev1, ftnlen dev1_len)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__;
    static real x[2], y[2], xx1, xx2, yy1, yy2;
    static integer len;
    static char hold[72];
    static real xrat, yrat, xdimd, ydimd, xdimr, ydimr;
    static char dummy[72];
    extern /* Subroutine */ int mpd2vc_(integer *, real *, real *, real *, 
	    real *), mport2_(real *, real *, real *, real *), getdum_(integer 
	    *, char *, integer *, ftnlen), pltbgn_(), pltfrm_(integer *), 
	    pltflu_(), mpview_(real *, real *, real *, real *), pltxth_(real *
	    , real *, char *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE PLTNXL = PLOTS THE CURRENT MESH FROM THE NXL ARRAY */
/* *********************************************************************** */
/*  INITIALIZE THE PLOTTING SURFACE */
    /* Parameter adjustments */
    nxl -= 3;
    --yn;
    --xn;

    /* Function Body */
    pltbgn_();
    xdimr = *xmax - *xmin;
    ydimr = *ymax - *ymin;
    xdimd = (float)1.;
    ydimd = (float).75;
    mpview_(&c_b2, &xdimd, &c_b2, &ydimd);
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
    i__1 = *lll;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nxl[(i__ << 1) + 1] > 0) {
	    x[1] = xn[nxl[(i__ << 1) + 2]];
	    y[1] = yn[nxl[(i__ << 1) + 2]];
	    x[0] = xn[nxl[(i__ << 1) + 1]];
	    y[0] = yn[nxl[(i__ << 1) + 1]];
	    mpd2vc_(&c__1, x, y, &x[1], &y[1]);
	}
/* L100: */
    }
    pltflu_();
    return 0;
} /* plotl_ */

#ifdef __cplusplus
	}
#endif

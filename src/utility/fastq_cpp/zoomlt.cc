/* zoomlt.f -- translated by f2c (version 20160102).
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

static real c_b9 = (float)0.;
static real c_b10 = (float)1.;
static real c_b12 = (float).75;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int zoomlt_(integer *mcom, integer *icom, integer *jcom, 
	char *cin, real *rin, integer *iin, integer *kin, integer *idump, 
	logical *drawn, logical *alpha, char *dev1, real *x1, real *x2, real *
	y1, real *y2, real *xx1, real *xx2, real *yy1, real *yy2, real *xmin1,
	 real *xmax1, real *ymin1, real *ymax1, real *xmin, real *xmax, real *
	ymin, real *ymax, ftnlen cin_len, ftnlen dev1_len)
{
    /* System generated locals */
    real r__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static char ans[3];
    static real xtest1, ytest1, xtest2, ytest2;
    extern /* Subroutine */ int pltflu_(), pltcrs_(
	    real *, real *, char *, ftnlen), pltdrw_(real *, real *), pltmov_(
	    real *, real *);

/* *********************************************************************** */
/*  ZOOMPL = SUBROUTINE TO INPUT NEW ZOOM LIMITS */
/* *********************************************************************** */
    /* Parameter adjustments */
    --kin;
    --iin;
    --rin;
    cin -= 72;

    /* Function Body */
    if (*icom <= *jcom && *drawn && (*(unsigned char *)&cin[*icom * 72] == 
	    'C' || *(unsigned char *)&cin[*icom * 72] == 'c') && ! (*alpha)) {
	s_copy(cin + *icom * 72, "PLOT", (ftnlen)72, (ftnlen)4);
/*  USE CURSOR INPUT FROM THE SCREEN */
	mesage_(" ", (ftnlen)1);
	mesage_(" ", (ftnlen)1);
	mesage_("LOCATE ONE CORNER WITH CURSOR", (ftnlen)29);
	mesage_("THEN HIT ANY KEY", (ftnlen)16);
/*         X1 = .45 */
/*         Y1 = .325 */
	pltcrs_(&xtest1, &ytest1, ans, (ftnlen)3);
/*         XDRAW = ABS ( (XTEST1 * (XX2 - XX1))) + XX1 */
/*         YDRAW = ABS ( ((YTEST1 / .75) * (YY2 - YY1)) ) + YY1 */
	pltmov_(&c_b9, &ytest1);
	pltdrw_(&c_b10, &ytest1);
	pltmov_(&xtest1, &c_b9);
	pltdrw_(&xtest1, &c_b12);
	pltflu_();
/*         X2 = MAX( X1+.05, .55) */
/*         Y2 = MAX( Y1+.05, .425) */
	pltcrs_(&xtest2, &ytest2, ans, (ftnlen)3);
	pltmov_(&c_b9, &ytest2);
	pltdrw_(&c_b10, &ytest2);
	pltmov_(&xtest2, &c_b9);
	pltdrw_(&xtest2, &c_b12);
	mesage_("LOCATE THE OTHER CORNER WITH CURSOR", (ftnlen)35);
	mesage_("THEN HIT ANY KEY", (ftnlen)16);
	pltflu_();
	*x1 = dmin(xtest1,xtest2);
	*x2 = dmax(xtest1,xtest2);
	*y1 = dmin(ytest1,ytest2);
	*y2 = dmax(ytest1,ytest2);
	*xmin = (r__1 = *x1 * (*xx2 - *xx1), dabs(r__1)) + *xx1;
	*xmax = (r__1 = *x2 * (*xx2 - *xx1), dabs(r__1)) + *xx1;
	*ymin = (r__1 = *y1 / (float).75 * (*yy2 - *yy1), dabs(r__1)) + *yy1;
	*ymax = (r__1 = *y2 / (float).75 * (*yy2 - *yy1), dabs(r__1)) + *yy1;
/*  USE USER INPUT FROM THE KEYPAD */
    } else {
	if (*(unsigned char *)&cin[*icom * 72] == 'C' || *(unsigned char *)&
		cin[*icom * 72] == 'c') {
	    ++(*icom);
	    mesage_(" ", (ftnlen)1);
	    mesage_("NO CURRENT PLOT FOR CURSOR ZOOM", (ftnlen)31);
	    mesage_("CURRENT PLOT LIMITS UNCHANGED", (ftnlen)29);
	    mesage_("* IN OTHER WORDS ... PLOT FIRST (P) AND THEN ZOOM (Z,C)\
 *", (ftnlen)57);
/*  SEE IF ANY OF THE VALUES ARE REDEFINED */
	} else if (*icom <= *jcom && (kin[*icom] > 0 || kin[*icom + 1] > 0 || 
		kin[*icom + 2] > 0 || kin[*icom + 3] > 0)) {
	    if (kin[*icom] > 0) {
		*xmin = rin[*icom];
	    }
	    ++(*icom);
	    if (*icom <= *jcom) {
		if (kin[*icom] > 0) {
		    *xmax = rin[*icom];
		}
		++(*icom);
		if (*icom <= *jcom) {
		    if (kin[*icom] > 0) {
			*ymin = rin[*icom];
		    }
		    ++(*icom);
		    if (*icom <= *jcom) {
			if (kin[*icom] > 0) {
			    *ymax = rin[*icom];
			}
			++(*icom);
		    }
		}
	    }
	} else {
	    *xmin = *xmin1;
	    *ymin = *ymin1;
	    *xmax = *xmax1;
	    *ymax = *ymax1;
	    mesage_(" ", (ftnlen)1);
	    mesage_("ZOOM LIMITS RESET TO PLOT EXTREMES", (ftnlen)34);
	}
    }
    return 0;
} /* zoomlt_ */

#ifdef __cplusplus
	}
#endif

/* setaxs.f -- translated by f2c (version 20160102).
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
static integer c__2 = 2;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__37 = 37;
static doublereal c_b9 = 10.;
static integer c__11 = 11;
static integer c_n2 = -2;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int setaxs_(real *x, real *y)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real x01, y01, x02, y02, buff[11];
    static integer mask;
    static real xend, yend, xint, yint;
    extern /* Subroutine */ int mp2pt_(integer *, real *, real *, real *, 
	    real *, integer *);
    static integer ixtic, iytic, ixexp, iyexp;
    extern /* Subroutine */ int mport2_(real *, real *, real *, real *), 
	    pltgph_(real *, real *, integer *, char *, char *, char *, char *,
	     ftnlen, ftnlen, ftnlen, ftnlen), pltini_(real *, real *, real *, 
	    real *, real *, integer *, integer *), mpview_(real *, real *, 
	    real *, real *), pltstg_(integer *, real *);
    static real xstart, ystart;

/* *********************************************************************** */
/*  SETAXS = SETS UP THE AXIS AS NEEDED FOR PLOTTING */
/* *********************************************************************** */
/*  GET THE AXIS ON THE CORRECT LOCATION ON THE GRAPH */
    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    mp2pt_(&c__1, &x[1], &y[1], &x01, &y01, &mask);
    mp2pt_(&c__1, &x[2], &y[2], &x02, &y02, &mask);
/*  FORCE X AND Y LIMITS ON THE GRAPH */
    buff[0] = x01;
    pltstg_(&c__1, buff);
    buff[0] = y01;
    pltstg_(&c__2, buff);
    buff[0] = x02 - x01;
    pltstg_(&c__3, buff);
    buff[0] = y02 - y01;
    pltstg_(&c__4, buff);
/*  TURN OFF THE ZERO LINE PLOT */
    buff[0] = (float)0.;
    pltstg_(&c__37, buff);
/*  GET NICE INTERVALS ON THE AXIS */
    pltini_(&x[1], &x[2], &xstart, &xend, &xint, &ixexp, &ixtic);
    pltini_(&y[1], &y[2], &ystart, &yend, &yint, &iyexp, &iytic);
/*  SET ALL THE BUFFER PARAMETERS */
    buff[0] = (float)4.;
    buff[1] = x[1];
    if (ixexp == 0) {
	buff[2] = xstart;
	buff[4] = xint;
    } else {
	d__1 = (doublereal) ixexp;
	buff[2] = xstart * pow_dd(&c_b9, &d__1);
	d__1 = (doublereal) ixexp;
	buff[4] = xint * pow_dd(&c_b9, &d__1);
    }
    buff[3] = x[2];
    buff[5] = (float)1.;
    buff[6] = y[1];
    if (iyexp == 0) {
	buff[7] = ystart;
	buff[9] = yint;
    } else {
	d__1 = (doublereal) iyexp;
	buff[7] = ystart * pow_dd(&c_b9, &d__1);
	d__1 = (doublereal) iyexp;
	buff[9] = yint * pow_dd(&c_b9, &d__1);
    }
    buff[8] = y[2];
    buff[10] = (float)1.;
/*  FORCE THE CORRECT AXIS SETUP */
    pltstg_(&c__11, buff);
/*  PLOT THE AXIS */
    pltgph_(&x[1], &y[1], &c_n2, "X", " ", "Y", " ", (ftnlen)1, (ftnlen)1, (
	    ftnlen)1, (ftnlen)1);
/*  PUT THE CLIPPING RECTANGLE RIGHT AT THE AXIS */
    mp2pt_(&c__1, &x[1], &y[1], &x01, &y01, &mask);
    mp2pt_(&c__1, &x[2], &y[2], &x02, &y02, &mask);
    mpview_(&x01, &x02, &y01, &y02);
    mport2_(&x[1], &x[2], &y[1], &y[2]);
    return 0;
} /* setaxs_ */

#ifdef __cplusplus
	}
#endif

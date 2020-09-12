/* pltrev.f -- translated by f2c (version 20160102).
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
//#include "f2c.h"
#include "paving.h"

/* Common Block Declarations */

struct {
    real devcap[23], defout[7];
} status_;

#define status_1 status_

struct {
    real devp[5];
} device_;

#define device_1 device_

struct {
    real colp[3], palett[48]	/* was [3][16] */;
} color_;

#define color_1 color_

struct {
    real textp[40];
} text_;

#define text_1 text_

struct {
    real vectp[5], xcur, ycur;
} vectrc_;

#define vectrc_1 vectrc_

struct {
    integer idex[400]	/* was [200][2] */, nvect[400]	/* was [200][2] */;
    real xsize[400]	/* was [200][2] */, ysize[400]	/* was [200][2] */, 
	    x0[4600]	/* was [2300][2] */, y0[4600]	/* was [2300][2] */, 
	    x1[4600]	/* was [2300][2] */, y1[4600]	/* was [2300][2] */;
} font_;

#define font_1 font_

struct {
    real graphp[100];
} graph_;

#define graph_1 graph_

struct {
    real mapp[11];
} mappar_;

#define mappar_1 mappar_

struct {
    integer memory[1000];
} storag_;

#define storag_1 storag_

struct {
    real tdevp[50]	/* was [5][10] */, ttextp[400]	/* was [40][10] */, 
	    tvectp[50]	/* was [5][10] */, tgraph[1000]	/* was [100][10] */, 
	    tmapp[110]	/* was [11][10] */;
    integer ipopd, ipopt, ipopv, ipopg, ipopm;
} psave_;

#define psave_1 psave_

/* Table of constant values */

static integer c__1 = 1;
static integer c__2 = 2;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltrev_()
{
    static integer i__;
    extern /* Subroutine */ int pltstv_(integer *, real *);

    if (vectrc_1.vectp[0] != psave_1.tvectp[psave_1.ipopv * 5 - 5]) {
	pltstv_(&c__1, &psave_1.tvectp[psave_1.ipopv * 5 - 5]);
    }
    if (vectrc_1.vectp[1] != psave_1.tvectp[psave_1.ipopv * 5 - 4]) {
	pltstv_(&c__2, &psave_1.tvectp[psave_1.ipopv * 5 - 4]);
    }
    for (i__ = 1; i__ <= 5; ++i__) {
	vectrc_1.vectp[i__ - 1] = psave_1.tvectp[i__ + psave_1.ipopv * 5 - 6];
/* L2200: */
    }
    if (psave_1.ipopv != 1) {
	--psave_1.ipopv;
    }
    return 0;
} /* pltrev_ */

#ifdef __cplusplus
	}
#endif

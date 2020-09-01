/* pltrst.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

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

/* Table of constant values */

static integer c__12 = 12;
static real c_b3 = (float)2.;
static real c_b5 = (float)3.;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltrst_()
{
    static integer i__;
    static real buff;
    extern /* Subroutine */ int vdstcs_(real *), pltitm_(), pltstt_(integer *,
	     real *);

    if ((status_1.devcap[14] + status_1.devcap[15]) / (float)2. <= (float)
	    800.) {
	pltstt_(&c__12, &c_b3);
    } else {
	pltstt_(&c__12, &c_b5);
    }
    buff = (float).017241379310344827;
    vdstcs_(&buff);
    text_1.textp[34] = status_1.defout[5];
    text_1.textp[35] = status_1.defout[6];
    text_1.textp[0] = (float).015;
    text_1.textp[1] = (float)0.;
    text_1.textp[2] = (float)0.;
    for (i__ = 4; i__ <= 11; ++i__) {
	text_1.textp[i__ - 1] = (float)0.;
/* L2340: */
    }
    text_1.textp[19] = (float)0.;
    text_1.textp[20] = (float)0.;
    text_1.textp[21] = (float)1.;
    text_1.textp[22] = (float)0.;
    text_1.textp[23] = text_1.textp[21];
    text_1.textp[24] = (float).75;
    text_1.textp[25] = (float)0.;
    text_1.textp[26] = text_1.textp[24];
    text_1.textp[29] = (float)1.5;
    text_1.textp[30] = (float)1.;
    text_1.textp[31] = (float).7;
    text_1.textp[32] = (float).8;
    text_1.textp[33] = (float).5;
    text_1.textp[36] = (float)160.;
    pltitm_();
    return 0;
} /* pltrst_ */

#ifdef __cplusplus
	}
#endif

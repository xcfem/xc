/* pltitm.f -- translated by f2c (version 20160102).
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

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltitm_()
{
    /* Initialized data */

    static real dpr = (float)57.29577951;

    /* System generated locals */
    real r__1;

    /* Builtin functions */
    double cos(doublereal), sin(doublereal);

    r__1 = text_1.textp[1] + text_1.textp[2];
    text_1.textp[11] = cos(r__1 / dpr);
    r__1 = text_1.textp[1] + text_1.textp[2];
    text_1.textp[12] = sin(r__1 / dpr);
    text_1.textp[13] = text_1.textp[11] * text_1.textp[0];
    text_1.textp[14] = text_1.textp[12] * text_1.textp[0];
    text_1.textp[15] = -text_1.textp[12] * text_1.textp[0];
    text_1.textp[16] = text_1.textp[11] * text_1.textp[0];
    text_1.textp[27] = cos(text_1.textp[1] / dpr);
    text_1.textp[28] = sin(text_1.textp[1] / dpr);
    return 0;
} /* pltitm_ */

#ifdef __cplusplus
	}
#endif

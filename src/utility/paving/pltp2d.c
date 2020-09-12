/* pltp2d.f -- translated by f2c (version 20160102).
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
    xc_float devcap[23], defout[7];
} status_;

#define status_1 status_

struct {
    xc_float devp[5];
} device_;

#define device_1 device_

struct {
    xc_float colp[3], palett[48]	/* was [3][16] */;
} color_;

#define color_1 color_

struct {
    xc_float textp[40];
} text_;

#define text_1 text_

struct {
    xc_float vectp[5], xcur, ycur;
} vectrc_;

#define vectrc_1 vectrc_

struct {
    integer idex[400]	/* was [200][2] */, nvect[400]	/* was [200][2] */;
    xc_float xsize[400]	/* was [200][2] */, ysize[400]	/* was [200][2] */, 
	    x0[4600]	/* was [2300][2] */, y0[4600]	/* was [2300][2] */, 
	    x1[4600]	/* was [2300][2] */, y1[4600]	/* was [2300][2] */;
} font_;

#define font_1 font_

struct {
    xc_float graphp[100];
} graph_;

#define graph_1 graph_

struct {
    xc_float mapp[11];
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
/* Subroutine */ int pltp2d_(xc_float *x, xc_float *y, xc_float *xn, xc_float *yn)
{
    /* Initialized data */

    static xc_float dtr = (xc_float).01745329;

    /* Builtin functions */
    

    /* Local variables */
    static xc_float sx, xt, yt, sy, theta;

    xt = *x - device_1.devp[3] / (xc_float)2.;
    yt = *y - device_1.devp[4] / (xc_float)2.;
    theta = mappar_1.mapp[4] * dtr;
    xt *= mappar_1.mapp[0];
    yt *= mappar_1.mapp[1];
    *xn = xt * cos(theta) - yt * sin(theta) + device_1.devp[3] / (xc_float)2. + 
	    mappar_1.mapp[2];
    *yn = yt * cos(theta) + xt * sin(theta) + device_1.devp[4] / (xc_float)2. + 
	    mappar_1.mapp[3];
    sx = (mappar_1.mapp[7] - mappar_1.mapp[5]) / device_1.devp[3];
    sy = (mappar_1.mapp[8] - mappar_1.mapp[6]) / device_1.devp[4];
    *xn = *xn * sx + mappar_1.mapp[5];
    *yn = *yn * sy + mappar_1.mapp[6];
    return 0;
} /* pltp2d_ */

#ifdef __cplusplus
	}
#endif

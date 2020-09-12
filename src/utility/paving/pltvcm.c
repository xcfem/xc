/* pltvcm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int pltvcm_(integer *n, integer *mask, real *xx0, real *yy0, 
	real *xx1, real *yy1)
{
    /* Initialized data */

    static integer izbit[32] = { 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,
	    8192,16384,32768,65536,131072,262144,524288,1048576,2097152,
	    4194304,8388608,16777216,33554432,67108864,134217728,268435456,
	    536870912,1073741824,2147483648 };

    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer j, k, m, j1, jb, km, jn;
    extern /* Subroutine */ int pltdrw_(real *, real *), pltmov_(real *, real 
	    *);

    /* Parameter adjustments */
    --yy1;
    --xx1;
    --yy0;
    --xx0;
    --mask;

    /* Function Body */
    if (vectrc_1.vectp[0] <= (float)0. || vectrc_1.vectp[1] <= (float)0.) {
	vectrc_1.xcur = xx1[*n];
	vectrc_1.ycur = yy1[*n];
	return 0;
    }
    j = 0;
    km = 0;
L2180:
    if (! (j < *n)) {
	goto L2190;
    }
/* Computing MIN */
    i__1 = *n - j;
    jn = int_min(i__1,32);
    ++km;
    j1 = j;
    j += jn;
    m = mask[km];
    if (m == 0) {
	goto L2180;
    }
    i__1 = jn;
    for (k = 1; k <= i__1; ++k) {
	jb = izbit[k - 1];
	if ((m & jb) == 0) {
	    goto L2200;
	}
	if (xx0[k + j1] != vectrc_1.xcur || yy0[k + j1] != vectrc_1.ycur) {
	    pltmov_(&xx0[k + j1], &yy0[k + j1]);
	}
	pltdrw_(&xx1[k + j1], &yy1[k + j1]);
	vectrc_1.xcur = xx1[k + j1];
	vectrc_1.ycur = yy1[k + j1];
L2200:
	;
    }
    goto L2180;
L2190:
    return 0;
} /* pltvcm_ */

#ifdef __cplusplus
	}
#endif

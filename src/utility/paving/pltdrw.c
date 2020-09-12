/* pltdrw.f -- translated by f2c (version 20160102).
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
    real savlen;
    integer idshsv;
} pltsty_;

#define pltsty_1 pltsty_

/* Table of constant values */

static integer c__0 = 0;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltdrw_(real *x, real *y)
{
    /* Initialized data */

    static integer istyle[40]	/* was [8][5] */ = { 1,0,1,0,1,0,1,0,1,1,1,1,
	    1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,0,0,1,1,1,1,0,0,0,0 };

    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);
    integer i_nint(real *);

    /* Local variables */
    static integer i__;
    static real x2, y2, dx, dy, xx, yy, dshdx, dshdy, dshlen, linlen, dshdrn;
    static integer idshno;
    extern /* Subroutine */ int pltlig_(real *, real *);
    static real drwlen;
    static integer numdsh;
    static real vector[7];
    extern /* Subroutine */ int vdiqos_(real *);
    static real prvdrn;
    static integer nstyle;
    extern /* Subroutine */ int vdstls_(integer *), pltmov_(real *, real *);

    if (vectrc_1.vectp[0] == (float)0.) {
	vectrc_1.xcur = *x;
	vectrc_1.ycur = *y;
	return 0;
    }
    vdiqos_(vector);
    vdstls_(&c__0);
    dx = *x - vectrc_1.xcur;
    dy = *y - vectrc_1.ycur;
    linlen = sqrt(dx * dx + dy * dy);
    nstyle = (integer) vector[3];
    if (nstyle == 0 || linlen == (float)0.) {
	pltlig_(x, y);
    } else {
	idshno = pltsty_1.idshsv;
	dshdrn = pltsty_1.savlen;
/* Computing MAX */
	r__1 = vector[4] * (float).02;
	dshlen = dbl_max(r__1,(float).002);
	dshlen = dbl_min(dshlen,(float).005);
	dshdx = dx * (float).5 * (dshlen / linlen);
	dshdy = dy * (float).5 * (dshlen / linlen);
	drwlen = linlen - dshlen + dshdrn;
	if (drwlen <= (float)0.) {
	    xx = vectrc_1.xcur;
	    yy = vectrc_1.ycur;
	} else {
	    ++idshno;
	    xx = vectrc_1.xcur + dx * (dshlen - dshdrn) / linlen;
	    yy = vectrc_1.ycur + dy * (dshlen - dshdrn) / linlen;
	    if (istyle[idshno % 8 + (nstyle << 3) - 8] != 1) {
		if (istyle[(idshno + 1) % 8 + (nstyle << 3) - 8] == 1) {
		    pltmov_(&xx, &yy);
		}
	    } else if (istyle[(idshno + 1) % 8 + (nstyle << 3) - 8] != 1 && 
		    dshdrn < dshlen / 2) {
		r__1 = (vectrc_1.xcur + xx) * (float).5;
		r__2 = (vectrc_1.ycur + yy) * (float).5;
		pltlig_(&r__1, &r__2);
	    }
	    dshdrn = (float)0.;
/* Computing MAX */
	    r__1 = drwlen / dshlen;
	    i__1 = i_nint(&r__1);
	    numdsh = int_max(i__1,1);
	    i__1 = numdsh - 1;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		++idshno;
		x2 = xx + dshdx;
		y2 = yy + dshdy;
		xx = x2 + dshdx;
		yy = y2 + dshdy;
		if (istyle[idshno % 8 + (nstyle << 3) - 8] != 1) {
		    if (istyle[(idshno + 1) % 8 + (nstyle << 3) - 8] == 1) {
			pltmov_(&xx, &yy);
		    }
		} else if (istyle[(idshno + 1) % 8 + (nstyle << 3) - 8] != 1) 
			{
		    pltlig_(&x2, &y2);
		}
/* L2020: */
	    }
	}
	dx = *x - xx;
	dy = *y - yy;
	prvdrn = dshdrn;
	dshdrn = sqrt(dx * dx + dy * dy) + prvdrn;
	if (istyle[(idshno + 1) % 8 + (nstyle << 3) - 8] != 1) {
	    pltmov_(x, y);
	} else if (istyle[(idshno + 2) % 8 + (nstyle << 3) - 8] != 1 && 
		dshdrn > dshlen / 2) {
	    if (prvdrn < dshlen / 2) {
		r__1 = xx + dshdx;
		r__2 = yy + dshdy;
		pltlig_(&r__1, &r__2);
	    }
	    pltmov_(x, y);
	} else {
	    pltlig_(x, y);
	}
    }
    pltsty_1.idshsv = idshno;
    pltsty_1.savlen = dshdrn;
    vectrc_1.xcur = *x;
    vectrc_1.ycur = *y;
    i__1 = (integer) vector[3];
    vdstls_(&i__1);
    return 0;
} /* pltdrw_ */

#ifdef __cplusplus
	}
#endif

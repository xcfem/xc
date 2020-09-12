/* mpd2sy.f -- translated by f2c (version 20160102).
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
    real model[16]	/* was [4][4] */, view[16]	/* was [4][4] */, 
	    proj[16]	/* was [4][4] */, cpnear, cpfar, vwport[4], mvp[16]	
	    /* was [4][4] */, vp[16]	/* was [4][4] */, cpline[40]	/* 
	    was [2][2][10] */, cpplan[60]	/* was [2][3][10] */, peye[3],
	     plook[3], etwist;
    integer ncplin, ncplan;
    real tmat1[16]	/* was [4][4] */, tmat2[16]	/* was [4][4] */, 
	    tmat3[16]	/* was [4][4] */, tvec1[4], tvec2[4], tvec3[4], tvec4[
	    4], tarr1[32], tarr2[32], tarr3[32], tarr4[32], tarr5[32], tarr6[
	    32], tarr7[32], tarr8[32];
} map_;

#define map_1 map_

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int mpd2sy_(integer *n, real *x0, real *y0, char *sym, 
	ftnlen sym_len)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, j, k;
    static real c1[2], c2[2];
    static integer j1, jn;
    static real vcx, vcy, vsx, vsy;
    static integer mask[1];
    extern /* Subroutine */ int pltcp2_(integer *, integer *, real *, real *, 
	    real *, real *), mpmul2_(integer *, real *, real *, real *, real *
	    , real *, real *, real *), mpmul4_(integer *, integer *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, real *);
    static logical noclip;
    extern /* Subroutine */ int pltsbm_(integer *, integer *, real *, real *, 
	    char *, ftnlen), pltvwp_(real *, real *, integer *, integer *, 
	    real *, real *);

    /* Parameter adjustments */
    --y0;
    --x0;

    /* Function Body */
    vcx = (map_1.vwport[0] + map_1.vwport[1]) / (float)2.;
    vsx = (map_1.vwport[1] - map_1.vwport[0]) / (float)2.;
    vcy = (map_1.vwport[2] + map_1.vwport[3]) / (float)2.;
    vsy = (map_1.vwport[3] - map_1.vwport[2]) / (float)2.;
    noclip = map_1.ncplin == 0;
    j = 0;
L2600:
    if (! (j < *n)) {
	goto L2610;
    }
/* Computing MIN */
    i__1 = *n - j;
    jn = int_min(i__1,32);
    j1 = j;
    j += jn;
    mask[0] = -1;
    if (noclip) {
	mpmul2_(&jn, &x0[j1 + 1], &y0[j1 + 1], map_1.mvp, map_1.tarr1, 
		map_1.tarr2, map_1.tarr3, map_1.tarr4);
    } else {
	mpmul2_(&jn, &x0[j1 + 1], &y0[j1 + 1], map_1.model, map_1.tarr1, 
		map_1.tarr2, map_1.tarr3, map_1.tarr4);
	i__1 = map_1.ncplin;
	for (k = 1; k <= i__1; ++k) {
	    c1[0] = map_1.cpline[((k << 1) + 1 << 1) - 6];
	    c1[1] = map_1.cpline[((k << 1) + 2 << 1) - 6];
	    c2[0] = map_1.cpline[((k << 1) + 1 << 1) - 5];
	    c2[1] = map_1.cpline[((k << 1) + 2 << 1) - 5];
	    pltcp2_(&jn, mask, map_1.tarr1, map_1.tarr2, c1, c2);
/* L2620: */
	}
	mpmul4_(&jn, mask, map_1.tarr1, map_1.tarr2, map_1.tarr3, map_1.tarr4,
		 map_1.vp, map_1.tarr1, map_1.tarr2, map_1.tarr3, map_1.tarr4)
		;
    }
    i__1 = jn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	map_1.tarr1[i__ - 1] = map_1.tarr1[i__ - 1] / map_1.tarr4[i__ - 1] * 
		vsx + vcx;
	map_1.tarr2[i__ - 1] = map_1.tarr2[i__ - 1] / map_1.tarr4[i__ - 1] * 
		vsy + vcy;
/* L2640: */
    }
    c1[0] = map_1.vwport[0];
    c1[1] = map_1.vwport[2];
    c2[0] = map_1.vwport[1];
    c2[1] = map_1.vwport[3];
    pltvwp_(c1, c2, &jn, mask, map_1.tarr1, map_1.tarr2);
    pltsbm_(&jn, mask, map_1.tarr1, map_1.tarr2, sym, sym_len);
    goto L2600;
L2610:
    return 0;
} /* mpd2sy_ */

#ifdef __cplusplus
	}
#endif

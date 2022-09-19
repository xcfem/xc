/* mpd2vc.f -- translated by f2c (version 20160102).
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

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int mpd2vc_(integer *n, xc_float *x0, xc_float *y0, xc_float *x1, xc_float *
	y1)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, j, k;
    static xc_float c1[2], c2[2];
    static integer j1, jn;
    static xc_float vcx, vcy, vsx, vsy;
    static integer mask[1];
    extern /* Subroutine */ int pltcv2_(integer *, integer *, xc_float *, xc_float *, 
	    xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *), 
	    mpmul2_(integer *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *,
	     xc_float *), mpmul4_(integer *, integer *, xc_float *, xc_float *, xc_float *, 
	    xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *);
    static logical noclip;
    extern /* Subroutine */ int pltvcm_(integer *, integer *, xc_float *, xc_float *, 
	    xc_float *, xc_float *), pltvwv_(xc_float *, xc_float *, integer *, integer *, 
	    xc_float *, xc_float *, xc_float *, xc_float *);

    /* Parameter adjustments */
    --y1;
    --x1;
    --y0;
    --x0;

    /* Function Body */
    vcx = (map_1.vwport[0] + map_1.vwport[1]) / (xc_float)2.;
    vsx = (map_1.vwport[1] - map_1.vwport[0]) / (xc_float)2.;
    vcy = (map_1.vwport[2] + map_1.vwport[3]) / (xc_float)2.;
    vsy = (map_1.vwport[3] - map_1.vwport[2]) / (xc_float)2.;
    noclip = map_1.ncplin == 0;
    j = 0;
L10:
    if (j < *n) {
/* Computing MIN */
	i__1 = *n - j;
	jn = int_min(i__1,32);
	j1 = j;
	j += jn;
	mask[0] = -1;
	if (noclip) {
	    mpmul2_(&jn, &x0[j1 + 1], &y0[j1 + 1], map_1.mvp, map_1.tarr1, 
		    map_1.tarr2, map_1.tarr3, map_1.tarr4);
	    mpmul2_(&jn, &x1[j1 + 1], &y1[j1 + 1], map_1.mvp, map_1.tarr5, 
		    map_1.tarr6, map_1.tarr7, map_1.tarr8);
	} else {
	    mpmul2_(&jn, &x0[j1 + 1], &y0[j1 + 1], map_1.model, map_1.tarr1, 
		    map_1.tarr2, map_1.tarr3, map_1.tarr4);
	    mpmul2_(&jn, &x1[j1 + 1], &y1[j1 + 1], map_1.model, map_1.tarr5, 
		    map_1.tarr6, map_1.tarr7, map_1.tarr8);
	    i__1 = map_1.ncplin;
	    for (k = 1; k <= i__1; ++k) {
		c1[0] = map_1.cpline[((k << 1) + 1 << 1) - 6];
		c1[1] = map_1.cpline[((k << 1) + 2 << 1) - 6];
		c2[0] = map_1.cpline[((k << 1) + 1 << 1) - 5];
		c2[1] = map_1.cpline[((k << 1) + 2 << 1) - 5];
		pltcv2_(&jn, mask, map_1.tarr1, map_1.tarr2, map_1.tarr5, 
			map_1.tarr6, map_1.tarr1, map_1.tarr2, map_1.tarr5, 
			map_1.tarr6, c1, c2);
/* L20: */
	    }
	    mpmul4_(&jn, mask, map_1.tarr1, map_1.tarr2, map_1.tarr3, 
		    map_1.tarr4, map_1.vp, map_1.tarr1, map_1.tarr2, 
		    map_1.tarr3, map_1.tarr4);
	    mpmul4_(&jn, mask, map_1.tarr5, map_1.tarr6, map_1.tarr7, 
		    map_1.tarr8, map_1.vp, map_1.tarr5, map_1.tarr6, 
		    map_1.tarr7, map_1.tarr8);
	}
	i__1 = jn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    map_1.tarr1[i__ - 1] = map_1.tarr1[i__ - 1] / map_1.tarr4[i__ - 1]
		     * vsx + vcx;
	    map_1.tarr2[i__ - 1] = map_1.tarr2[i__ - 1] / map_1.tarr4[i__ - 1]
		     * vsy + vcy;
	    map_1.tarr5[i__ - 1] = map_1.tarr5[i__ - 1] / map_1.tarr8[i__ - 1]
		     * vsx + vcx;
	    map_1.tarr6[i__ - 1] = map_1.tarr6[i__ - 1] / map_1.tarr8[i__ - 1]
		     * vsy + vcy;
/* L30: */
	}
	c1[0] = map_1.vwport[0];
	c1[1] = map_1.vwport[2];
	c2[0] = map_1.vwport[1];
	c2[1] = map_1.vwport[3];
	pltvwv_(c1, c2, &jn, mask, map_1.tarr1, map_1.tarr2, map_1.tarr5, 
		map_1.tarr6);
	pltvcm_(&jn, mask, map_1.tarr1, map_1.tarr2, map_1.tarr5, map_1.tarr6)
		;
	goto L10;
    }
    return 0;
} /* mpd2vc_ */

#ifdef __cplusplus
	}
#endif

/* pltdv2.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int pltdv2_(real *map, integer *n, real *px, real *py, real *
	qx, real *qy)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, j, j1, jn;
    static real ppx[32], ppy[32], qqx[32], qqy[32];
    static integer mask[1];
    extern /* Subroutine */ int pltvcm_(integer *, integer *, real *, real *, 
	    real *, real *), pltvwv_(real *, real *, integer *, integer *, 
	    real *, real *, real *, real *);

    /* Parameter adjustments */
    --qy;
    --qx;
    --py;
    --px;
    --map;

    /* Function Body */
    j = 0;
L2470:
    if (j < *n) {
/* Computing MIN */
	i__1 = *n - j;
	jn = int_min(i__1,32);
	j1 = j;
	j += jn;
	i__1 = jn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ppx[i__ - 1] = map[1] * px[i__ + j1] + map[3] * py[i__ + j1] + 
		    map[5];
	    qqx[i__ - 1] = map[1] * qx[i__ + j1] + map[3] * qy[i__ + j1] + 
		    map[5];
	    ppy[i__ - 1] = map[2] * px[i__ + j1] + map[4] * py[i__ + j1] + 
		    map[6];
	    qqy[i__ - 1] = map[2] * qx[i__ + j1] + map[4] * qy[i__ + j1] + 
		    map[6];
/* L2490: */
	}
	mask[0] = -1;
	pltvwv_(&map[7], &map[11], &jn, mask, ppx, ppy, qqx, qqy);
	pltvcm_(&jn, mask, ppx, ppy, qqx, qqy);
	goto L2470;
    }
    return 0;
} /* pltdv2_ */

#ifdef __cplusplus
	}
#endif

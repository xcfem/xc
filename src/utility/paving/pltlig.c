/* pltlig.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__1 = 1;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltlig_(xc_float *x, xc_float *y)
{
    static xc_float xc, yc, xp, yp;
    static integer mask[1];
    extern /* Subroutine */ int pltp2d_(xc_float *, xc_float *, xc_float *, xc_float *), 
	    vdlina_(xc_float *, xc_float *), vdmova_(xc_float *, xc_float *), pltvwv_(xc_float *, 
	    xc_float *, integer *, integer *, xc_float *, xc_float *, xc_float *, xc_float *);

    if (mappar_1.mapp[9] == (xc_float)1.) {
	pltp2d_(x, y, &xp, &yp);
	pltp2d_(&vectrc_1.xcur, &vectrc_1.ycur, &xc, &yc);
    } else {
	xp = *x;
	yp = *y;
	xc = vectrc_1.xcur;
	yc = vectrc_1.ycur;
    }
    mask[0] = -1;
    pltvwv_(&mappar_1.mapp[5], &mappar_1.mapp[7], &c__1, mask, &xc, &yc, &xp, 
	    &yp);
    if (mask[0] == -1) {
	vdmova_(&xc, &yc);
	vdlina_(&xp, &yp);
    }
    return 0;
} /* pltlig_ */

#ifdef __cplusplus
	}
#endif

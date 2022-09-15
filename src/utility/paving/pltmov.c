/* pltmov.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int pltmov_(xc_float *x, xc_float *y)
{
    static xc_float xp, yp;
    static integer mask[1];
    extern /* Subroutine */ int pltp2d_(xc_float *, xc_float *, xc_float *, xc_float *), 
	    vdmova_(xc_float *, xc_float *), pltvwp_(xc_float *, xc_float *, integer *, 
	    integer *, xc_float *, xc_float *);

    if (mappar_1.mapp[9] == (xc_float)1.) {
	pltp2d_(x, y, &xp, &yp);
    } else {
	xp = *x;
	yp = *y;
    }
    mask[0] = -1;
    pltvwp_(&mappar_1.mapp[5], &mappar_1.mapp[7], &c__1, mask, &xp, &yp);
    if (mask[0] == -1) {
	vdmova_(&xp, &yp);
    }
    vectrc_1.xcur = *x;
    vectrc_1.ycur = *y;
    pltsty_1.idshsv = 0;
    pltsty_1.savlen = (xc_float)0.;
    return 0;
} /* pltmov_ */

#ifdef __cplusplus
	}
#endif

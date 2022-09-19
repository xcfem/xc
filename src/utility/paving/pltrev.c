/* pltrev.f -- translated by f2c (version 20160102).
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
static integer c__2 = 2;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltrev_()
{
    static integer i__;
    extern /* Subroutine */ int pltstv_(integer *, xc_float *);

    if (vectrc_1.vectp[0] != psave_1.tvectp[psave_1.ipopv * 5 - 5]) {
	pltstv_(&c__1, &psave_1.tvectp[psave_1.ipopv * 5 - 5]);
    }
    if (vectrc_1.vectp[1] != psave_1.tvectp[psave_1.ipopv * 5 - 4]) {
	pltstv_(&c__2, &psave_1.tvectp[psave_1.ipopv * 5 - 4]);
    }
    for (i__ = 1; i__ <= 5; ++i__) {
	vectrc_1.vectp[i__ - 1] = psave_1.tvectp[i__ + psave_1.ipopv * 5 - 6];
/* L2200: */
    }
    if (psave_1.ipopv != 1) {
	--psave_1.ipopv;
    }
    return 0;
} /* pltrev_ */

#ifdef __cplusplus
	}
#endif

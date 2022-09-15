/* pltbgn.f -- translated by f2c (version 20160102).
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

static xc_float c_b2 = (xc_float)0.;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltbgn_()
{
    static integer i__;
    extern /* Subroutine */ int vdnwpg_(), pltmov_(xc_float *, xc_float *);

    vdnwpg_();
    pltmov_(&c_b2, &c_b2);
    for (i__ = 4; i__ <= 11; ++i__) {
	text_1.textp[i__ - 1] = (xc_float)0.;
/* L2000: */
    }
    return 0;
} /* pltbgn_ */

#ifdef __cplusplus
	}
#endif

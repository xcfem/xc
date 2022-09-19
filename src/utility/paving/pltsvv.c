/* pltsvv.f -- translated by f2c (version 20160102).
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

static integer c__3 = 3;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltsvv_()
{
    extern /* Subroutine */ int pltflu_(), siorpt_(char *, char *, integer *, 
	    ftnlen, ftnlen);

    if (psave_1.ipopv == 10) {
	pltflu_();
	siorpt_("PLTSVV", "Too many calls to PLTSVV.", &c__3, (ftnlen)6, (
		ftnlen)25);
	return 0;
    }
    ++psave_1.ipopv;
    psave_1.tvectp[psave_1.ipopv * 5 - 5] = vectrc_1.vectp[0];
    psave_1.tvectp[psave_1.ipopv * 5 - 4] = vectrc_1.vectp[1];
    psave_1.tvectp[psave_1.ipopv * 5 - 3] = vectrc_1.vectp[2];
    psave_1.tvectp[psave_1.ipopv * 5 - 2] = vectrc_1.vectp[3];
    psave_1.tvectp[psave_1.ipopv * 5 - 1] = vectrc_1.vectp[4];
    return 0;
} /* pltsvv_ */

#ifdef __cplusplus
	}
#endif

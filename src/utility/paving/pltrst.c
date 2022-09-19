/* pltrst.f -- translated by f2c (version 20160102).
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

static integer c__12 = 12;
static xc_float c_b3 = (xc_float)2.;
static xc_float c_b5 = (xc_float)3.;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltrst_()
{
    static integer i__;
    static xc_float buff;
    extern /* Subroutine */ int vdstcs_(xc_float *), pltitm_(), pltstt_(integer *,
	     xc_float *);

    if ((status_1.devcap[14] + status_1.devcap[15]) / (xc_float)2. <= (xc_float)
	    800.) {
	pltstt_(&c__12, &c_b3);
    } else {
	pltstt_(&c__12, &c_b5);
    }
    buff = (xc_float).017241379310344827;
    vdstcs_(&buff);
    text_1.textp[34] = status_1.defout[5];
    text_1.textp[35] = status_1.defout[6];
    text_1.textp[0] = (xc_float).015;
    text_1.textp[1] = (xc_float)0.;
    text_1.textp[2] = (xc_float)0.;
    for (i__ = 4; i__ <= 11; ++i__) {
	text_1.textp[i__ - 1] = (xc_float)0.;
/* L2340: */
    }
    text_1.textp[19] = (xc_float)0.;
    text_1.textp[20] = (xc_float)0.;
    text_1.textp[21] = (xc_float)1.;
    text_1.textp[22] = (xc_float)0.;
    text_1.textp[23] = text_1.textp[21];
    text_1.textp[24] = (xc_float).75;
    text_1.textp[25] = (xc_float)0.;
    text_1.textp[26] = text_1.textp[24];
    text_1.textp[29] = (xc_float)1.5;
    text_1.textp[30] = (xc_float)1.;
    text_1.textp[31] = (xc_float).7;
    text_1.textp[32] = (xc_float).8;
    text_1.textp[33] = (xc_float).5;
    text_1.textp[36] = (xc_float)160.;
    pltitm_();
    return 0;
} /* pltrst_ */

#ifdef __cplusplus
	}
#endif

/* mport2.f -- translated by f2c (version 20160102).
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

static integer c__2 = 2;
static integer c__4 = 4;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical mport2_(xc_float *left, xc_float *right, xc_float *bottom, xc_float *top)
{
    /* System generated locals */
    logical ret_val;

    /* Local variables */
    extern /* Subroutine */ int pltflu_(), siorpt_(char *, char *, integer *, 
	    ftnlen, ftnlen), mxzero_(integer *, xc_float *), mxmult_(integer *, 
	    xc_float *, xc_float *, xc_float *);

    ret_val = FALSE_;
    if (*right == *left) {
	pltflu_();
	siorpt_("MPORT2", "You cannot specify the right and left edges of th\
e clipping rectangle as equal", &c__2, (ftnlen)6, (ftnlen)78);
	return ret_val;
    }
    if (*top == *bottom) {
	pltflu_();
	siorpt_("MPORT2", "You cannot specify the top and bottom edges of th\
e clipping rectangle as equal", &c__2, (ftnlen)6, (ftnlen)78);
	return ret_val;
    }
    ret_val = TRUE_;
    mxzero_(&c__4, map_1.proj);
    map_1.proj[0] = (xc_float)2. / (*right - *left);
    map_1.proj[5] = (xc_float)2. / (*top - *bottom);
    map_1.proj[10] = (xc_float)-1.;
    map_1.proj[15] = (xc_float)1.;
    map_1.proj[3] = -(*right + *left) / (*right - *left);
    map_1.proj[7] = -(*top + *bottom) / (*top - *bottom);
    mxmult_(&c__4, map_1.view, map_1.proj, map_1.vp);
    mxmult_(&c__4, map_1.model, map_1.vp, map_1.mvp);
    return ret_val;
} /* mport2_ */

#ifdef __cplusplus
	}
#endif

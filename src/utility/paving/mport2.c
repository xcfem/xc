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
//#include "f2c.h"
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

/* Table of constant values */

static integer c__2 = 2;
static integer c__4 = 4;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical mport2_(real *left, real *right, real *bottom, real *top)
{
    /* System generated locals */
    logical ret_val;

    /* Local variables */
    extern /* Subroutine */ int pltflu_(), siorpt_(char *, char *, integer *, 
	    ftnlen, ftnlen), mxzero_(integer *, real *), mxmult_(integer *, 
	    real *, real *, real *);

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
    map_1.proj[0] = (float)2. / (*right - *left);
    map_1.proj[5] = (float)2. / (*top - *bottom);
    map_1.proj[10] = (float)-1.;
    map_1.proj[15] = (float)1.;
    map_1.proj[3] = -(*right + *left) / (*right - *left);
    map_1.proj[7] = -(*top + *bottom) / (*top - *bottom);
    mxmult_(&c__4, map_1.view, map_1.proj, map_1.vp);
    mxmult_(&c__4, map_1.model, map_1.vp, map_1.mvp);
    return ret_val;
} /* mport2_ */

#ifdef __cplusplus
	}
#endif

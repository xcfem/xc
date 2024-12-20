/* mpview.f -- translated by f2c (version 20160102).
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

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical mpview_(xc_float *left, xc_float *right, xc_float *bottom, xc_float *top)
{
    /* System generated locals */
    logical ret_val;

    /* Local variables */
    extern /* Subroutine */ int pltflu_(), siorpt_(char *, char *, integer *, 
	    ftnlen, ftnlen);

    ret_val = FALSE_;
    if (*left >= *right) {
	pltflu_();
	siorpt_("MPVIEW", "You cannot specify the left viewport edge >= to t\
he right edge", &c__2, (ftnlen)6, (ftnlen)62);
	return ret_val;
    }
    if (*top <= *bottom) {
	pltflu_();
	siorpt_("MPVIEW", "You cannot specify the top viewport edge <= to th\
e bottom edge", &c__2, (ftnlen)6, (ftnlen)62);
	return ret_val;
    }
    if (*top < (xc_float)0. || *top > device_1.devp[4]) {
	pltflu_();
	siorpt_("MPVIEW", "Top viewport specification out of range", &c__2, (
		ftnlen)6, (ftnlen)39);
	return ret_val;
    }
    if (*bottom < (xc_float)0. || *bottom > device_1.devp[4]) {
	pltflu_();
	siorpt_("MPVIEW", "Bottom viewport specification out of range", &c__2,
		 (ftnlen)6, (ftnlen)42);
	return ret_val;
    }
    if (*left < (xc_float)0. || *left > device_1.devp[3]) {
	pltflu_();
	siorpt_("MPVIEW", "Left viewport specification out of range", &c__2, (
		ftnlen)6, (ftnlen)40);
	return ret_val;
    }
    if (*right < (xc_float)0. || *right > device_1.devp[3]) {
	pltflu_();
	siorpt_("MPVIEW", "Right viewport specification out of range", &c__2, 
		(ftnlen)6, (ftnlen)41);
	return ret_val;
    }
    ret_val = TRUE_;
    map_1.vwport[0] = *left;
    map_1.vwport[1] = *right;
    map_1.vwport[2] = *bottom;
    map_1.vwport[3] = *top;
    return ret_val;
} /* mpview_ */

#ifdef __cplusplus
	}
#endif

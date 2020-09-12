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

struct {
    real devcap[23], defout[7];
} status_;

#define status_1 status_

struct {
    real devp[5];
} device_;

#define device_1 device_

struct {
    real colp[3], palett[48]	/* was [3][16] */;
} color_;

#define color_1 color_

struct {
    real textp[40];
} text_;

#define text_1 text_

struct {
    real vectp[5], xcur, ycur;
} vectrc_;

#define vectrc_1 vectrc_

struct {
    integer idex[400]	/* was [200][2] */, nvect[400]	/* was [200][2] */;
    real xsize[400]	/* was [200][2] */, ysize[400]	/* was [200][2] */, 
	    x0[4600]	/* was [2300][2] */, y0[4600]	/* was [2300][2] */, 
	    x1[4600]	/* was [2300][2] */, y1[4600]	/* was [2300][2] */;
} font_;

#define font_1 font_

struct {
    real graphp[100];
} graph_;

#define graph_1 graph_

struct {
    real mapp[11];
} mappar_;

#define mappar_1 mappar_

struct {
    integer memory[1000];
} storag_;

#define storag_1 storag_

/* Table of constant values */

static integer c__2 = 2;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical mpview_(real *left, real *right, real *bottom, real *top)
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
    if (*top < (float)0. || *top > device_1.devp[4]) {
	pltflu_();
	siorpt_("MPVIEW", "Top viewport specification out of range", &c__2, (
		ftnlen)6, (ftnlen)39);
	return ret_val;
    }
    if (*bottom < (float)0. || *bottom > device_1.devp[4]) {
	pltflu_();
	siorpt_("MPVIEW", "Bottom viewport specification out of range", &c__2,
		 (ftnlen)6, (ftnlen)42);
	return ret_val;
    }
    if (*left < (float)0. || *left > device_1.devp[3]) {
	pltflu_();
	siorpt_("MPVIEW", "Left viewport specification out of range", &c__2, (
		ftnlen)6, (ftnlen)40);
	return ret_val;
    }
    if (*right < (float)0. || *right > device_1.devp[3]) {
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

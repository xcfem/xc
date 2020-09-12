/* pltstt.f -- translated by f2c (version 20160102).
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

/* Common Block Declarations */

struct {
    xc_float devcap[23], defout[7];
} status_;

#define status_1 status_

struct {
    xc_float devp[5];
} device_;

#define device_1 device_

struct {
    xc_float colp[3], palett[48]	/* was [3][16] */;
} color_;

#define color_1 color_

struct {
    xc_float textp[40];
} text_;

#define text_1 text_

struct {
    xc_float vectp[5], xcur, ycur;
} vectrc_;

#define vectrc_1 vectrc_

struct {
    integer idex[400]	/* was [200][2] */, nvect[400]	/* was [200][2] */;
    xc_float xsize[400]	/* was [200][2] */, ysize[400]	/* was [200][2] */, 
	    x0[4600]	/* was [2300][2] */, y0[4600]	/* was [2300][2] */, 
	    x1[4600]	/* was [2300][2] */, y1[4600]	/* was [2300][2] */;
} font_;

#define font_1 font_

struct {
    xc_float graphp[100];
} graph_;

#define graph_1 graph_

struct {
    xc_float mapp[11];
} mappar_;

#define mappar_1 mappar_

struct {
    integer memory[1000];
} storag_;

#define storag_1 storag_

/* Table of constant values */

static integer c__2 = 2;
static integer c__3 = 3;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical pltstt_(integer *indx, xc_float *buff)
{
    /* System generated locals */
    char * a__1[3];
    integer i__1[3];
    logical ret_val;
    char ch__1[49], ch__2[31];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer i__, l;
    extern /* Subroutine */ int chric_(integer *, char *, integer *, ftnlen);
    static xc_float dummy[7];
    extern /* Subroutine */ int chrrvc_(xc_float *, char *, integer *, ftnlen);
    static char ierror[16];
    extern /* Subroutine */ int vdiqos_(xc_float *), vdstcs_(xc_float *), pltfnt_(
	    char *, ftnlen), pltitm_(), pltflu_(), siorpt_(char *, char *, 
	    integer *, ftnlen, ftnlen), pltrst_();

    /* Parameter adjustments */
    --buff;

    /* Function Body */
    ret_val = TRUE_;
    if (*indx == 0) {
	pltrst_();
    } else if (*indx == 1) {
	vdstcs_(&buff[1]);
	vdiqos_(dummy);
	text_1.textp[34] = dummy[5];
	text_1.textp[35] = dummy[6];
    } else if (*indx == 2) {
	text_1.textp[0] = buff[1];
	pltitm_();
    } else if (*indx == 3) {
	text_1.textp[1] = buff[1];
	pltitm_();
    } else if (*indx == 4) {
	text_1.textp[2] = buff[1];
	pltitm_();
    } else if (*indx == 5) {
	for (i__ = 20; i__ <= 27; ++i__) {
	    text_1.textp[i__ - 1] = buff[i__ - 19];
/* L2300: */
	}
    } else if (*indx == 6) {
	text_1.textp[29] = buff[1];
    } else if (*indx == 7) {
	text_1.textp[30] = buff[1];
    } else if (*indx == 8) {
	text_1.textp[31] = buff[1];
    } else if (*indx == 9) {
	text_1.textp[32] = buff[1];
    } else if (*indx == 10) {
	text_1.textp[33] = buff[1];
    } else if (*indx == 11) {
	text_1.textp[36] = buff[1];
    } else if (*indx == 12) {
	if (buff[1] == (xc_float)1.) {
	    pltfnt_("ROMFNT", (ftnlen)6);
	    text_1.textp[37] = (xc_float)1.;
	} else if (buff[1] == (xc_float)2.) {
	    pltfnt_("STKFNT", (ftnlen)6);
	    text_1.textp[37] = (xc_float)2.;
	} else if (buff[1] == (xc_float)3.) {
	    pltfnt_("SSRFNT", (ftnlen)6);
	    text_1.textp[37] = (xc_float)3.;
	} else {
	    chrrvc_(&buff[1], ierror, &l, (ftnlen)16);
	    pltflu_();
/* Writing concatenation */
	    i__1[0] = 15, a__1[0] = "Illegal buffer ";
	    i__1[1] = l, a__1[1] = ierror;
	    i__1[2] = 18, a__1[2] = " passed to PLTSTT.";
	    s_cat(ch__1, a__1, i__1, &c__3, (ftnlen)49);
	    siorpt_("PLTSTT", ch__1, &c__2, (ftnlen)6, l + 33);
	    ret_val = FALSE_;
	}
    } else {
	chric_(indx, ierror, &l, (ftnlen)16);
	pltflu_();
/* Writing concatenation */
	i__1[0] = 14, a__1[0] = "Illegal index ";
	i__1[1] = l, a__1[1] = ierror;
	i__1[2] = 1, a__1[2] = ".";
	s_cat(ch__2, a__1, i__1, &c__3, (ftnlen)31);
	siorpt_("PLTSTT", ch__2, &c__2, (ftnlen)6, l + 15);
	ret_val = FALSE_;
	return ret_val;
    }
    return ret_val;
} /* pltstt_ */

#ifdef __cplusplus
	}
#endif

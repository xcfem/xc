/* pltstg.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/* Common Block Declarations */

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
static integer c__3 = 3;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical pltstg_(integer *indx, real *buff)
{
    /* System generated locals */
    address a__1[3];
    integer i__1[3];
    logical ret_val;
    char ch__1[60], ch__2[31];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer i__, l;
    extern /* Subroutine */ int chric_(integer *, char *, integer *, ftnlen), 
	    chrrvc_(real *, char *, integer *, ftnlen);
    static char ierror[16];
    extern /* Subroutine */ int pltflu_(), pltrsg_(), siorpt_(char *, char *, 
	    integer *, ftnlen, ftnlen);

    /* Parameter adjustments */
    --buff;

    /* Function Body */
    ret_val = TRUE_;
    if (*indx == 0) {
	pltrsg_();
    } else if (*indx == 1) {
	graph_1.graphp[0] = buff[1];
    } else if (*indx == 2) {
	graph_1.graphp[1] = buff[1];
    } else if (*indx == 3) {
	graph_1.graphp[2] = buff[1];
    } else if (*indx == 4) {
	graph_1.graphp[3] = buff[1];
    } else if (*indx == 5) {
	graph_1.graphp[4] = buff[1];
    } else if (*indx == 6) {
	graph_1.graphp[37] = buff[1];
    } else if (*indx == 7) {
	if (buff[1] == (float)0.) {
	    graph_1.graphp[5] = (float)0.;
	} else {
	    graph_1.graphp[5] = (float)1.;
	    graph_1.graphp[46] = buff[1] + (float)4.;
	}
    } else if (*indx == 8) {
	if (buff[1] <= (float)0.) {
	    pltflu_();
	    siorpt_("PLTSTG", "Symbol increment must be greater than zero.", &
		    c__2, (ftnlen)6, (ftnlen)43);
	    ret_val = FALSE_;
	    return ret_val;
	}
	graph_1.graphp[22] = buff[1];
    } else if (*indx == 9) {
	graph_1.graphp[20] = buff[1];
    } else if (*indx == 10) {
	graph_1.graphp[36] = buff[1];
    } else if (*indx == 11) {
	if (buff[1] == (float)1.) {
	    graph_1.graphp[21] = buff[1];
	} else if (buff[1] == (float)2.) {
	    graph_1.graphp[21] = buff[1];
	} else if (buff[1] == (float)3.) {
	    if (buff[2] == buff[3]) {
		pltflu_();
		siorpt_("PLTSTG", "XMIN cannot be equal to XMAX.", &c__2, (
			ftnlen)6, (ftnlen)29);
		ret_val = FALSE_;
		return ret_val;
	    }
	    if (buff[6] == buff[7]) {
		pltflu_();
		siorpt_("PLTSTG", "YMIN cannot be equal to YMAX.", &c__2, (
			ftnlen)6, (ftnlen)29);
		ret_val = FALSE_;
		return ret_val;
	    }
	    if (buff[4] == (float)0.) {
		pltflu_();
		siorpt_("PLTSTG", "Number of major x intervals cannot equal \
zero.", &c__2, (ftnlen)6, (ftnlen)46);
		ret_val = FALSE_;
		return ret_val;
	    }
	    if (buff[8] == (float)0.) {
		pltflu_();
		siorpt_("PLTSTG", "Number of major y intervals cannot equal \
zero.", &c__2, (ftnlen)6, (ftnlen)46);
		ret_val = FALSE_;
		return ret_val;
	    }
	    graph_1.graphp[21] = buff[1];
	    for (i__ = 0; i__ <= 7; ++i__) {
		graph_1.graphp[i__ + 23] = buff[i__ + 2];
/* L2220: */
	    }
	} else if (buff[1] == (float)4.) {
	    if (buff[4] == buff[3]) {
		pltflu_();
		siorpt_("PLTSTG", "XMAX cannot equal first nice X number.", &
			c__2, (ftnlen)6, (ftnlen)38);
		ret_val = FALSE_;
		return ret_val;
	    }
	    if (buff[5] == (float)0.) {
		pltflu_();
		siorpt_("PLTSTG", "X interval cannot equal zero.", &c__2, (
			ftnlen)6, (ftnlen)29);
		ret_val = FALSE_;
		return ret_val;
	    }
	    if (buff[9] == buff[8]) {
		pltflu_();
		siorpt_("PLTSTG", "YMAX cannot equal first nice Y number.", &
			c__2, (ftnlen)6, (ftnlen)38);
		ret_val = FALSE_;
		return ret_val;
	    }
	    if (buff[10] == (float)0.) {
		pltflu_();
		siorpt_("PLTSTG", "Y interval cannot equal zero.", &c__2, (
			ftnlen)6, (ftnlen)29);
		ret_val = FALSE_;
		return ret_val;
	    }
	    if (buff[3] < buff[2] && buff[4] > buff[2] || buff[3] > buff[2] &&
		     buff[4] < buff[3]) {
		pltflu_();
		siorpt_("PLTSTG", "Invalid specification of XMIN, XNICE and \
XMAX.", &c__2, (ftnlen)6, (ftnlen)46);
		ret_val = FALSE_;
		return ret_val;
	    }
	    if (buff[8] < buff[7] && buff[9] > buff[7] || buff[8] > buff[7] &&
		     buff[9] < buff[8]) {
		pltflu_();
		siorpt_("PLTSTG", "Invalid specification of YMIN, YNICE and \
YMAX.", &c__2, (ftnlen)6, (ftnlen)46);
		ret_val = FALSE_;
		return ret_val;
	    }
	    if (buff[4] < buff[2] && buff[5] > (float)0.) {
		pltflu_();
		siorpt_("PLTSTG", "Setting X interval negative as XMAX < XMI\
N.", &c__2, (ftnlen)6, (ftnlen)43);
		buff[5] = -buff[5];
	    }
	    if (buff[9] < buff[7] && buff[10] > (float)0.) {
		pltflu_();
		siorpt_("PLTSTG", "Setting Y interval negative as YMAX < YMI\
N.", &c__2, (ftnlen)6, (ftnlen)43);
		buff[10] = -buff[10];
	    }
	    graph_1.graphp[21] = buff[1];
	    for (i__ = 0; i__ <= 9; ++i__) {
		graph_1.graphp[i__ + 77] = buff[i__ + 2];
/* L2240: */
	    }
	} else {
	    chrrvc_(&buff[1], ierror, &l, (ftnlen)16);
	    pltflu_();
/* Writing concatenation */
	    i__1[0] = 21, a__1[0] = "Illegal buffer value ";
	    i__1[1] = l, a__1[1] = ierror;
	    i__1[2] = 23, a__1[2] = " in graph scaling type.";
	    s_cat(ch__1, a__1, i__1, &c__3, (ftnlen)60);
	    siorpt_("PLTSTG", ch__1, &c__2, (ftnlen)6, l + 44);
	    ret_val = FALSE_;
	    return ret_val;
	}
    } else if (*indx == 12) {
	graph_1.graphp[31] = buff[1];
    } else if (*indx == 13) {
	graph_1.graphp[90] = buff[1];
    } else if (*indx == 14) {
	graph_1.graphp[89] = buff[1];
    } else if (*indx == 15) {
	graph_1.graphp[34] = buff[1];
    } else if (*indx == 16) {
	graph_1.graphp[35] = buff[1];
    } else if (*indx == 17) {
	graph_1.graphp[38] = buff[1];
    } else if (*indx == 18) {
	graph_1.graphp[39] = buff[1];
    } else if (*indx == 19) {
	graph_1.graphp[40] = buff[1];
    } else if (*indx == 20) {
	graph_1.graphp[41] = buff[1];
    } else if (*indx == 21) {
	graph_1.graphp[91] = buff[1];
    } else if (*indx == 22) {
	graph_1.graphp[43] = buff[1];
    } else if (*indx == 23) {
	graph_1.graphp[44] = buff[1];
    } else if (*indx == 47) {
	graph_1.graphp[87] = buff[1];
    } else if (*indx == 48) {
	graph_1.graphp[88] = buff[1];
    } else if (*indx == 24) {
	graph_1.graphp[45] = buff[1];
    } else if (*indx == 25) {
	graph_1.graphp[47] = buff[1];
    } else if (*indx == 26) {
	graph_1.graphp[48] = buff[1];
    } else if (*indx == 27) {
	for (i__ = 0; i__ <= 13; ++i__) {
	    graph_1.graphp[i__ + 6] = buff[i__ + 1];
/* L2260: */
	}
    } else if (*indx == 28) {
	graph_1.graphp[61] = buff[1];
    } else if (*indx == 29) {
	graph_1.graphp[62] = buff[1];
    } else if (*indx == 30) {
	graph_1.graphp[63] = buff[1];
    } else if (*indx == 31) {
	graph_1.graphp[64] = buff[1];
    } else if (*indx == 32) {
	graph_1.graphp[65] = buff[1];
    } else if (*indx == 33) {
	graph_1.graphp[66] = buff[1];
    } else if (*indx == 34) {
	graph_1.graphp[67] = buff[1];
    } else if (*indx == 35) {
	graph_1.graphp[68] = buff[1];
    } else if (*indx == 36) {
	graph_1.graphp[69] = buff[1];
    } else if (*indx == 37) {
	graph_1.graphp[70] = buff[1];
    } else if (*indx == 38) {
	graph_1.graphp[71] = buff[1];
    } else if (*indx == 39) {
	graph_1.graphp[72] = buff[1];
    } else if (*indx == 43) {
	graph_1.graphp[73] = buff[1];
    } else if (*indx == 44) {
	graph_1.graphp[74] = buff[1];
    } else if (*indx == 45) {
	graph_1.graphp[75] = buff[1];
    } else if (*indx == 46) {
	graph_1.graphp[76] = buff[1];
    } else {
	chric_(indx, ierror, &l, (ftnlen)16);
	pltflu_();
/* Writing concatenation */
	i__1[0] = 14, a__1[0] = "Illegal index ";
	i__1[1] = l, a__1[1] = ierror;
	i__1[2] = 1, a__1[2] = ".";
	s_cat(ch__2, a__1, i__1, &c__3, (ftnlen)31);
	siorpt_("PLTSTG", ch__2, &c__2, (ftnlen)6, l + 15);
	ret_val = FALSE_;
	return ret_val;
    }
    return ret_val;
} /* pltstg_ */

/* .. Same as PLTSTG */
logical pltstg1_(integer *indx, real *buff)
{
    /* System generated locals */
    address a__1[3];
    integer i__1[3];
    logical ret_val;
    char ch__1[31];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer l;
    extern /* Subroutine */ int chric_(integer *, char *, integer *, ftnlen);
    static char ierror[16];
    extern /* Subroutine */ int pltflu_(), pltrsg_(), siorpt_(char *, char *, 
	    integer *, ftnlen, ftnlen);

    ret_val = TRUE_;
    if (*indx == 0) {
	pltrsg_();
    } else if (*indx == 1) {
	graph_1.graphp[0] = *buff;
    } else if (*indx == 2) {
	graph_1.graphp[1] = *buff;
    } else if (*indx == 3) {
	graph_1.graphp[2] = *buff;
    } else if (*indx == 4) {
	graph_1.graphp[3] = *buff;
    } else if (*indx == 5) {
	graph_1.graphp[4] = *buff;
    } else if (*indx == 6) {
	graph_1.graphp[37] = *buff;
    } else if (*indx == 7) {
	if (*buff == (float)0.) {
	    graph_1.graphp[5] = (float)0.;
	} else {
	    graph_1.graphp[5] = (float)1.;
	    graph_1.graphp[46] = *buff + (float)4.;
	}
    } else if (*indx == 8) {
	if (*buff <= (float)0.) {
	    pltflu_();
	    siorpt_("PLTSTG", "Symbol increment must be greater than zero.", &
		    c__2, (ftnlen)6, (ftnlen)43);
	    ret_val = FALSE_;
	    return ret_val;
	}
	graph_1.graphp[22] = *buff;
    } else if (*indx == 9) {
	graph_1.graphp[20] = *buff;
    } else if (*indx == 10) {
	graph_1.graphp[36] = *buff;
    } else if (*indx == 12) {
	graph_1.graphp[31] = *buff;
    } else if (*indx == 13) {
	graph_1.graphp[90] = *buff;
    } else if (*indx == 14) {
	graph_1.graphp[89] = *buff;
    } else if (*indx == 15) {
	graph_1.graphp[34] = *buff;
    } else if (*indx == 16) {
	graph_1.graphp[35] = *buff;
    } else if (*indx == 17) {
	graph_1.graphp[38] = *buff;
    } else if (*indx == 18) {
	graph_1.graphp[39] = *buff;
    } else if (*indx == 19) {
	graph_1.graphp[40] = *buff;
    } else if (*indx == 20) {
	graph_1.graphp[41] = *buff;
    } else if (*indx == 21) {
	graph_1.graphp[91] = *buff;
    } else if (*indx == 22) {
	graph_1.graphp[43] = *buff;
    } else if (*indx == 23) {
	graph_1.graphp[44] = *buff;
    } else if (*indx == 47) {
	graph_1.graphp[87] = *buff;
    } else if (*indx == 48) {
	graph_1.graphp[88] = *buff;
    } else if (*indx == 24) {
	graph_1.graphp[45] = *buff;
    } else if (*indx == 25) {
	graph_1.graphp[47] = *buff;
    } else if (*indx == 26) {
	graph_1.graphp[48] = *buff;
    } else if (*indx == 28) {
	graph_1.graphp[61] = *buff;
    } else if (*indx == 29) {
	graph_1.graphp[62] = *buff;
    } else if (*indx == 30) {
	graph_1.graphp[63] = *buff;
    } else if (*indx == 31) {
	graph_1.graphp[64] = *buff;
    } else if (*indx == 32) {
	graph_1.graphp[65] = *buff;
    } else if (*indx == 33) {
	graph_1.graphp[66] = *buff;
    } else if (*indx == 34) {
	graph_1.graphp[67] = *buff;
    } else if (*indx == 35) {
	graph_1.graphp[68] = *buff;
    } else if (*indx == 36) {
	graph_1.graphp[69] = *buff;
    } else if (*indx == 37) {
	graph_1.graphp[70] = *buff;
    } else if (*indx == 38) {
	graph_1.graphp[71] = *buff;
    } else if (*indx == 39) {
	graph_1.graphp[72] = *buff;
    } else if (*indx == 43) {
	graph_1.graphp[73] = *buff;
    } else if (*indx == 44) {
	graph_1.graphp[74] = *buff;
    } else if (*indx == 45) {
	graph_1.graphp[75] = *buff;
    } else if (*indx == 46) {
	graph_1.graphp[76] = *buff;
    } else {
	chric_(indx, ierror, &l, (ftnlen)16);
	pltflu_();
/* Writing concatenation */
	i__1[0] = 14, a__1[0] = "Illegal index ";
	i__1[1] = l, a__1[1] = ierror;
	i__1[2] = 1, a__1[2] = ".";
	s_cat(ch__1, a__1, i__1, &c__3, (ftnlen)31);
	siorpt_("PLTSTG", ch__1, &c__2, (ftnlen)6, l + 15);
	ret_val = FALSE_;
	return ret_val;
    }
    return ret_val;
} /* pltstg1_ */

#ifdef __cplusplus
	}
#endif

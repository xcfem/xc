/* pltxts.f -- translated by f2c (version 20160102).
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

static integer c__1 = 1;
static real c_b4 = (float)1.;
static integer c__2 = 2;
static real c_b49 = (float)3.;
static real c_b51 = (float)0.;
static real c_b52 = (float).5;
static real c_b53 = (float)4.;
static real c_b58 = (float)2.;
static real c_b67 = (float)5.;
static real c_b76 = (float)6.;
static integer c__3 = 3;
static logical c_false = FALSE_;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltxts_(real *x, real *y, char *text, ftnlen text_len)
{
    /* System generated locals */
    char * a__1[3];
    integer i__1, i__2[3], i__3;
    char ch__1[72];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_cmp(char *, char *, ftnlen, ftnlen);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);
    integer i_len(char *, ftnlen);

    /* Local variables */
    static integer i__, j, l, li, jn;
    static char esc[20];
    static integer iesc, iflag, ascii;
    extern logical chrci_(char *, integer *, ftnlen);
    extern /* Subroutine */ int chric_(integer *, char *, integer *, ftnlen);
    static integer nchar, ifont;
    extern /* Subroutine */ int chrup_(char *, char *, ftnlen, ftnlen);
    static real ysave, ybump;
    extern /* Subroutine */ int pltdv2_(real *, integer *, real *, real *, 
	    real *, real *);
    static char escchr[1];
    extern logical cpuifc_(logical *);
    static char laschr[1];
    extern /* Subroutine */ int pltesc_(char *, integer *, char *, ftnlen, 
	    ftnlen);
    static integer novect;
    extern /* Subroutine */ int pltflu_(), pltrim_(char *, integer *, ftnlen),
	     pltsub_(real *, real *), pltrev_(), pltnor_(real *, real *), 
	    siorpt_(char *, char *, integer *, ftnlen, ftnlen), pltmov_(real *
	    , real *);
    static real ychrsz;
    static logical status;
    extern /* Subroutine */ int pltsup_(real *, real *), pltstv_(integer *, 
	    real *), pltsvv_();
    static char textip1[1];

    *(unsigned char *)escchr = '\\';
    ifont = 1;
    pltsvv_();
    pltstv_(&c__1, &c_b4);
    pltstv_(&c__2, &text_1.textp[36]);
    pltrim_(text, &nchar, text_len);
    text_1.textp[17] = *x;
    text_1.textp[18] = *y;
    iflag = 0;
    ysave = *y;
    ychrsz = text_1.textp[0];
    *(unsigned char *)laschr = 'M';
    i__ = 1;
L2020:
    if (! (i__ <= nchar)) {
	goto L2040;
    }
    ascii = *(unsigned char *)&text[i__ - 1];
    if (ascii < 1 || ascii > 126) {
	chric_(&ascii, laschr, &li, (ftnlen)1);
	pltflu_();
	return 0;
    }
    if (i__ < nchar) {
	i__1 = i__;
	s_copy(textip1, text + i__1, (ftnlen)1, i__ + 1 - i__1);
    } else {
	*(unsigned char *)textip1 = *(unsigned char *)escchr;
    }
    if (ascii == *(unsigned char *)escchr && *(unsigned char *)textip1 == *(
	    unsigned char *)escchr) {
	++i__;
    } else if (ascii == *(unsigned char *)escchr) {
	pltesc_(text, &i__, esc, text_len, (ftnlen)20);
	chrup_(esc, esc, (ftnlen)20, (ftnlen)20);
	if (s_cmp(esc, "^", (ftnlen)20, (ftnlen)1) == 0) {
	    if (iflag != 1) {
		if (iflag == -1) {
		    pltnor_(&ybump, &ychrsz);
		}
		pltsup_(&ybump, &ychrsz);
		iflag = 1;
		goto L2030;
	    }
	} else if (s_cmp(esc, "_", (ftnlen)20, (ftnlen)1) == 0) {
	    if (iflag != -1) {
		if (iflag == 1) {
		    pltnor_(&ybump, &ychrsz);
		}
		pltsub_(&ybump, &ychrsz);
	    }
	    iflag = -1;
	    goto L2030;
	} else if (s_cmp(esc, "-", (ftnlen)20, (ftnlen)1) == 0) {
	    if (iflag != 0) {
		pltnor_(&ybump, &ychrsz);
	    }
	    iflag = 0;
	    goto L2030;
	} else if (s_cmp(esc, "CLO", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 4;
	} else if (s_cmp(esc, "CSQ", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 5;
	} else if (s_cmp(esc, "CDI", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 6;
	} else if (s_cmp(esc, "CCS", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 7;
	} else if (s_cmp(esc, "CX", (ftnlen)20, (ftnlen)2) == 0) {
	    ascii = 8;
	} else if (s_cmp(esc, "CTR", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 9;
	} else if (s_cmp(esc, "CCI", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 10;
	} else if (s_cmp(esc, "CDO", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 11;
	} else if (s_cmp(esc, "LO", (ftnlen)20, (ftnlen)2) == 0) {
	    ascii = 12;
	} else if (s_cmp(esc, "SQ", (ftnlen)20, (ftnlen)2) == 0) {
	    ascii = 13;
	} else if (s_cmp(esc, "DI", (ftnlen)20, (ftnlen)2) == 0) {
	    ascii = 14;
	} else if (s_cmp(esc, "CS", (ftnlen)20, (ftnlen)2) == 0) {
	    ascii = 15;
	} else if (s_cmp(esc, "X", (ftnlen)20, (ftnlen)1) == 0) {
	    ascii = 16;
	} else if (s_cmp(esc, "TR", (ftnlen)20, (ftnlen)2) == 0) {
	    ascii = 17;
	} else if (s_cmp(esc, "CI", (ftnlen)20, (ftnlen)2) == 0) {
	    ascii = 18;
	} else if (s_cmp(esc, "DO", (ftnlen)20, (ftnlen)2) == 0) {
	    ascii = 19;
	} else if (s_cmp(esc, "PLUSMIN", (ftnlen)20, (ftnlen)7) == 0) {
	    ascii = 20;
	} else if (s_cmp(esc, "LEQ", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 21;
	} else if (s_cmp(esc, "GEQ", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 22;
	} else if (s_cmp(esc, "NEQ", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 23;
	} else if (s_cmp(esc, "PRIME", (ftnlen)20, (ftnlen)5) == 0) {
	    ascii = 24;
	} else if (s_cmp(esc, "NLEQ", (ftnlen)20, (ftnlen)4) == 0) {
	    ascii = 25;
	} else if (s_cmp(esc, "NGEQ", (ftnlen)20, (ftnlen)4) == 0) {
	    ascii = 26;
	} else if (s_cmp(esc, "LL", (ftnlen)20, (ftnlen)2) == 0) {
	    ascii = 27;
	} else if (s_cmp(esc, "GG", (ftnlen)20, (ftnlen)2) == 0) {
	    ascii = 28;
	} else if (s_cmp(esc, "SUM", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 29;
	} else if (s_cmp(esc, "NLT", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 30;
	} else if (s_cmp(esc, "NGT", (ftnlen)20, (ftnlen)3) == 0) {
	    ascii = 31;
	} else if (s_cmp(esc, "APPROX", (ftnlen)20, (ftnlen)6) == 0) {
	    ascii = 127;
	} else if (s_cmp(esc, "CR", (ftnlen)20, (ftnlen)2) == 0) {
	    text_1.textp[17] = *x + text_1.textp[29] * text_1.textp[0] * 
		    text_1.textp[28];
	    goto L2030;
	} else if (s_cmp(esc, "LF", (ftnlen)20, (ftnlen)2) == 0) {
	    text_1.textp[18] = ysave - text_1.textp[29] * text_1.textp[0] * 
		    text_1.textp[27];
	    ysave = text_1.textp[18];
	    goto L2030;
	} else if (s_cmp(esc, "CL", (ftnlen)20, (ftnlen)2) == 0) {
	    text_1.textp[17] = *x + text_1.textp[29] * text_1.textp[0] * 
		    text_1.textp[28];
	    text_1.textp[18] = ysave - text_1.textp[29] * text_1.textp[0] * 
		    text_1.textp[27];
	    ysave = text_1.textp[18];
	    goto L2030;
	} else if (s_cmp(esc, "ENG", (ftnlen)20, (ftnlen)3) == 0) {
	    ifont = 1;
	    goto L2030;
	} else if (s_cmp(esc, "GR", (ftnlen)20, (ftnlen)2) == 0) {
	    ifont = 2;
	    goto L2030;
	} else if (s_cmp(esc, "DDLINE", (ftnlen)20, (ftnlen)6) == 0) {
	    pltsvv_();
	    pltstv_(&c__1, &c_b49);
	    pltdv2_(&text_1.textp[13], &c__1, &c_b51, &c_b52, &c_b53, &c_b52);
	    text_1.textp[17] += text_1.textp[27] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    text_1.textp[18] += text_1.textp[28] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    *(unsigned char *)laschr = 'M';
	    pltrev_();
	    goto L2030;
	} else if (s_cmp(esc, "DLINE", (ftnlen)20, (ftnlen)5) == 0) {
	    pltsvv_();
	    pltstv_(&c__1, &c_b58);
	    pltdv2_(&text_1.textp[13], &c__1, &c_b51, &c_b52, &c_b53, &c_b52);
	    text_1.textp[17] += text_1.textp[27] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    text_1.textp[18] += text_1.textp[28] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    *(unsigned char *)laschr = 'M';
	    pltrev_();
	    goto L2030;
	} else if (s_cmp(esc, "LDLINE", (ftnlen)20, (ftnlen)6) == 0) {
	    pltsvv_();
	    pltstv_(&c__1, &c_b67);
	    pltdv2_(&text_1.textp[13], &c__1, &c_b51, &c_b52, &c_b53, &c_b52);
	    text_1.textp[17] += text_1.textp[27] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    text_1.textp[18] += text_1.textp[28] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    *(unsigned char *)laschr = 'M';
	    pltrev_();
	    goto L2030;
	} else if (s_cmp(esc, "MDLINE", (ftnlen)20, (ftnlen)6) == 0) {
	    pltsvv_();
	    pltstv_(&c__1, &c_b76);
	    pltdv2_(&text_1.textp[13], &c__1, &c_b51, &c_b52, &c_b53, &c_b52);
	    text_1.textp[17] += text_1.textp[27] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    text_1.textp[18] += text_1.textp[28] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    *(unsigned char *)laschr = 'M';
	    pltrev_();
	    goto L2030;
	} else if (s_cmp(esc, "SDLINE", (ftnlen)20, (ftnlen)6) == 0) {
	    pltsvv_();
	    pltstv_(&c__1, &c_b53);
	    pltdv2_(&text_1.textp[13], &c__1, &c_b51, &c_b52, &c_b53, &c_b52);
	    text_1.textp[17] += text_1.textp[27] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    text_1.textp[18] += text_1.textp[28] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    *(unsigned char *)laschr = 'M';
	    pltrev_();
	    goto L2030;
	} else if (s_cmp(esc, "SLINE", (ftnlen)20, (ftnlen)5) == 0) {
	    pltsvv_();
	    pltstv_(&c__1, &c_b4);
	    pltdv2_(&text_1.textp[13], &c__1, &c_b51, &c_b52, &c_b53, &c_b52);
	    text_1.textp[17] += text_1.textp[27] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    text_1.textp[18] += text_1.textp[28] * (float)4. * ychrsz * 
		    text_1.textp[30];
	    *(unsigned char *)laschr = 'M';
	    pltrev_();
	    goto L2030;
	} else {
	    status = chrci_(esc, &iesc, (ftnlen)20);
	    if (status) {
		ascii = iesc;
	    } else {
		pltrim_(esc, &l, (ftnlen)20);
		pltflu_();
/* Writing concatenation */
		i__2[0] = 25, a__1[0] = "Invalid escape sequence \"";
		i__2[1] = l, a__1[1] = esc;
		i__2[2] = 27, a__1[2] = "\"; escape sequence ignored.";
		s_cat(ch__1, a__1, i__2, &c__3, (ftnlen)72);
		siorpt_("PLTXTS", ch__1, &c__2, (ftnlen)6, l + 52);
		goto L2030;
	    }
	}
    }
    novect = font_1.nvect[ascii + ifont * 200 - 201];
    j = 0;
L2050:
    if (! (j < novect)) {
	goto L2060;
    }
/* Computing MIN */
    i__1 = 32, i__3 = novect - j;
    jn = int_min(i__1,i__3);
    pltdv2_(&text_1.textp[13], &jn, &font_1.x0[font_1.idex[ascii + ifont * 
	    200 - 201] + j + ifont * 2300 - 2301], &font_1.y0[font_1.idex[
	    ascii + ifont * 200 - 201] + j + ifont * 2300 - 2301], &font_1.x1[
	    font_1.idex[ascii + ifont * 200 - 201] + j + ifont * 2300 - 2301],
	     &font_1.y1[font_1.idex[ascii + ifont * 200 - 201] + j + ifont * 
	    2300 - 2301]);
    j += jn;
    goto L2050;
L2060:
    text_1.textp[17] += font_1.xsize[ascii + ifont * 200 - 201] * 
	    text_1.textp[27] * ychrsz * text_1.textp[30];
    text_1.textp[18] += font_1.xsize[ascii + ifont * 200 - 201] * 
	    text_1.textp[28] * ychrsz * text_1.textp[30];
    if (i__ <= i_len(text, text_len)) {
	*(unsigned char *)laschr = *(unsigned char *)&text[i__ - 1];
    }
    if (cpuifc_(&c_false)) {
	goto L2040;
    }
L2030:
    ++i__;
    goto L2020;
L2040:
    pltmov_(x, y);
    text_1.textp[7] = text_1.textp[17];
    text_1.textp[8] = text_1.textp[18];
    text_1.textp[9] = *x;
    text_1.textp[10] = ysave;
    if (iflag != 0) {
	for (i__ = 14; i__ <= 17; ++i__) {
	    text_1.textp[i__ - 1] /= text_1.textp[31];
/* L2070: */
	}
    }
    pltrev_();
    return 0;
} /* pltxts_ */

#ifdef __cplusplus
	}
#endif

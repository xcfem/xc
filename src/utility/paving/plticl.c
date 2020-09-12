/* plticl.f -- translated by f2c (version 20160102).
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

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical plticl_(char *clr, real *val, ftnlen clr_len)
{
    /* System generated locals */
    logical ret_val;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static char icol[16];
    extern /* Subroutine */ int chrup_(char *, char *, ftnlen, ftnlen);
    extern logical chrcmp_(char *, char *, char *, ftnlen, ftnlen, ftnlen);

    ret_val = TRUE_;
    s_copy(icol, "                ", (ftnlen)16, (ftnlen)16);
    chrup_(clr, icol, clr_len, (ftnlen)16);
    if (chrcmp_(icol, "BLA", "CK", (ftnlen)16, (ftnlen)3, (ftnlen)2)) {
	*val = (float)0.;
    } else if (chrcmp_(icol, "RED", " ", (ftnlen)16, (ftnlen)3, (ftnlen)1)) {
	*val = (float)1.;
    } else if (chrcmp_(icol, "GRE", "EN", (ftnlen)16, (ftnlen)3, (ftnlen)2)) {
	*val = (float)2.;
    } else if (chrcmp_(icol, "YEL", "LOW", (ftnlen)16, (ftnlen)3, (ftnlen)3)) 
	    {
	*val = (float)3.;
    } else if (chrcmp_(icol, "BLU", "E", (ftnlen)16, (ftnlen)3, (ftnlen)1)) {
	*val = (float)4.;
    } else if (chrcmp_(icol, "MAG", "ENTA", (ftnlen)16, (ftnlen)3, (ftnlen)4))
	     {
	*val = (float)5.;
    } else if (chrcmp_(icol, "CYA", "N", (ftnlen)16, (ftnlen)3, (ftnlen)1)) {
	*val = (float)6.;
    } else if (chrcmp_(icol, "WHI", "TE", (ftnlen)16, (ftnlen)3, (ftnlen)2)) {
	*val = (float)7.;
    } else if (chrcmp_(icol, "P1", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (float)8.;
    } else if (chrcmp_(icol, "P2", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (float)9.;
    } else if (chrcmp_(icol, "P3", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (float)10.;
    } else if (chrcmp_(icol, "P4", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (float)11.;
    } else if (chrcmp_(icol, "P5", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (float)12.;
    } else if (chrcmp_(icol, "P6", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (float)13.;
    } else if (chrcmp_(icol, "P7", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (float)14.;
    } else if (chrcmp_(icol, "P8", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (float)15.;
    } else if (chrcmp_(icol, "GR", "AY", (ftnlen)16, (ftnlen)2, (ftnlen)2)) {
	*val = (float)8.;
    } else if (chrcmp_(icol, "LTG", "RAY", (ftnlen)16, (ftnlen)3, (ftnlen)3)) 
	    {
	*val = (float)9.;
    } else if (chrcmp_(icol, "DKG", "RAY", (ftnlen)16, (ftnlen)3, (ftnlen)3)) 
	    {
	*val = (float)10.;
    } else if (chrcmp_(icol, "PI", "NK", (ftnlen)16, (ftnlen)2, (ftnlen)2)) {
	*val = (float)11.;
    } else if (chrcmp_(icol, "LI", "ME", (ftnlen)16, (ftnlen)2, (ftnlen)2)) {
	*val = (float)12.;
    } else if (chrcmp_(icol, "LTB", "LUE", (ftnlen)16, (ftnlen)3, (ftnlen)3)) 
	    {
	*val = (float)13.;
    } else if (chrcmp_(icol, "VIO", "LET", (ftnlen)16, (ftnlen)3, (ftnlen)3)) 
	    {
	*val = (float)14.;
    } else if (chrcmp_(icol, "OR", "ANGE", (ftnlen)16, (ftnlen)2, (ftnlen)4)) 
	    {
	*val = (float)15.;
    } else {
	ret_val = FALSE_;
	*val = (float)7.;
    }
    return ret_val;
} /* plticl_ */

#ifdef __cplusplus
	}
#endif

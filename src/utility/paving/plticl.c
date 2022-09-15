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

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical plticl_(char *clr, xc_float *val, ftnlen clr_len)
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
	*val = (xc_float)0.;
    } else if (chrcmp_(icol, "RED", " ", (ftnlen)16, (ftnlen)3, (ftnlen)1)) {
	*val = (xc_float)1.;
    } else if (chrcmp_(icol, "GRE", "EN", (ftnlen)16, (ftnlen)3, (ftnlen)2)) {
	*val = (xc_float)2.;
    } else if (chrcmp_(icol, "YEL", "LOW", (ftnlen)16, (ftnlen)3, (ftnlen)3)) 
	    {
	*val = (xc_float)3.;
    } else if (chrcmp_(icol, "BLU", "E", (ftnlen)16, (ftnlen)3, (ftnlen)1)) {
	*val = (xc_float)4.;
    } else if (chrcmp_(icol, "MAG", "ENTA", (ftnlen)16, (ftnlen)3, (ftnlen)4))
	     {
	*val = (xc_float)5.;
    } else if (chrcmp_(icol, "CYA", "N", (ftnlen)16, (ftnlen)3, (ftnlen)1)) {
	*val = (xc_float)6.;
    } else if (chrcmp_(icol, "WHI", "TE", (ftnlen)16, (ftnlen)3, (ftnlen)2)) {
	*val = (xc_float)7.;
    } else if (chrcmp_(icol, "P1", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (xc_float)8.;
    } else if (chrcmp_(icol, "P2", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (xc_float)9.;
    } else if (chrcmp_(icol, "P3", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (xc_float)10.;
    } else if (chrcmp_(icol, "P4", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (xc_float)11.;
    } else if (chrcmp_(icol, "P5", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (xc_float)12.;
    } else if (chrcmp_(icol, "P6", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (xc_float)13.;
    } else if (chrcmp_(icol, "P7", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (xc_float)14.;
    } else if (chrcmp_(icol, "P8", " ", (ftnlen)16, (ftnlen)2, (ftnlen)1)) {
	*val = (xc_float)15.;
    } else if (chrcmp_(icol, "GR", "AY", (ftnlen)16, (ftnlen)2, (ftnlen)2)) {
	*val = (xc_float)8.;
    } else if (chrcmp_(icol, "LTG", "RAY", (ftnlen)16, (ftnlen)3, (ftnlen)3)) 
	    {
	*val = (xc_float)9.;
    } else if (chrcmp_(icol, "DKG", "RAY", (ftnlen)16, (ftnlen)3, (ftnlen)3)) 
	    {
	*val = (xc_float)10.;
    } else if (chrcmp_(icol, "PI", "NK", (ftnlen)16, (ftnlen)2, (ftnlen)2)) {
	*val = (xc_float)11.;
    } else if (chrcmp_(icol, "LI", "ME", (ftnlen)16, (ftnlen)2, (ftnlen)2)) {
	*val = (xc_float)12.;
    } else if (chrcmp_(icol, "LTB", "LUE", (ftnlen)16, (ftnlen)3, (ftnlen)3)) 
	    {
	*val = (xc_float)13.;
    } else if (chrcmp_(icol, "VIO", "LET", (ftnlen)16, (ftnlen)3, (ftnlen)3)) 
	    {
	*val = (xc_float)14.;
    } else if (chrcmp_(icol, "OR", "ANGE", (ftnlen)16, (ftnlen)2, (ftnlen)4)) 
	    {
	*val = (xc_float)15.;
    } else {
	ret_val = FALSE_;
	*val = (xc_float)7.;
    }
    return ret_val;
} /* plticl_ */

#ifdef __cplusplus
	}
#endif

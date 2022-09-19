/* pltxth.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int pltxth_(xc_float *x, xc_float *y, char *text, ftnlen text_len)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, l, ic, nc, inchar[133];
    extern /* Subroutine */ int vdiqcp_(xc_float *, xc_float *), pltrim_(char *, 
	    integer *, ftnlen), vdtext_(integer *, integer *), pltmov_(xc_float *,
	     xc_float *);

    pltrim_(text, &l, text_len);
    if (l <= 0) {
	return 0;
    }
    nc = 0;
    i__1 = l;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ic = *(unsigned char *)&text[i__ - 1];
	if (ic != 0) {
	    ++nc;
	    inchar[nc - 1] = ic;
	}
/* L2000: */
    }
    if (nc == 0) {
	return 0;
    }
    pltmov_(x, y);
    vdtext_(&nc, inchar);
    vdiqcp_(&text_1.textp[3], &text_1.textp[4]);
    pltmov_(x, y);
    text_1.textp[5] = *x;
    text_1.textp[6] = *y;
    return 0;
} /* pltxth_ */

#ifdef __cplusplus
	}
#endif

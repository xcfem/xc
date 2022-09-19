/* pltgtt.f -- translated by f2c (version 20160102).
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
static integer c__3 = 3;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical pltgtt_(integer *indx, xc_float *buff)
{
    /* System generated locals */
    char * a__1[3];
    integer i__1[3];
    logical ret_val;
    char ch__1[31];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer i__, l;
    extern /* Subroutine */ int chric_(integer *, char *, integer *, ftnlen);
    static char ierror[16];
    extern /* Subroutine */ int pltflu_(), siorpt_(char *, char *, integer *, 
	    ftnlen, ftnlen);

    /* Parameter adjustments */
    --buff;

    /* Function Body */
    ret_val = TRUE_;
    if (*indx == 1) {
	buff[1] = text_1.textp[34];
    } else if (*indx == 2) {
	buff[1] = text_1.textp[0];
    } else if (*indx == 3) {
	buff[1] = text_1.textp[1];
    } else if (*indx == 4) {
	buff[1] = text_1.textp[2];
    } else if (*indx == 5) {
	for (i__ = 20; i__ <= 27; ++i__) {
	    buff[i__ - 19] = text_1.textp[i__ - 1];
/* L2320: */
	}
    } else if (*indx == 6) {
	buff[1] = text_1.textp[29];
    } else if (*indx == 7) {
	buff[1] = text_1.textp[30];
    } else if (*indx == 8) {
	buff[1] = text_1.textp[31];
    } else if (*indx == 9) {
	buff[1] = text_1.textp[32];
    } else if (*indx == 10) {
	buff[1] = text_1.textp[33];
    } else if (*indx == 11) {
	buff[1] = text_1.textp[36];
    } else if (*indx == 12) {
	buff[1] = text_1.textp[37];
    } else {
	chric_(indx, ierror, &l, (ftnlen)16);
	pltflu_();
/* Writing concatenation */
	i__1[0] = 14, a__1[0] = "Illegal index ";
	i__1[1] = l, a__1[1] = ierror;
	i__1[2] = 1, a__1[2] = ".";
	s_cat(ch__1, a__1, i__1, &c__3, (ftnlen)31);
	siorpt_("PLTGTT", ch__1, &c__2, (ftnlen)6, l + 15);
	ret_val = FALSE_;
	return ret_val;
    }
    return ret_val;
} /* pltgtt_ */

#ifdef __cplusplus
	}
#endif

/* pltstv.f -- translated by f2c (version 20160102).
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
logical pltstv_(integer *indx, xc_float *buff)
{
    /* Initialized data */

    static xc_float zzzls = (xc_float)-1.;

    /* System generated locals */
    char * a__1[3];
    integer i__1[3];
    xc_float r__1;
    logical ret_val;
    char ch__1[31];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer l;
    extern /* Subroutine */ int chric_(integer *, char *, integer *, ftnlen);
    static integer ibufft;
    static char ierror[16];
    extern /* Subroutine */ int pltflu_(), vdstls_(integer *), siorpt_(char *,
	     char *, integer *, ftnlen, ftnlen), vdstlw_(xc_float *), pltrsv_();

    /* Parameter adjustments */
    --buff;

    /* Function Body */
    ret_val = TRUE_;
    if (*indx == 0) {
	pltrsv_();
    } else if (*indx == 1) {
	if (zzzls == buff[1]) {
	    return ret_val;
	}
	zzzls = buff[1];
	vectrc_1.vectp[0] = buff[1];
	if (buff[1] != (xc_float)0.) {
	    ibufft = (integer) buff[1] - 1;
	    vdstls_(&ibufft);
	}
    } else if (*indx == 2) {
	r__1 = buff[1] / (xc_float)1e3;
	vdstlw_(&r__1);
	vectrc_1.vectp[1] = buff[1];
    } else {
	chric_(indx, ierror, &l, (ftnlen)16);
	pltflu_();
/* Writing concatenation */
	i__1[0] = 14, a__1[0] = "Illegal index ";
	i__1[1] = l, a__1[1] = ierror;
	i__1[2] = 1, a__1[2] = ".";
	s_cat(ch__1, a__1, i__1, &c__3, (ftnlen)31);
	siorpt_("PLTSTV", ch__1, &c__2, (ftnlen)6, l + 15);
	ret_val = FALSE_;
	return ret_val;
    }
    return ret_val;
} /* pltstv_ */

#ifdef __cplusplus
	}
#endif

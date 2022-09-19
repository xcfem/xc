/* pltstd.f -- translated by f2c (version 20160102).
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
logical pltstd_(integer *indx, xc_float *buff)
{
    /* Initialized data */

    static xc_float zzzfc = (xc_float)-1.;
    static xc_float zzzbc = (xc_float)-1.;
    static xc_float zzzin = (xc_float)-1.;

    /* System generated locals */
    char * a__1[3];
    integer i__1, i__2[3];
    xc_float r__1;
    logical ret_val;
    char ch__1[31];

    /* Builtin functions */
    integer i_nint(xc_float *);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer j, l;
    extern /* Subroutine */ int chric_(integer *, char *, integer *, ftnlen), 
	    vdstbc_(integer *), vdstfc_(integer *);
    static char ierror[16];
    extern /* Subroutine */ int pltflu_(), vdstin_(xc_float *), pltrsd_(), 
	    siorpt_(char *, char *, integer *, ftnlen, ftnlen);

    /* Parameter adjustments */
    --buff;

    /* Function Body */
    ret_val = TRUE_;
    if (*indx == 0) {
	pltrsd_();
    } else if (*indx == 1) {
	if (zzzfc == buff[1]) {
	    return ret_val;
	}
	zzzfc = buff[1];
	j = i_nint(&buff[1]);
	vdstfc_(&j);
	device_1.devp[0] = buff[1];
    } else if (*indx == 2) {
	if (zzzbc == buff[1]) {
	    return ret_val;
	}
	zzzbc = buff[1];
	i__1 = (integer) buff[1];
	vdstbc_(&i__1);
	device_1.devp[1] = buff[1];
    } else if (*indx == 3) {
	if (zzzin == buff[1]) {
	    return ret_val;
	}
	zzzin = buff[1];
	r__1 = buff[1] / (xc_float)100.;
	vdstin_(&r__1);
	device_1.devp[2] = buff[1];
    } else {
	chric_(indx, ierror, &l, (ftnlen)16);
	pltflu_();
/* Writing concatenation */
	i__2[0] = 14, a__1[0] = "Illegal index ";
	i__2[1] = l, a__1[1] = ierror;
	i__2[2] = 1, a__1[2] = ".";
	s_cat(ch__1, a__1, i__2, &c__3, (ftnlen)31);
	siorpt_("PLTSTD", ch__1, &c__2, (ftnlen)6, l + 15);
	ret_val = FALSE_;
	return ret_val;
    }
    return ret_val;
} /* pltstd_ */

#ifdef __cplusplus
	}
#endif

/* lcolor.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__1 = 1;
static real c_b4 = (float)7.;
static real c_b7 = (float)0.;
static real c_b10 = (float)3.;
static real c_b13 = (float)5.;
static real c_b16 = (float)1.;
static real c_b19 = (float)4.;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int lcolor_(char *color, ftnlen color_len)
{
    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    extern /* Subroutine */ int pltstd_(integer *, real *);

/* *********************************************************************** */
/*  SUBROUTINE LCOLOR = SETS THE LINE COLOR */
/* *********************************************************************** */
    if (s_cmp(color, "WHITE", (ftnlen)5, (ftnlen)5) == 0) {
	pltstd_(&c__1, &c_b4);
    } else if (s_cmp(color, "BLACK", (ftnlen)5, (ftnlen)5) == 0) {
	pltstd_(&c__1, &c_b7);
    } else if (s_cmp(color, "YELOW", (ftnlen)5, (ftnlen)5) == 0) {
	pltstd_(&c__1, &c_b10);
    } else if (s_cmp(color, "PINK ", (ftnlen)5, (ftnlen)5) == 0) {
	pltstd_(&c__1, &c_b13);
    } else if (s_cmp(color, "RED  ", (ftnlen)5, (ftnlen)5) == 0) {
	pltstd_(&c__1, &c_b16);
    } else if (s_cmp(color, "BLUE ", (ftnlen)5, (ftnlen)5) == 0) {
	pltstd_(&c__1, &c_b19);
    }
    return 0;
} /* lcolor_ */

#ifdef __cplusplus
	}
#endif

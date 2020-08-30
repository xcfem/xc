/* d2grid.f -- translated by f2c (version 20160102).
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
#include "fastq_cpp.h"

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int d2grid_(real *x1, real *y1, real *x2, real *y2)
{
    static real x[2], y[2];
    extern /* Subroutine */ int mpd2vc_(integer *, real *, real *, real *, 
	    real *);

/* *********************************************************************** */
/*  SUBROUTINE D2GRID = DRAWS A LINE BETWEEN TWO GRIDS */
/* *********************************************************************** */
    x[0] = *x1;
    x[1] = *x2;
    y[0] = *y1;
    y[1] = *y2;
    mpd2vc_(&c__1, x, y, &x[1], &y[1]);
    return 0;
} /* d2grid_ */

#ifdef __cplusplus
	}
#endif

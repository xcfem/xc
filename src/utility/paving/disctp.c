/* disctp.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
logical disctp_(real *angle)
{
    /* Initialized data */

    static real eps = (float).31;

    /* System generated locals */
    logical ret_val;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static real pi;

/* *********************************************************************** */
/*  FUNCTION DISCTP = LOGICAL FUNCTION THAT RETURNS TRUE IF THE ANGLE IS */
/*                    WITHIN THE CURRENT DEFINITION OF A */
/*                    DISSECTION VERTEX */
/* *********************************************************************** */
    pi = atan2((float)0., (float)-1.);
    if (*angle > pi + eps) {
	ret_val = TRUE_;
    } else {
	ret_val = FALSE_;
    }
    return ret_val;
} /* disctp_ */

#ifdef __cplusplus
	}
#endif

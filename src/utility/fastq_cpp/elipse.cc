/* elipse.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
doublereal elipse_(real *a7, real *a8, real *a2, real *ang)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sin(doublereal);

    /* Local variables */
    static real a3, a4, a5, a6, pi;

/* *********************************************************************** */
/*  FUNCTION ELIPSE = CALCULATES THE ANGULAR EQUATION ERROR WHEN FINDING */
/*                    AN ELIPSE PATTERN */
/* *********************************************************************** */
    pi = atan2((float)0., (float)-1.);
    a4 = *a8 - *ang;
    a5 = *a7 - *ang;
    a3 = *a2 - a4;
    a6 = pi - a5 - *a2;
    ret_val = sin(a4) * sin(a6) - sin(a5) * sin(a3);
    return ret_val;
} /* elipse_ */

#ifdef __cplusplus
	}
#endif

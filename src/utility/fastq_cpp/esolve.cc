/* esolve.f -- translated by f2c (version 20160102).
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
doublereal esolve_(real *a7, real *a8, real *a2, real *ang1, real *ang2)
{
    /* System generated locals */
    real ret_val, r__1;

    /* Local variables */
    static real f1, f2, f3, eps, ang3;
    extern doublereal elipse_(real *, real *, real *, real *);

/* *********************************************************************** */
/*  FUNCTION ESOLVE = FINDS A SOLUTION TO THE ELIPSE EQUATION */
/*                    GIVEN AN INTERVAL THAT CONTAINS THE SOLUTION */
/* *********************************************************************** */
    eps = (float)1e-6;
    f1 = elipse_(a7, a8, a2, ang1);
    if (dabs(f1) < eps) {
	ret_val = *ang1;
	goto L110;
    }
    f2 = elipse_(a7, a8, a2, ang2);
    if (dabs(f2) < eps) {
	ret_val = *ang2;
	goto L110;
    }
L100:
    if ((r__1 = *ang1 - *ang2, dabs(r__1)) < eps) {
	ret_val = (*ang1 + *ang2) / (float)2.;
    } else {
	ang3 = (*ang1 + *ang2) / (float)2.;
	f3 = elipse_(a7, a8, a2, &ang3);
	if (dabs(f3) < eps) {
	    ret_val = ang3;
	    goto L110;
	} else if (f1 / f3 < (float)0.) {
	    *ang2 = ang3;
	    f2 = f3;
	} else {
	    *ang1 = ang3;
	    f1 = f3;
	}
	goto L100;
    }
L110:
    return ret_val;
} /* esolve_ */

#ifdef __cplusplus
	}
#endif

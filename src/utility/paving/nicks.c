/* nicks.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
doublereal nicks_(real *angle, integer *lxn)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static real pi, adiff;

/* *********************************************************************** */
/*  FUNCTION NICKS = RETURNS THE PENALTY FOR A BAD ANGLE AND A BAD */
/*                   CONNECTIVITY ON A SIDE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --lxn;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    adiff = *angle - pi;
/*  PENALIZE A LARGE ANGLE MORE THAN A SMALL ANGLE */
    if (adiff < (float)0.) {
	adiff = -adiff;
    } else {
	adiff *= (float)1.2;
    }
/*  IF THE ANGLE HAS 3 LINES ATTACHED */
/*  A REGULAR NODE WOULD BE FORMED - PENALIZE IT LIGHTLY */
/*  USE THIS SAME PENALTY FOR BOUNDARY NODES */
    if (lxn[4] == 0 && lxn[3] != 0 || lxn[2] < 0) {
	ret_val = adiff;
/*  IF THE ANGLE HAS 2 LINES ATTACHED */
/*  THEN A THREE DEGREE IRREGULAR NODE WOULD BE FORMED */
/*  PENALIZE IT MORE STRINGENTLY */
    } else if (lxn[3] == 0) {
	ret_val = adiff * (float)1.3;
/*  IF THE ANGLE HAS 4 LINES ATTACHED */
/*  THEN A FIVE DEGREE IRREGULAR NODE WOULD BE FORMED */
/*  PENALIZE IT LESS STRINGENTLY */
    } else if (lxn[4] > 0) {
	ret_val = adiff * (float)1.15;
/*  IF THE ANGLE HAS MORE THAN 4 LINES ATTACHED */
/*  THEN A FIVE+ DEGREE IRREGULAR NODE WOULD BE FORMED (HIGHLY UNLIKELY) */
/*  PENALIZE IT SEVERELY */
    } else if (lxn[3] == 0) {
	ret_val = adiff * (float)1.6;
    } else {
/*         write (*,*) 'Undefined Option in nicks', */
/*     $        adiff, lxn(1), lxn(2), lxn(3), lxn(4) */
	ret_val = adiff * (float)2.;
    }
    return ret_val;
} /* nicks_ */

#ifdef __cplusplus
	}
#endif

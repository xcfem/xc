/* fq_check.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int check_(integer *min__, integer *max__, integer *i__)
{
/* *********************************************************************** */
/*  SUBROUTINE CHECK = CHECKS 2 VALUES FOR BEING OUT OF PRESCRIBED BOUNDS */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     LIST   = LISTS POINTS,  LINES,  AND REGIONS USED IN MESH DEFINITION */
/*     ERASE  = DELETES POINTS,  LINES,  AND REGIONS FROM THE MESH */
/*              DEFINITIONS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     MIN   = MINIMUM VALUE TO BE TESTED */
/*     MAX   = MAXIMUM VALUE TO BE TESTED */
/*     I     = THE ABSOLUTE MAXIMUM VALUE ALLOWED  (THE MINIMUM IS 1) */
/* ************************************************************************ */
    if (*min__ < 1) {
	*min__ = 1;
    }
    if (*max__ > *i__) {
	*max__ = *i__;
    }
    if (*max__ < *min__) {
	*max__ = *min__;
    }
    return 0;
} /* check_ */

#ifdef __cplusplus
	}
#endif

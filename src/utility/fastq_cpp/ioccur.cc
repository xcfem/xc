/* ioccur.f -- translated by f2c (version 20160102).
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
integer ioccur_(integer *n, integer *l, integer *new__)
{
    /* System generated locals */
    integer ret_val, i__1;

    /* Local variables */
    static integer i__;

/* *********************************************************************** */
/*  FUNCTION IOCCUR = CHECKS TO SEE IF NEW OCCURS IN  (L (I), I=1, N) */
/* *********************************************************************** */
/*     RETURN 0 IF NEW DOES NOT OCCUR IN  (L (I), I=1, N) */
/*     RETURN 1 IF IT DOES */
/* *********************************************************************** */
    /* Parameter adjustments */
    --l;

    /* Function Body */
    if (*n < 1) {
	ret_val = 0;
	return ret_val;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (l[i__] == *new__) {
	    ret_val = 1;
	    return ret_val;
	}
/* L100: */
    }
    ret_val = 0;
    return ret_val;
} /* ioccur_ */

#ifdef __cplusplus
	}
#endif

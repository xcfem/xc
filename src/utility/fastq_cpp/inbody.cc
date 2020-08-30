/* inbody.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inbody_(integer *mr, integer *n9, integer *iin, integer *
	ifound, integer *irpb, logical *addold, logical *noroom)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j, jj;

/* *********************************************************************** */
/*  SUBROUTINE INBODY = INPUTS A BODY LIST INTO THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --irpb;
    --iin;

    /* Function Body */
    *noroom = TRUE_;
    if (! (*addold)) {
	*n9 = 0;
    }
    i__1 = *ifound;
    for (i__ = 1; i__ <= i__1; ++i__) {
	jj = iin[i__];
	if (jj == 0) {
	    goto L130;
	}
	if (*n9 + 1 > *mr) {
	    return 0;
	}
/*  SEE IF THE REGION IS ALREADY IN THE BODY LIST */
	i__2 = *n9;
	for (j = 1; j <= i__2; ++j) {
	    if (irpb[j] == jj) {
		goto L110;
	    }
/* L100: */
	}
	++(*n9);
	irpb[*n9] = jj;
L110:
/* L120: */
	;
    }
L130:
    *noroom = FALSE_;
    return 0;
} /* inbody_ */

#ifdef __cplusplus
	}
#endif

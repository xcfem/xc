/* keep34.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int keep34_(integer *itest, integer *ltest, integer *nbegin, 
	integer *nend, integer *ichng)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer mmax;

/* *********************************************************************** */
/*  SUBROTINE KEEP34 = GETS AN ACCEPTABLE SIDE FOR FILLING TO KEEP A */
/*                     TRIANGLE VALID OR CHANGING TO A RECTANGLE */
/* *********************************************************************** */
/*  MAKE SURE THAT THE NBEGIN STARTS AT ONE OF THE CORNERS */
    /* Parameter adjustments */
    --ltest;
    --itest;

    /* Function Body */
    if (*nbegin == itest[1]) {
	*nend = itest[2];
    } else if (*nbegin == itest[2]) {
	*nend = itest[3];
    } else if (*nbegin == itest[3]) {
	*nend = itest[1];
    } else {
	*nbegin = itest[1];
	*nend = itest[3];
    }
/*  FIND THE CORRECT ROW (THIS ALREADY ASSUMES THAT THE */
/*  SUM OF THE SMALLER TWO IS EQUAL TO THE LARGEST ONE) */
/* Computing MAX */
    i__1 = max(ltest[1],ltest[2]);
    mmax = max(i__1,ltest[3]);
    if (ltest[1] == mmax) {
	if (*nbegin == itest[1]) {
	    *nend = *ichng;
	}
    } else if (ltest[2] == mmax) {
	if (*nbegin == itest[2]) {
	    *nend = *ichng;
	}
    } else {
	if (*nbegin == itest[3]) {
	    *nend = *ichng;
	}
    }
    return 0;
} /* keep34_ */

#ifdef __cplusplus
	}
#endif

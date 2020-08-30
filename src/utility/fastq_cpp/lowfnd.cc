/* lowfnd.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int lowfnd_(integer *mxnd, integer *nuid, integer *n, 
	integer *indx, integer *i__, integer *iold)
{
    static integer ii, ibot, itop;

/* *********************************************************************** */
/*  SUBROUTINE LOWFND  =  LINEAR INDEXED SEARCH FOR MATCHING NUID VALUES */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nuid;
    --indx;

    /* Function Body */
    ibot = 1;
    itop = *n;
L100:
    ii = (ibot + itop) / 2;
    if (nuid[indx[ii]] == nuid[*i__]) {
	*iold = indx[ii];
	return 0;
    } else if (nuid[indx[ii]] > nuid[*i__]) {
	itop = ii - 1;
    } else {
	ibot = ii + 1;
    }
    if (ibot <= itop) {
	goto L100;
    }
    *iold = 0;
    return 0;
} /* lowfnd_ */

#ifdef __cplusplus
	}
#endif

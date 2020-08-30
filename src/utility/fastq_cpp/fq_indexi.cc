/* fq_indexi.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int indexi_fq__(integer *nmax, integer *iarray, integer *n, 
	integer *indx)
{
    static integer i__, j, l, ir, jq, indxt;

/* *********************************************************************** */
/*     INDEXI - SORT THE N ELEMENTS OF IARRAY WHOSE POSITION IS STORED */
/*              IN INDX.  ONLY THE ORDER OF THE INDEX ARRAY, INDX, IS */
/*              MODIFIED */
/* *********************************************************************** */
    /* Parameter adjustments */
    --iarray;
    --indx;

    /* Function Body */
    l = *n / 2 + 1;
    ir = *n;
L100:
    if (l > 1) {
	--l;
	indxt = indx[l];
	jq = iarray[indxt];
    } else {
	indxt = indx[ir];
	jq = iarray[indxt];
	indx[ir] = indx[1];
	--ir;
	if (ir == 1) {
	    indx[1] = indxt;
	    return 0;
	}
    }
    i__ = l;
    j = l + l;
L110:
    if (j <= ir) {
	if (j < ir) {
	    if (iarray[indx[j]] < iarray[indx[j + 1]]) {
		++j;
	    }
	}
	if (jq < iarray[indx[j]]) {
	    indx[i__] = indx[j];
	    i__ = j;
	    j += j;
	} else {
	    j = ir + 1;
	}
	goto L110;
    }
    indx[i__] = indxt;
    goto L100;
} /* indexi_fq__ */

#ifdef __cplusplus
	}
#endif

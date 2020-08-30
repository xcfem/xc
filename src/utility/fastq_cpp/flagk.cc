/* flagk.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int flagk_(integer *npelem, integer *nnxk, integer *nxk, 
	integer *mapdxg, integer *i1, integer *i2, logical *setflg, logical *
	old)
{
    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j;

/* ************************************************************************ */
/*  SUBROUTINE FLAGK = FLAGS ELEMENTS FOR PLOTTING OR NOT PLOTTING */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NPELEMS = NUMBER OF PROCESSED ELEMENTS */
/*     NXK     = NODES PER ELEMENT ARRAY (CONNECTIVITY) */
/*     I1      = BEGINNING ELEMENT TO BE FLAGGED */
/*     I2      = ENDING ELEMENT TO BE FLAGGED */
/*     SETFLG  = .TRUE. IF THE ELEMENT IS TO BE FLAGGED FOR PLOTTING */
/*     OLD     = .TRUE. IF THE OLD ELEMENT NUMBERS ARE TO BE USED */
/* *********************************************************************** */
/*  NOTE: */
/*     THE ELEMENT IS FLAGGED FOR PLOTTING BY FORCING THE FIRST NODE TO */
/*     BE POSITIVE AND VICE VERSUS */
/* *********************************************************************** */
    /* Parameter adjustments */
    --mapdxg;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;

    /* Function Body */
    if (*old) {
	if (*setflg) {
	    i__1 = *i2;
	    for (i__ = *i1; i__ <= i__1; ++i__) {
		nxk[i__ * nxk_dim1 + 1] = (i__2 = nxk[i__ * nxk_dim1 + 1], 
			abs(i__2));
/* L100: */
	    }
	} else {
	    i__1 = *i2;
	    for (i__ = *i1; i__ <= i__1; ++i__) {
		nxk[i__ * nxk_dim1 + 1] = -(i__2 = nxk[i__ * nxk_dim1 + 1], 
			abs(i__2));
/* L110: */
	    }
	}
    } else {
	if (*setflg) {
	    i__1 = *i2;
	    for (i__ = *i1; i__ <= i__1; ++i__) {
		j = mapdxg[i__];
		nxk[j * nxk_dim1 + 1] = (i__2 = nxk[j * nxk_dim1 + 1], abs(
			i__2));
/* L120: */
	    }
	} else {
	    i__1 = *i2;
	    for (i__ = *i1; i__ <= i__1; ++i__) {
		j = mapdxg[i__];
		nxk[j * nxk_dim1 + 1] = -(i__2 = nxk[j * nxk_dim1 + 1], abs(
			i__2));
/* L130: */
	    }
	}
    }
    return 0;
} /* flagk_ */

#ifdef __cplusplus
	}
#endif

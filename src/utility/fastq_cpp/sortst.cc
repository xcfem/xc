/* sortst.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int sortst_(integer *n, real *arrin, integer *itopp, integer 
	*ibotp, integer *irange, integer *indx)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, j, l;
    static real q;
    static integer ir, ibot, itop, indxt;

/* *********************************************************************** */
/* SUBROUTINE SORTST = THIS SUBROUTINE SORTS A REAL ARRAY IN ASCENDING */
/*                     ORDER  AND DOES IT JUST CHANGING THE INDEX ARRAY. */
/* *********************************************************************** */
/* VARIABLES   IN : N ......NUMBER OF ELEMENTS IN THE ARRAY */
/*                  ARRIN ..REAL ARRAY WITH DATA TO BE SORTED */
/*                  ITOPP...TOP POINTER IN THE X ARRAY */
/*                  IBOTP...BOTTOM POINTER IN THE X ARRAY */
/*             OUT: INDX ...INDEX ARRAY WITH ITS SORTED VALUES IN */
/*                            ASCENDING ORDER. */
/*                  IRANGE..RANGE BETWEEN THE ARRAY 'INDX' WAS SORTED */
/* WRITTEN BY:  HORACIO RECALDE                 DATE: JAN 20, 1988 */
/* MODIFIED BY: MB STEPHENSON                   DATA: MAR 08, 1989 */
/*   REPLACED EXCHANGE SORT WITH HEAPSORT FOR EFFICIENCY */
/* *********************************************************************** */
/* ...  CHECK POINTERS */
    /* Parameter adjustments */
    --indx;
    --arrin;

    /* Function Body */
    itop = *itopp;
    ibot = *ibotp;
    if (itop > *n) {
	itop -= *n;
    }
    if (ibot > *n) {
	ibot -= *n;
    }
/* ---  CALCULATE THE RANGE AND INITIALIZE INDEX ARRAY */
    if (itop == ibot) {
	*irange = 1;
	indx[1] = itop;
	return 0;
    } else if (itop < ibot) {
	*irange = ibot - itop + 1;
    } else {
	*irange = ibot - itop + *n + 1;
    }
    i__1 = *irange;
    for (j = 1; j <= i__1; ++j) {
	indx[j] = itop;
	++itop;
	if (itop > *n) {
	    itop = 1;
	}
/* L100: */
    }
/* ---  PERFORM A HEAPSORT ON THE ELEMENTS */
/*           (SEE NUMERICAL RECEIPTS, PG. 233) */
/*           NOTE:  THERE MUST BE AT LEAST 2 ELEMENTS IN THE ARRAY */
    l = *irange / 2 + 1;
    ir = *irange;
L110:
    if (l > 1) {
	--l;
	indxt = indx[l];
	q = arrin[indxt];
    } else {
	indxt = indx[ir];
	q = arrin[indxt];
	indx[ir] = indx[1];
	--ir;
	if (ir == 1) {
	    indx[1] = indxt;
	    return 0;
	}
    }
    i__ = l;
    j = l + l;
L120:
    if (j <= ir) {
	if (j < ir) {
	    if (arrin[indx[j]] < arrin[indx[j + 1]]) {
		++j;
	    }
	}
	if (q < arrin[indx[j]]) {
	    indx[i__] = indx[j];
	    i__ = j;
	    j += j;
	} else {
	    j = ir + 1;
	}
	goto L120;
    }
    indx[i__] = indxt;
    goto L110;
} /* sortst_ */

#ifdef __cplusplus
	}
#endif

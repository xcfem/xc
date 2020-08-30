/* sortia.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int sortia_(integer *n, integer *iar, integer *irange, 
	integer *i__)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer j, k, l, min__;

/* *********************************************************************** */
/*  SUBROUTINE SORTIA = THIS SUBROUTINE SORTS AN INTEGER ARRAY IN */
/*                      ASCENDING ORDER */
/* *********************************************************************** */
/* VARIABLES   IN : N ...   NUMBER OF ELEMENTS IN THE ARRAY */
/*                  IAR ... INTEGER ARRAY WITH DATA TO BE SORTED */
/*             OUT: I ...   INDEX ARRAY WITH ITS SORTED VALUES IN */
/*                          ASCENDING ORDER. */
/* WRITTEN BY:  HORACIO RECALDE                 DATE: FEB 25,  1988 */
/* *********************************************************************** */
/* -- COPY ELEMENTS IN THE I ARRAY */
    /* Parameter adjustments */
    --i__;
    --iar;

    /* Function Body */
    i__1 = *irange;
    for (j = 1; j <= i__1; ++j) {
	i__[j] = iar[j];
/* L100: */
    }
/* ---  PERFORM AN EXCHANGE SORT ON THE FIRST IRANGE-1 */
    i__1 = *irange - 1;
    for (k = 1; k <= i__1; ++k) {
	min__ = i__[k];
/* ---  EXCHANGE THE K-TH ELEMENTS WITH THE MINIMUM ELEMENT REMAIN */
	i__2 = *irange;
	for (j = k + 1; j <= i__2; ++j) {
	    if (i__[j] < min__) {
		l = i__[j];
		i__[j] = i__[k];
		i__[k] = l;
		min__ = i__[k];
	    }
/* L110: */
	}
/* L120: */
    }
    return 0;
} /* sortia_ */

#ifdef __cplusplus
	}
#endif

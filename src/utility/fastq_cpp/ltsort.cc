/* ltsort.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int ltsort_(integer *mdim, integer *link, integer *id, 
	integer *ipntr, logical *addlnk)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    static integer i__;
    static real hold;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static integer lookup;

/* *********************************************************************** */
/*  SUBROUTINE LTSORT = LOOKUP TABLE SORT FOR DATA POINTER ARRAYS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     MDIM   = DIMENSION OF LINK ARRAY,  AND BASE FOR LOOKUP START */
/*     LINK   = LOOKUP TABLE ARRAY OF ID'S AND POINTERS */
/*              LINK (1, I) = ID VALUE STORED IN I'TH ROW  (0 IF EMPTY) */
/*              LINK (2, I) = DATA POINTER ASSOCIATED W/THIS I'TH ID VALUE */
/*     ID     = THE ID OF THE DATA BEING FOUND OR PLACED */
/*     IPNTR  = THE DATA POINTER ASSOCIATED WITH THE ID BEING USED */
/*     ADDLNK = .TRUE. IF DATA IS BEING PLACED IN THE LOOKUP TABLE */
/*            = .FALSE. IF DATA IS BEING FOUND ONLY */
/* *********************************************************************** */
/*  CALCULATE THE BEGINNING LOOKUP VALUE */
    /* Parameter adjustments */
    link -= 3;

    /* Function Body */
    if (*id < 0) {
	s_stop("LTSORT: Internal error", (ftnlen)22);
    }
    hold = (doublereal) (*id) * (float)3.1830989;
    lookup = (integer) ((hold - (integer) hold) * (doublereal) (*mdim) + 1);
/*  SET UP THE LOOP TO ONLY SEARCH THROUGH THE TABLE ONCE */
    i__1 = *mdim;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*  IF LOOKUP SPOT IS EMPTY THEN FILL AND RETURN IF ADDING AND IPNTR .NE. 0 */
/*  OR FLAG IPNTR AS BEING EMPTY AND RETURN IF FINDING */
	if (link[(lookup << 1) + 1] == 0) {
	    if (*addlnk && *ipntr != 0) {
		link[(lookup << 1) + 1] = *id;
		link[(lookup << 1) + 2] = *ipntr;
	    } else if (*ipntr != 0) {
		*ipntr = 0;
	    }
	    return 0;
/*  IF LOOKUP SLOT IS FULL,  CHECK TO SEE IF IT MATCHES THE CURRENT ID */
/*  IF IT MATCHES AND IF ADDING,  SET THE NEW POINTER  (OVERWRITE) */
/*  IF IT MATCHES AND IF FINDING,  RETURN THE CORRECT POINTER */
/*  IF NO MATCH,  THEN INCREMENT LOOKUP AND TRY AGAIN IN THE TABLE */
	} else {
	    if (*id == link[(lookup << 1) + 1]) {
		if (*addlnk) {
		    link[(lookup << 1) + 2] = *ipntr;
		} else {
		    *ipntr = link[(lookup << 1) + 2];
		}
		return 0;
	    } else {
		++lookup;
		if (lookup > *mdim) {
		    lookup = 1;
		}
	    }
	}
/* L100: */
    }
/*  ACT ON THE EXHAUSTED SEARCH */
    if (*addlnk) {
	mesage_("LOOKUP TABLE OVERFLOW", (ftnlen)21);
	mesage_("SERIOUS DATA PROBLEMS HAVE BEEN CAUSED", (ftnlen)38);
    } else {
	*ipntr = 0;
    }
    return 0;
} /* ltsort_ */

#ifdef __cplusplus
	}
#endif

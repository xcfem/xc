/* ndstat.f -- translated by f2c (version 20160102).
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
#include "paving.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int ndstat_(integer *node, integer *lxn, real *angle, 
	integer *jstat)
{
/* *********************************************************************** */
/*  SUBROUTINE NDSTAT = DETERMINES THE MOST APPROPRIATE STATUS OF A */
/*                      GIVEN NODE */
/* *********************************************************************** */
/*  AN UNDECIDED NODE HAS BEEN FOUND - TEST ANGLE AND CONNECTIVITY */
/*  THE NODE IS ON THE BOUNDARY */
    /* Parameter adjustments */
    --lxn;

    /* Function Body */
    if (lxn[2] < 0) {
/* IF THE NODE HAS LESS THAN FOUR LINES ATTACHED */
/*  CUTOFFS ARE:    0 TO 135 DEGREES = ROW END */
/*                135 TO 225 DEGREES = ROW SIDE */
/*                225 TO 290 DEGREES = ROW CORNER */
/*                OVER 290 DEGREES   = ROW REVERSAL */
	if (lxn[4] <= 0) {
	    if (*angle < (float)2.3561945) {
		*jstat = 1;
	    } else if (*angle < (float)3.9269908) {
		*jstat = 3;
	    } else if (*angle < (float)5.0614548) {
		*jstat = 5;
	    } else {
		*jstat = 7;
	    }
/* IF THE NODE HAS FOUR LINES ATTACHED */
/*  CUTOFFS ARE:    0 TO 110 DEGREES = ROW END */
/*                110 TO 225 DEGREES = ROW SIDE */
/*                OVER 225 DEGREES   = ROW CORNER (NEARLY IMPOSSIBLE) */
	} else {
	    if (*angle < (float)1.9198622) {
		*jstat = 1;
	    } else if (*angle < (float)3.9269908) {
		*jstat = 3;
	    } else {
		*jstat = 5;
	    }
	}
/*  THE NODE IS NOT ON THE BOUNDARY - CUTOFFS ARE ADJUSTED BASED */
/*  ON THE CONNECTIVITY AND THE ANGLE */
    } else {
/*  ONLY TWO LINES ARE ATTACHED - LEAN TOWARDS A ROW CORNER NODE */
/*  OR A ROW END NODE */
	if (lxn[3] == 0) {
/*  CUTOFFS ARE:    0 TO 135 DEGREES = ROW END */
/*                135 TO 210 DEGREES = ROW SIDE */
/*                210 TO 320 DEGREES = ROW CORNER */
/*                OVER 320 DEGREES   = ROW REVERSAL */
	    if (*angle < (float)2.3561945) {
		*jstat = 1;
	    } else if (*angle < (float)3.6651914) {
		*jstat = 3;
	    } else if (*angle < (float)5.5850536) {
		*jstat = 5;
	    } else {
		*jstat = 7;
	    }
/*  THREE LINES ARE ATTACHED - LEAN TOWARDS A ROW SIDE */
	} else if (lxn[4] == 0) {
/*  CUTOFFS ARE:    0 TO 110 DEGREES = ROW END */
/*                110 TO 240 DEGREES = ROW SIDE */
/*                240 TO 320 DEGREES = ROW CORNER */
/*                OVER 320 DEGREES   = ROW REVERSAL (REALLY IMPOSSIBLE) */
	    if (*angle < (float)1.9198622) {
		*jstat = 1;
	    } else if (*angle < (float)4.1887902) {
		*jstat = 3;
	    } else if (*angle < (float)5.5850536) {
		*jstat = 5;
	    } else {
		*jstat = 7;
	    }
/*  FOUR LINES ARE ATTACHED - LEAN TOWARDS A ROW END NODE */
	} else {
/*  CUTOFFS ARE:    0 TO 145 DEGREES = ROW END */
/*                145 TO 225 DEGREES = ROW SIDE */
/*                OVER 225 DEGREES   = ROW CORNER (REALLY IMPOSSIBLE) */
	    if (*angle < (float)2.5307274) {
		*jstat = 1;
	    } else if (*angle < (float)3.9269908) {
		*jstat = 3;
	    } else {
		*jstat = 5;
	    }
	}
    }
    return 0;
} /* ndstat_ */

#ifdef __cplusplus
	}
#endif

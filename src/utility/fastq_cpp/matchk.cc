/* matchk.f -- translated by f2c (version 20160102).
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
logical matchk_(integer *mxnd, integer *i1, integer *i2, integer *j1, integer 
	*j2, integer *lxn)
{
    /* System generated locals */
    logical ret_val;

/* *********************************************************************** */
/*  FUNCTION MATCHK = CHECKS THE CURRENT COLAPSED LINES TO SEE IF THEY */
/*                    CAN BE JOINED WITHOUT AFFECTING THE BOUNDARY. */
/*                    I1 & I2 MAY END UP SWITCHED WITH J1 & J2. */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;

    /* Function Body */
    if (lxn[(*i1 << 2) + 2] < 0 || lxn[(*i2 << 2) + 2] < 0 || lxn[(*j1 << 2) 
	    + 2] < 0 || lxn[(*j2 << 2) + 2] < 0) {
/*  FIRST CHECK FOR COMPLETELY HOOKED BOUNDARY LINES. */
	if (lxn[(*j1 << 2) + 2] < 0 && lxn[(*j2 << 2) + 2] < 0) {
	    ret_val = FALSE_;
	} else if (lxn[(*i1 << 2) + 2] < 0 && lxn[(*j2 << 2) + 2] < 0 || lxn[(
		*i2 << 2) + 2] < 0 && lxn[(*j1 << 2) + 2] < 0) {
	    ret_val = FALSE_;
	} else {
	    ret_val = TRUE_;
	}
    } else {
	ret_val = TRUE_;
    }
    return ret_val;
} /* matchk_ */

#ifdef __cplusplus
	}
#endif

/* fndlin.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int fndlin_fq__(integer *mxnd, integer *lxn, integer *node1, 
	integer *node2, integer *line, logical *err)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j, nl1, nl2, lines1[20], lines2[20];
    extern /* Subroutine */ int getlxn_(integer *, integer *, integer *, 
	    integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE FNDLIN =  FINDS THE LINE WITH ENDS NODE1 & NODE2 */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;

    /* Function Body */
    *err = FALSE_;
    getlxn_(mxnd, &lxn[5], node1, lines1, &nl1, err);
    getlxn_(mxnd, &lxn[5], node2, lines2, &nl2, err);
    if (! (*err)) {
	*err = TRUE_;
	i__1 = nl1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = nl2;
	    for (j = 1; j <= i__2; ++j) {
		if (lines1[i__ - 1] == lines2[j - 1]) {
		    *line = lines1[i__ - 1];
		    *err = FALSE_;
		    return 0;
		}
/* L100: */
	    }
/* L110: */
	}
    }
    return 0;
} /* fndlin_fq__ */

#ifdef __cplusplus
	}
#endif

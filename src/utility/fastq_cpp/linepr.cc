/* linepr.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int linepr_(integer *ml, integer *mp, integer *linkp, 
	integer *lcon, integer *ii, integer *i1, integer *i2, integer *i3, 
	integer *j1, integer *j2, integer *j3)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE LINEPR = GETS THE LINE PARAMETERS */
/* *********************************************************************** */
    /* Parameter adjustments */
    lcon -= 4;
    linkp -= 3;

    /* Function Body */
    addlnk = FALSE_;
    *i1 = lcon[*ii * 3 + 1];
    *i2 = lcon[*ii * 3 + 2];
    *i3 = lcon[*ii * 3 + 3];
    ltsort_(mp, &linkp[3], i1, j1, &addlnk);
    ltsort_(mp, &linkp[3], i2, j2, &addlnk);
    if (*i3 != 0) {
	i__1 = abs(*i3);
	ltsort_(mp, &linkp[3], &i__1, j3, &addlnk);
    } else {
	*j3 = 0;
    }
    return 0;
} /* linepr_ */

#ifdef __cplusplus
	}
#endif

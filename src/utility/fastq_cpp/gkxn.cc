/* gkxn.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int gkxn_(integer *mxnd, integer *kxl, integer *lxn, integer 
	*n, integer *ks, integer *klist, logical *err)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, k, l, ik, il, kl[20], nl, lines[20], kount;
    extern /* Subroutine */ int getlxn_(integer *, integer *, integer *, 
	    integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE GKXN = GENERATES THE LIST OF ELEMENTS ASSOCIATED WITH */
/*                    NODE N */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    kxl -= 3;
    --klist;

    /* Function Body */
    *err = FALSE_;
    *ks = 0;
    if (lxn[(*n << 2) + 1] <= 0) {
	return 0;
    }
    getlxn_(mxnd, &lxn[5], n, lines, &nl, err);
    if (*err) {
	return 0;
    }
/*  LOOP THROUGH ALL LINES CONNECTED TO THIS NODE */
    kount = 0;
    i__1 = nl;
    for (il = 1; il <= i__1; ++il) {
	l = lines[il - 1];
/*  LOOK AT ELEMENTS ON BOTH SIDES OF THIS LINE */
	for (ik = 1; ik <= 2; ++ik) {
	    k = kxl[ik + (l << 1)];
	    if (k > 0) {
		if (kount > 0) {
		    i__2 = kount;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			if (k == kl[i__ - 1]) {
			    goto L120;
			}
/* L100: */
		    }
		    if (kount >= 20) {
			*err = TRUE_;
			i__2 = kount;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    klist[i__] = kl[i__ - 1];
/* L110: */
			}
			*ks = kount;
			return 0;
		    }
		}
		++kount;
		kl[kount - 1] = k;
	    }
L120:
/* L130: */
	    ;
	}
/* L140: */
    }
/*  RETURN RESULTS */
    i__1 = kount;
    for (i__ = 1; i__ <= i__1; ++i__) {
	klist[i__] = kl[i__ - 1];
/* L150: */
    }
    *ks = kount;
    return 0;
} /* gkxn_ */

#ifdef __cplusplus
	}
#endif

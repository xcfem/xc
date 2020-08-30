/* chkkxl.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int chkkxl_(integer *mxnd, integer *lxk, integer *kxl, 
	integer *lll, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002KXL(\002,i4,\002,\002,i4,\002) = \002,\
i4,\002 IS NOT IN LXK ARRAY  -  CHKKXL\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, k, l, ik;

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE CHKKXL = CHECKS TO SEE IF THE KXL COMPARES CORRECTLY TO */
/*                      THE LXK ARRAY */
/* *********************************************************************** */
    /* Parameter adjustments */
    kxl -= 3;
    lxk -= 5;

    /* Function Body */
    *err = TRUE_;
    i__1 = *lll;
    for (l = 1; l <= i__1; ++l) {
	for (ik = 1; ik <= 2; ++ik) {
	    k = kxl[ik + (l << 1)];
	    if (k != 0) {
		for (i__ = 1; i__ <= 4; ++i__) {
		    if (lxk[i__ + (k << 2)] == l) {
			goto L110;
		    }
/* L100: */
		}
		s_wsfe(&io___5);
		do_fio(&c__1, (char *)&ik, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&l, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
		e_wsfe();
		return 0;
	    }
L110:
/* L120: */
	    ;
	}
/* L130: */
    }
    *err = FALSE_;
    return 0;
} /* chkkxl_ */

#ifdef __cplusplus
	}
#endif

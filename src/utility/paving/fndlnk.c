/* fndlnk.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int fndlnk_(integer *mxnd, integer *lxk, integer *nxl, 
	integer *k, integer *n1, integer *n2, integer *l, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 IN FNDLNK, NO LINE CAN BE FOUND FOR K, \
N1, N2: \002,3i5)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, m1, m2, ll;

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE FNDLNK = FIND THE LINE IN ELEMENT K WITH NODES N1 AND N2 */
/* *********************************************************************** */
    /* Parameter adjustments */
    nxl -= 3;
    lxk -= 5;

    /* Function Body */
    *err = FALSE_;
    for (i__ = 1; i__ <= 4; ++i__) {
	ll = lxk[i__ + (*k << 2)];
	m1 = nxl[(ll << 1) + 1];
	m2 = nxl[(ll << 1) + 2];
	if (m1 == *n1 && m2 == *n2 || m2 == *n1 && m1 == *n2) {
	    *l = ll;
	    return 0;
	}
/* L100: */
    }
    *l = 0;
    *err = TRUE_;
    s_wsfe(&io___5);
    do_fio(&c__1, (char *)&(*k), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*n1), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*n2), (ftnlen)sizeof(integer));
    e_wsfe();
    return 0;
} /* fndlnk_ */

#ifdef __cplusplus
	}
#endif

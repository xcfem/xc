/* ingrid.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int ingrid_(integer *msnap, real *snapdx, integer *nsnap, 
	integer *ii, real *rin, integer *ifound, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 GRID INDEX OUT-OF-RANGE [1,2] IN INGRID\
: \002,i3)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__;
    extern /* Subroutine */ int addsnp_(integer *, real *, integer *, integer 
	    *, real *, logical *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INGRID = INPUTS A X OR Y GRID CARD */
/* *********************************************************************** */
    /* Parameter adjustments */
    snapdx -= 3;
    --nsnap;
    --rin;

    /* Function Body */
    *err = FALSE_;
    if (*ii < 1 || *ii > 2) {
	*err = TRUE_;
	s_wsfe(&io___1);
	do_fio(&c__1, (char *)&(*ii), (ftnlen)sizeof(integer));
	e_wsfe();
	goto L110;
    }
    i__1 = *ifound;
    for (i__ = 1; i__ <= i__1; ++i__) {
	addsnp_(msnap, &snapdx[3], &nsnap[1], ii, &rin[i__], err);
	if (*err) {
	    goto L110;
	}
/* L100: */
    }
L110:
    return 0;
} /* ingrid_ */

#ifdef __cplusplus
	}
#endif

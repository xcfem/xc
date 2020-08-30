/* kxnadd.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int kxnadd_(integer *maxkxn, integer *nnxk, integer *kxn, 
	integer *numkxn, integer *k, integer *node, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002FOR ELEMENT\002,i5,\002,  AND NODE\002,i\
5)";

    /* System generated locals */
    integer kxn_dim1, kxn_offset, i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, nn;
    extern /* Subroutine */ int mesage_(char *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, fmt_10000, 0 };


/* ************************************************************************ */
/*  SUBROUTINE KXNADD = ADDS K AS AN ELEMENT OF NODE */
/* *********************************************************************** */
/*  NOTE: */
/*     IT IS ASSUMED K IS NOT ALREADY AN ELEMENT OF NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    kxn_dim1 = *nnxk;
    kxn_offset = 1 + kxn_dim1;
    kxn -= kxn_offset;

    /* Function Body */
    *err = FALSE_;
    nn = *node;
L100:
/*  LINE CONTINUES  -  FIND NEW CONTINUATION LINE */
    if (kxn[nn * kxn_dim1 + 4] < 0) {
	nn = (i__1 = kxn[nn * kxn_dim1 + 4], abs(i__1));
	goto L100;
/*  ADD THE ELEMENT TO NODE */
    } else if (kxn[nn * kxn_dim1 + 4] == 0) {
	for (i__ = 1; i__ <= 4; ++i__) {
	    if (kxn[i__ + nn * kxn_dim1] == 0) {
		kxn[i__ + nn * kxn_dim1] = *k;
		return 0;
	    }
/* L110: */
	}
	mesage_("IMPOSSIBLE SITUATION IN KXNADD", (ftnlen)30);
	s_wsfe(&io___3);
	do_fio(&c__1, (char *)&(*k), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(integer));
	e_wsfe();
	*err = TRUE_;
	return 0;
/*  ADD A CONTINUATION LINE,  AND ADD THE ELEMENT TO NODE */
    } else {
	if (*numkxn >= *maxkxn) {
	    mesage_("NO ROOM FOR KXN TABLE IN KXNADD", (ftnlen)31);
	    *err = TRUE_;
	    return 0;
	}
	++(*numkxn);
	kxn[*numkxn * kxn_dim1 + 1] = kxn[nn * kxn_dim1 + 4];
	kxn[*numkxn * kxn_dim1 + 2] = *k;
	kxn[*numkxn * kxn_dim1 + 3] = 0;
	kxn[*numkxn * kxn_dim1 + 4] = 0;
	kxn[nn * kxn_dim1 + 4] = -(*numkxn);
	return 0;
    }
    return 0;
} /* kxnadd_ */

#ifdef __cplusplus
	}
#endif

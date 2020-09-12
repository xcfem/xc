/* chrrvc.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int chrrvc_(f2c_float *buff, char *txt, integer *l, ftnlen 
	txt_len)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer i_len(char *, ftnlen), s_wsfi(icilist *), do_fio(integer *, char *
	    , ftnlen), e_wsfi();
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, l1, l2, lt;
    static char loctxt[16];

    /* Fortran I/O blocks */
    static icilist io___3 = { 0, loctxt, 0, "(1pg13.6)", 16, 1 };


    lt = i_len(txt, txt_len);
    s_wsfi(&io___3);
    do_fio(&c__1, (char *)&(*buff), (ftnlen)sizeof(f2c_float));
    e_wsfi();
    i__1 = lt;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (*(unsigned char *)&loctxt[i__ - 1] != ' ') {
	    l1 = i__;
	    goto L2120;
	}
/* L2110: */
    }
L2120:
    i__1 = lt;
    for (i__ = l1; i__ <= i__1; ++i__) {
	if (*(unsigned char *)&loctxt[i__ - 1] == ' ') {
	    l2 = i__;
	    goto L2140;
	}
/* L2130: */
    }
L2140:
    s_copy(txt, loctxt + (l1 - 1), txt_len, l2 - (l1 - 1));
    *l = l2 - l1;
    return 0;
} /* chrrvc_ */

#ifdef __cplusplus
	}
#endif

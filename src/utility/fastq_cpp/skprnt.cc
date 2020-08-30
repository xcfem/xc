/* skprnt.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int skprnt_(integer *lunit, integer *len, real *rstack, 
	integer *ndim)
{
    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__;

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, "(I8,G12.5)", 0 };


/* *********************************************************************** */
/*  SUBROUTINE SKPRNT = STACK PRINTING ROUTINE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --rstack;

    /* Function Body */
    io___1.ciunit = *lunit;
    s_wsfe(&io___1);
    for (i__ = *len; i__ >= 1; --i__) {
	do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&rstack[i__ + 2], (ftnlen)sizeof(real));
    }
    e_wsfe();
    return 0;
} /* skprnt_ */

#ifdef __cplusplus
	}
#endif

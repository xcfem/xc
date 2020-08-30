/* erasec.f -- translated by f2c (version 20160102).
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

static integer c__9 = 9;
static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int erasec_(logical *oldcur)
{
    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle();

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 6, 0, 0, 0 };


/* *********************************************************************** */
/*  SUBROUTINE ERASEC = DEACTIVATES THE CROSSHAIRS */
/* *********************************************************************** */
    if (*oldcur) {
	s_wsle(&io___1);
	do_lio(&c__9, &c__1, "\033G0", (ftnlen)3);
	e_wsle();
	*oldcur = FALSE_;
    }
    return 0;
} /* erasec_ */

#ifdef __cplusplus
	}
#endif

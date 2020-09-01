/* grabrt.f -- translated by f2c (version 20160102).
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

static logical c_false = FALSE_;
static integer c__0 = 0;
static integer c__1 = 1;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical grabrt_()
{
    /* System generated locals */
    logical ret_val;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    extern logical cpuifc_(logical *);
    extern /* Subroutine */ int pltbel_(), grsnap_(char *, integer *, ftnlen),
	     pltflu_();

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 6, 0, "(1X, A)", 0 };


/* ======================================================================= */
/*   --*** GRABRT *** (GRPLIB) Check cancel function (PLT) */
/*   --   Written by Amy Gilkey - revised 04/28/87 */
/*   -- */
/*   --GRABRT checks the cancel flag.  If it is set, it aborts pending */
/*   --graphics and returns the terminal to alphanumeric mode. */
/*   --In any case, the value of the cancel flag is returned as the */
/*   --function value. */
/*   --Routines Called: */
/*   --   CPUIFC - (PLTLIB) Check interrupt flag */
/*   --   PLTBEL - (PLTLIB) Ring bell */
/*   --   PLTBGN - (PLTLIB) Erase display surface */
/*   --   PLTFLU - (PLTLIB) Flush graphics buffer and set alphanumeric mode */
/*   --   GRSNAP - (GRPLIB) Handle device frame snapping */
/*   --Return cancel flag */
    ret_val = cpuifc_(&c_false);
    if (ret_val) {
/*      --Abort plot that is being snapped */
	grsnap_("ABORT", &c__0, (ftnlen)5);
/*      --Flush buffer (do not erase screen), ring bell, set alphanumeric mode */
	pltflu_();
	pltbel_();
	pltflu_();
/*      --Send abort message */
	s_wsfe(&io___1);
	do_fio(&c__1, "*** Plot set aborted ***", (ftnlen)24);
	e_wsfe();
    }
    return ret_val;
} /* grabrt_ */

#ifdef __cplusplus
	}
#endif

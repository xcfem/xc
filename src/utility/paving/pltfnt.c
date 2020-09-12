/* pltfnt.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__2 = 2;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltfnt_(char *filenm, ftnlen filenm_len)
{
    /* System generated locals */
    char * a__1[2];
    integer i__1[2];

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    extern /* Subroutine */ int plt_roman__(), plt_stick__(), plt_sanserif__()
	    ;
    static char tline[132];
    extern /* Subroutine */ int pltflu_(), siorpt_(char *, char *, integer *, 
	    ftnlen, ftnlen);

    if (s_cmp(filenm, "STKFNT", filenm_len, (ftnlen)6) == 0) {
	plt_stick__();
    } else if (s_cmp(filenm, "SSRFNT", filenm_len, (ftnlen)6) == 0) {
	plt_sanserif__();
    } else if (s_cmp(filenm, "ROMFNT", filenm_len, (ftnlen)6) == 0) {
	plt_roman__();
    } else {
	pltflu_();
/* Writing concatenation */
	i__1[0] = 39, a__1[0] = "Error: Unrecognized font specification ";
	i__1[1] = filenm_len, a__1[1] = filenm;
	s_cat(tline, a__1, i__1, &c__2, (ftnlen)132);
	siorpt_("PLTFNT", tline, &c__2, (ftnlen)6, (ftnlen)132);
    }
    return 0;
} /* pltfnt_ */

#ifdef __cplusplus
	}
#endif

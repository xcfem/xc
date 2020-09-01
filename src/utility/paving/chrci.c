/* chrci.f -- translated by f2c (version 20160102).
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

static integer c__3 = 3;
static integer c__1 = 1;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
logical chrci_(char *line, integer *inte, ftnlen line_len)
{
    /* System generated locals */
    address a__1[3];
    integer i__1[3], i__2;
    logical ret_val;
    icilist ici__1;

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);
    integer s_rsfi(icilist *), do_fio(integer *, char *, ftnlen), e_rsfi();

    /* Local variables */
    static char cl[2];
    static integer ll, nl;
    static char form[10];
    extern /* Subroutine */ int chric_(integer *, char *, integer *, ftnlen), 
	    chrtrm_(char *, integer *, ftnlen);

    chrtrm_(line, &ll, line_len);
    chric_(&ll, cl, &nl, (ftnlen)2);
/* Writing concatenation */
    i__1[0] = 2, a__1[0] = "(i";
    i__1[1] = nl, a__1[1] = cl;
    i__1[2] = 1, a__1[2] = ")";
    s_cat(form, a__1, i__1, &c__3, (ftnlen)10);
    ici__1.icierr = 1;
    ici__1.iciend = 0;
    ici__1.icirnum = 1;
    ici__1.icirlen = ll;
    ici__1.iciunit = line;
    ici__1.icifmt = form;
    i__2 = s_rsfi(&ici__1);
    if (i__2 != 0) {
	goto L10;
    }
    i__2 = do_fio(&c__1, (char *)&(*inte), (ftnlen)sizeof(integer));
    if (i__2 != 0) {
	goto L10;
    }
    i__2 = e_rsfi();
    if (i__2 != 0) {
	goto L10;
    }
    ret_val = TRUE_;
    return ret_val;
L10:
    ret_val = FALSE_;
    return ret_val;
} /* chrci_ */

#ifdef __cplusplus
	}
#endif

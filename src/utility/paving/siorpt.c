/* siorpt.f -- translated by f2c (version 20160102).
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

static integer c__4 = 4;
static integer c__1 = 1;
static integer c__2 = 2;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int siorpt_(char *module, char *mess, integer *disp, ftnlen 
	module_len, ftnlen mess_len)
{
    /* Format strings */
    static char fmt_10[] = "(1x,a)";

    /* System generated locals */
    address a__1[4], a__2[2];
    integer i__1[4], i__2[2];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen), s_cat(char *,
	     char **, integer *, integer *, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer l, lt;
    extern /* Subroutine */ int cpudac_(char *, ftnlen);
    static char loclin[256], errmod[8], errmsg[120];
    extern /* Subroutine */ int chrtrm_(char *, integer *, ftnlen);
    static char errort[20];

    /* Fortran I/O blocks */
    static cilist io___7 = { 0, 6, 0, fmt_10, 0 };
    static cilist io___8 = { 0, 6, 0, fmt_10, 0 };
    static cilist io___9 = { 0, 6, 0, fmt_10, 0 };
    static cilist io___10 = { 0, 6, 0, fmt_10, 0 };
    static cilist io___11 = { 0, 6, 0, fmt_10, 0 };
    static cilist io___12 = { 0, 6, 0, fmt_10, 0 };
    static cilist io___13 = { 0, 6, 0, fmt_10, 0 };
    static cilist io___14 = { 0, 6, 0, fmt_10, 0 };


    s_copy(errmod, module, (ftnlen)8, module_len);
    s_copy(errmsg, mess, (ftnlen)120, mess_len);
    cpudac_(errort, (ftnlen)20);
    chrtrm_(errort, &lt, (ftnlen)20);
    chrtrm_(errmod, &l, (ftnlen)8);
    if (l == 0) {
	l = 1;
    }
    if (*disp == 1) {
    } else if (*disp == 2) {
/* Writing concatenation */
	i__1[0] = 31, a__1[0] = "#PLT error (warning) in module ";
	i__1[1] = l, a__1[1] = errmod;
	i__1[2] = 4, a__1[2] = " at ";
	i__1[3] = lt, a__1[3] = errort;
	s_cat(loclin, a__1, i__1, &c__4, (ftnlen)256);
	s_wsfe(&io___7);
	do_fio(&c__1, loclin, (ftnlen)256);
	e_wsfe();
/* Writing concatenation */
	i__2[0] = 16, a__2[0] = "#Error message: ";
	i__2[1] = 120, a__2[1] = errmsg;
	s_cat(loclin, a__2, i__2, &c__2, (ftnlen)256);
	s_wsfe(&io___8);
	do_fio(&c__1, loclin, (ftnlen)256);
	e_wsfe();
    } else if (*disp == 3) {
/* Writing concatenation */
	i__1[0] = 33, a__1[0] = "#PLT error (traceback) in module ";
	i__1[1] = l, a__1[1] = errmod;
	i__1[2] = 4, a__1[2] = " at ";
	i__1[3] = lt, a__1[3] = errort;
	s_cat(loclin, a__1, i__1, &c__4, (ftnlen)256);
	s_wsfe(&io___9);
	do_fio(&c__1, loclin, (ftnlen)256);
	e_wsfe();
/* Writing concatenation */
	i__2[0] = 16, a__2[0] = "#Error message: ";
	i__2[1] = 120, a__2[1] = errmsg;
	s_cat(loclin, a__2, i__2, &c__2, (ftnlen)256);
	s_wsfe(&io___10);
	do_fio(&c__1, loclin, (ftnlen)256);
	e_wsfe();
    } else if (*disp == 4) {
/* Writing concatenation */
	i__1[0] = 29, a__1[0] = "#PLT error (fatal) in module ";
	i__1[1] = l, a__1[1] = errmod;
	i__1[2] = 4, a__1[2] = " at ";
	i__1[3] = lt, a__1[3] = errort;
	s_cat(loclin, a__1, i__1, &c__4, (ftnlen)256);
	s_wsfe(&io___11);
	do_fio(&c__1, loclin, (ftnlen)256);
	e_wsfe();
/* Writing concatenation */
	i__2[0] = 16, a__2[0] = "#Error message: ";
	i__2[1] = 120, a__2[1] = errmsg;
	s_cat(loclin, a__2, i__2, &c__2, (ftnlen)256);
	s_wsfe(&io___12);
	do_fio(&c__1, loclin, (ftnlen)256);
	e_wsfe();
    } else {
/* Writing concatenation */
	i__1[0] = 29, a__1[0] = "#PLT error (fatal) in module ";
	i__1[1] = l, a__1[1] = errmod;
	i__1[2] = 4, a__1[2] = " at ";
	i__1[3] = lt, a__1[3] = errort;
	s_cat(loclin, a__1, i__1, &c__4, (ftnlen)256);
	s_wsfe(&io___13);
	do_fio(&c__1, loclin, (ftnlen)256);
	e_wsfe();
/* Writing concatenation */
	i__2[0] = 16, a__2[0] = "#Error message: ";
	i__2[1] = 120, a__2[1] = errmsg;
	s_cat(loclin, a__2, i__2, &c__2, (ftnlen)256);
	s_wsfe(&io___14);
	do_fio(&c__1, loclin, (ftnlen)256);
	e_wsfe();
    }
    return 0;
} /* siorpt_ */

#ifdef __cplusplus
	}
#endif

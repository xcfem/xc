/* skinit.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int skinit_0_(int n__, integer *stack, integer *ndim, 
	integer *length, integer *ierror, integer *ivalue, integer *lout, 
	char *type__, ftnlen type_len)
{
    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(), 
	    s_cmp(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, lunit;
    extern /* Subroutine */ int skprnt_(integer *, integer *, integer *, 
	    integer *);

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 0, 0, "(A)", 0 };
    static cilist io___3 = { 0, 0, 0, "(A)", 0 };
    static cilist io___4 = { 0, 0, 0, "(A)", 0 };
    static cilist io___5 = { 0, 0, 0, "(A)", 0 };
    static cilist io___6 = { 0, 0, 0, "(A)", 0 };
    static cilist io___7 = { 0, 0, 0, "(2I8)", 0 };


/* *********************************************************************** */
/*  SUBROUTINE SKINIT = STACK MANAGEMENT ROUTINE */
/* *********************************************************************** */
/* ** PARAMETERS */
/*      STACK  = STACK ARRAY */
/*      NDIM   = DIMENSIONED SIZE OF STACK IN CALLING PROGRAM */
/*      LENGTH = LENGTH OF STACK .LE. NDIM - 2 */
/*      IERROR = 0 - NO ERROR */
/*               1 - STACK TOO SHORT (I.E. LENGTH > NDIM - 2 */
/*               2 - STACK EMPTY */
/*               3 - STACK FULL */
/*               4 - INVALID STACK TYPE */
/* ********************************************************************** */
    /* Parameter adjustments */
    if (stack) {
	--stack;
	}

    /* Function Body */
    switch(n__) {
	case 1: goto L_skpop;
	case 2: goto L_skpush;
	case 3: goto L_skeror;
	case 4: goto L_skprin;
	}

    if (*ndim < *length + 2) {
	*ierror = 1;
    } else {
	stack[1] = 0;
	stack[2] = *length;
	*ierror = 0;
    }
    return 0;
/* ======================================================================= */

L_skpop:
    if (stack[1] == 0) {
	*ierror = 2;
    } else {
	*ivalue = stack[stack[1] + 2];
	--stack[1];
	*ierror = 0;
    }
    return 0;
/* ======================================================================= */

L_skpush:
    if (stack[1] == stack[2]) {
	*ierror = 3;
    } else {
	++stack[1];
	stack[stack[1] + 2] = *ivalue;
	*ierror = 0;
    }
    return 0;
/* ======================================================================= */

L_skeror:
    if (*lout == 0) {
	lunit = 6;
    } else {
	lunit = *lout;
    }
    if (*ierror == 0) {
    } else if (*ierror == 1) {
	io___2.ciunit = lunit;
	s_wsfe(&io___2);
	do_fio(&c__1, " STACK ERROR:  ARRAY TOO SHORT", (ftnlen)30);
	e_wsfe();
    } else if (*ierror == 2) {
	io___3.ciunit = lunit;
	s_wsfe(&io___3);
	do_fio(&c__1, " STACK ERROR:  STACK EMPTY", (ftnlen)26);
	e_wsfe();
    } else if (*ierror == 3) {
	io___4.ciunit = lunit;
	s_wsfe(&io___4);
	do_fio(&c__1, " STACK ERROR:  STACK FULL", (ftnlen)25);
	e_wsfe();
    } else if (*ierror == 4) {
	io___5.ciunit = lunit;
	s_wsfe(&io___5);
	do_fio(&c__1, " STACK ERROR:  INVALID TYPE", (ftnlen)27);
	e_wsfe();
    } else {
	io___6.ciunit = lunit;
	s_wsfe(&io___6);
	do_fio(&c__1, " STACK ERROR:  UNKNOWN ERROR", (ftnlen)28);
	e_wsfe();
    }
    *ierror = 0;
    return 0;
/* ======================================================================= */

L_skprin:
    if (*lout == 0) {
	lunit = 6;
    } else {
	lunit = *lout;
    }
    if (stack[1] == 0) {
	*ierror = 2;
    } else if (s_cmp(type__, "I", type_len, (ftnlen)1) == 0) {
	io___7.ciunit = lunit;
	s_wsfe(&io___7);
	for (i__ = stack[1]; i__ >= 1; --i__) {
	    do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&stack[i__ + 2], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    } else if (s_cmp(type__, "R", type_len, (ftnlen)1) == 0) {
	skprnt_(&lunit, &stack[1], &stack[1], ndim);
    } else {
	*ierror = 4;
    }
    return 0;
} /* skinit_ */

/* Subroutine */ int skinit_(integer *stack, integer *ndim, integer *length, 
	integer *ierror)
{
    return skinit_0_(0, stack, ndim, length, ierror, (integer *)0, (integer *)
	    0, (char *)0, (ftnint)0);
    }

/* Subroutine */ int skpop_(integer *stack, integer *ndim, integer *ivalue, 
	integer *ierror)
{
    return skinit_0_(1, stack, ndim, (integer *)0, ierror, ivalue, (integer *)
	    0, (char *)0, (ftnint)0);
    }

/* Subroutine */ int skpush_(integer *stack, integer *ndim, integer *ivalue, 
	integer *ierror)
{
    return skinit_0_(2, stack, ndim, (integer *)0, ierror, ivalue, (integer *)
	    0, (char *)0, (ftnint)0);
    }

/* Subroutine */ int skeror_(integer *lout, integer *ierror)
{
    return skinit_0_(3, (integer *)0, (integer *)0, (integer *)0, ierror, (
	    integer *)0, lout, (char *)0, (ftnint)0);
    }

/* Subroutine */ int skprin_(integer *lout, integer *stack, integer *ndim, 
	char *type__, integer *ierror, ftnlen type_len)
{
    return skinit_0_(4, stack, ndim, (integer *)0, ierror, (integer *)0, lout,
	     type__, type_len);
    }

#ifdef __cplusplus
	}
#endif

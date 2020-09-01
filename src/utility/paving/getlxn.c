/* getlxn.f -- translated by f2c (version 20160102).
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

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int getlxn_(integer *mxnd, integer *lxn, integer *node, 
	integer *lines, integer *nl, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 IN GETLXN, TOO MANY NODES CONNECTED TO \
NODE\002,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer l, nn, num;

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE GETLXN = GET THE FULL LIST OF LINES CONNECTED TO NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    --lines;

    /* Function Body */
    *err = FALSE_;
    nn = *node;
    num = 0;
    if (lxn[(nn << 2) + 1] <= 0) {
	*nl = 0;
	*err = TRUE_;
	return 0;
    }
L100:
    lines[num + 1] = (i__1 = lxn[(nn << 2) + 1], abs(i__1));
    num += 2;
    lines[num] = (i__1 = lxn[(nn << 2) + 2], abs(i__1));
    l = lxn[(nn << 2) + 3];
    if (l == 0) {
	*nl = num;
	return 0;
    }
    ++num;
    lines[num] = abs(l);
    l = lxn[(nn << 2) + 4];
    if (l < 0) {
	nn = -l;
	if (num < 18) {
	    goto L100;
	} else {
	    s_wsfe(&io___4);
	    do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    return 0;
	}
    } else if (l == 0) {
	*nl = num;
	return 0;
    } else {
	++num;
	lines[num] = l;
	*nl = num;
	return 0;
    }
    return 0;
} /* getlxn_ */

#ifdef __cplusplus
	}
#endif

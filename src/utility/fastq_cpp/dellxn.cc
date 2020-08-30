/* dellxn.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int dellxn_(integer *mxnd, integer *lxn, integer *nuid, 
	integer *navail, integer *iavail, integer *node, integer *line, 
	integer *nnn, logical *err, logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 ERROR IN DELLXN - NODE\002,i5,\002 HAS \
NO LINES\002)";
    static char fmt_10010[] = "(\002 ERROR IN DELLXN - NODE:\002,i5,/,\002 L\
INES:\002,20i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, k, nl, lines[20];
    extern /* Subroutine */ int getlxn_(integer *, integer *, integer *, 
	    integer *, integer *, logical *), putlxn_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___6 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE DELLXN = DELETE LINE FROM THE LIST OF LINES FOR THIS NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nuid;
    lxn -= 5;

    /* Function Body */
    getlxn_(mxnd, &lxn[5], node, lines, &nl, err);
    if (nl < 1) {
	s_wsfe(&io___3);
	do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(integer));
	e_wsfe();
	goto L110;
    }
    if (*err) {
	goto L110;
    }
    k = 0;
    i__1 = nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (lines[i__ - 1] != *line) {
	    ++k;
	    lines[k - 1] = lines[i__ - 1];
	}
/* L100: */
    }
    if (k != nl - 1) {
	s_wsfe(&io___6);
	do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(integer));
	i__1 = nl;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&lines[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
	*err = TRUE_;
	goto L110;
    }
    --nl;
    putlxn_(mxnd, &nl, &lxn[5], &nuid[1], node, lines, navail, iavail, nnn, 
	    err, noroom);
L110:
    return 0;
} /* dellxn_ */

#ifdef __cplusplus
	}
#endif

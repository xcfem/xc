/* getkxn.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getkxn_(integer *npnode, integer *maxkxn, integer *nnxk, 
	integer *kxn, integer *nuid, integer *node, integer *klist, integer *
	numk, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 NO ELEMENTS CONNECTED TO NODE\002,i5\
,\002, NUID  = \002,i10)";
    static char fmt_10010[] = "(\002 TOO MANY ELEMENTS CONNECTED TO NODE\002\
,i5,\002, NUID  = \002,i10)";

    /* System generated locals */
    integer kxn_dim1, kxn_offset, i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, nn, num;

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___5 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___6 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE GETKXN = GET THE LIST OF ELEMENTS RELATED TO THIS NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nuid;
    kxn_dim1 = *nnxk;
    kxn_offset = 1 + kxn_dim1;
    kxn -= kxn_offset;
    --klist;

    /* Function Body */
    *err = FALSE_;
    num = 0;
    nn = *node;
/*  ADD IN THE FIRST THREE NODES LISTED */
L100:
    for (i__ = 1; i__ <= 3; ++i__) {
	if (kxn[i__ + nn * kxn_dim1] == 0) {
	    *numk = num;
	    if (*numk >= 1) {
		return 0;
	    } else {
		s_wsfe(&io___4);
		do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&nuid[*node], (ftnlen)sizeof(integer));
		e_wsfe();
		*err = TRUE_;
		return 0;
	    }
	}
	++num;
	klist[num] = kxn[i__ + nn * kxn_dim1];
/* L110: */
    }
/*  CHECK THE FOURTH NODE FOR CONTINUATION */
    if (kxn[nn * kxn_dim1 + 4] < 0) {
	nn = (i__1 = kxn[nn * kxn_dim1 + 4], abs(i__1));
	if (num < 18) {
	    goto L100;
	} else {
	    s_wsfe(&io___5);
	    do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nuid[*node], (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    return 0;
	}
/*  ADD IN THE LAST NODE IF IT IS NONZERO */
    } else {
	if (kxn[nn * kxn_dim1 + 4] != 0) {
	    ++num;
	    klist[num] = kxn[nn * kxn_dim1 + 4];
	}
	*numk = num;
	if (*numk >= 1) {
	    return 0;
	} else {
	    s_wsfe(&io___6);
	    do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nuid[*node], (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    return 0;
	}
    }
    return 0;
} /* getkxn_ */

#ifdef __cplusplus
	}
#endif

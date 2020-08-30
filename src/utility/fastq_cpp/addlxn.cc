/* addlxn.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int addlxn_(integer *mxnd, integer *lxn, integer *nuid, 
	integer *navail, integer *iavail, integer *node, integer *line, 
	integer *nnn, logical *err, logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 NODE TABLE OVERFLOW IN ADDLXN AT NOD\
E\002,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, nn, new__;
    extern /* Subroutine */ int mesage_(char *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE ADDLXN = ADDS LINE TO THE LIST OF LINES FOR THIS NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nuid;
    lxn -= 5;

    /* Function Body */
    *err = FALSE_;
    nn = *node;
L100:
/*     LINK TO CONTINUATION */
    if (lxn[(nn << 2) + 4] < 0) {
	nn = (i__1 = lxn[(nn << 2) + 4], abs(i__1));
	goto L100;
/*  THERE IS ROOM FOR THE NEW LINE WITHOUT CONTINUING */
    } else if (lxn[(nn << 2) + 4] == 0) {
	for (i__ = 1; i__ <= 4; ++i__) {
	    if (lxn[i__ + (nn << 2)] == 0) {
		lxn[i__ + (nn << 2)] = *line;
		return 0;
	    }
/* L110: */
	}
/*  THIS CAN'T HAPPEN */
	mesage_("ERROR IN ADDLXN", (ftnlen)15);
	*err = TRUE_;
    } else {
/*  CREATE A CONTINUATION ENTRY */
	if (*navail < 1) {
	    s_wsfe(&io___3);
	    do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    *noroom = TRUE_;
	    return 0;
	}
	new__ = *iavail;
	if (new__ > *nnn) {
	    *nnn = new__;
	}
	*iavail = lxn[(*iavail << 2) + 4];
	--(*navail);
	lxn[(new__ << 2) + 1] = -lxn[(nn << 2) + 4];
	lxn[(new__ << 2) + 2] = *line;
	if (lxn[(nn << 2) + 2] < 0) {
	    lxn[(new__ << 2) + 2] = -(*line);
	}
	lxn[(new__ << 2) + 3] = 0;
	lxn[(new__ << 2) + 4] = 0;
	lxn[(nn << 2) + 4] = -new__;
	nuid[new__] = 0;
    }
    return 0;
} /* addlxn_ */

#ifdef __cplusplus
	}
#endif

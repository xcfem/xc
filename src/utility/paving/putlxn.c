/* putlxn.f -- translated by f2c (version 20160102).
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

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int putlxn_(integer *mxnd, integer *nl, integer *lxn, 
	integer *nuid, integer *node, integer *lines, integer *navail, 
	integer *iavail, integer *nnn, logical *err, logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 NODE TABLE OVERFLOW IN PUTLXN - NODE =\
 \002,i5)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, n4, nn, nr, nz, new__, ndone;
    static logical bound;
    static integer nextr;

    /* Fortran I/O blocks */
    static cilist io___10 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE PUTLXN = DEFINE THE LINES FOR NODE AS  (LINES (I), I=1, NL) */
/* *********************************************************************** */
/*  NOTE: */
/*     SAME CONTINUATION ENTRIES ARE USED AS ALREADY IN USE */
/*     FOR THIS NODE. */
/*     THIS NODE WILL BE FLAGGED AS A BOUNDARY NODE IF */
/*     LXN (2, NODE) .LT. 0   (NOT IF LINES (2) .LT. 0) */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nuid;
    lxn -= 5;
    --lines;

    /* Function Body */
    bound = FALSE_;
    if (lxn[(*node << 2) + 2] < 0) {
	bound = TRUE_;
    }
    nn = *node;
    ndone = 0;
/*  FILL IN NEXT 3  (4 LAST TIME) NODES */
L100:
    n4 = lxn[(nn << 2) + 4];
/* Computing MIN */
    i__1 = 4, i__2 = *nl - ndone;
    nr = int_min(i__1,i__2);
    i__1 = nr;
    for (i__ = 1; i__ <= i__1; ++i__) {
	j = ndone + i__;
	lxn[i__ + (nn << 2)] = lines[j];
/* L110: */
    }
/*  CLEAR REMAINING PORTION */
    if (nr < 4) {
	nz = nr + 1;
	for (i__ = nz; i__ <= 4; ++i__) {
	    lxn[i__ + (nn << 2)] = 0;
/* L120: */
	}
    }
/*  TAG BOUNDARY NODES */
    if (bound) {
	lxn[(nn << 2) + 2] = -lxn[(nn << 2) + 2];
    }
/*  TAG CONTINUATIONS */
    if (ndone > 1) {
	lxn[(nn << 2) + 1] = -lxn[(nn << 2) + 1];
    }
    if (ndone + 4 >= *nl) {
/*  COLLECT GARBAGE */
L130:
	if (n4 >= 0) {
/*  UPDATE NNN */
L140:
	    if (lxn[(*nnn << 2) + 1] != 0) {
		return 0;
	    } else {
		--(*nnn);
		goto L140;
	    }
	}
	nr = -n4;
	n4 = lxn[(nr << 2) + 4];
	lxn[(nr << 2) + 1] = 0;
	lxn[(nr << 2) + 2] = 0;
	lxn[(nr << 2) + 3] = 0;
	lxn[(nr << 2) + 4] = *iavail;
	*iavail = nr;
	++(*navail);
	goto L130;
    }
/*  NEED ANOTHER LINE IN THE TABLE */
    ndone += 3;
    nextr = abs(n4);
    if (n4 < 0) {
	lxn[(nn << 2) + 4] = -nextr;
	nn = nextr;
	goto L100;
    }
/*  RESERVE A NEW LINE IN LXN TABLE */
    if (*navail < 1) {
	s_wsfe(&io___10);
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
    lxn[(new__ << 2) + 4] = 0;
    nuid[new__] = 0;
    nextr = new__;
/*  INSERT LINK TO NEXT LINE */
    lxn[(nn << 2) + 4] = -nextr;
    nn = nextr;
    goto L100;
} /* putlxn_ */

#ifdef __cplusplus
	}
#endif

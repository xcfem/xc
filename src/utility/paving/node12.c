/* node12.f -- translated by f2c (version 20160102).
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
//#include "f2c.h"
#include "paving.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int node12_(integer *mxnd, integer *mln, integer *lnodes, 
	integer *i1, integer *i2, integer *nloop1, integer *nloop2, integer *
	node1, integer *node2, integer *node, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static integer ntest, kount;
    extern /* Subroutine */ int mesage_(char *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE NODE12 = FINDS THE CURRENT NODE IN BOTH NEW LOOPS, AND */
/*                      KEEPS IT A CONSTANT */
/* *********************************************************************** */
    /* Parameter adjustments */
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *err = FALSE_;
    kount = 0;
    ntest = *i1;
L100:
    ++kount;
    if (kount > *nloop2) {
	goto L110;
    }
    if (ntest == *node) {
	*node1 = *node;
	*node2 = *i2;
	goto L130;
    }
    ntest = lnodes[ntest * lnodes_dim1 + 3];
    goto L100;
L110:
    kount = 0;
    ntest = *i2;
L120:
    ++kount;
    if (kount > *nloop1) {
	mesage_("** PROBLEMS IN NODE12 FINDING NODE **", (ftnlen)37);
	*err = TRUE_;
	goto L130;
    }
    if (ntest == *node) {
	*node1 = *i1;
	*node2 = *node;
	goto L130;
    }
    ntest = lnodes[ntest * lnodes_dim1 + 3];
    goto L120;
L130:
    return 0;
} /* node12_ */

#ifdef __cplusplus
	}
#endif

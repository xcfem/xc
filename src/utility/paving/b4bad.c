/* b4bad.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int b4bad_(integer *mxnd, integer *mln, real *xn, real *yn, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, integer *
	lnodes, real *angle, integer *i1, integer *i2, integer *j1, integer *
	j2, integer *nloop, integer *kountl, logical *bok, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static integer i__, node[4];

/* *********************************************************************** */
/*  SUBROUTINE BCROSS = CHECKS TO MAKE SURE THAT A BOUNDARY IS NOT */
/*                      BECOMING A PERMANENT CROSS */
/* *********************************************************************** */
    /* Parameter adjustments */
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *bok = TRUE_;
    *err = FALSE_;
/*  GET THE NODES THAT FORM THE REMAINING ELEMENT */
    if (*kountl == 4) {
	node[0] = *i2;
	node[1] = lnodes[node[0] * lnodes_dim1 + 3];
	node[2] = lnodes[node[1] * lnodes_dim1 + 3];
	node[3] = lnodes[node[2] * lnodes_dim1 + 3];
    } else if (*nloop - *kountl - 2 == 4) {
	node[0] = *i1;
	node[1] = lnodes[*j2 * lnodes_dim1 + 3];
	node[2] = lnodes[node[1] * lnodes_dim1 + 3];
	node[3] = lnodes[node[2] * lnodes_dim1 + 3];
    } else {
	goto L110;
    }
/*  NOW CHECK ALL THE NODES TO SEE IF THEY ARE ON THE BOUNDARY */
/*  AND CAN BE CLASSIFIED AS CORNERS */
    for (i__ = 1; i__ <= 4; ++i__) {
	if (lxn[(node[i__ - 1] << 2) + 2] < 0 && lnodes[node[i__ - 1] * 
		lnodes_dim1 + 6] >= 3) {
	    *bok = FALSE_;
	    goto L110;
	}
/* L100: */
    }
L110:
    return 0;
} /* b4bad_ */

#ifdef __cplusplus
	}
#endif

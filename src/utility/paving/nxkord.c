/* nxkord.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int nxkord_(integer *nodes, integer *n1)
{
    static integer i__, nsave, nsave2;

/* *********************************************************************** */
/*  SUBROUTINE NXKORD = ROTATES THE LIST OF FOUR NODES SO N1 APPEARS */
/*                      FIRST IF IT IS IN THE LIST */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nodes;

    /* Function Body */
    for (i__ = 1; i__ <= 4; ++i__) {
	if (nodes[i__] == *n1) {
	    if (i__ == 1) {
		return 0;
	    } else if (i__ == 2) {
		nsave = nodes[1];
		nodes[1] = nodes[2];
		nodes[2] = nodes[3];
		nodes[3] = nodes[4];
		nodes[4] = nsave;
	    } else if (i__ == 3) {
		nsave = nodes[1];
		nsave2 = nodes[2];
		nodes[1] = nodes[3];
		nodes[2] = nodes[4];
		nodes[3] = nsave;
		nodes[4] = nsave2;
	    } else {
		nsave = nodes[4];
		nodes[4] = nodes[3];
		nodes[3] = nodes[2];
		nodes[2] = nodes[1];
		nodes[1] = nsave;
	    }
	    return 0;
	}
/* L100: */
    }
    return 0;
} /* nxkord_ */

#ifdef __cplusplus
	}
#endif

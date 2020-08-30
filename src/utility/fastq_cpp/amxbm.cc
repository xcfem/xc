/* amxbm.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int amxbm_(integer *npnode, integer *npelem, integer *nxk, 
	real *amesur, real *bmesur, integer *knode)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, j, node;

/* *********************************************************************** */
/*  SUBROUTINE AMXBM = ROUTINE TO TRANSFER ELEMENT VARIABLES TO NODES */
/* *********************************************************************** */
    /* Parameter adjustments */
    --knode;
    --bmesur;
    --amesur;
    nxk -= 10;

    /* Function Body */
    i__1 = *npnode;
    for (i__ = 1; i__ <= i__1; ++i__) {
	bmesur[i__] = (float)0.;
	knode[i__] = 0;
/* L100: */
    }
/*  GATHER ALL THE VARIABLES TO THE NODES AND COUNT HOW MANY AT EACH NODE */
    i__1 = *npelem;
    for (i__ = 1; i__ <= i__1; ++i__) {
	for (j = 1; j <= 4; ++j) {
	    node = nxk[j + i__ * 9];
	    bmesur[node] += amesur[i__];
	    ++knode[node];
/* L110: */
	}
/* L120: */
    }
/*  GET THE AVERAGE VALUE AT EACH NODE */
    i__1 = *npnode;
    for (node = 1; node <= i__1; ++node) {
	bmesur[node] /= (doublereal) knode[node];
/* L130: */
    }
    return 0;
} /* amxbm_ */

#ifdef __cplusplus
	}
#endif

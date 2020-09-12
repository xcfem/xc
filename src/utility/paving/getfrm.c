/* getfrm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getfrm_(integer *mxnd, integer *lines, integer *nl, 
	integer *nxl, integer *node, integer *n0, integer *n2, integer *nfrom)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer il, ill, ntest;
    extern /* Subroutine */ int mesage_(char *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE GETFRM = GETS THE NODES THAT THE CURRENT NODE CAME FROM */
/* *********************************************************************** */
    /* Parameter adjustments */
    nxl -= 3;
    --lines;

    /* Function Body */
    *nfrom = 0;
    if (*nl == 3) {
	i__1 = *nl;
	for (il = 1; il <= i__1; ++il) {
	    ill = lines[il];
	    if (nxl[(ill << 1) + 1] == *node) {
		ntest = nxl[(ill << 1) + 2];
	    } else if (nxl[(ill << 1) + 2] == *node) {
		ntest = nxl[(ill << 1) + 1];
	    } else {
		mesage_("** PROBLEMS IN GETFRM **", (ftnlen)24);
		goto L110;
	    }
	    if (ntest != *n0 && ntest != *n2) {
		*nfrom = ntest;
		goto L110;
	    }
/* L100: */
	}
    }
L110:
    return 0;
} /* getfrm_ */

#ifdef __cplusplus
	}
#endif

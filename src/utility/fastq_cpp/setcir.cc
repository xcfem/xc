/* setcir.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int setcir_(integer *mxnd, integer *mln, integer *nloop, 
	integer *lnodes, integer *node, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static integer inow, kount;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static integer newnod;

/* *********************************************************************** */
/*  SUBROUTINE SETCIR = MARKS ALL THE NODES IN THE CIRCULAR LOOP */
/*  AS SIDES EXCEPT FOR ROW CORNERS */
/* *********************************************************************** */
    /* Parameter adjustments */
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *err = FALSE_;
    kount = 0;
    inow = *node;
    newnod = *node;
L100:
    inow = lnodes[inow * lnodes_dim1 + 3];
    if (lnodes[inow * lnodes_dim1 + 1] <= 4) {
	lnodes[inow * lnodes_dim1 + 1] = 3;
    } else if (lnodes[inow * lnodes_dim1 + 1] == 6) {
	lnodes[inow * lnodes_dim1 + 1] = 5;
    }
    if (lnodes[newnod * lnodes_dim1 + 1] == 3 && (lnodes[inow * lnodes_dim1 + 
	    1] == 5 || lnodes[inow * lnodes_dim1 + 1] == 7)) {
	newnod = inow;
    }
    if (inow == *node) {
	*node = newnod;
	return 0;
    }
    ++kount;
    if (kount > *nloop) {
	mesage_("PROBLEMS IN SETCIR WITH LOOP NOT CLOSING", (ftnlen)40);
	*err = TRUE_;
	goto L110;
    }
    goto L100;
L110:
    return 0;
} /* setcir_ */

#ifdef __cplusplus
	}
#endif

/* nsplit.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int nsplit_(integer *mxnd, integer *mln, integer *lnodes, 
	real *angle, integer *nstart, integer *kang, integer *inode, integer *
	nnode, integer *nwant, logical *maxsiz)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;
    real r__1, r__2;
    doublereal d__1;

    /* Builtin functions */
    integer i_dnnt(doublereal *), i_nint(real *);

    /* Local variables */
    static integer i__, nnow;
    static real xkang;
    static integer njump, njump1, njump2, njump3;
    extern integer jumplp_(integer *, integer *, integer *, integer *, 
	    integer *);

/* *********************************************************************** */
/*  SUBROUTINE NSPLIT = SPLITS UP THE KANG CONSECUTIVE NODES STARTING */
/*                      AT NSTART INTO NWANT INTERVALS (OR AS CLOSE */
/*                      AS AN BE EXPECTED).  THE MAXIMUM NWANT SHOULD */
/*                      BE IS 4. */
/* *********************************************************************** */
    /* Parameter adjustments */
    --angle;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    --inode;

    /* Function Body */
    *nnode = 0;
    if (*kang <= *nwant) {
	nnow = *nstart;
	i__1 = *kang;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    inode[i__] = nnow;
	    nnow = lnodes[nnow * lnodes_dim1 + 3];
/* L100: */
	}
	*nnode = *kang;
    } else if (*nwant == 1) {
	*nnode = 1;
	if (*kang == 2) {
	    if (*maxsiz) {
		if (angle[*nstart] > angle[lnodes[*nstart * lnodes_dim1 + 3]])
			 {
		    inode[1] = *nstart;
		} else {
		    inode[1] = lnodes[*nstart * lnodes_dim1 + 3];
		}
	    } else {
		if (angle[*nstart] > angle[lnodes[*nstart * lnodes_dim1 + 3]])
			 {
		    inode[1] = lnodes[*nstart * lnodes_dim1 + 3];
		} else {
		    inode[1] = *nstart;
		}
	    }
	} else {
	    i__1 = *kang / 2;
	    inode[1] = jumplp_(mxnd, mln, &lnodes[lnodes_offset], nstart, &
		    i__1);
	}
    } else if (*nwant == 2) {
	*nnode = 2;
	d__1 = (doublereal) (*kang + 1) / (float)4.;
	njump = i_dnnt(&d__1);
	i__1 = njump - 1;
	inode[1] = jumplp_(mxnd, mln, &lnodes[lnodes_offset], nstart, &i__1);
	i__1 = *kang - njump;
	inode[2] = jumplp_(mxnd, mln, &lnodes[lnodes_offset], nstart, &i__1);
    } else if (*nwant == 3) {
	*nnode = 3;
	d__1 = (doublereal) (*kang + 1) / (float)6.;
	njump1 = i_dnnt(&d__1);
	d__1 = (doublereal) (*kang + 1) / (float)2.;
	njump2 = i_dnnt(&d__1);
	i__1 = njump1 - 1;
	inode[1] = jumplp_(mxnd, mln, &lnodes[lnodes_offset], nstart, &i__1);
	i__1 = njump2 - 1;
	inode[2] = jumplp_(mxnd, mln, &lnodes[lnodes_offset], nstart, &i__1);
	i__1 = *kang - njump1;
	inode[3] = jumplp_(mxnd, mln, &lnodes[lnodes_offset], nstart, &i__1);
    } else if (*nwant == 4) {
	*nnode = 4;
	xkang = (real) (*kang + 1);
	r__1 = xkang / (float)8.;
	njump1 = i_nint(&r__1) - 1;
	r__1 = xkang / (float)2.;
	r__2 = xkang / (float)8.;
	njump2 = i_nint(&r__1) - i_nint(&r__2) - 1;
	r__1 = xkang / (float)2.;
	r__2 = xkang / (float)8.;
	njump3 = i_nint(&r__1) + i_nint(&r__2) - 1;
	inode[1] = jumplp_(mxnd, mln, &lnodes[lnodes_offset], nstart, &njump1)
		;
	inode[2] = jumplp_(mxnd, mln, &lnodes[lnodes_offset], nstart, &njump2)
		;
	inode[3] = jumplp_(mxnd, mln, &lnodes[lnodes_offset], nstart, &njump3)
		;
	i__1 = *kang - njump1 - 1;
	inode[4] = jumplp_(mxnd, mln, &lnodes[lnodes_offset], nstart, &i__1);
    }
    return 0;
} /* nsplit_ */

#ifdef __cplusplus
	}
#endif

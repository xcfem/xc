/* mnmxk.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int mnmxk_(integer *npelem, integer *npnode, integer *nnxk, 
	integer *nxk, real *xn, real *yn, real *centk, integer *kkk, real *
	xmin, real *xmax, real *ymin, real *ymax)
{
    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2;
    real r__1, r__2;

    /* Local variables */
    static integer i__, j, k, jx1, jx2, jy1, jy2;

/* *********************************************************************** */
/*  SUBROUTINE MNMXK = FINDS MIN AND MAX DIMENSIONS FOR FLAGGED ELEMENTS */
/* ********************************************************************** */
/*  FIND THE FIRST ELEMENT TO BE PLOTTED */
    /* Parameter adjustments */
    centk -= 3;
    --yn;
    --xn;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;

    /* Function Body */
    i__1 = *kkk;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nxk[i__ * nxk_dim1 + 1] > 0) {
	    jx1 = i__;
	    jx2 = i__;
	    jy1 = i__;
	    jy2 = i__;
/*  COMPARE CENTERS TO GET MIN AND MAX ELEMENTS */
	    i__2 = *kkk;
	    for (j = i__ + 1; j <= i__2; ++j) {
		if (nxk[j * nxk_dim1 + 1] > 0) {
		    if (centk[(j << 1) + 1] < centk[(jx1 << 1) + 1]) {
			jx1 = j;
		    }
		    if (centk[(j << 1) + 1] > centk[(jx2 << 1) + 1]) {
			jx2 = j;
		    }
		    if (centk[(j << 1) + 2] < centk[(jy1 << 1) + 2]) {
			jy1 = j;
		    }
		    if (centk[(j << 1) + 2] > centk[(jy2 << 1) + 2]) {
			jy2 = j;
		    }
		}
/* L100: */
	    }
/*  FIND CORRECT MIN AND MAX FROM NODES OF MIN AND MAX ELEMENTS */
	    *xmin = xn[nxk[jx1 * nxk_dim1 + 1]];
	    i__2 = *nnxk;
	    for (k = 2; k <= i__2; ++k) {
		if (nxk[k + jx1 * nxk_dim1] > 0) {
/* Computing MIN */
		    r__1 = *xmin, r__2 = xn[nxk[k + jx1 * nxk_dim1]];
		    *xmin = dmin(r__1,r__2);
		}
/* L110: */
	    }
	    *xmax = xn[nxk[jx2 * nxk_dim1 + 1]];
	    i__2 = *nnxk;
	    for (k = 2; k <= i__2; ++k) {
		if (nxk[k + jx2 * nxk_dim1] > 0) {
/* Computing MAX */
		    r__1 = *xmax, r__2 = xn[nxk[k + jx2 * nxk_dim1]];
		    *xmax = dmax(r__1,r__2);
		}
/* L120: */
	    }
	    *ymin = yn[nxk[jy1 * nxk_dim1 + 1]];
	    i__2 = *nnxk;
	    for (k = 2; k <= i__2; ++k) {
		if (nxk[k + jy1 * nxk_dim1] > 0) {
/* Computing MIN */
		    r__1 = *ymin, r__2 = yn[nxk[k + jy1 * nxk_dim1]];
		    *ymin = dmin(r__1,r__2);
		}
/* L130: */
	    }
	    *ymax = yn[nxk[jy2 * nxk_dim1 + 1]];
	    i__2 = *nnxk;
	    for (k = 2; k <= i__2; ++k) {
		if (nxk[k + jy2 * nxk_dim1] > 0) {
/* Computing MAX */
		    r__1 = *ymax, r__2 = yn[nxk[k + jy2 * nxk_dim1]];
		    *ymax = dmax(r__1,r__2);
		}
/* L140: */
	    }
	    return 0;
	}
/* L150: */
    }
/*  RETURN WITH DEFAULT MINS AND MAXS */
    *xmin = (float)0.;
    *xmax = (float)1.;
    *ymin = (float)0.;
    *ymax = (float)1.;
    return 0;
} /* mnmxk_ */

#ifdef __cplusplus
	}
#endif

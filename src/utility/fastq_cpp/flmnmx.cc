/* flmnmx.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int flmnmx_(integer *mxnd, integer *mln, integer *maxprm, 
	integer *linkpr, integer *kperim, integer *lnodes, real *xn, real *yn,
	 integer *nloop, integer *node, real *xmin, real *xmax, real *ymin, 
	real *ymax, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;
    real r__1, r__2;

    /* Local variables */
    static integer kmax, inow, kount;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static integer iperim;

/* *********************************************************************** */
/*  SUBROUTINE FLMNMX = SET MIN AND MAX FOR CURRENT FILL BOUNDARY */
/* *********************************************************************** */
    /* Parameter adjustments */
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    linkpr -= 4;

    /* Function Body */
    kount = 0;
    inow = *node;
    *xmin = xn[*node];
    *xmax = xn[*node];
    *ymin = yn[*node];
    *ymax = yn[*node];
L100:
    inow = lnodes[inow * lnodes_dim1 + 3];
    if (inow != *node) {
/* Computing MIN */
	r__1 = *xmin, r__2 = xn[inow];
	*xmin = dmin(r__1,r__2);
/* Computing MIN */
	r__1 = *ymin, r__2 = yn[inow];
	*ymin = dmin(r__1,r__2);
/* Computing MAX */
	r__1 = *xmax, r__2 = xn[inow];
	*xmax = dmax(r__1,r__2);
/* Computing MAX */
	r__1 = *ymax, r__2 = yn[inow];
	*ymax = dmax(r__1,r__2);
	++kount;
	if (kount > *nloop) {
	    mesage_("PROBLEMS IN FLMNMX WITH LOOP NOT CLOSING", (ftnlen)40);
	    *err = TRUE_;
	    goto L130;
	}
	goto L100;
    }
/*  LOOP THROUGH ALL THE REMAINING PERIMETERS CHECKING FOR CROSSINGS */
    iperim = *kperim;
L110:
    iperim = linkpr[iperim * 3 + 2];
    if (iperim == 0 || iperim == *kperim) {
	goto L130;
    }
    kmax = linkpr[iperim * 3 + 3];
    inow = linkpr[iperim * 3 + 1];
    kount = 0;
L120:
/* Computing MIN */
    r__1 = *xmin, r__2 = xn[inow];
    *xmin = dmin(r__1,r__2);
/* Computing MIN */
    r__1 = *ymin, r__2 = yn[inow];
    *ymin = dmin(r__1,r__2);
/* Computing MAX */
    r__1 = *xmax, r__2 = xn[inow];
    *xmax = dmax(r__1,r__2);
/* Computing MAX */
    r__1 = *ymax, r__2 = yn[inow];
    *ymax = dmax(r__1,r__2);
    ++kount;
    inow = lnodes[inow * lnodes_dim1 + 3];
    if (inow == linkpr[iperim * 3 + 1]) {
	goto L110;
    }
    if (kount > kmax + 1) {
	mesage_("PROBLEMS IN FLMNMX WITH LOOP NOT CLOSING", (ftnlen)40);
	*err = TRUE_;
	goto L130;
    }
    goto L120;
L130:
    return 0;
} /* flmnmx_ */

#ifdef __cplusplus
	}
#endif

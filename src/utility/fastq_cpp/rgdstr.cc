/* rgdstr.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int rgdstr_(integer *npnode, integer *npelem, integer *kkk, 
	integer *nnxk, real *xn, real *yn, integer *nxk)
{
    /* Format strings */
    static char fmt_10000[] = "(\002  THE MINIMUM DISTORTION IS: \002,g14.7\
,\002 IN ELEMENT: \002,i10,/,\002  THE MAXIMUM DISTORTION IS: \002,g14.7,\
\002 IN ELEMENT: \002,i10,/,\002  THE AVERAGE DISTORTION IS: \002,g14.7)";

    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, n1, n2, n3, n4;
    static real sum;
    static integer kmin, kmax;
    static real vmin, vmax, value;
    extern /* Subroutine */ int dstort_(real *, real *, real *, real *, real *
	    , real *, real *, real *, real *);

    /* Fortran I/O blocks */
    static cilist io___12 = { 0, 6, 0, fmt_10000, 0 };


/* ************************************************************************ */
/*  SUBROUTINE RGDSTR = CALCULATES A REGION DISTORTION MEASURE */
/* *********************************************************************** */
/*  CALCULATE THE ELEMENT DISTORTION */
    /* Parameter adjustments */
    --yn;
    --xn;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;

    /* Function Body */
    n1 = nxk[nxk_dim1 + 1];
    n2 = nxk[nxk_dim1 + 2];
    n3 = nxk[nxk_dim1 + 3];
    n4 = nxk[nxk_dim1 + 4];
    dstort_(&xn[n1], &xn[n2], &xn[n3], &xn[n4], &yn[n1], &yn[n2], &yn[n3], &
	    yn[n4], &value);
    vmin = value;
    vmax = value;
    sum = value;
    kmin = 1;
    kmax = 1;
    i__1 = *kkk;
    for (i__ = 1; i__ <= i__1; ++i__) {
	n1 = nxk[i__ * nxk_dim1 + 1];
	n2 = nxk[i__ * nxk_dim1 + 2];
	n3 = nxk[i__ * nxk_dim1 + 3];
	n4 = nxk[i__ * nxk_dim1 + 4];
	dstort_(&xn[n1], &xn[n2], &xn[n3], &xn[n4], &yn[n1], &yn[n2], &yn[n3],
		 &yn[n4], &value);
	if (vmin > value) {
	    vmin = value;
	    kmin = i__;
	} else if (vmax < value) {
	    vmax = value;
	    kmax = i__;
	}
	sum += value;
/* L100: */
    }
/*  PRINT OUT THE RESULTS */
    sum /= (doublereal) (*kkk);
    s_wsfe(&io___12);
    do_fio(&c__1, (char *)&vmin, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&kmin, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&vmax, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&kmax, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&sum, (ftnlen)sizeof(real));
    e_wsfe();
    return 0;
} /* rgdstr_ */

#ifdef __cplusplus
	}
#endif

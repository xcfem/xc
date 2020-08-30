/* unisnp.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int unisnp_(integer *msnap, real *snapdx, integer *nsnap, 
	integer *index, real *xmin, real *xmax, real *step)
{
    /* Initialized data */

    static char axis[1*2+1] = "XY";

    /* Format strings */
    static char fmt_10000[] = "(\002 THE LAST SUCCESSFUL \002,a1,\002 GRID I\
NPUT WAS: \002,g14.7)";

    /* System generated locals */
    integer i__1;
    real r__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__;
    static logical err;
    static real xgrid;
    static integer iloop;
    extern /* Subroutine */ int addsnp_(integer *, real *, integer *, integer 
	    *, real *, logical *);

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE UNISNP = GENERATES UNIFORM SNAP GRID */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     MSNAP  = DIMENSION OV SNAP ARRAYS */
/*     SNAPDX = THE SNAP GRID VALUES ARRAY  (X AND Y) */
/*     NSNAP  = THE NUMBER OF SNAP GRID VALUES IN X AND Y */
/*     INDEX  = 1 FOR X VALUES,  2 FOR Y VALUES */
/* *********************************************************************** */
    /* Parameter adjustments */
    snapdx -= 3;
    --nsnap;

    /* Function Body */
/*  DEFINE THE GRID */
    if (*step == (float)0.) {
	return 0;
    }
    iloop = (integer) ((*xmax - *xmin) / *step + 2);
    xgrid = *xmin;
    i__1 = iloop;
    for (i__ = 1; i__ <= i__1; ++i__) {
	addsnp_(msnap, &snapdx[3], &nsnap[1], index, &xgrid, &err);
	if (err) {
	    s_wsfe(&io___6);
	    do_fio(&c__1, axis + (*index - 1), (ftnlen)1);
	    r__1 = xgrid - *step;
	    do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	    e_wsfe();
	    return 0;
	}
	xgrid += *step;
	if (xgrid >= *step + *xmax) {
	    return 0;
	}
/* L100: */
    }
    return 0;
} /* unisnp_ */

#ifdef __cplusplus
	}
#endif

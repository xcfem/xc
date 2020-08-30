/* addsnp.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int addsnp_(integer *msnap, real *snapdx, integer *nsnap, 
	integer *index, real *value, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 THE MAXIMUM NUMBER OF GRID LINES IS:\
 \002,i10)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, kount;
    extern /* Subroutine */ int mesage_(char *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*   SUBROUTINE ADDSNP = ADDS SNAP GRID LINE DEFINITIONS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     MSNAP  = DIMENSION OV SNAP ARRAYS */
/*     SNAPDX = THE SNAP GRID VALUES ARRAY (X AND Y) */
/*     NSNAP  = THE NUMBER OF SNAP GRID VALUES IN X AND Y */
/*     INDEX  = 1 FOR X VALUES, 2 FOR Y VALUES */
/*     VALUE  = THE GRID VALUE TO BE ADDED */
/*     KOUNT  = THE LOCATION OF THE SNAPDX VALUE JUST LESS THAN VALUE */
/* *********************************************************************** */
/*  ADD THE SNAP GRID VALUE WHERE IT FITS IN NUMERICAL ORDER */
    /* Parameter adjustments */
    snapdx -= 3;
    --nsnap;

    /* Function Body */
    *err = FALSE_;
    if (nsnap[*index] > 0) {
	kount = 0;
	i__1 = nsnap[*index];
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (*value < snapdx[*index + (i__ << 1)]) {
		goto L110;
	    }
	    kount = i__;
/*  IF THE VALUE IS ALREADY THERE, THEN DON'T ADD IT AGAIN - JUST RETURN */
	    if (*value == snapdx[*index + (i__ << 1)]) {
		return 0;
	    }
/* L100: */
	}
L110:
	if (nsnap[*index] == *msnap) {
	    mesage_("** NO MORE ROOM FOR ADDITIONAL GRID LINES **", (ftnlen)
		    44);
	    s_wsfe(&io___3);
	    do_fio(&c__1, (char *)&(*msnap), (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    return 0;
	}
	++nsnap[*index];
	i__1 = kount + 2;
	for (i__ = nsnap[*index]; i__ >= i__1; --i__) {
	    snapdx[*index + (i__ << 1)] = snapdx[*index + (i__ - 1 << 1)];
/* L120: */
	}
	snapdx[*index + (kount + 1 << 1)] = *value;
/*  JUST PUT THE FIRST VALUE WHERE IT BELONGS */
    } else {
	nsnap[*index] = 1;
	snapdx[*index + 2] = *value;
    }
    return 0;
} /* addsnp_ */

#ifdef __cplusplus
	}
#endif

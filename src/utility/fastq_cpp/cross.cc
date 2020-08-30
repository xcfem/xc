/* cross.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int ccross_(integer *j1, integer *j2, integer *i1, integer *
	i2, integer *jxi, integer *ixj, integer *istart, integer *iclear, 
	logical *noroom, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 OUT-OF-BOUNDS REFERENCE IN CROSS (INDEX\
 = \002,i5,\002, MAX = \002,i5,\002)\002)";

    /* System generated locals */
    integer jxi_dim1, jxi_offset, ixj_dim1, ixj_offset, i__1, i__2, i__3;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, k, l, kk;
    extern /* Subroutine */ int mesage_(char *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE CROSS = CREATE OR ADD TO THE CROSS - REFERENCE ARRAY FOR */
/*                     JXI (J1, J2) IN IXJ (I1, I2) */
/* *********************************************************************** */
/*  NOTE: */
/*     THE NEW ITEMS MUST BEGIN AT J1=1,  J2=ISTART. */
/*     THE CROSS REFERENCE ARRAY WILL BE CLEARED FROM I1=1,  I2=ICLEAR */
/*     TO THE END OF THE ARRAY. */
/* *********************************************************************** */
/*  CLEAR */
    /* Parameter adjustments */
    jxi_dim1 = *j1;
    jxi_offset = 1 + jxi_dim1;
    jxi -= jxi_offset;
    ixj_dim1 = *i1;
    ixj_offset = 1 + ixj_dim1;
    ixj -= ixj_offset;

    /* Function Body */
    *err = TRUE_;
    *noroom = FALSE_;
    i__1 = *i2;
    for (j = *iclear; j <= i__1; ++j) {
	i__2 = *i1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    ixj[i__ + j * ixj_dim1] = 0;
/* L100: */
	}
/* L110: */
    }
/*  REFILE EACH ITEM */
    i__1 = *j2;
    for (j = *istart; j <= i__1; ++j) {
	i__2 = *j1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    l = (i__3 = jxi[i__ + j * jxi_dim1], abs(i__3));
	    if (l != 0) {
		if (l > *i2) {
		    s_wsfe(&io___4);
		    do_fio(&c__1, (char *)&l, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&(*i2), (ftnlen)sizeof(integer));
		    e_wsfe();
		    return 0;
		}
/*  FIND EMPTY SPOT FOR THIS ITEM */
		i__3 = *i1;
		for (k = 1; k <= i__3; ++k) {
		    kk = k;
		    if (ixj[k + l * ixj_dim1] == 0) {
			goto L130;
		    }
/* L120: */
		}
		mesage_("NO ROOM FOR REFERENCE - ERROR IN CROSS", (ftnlen)38);
		*noroom = TRUE_;
		return 0;
L130:
/*  FILE THIS ITEM */
		ixj[kk + l * ixj_dim1] = j;
	    }
/* L140: */
	}
/* L150: */
    }
    *err = FALSE_;
    return 0;
} /* ccross_ */

#ifdef __cplusplus
	}
#endif

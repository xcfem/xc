/* inhole.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inhole_(integer *mr, integer *n7, integer *n29, integer *
	jpntr, integer *iin, integer *ifound, integer *ifhole, integer *nhpr, 
	integer *ihlist, logical *merge, logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 REGION:\002,i5,\002 HAS LESS THAN ONE H\
OLE\002,/,\002 THE HOLES FOR THIS REGION WILL NOT BE INPUT INTO DATABASE\002)"
	    ;

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, jj;

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INHOLE  =  INPUTS A REGION'S HOLES INTO THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --ihlist;
    --nhpr;
    --ifhole;
    --iin;

    /* Function Body */
    *noroom = TRUE_;
/*  ADD THE REGION INTO THE DATABASE */
    j = *jpntr;
    ifhole[j] = *n29 + 1;
    i__1 = *ifound;
    for (i__ = 1; i__ <= i__1; ++i__) {
	jj = iin[i__];
	if (jj == 0) {
	    goto L110;
	}
	++(*n29);
	if (*n29 > *mr << 1) {
	    return 0;
	}
	ihlist[*n29] = jj;
/* L100: */
    }
L110:
    nhpr[j] = *n29 - ifhole[j] + 1;
    if (nhpr[j] < 1) {
	s_wsfe(&io___4);
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	e_wsfe();
	*n29 = ifhole[j] - 1;
    }
    *noroom = FALSE_;
    return 0;
} /* inhole_ */

#ifdef __cplusplus
	}
#endif

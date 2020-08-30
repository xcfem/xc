/* inpoin.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inpoin_(integer *mp, integer *n1, integer *n18, integer *
	jj, real *x, real *y, integer *nholdp, integer *iholdp, integer *
	ipoint, real *coor, integer *ipboun, integer *linkp, logical *merge, 
	logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002  OLD POINT NO:\002,i5,\002  TO NEW POIN\
T NO:\002,i5)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer j, jhold, ipntr;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INPOIN = ENTERS A POINT INTO THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    --ipboun;
    coor -= 3;
    --ipoint;
    iholdp -= 3;

    /* Function Body */
    *noroom = TRUE_;
    jhold = *jj;
/*  ZERO OUT THE LINK ARRAY IF NEEDED */
    if (*jj > *n18) {
	*n18 = *jj;
/*  GET THE CORRECT NODE NUMBER IF MERGING */
    } else if (*merge) {
	addlnk = FALSE_;
	ltsort_(mp, &linkp[3], jj, &ipntr, &addlnk);
	if (ipntr > 0) {
	    if (jhold > *nholdp) {
		*nholdp = jhold;
	    }
	    ltsort_(mp, &iholdp[3], &jhold, &ipntr, &addlnk);
	    if (ipntr > 0) {
		*jj = ipntr;
	    } else {
		*jj = *n18 + 1;
		++(*n18);
		s_wsfe(&io___4);
		do_fio(&c__1, (char *)&jhold, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jj), (ftnlen)sizeof(integer));
		e_wsfe();
		addlnk = TRUE_;
		ltsort_(mp, &iholdp[3], &jhold, jj, &addlnk);
	    }
	}
    }
/*  INPUT THE POINT DATA */
    ++(*n1);
    j = *n1;
    if (j > *mp) {
	return 0;
    }
    addlnk = TRUE_;
    ltsort_(mp, &linkp[3], jj, &j, &addlnk);
    ipoint[j] = *jj;
    coor[(j << 1) + 1] = *x;
    coor[(j << 1) + 2] = *y;
    ipboun[j] = 0;
    *noroom = FALSE_;
    return 0;
} /* inpoin_ */

#ifdef __cplusplus
	}
#endif

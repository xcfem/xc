/* inline.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inline_(integer *ml, integer *n2, integer *n19, integer *
	jj, integer *ltyp, integer *ip1, integer *ip2, integer *ip3, integer *
	nn, real *fact, integer *nholdl, integer *iholdl, integer *iline, 
	integer *ltype, integer *nint, real *factor, integer *lcon, integer *
	ilboun, integer *isboun, integer *linkl, logical *merge, logical *
	noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002   OLD LINE NO:\002,i5,\002   TO NEW LIN\
E NO:\002,i5)";
    static char fmt_10010[] = "(\002WARNING: Intervals on line \002,i5,\002 \
are negative.\002,\002 Changed to positive.\002)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer j, jhold, ipntr;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___6 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INLINE = INPUTS A LINE INTO THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkl -= 3;
    --isboun;
    --ilboun;
    lcon -= 4;
    --factor;
    --nint;
    --ltype;
    --iline;
    iholdl -= 3;

    /* Function Body */
    *noroom = TRUE_;
    jhold = *jj;
/*  ADJUST THE COUNTER IF NEEDED */
    if (*jj > *n19) {
	*n19 = *jj;
/*  GET THE CORRECT LINE NUMBER IF MERGING */
    } else if (*merge) {
	addlnk = FALSE_;
	ltsort_(ml, &linkl[3], jj, &ipntr, &addlnk);
	if (ipntr > 0) {
	    if (jhold > *nholdl) {
		*nholdl = jhold;
	    }
	    ltsort_(ml, &iholdl[3], &jhold, &ipntr, &addlnk);
	    if (ipntr > 0) {
		*jj = ipntr;
	    } else {
		*jj = *n19 + 1;
		*n19 = *jj;
		s_wsfe(&io___4);
		do_fio(&c__1, (char *)&jhold, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jj), (ftnlen)sizeof(integer));
		e_wsfe();
		addlnk = TRUE_;
		ltsort_(ml, &iholdl[3], &jhold, jj, &addlnk);
	    }
	}
    }
/*  INPUT THE LINE DATA */
    ++(*n2);
    j = *n2;
    if (j > *ml) {
	return 0;
    }
    addlnk = TRUE_;
    ltsort_(ml, &linkl[3], jj, &j, &addlnk);
    iline[j] = *jj;
    ltype[j] = *ltyp;
    lcon[j * 3 + 1] = *ip1;
    lcon[j * 3 + 2] = *ip2;
    lcon[j * 3 + 3] = *ip3;
    if (*nn < 0) {
	nint[j] = -(*nn);
	s_wsfe(&io___6);
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	nint[j] = *nn;
    }
    factor[j] = *fact;
    if (factor[j] <= (float)0.) {
	factor[j] = (float)1.;
    }
    ilboun[j] = 0;
    isboun[j] = 0;
    *noroom = FALSE_;
    return 0;
} /* inline_ */

#ifdef __cplusplus
	}
#endif

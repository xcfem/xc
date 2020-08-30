/* ingrpn.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int ingrpn_(integer *ms, integer *mr, integer *n7, integer *
	n8, integer *n22, integer *jj, integer *iin, integer *ifound, integer 
	*iregn, integer *nspr, integer *ifside, integer *islist, integer *
	linkr, integer *nholdr, integer *iholdr, integer *irgflg, logical *
	merge, logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 OLD GROUP NO:\002,i5,\002 TO NEW GROUP \
NO:\002,i5)";
    static char fmt_10010[] = "(\002 GROUP:\002,i5,\002 HAS LESS THAN ONE RE\
GION\002,/,\002 THIS GROUP WILL NOT BE INPUT INTO DATABASE\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, iz, jhold, ipntr;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___8 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INGRPN = INPUTS A REGION INTO THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --irgflg;
    iholdr -= 3;
    linkr -= 3;
    --islist;
    --ifside;
    --nspr;
    --iregn;
    --iin;

    /* Function Body */
    iz = 0;
    *noroom = TRUE_;
    addlnk = FALSE_;
/*  ZERO THE LINK ARRAY IF NEEDED */
    if (*jj > *n22) {
	*n22 = *jj;
/*  SET UP POINTERS FOR MERGING DATA */
    } else if (*merge) {
	jhold = *jj;
	ltsort_(mr, &linkr[3], jj, &ipntr, &addlnk);
	if (ipntr > 0) {
	    if (jhold > *nholdr) {
		*nholdr = jhold;
	    }
	    ltsort_(mr, &iholdr[3], &jhold, &ipntr, &addlnk);
	    if (ipntr > 0) {
		*jj = ipntr;
	    } else {
		*jj = *n22 + 1;
		*n22 = *jj;
		s_wsfe(&io___5);
		do_fio(&c__1, (char *)&jhold, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jj), (ftnlen)sizeof(integer));
		e_wsfe();
		addlnk = TRUE_;
		ltsort_(mr, &iholdr[3], &jhold, jj, &addlnk);
	    }
	}
    }
/*  ADD THE REGION INTO THE DATABASE */
    ++(*n7);
    j = *n7;
    if (j > *mr) {
	return 0;
    }
    addlnk = TRUE_;
    ltsort_(mr, &linkr[3], jj, &j, &addlnk);
    iregn[j] = *jj;
    ifside[j] = *n8 + 1;
    irgflg[j] = 1;
    i__1 = *ifound;
    for (i__ = 1; i__ <= i__1; ++i__) {
	*jj = iin[i__];
	if (*jj == 0) {
	    goto L110;
	}
	++(*n8);
	if (*n8 > *mr << 2) {
	    return 0;
	}
	islist[*n8] = *jj;
/* L100: */
    }
L110:
    nspr[j] = *n8 - ifside[j] + 1;
    if (nspr[j] < 1) {
	s_wsfe(&io___8);
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	e_wsfe();
	ltsort_(mr, &linkr[3], &iregn[j], &iz, &addlnk);
    }
    *noroom = FALSE_;
    return 0;
} /* ingrpn_ */

#ifdef __cplusplus
	}
#endif

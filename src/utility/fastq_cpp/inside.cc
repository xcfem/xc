/* inside.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inside_(integer *ms, integer *n3, integer *n4, integer *
	n20, integer *jj, integer *iin, integer *ifound, integer *iside, 
	integer *nlps, integer *ifline, integer *illist, integer *links, 
	integer *nholds, integer *iholds, logical *merge, logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002   OLD SIDE NO:\002,i5,\002   TO NEW SID\
E NO:\002,i5)";
    static char fmt_10010[] = "(\002 SIDE:\002,i5,\002 HAS LESS THAN ONE L\
INE\002,/,\002 THIS SIDE WILL NOT BE INPUT INTO DATABASE\002)";

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
/*  SUBROUTINE INSIDE = INPUTS A SIDE INTO THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    iholds -= 3;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --iside;
    --iin;

    /* Function Body */
    iz = 0;
    *noroom = TRUE_;
/*  ZERO OUT THE LINK ARRAY IF NEEDED */
    if (*jj > *n20) {
	*n20 = *jj;
/*  FIND THE CORRECT LINE NUMBER IF MERGING */
    } else if (*merge) {
	addlnk = FALSE_;
	ltsort_(ms, &links[3], jj, &ipntr, &addlnk);
	jhold = *jj;
	if (ipntr > 0) {
	    if (jhold > *nholds) {
		*nholds = jhold;
	    }
	    ltsort_(ms, &iholds[3], &jhold, &ipntr, &addlnk);
	    if (ipntr > 0) {
		*jj = ipntr;
	    } else {
		*jj = *n20 + 1;
		++(*n20);
		s_wsfe(&io___5);
		do_fio(&c__1, (char *)&jhold, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jj), (ftnlen)sizeof(integer));
		e_wsfe();
		addlnk = TRUE_;
		ltsort_(ms, &iholds[3], &jhold, jj, &addlnk);
	    }
	}
    }
/*  INPUT THE SIDE DATA INTO THE DATABASE */
    ++(*n3);
    j = *n3;
    if (j > *ms) {
	return 0;
    }
    addlnk = TRUE_;
    ltsort_(ms, &links[3], jj, &j, &addlnk);
    iside[j] = *jj;
    ifline[j] = *n4 + 1;
    i__1 = *ifound;
    for (i__ = 1; i__ <= i__1; ++i__) {
	*jj = iin[i__];
	if (*jj == 0) {
	    goto L110;
	}
	++(*n4);
	if (*n4 > *ms * 3) {
	    return 0;
	}
	illist[*n4] = *jj;
/* L100: */
    }
L110:
    nlps[j] = *n4 - ifline[j] + 1;
    if (nlps[j] < 1) {
	s_wsfe(&io___8);
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	e_wsfe();
	ltsort_(ms, &links[3], &iside[j], &iz, &addlnk);
    }
    *noroom = FALSE_;
    return 0;
} /* inside_ */

#ifdef __cplusplus
	}
#endif

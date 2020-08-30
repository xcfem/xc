/* inintr.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inintr_(integer *ml, integer *ms, integer *ifound, 
	integer *newint, integer *iin, integer *n19, integer *n20, integer *
	nint, integer *nlps, integer *ifline, integer *illist, integer *linkl,
	 integer *links, logical *addlnk)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 SIDE NO:\002,i5,\002 IS NOT IN THE DATA\
BASE\002,/,\002 THUS NO INTERVALS CAN BE ENTERED\002)";
    static char fmt_10010[] = "(\002 LINES IN SIDE NO:\002,i5,\002 CANNOT HA\
VE INTERVALS OF:\002,i8)";
    static char fmt_10020[] = "(\002 LINE NO:\002,i5,\002 IS NOT IN THE DATA\
BASE\002,/,\002 THUS NO INTERVAL CAN BE ENTERED\002)";
    static char fmt_10030[] = "(\002 LINE NO:\002,i5,\002 CANNOT HAVE AN INT\
ERVAL OF:\002,i8)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, jj, jjj, ipntr, jpntr;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___5 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___8 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___9 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___10 = { 0, 6, 0, fmt_10030, 0 };


/* *********************************************************************** */
/*  SUBROUTINE ININTR = ENTERS INTERVALS INTO THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkl -= 3;
    --nint;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --iin;

    /* Function Body */
    i__1 = *ifound;
    for (i__ = 1; i__ <= i__1; ++i__) {
	jj = iin[i__];
/*  APPLY INTERVALS TO A SIDE */
	if (jj < 0) {
	    jj = -jj;
	    ltsort_(ms, &links[3], &jj, &ipntr, addlnk);
	    if (jj > *n20 || ipntr <= 0) {
		s_wsfe(&io___4);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
	    } else if (*newint < 0) {
		s_wsfe(&io___5);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iin[2], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		i__2 = ifline[ipntr] + nlps[ipntr] - 1;
		for (jjj = ifline[ipntr]; jjj <= i__2; ++jjj) {
		    ltsort_(ml, &linkl[3], &illist[jjj], &jpntr, addlnk);
		    if (illist[jjj] > *n19 || ipntr <= 0) {
			s_wsfe(&io___8);
			do_fio(&c__1, (char *)&illist[jjj], (ftnlen)sizeof(
				integer));
			e_wsfe();
		    } else {
			nint[jpntr] = *newint;
		    }
/* L100: */
		}
	    }
	} else {
/*  INPUT INTERVALS ON A LINE */
	    ltsort_(ml, &linkl[3], &jj, &ipntr, addlnk);
	    if (jj > *n19 || ipntr <= 0) {
		s_wsfe(&io___9);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
	    } else if (*newint < 0) {
		s_wsfe(&io___10);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*newint), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		nint[ipntr] = *newint;
	    }
	}
/* L110: */
    }
    return 0;
} /* inintr_ */

#ifdef __cplusplus
	}
#endif

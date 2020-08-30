/* listbf.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int listbf_(integer *mdim, integer *n, char *choice, integer 
	*link, integer *iflag, integer *inum, integer *ifirst, integer *list, 
	integer *iwt, ftnlen choice_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002  FLAG    BOUN.  WEIGHT FIRST  FIRST\002\
,/,\002 NUMBER   TYPE    SIDE  WT PNT WT LIN     POINT OR LINE \002//\002LIS\
TING\002,/,\002 ------ -------- ------ ------ ------ \002,\002--------------\
--------------------\002)";
    static char fmt_10010[] = "(\002 \002,37x,6i6)";
    static char fmt_10020[] = "(\002 \002,1x,i5,2x,a7,2x,\002-----\002,2x\
,\002-----\002,2x,\002-----\002,2x,6i6)";
    static char fmt_10030[] = "(\002 \002,1x,i5,2x,a7,2x,i5,2x,i5,2x,\002---\
--\002,2x,6i6)";
    static char fmt_10040[] = "(\002 \002,1x,i5,2x,a7,2x,i5,2x,i5,2x,i5,2x,6\
i6)";
    static char fmt_10050[] = "(\002 *** NO \002,a7,\002 FLAGS IN THE CURREN\
T DATABASE ***\002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen), s_wsfe(cilist *), e_wsfe(),
	     do_fio(integer *, char *, ftnlen);

    /* Local variables */
    static integer i__, l, l1, l2;
    static logical found, extra;
    static integer ipntr;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___9 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___11 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___12 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___13 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___14 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___15 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___17 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___18 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___19 = { 0, 6, 0, fmt_10050, 0 };


/* *********************************************************************** */
/*  SUBROUTINE LISTBF = LISTS BOUNDARY CONDITIONS BY FLAG NUMBERS */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     LIST = LISTS POINTS,  LINES,  REGIONS,  SCHEMES,  AND BOUNDARY */
/*            DEFINITIONS */
/* *********************************************************************** */
    /* Parameter adjustments */
    iwt -= 4;
    list -= 3;
    --ifirst;
    --inum;
    --iflag;
    link -= 3;

    /* Function Body */
    addlnk = FALSE_;
    found = FALSE_;
    if (s_cmp(choice, "POINT", (ftnlen)5, (ftnlen)5) == 0) {
	s_wsfe(&io___3);
	e_wsfe();
    }
    if (*n > 0) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ltsort_(mdim, &link[3], &i__, &ipntr, &addlnk);
	    if (ipntr > 0) {
		found = TRUE_;
		extra = FALSE_;
		l1 = ifirst[ipntr];
		l2 = l1 + inum[ipntr] - 1;
L100:
		if (l2 - l1 + 1 > 6) {
		    l2 = l1 + 5;
		    if (extra) {
			s_wsfe(&io___9);
			i__2 = l2;
			for (l = l1; l <= i__2; ++l) {
			    do_fio(&c__1, (char *)&list[(l << 1) + 1], (
				    ftnlen)sizeof(integer));
			}
			e_wsfe();
		    } else {
			if (iwt[ipntr * 3 + 1] == 0) {
			    s_wsfe(&io___11);
			    do_fio(&c__1, (char *)&iflag[ipntr], (ftnlen)
				    sizeof(integer));
			    do_fio(&c__1, choice, (ftnlen)7);
			    i__2 = l2;
			    for (l = l1; l <= i__2; ++l) {
				do_fio(&c__1, (char *)&list[(l << 1) + 1], (
					ftnlen)sizeof(integer));
			    }
			    e_wsfe();
			} else {
			    if (iwt[ipntr * 3 + 3] == 0) {
				s_wsfe(&io___12);
				do_fio(&c__1, (char *)&iflag[ipntr], (ftnlen)
					sizeof(integer));
				do_fio(&c__1, choice, (ftnlen)7);
				do_fio(&c__1, (char *)&iwt[ipntr * 3 + 1], (
					ftnlen)sizeof(integer));
				do_fio(&c__1, (char *)&iwt[ipntr * 3 + 2], (
					ftnlen)sizeof(integer));
				i__2 = l2;
				for (l = l1; l <= i__2; ++l) {
				    do_fio(&c__1, (char *)&list[(l << 1) + 1],
					     (ftnlen)sizeof(integer));
				}
				e_wsfe();
			    } else {
				s_wsfe(&io___13);
				do_fio(&c__1, (char *)&iflag[ipntr], (ftnlen)
					sizeof(integer));
				do_fio(&c__1, choice, (ftnlen)7);
				do_fio(&c__1, (char *)&iwt[ipntr * 3 + 1], (
					ftnlen)sizeof(integer));
				do_fio(&c__1, (char *)&iwt[ipntr * 3 + 2], (
					ftnlen)sizeof(integer));
				do_fio(&c__1, (char *)&iwt[ipntr * 3 + 3], (
					ftnlen)sizeof(integer));
				i__2 = l2;
				for (l = l1; l <= i__2; ++l) {
				    do_fio(&c__1, (char *)&list[(l << 1) + 1],
					     (ftnlen)sizeof(integer));
				}
				e_wsfe();
			    }
			}
			extra = TRUE_;
		    }
		    l1 = l2 + 1;
		    l2 = ifirst[ipntr] + inum[ipntr] - 1;
		    goto L100;
		} else {
		    if (extra) {
			s_wsfe(&io___14);
			i__2 = l2;
			for (l = l1; l <= i__2; ++l) {
			    do_fio(&c__1, (char *)&list[(l << 1) + 1], (
				    ftnlen)sizeof(integer));
			}
			e_wsfe();
		    } else {
			if (iwt[ipntr * 3 + 1] == 0) {
			    s_wsfe(&io___15);
			    do_fio(&c__1, (char *)&iflag[ipntr], (ftnlen)
				    sizeof(integer));
			    do_fio(&c__1, choice, (ftnlen)7);
			    i__2 = l2;
			    for (l = l1; l <= i__2; ++l) {
				do_fio(&c__1, (char *)&list[(l << 1) + 1], (
					ftnlen)sizeof(integer));
			    }
			    e_wsfe();
			} else {
			    if (iwt[ipntr * 3 + 3] == 0) {
				s_wsfe(&io___16);
				do_fio(&c__1, (char *)&iflag[ipntr], (ftnlen)
					sizeof(integer));
				do_fio(&c__1, choice, (ftnlen)7);
				do_fio(&c__1, (char *)&iwt[ipntr * 3 + 1], (
					ftnlen)sizeof(integer));
				do_fio(&c__1, (char *)&iwt[ipntr * 3 + 2], (
					ftnlen)sizeof(integer));
				i__2 = l2;
				for (l = l1; l <= i__2; ++l) {
				    do_fio(&c__1, (char *)&list[(l << 1) + 1],
					     (ftnlen)sizeof(integer));
				}
				e_wsfe();
			    } else {
				s_wsfe(&io___17);
				do_fio(&c__1, (char *)&iflag[ipntr], (ftnlen)
					sizeof(integer));
				do_fio(&c__1, choice, (ftnlen)7);
				do_fio(&c__1, (char *)&iwt[ipntr * 3 + 1], (
					ftnlen)sizeof(integer));
				do_fio(&c__1, (char *)&iwt[ipntr * 3 + 2], (
					ftnlen)sizeof(integer));
				do_fio(&c__1, (char *)&iwt[ipntr * 3 + 3], (
					ftnlen)sizeof(integer));
				i__2 = l2;
				for (l = l1; l <= i__2; ++l) {
				    do_fio(&c__1, (char *)&list[(l << 1) + 1],
					     (ftnlen)sizeof(integer));
				}
				e_wsfe();
			    }
			}
		    }
		}
	    }
/* L110: */
	}
    } else {
	found = TRUE_;
	s_wsfe(&io___18);
	do_fio(&c__1, choice, (ftnlen)7);
	e_wsfe();
    }
    if (! found) {
	s_wsfe(&io___19);
	do_fio(&c__1, choice, (ftnlen)7);
	e_wsfe();
    }
    return 0;
} /* listbf_ */

#ifdef __cplusplus
	}
#endif

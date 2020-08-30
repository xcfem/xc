/* inrenm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inrenm_(integer *msc, integer *n23, char *cflag, real *
	rin, integer *iin, integer *ifound, char *number, logical *noroom, 
	ftnlen cflag_len, ftnlen number_len)
{
    /* Format strings */
    static char fmt_10000[] = "(i5)";
    static char fmt_10010[] = "(1pe10.3)";
    static char fmt_10020[] = "(i10)";
    static char fmt_10030[] = "(\002 RENUMBERING KEY WORD: \002,a5,\002 IS N\
OT ALLOWABLE\002,/,\002 THIS RENUMBERING LIST WILL NOT BE INPUT INTO DATABASE\
\002)";

    /* System generated locals */
    integer i__1;
    icilist ici__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_cmp(char *, char *, ftnlen, ftnlen), s_wsfi(icilist *), do_fio(
	    integer *, char *, ftnlen), e_wsfi(), s_wsfe(cilist *), e_wsfe();

    /* Local variables */
    static integer i1, i2, ij;

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10030, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INRENM = INPUTS A RENUMBERING CARD */
/* *********************************************************************** */
    /* Parameter adjustments */
    number -= 80;
    --iin;
    --rin;

    /* Function Body */
    *noroom = TRUE_;
    ++(*n23);
    if (*n23 > *msc) {
	return 0;
    }
    s_copy(number + *n23 * 80, " ", (ftnlen)80, (ftnlen)1);
    s_copy(number + *n23 * 80, cflag, (ftnlen)5, (ftnlen)5);
/*  INPUT A POINT - LINE - POINT CARD */
    if (s_cmp(cflag, "P-L-P", (ftnlen)5, (ftnlen)5) == 0) {
	*ifound = min(*ifound,15);
	i__1 = *ifound;
	for (ij = 1; ij <= i__1; ++ij) {
	    i2 = (ij + 1) * 5;
	    i1 = i2 - 4;
	    ici__1.icierr = 0;
	    ici__1.icirnum = 1;
	    ici__1.icirlen = i2 - (i1 - 1);
	    ici__1.iciunit = number + (*n23 * 80 + (i1 - 1));
	    ici__1.icifmt = fmt_10000;
	    s_wsfi(&ici__1);
	    do_fio(&c__1, (char *)&iin[ij], (ftnlen)sizeof(integer));
	    e_wsfi();
/* L100: */
	}
/*  INPUT AN X, Y LOCATION RENUMBERING CARD */
    } else if (s_cmp(cflag, "X-Y", (ftnlen)3, (ftnlen)3) == 0) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 10;
	ici__1.iciunit = number + (*n23 * 80 + 10);
	ici__1.icifmt = fmt_10010;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&rin[1], (ftnlen)sizeof(real));
	e_wsfi();
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 10;
	ici__1.iciunit = number + (*n23 * 80 + 20);
	ici__1.icifmt = fmt_10010;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&rin[2], (ftnlen)sizeof(real));
	e_wsfi();
/*  INPUT A NODE UNIQUE ID RENUMBERING CARD */
    } else if (s_cmp(cflag, "NODE", (ftnlen)4, (ftnlen)4) == 0) {
	*ifound = min(*ifound,7);
	i__1 = *ifound;
	for (ij = 1; ij <= i__1; ++ij) {
	    i2 = (ij + 1) * 10;
	    i1 = i2 - 9;
	    ici__1.icierr = 0;
	    ici__1.icirnum = 1;
	    ici__1.icirlen = i2 - (i1 - 1);
	    ici__1.iciunit = number + (*n23 * 80 + (i1 - 1));
	    ici__1.icifmt = fmt_10020;
	    s_wsfi(&ici__1);
	    do_fio(&c__1, (char *)&iin[ij], (ftnlen)sizeof(integer));
	    e_wsfi();
/* L110: */
	}
/*  INDICATE ERROR IN RENUMBERING FLAG */
    } else {
	--(*n23);
	s_wsfe(&io___4);
	do_fio(&c__1, cflag, (ftnlen)5);
	e_wsfe();
    }
    *noroom = FALSE_;
    return 0;
} /* inrenm_ */

#ifdef __cplusplus
	}
#endif

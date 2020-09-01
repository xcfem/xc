/* getdum.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int getdum_(integer *i__, char *dummy, integer *len, ftnlen 
	dummy_len)
{
    /* Format strings */
    static char fmt_10050[] = "(i6)";
    static char fmt_10040[] = "(i5)";
    static char fmt_10030[] = "(i4)";
    static char fmt_10020[] = "(i3)";
    static char fmt_10010[] = "(i2)";
    static char fmt_10000[] = "(i1)";

    /* System generated locals */
    icilist ici__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi();

/* *********************************************************************** */
/*  SUBROUTINE GETDUM = GETS AN INTEGER INTO A DUMMY CHARACTER STRING */
/* *********************************************************************** */
    s_copy(dummy, " ", (ftnlen)72, (ftnlen)1);
    if (*i__ < -9999) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 6;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10050;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	e_wsfi();
	*len = 6;
    } else if (*i__ < -999) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 5;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10040;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	e_wsfi();
	*len = 5;
    } else if (*i__ < -99) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 4;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10030;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	e_wsfi();
	*len = 4;
    } else if (*i__ < -9) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 3;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10020;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	e_wsfi();
	*len = 3;
    } else if (*i__ < 0) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 2;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10010;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	e_wsfi();
	*len = 6;
    } else if (*i__ < 10) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 1;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10000;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	e_wsfi();
	*len = 1;
    } else if (*i__ < 100) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 2;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10010;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	e_wsfi();
	*len = 2;
    } else if (*i__ < 1000) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 3;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10020;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	e_wsfi();
	*len = 3;
    } else if (*i__ < 10000) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 4;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10030;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	e_wsfi();
	*len = 4;
    } else {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 5;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10040;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	e_wsfi();
	*len = 5;
    }
    return 0;
} /* getdum_ */

#ifdef __cplusplus
	}
#endif

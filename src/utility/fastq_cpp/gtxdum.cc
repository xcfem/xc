/* gtxdum.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int gtxdum_(real *x, char *dummy, integer *len, ftnlen 
	dummy_len)
{
    /* Format strings */
    static char fmt_10000[] = "(1pe10.3)";
    static char fmt_10010[] = "(f6.0)";
    static char fmt_10020[] = "(f6.1)";
    static char fmt_10030[] = "(f6.2)";
    static char fmt_10040[] = "(f6.3)";
    static char fmt_10050[] = "(f6.4)";
    static char fmt_10060[] = "(1pe10.3)";
    static char fmt_10070[] = "(f5.0)";
    static char fmt_10080[] = "(f5.1)";
    static char fmt_10090[] = "(f5.2)";
    static char fmt_10100[] = "(f5.3)";
    static char fmt_10110[] = "(f5.4)";

    /* System generated locals */
    icilist ici__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi();

/* *********************************************************************** */
/*  SUBROUTINE GTXDUM = GETS A REAL INTO A DUMMY CHARACTER STRING */
/* *********************************************************************** */
    s_copy(dummy, " ", (ftnlen)72, (ftnlen)1);
    if (*x <= (float)-1e4) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 10;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10000;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 10;
    } else if (*x <= (float)-1e3) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 6;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10010;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 6;
    } else if (*x <= (float)-100.) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 6;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10020;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 6;
    } else if (*x <= (float)-10.) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 6;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10030;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 6;
    } else if (*x <= (float)-1.) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 6;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10040;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 6;
    } else if (*x < (float)0.) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 6;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10050;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 6;
    } else if (*x >= (float)1e4) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 10;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10060;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 9;
    } else if (*x >= (float)1e3) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 5;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10070;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 5;
    } else if (*x >= (float)100.) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 5;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10080;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 5;
    } else if (*x >= (float)10.) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 5;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10090;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 5;
    } else if (*x >= (float)1.) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 5;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10100;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 5;
    } else {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 5;
	ici__1.iciunit = dummy;
	ici__1.icifmt = fmt_10110;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*x), (ftnlen)sizeof(real));
	e_wsfi();
	*len = 5;
    }
    return 0;
} /* gtxdum_ */

#ifdef __cplusplus
	}
#endif

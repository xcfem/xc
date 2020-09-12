/* chric.f -- translated by f2c (version 20160102).
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
//#include "f2c.h"
#include "paving.h"

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int chric_(integer *jin, char *ds, integer *nd, ftnlen 
	ds_len)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, j, kd, ij;
    static char sdg[16];

    s_copy(ds, " ", ds_len, (ftnlen)1);
    j = abs(*jin);
    *nd = 0;
L2030:
    ++(*nd);
    kd = j % 10;
    j /= 10;
    *(unsigned char *)&sdg[*nd - 1] = (char) ('0' + kd);
    if (! (j == 0)) {
	goto L2030;
    }
    ij = 1;
    if (*jin < 0) {
	ij = 2;
	*(unsigned char *)ds = '-';
	++(*nd);
    }
    i__ = ij;
L2060:
    if (! (i__ <= *nd)) {
	goto L2080;
    }
    i__1 = *nd - i__;
    s_copy(ds + (i__ - 1), sdg + i__1, (ftnlen)1, *nd - i__ + 1 - i__1);
    ++i__;
    goto L2060;
L2080:
    return 0;
} /* chric_ */

#ifdef __cplusplus
	}
#endif

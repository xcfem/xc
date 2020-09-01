/* chrup.f -- translated by f2c (version 20160102).
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

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int chrup_(char *line1, char *line2, ftnlen line1_len, 
	ftnlen line2_len)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer i_len(char *, ftnlen);

    /* Local variables */
    static integer i__, l, ich;

/* Computing MIN */
    i__1 = i_len(line1, line1_len), i__2 = i_len(line2, line2_len);
    l = min(i__1,i__2);
    i__ = 1;
L2210:
    if (! (i__ <= l)) {
	goto L2230;
    }
    ich = *(unsigned char *)&line1[i__ - 1];
    if (ich >= 97 && ich <= 122) {
	ich += -32;
    }
    *(unsigned char *)&line2[i__ - 1] = (char) ich;
    ++i__;
    goto L2210;
L2230:
    return 0;
} /* chrup_ */

#ifdef __cplusplus
	}
#endif

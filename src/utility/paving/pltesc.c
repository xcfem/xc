/* pltesc.f -- translated by f2c (version 20160102).
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
#include "paving.h"

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltesc_(char *text, integer *i__, char *esc, ftnlen 
	text_len, ftnlen esc_len)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer i_len(char *, ftnlen);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer j, ip, lt;

    ip = *i__ + 1;
    lt = i_len(text, text_len);
    if (*(unsigned char *)&text[ip - 1] == '^') {
	s_copy(esc, "^", esc_len, (ftnlen)1);
	++(*i__);
	return 0;
    } else if (*(unsigned char *)&text[ip - 1] == '-') {
	s_copy(esc, "-", esc_len, (ftnlen)1);
	++(*i__);
	return 0;
    } else if (*(unsigned char *)&text[ip - 1] == '_') {
	s_copy(esc, "_", esc_len, (ftnlen)1);
	++(*i__);
	return 0;
    }
    i__1 = lt;
    for (j = ip; j <= i__1; ++j) {
	if (*(unsigned char *)&text[j - 1] == ' ' || *(unsigned char *)&text[
		j - 1] == '\\') {
	    goto L2100;
	}
/* L2090: */
    }
L2100:
    s_copy(esc, text + (ip - 1), esc_len, j - 1 - (ip - 1));
    *i__ = j;
    if (*i__ >= lt) {
	return 0;
    }
    if (*(unsigned char *)&text[j - 1] == '\\') {
	*i__ = j - 1;
    }
    return 0;
} /* pltesc_ */

#ifdef __cplusplus
	}
#endif

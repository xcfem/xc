/* chrcmp.f -- translated by f2c (version 20160102).
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
logical chrcmp_(char *kwd, char *part1, char *part2, ftnlen kwd_len, ftnlen 
	part1_len, ftnlen part2_len)
{
    /* System generated locals */
    integer i__1;
    logical ret_val;

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer lf, lk, lv;
    extern /* Subroutine */ int chrtrm_(char *, integer *, ftnlen);

    chrtrm_(kwd, &lk, kwd_len);
    chrtrm_(part1, &lf, part1_len);
    chrtrm_(part2, &lv, part2_len);
    if (lk < lf || lk > lf + lv) {
	ret_val = FALSE_;
	return ret_val;
    }
    if (s_cmp(kwd, part1, lf, lf) != 0) {
	ret_val = FALSE_;
	return ret_val;
    }
    if (lk == lf) {
	ret_val = TRUE_;
	return ret_val;
    }
    i__1 = lf;
    if (s_cmp(kwd + i__1, part2, lk - i__1, lk - lf) != 0) {
	ret_val = FALSE_;
	return ret_val;
    }
    ret_val = TRUE_;
    return ret_val;
} /* chrcmp_ */

#ifdef __cplusplus
	}
#endif

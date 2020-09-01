/* chrtrm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int chrtrm_(char *line, integer *l, ftnlen line_len)
{
    /* Builtin functions */
    integer i_len(char *, ftnlen);

    /* Local variables */
    static integer l1;
    static char ch[1];

    l1 = i_len(line, line_len);
L2180:
    if (! (l1 > 0)) {
	goto L2200;
    }
    *(unsigned char *)ch = *(unsigned char *)&line[l1 - 1];
    if (*(unsigned char *)ch == ' ') {
	goto L2190;
    }
    if (*(unsigned char *)ch == '\t') {
	goto L2190;
    }
    if (*(unsigned char *)ch == '\0') {
	goto L2190;
    }
    goto L2200;
L2190:
    --l1;
    goto L2180;
L2200:
    *l = l1;
    return 0;
} /* chrtrm_ */

#ifdef __cplusplus
	}
#endif

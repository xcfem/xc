/* strlng.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int strlng_(char *string, integer *len, ftnlen string_len)
{
    static integer ileft;
    extern /* Subroutine */ int stripb_(char *, integer *, integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE STRLNG = FINDS THE NO. OF NONBLANK STRING CHARACTERS */
/* *********************************************************************** */
    stripb_(string, &ileft, len, string_len);
    if (*len == 0) {
	*len = 1;
    }
    return 0;
} /* strlng_ */

#ifdef __cplusplus
	}
#endif

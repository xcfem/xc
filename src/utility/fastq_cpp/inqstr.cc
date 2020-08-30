/* inqstr.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inqstr_(char *prompt, char *ians, ftnlen prompt_len, 
	ftnlen ians_len)
{
    /* Format strings */
    static char fmt_10010[] = "(\002 BAD CHARACTER STRING  -  TRY AGAIN\002)";

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), e_wsfe();

    /* Local variables */
    static integer iz;
    static char hold[80];
    extern /* Subroutine */ int getinp_(integer *, integer *, char *, char *, 
	    integer *, ftnlen, ftnlen);
    static integer iostat;
    extern /* Subroutine */ int strcut_(char *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INQSTR = INPUTS CHARACTER STRINGS */
/* *********************************************************************** */
    iz = 0;
L100:
    getinp_(&iz, &iz, prompt, hold, &iostat, prompt_len, (ftnlen)80);
    if (iostat == 0) {
	strcut_(hold, (ftnlen)80);
	s_copy(ians, hold, ians_len, (ftnlen)80);
	return 0;
    } else if (iostat < 0) {
	s_copy(ians, " ", ians_len, (ftnlen)1);
	return 0;
    } else if (iostat > 0) {
	s_wsfe(&io___4);
	e_wsfe();
	goto L100;
    }
    return 0;
} /* inqstr_ */

#ifdef __cplusplus
	}
#endif

/* mesage.f -- translated by f2c (version 20160102).
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
#include <stdio.h>
  
/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int mesage_(const char *msg, ftnlen prompt_len)
  {
    printf("%s", msg);
//     /* Format strings */
//     static char fmt_10000[] = "(/)";
//     static char fmt_10010[] = "(\002 \002,a)";

//     /* Builtin functions */
//     integer s_cmp(char *, char *, ftnlen, ftnlen), s_wsfe(cilist *), e_wsfe(),
// 	     do_fio(integer *, char *, ftnlen);

//     /* Fortran I/O blocks */
//     static cilist io___1 = { 0, 6, 0, fmt_10000, 0 };
//     static cilist io___2 = { 0, 6, 0, fmt_10010, 0 };


// /* *********************************************************************** */
// /*  SUBROUTINE MESAGE = PRINTS A MESSAGE ONTO THE SCREEN */
// /* *********************************************************************** */
//     if (s_cmp(prompt, " ", prompt_len, (ftnlen)1) == 0) {
// 	s_wsfe(&io___1);
// 	e_wsfe();
//     } else {
// 	s_wsfe(&io___2);
// 	do_fio(&c__1, prompt, prompt_len);
// 	e_wsfe();
//     }
//     return 0;
} /* mesage_ */

#ifdef __cplusplus
	}
#endif

/* inqtru.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inqtru_(char *prompt, logical *ians, ftnlen prompt_len)
{
    /* Initialized data */

    static char ans[1*4+1] = "YyNn";

    /* Format strings */
    static char fmt_10000[] = "(\002 \002,a,\002? \002)";
    static char fmt_10010[] = "(a1)";
    static char fmt_10020[] = "(\002 RESPONSE MUST BE EITHER YES OR NO  -  T\
RY AGAIN\002)";
    static char fmt_10030[] = "(\002 END OF DATA ENCOUNTERED  -  TRY AGAI\
N\002)";
    static char fmt_10040[] = "(\002 ERROR IN RESPONSE  -  TRY AGAIN\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(), 
	    s_rsfe(cilist *), e_rsfe();

    /* Local variables */
    static char result[1];

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___3 = { 1, 5, 1, fmt_10010, 0 };
    static cilist io___5 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___6 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___7 = { 0, 6, 0, fmt_10040, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INQTRU = INPUTS A YES OR NO ANSWER */
/* *********************************************************************** */
L100:
    s_wsfe(&io___2);
    do_fio(&c__1, prompt, prompt_len);
    e_wsfe();
    i__1 = s_rsfe(&io___3);
    if (i__1 != 0) {
	goto L100001;
    }
    i__1 = do_fio(&c__1, result, (ftnlen)1);
    if (i__1 != 0) {
	goto L100001;
    }
    i__1 = e_rsfe();
L100001:
    if (i__1 < 0) {
	goto L110;
    }
    if (i__1 > 0) {
	goto L120;
    }
    if (*(unsigned char *)result == *(unsigned char *)&ans[0] || *(unsigned 
	    char *)result == *(unsigned char *)&ans[1]) {
	*ians = TRUE_;
    } else if (*(unsigned char *)result == *(unsigned char *)&ans[2] || *(
	    unsigned char *)result == *(unsigned char *)&ans[3]) {
	*ians = FALSE_;
    } else {
	s_wsfe(&io___5);
	e_wsfe();
	goto L100;
    }
    return 0;
L110:
    s_wsfe(&io___6);
    e_wsfe();
    goto L100;
L120:
    s_wsfe(&io___7);
    e_wsfe();
    goto L100;
} /* inqtru_ */

#ifdef __cplusplus
	}
#endif

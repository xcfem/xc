/* inschm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inschm_(integer *mr, integer *msc, integer *n8, integer *
	n19, integer *jj, char *dummy, integer *ischm, char *scheme, integer *
	linksc, char *defsch, logical *noroom, logical *dolink, ftnlen 
	dummy_len, ftnlen scheme_len, ftnlen defsch_len)
{
    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer j;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE INSCHM = INPUTS A SCHEME INTO THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    linksc -= 3;
    scheme -= 72;
    --ischm;

    /* Function Body */
    *noroom = TRUE_;
    addlnk = TRUE_;
/*  ENTER THE DEFAULT SCHEME IF THE REGION NUMBER IS ZERO */
    if (*jj == 0) {
	s_copy(defsch, dummy, (ftnlen)72, (ftnlen)72);
/*  ENTER THE SCHEME */
    } else {
	if (*dolink && *jj > *n19) {
	    *n19 = *jj;
	}
	++(*n8);
	j = *n8;
	if (*n8 > *msc) {
	    return 0;
	}
	ischm[j] = *jj;
	if (*dolink) {
	    ltsort_(mr, &linksc[3], jj, &j, &addlnk);
	}
	s_copy(scheme + j * 72, dummy, (ftnlen)72, (ftnlen)72);
    }
    *noroom = FALSE_;
    return 0;
} /* inschm_ */

#ifdef __cplusplus
	}
#endif

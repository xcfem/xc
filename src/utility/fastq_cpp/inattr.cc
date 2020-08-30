/* inattr.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inattr_(integer *ms, integer *mr, integer *ma, integer *
	n17, integer *n23, integer *jj, E_fp rin, integer *ifound, real *
	attrib, integer *linkm, logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 FOR MATERIAL NUMBER:\002,i5,\002 NUMBER\
 OF ATTRIBUTES READ:\002,i5,/,\002                               EXCEEDS MAX\
 ALLOWED OF:\002,i5)";

    /* System generated locals */
    integer attrib_dim1, attrib_offset, i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, iend, ipntr;
    static logical addlnk;
    static integer minusj;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INATTR = INPUTS MATERIAL ATTRIBUTES INTO THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkm -= 3;
    attrib_dim1 = *ma;
    attrib_offset = 1 + attrib_dim1;
    attrib -= attrib_offset;

    /* Function Body */
    *noroom = TRUE_;
    addlnk = FALSE_;
/*  UPDATE THE COUNTER IF NEEDED */
    if (*jj > *n23) {
	*n23 = *jj;
    }
/*  ADD THE ATTRIBUTES INTO THE DATABASE */
    ++(*n17);
    j = *n17;
    if (j > *ms + *mr) {
	return 0;
    }
    i__1 = *ms + *mr;
    ltsort_(&i__1, &linkm[3], jj, &ipntr, &addlnk);
    addlnk = TRUE_;
    if (ipntr <= 0) {
	j = -j;
	minusj = -j;
	i__1 = *ms + *mr;
	ltsort_(&i__1, &linkm[3], jj, &minusj, &addlnk);
	j = abs(minusj);
    } else {
	i__1 = *ms + *mr;
	ltsort_(&i__1, &linkm[3], jj, &j, &addlnk);
    }
    if (*ifound > *ma) {
	iend = *ma;
	s_wsfe(&io___6);
	do_fio(&c__1, (char *)&(*ifound), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*ma), (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	iend = *ifound;
    }
    i__1 = iend;
    for (i__ = 1; i__ <= i__1; ++i__) {
	attrib[j + i__ * attrib_dim1] = (*rin)(&i__);
/* L100: */
    }
    *noroom = FALSE_;
    return 0;
} /* inattr_ */

#ifdef __cplusplus
	}
#endif

/* lswap.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int lswap_(integer *mxnd, integer *lxk, integer *kxl, 
	integer *k1, integer *l1, integer *k2, integer *l2, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 ERROR IN LSWAP.  K1, L1, K2, L2 :\002,4\
i5)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, k, l;

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___6 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___7 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___8 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE LSWAP = EXCHANGE LINE L1 IN ELEMENT K1 WITH LINE L2 IN */
/*                     ELEMENT K2 */
/* *********************************************************************** */
    /* Parameter adjustments */
    kxl -= 3;
    lxk -= 5;

    /* Function Body */
    *err = TRUE_;
/*  INSERT L2 FOR L1 */
    for (i__ = 1; i__ <= 4; ++i__) {
	if (lxk[i__ + (*k1 << 2)] == *l1) {
	    lxk[i__ + (*k1 << 2)] = *l2;
/*  INSERT L1 FOR L2 */
	    for (j = 1; j <= 4; ++j) {
		if (lxk[j + (*k2 << 2)] == *l2) {
		    lxk[j + (*k2 << 2)] = *l1;
/*  INSERT K2 FOR K1 */
		    for (k = 1; k <= 2; ++k) {
			if (kxl[k + (*l1 << 1)] == *k1) {
			    kxl[k + (*l1 << 1)] = *k2;
/*  INSERT K1 FOR K2 */
			    for (l = 1; l <= 2; ++l) {
				if (kxl[l + (*l2 << 1)] == *k2) {
				    kxl[l + (*l2 << 1)] = *k1;
/*  EVERYTHING INSERTED OK */
				    *err = FALSE_;
				    return 0;
				}
/* L100: */
			    }
			    s_wsfe(&io___5);
			    do_fio(&c__1, (char *)&(*k1), (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&(*l1), (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&(*k2), (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&(*l2), (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    return 0;
			}
/* L110: */
		    }
		    s_wsfe(&io___6);
		    do_fio(&c__1, (char *)&(*k1), (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&(*l1), (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&(*k2), (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&(*l2), (ftnlen)sizeof(integer));
		    e_wsfe();
		    return 0;
		}
/* L120: */
	    }
	    s_wsfe(&io___7);
	    do_fio(&c__1, (char *)&(*k1), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*l1), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*k2), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*l2), (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
/* L130: */
    }
    s_wsfe(&io___8);
    do_fio(&c__1, (char *)&(*k1), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*l1), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*k2), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*l2), (ftnlen)sizeof(integer));
    e_wsfe();
    return 0;
} /* lswap_ */

#ifdef __cplusplus
	}
#endif

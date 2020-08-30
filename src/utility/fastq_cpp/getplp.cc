/* getplp.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getplp_(integer *npnode, integer *npelem, integer *
	maxkxn, integer *nnxk, integer *mxlist, integer *kxn, integer *nxk, 
	integer *nuid, integer *ip1, integer *line, integer *ip2, integer *
	list, integer *nlist, integer *nnn, integer *lastn, logical *noroom, 
	logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 POINT\002,i5,\002 IS NOT IN THE MESH\
\002)";
    static char fmt_10010[] = "(\002 P-L-P SEQUENCE OF\002,i5,\002 -\002,i5\
,\002 -\002,i5,\002IS AN ILLEGAL SEQUENCE\002)";

    /* System generated locals */
    integer kxn_dim1, kxn_offset, nxk_dim1, nxk_offset, i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, l, n;
    static logical all;
    static integer new__;
    extern integer indx_(integer *, integer *, integer *);
    static integer numn, nprev;
    extern /* Subroutine */ int getnxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *);
    static integer nxnlst[20];

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___10 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE GETPLP = PRODUCES THE LIST OF NODES FROM POINT IP1 */
/*                      THROUGH LINE TO POINT IP2 */
/* *********************************************************************** */
/*  NOTE: */
/*     THIS LIST WILL BE (LIST (I), I=1,NLIST) AND THESE WILL BE INDICES */
/*     INTO THE NODE TABLE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nuid;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;
    kxn_dim1 = *nnxk;
    kxn_offset = 1 + kxn_dim1;
    kxn -= kxn_offset;
    --list;

    /* Function Body */
    *err = FALSE_;
    *noroom = FALSE_;
/*  FIND FIRST POINT */
    if (*nlist == 0) {
	n = indx_(nnn, &nuid[1], ip1);
	if (n <= 0) {
	    s_wsfe(&io___2);
	    do_fio(&c__1, (char *)&(*ip1), (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    return 0;
	}
	++(*nlist);
	if (*nlist > *mxlist) {
	    *noroom = TRUE_;
	    return 0;
	}
	list[*nlist] = n;
    } else {
	*nlist = 0;
	n = *lastn;
    }
/*  FOLLOW THE LINE */
    if (*line <= 0) {
	return 0;
    }
    nprev = 0;
L100:
    all = FALSE_;
    getnxn_(npnode, npelem, maxkxn, nnxk, &kxn[kxn_offset], &nxk[nxk_offset], 
	    &nuid[1], &n, nxnlst, &numn, &all, err);
    if (*err) {
	return 0;
    }
    i__1 = numn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	new__ = nxnlst[i__ - 1];
	if (new__ != nprev && nuid[new__] >= 1000000000) {
	    l = (nuid[new__] - 1000000000) / 100000;
	    if (l == *line) {
		++(*nlist);
		if (*nlist > *mxlist) {
		    *noroom = TRUE_;
		    return 0;
		}
		list[*nlist] = new__;
		nprev = n;
		n = new__;
		goto L100;
	    }
	}
/* L110: */
    }
/*  LINE FINISHED  -  FIND IP2 */
    if (*ip2 <= 0) {
	return 0;
    }
    i__1 = numn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	new__ = nxnlst[i__ - 1];
	if (nuid[new__] == *ip2) {
	    ++(*nlist);
	    if (*nlist > *mxlist) {
		*noroom = TRUE_;
		return 0;
	    }
	    list[*nlist] = new__;
	    *lastn = new__;
	    return 0;
	}
/* L120: */
    }
/*  LINE DID NOT MATCH UP RIGHT */
    s_wsfe(&io___10);
    do_fio(&c__1, (char *)&(*ip1), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*line), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*ip2), (ftnlen)sizeof(integer));
    e_wsfe();
    *err = TRUE_;
    return 0;
} /* getplp_ */

#ifdef __cplusplus
	}
#endif

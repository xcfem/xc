/* getnxn.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getnxn_(integer *npnode, integer *npelem, integer *
	maxkxn, integer *nnxk, integer *kxn, integer *nxk, integer *nuid, 
	integer *node, integer *nlist, integer *numn, logical *all, logical *
	err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 TOO MANY NODES CONNECTED TO NODE\002,\
i5,\002, NUID  = \002,i10)";

    /* System generated locals */
    integer kxn_dim1, kxn_offset, nxk_dim1, nxk_offset, i__1, i__2, i__3;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, k, m[3], n, ik, ido, ndo, nod, nlk, num, mido, numk, 
	    klist[20];
    extern /* Subroutine */ int getkxn_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___14 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE GETNXN = GETS THE LIST OF NODES CONNECTED TO NODE */
/* *********************************************************************** */
/*  NOTE: */
/*     NODES FOR WHICH NUID (NODE) IS NEGATIVE WILL NOT BE INCLUDED. */
/*     IF ALL=.FALSE.,  ONLY DIRECTLY CONNECTED NODES WILL BE LISTED. */
/*     IF ALL=.TRUE.,  INDIRECTLY CONNECTED NODES WILL ALSO BE LISTED. */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nuid;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;
    kxn_dim1 = *nnxk;
    kxn_offset = 1 + kxn_dim1;
    kxn -= kxn_offset;
    --nlist;

    /* Function Body */
    *err = FALSE_;
    getkxn_(npnode, maxkxn, nnxk, &kxn[kxn_offset], &nuid[1], node, klist, &
	    numk, err);
    if (*err) {
	return 0;
    }
    if (*all) {
	ndo = 3;
    } else {
	ndo = 2;
    }
    num = 0;
    nod = *node;
    i__1 = numk;
    for (ik = 1; ik <= i__1; ++ik) {
	k = klist[ik - 1];
	if (nxk[k * nxk_dim1 + 1] == nod) {
	    m[0] = 4;
	    m[1] = 2;
	    m[2] = 3;
	} else if (nxk[k * nxk_dim1 + 2] == nod) {
	    m[0] = 1;
	    m[1] = 3;
	    m[2] = 4;
	} else if (nxk[k * nxk_dim1 + 3] == nod) {
	    m[0] = 2;
	    m[1] = 4;
	    m[2] = 1;
	} else if (nxk[k * nxk_dim1 + 4] == nod) {
	    m[0] = 3;
	    m[1] = 1;
	    m[2] = 2;
	} else {
	    mesage_("IMPOSSIBLE SITUATION IN GETNXN, LOOP 50", (ftnlen)39);
	    *err = TRUE_;
	    return 0;
	}
	nlk = num;
	i__2 = ndo;
	for (ido = 1; ido <= i__2; ++ido) {
	    mido = m[ido - 1];
	    n = nxk[mido + k * nxk_dim1];
	    if (n > 0 && nuid[n] > 0) {
		if (nlk <= 0) {
		    ++num;
		    nlist[num] = n;
		} else {
		    i__3 = nlk;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			if (nlist[i__] == n) {
			    goto L110;
			}
/* L100: */
		    }
		    ++num;
		    nlist[num] = n;
		}
	    }
L110:
/* L120: */
	    ;
	}
/* L130: */
    }
    *numn = num;
    if (*numn > 20) {
	s_wsfe(&io___14);
	do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nuid[*node], (ftnlen)sizeof(integer));
	e_wsfe();
	*err = TRUE_;
    }
    return 0;
} /* getnxn_ */

#ifdef __cplusplus
	}
#endif

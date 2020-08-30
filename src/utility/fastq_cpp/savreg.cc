/* savreg.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int savreg_(integer *mxnd, integer *maxnbc, integer *maxsbc, 
	real *xn, real *yn, integer *nuid, integer *lxk, integer *nxl, 
	integer *lxn, integer *lstnbc, integer *lstsbc, integer *knbc, 
	integer *ksbc, integer *nnn, integer *kkk, integer *numreg, integer *
	iunit, logical *bar, integer *m1)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsue(cilist *), do_uio(integer *, char *, ftnlen), e_wsue();

    /* Local variables */
    static integer i__, j, k, n, jj;
    static logical ccw;
    static real area;
    static integer idel, nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___10 = { 0, 0, 0, 0, 0 };
    static cilist io___11 = { 0, 0, 0, 0, 0 };
    static cilist io___12 = { 0, 0, 0, 0, 0 };
    static cilist io___13 = { 0, 0, 0, 0, 0 };
    static cilist io___14 = { 0, 0, 0, 0, 0 };


/* *********************************************************************** */
/*  SUBROUTINE SAVREG = SAVES THE NODE AND ELEMENT DESCRIPTIONS AS WELL */
/*                      AS THE BOUNDARY CONDITIONS */
/* *********************************************************************** */
/*  NOTE: */
/*     THE MESH TABLES ARE EFFECTIVELY DESTROYED BY THIS ROUTINE */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;
    --lstnbc;
    --lstsbc;

    /* Function Body */
    ccw = TRUE_;
    if (! (*bar)) {
/*  DEFINE NUID-S FOR INTERIOR NODES. */
/*  SKIP DELETED NODES AND CONTINUATIONS. */
	k = 0;
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (nuid[i__] == 0 && lxn[(i__ << 2) + 1] > 0) {
		++k;
		nuid[i__] = *numreg * 100000 + k;
	    }
/* L100: */
	}
/*  GET COUNTER-CLOCKWISE NODE LISTS. */
/*   (THESE LISTS WILL OVERWRITE THE LXK ARRAY.) */
/*  DELETED ELEMENTS WILL BE SKIPPED. */
	j = 0;
	idel = 0;
	i__1 = *kkk;
	for (k = 1; k <= i__1; ++k) {
	    if (lxk[(k << 2) + 1] <= 0) {
		i__2 = *ksbc;
		for (jj = 2; jj <= i__2; jj += 3) {
		    if (lstsbc[jj] >= k - idel) {
			--lstsbc[jj];
		    }
/* L110: */
		}
		++idel;
	    } else {
		gnxka_(mxnd, &xn[1], &yn[1], &k, nodes, &area, &lxk[5], &nxl[
			3], &ccw);
		++j;
		for (i__ = 1; i__ <= 4; ++i__) {
		    n = nodes[i__ - 1];
		    lxk[i__ + (j << 2)] = (i__2 = nuid[n], abs(i__2));
/* L120: */
		}
	    }
/* L130: */
	}
	*kkk = j;
    } else {
	i__1 = *kkk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    lxk[(i__ << 2) + 1] = (i__2 = nuid[lxk[(i__ << 2) + 1]], abs(i__2)
		    );
	    lxk[(i__ << 2) + 2] = (i__2 = nuid[lxk[(i__ << 2) + 2]], abs(i__2)
		    );
/* L140: */
	}
    }
/*  COLLAPSE THE NODE ARRAYS TO ELIMINATE DELETED NODES, */
/*  CONTINUATIONS,  AND NODES ALREADY WRITTEN OUT. */
    k = 0;
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((lxn[(i__ << 2) + 1] > 0 || *bar) && nuid[i__] > 0) {
	    ++k;
	    xn[k] = xn[i__];
	    yn[k] = yn[i__];
	    nuid[k] = nuid[i__];
	}
/* L150: */
    }
    *nnn = k;
/*  WRITE HEADER,  NODE LIST,  ELEMENT LIST,  AND BOUNDARY LISTS */
    io___10.ciunit = *iunit;
    s_wsue(&io___10);
    do_uio(&c__1, (char *)&(*kkk), (ftnlen)sizeof(integer));
    do_uio(&c__1, (char *)&(*nnn), (ftnlen)sizeof(integer));
    do_uio(&c__1, (char *)&(*knbc), (ftnlen)sizeof(integer));
    do_uio(&c__1, (char *)&(*ksbc), (ftnlen)sizeof(integer));
    do_uio(&c__1, (char *)&(*numreg), (ftnlen)sizeof(integer));
    do_uio(&c__1, (char *)&(*bar), (ftnlen)sizeof(logical));
    do_uio(&c__1, (char *)&(*m1), (ftnlen)sizeof(integer));
    e_wsue();
    if (*nnn >= 1) {
	io___11.ciunit = *iunit;
	s_wsue(&io___11);
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_uio(&c__1, (char *)&nuid[i__], (ftnlen)sizeof(integer));
	    do_uio(&c__1, (char *)&xn[i__], (ftnlen)sizeof(real));
	    do_uio(&c__1, (char *)&yn[i__], (ftnlen)sizeof(real));
	}
	e_wsue();
    }
    io___12.ciunit = *iunit;
    s_wsue(&io___12);
    i__1 = *kkk;
    for (j = 1; j <= i__1; ++j) {
	for (i__ = 1; i__ <= 4; ++i__) {
	    do_uio(&c__1, (char *)&lxk[i__ + (j << 2)], (ftnlen)sizeof(
		    integer));
	}
    }
    e_wsue();
    if (*knbc > 0) {
	io___13.ciunit = *iunit;
	s_wsue(&io___13);
	i__1 = *knbc;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_uio(&c__1, (char *)&lstnbc[i__], (ftnlen)sizeof(integer));
	}
	e_wsue();
    }
    if (*ksbc > 0) {
	io___14.ciunit = *iunit;
	s_wsue(&io___14);
	i__1 = *ksbc;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_uio(&c__1, (char *)&lstsbc[i__], (ftnlen)sizeof(integer));
	}
	e_wsue();
    }
    return 0;
} /* savreg_ */

#ifdef __cplusplus
	}
#endif

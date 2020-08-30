/* ncklce.f -- translated by f2c (version 20160102).
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
static integer c__4 = 4;
static integer c__2 = 2;
static integer c__3 = 3;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int ncklce_(integer *mxnd, real *xn, real *yn, integer *nuid,
	 integer *lxk, integer *kxl, integer *nxl, integer *lxn, integer *kkk,
	 integer *nnn, integer *nnnold, integer *lll, integer *navail, 
	integer *iavail, real *eps, logical *noroom, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 IN NCKLCE, THE PERIMETER IS NOT CONTINU\
OUS AT NODE\002,i5)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, l, il, im, nl, nnn1, node, iold, nold, numb;
    static real rone;
    static integer kkkx, lllx, nnnx, nnxt, nbdy1, lllx1, nnnx1, lines[20];
    extern /* Subroutine */ int smogs_(integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, real *, real *), 
	    mesage_(char *, ftnlen), addlxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *), ccross_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, logical *),
	     getlxn_(integer *, integer *, integer *, integer *, integer *, 
	    logical *);

    /* Fortran I/O blocks */
    static cilist io___8 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE NCKLCE = INSERTS AN EXRTRA RING OF ELEMENTS JUST INSIDE */
/*                      THE REGION BOUNDARY */
/* *********************************************************************** */
/*  NOTE: */
/*     ONLY ARRAYS LXK, NXL, XN, AND YN ARE INPUT TO NCKLCE. */
/*     ARRAYS KXL AND LXN ARE RECREATED BY SUBROUTINE CROSS AFTER */
/*     LXK AND NXL ARE MODIFIED. */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;

    /* Function Body */
    nnnx = *nnn;
    lllx = *lll;
    kkkx = *kkk;
    *err = TRUE_;
    *noroom = TRUE_;
/*  COUNT BOUNDARY NODES TO CHECK FOR IMPENDING OVERFLOW */
    numb = 0;
    i__1 = *nnn;
    for (i__ = *nnnold + 1; i__ <= i__1; ++i__) {
	if (lxn[(i__ << 2) + 2] < 0 && lxn[(i__ << 2) + 1] > 0) {
	    ++numb;
	}
/* L100: */
    }
    if (*nnn + numb > *mxnd) {
	mesage_("NODE TABLE OVERFLOW IN NCKLCE", (ftnlen)29);
	return 0;
    } else if (*lll + (numb << 1) > *mxnd * 3) {
	mesage_("LINE TABLE OVERFLOW IN NCKLCE", (ftnlen)29);
	return 0;
    } else if (*kkk + numb > *mxnd) {
	mesage_("ELEMENT TABLE OVERFLOW IN NCKLCE", (ftnlen)32);
	return 0;
    }
    *noroom = FALSE_;
/*     FIND FIRST BOUNDARY NODE */
    i__1 = *nnn;
    for (i__ = *nnnold + 1; i__ <= i__1; ++i__) {
	if (lxn[(i__ << 2) + 1] > 0) {
	    nbdy1 = i__;
	    if (lxn[(i__ << 2) + 2] < 0) {
		goto L120;
	    }
	}
/* L110: */
    }
    node = -1;
    s_wsfe(&io___8);
    do_fio(&c__1, (char *)&node, (ftnlen)sizeof(integer));
    e_wsfe();
    return 0;
L120:
    nold = -1;
    node = nbdy1;
/*  FIND NEXT NODE ON THE BOUNDARY */
/*  LOOK AT ALL NEIGHBORING NODES */
L130:
    getlxn_(mxnd, &lxn[5], &node, lines, &nl, err);
    if (*err) {
	return 0;
    }
    i__1 = nl;
    for (il = 1; il <= i__1; ++il) {
	l = lines[il - 1];
	im = nxl[(l << 1) + 1] + nxl[(l << 1) + 2] - node;
/*  DISALLOW PREVIOUS NODE AND NON-BOUNDARY LINES */
	if (im != nold && kxl[(l << 1) + 2] <= 0) {
	    nnxt = im;
	    goto L150;
	}
/* L140: */
    }
    *err = TRUE_;
    s_wsfe(&io___16);
    do_fio(&c__1, (char *)&node, (ftnlen)sizeof(integer));
    e_wsfe();
    return 0;
L150:
/*  CREATE A NEW NODE *ON-TOP-OF* OLD BOUNDARY NODE */
    ++(*nnn);
    xn[*nnn] = xn[node];
    yn[*nnn] = yn[node];
    nuid[*nnn] = nuid[node];
    nuid[node] = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	lxn[i__ + (*nnn << 2)] = 0;
/* L160: */
    }
/*  CREATE TWO NEW LINES -- ONE CONNECTING TO THIS NODE, AND */
/*  ONE ON *TOP* OF THE NEW ELEMENT */
    ++(*lll);
    nxl[(*lll << 1) + 1] = node;
    nxl[(*lll << 1) + 2] = *nnn;
    kxl[(*lll << 1) + 1] = 0;
    kxl[(*lll << 1) + 2] = 0;
    ++(*lll);
    nxl[(*lll << 1) + 1] = *nnn;
    nxl[(*lll << 1) + 2] = *nnn + 1;
    kxl[(*lll << 1) + 1] = 0;
    kxl[(*lll << 1) + 2] = 0;
    if (nnxt == nbdy1) {
	nxl[(*lll << 1) + 2] = nnnx + 1;
    }
/*     CREATE A NEW ELEMENT */
    ++(*kkk);
    lxk[(*kkk << 2) + 1] = l;
    lxk[(*kkk << 2) + 2] = *lll - 1;
    lxk[(*kkk << 2) + 3] = *lll;
    lxk[(*kkk << 2) + 4] = *lll + 1;
    if (nnxt == nbdy1) {
	lxk[(*kkk << 2) + 4] = lllx + 1;
    }
/*  CHECK FOR COMPLETION OF LOOP AROUND BOUNDARY */
    if (nnxt != nbdy1) {
	nold = node;
	node = nnxt;
	goto L130;
    }
/*     RE-SETUP AVAILABLE LXN-SPACE LINKS */
    iold = 0;
    *navail = 0;
    i__1 = nnnx;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (lxn[(i__ << 2) + 1] == 0) {
	    if (iold <= 0) {
		*iavail = i__;
	    } else {
		lxn[(iold << 2) + 4] = i__;
	    }
	    iold = i__;
	    ++(*navail);
	}
/* L170: */
    }
    if (iold <= 0) {
	*iavail = *nnn + 1;
    } else {
	lxn[(iold << 2) + 4] = *nnn + 1;
    }
    *navail += *mxnd - *nnn;
    if (*nnn < *mxnd) {
	nnn1 = *nnn + 1;
	i__1 = *mxnd;
	for (i__ = nnn1; i__ <= i__1; ++i__) {
	    lxn[(i__ << 2) + 1] = 0;
	    lxn[(i__ << 2) + 2] = 0;
	    lxn[(i__ << 2) + 3] = 0;
	    lxn[(i__ << 2) + 4] = i__ + 1;
/* L180: */
	}
    }
/*     COMPLETE KXL AND LXN ARRAYS */
    i__1 = kkkx + 1;
    i__2 = lllx + 1;
    ccross_(&c__4, kkk, &c__2, lll, &lxk[5], &kxl[3], &i__1, &i__2, noroom, 
	    err);
    if (*noroom) {
	return 0;
    }
    if (*err) {
	mesage_("ERROR IN NCKLCE - LXK TABLE GENERATION", (ftnlen)38);
	return 0;
    }
    lllx1 = lllx + 1;
    i__1 = *lll;
    for (l = lllx1; l <= i__1; ++l) {
	for (i__ = 1; i__ <= 2; ++i__) {
	    addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nxl[i__ + (l << 
		    1)], &l, nnn, err, noroom);
	    if (*err) {
		mesage_("ERROR IN NCKLCE - NXL TABLE GENERATION", (ftnlen)38);
		return 0;
	    }
/* L190: */
	}
/* L200: */
    }
/*  USE SMOGS TO REPOSITION THE OLD BOUNDARY NODES */
    i__1 = *nnn;
    for (i__ = *nnnold + 1; i__ <= i__1; ++i__) {
	lxn[(i__ << 2) + 2] = -lxn[(i__ << 2) + 2];
/* L210: */
    }
    rone = (float)1.;
    smogs_(mxnd, &xn[1], &yn[1], &nxl[3], &lxn[5], nnn, nnnold, &c__3, eps, &
	    rone);
/*  FLAG NEW BOUNDARY NODES (ONLY) */
    i__1 = *nnn;
    for (i__ = *nnnold + 1; i__ <= i__1; ++i__) {
	lxn[(i__ << 2) + 2] = (i__2 = lxn[(i__ << 2) + 2], abs(i__2));
/* L220: */
    }
    nnnx1 = nnnx + 1;
    i__1 = *nnn;
    for (i__ = nnnx1; i__ <= i__1; ++i__) {
	lxn[(i__ << 2) + 2] = -lxn[(i__ << 2) + 2];
/* L230: */
    }
    return 0;
} /* ncklce_ */

#ifdef __cplusplus
	}
#endif

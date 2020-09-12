/* delem.f -- translated by f2c (version 20160102).
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
#include "paving.h"
#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int delem_(integer *mxnd, real *xn, real *yn, integer *nuid, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, integer *nnn, 
	integer *navail, integer *iavail, integer *node1, integer *k, integer 
	*n2, integer *n4, logical *done, logical *check, logical *noroom, 
	logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 IN DELEM,  ELEMENT\002,i5,\002 DOES NOT\
 CONTAIN LINE\002,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, k1, l1, l2, n1, n3, l3, l4, k4, ll, nl;
    static logical ccw;
    static real area;
    static integer nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);
    static integer lines[4], l1list[20];
    extern /* Subroutine */ int mesage_(char *, ftnlen), addlxn_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *), fndlnk_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     dellxn_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, logical *), getlxn_(
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     nxkord_(integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___15 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE DELEM = DELETES AN ELEMENT BY COLAPSING NODE1 ONTO THE */
/*                     OPPOSING DIAGONAL NODE */
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
    *err = FALSE_;
    gnxka_(mxnd, &xn[1], &yn[1], k, nodes, &area, &lxk[5], &nxl[3], &ccw);
    if (*node1 != nodes[0] && *node1 != nodes[1] && *node1 != nodes[2] && *
	    node1 != nodes[3]) {
	mesage_("** PROBLEMS IN DELEM - NODE1 IS NOT IN ELEMENT K **", (
		ftnlen)51);
	*err = TRUE_;
	goto L190;
    }
/*  ARRANGE NODES SO THE COLLAPSING DIAGONAL IS FROM 1ST TO 3RD NODES */
/*  AND INSURE THAT THE NODE TO BE DELETED IS NOT A BOUNDARY NODE */
    nxkord_(nodes, node1);
    if (lxn[(nodes[0] << 2) + 2] <= 0) {
	nxkord_(nodes, &nodes[2]);
    }
    if (lxn[(nodes[0] << 2) + 2] <= 0) {
	*err = TRUE_;
	mesage_("** BOUNDARY ELEMENT CANNOT BE DELETED IN DELEM **", (ftnlen)
		49);
	goto L190;
    }
/*  PREPARE FOR THE SQUASH OF ELEMENT K */
    n1 = nodes[0];
    *n2 = nodes[1];
    n3 = nodes[2];
    *n4 = nodes[3];
    if (*check) {
	if (lxn[(n3 << 2) + 4] >= 0 && lxn[(n3 << 2) + 2] > 0) {
	    *done = TRUE_;
	} else {
	    *done = FALSE_;
	    goto L190;
	}
    }
/*  FIND THE LINES ASSOCIATED WITH THE ELEMENT TO BE DELETED */
    for (i__ = 1; i__ <= 4; ++i__) {
	j = i__ + 1;
	if (j >= 5) {
	    j = 1;
	}
	fndlnk_(mxnd, &lxk[5], &nxl[3], k, &nodes[i__ - 1], &nodes[j - 1], &
		lines[i__ - 1], err);
	if (*err) {
	    mesage_("** PROBLEMS IN DELEM GETTING NODE LINES **", (ftnlen)42);
	    goto L190;
	}
	if (lines[i__ - 1] == 0) {
	    mesage_("** PROBLEMS IN DELEM WITH 0 NODE LINES **", (ftnlen)41);
	    *err = TRUE_;
	    goto L190;
	}
/* L100: */
    }
/*  FIND ELEMENTS ON OTHER SIDES OF THE LINES */
/*  K2 AND K3 ARE NEVER NEEDED */
    l1 = lines[0];
    l2 = lines[1];
    l3 = lines[2];
    l4 = lines[3];
    k1 = kxl[(l1 << 1) + 1] + kxl[(l1 << 1) + 2] - *k;
    k4 = kxl[(l4 << 1) + 1] + kxl[(l4 << 1) + 2] - *k;
/*  FIX LXK ARRAY */
/*  DISCARD L1 FOR L2 IN K1 */
    for (i__ = 1; i__ <= 4; ++i__) {
	if (lxk[i__ + (k1 << 2)] == l1) {
	    lxk[i__ + (k1 << 2)] = l2;
	    goto L120;
	}
/* L110: */
    }
    s_wsfe(&io___15);
    do_fio(&c__1, (char *)&k1, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&l1, (ftnlen)sizeof(integer));
    e_wsfe();
    *err = TRUE_;
    goto L190;
L120:
/*  DISCARD L4 FOR L3 IN K4 */
    for (i__ = 1; i__ <= 4; ++i__) {
	if (lxk[i__ + (k4 << 2)] == l4) {
	    lxk[i__ + (k4 << 2)] = l3;
	    goto L140;
	}
/* L130: */
    }
    s_wsfe(&io___16);
    do_fio(&c__1, (char *)&k1, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&l1, (ftnlen)sizeof(integer));
    e_wsfe();
    *err = TRUE_;
    goto L190;
L140:
/*  DELETE ELEMENT K */
    for (i__ = 1; i__ <= 4; ++i__) {
	lxk[i__ + (*k << 2)] = 0;
/* L150: */
    }
/*  FIX KXL ARRAY */
/*  DISCARD K FOR K1 WITH L2 */
    if (kxl[(l2 << 1) + 1] == *k) {
	kxl[(l2 << 1) + 1] = k1;
    } else if (kxl[(l2 << 1) + 2] == *k) {
	kxl[(l2 << 1) + 2] = k1;
    }
/*  DISCARD K FOR K4 WITH L3 */
    if (kxl[(l3 << 1) + 1] == *k) {
	kxl[(l3 << 1) + 1] = k4;
    } else if (kxl[(l3 << 1) + 2] == *k) {
	kxl[(l3 << 1) + 2] = k4;
    }
/*  DELETE L1 AND L4 */
    kxl[(l1 << 1) + 1] = 0;
    kxl[(l1 << 1) + 2] = 0;
    kxl[(l4 << 1) + 1] = 0;
    kxl[(l4 << 1) + 2] = 0;
/*  FIX NXL ARRAY */
/*  DELETE L1 AND L4 */
    nxl[(l1 << 1) + 1] = 0;
    nxl[(l1 << 1) + 2] = 0;
    nxl[(l4 << 1) + 1] = 0;
    nxl[(l4 << 1) + 2] = 0;
/*  RECONNECT ALL LINES CONNECTING TO NODE 1 TO NODE 3 */
    getlxn_(mxnd, &lxn[5], &n1, l1list, &nl, err);
    if (*err) {
	return 0;
    }
    i__1 = nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ll = l1list[i__ - 1];
	if (nxl[(ll << 1) + 1] == n1) {
	    nxl[(ll << 1) + 1] = n3;
	} else if (nxl[(ll << 1) + 2] == n1) {
	    nxl[(ll << 1) + 2] = n3;
	}
/* L160: */
    }
/*  FIX LXN ARRAY */
/*  UNHOOK L1 FROM N2 AND L4 FROM N4 */
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, n2, &l1, nnn, err, 
	    noroom);
    if (*noroom || *err) {
	goto L190;
    }
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, n4, &l4, nnn, err, 
	    noroom);
    if (*noroom || *err) {
	goto L190;
    }
/*  ADD ALL LINES HOOKED TO N3 TO THE LIST OF LINES FOR N3 */
    i__1 = nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ll = l1list[i__ - 1];
	if (ll != l1 && ll != l4) {
	    addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &n3, &ll, nnn, 
		    err, noroom);
	    if (*noroom || *err) {
		goto L190;
	    }
	}
/* L170: */
    }
/*  DELETE N1 (UNHOOK EVERYTHING FROM IT) */
    for (i__ = 1; i__ <= 3; ++i__) {
	lxn[i__ + (n1 << 2)] = 0;
/* L180: */
    }
    lxn[(n1 << 2) + 4] = *iavail;
    *iavail = n1;
    ++(*navail);
/*  FIX XN AND YN ARRAYS */
/*  DEFINE POSITION OF N3 */
    if (lxn[(n3 << 2) + 2] > 0) {
	xn[n3] = (xn[n1] + xn[n3]) * (float).5;
	yn[n3] = (yn[n1] + yn[n3]) * (float).5;
    }
    nuid[n1] = 0;
    *done = TRUE_;
L190:
    return 0;
} /* delem_ */

#ifdef __cplusplus
	}
#endif

/* squash.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int squash_(integer *mxnd, real *xn, real *yn, integer *nuid,
	 integer *lxk, integer *kxl, integer *nxl, integer *lxn, integer *kkk,
	 integer *kkkold, integer *nnn, integer *navail, integer *iavail, 
	real *asmall, logical *done, logical *noroom, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 IN SQUASH,  ELEMENT\002,i5,\002 DOES NO\
T CONTAIN LINE\002,i5)";

    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sin(doublereal), cos(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, k, k1, l1, l2, n1, n2, n3, n4, l3, l4, k4, ik, ll;
    static real pi;
    static integer nl, kr[6];
    static logical ccw;
    static real area, dmax__, drat, alop, rhom[6], temp, diag1, diag2;
    static integer nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);
    static integer lines[4], ktemp, l1list[20];
    extern /* Subroutine */ int addlxn_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *), fndlnk_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     dellxn_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, logical *), getlxn_(
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     nxkord_(integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___29 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___30 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE SQUASH = TRIES TO SQUASH THE MOST SHARPELY RHOMBICAL */
/*                      ELEMENT IN THE MESH */
/* *********************************************************************** */
/*  VARIABLES: */
/*     ASMALL  =  AN ANGLE CONSIDERED TO BE TOO SMALL (DEGREES) */
/*     DONE    =  .TRUE. IF A SQUASH WAS DONE */
/* *********************************************************************** */
/*  CHECK FOR IMPENDING OVERFLOW */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;

    /* Function Body */
    *noroom = FALSE_;
    *err = FALSE_;
    *done = FALSE_;
    if (*nnn >= *mxnd - 1) {
	mesage_("INSUFFICIENT STORAGE IN SQUASH", (ftnlen)30);
	*noroom = TRUE_;
	*err = TRUE_;
	return 0;
    }
/*  INITIALIZATION */
    pi = atan2((float)0., (float)-1.);
    if (*asmall > (float)80.) {
	*asmall = (float)80.;
    }
    alop = *asmall * pi / (float)180.;
    dmax__ = sin(alop * (float).5) / cos(alop * (float).5);
/* Computing 2nd power */
    r__1 = dmax__;
    dmax__ = r__1 * r__1;
/*  CLEAR RECORDS */
    for (i__ = 1; i__ <= 6; ++i__) {
	rhom[i__ - 1] = (float)10.;
	kr[i__ - 1] = 0;
/* L100: */
    }
/*  ELEMENT LOOP */
    i__1 = *kkk;
    for (k = *kkkold + 1; k <= i__1; ++k) {
	if (lxk[(k << 2) + 1] > 0) {
	    ccw = FALSE_;
	    gnxka_(mxnd, &xn[1], &yn[1], &k, nodes, &area, &lxk[5], &nxl[3], &
		    ccw);
	    n1 = nodes[0];
	    n2 = nodes[1];
	    n3 = nodes[2];
	    n4 = nodes[3];
/* Computing 2nd power */
	    r__1 = xn[n3] - xn[n1];
/* Computing 2nd power */
	    r__2 = yn[n3] - yn[n1];
	    diag1 = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
	    r__1 = xn[n4] - xn[n2];
/* Computing 2nd power */
	    r__2 = yn[n4] - yn[n2];
	    diag2 = r__1 * r__1 + r__2 * r__2;
	    if (diag2 != (float)0.) {
		drat = diag1 / diag2;
		if (drat > (float)1.) {
		    drat = (float)1. / drat;
		}
	    } else {
		drat = (float)0.;
	    }
/*  RECORD WORST FIVE */
	    if (drat <= dmax__) {
		kr[5] = k;
		rhom[5] = drat;
		for (i__ = 1; i__ <= 5; ++i__) {
		    j = 7 - i__;
		    if (rhom[j - 1] >= rhom[j - 2]) {
			goto L120;
		    }
		    temp = rhom[j - 2];
		    rhom[j - 2] = rhom[j - 1];
		    rhom[j - 1] = temp;
		    ktemp = kr[j - 2];
		    kr[j - 2] = kr[j - 1];
		    kr[j - 1] = ktemp;
/* L110: */
		}
L120:
		;
	    }
	}
/* L130: */
    }
/*  SQUASH WORST ELEMENT NOT VIOLATING THE BOUNDARY */
    for (ik = 1; ik <= 5; ++ik) {
	k = kr[ik - 1];
	if (k > 0) {
	    ccw = TRUE_;
	    gnxka_(mxnd, &xn[1], &yn[1], &k, nodes, &area, &lxk[5], &nxl[3], &
		    ccw);
	    n1 = nodes[0];
	    n2 = nodes[1];
	    n3 = nodes[2];
	    n4 = nodes[3];
/* Computing 2nd power */
	    r__1 = xn[n3] - xn[n1];
/* Computing 2nd power */
	    r__2 = yn[n3] - yn[n1];
	    diag1 = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
	    r__1 = xn[n4] - xn[n2];
/* Computing 2nd power */
	    r__2 = yn[n4] - yn[n2];
	    diag2 = r__1 * r__1 + r__2 * r__2;
/*  ARRANGE NODES SO THE SHORT DIAGONAL IS FROM 1ST TO 3RD NODES */
/*  AND INSURE THAT THE NODE TO BE DELETED IS NOT A BOUNDARY NODE */
	    if (diag1 > diag2) {
		nxkord_(nodes, &n2);
	    }
	    n1 = nodes[0];
	    n3 = nodes[2];
	    if (lxn[(n1 << 2) + 2] <= 0) {
		nxkord_(nodes, &n3);
	    }
	    n1 = nodes[0];
	    if (lxn[(n1 << 2) + 2] > 0) {
		goto L150;
	    }
	}
/* L140: */
    }
    return 0;
/*  PREPARE FOR THE SQUASH OF ELEMENT K */
L150:
    *done = TRUE_;
    n1 = nodes[0];
    n2 = nodes[1];
    n3 = nodes[2];
    n4 = nodes[3];
/*  FIND THE LINES ASSOCIATED WITH THE ELEMENT TO BE DELETED */
    for (i__ = 1; i__ <= 4; ++i__) {
	j = i__ + 1;
	if (j >= 5) {
	    j = 1;
	}
	fndlnk_(mxnd, &lxk[5], &nxl[3], &k, &nodes[i__ - 1], &nodes[j - 1], &
		lines[i__ - 1], err);
	if (*err) {
	    return 0;
	}
	if (lines[i__ - 1] == 0) {
	    *err = TRUE_;
	    return 0;
	}
/* L160: */
    }
/*  FIND ELEMENTS ON OTHER SIDES OF THE LINES */
/*  K2 AND K3 ARE NEVER NEEDED */
    l1 = lines[0];
    l2 = lines[1];
    l3 = lines[2];
    l4 = lines[3];
    k1 = kxl[(l1 << 1) + 1] + kxl[(l1 << 1) + 2] - k;
    k4 = kxl[(l4 << 1) + 1] + kxl[(l4 << 1) + 2] - k;
/*  FIX LXK ARRAY */
/*  DISCARD L1 FOR L2 IN K1 */
    for (i__ = 1; i__ <= 4; ++i__) {
	if (lxk[i__ + (k1 << 2)] == l1) {
	    lxk[i__ + (k1 << 2)] = l2;
	    goto L180;
	}
/* L170: */
    }
    s_wsfe(&io___29);
    do_fio(&c__1, (char *)&k1, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&l1, (ftnlen)sizeof(integer));
    e_wsfe();
    *err = TRUE_;
    return 0;
L180:
/*  DISCARD L4 FOR L3 IN K4 */
    for (i__ = 1; i__ <= 4; ++i__) {
	if (lxk[i__ + (k4 << 2)] == l4) {
	    lxk[i__ + (k4 << 2)] = l3;
	    goto L200;
	}
/* L190: */
    }
    s_wsfe(&io___30);
    do_fio(&c__1, (char *)&k1, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&l1, (ftnlen)sizeof(integer));
    e_wsfe();
    *err = TRUE_;
    return 0;
L200:
/*  DELETE ELEMENT K */
    for (i__ = 1; i__ <= 4; ++i__) {
	lxk[i__ + (k << 2)] = 0;
/* L210: */
    }
/*  FIX KXL ARRAY */
/*  DISCARD K FOR K1 WITH L2 */
    if (kxl[(l2 << 1) + 1] == k) {
	kxl[(l2 << 1) + 1] = k1;
    } else if (kxl[(l2 << 1) + 2] == k) {
	kxl[(l2 << 1) + 2] = k1;
    }
/*  DISCARD K FOR K4 WITH L3 */
    if (kxl[(l3 << 1) + 1] == k) {
	kxl[(l3 << 1) + 1] = k4;
    } else if (kxl[(l3 << 1) + 2] == k) {
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
/* L220: */
    }
/*  FIX LXN ARRAY */
/*  UNHOOK L1 FROM N2 AND L4 FROM N4 */
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &n2, &l1, nnn, err, 
	    noroom);
    if (*err) {
	return 0;
    }
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &n4, &l4, nnn, err, 
	    noroom);
    if (*err) {
	return 0;
    }
/*  ADD ALL LINES HOOKED TO N3 TO THE LIST OF LINES FOR N3 */
    i__1 = nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ll = l1list[i__ - 1];
	if (ll != l1 && ll != l4) {
	    addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &n3, &ll, nnn, 
		    err, noroom);
	    if (*err) {
		return 0;
	    }
	}
/* L230: */
    }
/*  DELETE N1 (UNHOOK EVERYTHING FROM IT) */
    for (i__ = 1; i__ <= 3; ++i__) {
	lxn[i__ + (n1 << 2)] = 0;
/* L240: */
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
    return 0;
} /* squash_ */

#ifdef __cplusplus
	}
#endif

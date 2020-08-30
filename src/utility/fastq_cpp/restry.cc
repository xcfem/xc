/* restry.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int restry_(integer *mxnd, integer *k, integer *k2, integer *
	lxk, integer *nxl, integer *kxl, integer *lxn, real *xn, real *yn, 
	integer *nuid, integer *navail, integer *iavail, integer *nnn, 
	logical *done, logical *err, logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 IN RESTRY, NODES\002,2i5,/,\002 DO NOT \
DEFINE A LINE IN ELEMENT\002,i5)";
    static char fmt_10010[] = "(\002 IN RESTRY,  ELEMENTS\002,2i5,/,\002 DO \
NOT CONTAIN A COMMON SIDE USING NODES\002,2i5)";

    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, l, n1, n2, il;
    static real pi, al1[4], al2[4];
    static integer ns1[4], ns2[4];
    static logical ccw;
    static integer nsa[4], nsb[4], nsc[4], nsd[4];
    static real ang1[4], ang2[4];
    static integer l1ee, l2ee;
    static logical iokb;
    static integer node;
    static real amin;
    static logical iokf;
    static real arat, qavg, slen;
    static integer lint;
    static real qmax, area1, area2, cond1, cond2, a1max, a2max;
    static integer l1max, l2max;
    static real qrat1, s1max, srat1, s2max, qrat2, srat2;
    static logical carea;
    static real aratb, aratf;
    static logical lside;
    static real qavgb, qavgf;
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);
    static real qmaxb, smaxb, qmaxf;
    extern /* Subroutine */ int lswap_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *);
    static real pitol, dummy, area1b, area2b, area1f, area2f, cond1b, tolqx, 
	    cond2b, cond1f, cond2f, qrat1b, qrat2b, srat1b, srat2b, qrat1f, 
	    qrat2f, srat1f, srat2f;
    static logical ibgood;
    extern /* Subroutine */ int qaaval_(integer *, integer *, real *, real *, 
	    real *, real *, real *, logical *);
    static logical ifgood;
    extern /* Subroutine */ int addlxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *), fndlnk_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *);
    static real valueb;
    extern /* Subroutine */ int condno_(integer *, integer *, real *, real *, 
	    real *, real *, real *, real *, logical *);
    static real valuef;
    extern /* Subroutine */ int dellxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *);
    static integer nshold;
    extern /* Subroutine */ int nxkord_(integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___25 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___39 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE RESTRY = TRY TO RESTRUCTURE K AND ONE OF ITS NEIGHBORS */
/* *********************************************************************** */
/*  NOTE: */
/*     THE ELEMENT OPPOSITE THE LONGEST  -  OR IN SOME CASES THE SECOND */
/*     LONGEST  -  SIDE OF ELEMENT K WILL BE FOUND. */
/*     A RESTRUCTURE WILL ONLY BE DONE THEN IF */
/*          - THIS SAME SIDE IS THE LONGEST SIDE  (OR NEARLY SO) */
/*          OF THE SECOND ELEMENT ALSO. */
/*          - NO ANGLES LARGER THAN 180 DEGREES ARE CREATED */
/*          - THE MAX AND AVERAGE Q - NUMBERS BOTH DECREASE */
/*          - AN AREA BREAKDOWN CONSISTENT WITH THE IMPROVEMENT */
/*          IN THE AVERAGE Q - NUMBER OCCURS */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nuid;
    --yn;
    --xn;
    lxn -= 5;
    kxl -= 3;
    nxl -= 3;
    lxk -= 5;

    /* Function Body */
    *done = FALSE_;
    *err = FALSE_;
    pi = atan2((float)0., (float)-1.);
    pitol = pi * (float)1.0833333333333333;
/*  GET DATA FOR ELEMENT K */
    ccw = TRUE_;
    carea = FALSE_;
    gnxka_(mxnd, &xn[1], &yn[1], k, ns1, &area1, &lxk[5], &nxl[3], &ccw);
    qaaval_(mxnd, ns1, ang1, &qrat1, &dummy, &xn[1], &yn[1], &carea);
    condno_(mxnd, ns1, &qrat1, &srat1, &cond1, al1, &xn[1], &yn[1], &lside);
/*  FIND LONGEST AND SECOND LONGEST SIDES,  EXCLUDING BOUNDARY LINES */
    s1max = (float)-1.;
    l1max = 0;
    a1max = (float)0.;
    s2max = (float)-1.;
    for (i__ = 1; i__ <= 4; ++i__) {
	if (al1[i__ - 1] > s2max) {
	    n1 = ns1[i__ - 1];
	    j = i__ + 1;
	    if (i__ == 4) {
		j = 1;
	    }
	    n2 = ns1[j - 1];
	    for (il = 1; il <= 4; ++il) {
		l = lxk[il + (*k << 2)];
		node = nxl[(l << 1) + 1];
		if (node == n1 || node == n2) {
		    node = nxl[(l << 1) + 2];
		    if (node == n1 || node == n2) {
			if (kxl[(l << 1) + 2] > 0) {
			    goto L110;
			} else {
			    goto L120;
			}
		    }
		}
/* L100: */
	    }
	    s_wsfe(&io___25);
	    do_fio(&c__1, (char *)&n1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n2, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*k), (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    return 0;
/*  N1 TO N2 IS AN INTERIOR LINE */
L110:
/*  LONGEST INTERIOR LINE SO FAR */
	    if (al1[i__ - 1] > s1max) {
		s2max = s1max;
		l2max = l1max;
		a2max = a1max;
		s1max = al1[i__ - 1];
		l1max = l;
		a1max = ang1[i__ - 1] + ang1[j - 1];
/*  SECOND LONGEST LINE SO FAR */
	    } else {
		s2max = al1[i__ - 1];
		l2max = l;
		a2max = ang1[i__ - 1] + ang1[j - 1];
	    }
	}
L120:
/* L130: */
	;
    }
/* *********************************************************************** */
/*  NOTE: */
/*     IF LONGEST SIDE IS SUBSTANTIALLY LONGER THAN SECOND */
/*     LONGEST,  PAIR WITH ELEMENT OPPOSITE LONGEST SIDE. */
/*     IF LONGEST SIDE IS NOT SUBSTANTIALLY LONGER THAN SECOND */
/*     LONGEST SIDE,  PAIR WITH THE ELEMENT OPPOSITE EITHER THE */
/*     LONGEST OR SECOND LONGEST SIDE DEPENDING ON WHICH HAS THE */
/*     SMALLER SUM OF ADJACENT ANGLES.   (LOOK AT A TRAPEZOID TO */
/*     SEE WHY THIS IS REASONABLE.) */
/* *********************************************************************** */
    if (l1max <= 0) {
	return 0;
    }
    slen = s1max;
    lint = l1max;
    if (l2max > 0 && s1max <= s2max * (float)1.15 && a1max > a2max) {
	slen = s2max;
	lint = l2max;
    }
    *k2 = kxl[(lint << 1) + 1] + kxl[(lint << 1) + 2] - *k;
/*  DOUBLE CHECK */
    if (*k2 <= 0) {
	return 0;
    }
/*  GET DATA FOR ELEMENT K2 */
    gnxka_(mxnd, &xn[1], &yn[1], k2, ns2, &area2, &lxk[5], &nxl[3], &ccw);
    qaaval_(mxnd, ns2, ang2, &qrat2, &dummy, &xn[1], &yn[1], &carea);
    condno_(mxnd, ns2, &qrat2, &srat2, &cond2, al2, &xn[1], &yn[1], &lside);
/*  FIND LONGEST SIDE IN SECOND ELEMENT */
/* Computing MAX */
    r__1 = max(al2[0],al2[1]), r__1 = max(r__1,al2[2]);
    smaxb = dmax(r__1,al2[3]);
/*  IF THE INTERFACE SIDE IS SIGNIFICANTLY SHORTER THAN THE */
/*  LONGEST SIDE OF THE SECOND ELEMENT,  SKIP THE RESTRUCTURE. */
    if (slen < smaxb * (float).5) {
	return 0;
    }
/*  CIRCULARLY SHIFT THE TWO NODE LISTS TO CREATE CANONICAL ORDER. */
/*  IN CANONICAL ORDER THE FIRST NODE IS THE NODE IN BOTH ELEMENTS */
/*  WHOSE COUNTER - CLOCKWISE SUCCESSOR IN THE FIRST ELEMENT IS NOT */
/*  ALSO IN THE SECOND ELEMENT. */
/*  NOTE : ORDER OF SIDE LENGTH AND ANGLE DATA IS NO GOOD AFTER THIS */
    n1 = nxl[(lint << 1) + 1];
    nxkord_(ns1, &n1);
    for (i__ = 1; i__ <= 4; ++i__) {
	if (ns2[i__ - 1] == ns1[1]) {
	    n1 = nxl[(lint << 1) + 2];
	    nxkord_(ns1, &n1);
	    nxkord_(ns2, &n1);
	    goto L150;
	}
/* L140: */
    }
    nxkord_(ns2, &n1);
L150:
/*  SEE IF THEY MATCH AS THEY SHOULD  (BUTTERFLY ELEMENTS MAY CAUSE */
/*  PROBLEMS WITH THE CCW ROUTINES */
    if (ns1[3] != ns2[1]) {
	nshold = ns2[1];
	ns2[1] = ns2[3];
	ns2[3] = nshold;
	if (ns1[3] != ns2[1]) {
	    nshold = ns1[1];
	    ns1[1] = ns1[3];
	    ns1[3] = nshold;
	    if (ns1[3] != ns2[1]) {
/*  ERROR MATCHING ELEMENTS ALONG A COMMON SIDE */
		s_wsfe(&io___39);
		do_fio(&c__1, (char *)&(*k), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*k2), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ns1[0], (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ns1[3], (ftnlen)sizeof(integer));
		e_wsfe();
		*err = TRUE_;
		return 0;
	    }
	}
    }
/*  COMPUTE ALL RELEVANT DATA FOR ALL THREE STRUCTURES */
/*  ORIGINAL STRUCTURE */
    qmax = dmax(qrat1,qrat2);
    tolqx = qmax * (float).95 + (float).05;
    qavg = (qrat1 + qrat2) * (float).5;
    amin = dmin(area1,area2);
    if (amin > (float)0.) {
	arat = dmax(area1,area2) / amin;
    } else {
	arat = (float)1e10;
    }
/*   * BACKWARDS *  STRUCTURE */
    nsa[0] = ns2[3];
    nsa[1] = ns1[0];
    nsa[2] = ns1[1];
    nsa[3] = ns1[2];
    nsb[0] = ns2[3];
    nsb[1] = ns1[2];
    nsb[2] = ns2[1];
    nsb[3] = ns2[2];
    carea = TRUE_;
    qaaval_(mxnd, nsa, ang1, &qrat1b, &area1b, &xn[1], &yn[1], &carea);
    qaaval_(mxnd, nsb, ang2, &qrat2b, &area2b, &xn[1], &yn[1], &carea);
/* Computing MAX */
    r__1 = max(ang1[0],ang1[1]), r__1 = max(r__1,ang1[2]);
/* Computing MAX */
    r__2 = max(ang2[0],ang2[1]), r__2 = max(r__2,ang2[2]);
    if (dmax(r__1,ang1[3]) > pitol || dmax(r__2,ang2[3]) > pitol) {
	iokb = FALSE_;
    } else {
	iokb = TRUE_;
    }
    qmaxb = dmax(qrat1b,qrat2b);
    qavgb = (qrat1b + qrat2b) * (float).5;
    amin = dmin(area1b,area2b);
    if (amin > (float)0.) {
	aratb = dmax(area1b,area2b) / amin;
    } else {
	aratb = (float)1e10;
    }
/*   * FORWARDS *  STRUCTURE */
    nsc[0] = ns1[1];
    nsc[1] = ns1[2];
    nsc[2] = ns1[3];
    nsc[3] = ns2[2];
    nsd[0] = ns1[1];
    nsd[1] = ns2[2];
    nsd[2] = ns2[3];
    nsd[3] = ns2[0];
    qaaval_(mxnd, nsc, ang1, &qrat1f, &area1f, &xn[1], &yn[1], &carea);
    qaaval_(mxnd, nsd, ang2, &qrat2f, &area2f, &xn[1], &yn[1], &carea);
/* Computing MAX */
    r__1 = max(ang1[0],ang1[1]), r__1 = max(r__1,ang1[2]);
/* Computing MAX */
    r__2 = max(ang2[0],ang2[1]), r__2 = max(r__2,ang2[2]);
    if (dmax(r__1,ang1[3]) > pitol || dmax(r__2,ang2[3]) > pitol) {
	iokf = FALSE_;
    } else {
	iokf = TRUE_;
    }
    qmaxf = dmax(qrat1f,qrat2f);
    qavgf = (qrat1f + qrat2f) * (float).5;
    amin = dmin(area1f,area2f);
    if (amin > (float)0.) {
	aratf = dmax(area1f,area2f) / amin;
    } else {
	aratf = (float)1e10;
    }
/*  SEE IF BACKWARD IS BETTER THAN ORIGINAL */
    if (iokb && qmaxb <= tolqx && qavgb <= qavg && aratb * qavgb <= arat * 
	    qavg) {
	ibgood = TRUE_;
    } else {
	ibgood = FALSE_;
    }
/*  SEE IF FORWARD IS BETTER THAN ORIGINAL */
    if (iokf && qmaxf <= tolqx && qavgf <= qavg && aratf * qavgf <= arat * 
	    qavg) {
	ifgood = TRUE_;
    } else {
	ifgood = FALSE_;
    }
/*  CHOOSE BEST ALTERNATIVE */
/*  IF BOTH FORWARD AND BACKWARD IS BETTER THAN ORIGINAL,  THEN */
/*  COMPUTE PAIR - VALUES TO CHOOSE BETWEEN FORWARD AND BACKWARD. */
/*  VALUE  =   (AVERAGE CONDITION NUMBER)  *  SQRT (AREA RATIO) */
    if (ifgood && ibgood) {
	lside = FALSE_;
	condno_(mxnd, nsa, &qrat1b, &srat1b, &cond1b, al1, &xn[1], &yn[1], &
		lside);
	condno_(mxnd, nsb, &qrat2b, &srat2b, &cond2b, al1, &xn[1], &yn[1], &
		lside);
/* Computing 2nd power */
	r__1 = cond1b + cond2b;
	valueb = aratb * (r__1 * r__1);
	condno_(mxnd, nsc, &qrat1f, &srat1f, &cond1f, al1, &xn[1], &yn[1], &
		lside);
	condno_(mxnd, nsd, &qrat2f, &srat2f, &cond2f, al1, &xn[1], &yn[1], &
		lside);
/* Computing 2nd power */
	r__1 = cond1f + cond2f;
	valuef = aratf * (r__1 * r__1);
	if (valueb > valuef) {
	    ibgood = FALSE_;
	}
    }
/*  BACKWARD STRUCTURE IS BEST.  IMPLEMENT IT. */
/*  FIRST FIX LXK AND KXL ARRAYS */
    if (ibgood) {
	fndlnk_(mxnd, &lxk[5], &nxl[3], k, &ns1[2], &ns1[3], &l1ee, err);
	if (*err) {
	    return 0;
	}
	fndlnk_(mxnd, &lxk[5], &nxl[3], k2, ns2, &ns2[3], &l2ee, err);
	if (*err) {
	    return 0;
	}
	lswap_(mxnd, &lxk[5], &kxl[3], k, &l1ee, k2, &l2ee, err);
	if (*err) {
	    return 0;
	}
/*  FIX NXL ARRAY  (MOVE THE DIAGONAL) */
	nxl[(lint << 1) + 1] = ns2[3];
	nxl[(lint << 1) + 2] = ns1[2];
/*  FIX LXN ARRAY */
	dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, ns1, &lint, nnn, err,
		 noroom);
	if (*err) {
	    return 0;
	}
	dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &ns1[3], &lint, nnn, 
		err, noroom);
	if (*err) {
	    return 0;
	}
	addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &ns1[2], &lint, nnn, 
		err, noroom);
	if (*err) {
	    return 0;
	}
	addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &ns2[3], &lint, nnn, 
		err, noroom);
	if (*err) {
	    return 0;
	}
	*done = TRUE_;
/*  FORWARD STRUCTURE IS BEST.  IMPLEMENT IT. */
/*  FIX LXK AND KXL ARRAYS */
    } else if (ifgood) {
	fndlnk_(mxnd, &lxk[5], &nxl[3], k, ns1, &ns1[1], &l1ee, err);
	if (*err) {
	    return 0;
	}
	fndlnk_(mxnd, &lxk[5], &nxl[3], k2, &ns2[1], &ns2[2], &l2ee, err);
	if (*err) {
	    return 0;
	}
	lswap_(mxnd, &lxk[5], &kxl[3], k, &l1ee, k2, &l2ee, err);
	if (*err) {
	    return 0;
	}
/*  FIX NXL ARRAY  (MOVE THE DIAGONAL) */
	nxl[(lint << 1) + 1] = ns1[1];
	nxl[(lint << 1) + 2] = ns2[2];
/*  FIX LXN ARRAY */
	dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, ns1, &lint, nnn, err,
		 noroom);
	if (*err) {
	    return 0;
	}
	dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &ns1[3], &lint, nnn, 
		err, noroom);
	if (*err) {
	    return 0;
	}
	addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &ns1[1], &lint, nnn, 
		err, noroom);
	if (*err) {
	    return 0;
	}
	addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &ns2[2], &lint, nnn, 
		err, noroom);
	if (*err) {
	    return 0;
	}
	*done = TRUE_;
    }
    return 0;
} /* restry_ */

#ifdef __cplusplus
	}
#endif

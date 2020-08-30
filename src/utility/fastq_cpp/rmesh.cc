/* rmesh.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int rmesh_(integer *nper, integer *mxnd, real *x, real *y, 
	integer *nid, real *xn, real *yn, integer *nuid, integer *lxk, 
	integer *kxl, integer *nxl, integer *lxn, integer *m1, integer *m2, 
	integer *kkk, integer *kkkold, integer *nnn, integer *nnnold, integer 
	*lll, integer *lllold, integer *iavail, integer *navail, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 IN RMESH, N = \002,i5,\002 AND M1 = \
\002,i5,\002 ARE INCOMPATIBLE\002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, k, l, k1, k2, i11, kb, im, kl, ip, kr, kt, i1z, 
	    iz1;
    static real eta;
    static integer nlc, m1p1, m2p1;
    static real eps;
    static integer node;
    static real ometa, omeps;
    extern /* Subroutine */ int ccross_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *);
    static logical noroom;

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE RMESH = GENERATES AN INITIAL LOGICAL RECTANGULAR MESH */
/*                     WHOSE PERIMETER IS  (X (I), Y (I), I=1, N). */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     X     = X VALUES AROUND THE PERIMETER */
/*     Y     = Y VALUES AROUND THE PERIMETER */
/*     NID   = PERIMETER NODE UNIQUE ID'S */
/*     N     = NUMBER OF PERIMETER NODES */
/*     M1    = THE NUMBER OF INTERVALS ON THE FIRST SIDE OF THE RECTANGLE */
/*     IMAP  = CONTROLS THE DEFINITION OF THE VALUES OF THE COORDINATE */
/*             OF THE INTERIOR NODES. */
/*           = 1 MEANS SET ALL COORDINATES TO 0. */
/*           = 2 MEANS SET ALL COORDINATES TO THE CENTROID OF PERIMETER */
/*           = 3 MEANS APPLY THE UNIT SQUARE TRANSFORMATION FROM */
/*             W.A.COOK'S PAPER  (THIOKOL REPORT AFRDL - TR - 71 - 51) */
/*     ERR   = .TRUE. IF ERRORS WERE ENCOUNTERED */
/*     XN    = GLOBAL X VALUES OF NODES */
/*     YN    = GLOBAL Y VALUES OF NODES */
/*     NUID  = GLOBAL NODE UNIQUE IDENTIFIERS */
/*     LXK   = LINES PER ELEMENT */
/*     KXL   = ELEMENTS PER LINE */
/*     NXL   = NODES PER LINE */
/*     LXN   = LINES PER NODE */
/*  NOTE: */
/*     FOR *XN TABLES A NEGATIVE FLAG IN THE FOURTH COLUMN MEANS */
/*     GO TO THAT ROW FOR A CONTINUATION OF THE LIST.  IN THAT ROW */
/*     THE FIRST ELEMENT WILL BE NEGATED TO INDICATE THAT THIS IS */
/*     A CONTINUATION ROW.   (RMESH ITSELF GENERATES NO SUCH NEGATIVES.) */
/*     A NEGATIVE FLAG IN THE SECOND COLUMN OF THE LXN ARRAY MEANS */
/*     THAT THIS NODE IS A BOUNDARY NODE. */
/* *********************************************************************** */
/*  NOTE: NOROOM SHOULD NEVER BE TRUE WITH CROSS BEING CALLED IN RMESH. */
/*        THUS,  IT IS NEVER PASSED BACK TO QMESH. */
    /* Parameter adjustments */
    --nid;
    --y;
    --x;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;

    /* Function Body */
    *kkk = *m1 * *m2 + *kkkold;
    *lll = *m1 * (*m2 + 1) + *m2 * (*m1 + 1) + *lllold;
    *nnn = (*m1 + 1) * (*m2 + 1) + *nnnold;
    *err = TRUE_;
/*  CHECK INPUT */
    if (*nper / 2 << 1 != *nper) {
	mesage_("IN RMESH,  NO. OF PERIMETER NODES IS ODD", (ftnlen)40);
	return 0;
    } else if (*m1 < 1 || *m2 < 1) {
	s_wsfe(&io___1);
	do_fio(&c__1, (char *)&(*nper), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*m1), (ftnlen)sizeof(integer));
	e_wsfe();
	return 0;
    }
/*  COMPUTE CONSTANTS */
    nlc = (*m1 << 1) + 1;
    m1p1 = *m1 + 1;
    m2p1 = *m2 + 1;
/*  PRODUCE LXK ARRAY */
/*  LINES FOR FIRST ELEMENT */
    lxk[(*kkkold + 1 << 2) + 1] = *lllold + 1;
    lxk[(*kkkold + 1 << 2) + 2] = *m1 + 1 + *lllold;
    lxk[(*kkkold + 1 << 2) + 3] = *m1 + 2 + *lllold;
    lxk[(*kkkold + 1 << 2) + 4] = nlc + 1 + *lllold;
/*  FIRST ROW  (SHIFT FIRST ELEMENT TO SECOND,  ETC.) */
    if (*m1 > 1) {
	i__1 = *m1;
	for (k = 2; k <= i__1; ++k) {
	    for (i__ = 1; i__ <= 4; ++i__) {
		lxk[i__ + (k + *kkkold << 2)] = lxk[i__ + (k + *kkkold - 1 << 
			2)] + 1;
/* L100: */
	    }
/* L110: */
	}
    }
/*  SUCCEEDING ROWS  (SHIFT FIRST COLUMN TO SECOND, ETC.) */
    if (*m2 > 1) {
	k = *m1 + *kkkold;
	i__1 = *m2;
	for (k2 = 2; k2 <= i__1; ++k2) {
	    i__2 = *m1;
	    for (k1 = 1; k1 <= i__2; ++k1) {
		++k;
		kl = k - *m1;
		for (i__ = 1; i__ <= 4; ++i__) {
		    lxk[i__ + (k << 2)] = lxk[i__ + (kl << 2)] + nlc;
/* L120: */
		}
/* L130: */
	    }
/* L140: */
	}
    }
/*  PREPARE KXL TABLE BY USING SUBROUTINE CROSS ON THE LXK TABLE */
    i__1 = *kkkold + 1;
    i__2 = *lllold + 1;
    ccross_(&c__4, kkk, &c__2, lll, &lxk[5], &kxl[3], &i__1, &i__2, &noroom, 
	    err);
    if (*err) {
	return 0;
    }
    *err = TRUE_;
/*  MAKE NXL TABLE */
/*  FIRST DO HORIZONTAL LINES */
    i__1 = m2p1;
    for (j = 1; j <= i__1; ++j) {
	node = (j - 1) * (*m1 + 1) + 1 + *nnnold;
	l = (j - 1) * nlc + 1 + *lllold;
	i__2 = *m1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    nxl[(l << 1) + 1] = node;
	    nxl[(l << 1) + 2] = node + 1;
	    ++node;
	    ++l;
/* L150: */
	}
/* L160: */
    }
/*  NEXT DO VERTICAL LINES */
    i__1 = m1p1;
    for (j = 1; j <= i__1; ++j) {
	node = j + *nnnold;
	l = j + *m1 + *lllold;
	i__2 = *m2;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    nxl[(l << 1) + 1] = node;
	    nxl[(l << 1) + 2] = node + m1p1;
	    node += m1p1;
	    l += nlc;
/* L170: */
	}
/* L180: */
    }
/*  PREPARE LXN TABLE FROM NXL TABLE */
    i__1 = *lllold + 1;
    i__2 = *nnnold + 1;
    ccross_(&c__2, lll, &c__4, nnn, &nxl[3], &lxn[5], &i__1, &i__2, &noroom, 
	    err);
    if (*err) {
	return 0;
    }
    *err = TRUE_;
/*  LINK - UP AVAILABLE LXN SPACE */
    *iavail = *nnn + 1;
    *navail = *mxnd - *nnn;
    i__1 = *mxnd;
    for (i__ = *iavail; i__ <= i__1; ++i__) {
	lxn[(i__ << 2) + 1] = 0;
	lxn[(i__ << 2) + 2] = 0;
	lxn[(i__ << 2) + 3] = 0;
	lxn[(i__ << 2) + 4] = i__ + 1;
/* L190: */
    }
/*  LOGICAL CONNECTION TABLES ARE COMPLETE */
/*  FILL IN THE CO - ORDINATES OF THE INTERIOR POINTS */
/*  USE THE UNIT SQUARE TRANSFORMATION  (OF COOK / THIOKOL) */
    if (*m1 > 1 && *m2 > 1) {
/*  GET NODE NUMBERS FOR CORNERS */
	i1z = *m1 + 1;
	iz1 = *nper - (*m2 - 1);
	i11 = *m1 + *m2 + 1;
/*  COLUMN LOOP */
	i__1 = *m2;
	for (j = 2; j <= i__1; ++j) {
	    kl = *nper + 2 - j;
	    kr = *m1 + j;
	    eta = (doublereal) (j - 1) / (doublereal) (*m2);
	    ometa = (float)1. - eta;
/*  ROW LOOP */
	    i__2 = *m1;
	    for (i__ = 2; i__ <= i__2; ++i__) {
		kb = i__;
		kt = iz1 + 1 - i__;
		eps = (doublereal) (i__ - 1) / (doublereal) (*m1);
		omeps = (float)1. - eps;
		im = (j - 1) * m1p1 + i__ + *nnnold;
		xn[im] = ometa * x[kb] + eta * x[kt] + omeps * x[kl] + eps * 
			x[kr] - (x[1] * ometa * omeps + x[i1z] * ometa * eps 
			+ x[iz1] * eta * omeps + x[i11] * eta * eps);
		yn[im] = ometa * y[kb] + eta * y[kt] + omeps * y[kl] + eps * 
			y[kr] - (y[1] * ometa * omeps + y[i1z] * ometa * eps 
			+ y[iz1] * eta * omeps + y[i11] * eta * eps);
/* L200: */
	    }
/* L210: */
	}
    }
/*  DEFINE THE COORDINATES OF THE PERIMETER NODES. */
/*  ALSO FLAG SECOND ELEMENTS OF LXN ARRAY TO INDICATE */
/*  WHICH NODES ARE BOUNDARY NODES. */
/*  DEFINE UNIQUE NODE ID NUMBERS ALSO. */
    i__1 = *nnn;
    for (i__ = *nnnold + 1; i__ <= i__1; ++i__) {
	nuid[i__] = 0;
/* L220: */
    }
/*  BOTTOM */
    ip = 0;
    i__1 = m1p1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	im = i__ + *nnnold;
	++ip;
	lxn[(im << 2) + 2] = -lxn[(im << 2) + 2];
	nuid[im] = nid[ip];
	xn[im] = x[ip];
	yn[im] = y[ip];
/* L230: */
    }
/*  RIGHT */
    ip = m1p1;
    i__1 = m2p1;
    for (i__ = 2; i__ <= i__1; ++i__) {
	++ip;
	im += m1p1;
	lxn[(im << 2) + 2] = -lxn[(im << 2) + 2];
	nuid[im] = nid[ip];
	xn[im] = x[ip];
	yn[im] = y[ip];
/* L240: */
    }
/*  TOP */
    i__1 = m1p1;
    for (i__ = 2; i__ <= i__1; ++i__) {
	++ip;
	--im;
	lxn[(im << 2) + 2] = -lxn[(im << 2) + 2];
	nuid[im] = nid[ip];
	xn[im] = x[ip];
	yn[im] = y[ip];
/* L250: */
    }
/*  LEFT */
    i__1 = *m2;
    for (i__ = 2; i__ <= i__1; ++i__) {
	++ip;
	im -= m1p1;
	lxn[(im << 2) + 2] = -lxn[(im << 2) + 2];
	nuid[im] = nid[ip];
	xn[im] = x[ip];
	yn[im] = y[ip];
/* L260: */
    }
/*  EXIT */
    *err = FALSE_;
    return 0;
} /* rmesh_ */

#ifdef __cplusplus
	}
#endif

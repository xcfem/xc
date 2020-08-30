/* innerh.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int innerh_(integer *mxnd, integer *nxh, integer *nuid, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, integer *kkk, 
	integer *lll, integer *nnn, integer *nnnold, integer *nh, integer *
	istart, integer *iavail, integer *navail, logical *noroom, logical *
	err)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j, k;
    extern /* Subroutine */ int fndlin_fq__(integer *, integer *, integer *, 
	    integer *, integer *, logical *);
    static integer i1, i2, jj, line;
    extern /* Subroutine */ int addlxn_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *);
    static integer lllold, maxlnk;

/* *********************************************************************** */
/*  SUBROUTINE INNERH  =  INSERT A ROW OF ELEMENTS AROUND A HOLE */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --nxh;

    /* Function Body */
    *err = FALSE_;
    if (*kkk + *nh > *mxnd || *lll + (*nh << 1) > *mxnd * 3 || *nnn + *nh > *
	    mxnd) {
	*noroom = TRUE_;
	goto L200;
    } else {
	*noroom = FALSE_;
    }
/*  GENERATE LINES */
    lllold = *lll;
    i1 = *istart;
    i__1 = *nh;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++(*lll);
	nxl[(*lll << 1) + 1] = nxh[i1];
	addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nxl[(*lll << 1) + 1]
		, lll, nnn, err, noroom);
	if (*noroom || *err) {
	    goto L200;
	}
	nxl[(*lll << 1) + 2] = *nnnold + i__;
	++i1;
	if (i1 > *nh) {
	    i1 = 1;
	}
/* L100: */
    }
    i__1 = *nh - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++(*lll);
	nxl[(*lll << 1) + 1] = *nnnold + i__;
	nxl[(*lll << 1) + 2] = *nnnold + i__ + 1;
/* L110: */
    }
    ++(*lll);
    nxl[(*lll << 1) + 1] = *nnn;
    nxl[(*lll << 1) + 2] = *nnnold + 1;
/*  MAKE SPACE IN LXN LIST FOR NEW NODES */
    if (*iavail < *nnnold) {
	jj = *iavail;
	maxlnk = 0;
L120:
	if (lxn[(jj << 2) + 4] <= *nnnold) {
	    jj = lxn[(jj << 2) + 4];
	    ++maxlnk;
	    if (maxlnk > *nnnold) {
		mesage_("INNERH - LXN LINKED LIST ERROR", (ftnlen)30);
		*err = TRUE_;
		goto L200;
	    } else {
		goto L120;
	    }
	}
	lxn[(jj << 2) + 4] = *nnnold + *nh + 1;
	*navail -= *nh;
    } else if (*iavail > *nnnold) {
	i__1 = *iavail - 1;
	for (j = 1; j <= i__1; ++j) {
	    if (lxn[(j << 2) + 4] < 0 && (i__2 = lxn[(j << 2) + 4], abs(i__2))
		     > *nnnold) {
		lxn[(j << 2) + 4] -= *nh;
	    }
/* L130: */
	}
	jj = *nnnold + *nh;
	i__1 = *iavail - 1;
	for (j = *nnnold + 1; j <= i__1; ++j) {
	    ++jj;
	    for (k = 1; k <= 4; ++k) {
		lxn[k + (jj << 2)] = lxn[k + (j << 2)];
/* L140: */
	    }
/* L150: */
	}
	*iavail = jj + 1;
	*navail -= *nh;
    } else {
	*iavail = *nnn + 1;
	*navail = *mxnd - *nnn;
    }
/*  MARK NODES ON HOLE BOUNDARY */
    lxn[(*nnnold + 1 << 2) + 1] = lllold + 1;
    lxn[(*nnnold + 1 << 2) + 2] = -(lllold + (*nh << 1));
    lxn[(*nnnold + 1 << 2) + 3] = lllold + *nh + 1;
    lxn[(*nnnold + 1 << 2) + 4] = 0;
    *nnn = *nnnold + 1;
    i__1 = *nh;
    for (i__ = 2; i__ <= i__1; ++i__) {
	++(*nnn);
	lxn[(*nnn << 2) + 1] = lllold + i__;
	lxn[(*nnn << 2) + 2] = -(lllold + *nh + i__ - 1);
	lxn[(*nnn << 2) + 3] = lllold + *nh + i__;
	lxn[(*nnn << 2) + 4] = 0;
/* L160: */
    }
/*  GENERATE ELEMENTS */
    i__1 = *lll;
    for (i__ = lllold + 1; i__ <= i__1; ++i__) {
	for (j = 1; j <= 2; ++j) {
	    kxl[j + (i__ << 1)] = 0;
/* L170: */
	}
/* L180: */
    }
    i1 = *istart;
    i2 = i1 + 1;
    if (i2 > *nh) {
	i2 = 1;
    }
    i__1 = *nh - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	fndlin_fq__(mxnd, &lxn[5], &nxh[i1], &nxh[i2], &line, err);
	if (*err) {
	    goto L200;
	}
	++(*kkk);
	lxk[(*kkk << 2) + 1] = line;
	lxk[(*kkk << 2) + 2] = lllold + i__;
	lxk[(*kkk << 2) + 3] = lllold + i__ + 1;
	lxk[(*kkk << 2) + 4] = lllold + i__ + *nh;
	++i1;
	if (i1 > *nh) {
	    i1 = 1;
	}
	i2 = i1 + 1;
	if (i2 > *nh) {
	    i2 = 1;
	}
	if (kxl[(line << 1) + 1] == 0) {
	    kxl[(line << 1) + 1] = *kkk;
	} else if (kxl[(line << 1) + 2] == 0) {
	    kxl[(line << 1) + 2] = *kkk;
	} else {
	    mesage_("KXL TABLE FULL", (ftnlen)14);
	}
	kxl[(lllold + i__ << 1) + 2] = *kkk;
	kxl[(lllold + i__ + 1 << 1) + 1] = *kkk;
	kxl[(lllold + i__ + *nh << 1) + 1] = *kkk;
/* L190: */
    }
    fndlin_fq__(mxnd, &lxn[5], &nxh[i1], &nxh[i2], &line, err);
    if (*err) {
	goto L200;
    }
    ++(*kkk);
    lxk[(*kkk << 2) + 1] = line;
    lxk[(*kkk << 2) + 2] = lllold + *nh;
    lxk[(*kkk << 2) + 3] = lllold + 1;
    lxk[(*kkk << 2) + 4] = lllold + (*nh << 1);
    if (kxl[(line << 1) + 1] == 0) {
	kxl[(line << 1) + 1] = *kkk;
    } else if (kxl[(line << 1) + 2] == 0) {
	kxl[(line << 1) + 2] = *kkk;
    } else {
	mesage_("KXL TABLE FULL", (ftnlen)14);
    }
    kxl[(lllold + *nh << 1) + 2] = *kkk;
    kxl[(lllold + 1 << 1) + 1] = *kkk;
    kxl[(lllold + (*nh << 1) << 1) + 1] = *kkk;
L200:
    return 0;
} /* innerh_ */

#ifdef __cplusplus
	}
#endif

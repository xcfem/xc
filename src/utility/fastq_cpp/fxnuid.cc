/* fxnuid.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int fxnuid_(integer *nregn, integer *igroup, integer *mr, 
	integer *ms, integer *ml, integer *nspr, integer *iline, integer *
	iside, integer *nlps, integer *ifline, integer *illist, integer *lcon,
	 integer *islist, integer *ifside, integer *linkr, integer *links, 
	integer *linkl, integer *nnn, integer *maxnl, integer *mxnd, integer *
	listl, integer *nuid, integer *nxl, integer *lxn, integer *indx, 
	logical *noroom, logical *err)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j;
    extern /* Subroutine */ int indexi_fq__(integer *, integer *, integer *, 
	    integer *);
    static integer i1, i2, n1, nl, key, key1, node, ibot;
    static logical ldup;
    static integer numl, numn, iptr, lines[20];
    extern /* Subroutine */ int llist_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *);
    static integer kount;
    static logical addlnk;
    extern /* Subroutine */ int getlxn_(integer *, 
	    integer *, integer *, integer *, integer *, logical *), ltsort_(
	    integer *, integer *, integer *, integer *, logical *);

/* *********************************************************************** */
/*     FXNUID - FIX NUID'S:  RESETS NUID'S OF INTERIOR LINES IN GROUPS */
/*                           TO ZERO */
/* *********************************************************************** */
/*  GET LIST OF LINES */
    /* Parameter adjustments */
    --igroup;
    linkr -= 3;
    --ifside;
    --islist;
    --nspr;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --iside;
    linkl -= 3;
    lcon -= 4;
    --iline;
    --listl;
    --indx;
    lxn -= 5;
    nxl -= 3;
    --nuid;

    /* Function Body */
    *noroom = FALSE_;
    *err = FALSE_;
    addlnk = FALSE_;
    n1 = 1;
    i__1 = *nregn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mr, &linkr[3], &igroup[i__], &iptr, &addlnk);
	if (iptr > 0) {
	    llist_(ms, ml, maxnl, &nspr[iptr], &nl, &igroup[i__], &listl[n1], 
		    &iline[1], &iside[1], &nlps[1], &ifline[1], &illist[1], &
		    lcon[4], &islist[ifside[iptr]], &links[3], &linkl[3], err)
		    ;
	    n1 += nl;
	    if (n1 > *maxnl) {
		mesage_("IN FXNUID, LINE LIST OVERFLOW", (ftnlen)29);
		*noroom = TRUE_;
		*err = TRUE_;
		return 0;
	    }
	}
/* L100: */
    }
    numl = n1 - 1;
/*  SORT THE LINE LIST */
    if (numl > 1) {
	i__1 = numl;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    indx[i__] = i__;
/* L110: */
	}
	indexi_fq__(&numl, &listl[1], &numl, &indx[1]);
    } else {
	return 0;
    }
/*  IDENTIFY INTERIOR LINES */
    i1 = 1;
L120:
    ldup = FALSE_;
    if (i1 < numl) {
	i2 = i1 + 1;
L130:
	if (i2 <= numl) {
	    if (listl[indx[i1]] == listl[indx[i2]]) {
		indx[i2] = 0;
		++i2;
		ldup = TRUE_;
		goto L130;
	    } else {
		if (! ldup) {
		    indx[i1] = 0;
		}
		i1 = i2;
		goto L120;
	    }
	}
    }
/*  FORM SORTED LINE LIST IN INDX THEN COPY IT BACK TO LISTL */
    n1 = 0;
    i__1 = numl;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (indx[i__] > 0) {
	    ++n1;
	    indx[n1] = listl[indx[i__]];
	}
/* L140: */
    }
    numl = n1;
    i__1 = numl;
    for (i__ = 1; i__ <= i__1; ++i__) {
	listl[i__] = indx[i__];
/* L150: */
    }
/*  SORT NUID'S ON LINE'S FOR SPEEDY LOOKUP */
    n1 = 0;
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nuid[i__] > 1000000000) {
	    ++n1;
	    indx[n1] = i__;
	}
/* L160: */
    }
    numn = n1;
    if (numn > 1) {
	indexi_fq__(nnn, &nuid[1], &numn, &indx[1]);
    }
/*  LOOP FOR INTERIOR LINES */
    i__1 = numl;
    for (i__ = 1; i__ <= i__1; ++i__) {
	key = listl[i__] * 100000 + 1000000000;
/*  FIND LOW POINT */
	ibot = 0;
	i__2 = numn;
	for (j = 1; j <= i__2; ++j) {
	    ibot = j;
	    if (nuid[indx[j]] >= key) {
		goto L180;
	    }
/* L170: */
	}
L180:
/*  CHECK INDIVIDUAL POINTS BETWEEN LOW + 1 AND HIGH - 1 */
	key1 = key / 100000;
	i__2 = numn;
	for (j = ibot; j <= i__2; ++j) {
	    if (nuid[indx[j]] / 100000 == key1) {
		lxn[(indx[j] << 2) + 2] = (i__3 = lxn[(indx[j] << 2) + 2], 
			abs(i__3));
		indx[j] = 0;
	    } else if (nuid[indx[j]] > key) {
		goto L200;
	    }
/* L190: */
	}
L200:
/*  COMPACT NUID'S INDEX LIST */
	n1 = 0;
	i__2 = numn;
	for (j = 1; j <= i__2; ++j) {
	    if (indx[j] > 0) {
		++n1;
		indx[n1] = indx[j];
	    }
/* L210: */
	}
	numn = n1;
/* L220: */
    }
/*  CHECK ALL POINT NUID'S TO MAKE SURE THEY ARE ON BOUNDARY */
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nuid[i__] > 0 && nuid[i__] < 100000) {
	    node = i__;
	    getlxn_(mxnd, &lxn[5], &node, lines, &nl, err);
	    if (nl > 20) {
		mesage_("IN FXNUID, TOO MANY LINES/NODE", (ftnlen)30);
		*noroom = TRUE_;
		*err = TRUE_;
		return 0;
	    }
	    kount = 0;
	    i__2 = nl;
	    for (j = 1; j <= i__2; ++j) {
		i1 = nxl[(lines[j - 1] << 1) + 1] + nxl[(lines[j - 1] << 1) + 
			2] - i__;
		if (lxn[(i1 << 2) + 2] < 0) {
		    ++kount;
		}
/* L230: */
	    }
	    if (kount < 2) {
		lxn[(i__ << 2) + 2] = (i__2 = lxn[(i__ << 2) + 2], abs(i__2));
	    }
	}
/* L240: */
    }
    return 0;
} /* fxnuid_ */

#ifdef __cplusplus
	}
#endif

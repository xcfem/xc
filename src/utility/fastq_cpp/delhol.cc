/* delhol.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int delhol_(integer *iptr, integer *mxnd, integer *lxk, 
	integer *kxl, integer *nxl, integer *lxn, integer *nxh, integer *nuid,
	 integer *nnn, integer *iavail, integer *navail, logical *noroom, 
	logical *err)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer j, k, ks;
    extern /* Subroutine */ int gkxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *);
    static integer klist[20];
    extern /* Subroutine */ int dellxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *);

/* *********************************************************************** */
/*  SUBROUTINE DELNOD  =  DELETES ALL LINES, ELEMENTS, ETC. ATTACHED TO */
/*                        A NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nuid;
    --nxh;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;

    /* Function Body */
    *err = FALSE_;
    gkxn_(mxnd, &kxl[3], &lxn[5], iptr, &ks, klist, err);
    if (! (*err)) {
	*err = TRUE_;
/*  DELETE LINES PER ELEMENT; MARK ELEMENT NODES */
	i__1 = ks;
	for (j = 1; j <= i__1; ++j) {
	    for (k = 1; k <= 4; ++k) {
		if (nxh[nxl[(lxk[k + (klist[j - 1] << 2)] << 1) + 1]] == 0) {
		    nxh[nxl[(lxk[k + (klist[j - 1] << 2)] << 1) + 1]] = 1;
		}
		if (nxh[nxl[(lxk[k + (klist[j - 1] << 2)] << 1) + 2]] == 0) {
		    nxh[nxl[(lxk[k + (klist[j - 1] << 2)] << 1) + 2]] = 1;
		}
		if (kxl[(lxk[k + (klist[j - 1] << 2)] << 1) + 1] == klist[j - 
			1]) {
		    kxl[(lxk[k + (klist[j - 1] << 2)] << 1) + 1] = 0;
		}
		if (kxl[(lxk[k + (klist[j - 1] << 2)] << 1) + 2] == klist[j - 
			1]) {
		    kxl[(lxk[k + (klist[j - 1] << 2)] << 1) + 2] = 0;
		}
		lxk[k + (klist[j - 1] << 2)] = 0;
/* L100: */
	    }
/* L110: */
	}
	nxh[*iptr] = -1;
	for (j = 1; j <= 3; ++j) {
	    if (lxn[j + (*iptr << 2)] > 0) {
/*  DELETE LINE ATTACHED TO OPPOSITE END NODE */
		if (nxl[(lxn[j + (*iptr << 2)] << 1) + 1] == *iptr) {
		    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nxl[(
			    lxn[j + (*iptr << 2)] << 1) + 2], &lxn[j + (*iptr 
			    << 2)], nnn, err, noroom);
		} else if (nxl[(lxn[j + (*iptr << 2)] << 1) + 2] == *iptr) {
		    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nxl[(
			    lxn[j + (*iptr << 2)] << 1) + 1], &lxn[j + (*iptr 
			    << 2)], nnn, err, noroom);
		}
		if (*noroom) {
		    return 0;
		}
/*  DELETE NODES PER LINE; ELEMENTS PER LINE */
		nxl[(lxn[j + (*iptr << 2)] << 1) + 1] = 0;
		nxl[(lxn[j + (*iptr << 2)] << 1) + 2] = 0;
		kxl[(lxn[j + (*iptr << 2)] << 1) + 1] = 0;
		kxl[(lxn[j + (*iptr << 2)] << 1) + 2] = 0;
		lxn[j + (*iptr << 2)] = 0;
	    }
/* L120: */
	}
/*  FOR LAST LINE, SAVE LINK ON IAVAIL */
	if (lxn[(*iptr << 2) + 4] > 0) {
	    if (nxl[(lxn[(*iptr << 2) + 4] << 1) + 1] == *iptr) {
		dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nxl[(lxn[(*
			iptr << 2) + 4] << 1) + 2], &lxn[(*iptr << 2) + 4], 
			nnn, err, noroom);
	    } else if (nxl[(lxn[j + (*iptr << 2)] << 1) + 2] == *iptr) {
		dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nxl[(lxn[(*
			iptr << 2) + 4] << 1) + 1], &lxn[(*iptr << 2) + 4], 
			nnn, err, noroom);
	    }
	    if (*noroom) {
		return 0;
	    }
	    nxl[(lxn[(*iptr << 2) + 4] << 1) + 1] = 0;
	    nxl[(lxn[(*iptr << 2) + 4] << 1) + 2] = 0;
	    kxl[(lxn[(*iptr << 2) + 4] << 1) + 1] = 0;
	    kxl[(lxn[(*iptr << 2) + 4] << 1) + 2] = 0;
	}
	lxn[(*iptr << 2) + 4] = *iavail;
	*iavail = *iptr;
	++(*navail);
	*err = FALSE_;
    }
    return 0;
} /* delhol_ */

#ifdef __cplusplus
	}
#endif

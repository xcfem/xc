/* getcrn.f -- translated by f2c (version 20160102).
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
//#include "f2c.h"
#include "paving.h"

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int getcrn_(integer *mxnd, integer *mxcorn, integer *mln, 
	integer *lnodes, integer *ncorn, integer *lcorn, real *angle, real *
	xn, real *yn, integer *lxn, integer *nloop, integer *n1, logical *
	onlyc, logical *pposbl, logical *graph, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;

    /* Local variables */
    static integer j;
    static real x[1], y[1];
    static integer kkc, node, nold, isqr, istat, kount;
    extern /* Subroutine */ int mesage_(char *, ftnlen), lcolor_(char *, 
	    ftnlen), ndstat_(integer *, integer *, real *, integer *);
    static integer kountc;
    extern /* Subroutine */ int sflush_(), symbol_(integer *, real *, real *, 
	    char *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE GETCRN = SETS UP ALL THE POSSIBLE CORNER (OR NON-SIDE) */
/*                      LOCATIONS IN THE MESH */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    --yn;
    --xn;
    --angle;
    --lcorn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *err = FALSE_;
/*  COUNT THE CURRENT "POSSIBLE NON-SIDES" STARTING AT THE I COUNTER */
    node = *n1;
    nold = *n1;
    kountc = 0;
    kount = 0;
    *ncorn = 0;
    *pposbl = TRUE_;
L100:
    ++kount;
    if (kount > *nloop) {
	mesage_("PROBLEM IN GETCRN WITH UNCLOSED LOOP", (ftnlen)36);
	*err = TRUE_;
	goto L120;
    }
/*  CHECK IF A PRIMITIVE IS EVEN POSSIBLE HERE */
    if (lnodes[node * lnodes_dim1 + 6] > 4) {
	*pposbl = FALSE_;
    }
/*  CHECK FOR A POSSIBLE "NON - SIDE" NODE */
    if (*onlyc) {
	ndstat_(&node, &lxn[(node << 2) + 1], &angle[node], &istat);
    }
/*  A NEW "POSSIBLE NON-SIDE" NODE HAS BEEN FOUND */
    if (*onlyc && istat == 1 || ! (*onlyc) && lnodes[node * lnodes_dim1 + 6] 
	    != 3) {
	if (*graph) {
	    isqr = node;
	    x[0] = xn[isqr];
	    y[0] = yn[isqr];
	    if (*onlyc && istat == 1) {
		symbol_(&c__1, x, y, "DIAMND", (ftnlen)6);
	    } else if (! (*onlyc)) {
		if (lnodes[node * lnodes_dim1 + 6] == 1) {
		    symbol_(&c__1, x, y, "DIAMND", (ftnlen)6);
		} else if (lnodes[node * lnodes_dim1 + 6] == 2) {
		    symbol_(&c__1, x, y, "CENTRX", (ftnlen)6);
		} else if (lnodes[node * lnodes_dim1 + 6] == 4) {
		    symbol_(&c__1, x, y, "TRINGL", (ftnlen)6);
		} else {
		    symbol_(&c__1, x, y, "CIRCLE", (ftnlen)6);
		}
	    }
	    sflush_();
	}
/*  TOO MANY "NON-SIDES" HAVE BEEN FOUND - JUST PICK ONE AND GO */
	if (*ncorn == *mxcorn) {
	    *pposbl = FALSE_;
	    i__1 = *ncorn;
	    for (j = 1; j <= i__1; ++j) {
		if (lnodes[lcorn[j] * lnodes_dim1 + 6] == 1) {
		    node = j;
		    if (*graph) {
			lcolor_("WHITE", (ftnlen)5);
		    }
		    goto L120;
		}
/* L110: */
	    }
	    node = lcorn[1];
	    if (*graph) {
		lcolor_("WHITE", (ftnlen)5);
	    }
	    goto L120;
	}
/*  INPUT THIS "NON-SIDE" */
	++(*ncorn);
	lcorn[*ncorn] = node;
/*  ADD UP THE NUMBER OF NODES FROM THE LAST "NON-SIDE" */
	if (*ncorn > 1) {
	    lnodes[lcorn[*ncorn - 1] * lnodes_dim1 + 7] = kountc + 1;
	} else {
	    kkc = kountc + 1;
	}
	kountc = 0;
/*  THIS IS A SIDE - JUST CONTINUE */
    } else {
	++kountc;
    }
/*  CHECK FOR COMPLETION OF THE LOOP */
    node = lnodes[node * lnodes_dim1 + 3];
    if (node != nold) {
	goto L100;
    }
/*  GET THE FIRST CORNER'S DISTANCE FROM PREVIOUS CORNER CORRECT */
    if (*ncorn >= 1) {
	lnodes[lcorn[*ncorn] * lnodes_dim1 + 7] = kkc + kountc;
    }
L120:
    return 0;
} /* getcrn_ */

#ifdef __cplusplus
	}
#endif

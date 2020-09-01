/* close2.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int close2_(integer *mxnd, integer *mln, integer *nuid, real 
	*xn, real *yn, real *zn, integer *lxk, integer *kxl, integer *nxl, 
	integer *lxn, integer *lnodes, integer *iavail, integer *navail, 
	integer *nnn, integer *lll, integer *n1, real *xmin, real *xmax, real 
	*ymin, real *ymax, real *zmin, real *zmax, logical *pgraph, logical *
	video, char *dev1, integer *kreg, logical *noroom, logical *err, 
	ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static integer i__, n0, ii;
    static logical done;
    static integer kold, lold, knew, lnew, line1, line2, idum1, idum2;
    static logical check;
    extern /* Subroutine */ int delem_(integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *, logical *, logical *);
    static integer ngone;
    static logical found;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static integer nthere;
    extern /* Subroutine */ int dellxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *), snapit_(integer *), rplotl_(integer *, real *, real *,
	     real *, integer *, real *, real *, real *, real *, real *, real *
	    , integer *, char *, integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE CLOSE2 = SEALS OFF THE LAST 2 OPEN LINES WHILE CHECKING */
/*                      FOR FORMING A 2-LINE NODE ON THE INTERIOR */
/*                      (A 2-LINE NODE GENERATES 2 DEGENERATE QUADS) */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --zn;
    --yn;
    --xn;
    --nuid;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *err = FALSE_;
    check = FALSE_;
    n0 = lnodes[*n1 * lnodes_dim1 + 2];
    line1 = lnodes[n0 * lnodes_dim1 + 5];
    line2 = lnodes[*n1 * lnodes_dim1 + 5];
/*  CHECK TO MAKE SURE THAT AT LEAST ONE OF THE LINES */
/*  IS NOT A BOUNDARY LINE AND GET THE NODE TO BE DELETED */
L100:
    if (kxl[(line1 << 1) + 1] > 0 || kxl[(line2 << 1) + 1] > 0) {
	found = TRUE_;
	if (kxl[(line1 << 1) + 1] > 0) {
	    lnew = line2;
	    lold = line1;
	} else {
	    lnew = line1;
	    lold = line2;
	}
	kold = kxl[(lold << 1) + 1];
	knew = kxl[(lnew << 1) + 1];
/*  CHECK FOR ONE OF THE NODES BEING A TWO LINE NODE */
	if (kold == knew) {
	    if (lxn[(n0 << 2) + 3] == 0) {
		ngone = n0;
		nthere = *n1;
	    } else if (lxn[(*n1 << 2) + 3] == 0) {
		ngone = *n1;
		nthere = n0;
	    } else {
		mesage_("** PROBLEMS WITH NO TWO LINE NODE ATTACHED IN CLOSE\
2 **", (ftnlen)55);
		*err = TRUE_;
		goto L150;
	    }
/*  DELETE THE TWO-LINE NODE, THE TWO LINES, AND THE ELEMENT */
	    kxl[(lold << 1) + 1] = 0;
	    kxl[(lold << 1) + 2] = 0;
	    nxl[(lold << 1) + 1] = 0;
	    nxl[(lold << 1) + 2] = 0;
	    kxl[(lnew << 1) + 1] = 0;
	    kxl[(lnew << 1) + 2] = 0;
	    nxl[(lnew << 1) + 1] = 0;
	    nxl[(lnew << 1) + 2] = 0;
/*  UNHOOK BOTH LINES FROM NTHERE */
	    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nthere, &lold, 
		    nnn, err, noroom);
	    if (*err) {
		mesage_("** PROBLEMS IN CLOSE2 DELETING LOLD FROM NTHERE **", 
			(ftnlen)50);
		goto L150;
	    }
	    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nthere, &lnew, 
		    nnn, err, noroom);
	    if (*err) {
		mesage_("** PROBLEMS IN CLOSE2 DELETING LNEW FROM NTHERE **", 
			(ftnlen)50);
		goto L150;
	    }
/*  NOW DELETE NGONE AND THE ELEMENT */
	    for (i__ = 1; i__ <= 4; ++i__) {
		lxn[i__ + (ngone << 2)] = 0;
		if (lxk[i__ + (kold << 2)] == lnew || lxk[i__ + (kold << 2)] 
			== lold) {
		    lxk[i__ + (kold << 2)] = 0;
		}
/* L110: */
	    }
	    lold = 0;
	    lnew = 0;
	    for (i__ = 1; i__ <= 4; ++i__) {
		if (lxk[i__ + (kold << 2)] != 0) {
		    if (lold == 0) {
			lold = lxk[i__ + (kold << 2)];
		    } else {
			lnew = lxk[i__ + (kold << 2)];
		    }
		    lxk[i__ + (kold << 2)] = 0;
		}
/* L120: */
	    }
	    kxl[(lnew << 1) + 1] = kxl[(lnew << 1) + 1] + kxl[(lnew << 1) + 2]
		     - kold;
	    kxl[(lnew << 1) + 2] = 0;
	    kxl[(lold << 1) + 1] = kxl[(lold << 1) + 1] + kxl[(lold << 1) + 2]
		     - kold;
	    kxl[(lold << 1) + 2] = 0;
/*  NOW RESET THE NECESSARY VARIABLES */
	    *n1 = nxl[(lnew << 1) + 1];
	    n0 = nxl[(lnew << 1) + 2];
	    line1 = lold;
	    line2 = lnew;
	    goto L100;
	}
/*  DELETE THE OLD LINE AND REDO LINK ARRAYS */
	if (knew == 0) {
	    kxl[(lnew << 1) + 1] = kold;
	    kxl[(lnew << 1) + 2] = 0;
	} else {
	    kxl[(lnew << 1) + 1] = knew;
	    kxl[(lnew << 1) + 2] = kold;
	}
	kxl[(lold << 1) + 1] = 0;
	kxl[(lold << 1) + 2] = 0;
	nxl[(lold << 1) + 1] = 0;
	nxl[(lold << 1) + 2] = 0;
/*  FIX THE LINES PER ELEMENT ARRAY FOR THE ONE ELEMENT CHANGING */
	for (ii = 1; ii <= 4; ++ii) {
	    if (lxk[ii + (kold << 2)] == lold) {
		lxk[ii + (kold << 2)] = lnew;
		goto L140;
	    }
/* L130: */
	}
	mesage_("** PROBLEMS IN CLOSE2 WITH CHANGING ELEMENT **", (ftnlen)46);
	*err = TRUE_;
	goto L150;
L140:
/*  FIX LXN ARRAY */
/*  UNHOOK LOLD FROM N0 AND FROM N1 */
	dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &n0, &lold, nnn, err,
		 noroom);
	if (*err) {
	    mesage_("** PROBLEMS IN CLOSE2 DELETING NNN LINES **", (ftnlen)43)
		    ;
	    goto L150;
	}
	dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, n1, &lold, nnn, err, 
		noroom);
	if (*err) {
	    mesage_("** PROBLEMS IN CLOSE2 DELETING N1 LINES **", (ftnlen)42);
	    goto L150;
	}
/* NOW FIX THE LNODES ARRAY */
	lnodes[*n1 * lnodes_dim1 + 4] = -2;
	lnodes[n0 * lnodes_dim1 + 4] = -2;
    } else {
	mesage_("** PINCHED TOO FAR IN CLOSE2 **", (ftnlen)31);
	goto L150;
    }
/*  NOW SEE IF THE CLOSURE HAS PRODUCED A 2-LINE NODE AND */
/*  THUS REQUIRES THAT ONE OF THE ELEMENTS MUST BE SQUASHED */
    if (lxn[(n0 << 2) + 3] == 0 && lxn[(n0 << 2) + 2] > 0) {
	delem_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &
		lxn[5], nnn, navail, iavail, &n0, &kxl[(lnew << 1) + 1], &
		idum1, &idum2, &done, &check, noroom, err);
	if (*noroom || *err) {
	    goto L150;
	}
    } else if (lxn[(*n1 << 2) + 3] == 0 && lxn[(*n1 << 2) + 2] > 0) {
	delem_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &
		lxn[5], nnn, navail, iavail, n1, &kxl[(lnew << 1) + 1], &
		idum1, &idum2, &done, &check, noroom, err);
	if (*noroom || *err) {
	    goto L150;
	}
    }
    if (found && (*pgraph || *video)) {
	rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, ymax,
		 zmin, zmax, lll, dev1, kreg, (ftnlen)3);
	if (*video) {
	    snapit_(&c__1);
	}
    }
L150:
    return 0;
} /* close2_ */

#ifdef __cplusplus
	}
#endif

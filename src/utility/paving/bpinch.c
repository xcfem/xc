/* bpinch.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int bpinch_(integer *mxnd, integer *mln, integer *lnodes, 
	real *xn, real *yn, integer *lxn, integer *nxl, real *angle, integer *
	n0, integer *n1, integer *n2, integer *nloop, real *toler1, real *
	toler2, logical *bok, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static integer i__, ll, nl, np, n0a, n0b, n2a, n2b, ip1, ip2;
    static real ang, ang1, ang2;
    extern logical cornp_(real *);
    static real twopi;
    static integer l1list[20];
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static logical pangle;
    extern /* Subroutine */ int getlxn_(integer *, integer *, integer *, 
	    integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE BPINCH = CHECKS THAT A PINCH IS ALLOWABLE AND THAT IT */
/*                      DOESN'T FORCE A  DEGENERATE BOUNDARY ELEMENT */
/* *********************************************************************** */
    /* Parameter adjustments */
    --angle;
    nxl -= 3;
    lxn -= 5;
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    twopi = atan2((float)0., (float)-1.) * (float)2.;
/*  SEE IF THE ANGLE IS ELIGIBLE FOR PINCHING */
/*  FIRST CHECK A NONBOUNDARY NODE */
    if (lxn[(*n1 << 2) + 2] > 0) {
/*  CHECK A FOUR (OR LESS) LINE NODE */
	if (lxn[(*n1 << 2) + 4] >= 0) {
	    if (angle[*n1] < *toler1) {
		pangle = TRUE_;
	    } else {
		pangle = FALSE_;
	    }
/*  CHECK A FIVE (OR MORE) LINE NODE */
	} else {
	    if (angle[*n1] < *toler2) {
		pangle = TRUE_;
	    } else {
		pangle = FALSE_;
	    }
	}
/*  CHECK A BOUNDARY NODE */
    } else {
	if (angle[*n1] < *toler1 && lxn[(*n0 << 2) + 2] * lxn[(*n2 << 2) + 2] 
		< 0) {
	    pangle = TRUE_;
	} else if (angle[*n1] < *toler1 && lxn[(*n0 << 2) + 2] > 0 && lxn[(*
		n2 << 2) + 2] > 0) {
	    pangle = TRUE_;
	} else {
	    pangle = FALSE_;
	}
    }
    if (pangle) {
/*  ALL THREE ARE NOT ON THE BOUNDARY */
	if (lxn[(*n1 << 2) + 2] > 0 && lxn[(*n0 << 2) + 2] > 0 && lxn[(*n2 << 
		2) + 2] > 0) {
	    *bok = TRUE_;
/*  N0 AND N2 ARE ON THE BOUNDARY */
	} else if (lxn[(*n0 << 2) + 2] < 0 && lxn[(*n2 << 2) + 2] < 0) {
	    *bok = FALSE_;
/*  N1 AND N0 ARE ON THE BOUNDARY - FIND THE ANGLE THAT */
/*  THE BOUNDARY AT N1 MAKES */
	} else if (lxn[(*n0 << 2) + 2] < 0 && lxn[(*n1 << 2) + 2] < 0) {
	    getlxn_(mxnd, &lxn[5], n1, l1list, &nl, err);
	    if (*err) {
		mesage_("** PROBLEMS IN SEW2 FINDING LXN FOR J1 **", (ftnlen)
			41);
		goto L140;
	    }
	    i__1 = nl;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		ll = l1list[i__ - 1];
		if (ll != lnodes[*n0 * lnodes_dim1 + 5] && ll != lnodes[*n1 * 
			lnodes_dim1 + 5]) {
		    ip1 = nxl[(ll << 1) + 1];
		    ip2 = nxl[(ll << 1) + 2];
		    if (ip1 == *n1 && lxn[(ip2 << 2) + 2] < 0) {
			np = ip2;
			goto L110;
		    } else if (ip2 == *n1 && lxn[(ip1 << 2) + 2] < 0) {
			np = ip1;
			goto L110;
		    }
		}
/* L100: */
	    }
	    mesage_("** PROBLEMS IN BPINCH FINDING N1 BOUNDARY ANGLE NODE **",
		     (ftnlen)55);
	    goto L140;
L110:
	    ang1 = atan2(yn[*n0] - yn[*n1], xn[*n0] - xn[*n1]);
	    if (ang1 < (float)0.) {
		ang1 += twopi;
	    }
	    ang2 = atan2(yn[np] - yn[*n1], xn[np] - xn[*n1]);
	    if (ang2 < (float)0.) {
		ang2 += twopi;
	    }
	    ang = ang1 - ang2;
	    if (ang < (float)0.) {
		ang += twopi;
	    }
/*  NOW CHECK TO MAKE SURE THAT ANGLE IS NOT TOO LARGE */
	    if (ang < (float)2.3561945) {
		if (lxn[(*n1 << 2) + 3] == 0) {
		    *bok = FALSE_;
		} else {
		    *bok = TRUE_;
		}
	    } else {
		if (lxn[(*n1 << 2) + 4] == 0) {
		    *bok = FALSE_;
		} else {
		    *bok = TRUE_;
		}
	    }
/*  N1 AND N2 ARE ON THE BOUNDARY */
	} else if (lxn[(*n1 << 2) + 2] < 0 && lxn[(*n2 << 2) + 2] < 0) {
	    getlxn_(mxnd, &lxn[5], n1, l1list, &nl, err);
	    if (*err) {
		mesage_("** PROBLEMS IN SEW2 FINDING LXN FOR J1 **", (ftnlen)
			41);
		goto L140;
	    }
	    i__1 = nl;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		ll = l1list[i__ - 1];
		if (ll != lnodes[*n0 * lnodes_dim1 + 5] && ll != lnodes[*n1 * 
			lnodes_dim1 + 5]) {
		    ip1 = nxl[(ll << 1) + 1];
		    ip2 = nxl[(ll << 1) + 2];
		    if (ip1 == *n1 && lxn[(ip2 << 2) + 2] < 0) {
			np = ip2;
			goto L130;
		    } else if (ip2 == *n1 && lxn[(ip1 << 2) + 2] < 0) {
			np = ip1;
			goto L130;
		    }
		}
/* L120: */
	    }
	    mesage_("** PROBLEMS IN BPINCH FINDING N1 BOUNDARY ANGLE NODE **",
		     (ftnlen)55);
	    goto L140;
L130:
	    ang1 = atan2(yn[*n2] - yn[*n1], xn[*n2] - xn[*n1]);
	    if (ang1 < (float)0.) {
		ang1 += twopi;
	    }
	    ang2 = atan2(yn[np] - yn[*n1], xn[np] - xn[*n1]);
	    if (ang2 < (float)0.) {
		ang2 += twopi;
	    }
	    ang = ang2 - ang1;
	    if (ang < (float)0.) {
		ang += twopi;
	    }
/*  NOW CHECK THE ANGLE SIZE */
	    if (ang < (float)2.3561945) {
		if (lxn[(*n1 << 2) + 3] == 0) {
		    *bok = FALSE_;
		} else {
		    *bok = TRUE_;
		}
	    } else {
		if (lxn[(*n1 << 2) + 4] == 0) {
		    *bok = FALSE_;
		} else {
		    *bok = TRUE_;
		}
	    }
/*  ONLY N0 IS ON THE BOUNDARY */
	} else if (lxn[(*n0 << 2) + 2] < 0) {
	    n0a = lnodes[*n0 * lnodes_dim1 + 2];
	    n0b = lnodes[n0a * lnodes_dim1 + 2];
	    if (*nloop == 6 && lxn[(n0a << 2) + 2] < 0 && lxn[(n0b << 2) + 2] 
		    < 0 && ! cornp_(&angle[n0a])) {
		*bok = FALSE_;
	    } else {
		*bok = TRUE_;
	    }
/*  ONLY N1 IS ON THE BOUNDARY */
	} else if (lxn[(*n1 << 2) + 2] < 0) {
	    *bok = TRUE_;
/*  ONLY N2 IS ON THE BOUNDARY */
	} else if (lxn[(*n2 << 2) + 2] < 0) {
	    n2a = lnodes[*n2 * lnodes_dim1 + 3];
	    n2b = lnodes[n2a * lnodes_dim1 + 3];
	    if (*nloop == 6 && lxn[(n2a << 2) + 3] < 0 && lxn[(n2b << 2) + 3] 
		    < 0 && ! cornp_(&angle[n2a])) {
		*bok = FALSE_;
	    } else {
		*bok = TRUE_;
	    }
	}
    } else {
	*bok = FALSE_;
    }
L140:
    return 0;
} /* bpinch_ */

#ifdef __cplusplus
	}
#endif

/* match2.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int match2_(integer *mxnd, integer *mln, real *xn, real *yn, 
	integer *nxl, integer *lxn, integer *lnodes, real *angle, integer *n0,
	 integer *n1, integer *n2, integer *n3, integer *n0test, integer *
	n1test, integer *n2test, integer *n3test, integer *i1, integer *i2, 
	integer *j1, integer *j2, integer *kountl, logical *lmatch, integer *
	kount2, integer *node, real *u, real *w, integer *nloop, logical *
	pmatch, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;
    real r__1, r__2, r__3;

    /* Builtin functions */
    double sqrt(doublereal), acos(doublereal);

    /* Local variables */
    static real d0, d1, d2, db, df, xi, yi, xj, yj, d1b, d2b, d1f, d2f, xib, 
	    xjb, yjb, yib, xjf, yjf, xif, yif, dot, bdot, fdot;
    static integer ibac1, ibac2, jbac1, jbac2, ifor1, ifor2, jfor1, jfor2;
    extern logical sidep_(real *), cornp_(real *);
    static logical bwins, fwins;
    static integer i1hold, i2hold;
    extern logical matchk_(integer *, integer *, integer *, integer *, 
	    integer *, integer *);

/* *********************************************************************** */
/*  SUBROUTINE MATCH2 = MATCHES UP THE BEST PAIR OF LINES FOR COLLAPSING */
/* *********************************************************************** */
    /* Parameter adjustments */
    --angle;
    lxn -= 5;
    nxl -= 3;
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *err = FALSE_;
    *lmatch = TRUE_;
    bwins = FALSE_;
    fwins = FALSE_;
/*  MAKE SURE THAT AN ODD NUMBER OR SMALL NUMBER IN A LOOP HAS NOT BEEN */
/*  CUT OFF, AND IF IT HAS ADJUST THE INTERSECTION ACCORDINGLY. */
/*  FIRST CHECK A 2 NODE LOOP - A HIGHLY UNLIKELY CONDITION */
    if (*pmatch) {
	*i1 = *n1;
	*i2 = *n2;
	*j1 = *n1test;
	*j2 = *n2test;
	*kountl = *kount2 - 1;
    } else if (*kount2 == 2) {
	if (cornp_(&angle[*n2]) && lxn[(*n2 << 2) + 4] != 0) {
	    *i1 = *n1;
	    *i2 = *n2;
	    *j1 = *n2test;
	    *j2 = *n3test;
	    *kountl = 2;
	} else if (cornp_(&angle[*n1test]) && lxn[(*n1test << 2) + 4] != 0) {
	    *i1 = *n1;
	    *i2 = *n2;
	    *j1 = *n2test;
	    *j2 = *n3test;
	    *kountl = 2;
	} else {
	    if (cornp_(&angle[*n2])) {
		*node = *n2;
	    } else if (cornp_(&angle[*n1test])) {
		*node = *n1test;
	    } else {
		*node = *n2;
	    }
	    *lmatch = FALSE_;
	    goto L100;
	}
/*  NEXT CHECK A 3 NODE LOOP - THIS IS A MUCH MORE PLAUSIBLE CONDITION */
    } else if (*kount2 == 3) {
/*  CHECK FOR A 3-1 SEMICIRCLE BEING FORMED EITHER WAY */
	if (cornp_(&angle[*n2]) && lxn[(*n2 << 2) + 4] != 0 && sidep_(&angle[*
		n3])) {
	    *i1 = *n0;
	    *i2 = *n1;
	    *j1 = *n3;
	    *j2 = *n1test;
	    *kountl = 2;
	} else if (cornp_(&angle[*n1test]) && lxn[(*n1test << 2) + 4] != 0 && 
		sidep_(&angle[*n3])) {
	    *i1 = *n2;
	    *i2 = *n3;
	    *j1 = *n2test;
	    *j2 = *n3test;
	    *kountl = 2;
/*  JUST PUT IT AT TWO NODES LEFT */
	} else {
	    *i1 = *n1;
	    *i2 = *n2;
	    *j1 = *n1test;
	    *j2 = *n2test;
	    *kountl = 2;
	}
/*  NODE LOOP FOR AN EVEN NUMBER OF SPLITS - THE MATCH IS */
/*  NOT FINE, SO A SHIFT ONE WAY OR THE OTHER IS NEEDED */
    } else if (*kount2 % 2 == 0) {
	*i1 = *n1;
	*i2 = *n2;
	xi = xn[*i2] - xn[*i1];
	yi = yn[*i2] - yn[*i1];
	xjf = xn[*n2test] - xn[*n3test];
	yjf = yn[*n2test] - yn[*n3test];
	xjb = xn[*n0test] - xn[*n1test];
	yjb = yn[*n0test] - yn[*n1test];
	fdot = (xi * xjf + yi * yjf) / (sqrt(xi * xi + yi * yi) * sqrt(xjf * 
		xjf + yjf * yjf));
/* Computing 2nd power */
	r__1 = xn[*n2test] - xn[*i2];
/* Computing 2nd power */
	r__2 = yn[*n2test] - yn[*i2];
	d1f = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xn[*n3test] - xn[*i1];
/* Computing 2nd power */
	r__2 = yn[*n3test] - yn[*i1];
	d2f = sqrt(r__1 * r__1 + r__2 * r__2);
	df = (d1f + d2f) * (float).5;
	bdot = (xi * xjb + yi * yjb) / (sqrt(xi * xi + yi * yi) * sqrt(xjb * 
		xjb + yjb * yjb));
/* Computing 2nd power */
	r__1 = xn[*n0test] - xn[*i2];
/* Computing 2nd power */
	r__2 = yn[*n0test] - yn[*i2];
	d1b = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xn[*n1test] - xn[*i1];
/* Computing 2nd power */
	r__2 = yn[*n1test] - yn[*i1];
	d2b = sqrt(r__1 * r__1 + r__2 * r__2);
	db = (d1b + d2b) * (float).5;
/*  NOW COMPARE A FORWARD OR BACKWARD SHIFT AND PICK THE MOST */
/*  APPROPRIATE ONE BASED ON ANGLE COSINE AND END DISTANCES */
/*  IF ANY STICK OUT AS THE MOST APPROPRIATE */
	if (fdot > bdot && df <= db) {
	    *j1 = *n2test;
	    *j2 = *n3test;
	    *kountl = *kount2;
	} else if (bdot > fdot && db <= df && *kount2 > 4) {
	    *j1 = *n0test;
	    *j2 = *n1test;
	    *kountl = *kount2 - 2;
	} else if ((r__3 = (r__1 = acos(bdot), dabs(r__1)) - (r__2 = acos(
		fdot), dabs(r__2)), dabs(r__3)) <= (float).3490659) {
	    if (df <= db || *kount2 <= 4) {
		*j1 = *n2test;
		*j2 = *n3test;
		*kountl = *kount2;
	    } else {
		*j1 = *n0test;
		*j2 = *n1test;
		*kountl = *kount2 - 2;
	    }
/*  NONE STICK OUT AS THE OVIOUS WINNER - TAKE ONE BASED ON */
/*  INTERSECTION PORTIONS */
	} else {
	    if (*u < (float).5) {
		if (*w < (float).5 && *kount2 > 4) {
		    *j1 = *n0test;
		    *j2 = *n1test;
		    *kountl = *kount2 - 2;
		} else {
		    *j1 = *n2test;
		    *j2 = *n3test;
		    *kountl = *kount2;
		}
	    } else {
		if (*w < (float).5 && *kount2 > 4) {
		    *j1 = *n0test;
		    *j2 = *n1test;
		    *kountl = *kount2 - 2;
		} else {
		    *j1 = *n2test;
		    *j2 = *n3test;
		    *kountl = *kount2;
		}
	    }
	}
/*  NODE LOOP FOR AN ODD NUMBER OF SPLITS - THE MATCH IS FINE */
    } else {
	*i1 = *n1;
	*i2 = *n2;
	*j1 = *n1test;
	*j2 = *n2test;
	*kountl = *kount2 - 1;
    }
/*  NOW THAT THE INITIAL MATCH IS MADE, CHECK MOVING BOTH SIDES */
/*  FORWARD OR BACKWARD ONE NOTCH AND SEE IF THAT MATCH MAKES MORE SENSE */
/*  THEN THE CURRENT MATCH */
    ifor1 = *i2;
    ifor2 = lnodes[*i2 * lnodes_dim1 + 3];
    ibac1 = lnodes[*i1 * lnodes_dim1 + 2];
    ibac2 = *i1;
    jfor1 = *j1;
    jfor2 = lnodes[*j1 * lnodes_dim1 + 2];
    jbac1 = lnodes[*j2 * lnodes_dim1 + 3];
    jbac2 = *j2;
/*  NOW CALCULATE THE CROSS PRODUCT AND END DISTANCES */
    xif = xn[ifor2] - xn[ifor1];
    yif = yn[ifor2] - yn[ifor1];
    xjf = xn[jfor2] - xn[jfor1];
    yjf = yn[jfor2] - yn[jfor1];
    fdot = (xif * xjf + yif * yjf) / (sqrt(xif * xif + yif * yif) * sqrt(xjf *
	     xjf + yjf * yjf));
/* Computing 2nd power */
    r__1 = xn[ifor1] - xn[jfor1];
/* Computing 2nd power */
    r__2 = yn[ifor1] - yn[jfor1];
    d1f = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = xn[ifor2] - xn[jfor2];
/* Computing 2nd power */
    r__2 = yn[ifor2] - yn[jfor2];
    d2f = sqrt(r__1 * r__1 + r__2 * r__2);
    df = (d1f + d2f) * (float).5;
    xib = xn[ibac2] - xn[ibac1];
    yib = yn[ibac2] - yn[ibac1];
    xjb = xn[jbac2] - xn[jbac1];
    yjb = yn[jbac2] - yn[jbac1];
    bdot = (xib * xjb + yib * yjb) / (sqrt(xib * xib + yib * yib) * sqrt(xjb *
	     xjb + yjb * yjb));
/* Computing 2nd power */
    r__1 = xn[ibac1] - xn[jbac1];
/* Computing 2nd power */
    r__2 = yn[ibac1] - yn[jbac1];
    d1b = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = xn[ibac2] - xn[jbac2];
/* Computing 2nd power */
    r__2 = yn[ibac2] - yn[jbac2];
    d2b = sqrt(r__1 * r__1 + r__2 * r__2);
    db = (d1b + d2b) * (float).5;
    xi = xn[*i2] - xn[*i1];
    yi = yn[*i2] - yn[*i1];
    xj = xn[*j1] - xn[*j2];
    yj = yn[*j1] - yn[*j2];
    dot = (xi * xj + yi * yj) / (sqrt(xi * xi + yi * yi) * sqrt(xj * xj + yj *
	     yj));
/* Computing 2nd power */
    r__1 = xn[*i1] - xn[*j2];
/* Computing 2nd power */
    r__2 = yn[*i1] - yn[*j2];
    d1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = xn[*i2] - xn[*j1];
/* Computing 2nd power */
    r__2 = yn[*i2] - yn[*j1];
    d2 = sqrt(r__1 * r__1 + r__2 * r__2);
    d0 = (d1 + d2) * (float).5;
/*  NOW COMPARE TO SEE IF ANOTHER COMBINATION MAKES BETTER SENSE */
    if ((fdot > dot && df < d0 || fdot * (float).6 > dot && df * (float).7 < 
	    d0 || fdot * (float).2 > dot && df * (float).5 < d0) && *kountl > 
	    4) {
	fwins = TRUE_;
	d0 = df;
	dot = fdot;
    }
    if (bdot > dot && db < d0 || bdot * (float).6 > dot && db * (float).7 < 
	    d0 || bdot * (float).2 > dot && db * (float).5 < d0 && *nloop - *
	    kountl - 2 > 4) {
	bwins = TRUE_;
    }
    if (bwins) {
	*i1 = ibac1;
	*i2 = ibac2;
	*j1 = jbac2;
	*j2 = jbac1;
	*kountl += 2;
    } else if (fwins) {
	*i1 = ifor1;
	*i2 = ifor2;
	*j1 = jfor2;
	*j2 = jfor1;
	*kountl += -2;
    }
/*  NOW CHECK THAT TWO BOUNDARY LINES OR LINES CONNECTED TO THE */
/*  BOUNDARY ARE NOT BEING JOINED INAPPROPRIATELY */
    if (matchk_(mxnd, i1, i2, j1, j2, &lxn[5])) {
/*  TRY THE CURRENT I'S AND J'2 REVERSED */
    } else if (matchk_(mxnd, j1, j2, i1, i2, &lxn[5])) {
	i1hold = *i1;
	i2hold = *i2;
	*i1 = *j1;
	*i2 = *j2;
	*j1 = i1hold;
	*j2 = i2hold;
	*kountl = *nloop - *kountl - 2;
    } else {
/*  TRY ONE STEP FORWARD AND BACKWARDS (NORMAL AND I'S AND J'S REVERSED) */
	ifor1 = *i2;
	ifor2 = lnodes[*i2 * lnodes_dim1 + 3];
	jfor1 = *j1;
	jfor2 = lnodes[*j1 * lnodes_dim1 + 2];
	ibac1 = lnodes[*i1 * lnodes_dim1 + 2];
	ibac2 = *i1;
	jbac1 = lnodes[*j2 * lnodes_dim1 + 3];
	jbac2 = *j2;
	if (matchk_(mxnd, &ifor1, &ifor2, &jfor2, &jfor1, &lxn[5])) {
	    *i1 = ifor1;
	    *i2 = ifor2;
	    *j1 = jfor2;
	    *j2 = jfor1;
	    *kountl += -2;
	} else if (matchk_(mxnd, &jfor2, &jfor1, &ifor1, &ifor2, &lxn[5])) {
	    *i1 = jfor2;
	    *i2 = jfor1;
	    *j1 = ifor1;
	    *j2 = ifor2;
	    *kountl = *nloop - *kountl;
	} else if (matchk_(mxnd, &ibac1, &ibac2, &jbac2, &jbac1, &lxn[5])) {
	    *i1 = ibac1;
	    *i2 = ibac2;
	    *j1 = jbac2;
	    *j2 = jbac1;
	    *kountl += 2;
	} else if (matchk_(mxnd, &jbac2, &jbac1, &ibac1, &ibac2, &lxn[5])) {
	    *i1 = jbac2;
	    *i2 = jbac1;
	    *j1 = ibac1;
	    *j2 = ibac2;
	    *kountl = *nloop - *kountl - 4;
	} else {
	    *err = TRUE_;
	    goto L100;
	}
    }
L100:
    return 0;
} /* match2_ */

#ifdef __cplusplus
	}
#endif

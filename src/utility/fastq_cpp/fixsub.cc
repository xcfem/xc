/* fixsub.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int fixsub_(integer *mxnd, integer *nnnold, integer *nnn, 
	integer *lllold, integer *lll, integer *kkkold, integer *kkk, real *
	xn, real *yn, integer *nuid, integer *lxk, integer *kxl, integer *nxl,
	 integer *lxn, integer *indx, integer *iavail, integer *navail, 
	logical *final)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j, k, l;
    extern /* Subroutine */ int indexi_fq__(integer *, integer *, integer *, 
	    integer *);
    static integer n1, n2, ii, jj;
    static logical err;
    static integer kend, iold, lold, nper, kelem, lines[20];
    static logical found;
    extern /* Subroutine */ int addlxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *), dellxn_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, logical *),
	     lowfnd_(integer *, integer *, integer *, integer *, integer *, 
	    integer *), getlxn_(integer *, integer *, integer *, integer *, 
	    integer *, logical *);
    static logical noroom;

/* *********************************************************************** */
/*  SUBROUTINE FIXSUB = FIXES THE KXL, LXK, NXL, AND LXN ARRAYS FOR */
/*                      SUBREGIONS - TAKES OUT DUPLICATE LINES AND NODES */
/* *********************************************************************** */
/*  GENERATE A LIST OF NODES ON THE PERIMETER IN SORTED ORDER */
    /* Parameter adjustments */
    --indx;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;

    /* Function Body */
    nper = 0;
    i__1 = *nnnold;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nuid[i__] != 0) {
	    ++nper;
	    indx[nper] = i__;
	}
/* L100: */
    }
    indexi_fq__(nnnold, &nuid[1], &nper, &indx[1]);
/*  GO THROUGH ALL THE BOUNDARY NODES IN THE LIST CHECKING FOR DUPLICATES */
    i__ = *nnnold + 1;
L110:
    if (nuid[i__] != 0) {
/*  SEE IF ANOTHER NODE EXISTS WITH THE SAME NUID */
	lowfnd_(mxnd, &nuid[1], &nper, &indx[1], &i__, &iold);
/*  IF ANOTHER NODE EXISTS, THEN START CHECKING LINES */
	if (iold > 0) {
	    getlxn_(mxnd, &lxn[5], &iold, lines, &kend, &err);
/*  CHECK ALL THE LINES ATTACHED TO THE OLD NODE, TO SEE IF THEY ARE */
/*  THE SAME LINE ATTACHED TO THE NODE BEING CHECKED. */
/*  IF THE SAME LINE EXISTS, DELETE THE LINE, AND MARK THE NODE FOR */
/*  LATER DELETION */
	    for (j = 1; j <= 3; ++j) {
		if (lxn[j + (i__ << 2)] != 0) {
		    l = (i__1 = lxn[j + (i__ << 2)], abs(i__1));
		    n1 = nxl[(l << 1) + 1] + nxl[(l << 1) + 2] - i__;
		    found = FALSE_;
		    i__1 = kend;
		    for (k = 1; k <= i__1; ++k) {
			lold = lines[k - 1];
			n2 = nxl[(lold << 1) + 1] + nxl[(lold << 1) + 2] - 
				iold;
			if (nuid[n2] == nuid[n1] && nuid[n2] != 0) {
/*  THE SAME LINE HAS BEEN FOUND - CHANGE REFERENCES TO THE LATEST */
/*  NODE TO REFERENCES TO THE OLD NODE */
			    kxl[(lold << 1) + 2] = kxl[(l << 1) + 1];
			    kelem = kxl[(l << 1) + 1];
			    for (ii = 1; ii <= 4; ++ii) {
				if (lxk[ii + (kelem << 2)] == l) {
				    lxk[ii + (kelem << 2)] = lold;
				}
/* L120: */
			    }
			    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &
				    n1, &l, nnn, &err, &noroom);
/*  NOW RENUMBER THE REMAINING LINES IN THE KXL AND NXL ARRAYS */
			    --(*lll);
			    i__2 = *lll;
			    for (ii = l; ii <= i__2; ++ii) {
				kxl[(ii << 1) + 1] = kxl[(ii + 1 << 1) + 1];
				kxl[(ii << 1) + 2] = kxl[(ii + 1 << 1) + 2];
				nxl[(ii << 1) + 1] = nxl[(ii + 1 << 1) + 1];
				nxl[(ii << 1) + 2] = nxl[(ii + 1 << 1) + 2];
/* L130: */
			    }
			    kxl[(*lll + 1 << 1) + 1] = 0;
			    kxl[(*lll + 1 << 1) + 2] = 0;
			    nxl[(*lll + 1 << 1) + 1] = 0;
			    nxl[(*lll + 1 << 1) + 2] = 0;
/*  NOW RENUMBER ANY REFERENCES TO LINES ABOVE L IN THE LXK AND */
/*  THE LXN ARRAYS */
			    i__2 = *nnn;
			    for (ii = 1; ii <= i__2; ++ii) {
				for (jj = 1; jj <= 3; ++jj) {
				    if ((i__3 = lxn[jj + (ii << 2)], abs(i__3)
					    ) == l) {
					lxn[jj + (ii << 2)] = lold;
				    } else if ((i__3 = lxn[jj + (ii << 2)], 
					    abs(i__3)) > l) {
					lxn[jj + (ii << 2)] = (i__3 = lxn[jj 
						+ (ii << 2)], abs(i__3)) - 1;
				    }
/* L140: */
				}
				if (lxn[(ii << 2) + 4] == l) {
				    lxn[(ii << 2) + 4] = lold;
				} else if (lxn[(ii << 2) + 4] > l) {
				    --lxn[(ii << 2) + 4];
				}
/* L150: */
			    }
			    i__2 = *kkk;
			    for (ii = *kkkold + 1; ii <= i__2; ++ii) {
				for (jj = 1; jj <= 4; ++jj) {
				    if (lxk[jj + (ii << 2)] == l) {
					lxk[jj + (ii << 2)] = lold;
				    } else if (lxk[jj + (ii << 2)] > l) {
					--lxk[jj + (ii << 2)];
				    }
/* L160: */
				}
/* L170: */
			    }
			    found = TRUE_;
			    goto L190;
			}
/* L180: */
		    }
/*  END OF CHECK FOR THE SAME LINE - JUST ADD THE LINE TO THE IOLD NODE */
/*  IF THERE IS A PLACE FOR THE LINE (I.E. THE MAXIMUM IS FOUR/NODE WITH */
/*  THIS SCHEME). */
L190:
		    if (! found) {
			addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &
				iold, &l, nnn, &err, &noroom);
			if (nxl[(l << 1) + 1] == i__) {
			    nxl[(l << 1) + 1] = iold;
			} else {
			    nxl[(l << 1) + 2] = iold;
			}
		    }
		}
/* L200: */
	    }
/*  ALL THE OLD LINES HAVE BEEN GONE THROUGH - NOW DELETE THE NODE I */
	    i__1 = i__ - 1;
	    for (j = 1; j <= i__1; ++j) {
		if (lxn[(j << 2) + 4] < -i__) {
		    ++lxn[(j << 2) + 4];
		}
/* L210: */
	    }
	    --(*nnn);
	    i__1 = *nnn;
	    for (j = i__; j <= i__1; ++j) {
		for (k = 1; k <= 3; ++k) {
		    lxn[k + (j << 2)] = lxn[k + (j + 1 << 2)];
/* L220: */
		}
		if (lxn[(j + 1 << 2) + 4] < 0) {
		    lxn[(j << 2) + 4] = lxn[(j + 1 << 2) + 4] + 1;
		} else {
		    lxn[(j << 2) + 4] = lxn[(j + 1 << 2) + 4];
		}
		xn[j] = xn[j + 1];
		yn[j] = yn[j + 1];
		nuid[j] = nuid[j + 1];
/* L230: */
	    }
	    lxn[(*nnn + 1 << 2) + 1] = 0;
	    lxn[(*nnn + 1 << 2) + 2] = 0;
	    lxn[(*nnn + 1 << 2) + 3] = 0;
	    lxn[(*nnn + 1 << 2) + 4] = 0;
	    nuid[*nnn + 1] = 0;
	    i__1 = *lll;
	    for (j = *lllold + 1; j <= i__1; ++j) {
		if (nxl[(j << 1) + 1] >= i__) {
		    --nxl[(j << 1) + 1];
		}
		if (nxl[(j << 1) + 2] >= i__) {
		    --nxl[(j << 1) + 2];
		}
/* L240: */
	    }
	} else {
	    ++i__;
	}
    } else {
	++i__;
    }
    if (i__ <= *nnn) {
	goto L110;
    }
/*  IF THIS IS THE FINAL SUBREGION TO BE ADDED, THEN FLAG */
/*  THE LXN ARRAY FOR TRULY EXTERIOR NODES, AND CLEAR THE TEMPORARY */
/*  NUID'S OF THE SUBREGION ONLY BOUNDARY NODES */
    if (*final) {
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if ((i__2 = nuid[i__], abs(i__2)) > 1000000000 || (i__3 = nuid[
		    i__], abs(i__3)) < 100000 && nuid[i__] != 0) {
		lxn[(i__ << 2) + 2] = -(i__2 = lxn[(i__ << 2) + 2], abs(i__2))
			;
	    } else {
		nuid[i__] = 0;
		lxn[(i__ << 2) + 2] = (i__2 = lxn[(i__ << 2) + 2], abs(i__2));
	    }
/* L250: */
	}
/*  LINK-UP AVAILABLE LXN SPACE */
	*iavail = *nnn + 1;
	*navail = *mxnd - *nnn;
	i__1 = *mxnd;
	for (i__ = *iavail; i__ <= i__1; ++i__) {
	    lxn[(i__ << 2) + 1] = 0;
	    lxn[(i__ << 2) + 2] = 0;
	    lxn[(i__ << 2) + 3] = 0;
	    lxn[(i__ << 2) + 4] = i__ + 1;
/* L260: */
	}
    }
    return 0;
} /* fixsub_ */

#ifdef __cplusplus
	}
#endif

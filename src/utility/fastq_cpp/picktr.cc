/* picktr.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int picktr_(integer *nper, real *x, real *y, integer *nid, 
	real *angle, logical *halfc, integer *i1, integer *i2, integer *i3, 
	integer *i4, integer *i5, integer *i6, integer *i7, integer *i8)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5, i__6, i__7;
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal);

    /* Local variables */
    static integer i__, j, k, m1, m2, m3, m4, n1, n2, n3;
    extern /* Subroutine */ int fq_rotate__(integer *, real *, real *, 
	    integer *, integer *);
    static integer j6, j7, j8, ii, kk, kl, km;
    static real pi;
    static integer kn, ko, kp, isa, nsa;
    static real pid2, sum1, sum2, sum3, diff, gval, atol;
    static integer mmax;
    static real temp;
    static integer itmp, next;
    static real agold;
    static integer mbase;
    static real ahold, agnew, gbest, smang[7];
    static integer index[7], mleft, itemp, isort[4];
    static real twopi, theta1, theta2;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static real thetab, thetat;
    static integer ifirst;

/* *********************************************************************** */
/*  SUBROUTINE PICKTR = DETERMINES A REASONABLE SHAPE FOR A BACK-TO-BACK */
/*                      SET OF TRIANGLES (TRANSITION REGION) */
/* *********************************************************************** */
    /* Parameter adjustments */
    --angle;
    --nid;
    --y;
    --x;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    pid2 = pi * (float).5;
    twopi = pi * (float)2.;
/*  FORM THE LIST OF SMALLEST ANGLES */
    nsa = 6;
    i__1 = nsa;
    for (i__ = 1; i__ <= i__1; ++i__) {
	smang[i__ - 1] = (float)10.;
	index[i__ - 1] = 0;
/* L100: */
    }
    agold = atan2(y[1] - y[*nper], x[1] - x[*nper]);
    i__1 = *nper;
    for (j = 1; j <= i__1; ++j) {
/*  GET THE ANGLE FORMED BY THIS SET OF POINTS */
	next = j + 1;
	if (next > *nper) {
	    next = 1;
	}
	agnew = atan2(y[next] - y[j], x[next] - x[j]);
	diff = agnew - agold;
	if (diff > pi) {
	    diff -= twopi;
	}
	if (diff < -pi) {
	    diff += twopi;
	}
	angle[j] = pi - diff;
	agold = agnew;
/*  SORT THIS ANGLE AGAINST PREVIOUS ANGLES TO SEE IF IT IS ONE OF */
/*  THE SMALLEST */
	smang[nsa] = angle[j];
	index[nsa] = j;
	i__2 = nsa;
	for (ii = 1; ii <= i__2; ++ii) {
	    i__ = nsa + 1 - ii;
	    if (smang[i__] >= smang[i__ - 1]) {
		goto L120;
	    }
	    temp = smang[i__ - 1];
	    itemp = index[i__ - 1];
	    smang[i__ - 1] = smang[i__];
	    index[i__ - 1] = index[i__];
	    smang[i__] = temp;
	    index[i__] = itemp;
/* L110: */
	}
L120:
/* L130: */
	;
    }
/*  DETERMINE TWO/FOUR BEST CORNER POINTS FOR SEMICIRCLE/TRANSITION REGION */
    atol = pi * (float)150. / (float)180.;
/*  FIND SIDE DIVISION USING 4 SMALLEST ANGLES AND CHECK CONDITION */
    for (i__ = 1; i__ <= 4; ++i__) {
	isort[i__ - 1] = index[i__ - 1];
/* L140: */
    }
    for (i__ = 1; i__ <= 3; ++i__) {
	for (j = i__ + 1; j <= 4; ++j) {
	    if (isort[i__ - 1] > isort[j - 1]) {
		itmp = isort[i__ - 1];
		isort[i__ - 1] = isort[j - 1];
		isort[j - 1] = itmp;
	    }
/* L150: */
	}
/* L160: */
    }
    *i1 = isort[0];
    *i2 = isort[1];
    *i3 = isort[2];
    *i4 = isort[3];
    m1 = *i2 - *i1;
    if (m1 < 0) {
	m1 = *nper + m1;
    }
    m2 = *i3 - *i2;
    if (m2 < 0) {
	m2 = *nper + m2;
    }
    m3 = *i4 - *i3;
    if (m3 < 0) {
	m3 = *nper + m3;
    }
    m4 = *nper - m1 - m2 - m3;
/*  USE THE LONGEST SIDE THAT DOES NOT HAVE OPPOSITE */
/*  MATCHES AS THE CHOICE FOR THE BASE (OF TRANSITIONS) */
/*  THE BASE MUST BE AT LEAST 4 INTERVALS LONG */
    if (m1 == m3 && ! (*halfc)) {
	mmax = max(m2,m4);
	if (mmax >= 4) {
	    if (m2 == mmax) {
		ifirst = *i2;
		mbase = m2;
	    } else {
		ifirst = *i4;
		mbase = m4;
	    }
	}
    } else if (m2 == m4 && ! (*halfc)) {
	mmax = max(m1,m3);
	if (mmax >= 4) {
	    if (m1 == mmax) {
		ifirst = *i1;
		mbase = m1;
	    } else {
		ifirst = *i3;
		mbase = m3;
	    }
	}
    } else {
/* Computing MAX */
	i__1 = max(m1,m2), i__1 = max(i__1,m3);
	mmax = max(i__1,m4);
	if (mmax >= 4) {
	    if (m1 == mmax) {
		ifirst = *i1;
		mbase = m1;
	    } else if (m2 == mmax) {
		ifirst = *i2;
		mbase = m2;
	    } else if (m3 == mmax) {
		ifirst = *i3;
		mbase = m3;
	    } else if (m4 == mmax) {
		ifirst = *i4;
		mbase = m4;
	    }
	}
    }
    if (mmax >= 4) {
	if (*halfc) {
	    gbest = angle[*i1] + angle[*i2] + (r__1 = pi - angle[*i3], dabs(
		    r__1)) + (r__2 = pi - angle[*i4], dabs(r__2));
	} else {
	    gbest = angle[*i1] + angle[*i2] + angle[*i3] + angle[*i4];
	}
    } else {
	ifirst = 1;
	gbest = (float)1e6;
    }
/*  GO AROUND THE PERIMETER USING THE 6 SMALLEST ANGLES AS POSSIBLE */
/*  STARTING POINTS, AND THEN FIND THE BEST COMBINATION OF SIDE LENGTHS */
    i__1 = nsa;
    for (isa = 1; isa <= i__1; ++isa) {
	if (smang[isa - 1] <= atol) {
	    *i1 = index[isa - 1];
	    sum1 = angle[*i1];
	    if (*halfc) {
		if (sum1 > gbest) {
		    goto L200;
		}
	    } else {
		if (sum1 >= gbest) {
		    goto L200;
		}
	    }
/*  ASSIGN A TRIAL SECOND NODE */
	    i__2 = *nper - 4;
	    for (n1 = 1; n1 <= i__2; ++n1) {
		*i2 = *i1 + n1;
		if (*i2 > *nper) {
		    *i2 -= *nper;
		}
		sum2 = sum1 + angle[*i2];
		if (*halfc) {
		    if (sum2 > gbest) {
			goto L190;
		    }
		} else {
		    if (sum2 >= gbest) {
			goto L190;
		    }
		}
/*  ASSIGN A TRIAL THIRD NODE */
		i__3 = *nper - n1 - 3;
		for (n2 = 1; n2 <= i__3; ++n2) {
		    *i3 = *i2 + n2;
		    if (*i3 > *nper) {
			*i3 -= *nper;
		    }
		    if (*halfc) {
			sum3 = sum2 + (r__1 = pi - angle[*i3], dabs(r__1));
		    } else {
			sum3 = sum2 + angle[*i3];
		    }
		    if (*halfc) {
			if (sum3 > gbest) {
			    goto L180;
			}
		    } else {
			if (sum3 >= gbest) {
			    goto L180;
			}
		    }
/*  ASSIGN A TRIAL FOURTH NODE */
		    i__4 = *nper - n1 - n2 - 2;
		    for (n3 = 1; n3 <= i__4; ++n3) {
			*i4 = *i3 + n3;
			if (*i4 > *nper) {
			    *i4 -= *nper;
			}
			if (*halfc) {
			    gval = sum3 + (r__1 = pi - angle[*i4], dabs(r__1))
				    ;
			} else {
			    gval = sum3 + angle[*i4];
			}
			if (*halfc) {
			    if (gval > gbest) {
				goto L170;
			    }
			} else {
			    if (gval >= gbest) {
				goto L170;
			    }
			}
/*  FIND SIDE DIVISION AND CHECK CONDITION */
			m1 = *i2 - *i1;
			if (m1 < 0) {
			    m1 = *nper + m1;
			}
			m2 = *i3 - *i2;
			if (m2 < 0) {
			    m2 = *nper + m2;
			}
			m3 = *i4 - *i3;
			if (m3 < 0) {
			    m3 = *nper + m3;
			}
			m4 = *nper - m1 - m2 - m3;
/*  USE THE LONGEST SIDE THAT DOES NOT HAVE OPPOSITE */
/*  MATCHES AS THE CHOICE FOR THE BASE (OF TRANSITIONS) */
/*  THE BASE MUST BE AT LEAST 4 INTERVALS LONG */
			if (m1 == m3 && ! (*halfc)) {
			    mmax = max(m2,m4);
			    if (mmax >= 4) {
				if (m2 == mmax) {
				    ifirst = *i2;
				    mbase = m2;
				} else {
				    ifirst = *i4;
				    mbase = m4;
				}
			    }
			} else if (m2 == m4 && ! (*halfc)) {
			    mmax = max(m1,m3);
			    if (mmax >= 4) {
				if (m1 == mmax) {
				    ifirst = *i1;
				    mbase = m1;
				} else {
				    ifirst = *i3;
				    mbase = m3;
				}
			    }
			} else {
/* Computing MAX */
			    i__5 = max(m1,m2), i__5 = max(i__5,m3);
			    mmax = max(i__5,m4);
			    if (mmax >= 4) {
				if (m1 == mmax) {
				    ifirst = *i1;
				    mbase = m1;
				} else if (m2 == mmax) {
				    ifirst = *i2;
				    mbase = m2;
				} else if (m3 == mmax) {
				    ifirst = *i3;
				    mbase = m3;
				} else if (m4 == mmax) {
				    ifirst = *i4;
				    mbase = m4;
				}
			    }
			    if (mmax >= 4) {
				gbest = gval;
			    }
			}
L170:
			;
		    }
L180:
		    ;
		}
L190:
		;
	    }
	}
L200:
	;
    }
/*  ROTATE THE PERIMETER AND THE ANGLES SO THE BASE LEADS THE LIST */
    if (ifirst != 1) {
	fq_rotate__(nper, &x[1], &y[1], &nid[1], &ifirst);
    }
    i__1 = ifirst - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ahold = angle[1];
	i__2 = *nper - 1;
	for (j = 1; j <= i__2; ++j) {
	    angle[j] = angle[j + 1];
/* L210: */
	}
	angle[*nper] = ahold;
/* L220: */
    }
/*  DECIDE THE TRIANGLE CORNERS */
    gbest = (float)1e6;
/*  PICK AN ARBITRARY BASE CENTER (I3) */
    i__1 = mbase - 1;
    for (i__ = 3; i__ <= i__1; ++i__) {
/*  FOR THIS BASE CENTER, PICK AN ARBITRARY I2 LOCATION */
	i__2 = i__ - 1;
	for (j = 2; j <= i__2; ++j) {
/*  FOR THIS COMBINATION OF I3 AND I2, PICK AN ARBITRARY I4 LOCATION */
	    i__3 = mbase;
	    for (k = i__ + 1; k <= i__3; ++k) {
/*  CALCULATE I6 AND I8 AND ADD ANGLES TO FIND MINIMUM SUM */
		kn = mbase + 1 - k;
		kk = i__ - j;
		kl = j - 1;
		km = k - i__;
		mleft = *nper - mbase;
		ko = (mleft - kn + kl - kk - km) / 2;
		kp = kn + ko - kl;
/*  PROTECT AGAINST THE IMPOSSIBLE LENGTH PROBLEMS */
/*  AND THE ODD NUMBER IN THE PERIMETER INPUT ERRORS */
		if (ko > 0 && kp > 0) {
		    if (kp + kl == kn + ko) {
/*  NOW GET THE END POINTS GIVEN THESE SIDE LENGTHS */
			j6 = mbase + 1 + ko;
			j7 = mbase + 1 + ko + km;
			j8 = mbase + 1 + ko + km + kk;
/*  GET THE BASE ANGLE OF THE DIVIDER LINE */
			theta1 = atan2(y[i__ + 1] - y[i__], x[i__ + 1] - x[
				i__]);
			theta2 = atan2(y[j7] - y[i__], x[j7] - x[i__]);
			thetab = (r__1 = theta2 - theta1, dabs(r__1));
			if (thetab > pi) {
			    thetab -= pi;
			}
			if (thetab < pid2) {
			    thetab = pi - thetab;
			}
/*  GET THE TOP ANGLE OF THE DIVIDER LINE */
			theta1 = atan2(y[j7 + 1] - y[j7], x[j7 + 1] - x[j7]);
			thetat = (r__1 = theta2 - theta1, dabs(r__1));
			if (thetat > pi) {
			    thetat -= pi;
			}
			if (thetat < pid2) {
			    thetat = pi - thetat;
			}
/*  ADD THESE TO GET THE VALUE OF GVAL */
			if (*halfc) {
/* Computing MAX */
			    i__6 = (i__4 = kk - kl, abs(i__4)), i__7 = (i__5 =
				     km - kn, abs(i__5));
			    gval = thetab + thetat + (r__1 = pi - angle[j6], 
				    dabs(r__1)) + (r__2 = pi - angle[j8], 
				    dabs(r__2)) + max(i__6,i__7) * (float).1 /
				     *nper;
			} else {
			    gval = angle[j6] + angle[j8] + thetab + thetat;
			}
			if (gval < gbest) {
			    gbest = gval;
			    *i1 = 1;
			    *i2 = j;
			    *i3 = i__;
			    *i4 = k;
			    *i5 = mbase + 1;
			    *i6 = *i5 + ko;
			    *i7 = *i6 + km;
			    *i8 = *i7 + kk;
			}
		    } else {
			mesage_("ODD PERIMETER PROBLEMS", (ftnlen)22);
		    }
		}
/* L230: */
	    }
/* L240: */
	}
/* L250: */
    }
    return 0;
} /* picktr_ */

#ifdef __cplusplus
	}
#endif

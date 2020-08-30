/* dataok.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int dataok_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *l, integer *knum, real *coor, integer *iline, integer *
	ltype, integer *knint, integer *lcon, integer *nlps, integer *ifline, 
	integer *illist, integer *nspr, integer *ifside, integer *islist, 
	integer *linkp, integer *linkl, integer *links, real *size, logical *
	errchk, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 FOR REGION:\002,i5,\002 SIDE:\002,i5\
,\002 DOES NOT EXIST\002)";
    static char fmt_10010[] = "(\002 FOR SIDE:\002,i5,\002 LINE:\002,i5,\002\
 DOES NOT EXIST\002)";
    static char fmt_10030[] = "(\002 FOR LINE:\002,i5,\002 POINT:\002,i5,\
\002 DOES NOT EXIST\002)";
    static char fmt_10020[] = "(\002 FOR LINE:\002,i5,\002 INTERVAL OF:\002,\
i5,\002 IS NOT WORKING\002)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(), 
	    i_nint(real *);

    /* Local variables */
    static integer i__, j, i1, i2, i3, j1, j2, j3, ii, jj, kk, ll;
    static real dist;
    static integer ipntr;
    static logical addlnk;
    extern /* Subroutine */ int linlen_(integer *, real *, integer *, integer 
	    *, integer *, integer *, integer *, integer *, integer *, integer 
	    *, real *, logical *), ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___10 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___17 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___18 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___19 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___21 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___22 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___23 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___24 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___25 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___26 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___27 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE FILLOK = CHECKS TO MAKE SURE NONEXISTENT DATA IS NOT */
/*                      BEING REFERENCED IN THE REGION DEFINITIONS */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    linkl -= 3;
    lcon -= 4;
    --knint;
    --ltype;
    --iline;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --islist;
    --ifside;
    --nspr;

    /* Function Body */
    *err = TRUE_;
    addlnk = FALSE_;
    i__1 = ifside[*l] + nspr[*l] - 1;
    for (i__ = ifside[*l]; i__ <= i__1; ++i__) {
/*  CHECK TO MAKE SURE REGION'S SIDE DEFINITIONS ARE ALL THERE */
	if (islist[i__] > 0) {
	    ii = islist[i__];
	    ltsort_(ms, &links[3], &ii, &ipntr, &addlnk);
	    if (ipntr <= 0) {
		if (*errchk) {
		    s_wsfe(&io___5);
		    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&ii, (ftnlen)sizeof(integer));
		    e_wsfe();
		    return 0;
		} else {
		    goto L120;
		}
	    }
/*  CHECK TO MAKE SURE SIDE'S LINE DEFINITIONS ARE ALL THERE */
	    ltsort_(ms, &links[3], &ii, &jj, &addlnk);
	    i__2 = ifline[jj] + nlps[jj] - 1;
	    for (j = ifline[jj]; j <= i__2; ++j) {
		kk = illist[j];
		ltsort_(ml, &linkl[3], &kk, &ll, &addlnk);
		if (kk <= 0 || ll <= 0) {
		    if (*errchk) {
			s_wsfe(&io___10);
			do_fio(&c__1, (char *)&ii, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
			e_wsfe();
			return 0;
		    } else {
			goto L100;
		    }
		}
/*  CHECK TO MAKE SURE LINE'S POINT DEFINITIONS ARE ALL THERE */
		i1 = lcon[ll * 3 + 1];
		i2 = lcon[ll * 3 + 2];
		i3 = lcon[ll * 3 + 3];
		ltsort_(mp, &linkp[3], &i1, &j1, &addlnk);
		ltsort_(mp, &linkp[3], &i2, &j2, &addlnk);
		if (i3 != 0) {
		    i__3 = abs(i3);
		    ltsort_(mp, &linkp[3], &i__3, &j3, &addlnk);
		} else {
		    j3 = 0;
		}
		if (i1 <= 0 || j1 <= 0) {
		    if (*errchk) {
			s_wsfe(&io___17);
			do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
			e_wsfe();
			return 0;
		    } else {
			goto L100;
		    }
		} else if (i2 <= 0 || j2 <= 0) {
		    if (*errchk) {
			s_wsfe(&io___18);
			do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&i2, (ftnlen)sizeof(integer));
			e_wsfe();
			return 0;
		    } else {
			goto L100;
		    }
		} else if (ltype[ll] != 1 && (i3 == 0 || j3 <= 0)) {
		    if (*errchk) {
			s_wsfe(&io___19);
			do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&i3, (ftnlen)sizeof(integer));
			e_wsfe();
			return 0;
		    } else {
			goto L100;
		    }
		}
/*  CHECK TO INSURE AN INTEGRAL ASSIGNMENT */
		if ((i__3 = knint[ll], abs(i__3)) == 0) {
		    if (i3 < 0) {
			j3 = -j3;
		    }
		    linlen_(mp, &coor[3], &linkp[3], knum, &iline[ll], &ltype[
			    ll], &i3, &j1, &j2, &j3, &dist, err);
		    if (*err) {
			if (*errchk) {
			    s_wsfe(&io___21);
			    do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer)
				    );
			    i__4 = (i__3 = knint[ll], abs(i__3));
			    do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    return 0;
			} else {
			    goto L100;
			}
		    } else {
			if (*size <= (float)0.) {
			    knint[ll] = 1;
			} else {
/* Computing MAX */
			    r__1 = dist / *size;
			    i__3 = i_nint(&r__1);
			    knint[ll] = max(i__3,1);
			}
		    }
		}
L100:
/* L110: */
		;
	    }
/*  CHECK TO MAKE SURE REGION'S LINE DEFINITIONS ARE ALL THERE */
	} else if (islist[i__] < 0) {
	    kk = (i__2 = islist[i__], abs(i__2));
	    ltsort_(ml, &linkl[3], &kk, &ll, &addlnk);
	    if (kk <= 0 || ll <= 0) {
		if (*errchk) {
		    s_wsfe(&io___22);
		    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		    e_wsfe();
		    return 0;
		} else {
		    goto L120;
		}
	    }
/*  CHECK TO MAKE SURE LINE'S POINT DEFINITIONS ARE ALL THERE */
	    i1 = lcon[ll * 3 + 1];
	    i2 = lcon[ll * 3 + 2];
	    i3 = lcon[ll * 3 + 3];
	    ltsort_(mp, &linkp[3], &i1, &j1, &addlnk);
	    ltsort_(mp, &linkp[3], &i2, &j2, &addlnk);
	    if (i3 != 0) {
		i__2 = abs(i3);
		ltsort_(mp, &linkp[3], &i__2, &j3, &addlnk);
	    } else {
		j3 = 0;
	    }
	    if (i1 <= 0 || j1 <= 0) {
		if (*errchk) {
		    s_wsfe(&io___23);
		    do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
		    e_wsfe();
		    return 0;
		} else {
		    goto L120;
		}
	    } else if (i2 <= 0 || j2 <= 0) {
		if (*errchk) {
		    s_wsfe(&io___24);
		    do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&i2, (ftnlen)sizeof(integer));
		    e_wsfe();
		    return 0;
		} else {
		    goto L120;
		}
	    } else if (ltype[ll] != 1 && (i3 == 0 || j3 <= 0)) {
		if (*errchk) {
		    s_wsfe(&io___25);
		    do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&i3, (ftnlen)sizeof(integer));
		    e_wsfe();
		    return 0;
		} else {
		    goto L120;
		}
	    }
/*  CHECK TO MAKE SURE INTERVAL ASSIGNMENT IS HANDLED */
	    if ((i__2 = knint[ll], abs(i__2)) == 0) {
/* **MBS/29-JUN-1989/ DO NOT NEGATE POINTER TO CENTER OF CLOCKWISE ARC */
/*              IF (I3 .LT. 0)J3 = -J3 */
		linlen_(mp, &coor[3], &linkp[3], knum, &iline[ll], &ltype[ll],
			 &i3, &j1, &j2, &j3, &dist, err);
		if (*err) {
		    if (*errchk) {
			s_wsfe(&io___26);
			do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
			i__3 = (i__2 = knint[ll], abs(i__2));
			do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
			e_wsfe();
			return 0;
		    } else {
			goto L120;
		    }
		} else {
		    if (*size <= (float)0.) {
			knint[ll] = 1;
		    } else {
/* Computing MAX */
			r__1 = dist / *size;
			i__2 = i_nint(&r__1);
			knint[ll] = max(i__2,1);
		    }
		}
	    }
/*  A ZERO SIDE NUMBER HAS BEEN FOUND */
	} else {
	    if (*errchk) {
		s_wsfe(&io___27);
		do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&islist[i__], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		goto L120;
	    }
	}
L120:
/* L130: */
	;
    }
/*  ALL DEFINITIONS ARE IN ORDER */
    *err = FALSE_;
    return 0;
} /* dataok_ */

#ifdef __cplusplus
	}
#endif

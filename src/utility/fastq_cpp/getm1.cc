/* getm1.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getm1_(integer *ml, integer *ms, integer *mnnps, integer 
	*ns, integer *islist, integer *nint, integer *ifline, integer *nlps, 
	integer *illist, integer *linkl, integer *links, real *x, real *y, 
	integer *nid, integer *nnps, real *angle, integer *nper, char *schstr,
	 integer *m1, logical *ccw, logical *norm, logical *real__, logical *
	err, ftnlen schstr_len)
{
    static integer j;
    extern /* Subroutine */ int fq_rotate__(integer *, real *, real *, 
	    integer *, integer *), nps_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *), pickm1_(integer *, 
	    real *, real *, real *, integer *, integer *, logical *), irever_(
	    integer *, integer *);
    static integer ifirst;

/* *********************************************************************** */
/*  SUBROUTINE GETM1 = GETS THE APPROPRIATE M1 VALUE */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     QMESH = GENERATES QUAD ELEMENTS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NNPS  = ARRAY OF NUMBER OF NODES PER SIDE */
/*     CCW   = .TRUE. IF THE SIDE IS ORIENTED CCW */
/*     NORM  = .TRUE. IF THE FIRST SIDE IS TO BE TRIED AS THE BASE */
/* *********************************************************************** */
/*  SEE IF LETTER M OCCURS IN THE SCHEME (NOT THE "NORMAL" CASE) */
    /* Parameter adjustments */
    linkl -= 3;
    --nint;
    --links;
    --illist;
    --nlps;
    --ifline;
    --nnps;
    --islist;
    --angle;
    --nid;
    --y;
    --x;

    /* Function Body */
    *norm = TRUE_;
    for (j = 1; j <= 72; ++j) {
	if (*(unsigned char *)&schstr[j - 1] == 'M' || *(unsigned char *)&
		schstr[j - 1] == 'm') {
	    *norm = FALSE_;
	    goto L110;
	}
/* L100: */
    }
L110:
/*  CALCULATE THE NUMBER OF NODES PER SIDE */
    nps_(ml, ms, mnnps, ns, &islist[1], &nint[1], &ifline[1], &nlps[1], &
	    illist[1], &linkl[3], &links[1], &nnps[1], err);
    if (*err) {
	return 0;
    }
/*  NORMAL CASE - TRY THE FIRST SIDE IN THE SIDE LIST AS THE M1 BASE */
/*  IN CCW ORDER IF IT IS NOT RIDICULOUS */
    if (*norm) {
	*m1 = nnps[1] - 1;
	if (! (*ccw)) {
	    *m1 = *nper / 2 - *m1;
	}
	if (*m1 > 0 && *m1 <= *nper / 2) {
	    return 0;
	}
    }
    if (! (*ccw)) {
	irever_(&nnps[1], ns);
    }
/*  IF THE BOUNDARY IS A LOGICAL RECTANGLE,  USE IT */
    if (*ns == 4 && nnps[1] == nnps[3] && nnps[2] == nnps[4]) {
	*m1 = nnps[1] - 1;
/*  OTHERWISE,  FIND AN INITIAL M1 FOR A NON-LOGICAL RECTANGLE */
    } else {
	pickm1_(nper, &x[1], &y[1], &angle[1], m1, &ifirst, real__);
	if (ifirst != 1) {
	    fq_rotate__(nper, &x[1], &y[1], &nid[1], &ifirst);
	}
    }
    return 0;
} /* getm1_ */

#ifdef __cplusplus
	}
#endif

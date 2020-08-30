/* gettrn.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int gettrn_(integer *ml, integer *ms, integer *mnnps, 
	integer *ns, integer *islist, integer *nint, integer *ifline, integer 
	*nlps, integer *illist, integer *linkl, integer *links, real *x, real 
	*y, integer *nid, integer *nnps, real *angle, integer *nper, integer *
	i1, integer *i2, integer *i3, integer *i4, integer *i5, integer *i6, 
	integer *i7, integer *i8, real *xcen1, real *ycen1, real *xcen2, real 
	*ycen2, real *xmid1, real *ymid1, real *xmid2, real *ymid2, logical *
	ccw, logical *halfc, logical *err)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real d1, d2, d3, x1, y1, x2, y2, x3, y3, d1a, d1b, d2a, d2b, d3a, 
	    d3b;
    static integer int__;
    extern /* Subroutine */ int nps_(integer *, integer *, integer *, integer 
	    *, integer *, integer *, integer *, integer *, integer *, integer 
	    *, integer *, integer *, logical *);
    static real pro1, pro2, pro3, prop;
    extern /* Subroutine */ int irever_(integer *, integer *), picktr_(
	    integer *, real *, real *, integer *, real *, logical *, integer *
	    , integer *, integer *, integer *, integer *, integer *, integer *
	    , integer *);

/* *********************************************************************** */
/*  SUBROUTINE GETTRN = GETS THE APPROPRIATE SIDES,  CORNERS,  AND MIDPOINT */
/*                      VALUES FOR A TRANSITION REGION */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     QMESH = GENERATES QUAD ELEMENTS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NNPS  = ARRAY OF NUMBER OF NODES PER SIDE */
/*     CCW   = .TRUE. IF THE SIDE IS ORIENTED CCW */
/*     NORM  = .TRUE. IF THE FIRST SIDE IS TO BE TRIED AS THE BASE */
/* *********************************************************************** */
/*  CALCULATE THE NUMBER OF NODES PER SIDE */
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
    nps_(ml, ms, mnnps, ns, &islist[1], &nint[1], &ifline[1], &nlps[1], &
	    illist[1], &linkl[3], &links[1], &nnps[1], err);
    if (*err) {
	return 0;
    }
    if (! (*ccw)) {
	irever_(&nnps[1], ns);
    }
/*  FIND THE BEST CORNER NODES IN THE LIST */
    picktr_(nper, &x[1], &y[1], &nid[1], &angle[1], halfc, i1, i2, i3, i4, i5,
	     i6, i7, i8);
/*  DEFINE THE MIDDLE POINT OF BOTH TRIANGLES AS THE AVERAGE */
/*  OF PROPORIONAL DIVISIONS OF SIDE DIVISION POINT TO OPPOSITE */
/*  TRIANGLE CORNER LINES */
/*  FOR THE FIRST TRIANGLE, */
/*  FIND DISTANCES FROM CORNER TO CORNER,  AND CORNERS TO SIDE DIVISIONS */
    int__ = *i6 - *i4;
    prop = (doublereal) (*i6 - *i5) / (doublereal) int__;
    *xmid1 = x[*i3] + prop * (x[*i7] - x[*i3]);
    *ymid1 = y[*i3] + prop * (y[*i7] - y[*i3]);
/* Computing 2nd power */
    r__1 = x[*i5] - x[*i3];
/* Computing 2nd power */
    r__2 = y[*i5] - y[*i3];
    d1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i7] - x[*i5];
/* Computing 2nd power */
    r__2 = y[*i7] - y[*i5];
    d2 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i3] - x[*i7];
/* Computing 2nd power */
    r__2 = y[*i3] - y[*i7];
    d3 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i4] - x[*i3];
/* Computing 2nd power */
    r__2 = y[*i4] - y[*i3];
    d1a = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i5] - x[*i4];
/* Computing 2nd power */
    r__2 = y[*i5] - y[*i4];
    d1b = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i6] - x[*i5];
/* Computing 2nd power */
    r__2 = y[*i6] - y[*i5];
    d2a = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i7] - x[*i6];
/* Computing 2nd power */
    r__2 = y[*i7] - y[*i6];
    d2b = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = *xmid1 - x[*i7];
/* Computing 2nd power */
    r__2 = *ymid1 - y[*i7];
    d3a = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i3] - *xmid1;
/* Computing 2nd power */
    r__2 = y[*i3] - *ymid1;
    d3b = sqrt(r__1 * r__1 + r__2 * r__2);
/*  GET MIDPOINT TRIALS 1,  2,  AND 3 AS PROPORTIONS */
    pro1 = (d3a / d3 + d1b / d1) * (float).5;
    x1 = x[*i6] - pro1 * (x[*i6] - x[*i3]);
    y1 = y[*i6] - pro1 * (y[*i6] - y[*i3]);
    pro2 = (d2b / d2 + d1a / d1) * (float).5;
    x2 = *xmid1 - pro2 * (*xmid1 - x[*i5]);
    y2 = *ymid1 - pro2 * (*ymid1 - y[*i5]);
    pro3 = (d2a / d2 + d3b / d3) * (float).5;
    x3 = x[*i4] - pro3 * (x[*i4] - x[*i7]);
    y3 = y[*i4] - pro3 * (y[*i4] - y[*i7]);
/*  AVERAGE POINTS TO GET THE FIRST CENTER */
    *xcen1 = (x1 + x2 + x3) / (float)3.;
    *ycen1 = (y1 + y2 + y3) / (float)3.;
/*  FOR THE SECOND TRIANGLE, */
/*  FIND DISTANCES FROM CORNER TO CORNER,  AND CORNERS TO SIDE DIVISIONS */
    int__ = *i6 - *i4;
    prop = (doublereal) (*nper + 1 - *i8) / (doublereal) int__;
    *xmid2 = x[*i3] + prop * (x[*i7] - x[*i3]);
    *ymid2 = y[*i3] + prop * (y[*i7] - y[*i3]);
/* Computing 2nd power */
    r__1 = x[*i3] - x[*i1];
/* Computing 2nd power */
    r__2 = y[*i3] - y[*i1];
    d1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i7] - x[*i3];
/* Computing 2nd power */
    r__2 = y[*i7] - y[*i3];
    d2 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i1] - x[*i7];
/* Computing 2nd power */
    r__2 = y[*i1] - y[*i7];
    d3 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i2] - x[*i1];
/* Computing 2nd power */
    r__2 = y[*i2] - y[*i1];
    d1a = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i3] - x[*i2];
/* Computing 2nd power */
    r__2 = y[*i3] - y[*i2];
    d1b = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = *xmid2 - x[*i3];
/* Computing 2nd power */
    r__2 = *ymid2 - y[*i3];
    d2a = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i7] - *xmid2;
/* Computing 2nd power */
    r__2 = y[*i7] - *ymid2;
    d2b = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i8] - x[*i7];
/* Computing 2nd power */
    r__2 = y[*i8] - y[*i7];
    d3a = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = x[*i1] - x[*i8];
/* Computing 2nd power */
    r__2 = y[*i1] - y[*i8];
    d3b = sqrt(r__1 * r__1 + r__2 * r__2);
/*  GET MIDPOINT TRIALS 1,  2,  AND 3 AS PROPORTIONS */
    pro1 = (d3a / d3 + d1b / d1) * (float).5;
    x1 = *xmid2 - pro1 * (*xmid2 - x[*i1]);
    y1 = *ymid2 - pro1 * (*ymid2 - y[*i1]);
    pro2 = (d2b / d2 + d1a / d1) * (float).5;
    x2 = x[*i8] - pro2 * (x[*i8] - x[*i3]);
    y2 = y[*i8] - pro2 * (y[*i8] - y[*i3]);
    pro3 = (d2a / d2 + d3b / d3) * (float).5;
    x3 = x[*i2] - pro3 * (x[*i2] - x[*i7]);
    y3 = y[*i2] - pro3 * (y[*i2] - y[*i7]);
/*  AVERAGE POINTS TO GET THE CENTER */
    *xcen2 = (x1 + x2 + x3) / (float)3.;
    *ycen2 = (y1 + y2 + y3) / (float)3.;
    *err = FALSE_;
    return 0;
} /* gettrn_ */

#ifdef __cplusplus
	}
#endif

/* getwt.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getwt_(integer *mp, integer *ml, integer *mxlps, integer 
	*nix, integer *ilist, real *xlist, integer *iline, integer *lcon, 
	integer *ltype, real *coor, integer *linkp, real *fract, logical *
	addlnk, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 POINTS GIVEN FOR LINE\002,i5,\002 DO NO\
T DEFINE AN ARC\002)";
    static char fmt_10010[] = "(\002 NO X ON ARC LINE\002,i5,\002 FOUND IN G\
ETWT\002)";
    static char fmt_10020[] = "(\002 POINTS FOR LINE\002,i5,\002 DOES NOT DE\
FINE A PARABOLA\002)";
    static char fmt_10030[] = "(\002 ZERO LINE LENGTH FOR PARABOLA LINE\002,\
i5,\002 IN GETWT\002)";
    static char fmt_10040[] = "(\002 PARABOLA ALIGNMENT PROBLEMS FOR LINE\
\002,i5,\002 IN GETWT\002)";
    static char fmt_10050[] = "(\002 PARABOLA INTERSECTION PROBLEMS FOR LIN\
E\002,i5,\002 IN GETWT\002)";
    static char fmt_10060[] = "(\002 PARABOLA SOLUTION PROBLEMS FOR LINE\002\
,i5,\002 IN GETWT\002)";

    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2, r__3, r__4;

    /* Builtin functions */
    double atan2(doublereal, doublereal), cos(doublereal), sin(doublereal), 
	    sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    double log(doublereal), exp(doublereal);

    /* Local variables */
    static real a, b;
    static integer i__, j;
    static real r__, x, r1, r2, x0, x1, y1, x2, y2, aa, bb;
    static integer il;
    static real pi, dx, dy;
    static integer kt, ip1, ip2, ip3;
    static real del, ang, det;
    static integer ijk;
    static real arm, dot, eps, tang, xcen, ycen, xmid, ymid, perp, cost, cott,
	     sint, xnew, ynew, xtop, ytop;
    static integer ihold;
    static real chord, halfw, theta, divis, twopi, xtest, theta1, theta2, 
	    theta3;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static real height, xfract, radius, xdivis;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___29 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___39 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___45 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___48 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___55 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___61 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___63 = { 0, 6, 0, fmt_10060, 0 };


/* *********************************************************************** */
/*  SUBROUTINE GETWT = GETS A WEIGHT BASED ON A PERCENTAGE DISTANCE ALONG */
/*                     THE GIVEN SIDE LINE LIST. */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     ADDWT = ADDS THE WEIGHTING FACTORS TO ANY NODES WITH */
/*             FLAGS CONTAINING WEIGHTS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     FRACT = THE FRACTION OF TOTAL DISTANCE ALONG THE X AXIS */
/*               (TAKES BACK THE WEIGHT OR Y VALUE AT THAT % DISTANCE) */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    --ltype;
    lcon -= 4;
    --iline;
    --xlist;
    --ilist;

    /* Function Body */
    *addlnk = FALSE_;
    *err = FALSE_;
    pi = atan2((float)0., (float)-1.);
    twopi = pi * (float)2.;
/*  GET THE X VALUE */
    x = *fract * (xlist[*nix] - xlist[1]) + xlist[1];
/*  FIND THE LINE THIS BELONGS TO */
    i__1 = *nix - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (x <= xlist[i__ + 1] && x >= xlist[i__]) {
	    il = ilist[i__];
	    goto L110;
	}
/* L100: */
    }
    mesage_("PROBLEMS IN GETWT - NO X SPAN FOUND", (ftnlen)35);
    *err = TRUE_;
    return 0;
L110:
/*  NOW GET THE Y VALUE FOR THE X AND THE LINE  (AND TYPE OF LINE) GIVEN */
    kt = ltype[il];
    ltsort_(mp, &linkp[3], &lcon[il * 3 + 1], &ip1, addlnk);
    ltsort_(mp, &linkp[3], &lcon[il * 3 + 2], &ip2, addlnk);
    i__2 = (i__1 = lcon[il * 3 + 3], abs(i__1));
    ltsort_(mp, &linkp[3], &i__2, &ip3, addlnk);
    if (lcon[il * 3 + 3] < 0) {
	ip3 = -ip3;
    }
/*  CHECK FOR EXACT LINE END PLACEMENT */
    eps = (r__1 = xlist[*nix] - xlist[1], dabs(r__1)) * (float)1e-5;
    if ((r__1 = x - coor[(ip1 << 1) + 1], dabs(r__1)) < eps) {
	*fract = coor[(ip1 << 1) + 2];
	return 0;
    } else if ((r__1 = x - coor[(ip2 << 1) + 1], dabs(r__1)) < eps) {
	*fract = coor[(ip2 << 1) + 2];
	return 0;
    }
/*  GET INTERMEDIATE Y VALUE BASED ON THE LINE TYPE */
/*  FIRST - STRAIGHT LINES */
    if (kt == 1) {
	if (coor[(ip1 << 1) + 1] > coor[(ip2 << 1) + 1]) {
	    ihold = ip1;
	    ip1 = ip2;
	    ip2 = ihold;
	}
	xfract = (x - coor[(ip1 << 1) + 1]) / (coor[(ip2 << 1) + 1] - coor[(
		ip1 << 1) + 1]);
	*fract = xfract * (coor[(ip2 << 1) + 2] - coor[(ip1 << 1) + 2]) + 
		coor[(ip1 << 1) + 2];
/*  NEXT - CORNER LINES */
    } else if (kt == 2) {
	if (coor[(ip1 << 1) + 1] > coor[(ip2 << 1) + 1]) {
	    ihold = ip1;
	    ip1 = ip2;
	    ip2 = ihold;
	}
	if (coor[(ip3 << 1) + 1] > x) {
	    ip2 = ip3;
	    xfract = (x - coor[(ip1 << 1) + 1]) / (coor[(ip2 << 1) + 1] - 
		    coor[(ip1 << 1) + 1]);
	    *fract = xfract * (coor[(ip2 << 1) + 2] - coor[(ip1 << 1) + 2]) + 
		    coor[(ip1 << 1) + 2];
	} else if (coor[(ip3 << 1) + 1] < x) {
	    ip1 = ip3;
	    xfract = (x - coor[(ip1 << 1) + 1]) / (coor[(ip2 << 1) + 1] - 
		    coor[(ip1 << 1) + 1]);
	    *fract = xfract * (coor[(ip2 << 1) + 2] - coor[(ip1 << 1) + 2]) + 
		    coor[(ip1 << 1) + 2];
	} else {
	    *fract = coor[(ip3 << 1) + 2];
	}
/*  NEXT - ARCS */
    } else if (kt == 3 || kt == 4 || kt == 6) {
/*  ARCWITH CENTER GIVEN */
/*  ARCGOES FROM 1ST POINT TO 2ND IN *COUNTER-CLOCKWISE* DIRECTION. */
	if (kt == 3) {
	    xcen = coor[(abs(ip3) << 1) + 1];
	    ycen = coor[(abs(ip3) << 1) + 2];
/*  CIRCLE WITH THIRD POINT ON ARC. */
	} else if (kt == 4) {
	    theta1 = atan2(coor[(ip3 << 1) + 2] - coor[(ip1 << 1) + 2], coor[(
		    ip3 << 1) + 1] - coor[(ip1 << 1) + 1]) + pi / (float)2.;
	    theta2 = atan2(coor[(ip3 << 1) + 2] - coor[(ip2 << 1) + 2], coor[(
		    ip3 << 1) + 1] - coor[(ip2 << 1) + 1]) + pi / (float)2.;
	    det = -cos(theta1) * sin(theta2) + cos(theta2) * sin(theta1);
	    x1 = (coor[(ip1 << 1) + 1] + coor[(ip3 << 1) + 1]) * (float).5;
	    y1 = (coor[(ip1 << 1) + 2] + coor[(ip3 << 1) + 2]) * (float).5;
	    x2 = (coor[(ip2 << 1) + 1] + coor[(ip3 << 1) + 1]) * (float).5;
	    y2 = (coor[(ip2 << 1) + 2] + coor[(ip3 << 1) + 2]) * (float).5;
	    r__ = (-sin(theta2) * (x2 - x1) + cos(theta2) * (y2 - y1)) / det;
	    xcen = x1 + r__ * cos(theta1);
	    ycen = y1 + r__ * sin(theta1);
/*     CIRCLE WITH RADIUS GIVEN */
	} else if (kt == 6) {
	    dx = (coor[(ip2 << 1) + 1] - coor[(ip1 << 1) + 1]) * (float).5;
	    dy = (coor[(ip2 << 1) + 2] - coor[(ip1 << 1) + 2]) * (float).5;
	    chord = sqrt(dx * dx + dy * dy);
	    r__ = (r__1 = coor[(abs(ip3) << 1) + 1], dabs(r__1));
	    if (r__ <= chord) {
		xcen = (coor[(ip1 << 1) + 1] + coor[(ip2 << 1) + 1]) * (float)
			.5;
		ycen = (coor[(ip1 << 1) + 2] + coor[(ip2 << 1) + 2]) * (float)
			.5;
	    } else {
		arm = sqrt(r__ * r__ - chord * chord);
		if (ip3 < 0) {
		    xcen = coor[(ip1 << 1) + 1] + dx + arm * dy / chord;
		    ycen = coor[(ip1 << 1) + 2] + dy - arm * dx / chord;
		} else {
		    xcen = coor[(ip1 << 1) + 1] + dx - arm * dy / chord;
		    ycen = coor[(ip1 << 1) + 2] + dy + arm * dx / chord;
		}
	    }
	}
/* Computing 2nd power */
	r__1 = coor[(ip1 << 1) + 1] - xcen;
/* Computing 2nd power */
	r__2 = coor[(ip1 << 1) + 2] - ycen;
	r1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = coor[(ip2 << 1) + 1] - xcen;
/* Computing 2nd power */
	r__2 = coor[(ip2 << 1) + 2] - ycen;
	r2 = sqrt(r__1 * r__1 + r__2 * r__2);
	if (r1 == (float)0. || r2 == (float)0.) {
	    *err = TRUE_;
	    s_wsfe(&io___29);
	    do_fio(&c__1, (char *)&iline[il], (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
	theta1 = atan2(coor[(ip1 << 1) + 2] - ycen, coor[(ip1 << 1) + 1] - 
		xcen);
	theta2 = atan2(coor[(ip2 << 1) + 2] - ycen, coor[(ip2 << 1) + 1] - 
		xcen);
/*  ARCWITH THE CENTER GIVEN */
	if (kt == 3) {
	    if (ip3 >= 0 && theta2 <= theta1) {
		theta2 += twopi;
	    }
	    if (ip3 < 0 && theta1 <= theta2) {
		theta1 += twopi;
	    }
	    tang = theta2 - theta1;
/*  CIRCULAR ARC WITH 3RD POINT ON ARC - CLOCKWISE OR COUNTER-CLOCKWISE */
	} else if (kt == 4) {
	    theta3 = atan2(coor[(ip3 << 1) + 2] - ycen, coor[(ip3 << 1) + 1] 
		    - xcen);
	    if (theta2 <= theta1) {
		theta2 += twopi;
	    }
	    if (theta3 <= theta1) {
		theta3 += twopi;
	    }
	    tang = theta2 - theta1;
	    if (theta3 > theta2) {
		tang = -(twopi - tang);
	    }
/*  CIRCULAR ARC WITH RADIUS GIVEN - CLOCKWISE OR COUNTER-CLOCKWISE */
	} else if (kt == 6) {
	    if (ip3 >= 0 && theta2 <= theta1) {
		theta2 += twopi;
	    }
	    if (ip3 < 0 && theta1 <= theta2) {
		theta1 += twopi;
	    }
	    tang = theta2 - theta1;
	}
/*  NOW ITERATE UNTIL THE X VALUE IS WITHIN SOME EPSILON */
	aa = log(r2 / r1) / (theta2 - theta1);
	bb = r2 / exp(aa * theta2);
	ang = theta1;
	eps = (r__1 = coor[(ip1 << 1) + 1] - coor[(ip2 << 1) + 1], dabs(r__1))
		 * (float)1e-6;
	for (i__ = 1; i__ <= 10; ++i__) {
	    del = tang * (float).1;
	    for (j = 1; j <= 10; ++j) {
		ang += del;
		radius = bb * exp(aa * ang);
		xtest = xcen + cos(ang) * radius;
		if (eps >= (r__1 = xtest - x, dabs(r__1))) {
		    *fract = ycen + sin(ang) * radius;
		    goto L150;
		} else if (coor[(ip1 << 1) + 1] < coor[(ip2 << 1) + 1] && 
			xtest > x) {
		    ang -= del;
		    tang = del;
		    goto L130;
		} else if (coor[(ip1 << 1) + 1] > coor[(ip2 << 1) + 1] && 
			xtest < x) {
		    ang -= del;
		    tang = del;
		    goto L130;
		}
/* L120: */
	    }
L130:
/* L140: */
	    ;
	}
	*err = TRUE_;
	s_wsfe(&io___39);
	do_fio(&c__1, (char *)&iline[il], (ftnlen)sizeof(integer));
	e_wsfe();
	return 0;
L150:
/*  FINALLY PARABOLAS */
	;
    } else if (kt == 5) {
/*  CHECK LEGITIMACY OF DATA */
	if (coor[(ip1 << 1) + 1] > coor[(ip2 << 1) + 1]) {
	    ijk = ip1;
	    ip1 = ip2;
	    ip2 = ijk;
	}
	xmid = (coor[(ip1 << 1) + 1] + coor[(ip2 << 1) + 1]) * (float).5;
	ymid = (coor[(ip1 << 1) + 2] + coor[(ip2 << 1) + 2]) * (float).5;
	dot = (coor[(ip2 << 1) + 1] - coor[(ip1 << 1) + 1]) * (coor[(ip3 << 1)
		 + 1] - xmid) + (coor[(ip2 << 1) + 2] - coor[(ip1 << 1) + 2]) 
		* (coor[(ip3 << 1) + 2] - ymid);
/* Computing 2nd power */
	r__1 = coor[(ip2 << 1) + 1] - coor[(ip1 << 1) + 1];
/* Computing 2nd power */
	r__2 = coor[(ip2 << 1) + 2] - coor[(ip1 << 1) + 2];
/* Computing 2nd power */
	r__3 = coor[(ip3 << 1) + 1] - xmid;
/* Computing 2nd power */
	r__4 = coor[(ip3 << 1) + 2] - ymid;
	perp = sqrt(r__1 * r__1 + r__2 * r__2) * sqrt(r__3 * r__3 + r__4 * 
		r__4);
	if (dot >= perp * (float).05) {
	    s_wsfe(&io___45);
	    do_fio(&c__1, (char *)&iline[il], (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    return 0;
	}
/*  GET TRANSFORMATION TO PARABOLA COORDINATE SYSTEM  (Y = 4AX **2) */
/* Computing 2nd power */
	r__1 = coor[(ip2 << 1) + 1] - coor[(ip1 << 1) + 1];
/* Computing 2nd power */
	r__2 = coor[(ip2 << 1) + 2] - coor[(ip1 << 1) + 2];
	halfw = sqrt(r__1 * r__1 + r__2 * r__2) * (float).5;
/* Computing 2nd power */
	r__1 = xmid - coor[(ip3 << 1) + 1];
/* Computing 2nd power */
	r__2 = ymid - coor[(ip3 << 1) + 2];
	height = sqrt(r__1 * r__1 + r__2 * r__2);
	if (height == (float)0. || halfw == (float)0.) {
	    s_wsfe(&io___48);
	    do_fio(&c__1, (char *)&iline[il], (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    return 0;
	}
/* Computing 2nd power */
	r__1 = halfw;
	a = height / (r__1 * r__1);
	xtop = coor[(ip3 << 1) + 1];
	ytop = coor[(ip3 << 1) + 2];
	theta = atan2(ymid - ytop, xmid - xtop);
	sint = sin(theta);
	cost = cos(theta);
	if (sint == (float)0.) {
	    s_wsfe(&io___55);
	    do_fio(&c__1, (char *)&iline[il], (ftnlen)sizeof(integer));
	    e_wsfe();
	    *err = TRUE_;
	    return 0;
	}
	cott = cost / sint;
/*  FIND THE EQUATION OF THE LINE FOR X  =  CONSTANT IN NEW COORDINATES */
	x0 = x - xtop;
	b = -(sint * x0) - cott * cost * x0;
/*  IF THE LINE HAS A ZERO SLOPE,  THEN FIND THE SIMPLE SOLUTION */
	if (cott == (float)0.) {
	    ynew = b;
	} else {
	    divis = (float)1. - cott * (float)4. * a * b;
	    if (divis < (float)0.) {
		s_wsfe(&io___61);
		do_fio(&c__1, (char *)&iline[il], (ftnlen)sizeof(integer));
		e_wsfe();
		*err = TRUE_;
		return 0;
	    }
	    xdivis = sqrt(divis);
	    y1 = (xdivis + (float)1.) / (cott * (float)2. * a);
	    y2 = ((float)1. - xdivis) / (cott * (float)2. * a);
	    if (dabs(y1) <= halfw && dabs(y2) > halfw) {
		ynew = y1;
	    } else if (dabs(y2) <= halfw && dabs(y1) > halfw) {
		ynew = y2;
	    } else {
		s_wsfe(&io___63);
		do_fio(&c__1, (char *)&iline[il], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
/*  TRANSLATE THIS XNEW TO A Y VALUE */
	xnew = a * ynew * ynew;
	*fract = xnew * sint + ynew * cost + ytop;
    }
    return 0;
} /* getwt_ */

#ifdef __cplusplus
	}
#endif

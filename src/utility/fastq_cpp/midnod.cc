/* midnod.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int midnod_(integer *npnode, integer *nnuid, integer *npelem,
	 integer *nnxk, integer *mp, integer *ml, integer *kkk, integer *nnn, 
	integer *nall, integer *nl, integer *nxk, integer *nuid, real *xn, 
	real *yn, integer *listn, real *coor, integer *iline, integer *ltype, 
	integer *lcon, integer *linkp, integer *linkl, logical *three, 
	logical *eight, logical *nine)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 NODE ARRAY OVERFLOW IN MIDNOD\002,/\002\
 THERE ARE MORE THAN\002,i5,\002 NODES IN THE MESH\002)";

    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2, i__3;
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), cos(doublereal), sin(doublereal), 
	    sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, k, l;
    static real r__, r1, r2, x1, y1, x2, y2;
    static integer ii;
    static real pi, dx, dy;
    static integer kt, lt;
    static real rm;
    static integer ip1, ip2, ip3;
    static real det;
    static integer nhi;
    static real arm;
    static integer nlo;
    static real xcen, ycen;
    static integer lnew;
    static real tupi, xint, yint;
    static integer nsum, nodea, nodeb;
    static real chord;
    static integer nuida, nuidb;
    static logical itsok;
    static integer ltest;
    static real theta1, theta2;
    static logical addlnk;
    static real thetam;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___42 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE MIDNOD = GENERATES THE MIDSIDE NODE FOR ELEMENTS */
/* *********************************************************************** */
    /* Parameter adjustments */
    --yn;
    --xn;
    --listn;
    --nuid;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;
    linkp -= 3;
    coor -= 3;
    linkl -= 3;
    lcon -= 4;
    --ltype;
    --iline;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    tupi = pi * (float)2.;
    addlnk = FALSE_;
    *nall = *nnn;
    i__1 = *kkk;
    for (j = 1; j <= i__1; ++j) {
	for (i__ = 1; i__ <= 4; ++i__) {
/*  SKIP DUPLICATE DESCRIPTORS AND ELEMENTS THAT ARE NOT TO */
/*  HAVE MIDSIDE NODES */
	    if (nxk[i__ + j * nxk_dim1] < 0) {
/*  THIS ELEMENT IS A BAR ELEMENT */
		if (nxk[j * nxk_dim1 + 3] == 0) {
		    if (*three) {
			itsok = TRUE_;
		    } else {
			itsok = FALSE_;
		    }
/*  THIS ELEMENT IS A QUAD ELEMENT */
		} else if (*eight || *nine) {
		    itsok = TRUE_;
		} else {
		    itsok = FALSE_;
		}
	    } else {
		itsok = FALSE_;
	    }
/*  GENERATE THE MIDSIDE NODE IF APPROPRIATE */
	    if (itsok) {
		ii = i__ + 1;
		if (i__ >= 4) {
		    ii = 1;
		}
		nodea = (i__2 = nxk[i__ + j * nxk_dim1], abs(i__2));
		nodeb = (i__2 = nxk[ii + j * nxk_dim1], abs(i__2));
/*  CHECK TO SEE IF THE ELEMENT IS A BARSET */
		if (nodea > 0 && nodeb > 0) {
		    nuida = nuid[nodea];
		    nuidb = nuid[nodeb];
/*  IF ONE NODE OR THE OTHER IS INTERIOR,  USE LINEAR INTERPOLATION */
		    if (nuida > 100000 && nuida < 1000000000 || nuidb > 
			    100000 && nuidb < 1000000000) {
			xint = (xn[nodea] + xn[nodeb]) * (float).5;
			yint = (yn[nodea] + yn[nodeb]) * (float).5;
/*  BOTH ARE POINT OR LINE NODES. */
/*  FIND THE LINE THAT THEY BELONG TO */
		    } else {
			ltest = 0;
/*  SEE IF ONE IS NOT THE END POINT OF A LINE */
/*  THEN CHECK THE OTHER TO SEE IF IT IS ON THE SAME LINE */
/*  IF IT IS NOT ON THE SAME LINE,  THEN USE LINEAR INTERPOLATION */
			if (nuida > 1000000000) {
			    ltest = (nuida - 1000000000) / 100000;
			    ltsort_(ml, &linkl[3], &ltest, &lt, &addlnk);
			    if (nuidb > 1000000000) {
				lnew = (abs(nuidb) - 1000000000) / 100000;
				if (lnew != ltest) {
				    ltest = 0;
				}
			    } else {
				if (lcon[lt * 3 + 1] != nuidb && lcon[lt * 3 
					+ 2] != nuidb) {
				    ltest = 0;
				}
			    }
			} else if (nuidb > 1000000000) {
			    ltest = (nuidb - 1000000000) / 100000;
			    ltsort_(ml, &linkl[3], &ltest, &lt, &addlnk);
			    if (nuida > 1000000000) {
				lnew = (abs(nuida) - 1000000000) / 100000;
				if (lnew != ltest) {
				    ltest = 0;
				}
			    } else {
				if (lcon[lt * 3 + 1] != nuida && lcon[lt * 3 
					+ 2] != nuida) {
				    ltest = 0;
				}
			    }
			} else {
/*  BOTH ARE END POINTS OF LINES - SEE IF THEY ARE ON THE SAME LINE */
			    nsum = abs(nuida) + abs(nuidb);
			    i__2 = *nl;
			    for (l = 1; l <= i__2; ++l) {
				ltsort_(ml, &linkl[3], &iline[l], &k, &addlnk)
					;
				if (lcon[k * 3 + 1] + lcon[k * 3 + 2] == nsum)
					 {
				    if (lcon[k * 3 + 1] == abs(nuida) && lcon[
					    k * 3 + 2] == abs(nuidb) || lcon[
					    k * 3 + 1] == abs(nuidb) && lcon[
					    k * 3 + 2] == abs(nuida)) {
					ltest = iline[l];
					goto L110;
				    }
				}
/* L100: */
			    }
L110:
			    ;
			}
/*  IF THEY ARE NOT ON THE SAME LINE,  IT IS NOT IN ERROR */
/*  THEY SPAN A SINGLE WIDTH REGION */
/*  ASSUME LINEAR INTERPOLATION */
			if (ltest == 0) {
			    kt = 0;
			} else {
			    ltsort_(ml, &linkl[3], &ltest, &lt, &addlnk);
			    kt = ltype[lt];
			}
/*  CALCULATE THE MID-SIDE NODE BY LINEAR INTERPOLATION */
/*  IF THE LINE TYPE IS STRAIGHT OR CORNER OR THE 2 END POINTS */
/*  SPAN A SINGLE WIDTH REGION */
			if (kt < 3) {
			    xint = (xn[nodea] + xn[nodeb]) * (float).5;
			    yint = (yn[nodea] + yn[nodeb]) * (float).5;
/*  IF THE LINE IS A CIRCLE OR PARABOLA,  GENERATE THE MID-POINT BY USING */
/*  THE 2 NODES AS ARC ENDPOINTS AND AN INTERVAL OF 2 FOR THE LINE. */
/*  THE CENTER OF THE LINE MUST BE FOUND FROM THE LINE CARD ITSELF. */
			} else {
			    ltsort_(mp, &linkp[3], &lcon[lt * 3 + 1], &ip1, &
				    addlnk);
			    ltsort_(mp, &linkp[3], &lcon[lt * 3 + 2], &ip2, &
				    addlnk);
			    if (lcon[lt * 3 + 3] > 0) {
				ltsort_(mp, &linkp[3], &lcon[lt * 3 + 3], &
					ip3, &addlnk);
			    } else if (lcon[lt * 3 + 3] < 0) {
				i__3 = (i__2 = lcon[lt * 3 + 3], abs(i__2));
				ltsort_(mp, &linkp[3], &i__3, &ip3, &addlnk);
				ip3 = -ip3;
			    } else {
				ip3 = 0;
			    }
/*  ARC WITH CENTER GIVEN */
/*  ARC GOES FROM 1ST POINT TO 2ND IN *COUNTER-CLOCKWISE* DIRECTION. */
			    if (kt == 3) {
				xcen = coor[(abs(ip3) << 1) + 1];
				ycen = coor[(abs(ip3) << 1) + 2];
/*  CIRCLE WITH THIRD POINT ON ARC. */
			    } else if (kt == 4) {
				theta1 = atan2(coor[(ip3 << 1) + 2] - coor[(
					ip1 << 1) + 2], coor[(ip3 << 1) + 1] 
					- coor[(ip1 << 1) + 1]) + pi / (float)
					2.;
				theta2 = atan2(coor[(ip3 << 1) + 2] - coor[(
					ip2 << 1) + 2], coor[(ip3 << 1) + 1] 
					- coor[(ip2 << 1) + 1]) + pi / (float)
					2.;
				det = -cos(theta1) * sin(theta2) + cos(theta2)
					 * sin(theta1);
				x1 = (coor[(ip1 << 1) + 1] + coor[(ip3 << 1) 
					+ 1]) * (float).5;
				y1 = (coor[(ip1 << 1) + 2] + coor[(ip3 << 1) 
					+ 2]) * (float).5;
				x2 = (coor[(ip2 << 1) + 1] + coor[(ip3 << 1) 
					+ 1]) * (float).5;
				y2 = (coor[(ip2 << 1) + 2] + coor[(ip3 << 1) 
					+ 2]) * (float).5;
				r__ = (-sin(theta2) * (x2 - x1) + cos(theta2) 
					* (y2 - y1)) / det;
				xcen = x1 + r__ * cos(theta1);
				ycen = y1 + r__ * sin(theta1);
/*  PARABOLA WITH CENTER GIVEN */
			    } else if (kt == 5) {
				xcen = coor[(abs(ip3) << 1) + 1];
				ycen = coor[(abs(ip3) << 1) + 2];
/*  CIRCLE WITH RADIUS GIVEN */
			    } else if (kt == 6) {
				dx = (coor[(ip2 << 1) + 1] - coor[(ip1 << 1) 
					+ 1]) * (float).5;
				dy = (coor[(ip2 << 1) + 2] - coor[(ip1 << 1) 
					+ 2]) * (float).5;
				chord = sqrt(dx * dx + dy * dy);
				r__ = (r__1 = coor[(abs(ip3) << 1) + 1], dabs(
					r__1));
				if (r__ <= chord) {
				    xcen = (coor[(ip1 << 1) + 1] + coor[(ip2 
					    << 1) + 1]) * (float).5;
				    ycen = (coor[(ip1 << 1) + 2] + coor[(ip2 
					    << 1) + 2]) * (float).5;
				} else {
				    arm = sqrt(r__ * r__ - chord * chord);
				}
				if (ip3 < 0) {
				    xcen = coor[(ip1 << 1) + 1] + dx + arm * 
					    dy / chord;
				    ycen = coor[(ip1 << 1) + 2] + dy - arm * 
					    dx / chord;
				} else {
				    xcen = coor[(ip1 << 1) + 1] + dx - arm * 
					    dy / chord;
				    ycen = coor[(ip1 << 1) + 2] + dy + arm * 
					    dx / chord;
				}
			    }
/*  CALCULATE THE MIDPOINT ON THE ARC */
/* Computing 2nd power */
			    r__1 = xn[nodea] - xcen;
/* Computing 2nd power */
			    r__2 = yn[nodea] - ycen;
			    r1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
			    r__1 = xn[nodeb] - xcen;
/* Computing 2nd power */
			    r__2 = yn[nodeb] - ycen;
			    r2 = sqrt(r__1 * r__1 + r__2 * r__2);
			    rm = (r1 + r2) * (float).5;
			    theta1 = atan2(yn[nodea] - ycen, xn[nodea] - xcen)
				    ;
			    if (theta1 < (float)0.) {
				theta1 += tupi;
			    }
			    theta2 = atan2(yn[nodeb] - ycen, xn[nodeb] - xcen)
				    ;
			    if (theta2 < (float)0.) {
				theta2 += tupi;
			    }
			    thetam = (theta1 + theta2) * (float).5;
			    if ((r__1 = theta1 - theta2, dabs(r__1)) > pi) {
				thetam += pi;
			    }
			    xint = cos(thetam) * rm + xcen;
			    yint = sin(thetam) * rm + ycen;
			}
		    }
/*  ADD THIS NEW NODE TO THE NODE LIST. */
		    if (*nall >= *npnode) {
			s_wsfe(&io___42);
			do_fio(&c__1, (char *)&(*npnode), (ftnlen)sizeof(
				integer));
			e_wsfe();
			goto L140;
		    }
		    ++(*nall);
		    nlo = min(nodea,nodeb);
		    nhi = max(nodea,nodeb);
		    listn[*nall] = nlo * 100000 + nhi;
		    xn[*nall] = xint;
		    yn[*nall] = yint;
		}
	    }
/* L120: */
	}
/* L130: */
    }
L140:
    return 0;
} /* midnod_ */

#ifdef __cplusplus
	}
#endif

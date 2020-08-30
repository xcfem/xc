/* elaray.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int elaray_(real *xnold, real *ynold, integer *nxkold, 
	integer *mmpold, integer *linkeg, integer *listeg, integer *mlink, 
	integer *nprold, integer *npnold, integer *npeold, integer *nnxk, 
	real *xmin, real *xmax, real *ymin, real *ymax, integer *idivis)
{
    /* System generated locals */
    integer nxkold_dim1, nxkold_offset, i__1, i__2;
    real r__1, r__2;

    /* Local variables */
    static integer i__, j;
    static real u, w, x1, y1, x2, y2;
    static integer ic;
    static real xi, xl, yl, xu, yu, eta;
    static integer icon, jcon, kelem, index;
    static real xemin, yemin, xemax, yemax;
    static integer kount;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static logical inside;
    static real xdelta, ydelta;
    extern /* Subroutine */ int invmap_(real *, real *, real *, real *, real *
	    , real *, real *, real *, real *, real *, real *, real *, logical 
	    *), intsct_(real *, real *, real *, real *, real *, real *, real *
	    , real *, real *, real *, logical *);
    static logical lcross;

/* *********************************************************************** */
/*  SUBROUTINE ELARAY = PUTS ELEMENTS INTO AN ARRAY BASED ON THEIR */
/*                      PHYSICAL LOCATION */
/* *********************************************************************** */
/*  FIND THE EXTREMES FOR THE MESH DATA */
    /* Parameter adjustments */
    linkeg -= 3;
    mmpold -= 4;
    --ynold;
    --xnold;
    --listeg;
    nxkold_dim1 = *nnxk;
    nxkold_offset = 1 + nxkold_dim1;
    nxkold -= nxkold_offset;

    /* Function Body */
    *xmin = xnold[1];
    *xmax = xnold[1];
    *ymin = ynold[1];
    *ymax = ynold[1];
    i__1 = *npnold;
    for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing MIN */
	r__1 = *xmin, r__2 = xnold[i__];
	*xmin = dmin(r__1,r__2);
/* Computing MAX */
	r__1 = *xmax, r__2 = xnold[i__];
	*xmax = dmax(r__1,r__2);
/* Computing MIN */
	r__1 = *ymin, r__2 = ynold[i__];
	*ymin = dmin(r__1,r__2);
/* Computing MAX */
	r__1 = *ymax, r__2 = ynold[i__];
	*ymax = dmax(r__1,r__2);
/* L100: */
    }
/*  SET UP THE SIZE OF THE ARRAY BASED ON THE MLINK DIMENSION */
/*        IF MLINK = 55 THEN THERE ARE 5 COLUMNS AND 5 ROWS */
/*                 = 66 THEN THERE ARE 6 COLUMNS AND 6 ROWS, ETC. */
    if (*mlink == 22) {
	*idivis = 2;
    } else if (*mlink == 33) {
	*idivis = 3;
    } else if (*mlink == 44) {
	*idivis = 4;
    } else if (*mlink == 55) {
	*idivis = 5;
    } else if (*mlink == 66) {
	*idivis = 6;
    } else if (*mlink == 77) {
	*idivis = 7;
    } else if (*mlink == 88) {
	*idivis = 8;
    } else if (*mlink == 99) {
	*idivis = 9;
    }
/*  NOW THE ELEMENTS MUST BE SORTED INTO ANY ARRAY SPACE THAT THE ELEMENT */
/*  CROSSES.  THE ARRAY IS LOGICALLY A SQUARE, BUT PHYSICALLY CAN BE */
/*  RECTANGULAR SINCE THE X AND Y EXTREMES MAY FORM ANY SIZE RECTANGLE. */
/*  ROWS FIRST IN THE ARRAY AND THEN COLUMNS. */
    xdelta = (*xmax - *xmin) / (doublereal) (*idivis);
    ydelta = (*ymax - *ymin) / (doublereal) (*idivis);
    kount = 0;
    for (j = *idivis; j >= 1; --j) {
	if (j == 1) {
	    yl = *ymin;
	} else {
	    yl = *ymin + ydelta * (doublereal) (j - 1);
	}
	if (j == *idivis) {
	    yu = *ymax;
	} else {
	    yu = *ymin + ydelta * (doublereal) j;
	}
	i__1 = *idivis;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (i__ == 1) {
		xl = *xmin;
	    } else {
		xl = *xmin + xdelta * (doublereal) (i__ - 1);
	    }
	    if (i__ == *idivis) {
		xu = *xmax;
	    } else {
		xu = *xmin + xdelta * (doublereal) i__;
	    }
	    index = (*idivis - j + 1) * 10 + i__;
	    linkeg[(index << 1) + 1] = kount + 1;
	    linkeg[(index << 1) + 2] = 0;
/*  ONLY CHECK ELEMENTS OF THE SAME MATERIAL ID (BLOCK ID) */
	    i__2 = *npeold;
	    for (kelem = 1; kelem <= i__2; ++kelem) {
		for (icon = 1; icon <= 4; ++icon) {
		    x1 = xnold[nxkold[icon + kelem * nxkold_dim1]];
		    y1 = ynold[nxkold[icon + kelem * nxkold_dim1]];
/*  TEST TO SEE IF THE NODE FITS IN THE GRID */
		    if (x1 <= xu && x1 >= xl && (y1 <= yu && y1 >= yl)) {
			++kount;
			if (kount > *npeold << 2) {
			    mesage_("** ERROR - NOT ENOUGH ROOM IN LISTEG, S\
UBROUTINE ELARAY **", (ftnlen)58);
			    goto L170;
			}
			++linkeg[(index << 1) + 2];
			listeg[kount] = kelem;
			goto L130;
		    }
/*  TEST TO SEE IF THE EDGE OF THE ELEMENT CROSSES THE GRID */
		    if (icon == 4) {
			jcon = 1;
		    } else {
			jcon = icon + 1;
		    }
		    x2 = xnold[nxkold[jcon + kelem * nxkold_dim1]];
		    y2 = ynold[nxkold[jcon + kelem * nxkold_dim1]];
		    intsct_(&x1, &y1, &x2, &y2, &xl, &yl, &xu, &yl, &u, &w, &
			    lcross);
		    if (! lcross) {
			intsct_(&x1, &y1, &x2, &y2, &xu, &yl, &xu, &yu, &u, &
				w, &lcross);
		    }
		    if (! lcross) {
			intsct_(&x1, &y1, &x2, &y2, &xu, &yu, &xl, &yu, &u, &
				w, &lcross);
		    }
		    if (! lcross) {
			intsct_(&x1, &y1, &x2, &y2, &xl, &yu, &xl, &yl, &u, &
				w, &lcross);
		    }
		    if (lcross) {
			++kount;
			if (kount > *npeold << 2) {
			    mesage_("** ERROR - NOT ENOUGH ROOM IN LISTEG, S\
UBROUTINE ELARAY **", (ftnlen)58);
			    goto L170;
			}
			++linkeg[(index << 1) + 2];
			listeg[kount] = kelem;
			goto L130;
		    }
/*  OTHERWISE TEST TO SEE IF THE ELEMENT COMPLETELY ENCLOSES THE GRID */
		    xemin = xnold[nxkold[kelem * nxkold_dim1 + 1]];
		    xemax = xnold[nxkold[kelem * nxkold_dim1 + 1]];
		    yemin = ynold[nxkold[kelem * nxkold_dim1 + 1]];
		    yemax = ynold[nxkold[kelem * nxkold_dim1 + 1]];
		    for (ic = 2; ic <= 4; ++ic) {
/* Computing MIN */
			r__1 = xemin, r__2 = xnold[nxkold[ic + kelem * 
				nxkold_dim1]];
			xemin = dmin(r__1,r__2);
/* Computing MAX */
			r__1 = xemax, r__2 = xnold[nxkold[ic + kelem * 
				nxkold_dim1]];
			xemax = dmax(r__1,r__2);
/* Computing MIN */
			r__1 = yemin, r__2 = ynold[nxkold[ic + kelem * 
				nxkold_dim1]];
			yemin = dmin(r__1,r__2);
/* Computing MAX */
			r__1 = yemax, r__2 = ynold[nxkold[ic + kelem * 
				nxkold_dim1]];
			yemax = dmax(r__1,r__2);
/* L110: */
		    }
		    if (xl > xemin || xu < xemax || yl > yemin || yu < yemax) 
			    {
			inside = FALSE_;
		    } else {
			invmap_(&x1, &y1, &xl, &yl, &xu, &yl, &xu, &yu, &xl, &
				yu, &xi, &eta, &inside);
		    }
		    if (inside) {
			++kount;
			if (kount > *npeold << 2) {
			    mesage_("** ERROR - NOT ENOUGH ROOM IN LISTEG, S\
UBROUTINE ELARAY **", (ftnlen)58);
			    goto L170;
			}
			++linkeg[(index << 1) + 2];
			listeg[kount] = kelem;
			goto L130;
		    }
/* L120: */
		}
L130:
/* L140: */
		;
	    }
/* L150: */
	}
/* L160: */
    }
L170:
    return 0;
} /* elaray_ */

#ifdef __cplusplus
	}
#endif

/* lpntin.f -- translated by f2c (version 20160102).
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
logical lpntin_(integer *maxnp, real *cx, real *cy, integer *npnt, real *x, 
	real *y)
{
    /* System generated locals */
    real r__1;
    logical ret_val;

    /* Local variables */
    static real u, v;
    static integer i1, i2;
    static logical in;

/* *********************************************************************** */
/*  FUNCTION LPNTIN = .TRUE. IF THE POINT IS WITHIN THE PERIMETER */
/* *********************************************************************** */
    /* Parameter adjustments */
    --cy;
    --cx;

    /* Function Body */
    i1 = 1;
    in = TRUE_;
L100:
    if (i1 <= *npnt && in) {
	if (i1 == *npnt) {
	    i2 = 1;
	} else {
	    i2 = i1 + 1;
	}
/*  CHECK FOR VERTICAL LINE */
	if ((r__1 = cx[i1] - cx[i2], dabs(r__1)) < (float)1e-6) {
	    if (cy[i1] < cy[i2]) {
		in = *x < cx[i2];
	    } else {
		in = *x > cx[i2];
	    }
/*  CHECK FOR HORIZONTAL LINE */
	} else if ((r__1 = cy[i1] - cy[i2], dabs(r__1)) < (float)1e-6) {
	    if (cx[i1] < cx[i2]) {
		in = *y > cy[i2];
	    } else {
		in = *y < cy[i2];
	    }
/*  MUST BE INCLINED LINE */
	} else {
	    u = (*x - cx[i1]) / (cx[i2] - cx[i1]);
	    u = dmin(u,(float)1.);
	    u = dmax((float)0.,u);
	    v = (*y - cy[i1]) / (cy[i2] - cy[i1]);
	    v = dmin(v,(float)1.);
	    v = dmax((float)0.,v);
	    if (cx[i1] < cx[i2]) {
		if (cy[i1] < cy[i2]) {
		    in = u <= v;
		} else {
		    in = u >= v;
		}
	    } else {
		if (cy[i1] > cy[i2]) {
		    in = u <= v;
		} else {
		    in = u >= v;
		}
	    }
	}
	++i1;
	goto L100;
    }
    ret_val = in;
    return ret_val;
} /* lpntin_ */

#ifdef __cplusplus
	}
#endif

/* mixvar.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int mixvar_(integer *npnode, real *bmesur, real *dmesur, 
	logical *second, logical *terror, real *emin, real *emax, real *emins,
	 real *emaxs, real *e1s)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Local variables */
    static integer i__;
    static real x1, x2, y1, y2, ef1, rmax;
    extern /* Subroutine */ int normnd_(integer *, real *, real *);
    static logical trilin;

/* ** MODIFIED BY: TED BLACKER */
/* ** MODIFICATION DATE: 7/31/90 */
/* ** MODIFICATION: PASSED THE EMAX AND EMIN VARIABLES BACK OUT FOR USE */
/* ** MODIFIED BY: TED BLACKER */
/* ** MODIFICATION DATE: 8/2/90 */
/* ** MODIFICATION: PASSED THE EMAX AND EMIN VARIABLES IN NOW AS WELL */
/* *********************************************************************** */
/*  SUBROUTINE MIXVAR = MIXES THE NODAL VARIABLES TO BE USED IN THE */
/*                      REMESHING USING PAVING. */
/* *********************************************************************** */
    /* Parameter adjustments */
    --dmesur;
    --bmesur;

    /* Function Body */
    trilin = TRUE_;
    if (*second) {
/*  NORMALIZE THE NODE VARIABLES */
	if (trilin) {
/*  WEIGHT THE ERROR MEASURE (BMESUR) BY A */
/*  TRILINEAR STRESS MEASURE (DMESUR) FUNCTION */
	    rmax = (float)1.;
	    normnd_(npnode, &bmesur[1], &rmax);
	    normnd_(npnode, &dmesur[1], &rmax);
	    x1 = (float).3;
	    x2 = (float).7;
	    y1 = (float).3;
	    y2 = (float)1.;
	    i__1 = *npnode;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (dmesur[i__] <= x1) {
		    bmesur[i__] *= y1;
		} else if (dmesur[i__] <= x2) {
		    bmesur[i__] *= (y2 - y1) * (dmesur[i__] - x1) / (x2 - x1) 
			    + y1;
		}
/* L100: */
	    }
/*  NOW (RE)NORMALIZE THE ERROR (BMESUR) VARIABLE */
	    rmax = (float).6;
	    normnd_(npnode, &bmesur[1], &rmax);
	} else {
	    rmax = (float).6;
	    normnd_(npnode, &bmesur[1], &rmax);
	    normnd_(npnode, &dmesur[1], &rmax);
	    i__1 = *npnode;
	    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
		r__1 = bmesur[i__], r__2 = dmesur[i__];
		bmesur[i__] = dmax(r__1,r__2);
/* L110: */
	    }
	}
/* **               THE SIZE FACTOR TO BE 1.0 AT AN ERROR EQUAL TO */
/* **               THE TARGET ERROR. */
/* **               INSTEAD OF 1/7. */
/*  NOW NORMALIZE THE ERROR (BMESUR) VARIABLE */
    } else if (*terror) {
/* ** MODIFIED BY: TED BLACKER */
/* ** MODIFICATION DATE: 7/31/90 */
/* ** MODIFICATION: SET E1S TO BE .2 * TERR (WAS JUST TERR) AND SET */
/* **               EMAXS TO BE 1.5 * TERR (WAS 3.0 * TERR).  THIS IS */
/* **               CONSISTENT WITH THE TEST RUNS DONE ON HOW AGGRESSIVE THE */
/* **               ERROR MEASURE NEEDED TO BE IN ORDER TO GET TO A TARGET */
/* **               ERROR. */
	ef1 = (*emax - (float)1.) / (*emax - *emin);
	i__1 = *npnode;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (bmesur[i__] <= *emins) {
		bmesur[i__] = (float)0.;
	    } else if (bmesur[i__] <= *e1s) {
		bmesur[i__] = ef1 * (bmesur[i__] - *emins) / (*e1s - *emins);
	    } else if (bmesur[i__] < *emaxs) {
		bmesur[i__] = ef1 + ((float)1. - ef1) * (bmesur[i__] - *e1s) /
			 (*emaxs - *e1s);
	    } else {
		bmesur[i__] = (float)1.;
	    }
/* L120: */
	}
/*  NORMALIZE THE ERROR (BMESUR) VARIABLE */
    } else {
	rmax = (float).6;
	normnd_(npnode, &bmesur[1], &rmax);
    }
    return 0;
} /* mixvar_ */

#ifdef __cplusplus
	}
#endif

/* spaced.f -- translated by f2c (version 20160102).
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
#include "paving.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int spaced_(integer *mxnd, integer *mxcorn, integer *mln, 
        integer *ilen, integer *ncorn, integer *lcorn, integer *lnodes, 
        integer *icomb, integer *itest, integer *ltest, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;

    /* Local variables */
    static integer i__, klen, ihold;

    static integer kountc;

/* *********************************************************************** */
/*  SUBROUTINE SPACED = COUNTS THE INTERVAL SPACINGS FOR A COMBINATION */
/* *********************************************************************** */
    /* Parameter adjustments */
    --icomb;
    --lcorn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    --ltest;
    --itest;

    /* Function Body */
    *err = TRUE_;
    klen = 0;
    kountc = 0;
    i__1 = *ncorn;
    for (i__ = 1; i__ <= i__1; ++i__) {
        if (icomb[i__] == 1) {
            ++klen;
            if (klen > *ilen) {
                fprintf(stderr,"PROBLEMS IN SPACED - COUNTERS DON'T MATCH DATA");
                return 0;
            }
            itest[klen] = lcorn[i__];
            ltest[klen] = kountc;
            kountc = lnodes[lcorn[i__] * lnodes_dim1 + 7];
        } else {
            kountc += lnodes[lcorn[i__] * lnodes_dim1 + 7];
        }
/* L100: */
    }
/*  NOW ADD THE REMAINING KOUNTC ONTO THE FRONT */
    ltest[1] += kountc;
/*  NOW SWITCH THE COUNTS TO BE FOLLOWING THE CORNERS INSTEAD OF */
/*  BEFORE THE CORNERS */
    ihold = ltest[1];
    i__1 = klen;
    for (i__ = 2; i__ <= i__1; ++i__) {
        ltest[i__ - 1] = ltest[i__];
/* L110: */
    }
    ltest[klen] = ihold;
    *err = FALSE_;
    return 0;
} /* spaced_ */

#ifdef __cplusplus
        }
#endif

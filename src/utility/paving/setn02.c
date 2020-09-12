/* setn02.f -- translated by f2c (version 20160102).
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
//#include "f2c.h"
#include "paving.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int setn02_(integer *mxnd, integer *nxl, integer *lxk, 
        integer *kxl, integer *line, integer *nend, integer *node, integer *
        n0, integer *n2)
{
    static integer i__, i0, i2, k1, k2, l1, l0, l2, il, nnew1, nnew2;


/* *********************************************************************** */
/*  SUBROUTINE SETN02 = PICKS THE NEXT LINE AROUND THE ELEMENTS ATTACHED */
/*                      TO LINE WITH ONE END AT NEND, AND THE OTHER END */
/*                      NOT AT NODE, AND FROM THE CONNECTIVITY OF THE */
/*                      ELEMENTS DETERMINES THE BOUNDING ANGULAR LINES */
/*                      AND NODES. */
/* *********************************************************************** */
    /* Parameter adjustments */
    kxl -= 3;
    lxk -= 5;
    nxl -= 3;

    /* Function Body */
    k1 = kxl[(*line << 1) + 1];
    k2 = kxl[(*line << 1) + 2];
/*  FIND THE NEXT LINE IN K1 */
    for (i__ = 1; i__ <= 4; ++i__) {
        il = lxk[i__ + (k1 << 2)];
        if (nxl[(il << 1) + 1] == *nend && nxl[(il << 1) + 2] != *node) {
            l1 = il;
            nnew1 = nxl[(il << 1) + 2];
            goto L110;
        } else if (nxl[(il << 1) + 2] == *nend && nxl[(il << 1) + 1] != *node)
                 {
            l1 = il;
            nnew1 = nxl[(il << 1) + 1];
            goto L110;
        }
/* L100: */
    }
    fprintf(stderr,"** PROBLEMS IN SETN02 FINDING NNEW1 **");
    return 0;
L110:
/*  FIND THE NEXT LINE IN K2 */
    for (i__ = 1; i__ <= 4; ++i__) {
        il = lxk[i__ + (k2 << 2)];
        if (nxl[(il << 1) + 1] == *nend && nxl[(il << 1) + 2] != *node) {
            nnew2 = nxl[(il << 1) + 2];
            goto L130;
        } else if (nxl[(il << 1) + 2] == *nend && nxl[(il << 1) + 1] != *node)
                 {
            nnew2 = nxl[(il << 1) + 1];
            goto L130;
        }
/* L120: */
    }
    fprintf(stderr,"** PROBLEMS IN SETN02 FINDING NNEW2 **");
    return 0;
L130:
/*  NOW DETERMINE WHICH OF THESE NODES IS N0 AND WHICH IS N2 BASED */
/*  ON THE FACT THAT THE CONNECTIVITY OF THE ELEMENTS LINES IS ALWAYS IN */
/*  COUNTER-CLOCKWISE ORDER */
    for (i__ = 1; i__ <= 4; ++i__) {
        if (lxk[i__ + (k1 << 2)] == *line) {
            i0 = i__ - 1;
            i2 = i__ + 1;
            if (i__ == 1) {
                i0 = 4;
            } else if (i__ == 4) {
                i2 = 1;
            }
            l0 = lxk[i0 + (k1 << 2)];
            l2 = lxk[i2 + (k1 << 2)];
            if (l0 == l1) {
                *n0 = nnew1;
                *n2 = nnew2;
            } else if (l2 == l1) {
                *n0 = nnew2;
                *n2 = nnew1;
            } else {
                fprintf(stderr,"** PROBLEMS IN SETN02 FINDING N0 AND N2 **");
            }
            goto L150;
        }
/* L140: */
    }
    fprintf(stderr,"** PROBLEMS IN SETN02 FINDING LINE AGAIN **");
L150:
    return 0;
} /* setn02_ */

#ifdef __cplusplus
        }
#endif

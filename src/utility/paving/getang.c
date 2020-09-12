/* getang.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getang_(integer *mxnd, integer *mln, xc_float *xn, xc_float *yn, 
        integer *lnodes, integer *lxk, integer *kxl, integer *nxl, integer *
        lxn, integer *i__, integer *j, integer *k, xc_float *angle, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;


    /* Local variables */
    static integer i1, k1, l1, l2, k2, l3, k3;
    static xc_float v1, v2;
    static integer ii, kk1;
    static xc_float vvj, vvi1, vvk1;
    static integer iopp, kopp, iopp3;
    static xc_float v1opp, v2opp, v3opp;
    extern logical sidep_(xc_float *), cornp_(xc_float *);
    static integer ltest;
    static xc_float twopi, angle1, angle2;
    extern logical disctp_(xc_float *);

/* *********************************************************************** */
/* SUBROUTINE GETANG = RETURNS THE CCW ANGLE FROM A VECTOR DRAWN */
/*                     FROM NODE J TO K TO A VECTOR DRAWN */
/*                      FROM NODE J TO I */
/* *********************************************************************** */
/* ... The save statement was added during debugging on the AMD Opteron */
/*     system using the pgf77 5.1-3 compiler.  Without the save, the */
/*     code coredumps at line 120 if optimized....  Could not track */
/*     down a problem, but adding the SAVE did work... */
    /* Parameter adjustments */
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    --xn;
    --yn;
    lxk -= 5;
    kxl -= 3;
    nxl -= 3;
    lxn -= 5;

    /* Function Body */
    twopi = 2.0*M_PI;
    iopp = 0;
    kopp = 0;
    i1 = 0;
    kk1 = 0;
/*  CHECK FOR NODES ON TOP OF EACH OTHER */
    if (xn[*j] == xn[*k] && yn[*j] == yn[*k] || xn[*i__] == xn[*j] && yn[*i__]
             == yn[*j] || xn[*i__] == xn[*k] && yn[*i__] == yn[*k]) {
        *angle = (xc_float)0.;
        goto L220;
    }
    v1 = atan2(yn[*k] - yn[*j], xn[*k] - xn[*j]);
    if (v1 < (xc_float)0.) {
        v1 += twopi;
    }
    v2 = atan2(yn[*i__] - yn[*j], xn[*i__] - xn[*j]);
    if (v2 < (xc_float)0.) {
        v2 += twopi;
    }
    *angle = v2 - v1;
    if (*angle < (xc_float)0.) {
        *angle += twopi;
    }
/*  NOW CHECK TO MAKE SURE THAT THE ANGLE HAS NOT CROSSED THE PREVIOUS */
/*  ELEMENTS SIDES */
    l1 = lnodes[*i__ * lnodes_dim1 + 5];
    l2 = lnodes[*j * lnodes_dim1 + 5];
    k1 = kxl[(l1 << 1) + 1];
    k2 = kxl[(l2 << 1) + 1];
    if (k1 == k2) {
        goto L210;
    }
/*  SEE IF L2 CROSSES INTO K1 - FIRST GET THE NODE OPPOSITE I */
/*  AND THEN CHECK THE ANGLE FROM VECTOR J TO K AND VECTOR */
/*  J TO IOPP AGAINST THE INTERNAL ANGLE - SMALLER AND IT HAS */
/* CROSSED OVER. */
    if (k1 != 0) {
        for (ii = 1; ii <= 4; ++ii) {
            ltest = lxk[ii + (k1 << 2)];
            if (ltest != l1) {
                if (nxl[(ltest << 1) + 1] == *j) {
                    iopp = nxl[(ltest << 1) + 2];
                    l3 = ltest;
                    goto L110;
                } else if (nxl[(ltest << 1) + 2] == *j) {
                    iopp = nxl[(ltest << 1) + 1];
                    l3 = ltest;
                    goto L110;
                }
            }
/* L100: */
        }
        fprintf(stderr,"** PROBLEMS IN GETANG GETTING IOPP **\n");
        *err = TRUE_;
        goto L220;
L110:
/*  NOW TEST FOR CROSS-OVER */
        v2opp = atan2(yn[iopp] - yn[*j], xn[iopp] - xn[*j]);
        if (v2opp < (xc_float)0.) {
            v2opp += twopi;
        }
        angle1 = v2opp - v2;
        if (angle1 < (xc_float)0.) {
            angle1 += twopi;
        }
        angle2 = v2opp - v1;
        if (angle2 < (xc_float)0.) {
            angle2 += twopi;
        }
        if (angle2 <= angle1) {
            *angle -= twopi;
            goto L210;
        }
    }
/*  SEE IF L2 CROSSES INTO K1 - FIRST GET THE NODE OPPOSITE K */
/*  AND THEN CHECK THE ANGLE FROM VECTOR J TO K AND VECTOR */
/*  J TO IOPP AGAINST THE INTERNAL ANGLE - SMALLER AND IT HAS */
/*  CROSSED OVER. */
L120:
    if (k2 == 0) {
        goto L210;
    }
    for (ii = 1; ii <= 4; ++ii) {
        ltest = lxk[ii + (k2 << 2)];
        if (ltest != l2) {
            if (nxl[(ltest << 1) + 1] == *j) {
                kopp = nxl[(ltest << 1) + 2];
                goto L140;
            } else if (nxl[(ltest << 1) + 2] == *j) {
                kopp = nxl[(ltest << 1) + 1];
                goto L140;
            }
        }
/* L130: */
    }
    fprintf(stderr,"** PROBLEMS IN GETANG GETTING KOPP **\n");
    *err = TRUE_;
    goto L220;
L140:
/*  NOW TEST FOR CROSS-OVER */
    v1opp = atan2(yn[kopp] - yn[*j], xn[kopp] - xn[*j]);
    if (v1opp < (xc_float)0.) {
        v1opp += twopi;
    }
    angle1 = v1 - v1opp;
    if (angle1 < (xc_float)0.) {
        angle1 += twopi;
    }
    angle2 = v2 - v1opp;
    if (angle2 < (xc_float)0.) {
        angle2 += twopi;
    }
    if (angle2 <= angle1) {
        *angle -= twopi;
        goto L210;
    }
/*  NOW CHECK TO MAKE SURE THAT THE ANGLE HAS NOT CROSSED OVER TWO */
/*  ELEMENT SIDES IF THE NODE IS ATTACHED TO 5 OR MORE LINES */
    if (lxn[(*j << 2) + 4] < 0) {
        k3 = kxl[(l3 << 1) + 1] + kxl[(l3 << 1) + 2] - k1;
        if (k3 == k2) {
            goto L210;
        }
/*  SEE IF L2 CROSSES INTO K3 - FIRST GET THE NODE OPPOSITE J */
/*  AND THEN CHECK THE ANGLE FROM VECTOR J TO K AND VECTOR */
/*  J TO IOPP AGAINST THE INTERNAL ANGLE - SMALLER AND IT HAS */
/* CROSSED OVER. */
        if (k3 == 0) {
            goto L120;
        }
        for (ii = 1; ii <= 4; ++ii) {
            ltest = lxk[ii + (k3 << 2)];
            if (ltest != l3) {
                if (nxl[(ltest << 1) + 1] == *j) {
                    iopp3 = nxl[(ltest << 1) + 2];
                    goto L160;
                } else if (nxl[(ltest << 1) + 2] == *j) {
                    iopp3 = nxl[(ltest << 1) + 1];
                    goto L160;
                }
            }
/* L150: */
        }
        fprintf(stderr,"** PROBLEMS IN GETANG GETTING IOPP3 **\n");
        *err = TRUE_;
        goto L220;
L160:
/*  NOW TEST FOR CROSS-OVER */
        v3opp = atan2(yn[iopp3] - yn[*j], xn[iopp3] - xn[*j]);
        if (v3opp < (xc_float)0.) {
            v3opp += twopi;
        }
        angle1 = v3opp - v2;
        if (angle1 < (xc_float)0.) {
            angle1 += twopi;
        }
        angle2 = v3opp - v1;
        if (angle2 < (xc_float)0.) {
            angle2 += twopi;
        }
        if (angle2 <= angle1) {
            *angle -= twopi;
            goto L210;
        }
    }
/*  NOW CHECK FOR AN INVERTED THREE NODE ANGLE - VERY SPECIAL */
/*  CASE THAT FALLS THROUGH THE PREVIOUS CHECK */
    if (kopp == iopp) {
        for (ii = 1; ii <= 4; ++ii) {
            ltest = lxk[ii + (k1 << 2)];
            if (nxl[(ltest << 1) + 1] == iopp && nxl[(ltest << 1) + 2] != *j) 
                    {
                i1 = nxl[(ltest << 1) + 2];
                goto L180;
            } else if (nxl[(ltest << 1) + 2] == iopp && nxl[(ltest << 1) + 1] 
                    != *j) {
                i1 = nxl[(ltest << 1) + 1];
                goto L180;
            }
/* L170: */
        }
        fprintf(stderr,"** PROBLEMS IN GETANG GETTING I1 **\n");
        goto L220;
L180:
        for (ii = 1; ii <= 4; ++ii) {
            ltest = lxk[ii + (k2 << 2)];
            if (nxl[(ltest << 1) + 1] == kopp && nxl[(ltest << 1) + 2] != *j) 
                    {
                kk1 = nxl[(ltest << 1) + 2];
                goto L200;
            } else if (nxl[(ltest << 1) + 2] == kopp && nxl[(ltest << 1) + 1] 
                    != *j) {
                kk1 = nxl[(ltest << 1) + 1];
                goto L200;
            }
/* L190: */
        }
        fprintf(stderr,"** PROBLEMS IN GETANG KK1 **\n");
        goto L220;
L200:
/*  NOW TEST FOR INVERSION */
        vvj = atan2(yn[*j] - yn[kopp], xn[*j] - xn[kopp]);
        if (vvj < (xc_float)0.) {
            vvj += twopi;
        }
        vvi1 = atan2(yn[i1] - yn[kopp], xn[i1] - xn[kopp]);
        if (vvi1 < (xc_float)0.) {
            vvi1 += twopi;
        }
        vvk1 = atan2(yn[kk1] - yn[kopp], xn[kk1] - xn[kopp]);
        if (vvk1 < (xc_float)0.) {
            vvk1 += twopi;
        }
        angle1 = vvi1 - vvk1;
        if (angle1 < (xc_float)0.) {
            angle1 += twopi;
        }
        angle2 = vvj - vvk1;
        if (angle2 < (xc_float)0.) {
            angle2 += twopi;
        }
        if (angle2 > angle1) {
            *angle -= twopi;
        }
    }
/*  GET THE RIGHT CLASSIFICATION */
L210:
    if (cornp_(angle)) {
        if (sidep_(angle)) {
            lnodes[*j * lnodes_dim1 + 6] = 2;
        } else {
            lnodes[*j * lnodes_dim1 + 6] = 1;
        }
    } else if (sidep_(angle)) {
        if (disctp_(angle)) {
            lnodes[*j * lnodes_dim1 + 6] = 4;
        } else {
            lnodes[*j * lnodes_dim1 + 6] = 3;
        }
    } else {
        lnodes[*j * lnodes_dim1 + 6] = 5;
    }
L220:
    return 0;
} /* getang_ */

#ifdef __cplusplus
        }
#endif

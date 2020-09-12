/* longel.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int longel_(integer *mxnd, integer *mln, integer *lnodes, 
        real *xn, real *yn, integer *nuid, integer *lxk, integer *kxl, 
        integer *nxl, integer *lxn, integer *nnn, integer *navail, integer *
        iavail, integer *node, integer *kelem, real *ang, real *toler, 
        integer *n1, integer *n2, integer *kreg, real *xmin, real *xmax, real 
        *ymin, real *ymax, integer *kkk, integer *lll, logical *done, logical 
        *graph, logical *video, logical *noroom, logical *err, integer *
        kkkadd)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, l1, l2, l4, l3, l5;
    static real d12, d22;
    static integer il;
    static real dn1, dn2, dl3, dl4, dl5;
    static integer il1, il2;
    static logical ccw;
    static real area, dmin__, dmax__;
    static integer node2, node3, nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
            integer *, real *, integer *, integer *, logical *);
    static integer ltest;
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
            integer *);
    static integer kelem2;
    extern /* Subroutine */ int  undelm_(integer *, 
            integer *, integer *, real *, real *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, logical *, 
            logical *, logical *, logical *), lcolor_(char *, ftnlen), 
            sflush_();

/* *********************************************************************** */
/*  SUBROUTINE LONGEL = AN ELONGATED ELEMENT OVER 150 DEGREES GETS A */
/*                      3 ELEMENT REPLACEMENT FOR THE TWO ELEMENTS THERE */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    ccw = TRUE_;
/*  SEE IF THE ANGLE IS WITHIN BOUNDS */
    if (*ang > *toler) {
        gnxka_(mxnd, &xn[1], &yn[1], kelem, nodes, &area, &lxk[5], &nxl[3], &
                ccw);
        node2 = nodes[0] + nodes[1] + nodes[2] + nodes[3] - *node - *n1 - *n2;
/* Computing 2nd power */
        r__1 = xn[node2] - xn[*n1];
/* Computing 2nd power */
        r__2 = yn[node2] - yn[*n1];
        d12 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
        r__1 = xn[node2] - xn[*n2];
/* Computing 2nd power */
        r__2 = yn[node2] - yn[*n2];
        d22 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
        r__1 = xn[*node] - xn[*n1];
/* Computing 2nd power */
        r__2 = yn[*node] - yn[*n1];
        dn1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
        r__1 = xn[*node] - xn[*n2];
/* Computing 2nd power */
        r__2 = yn[*node] - yn[*n2];
        dn2 = sqrt(r__1 * r__1 + r__2 * r__2);
        dmin__ = (dn1 + dn2) * (float).5 * (float)1.7;
        dmax__ = (dn1 + dn2) * (float).5;
/*  SEE IF IT IS A LONG LEGGED BEAST */
        if (d12 > dmin__ || d22 > dmin__) {
/*  FIND L1, L2, L3, AND L4 */
            for (i__ = 1; i__ <= 4; ++i__) {
                ltest = lxk[i__ + (*kelem << 2)];
                if (nxl[(ltest << 1) + 1] == *node && nxl[(ltest << 1) + 2] ==
                         *n1 || nxl[(ltest << 1) + 2] == *node && nxl[(ltest 
                        << 1) + 1] == *n1) {
                    l1 = ltest;
                    goto L110;
                }
/* L100: */
            }
            fprintf(stderr,"** PROBLEMS IN LONGEL FINDING L1 **");
            *err = TRUE_;
            goto L250;
L110:
            for (i__ = 1; i__ <= 4; ++i__) {
                ltest = lxk[i__ + (*kelem << 2)];
                if (nxl[(ltest << 1) + 1] == *node && nxl[(ltest << 1) + 2] ==
                         *n2 || nxl[(ltest << 1) + 2] == *node && nxl[(ltest 
                        << 1) + 1] == *n2) {
                    l2 = ltest;
                    goto L130;
                }
/* L120: */
            }
            fprintf(stderr,"** PROBLEMS IN LONGEL FINDING L2 **");
            *err = TRUE_;
            goto L250;
L130:
            for (i__ = 1; i__ <= 4; ++i__) {
                ltest = lxk[i__ + (*kelem << 2)];
                if (nxl[(ltest << 1) + 1] == node2 && nxl[(ltest << 1) + 2] ==
                         *n1 || nxl[(ltest << 1) + 2] == node2 && nxl[(ltest 
                        << 1) + 1] == *n1) {
                    if (d12 > d22) {
                        l4 = ltest;
                    } else {
                        l3 = ltest;
                    }
                    goto L150;
                }
/* L140: */
            }
            fprintf(stderr,"** PROBLEMS IN LONGEL FINDING L4/L3 **");
            *err = TRUE_;
            goto L250;
L150:
            for (i__ = 1; i__ <= 4; ++i__) {
                ltest = lxk[i__ + (*kelem << 2)];
                if (nxl[(ltest << 1) + 1] == node2 && nxl[(ltest << 1) + 2] ==
                         *n2 || nxl[(ltest << 1) + 2] == node2 && nxl[(ltest 
                        << 1) + 1] == *n2) {
                    if (d12 > d22) {
                        l3 = ltest;
                    } else {
                        l4 = ltest;
                    }
                    goto L170;
                }
/* L160: */
            }
            fprintf(stderr,"** PROBLEMS IN LONGEL FINDING L3/L4 **");
            *err = TRUE_;
            goto L250;
L170:
/*  NOW FIND KELEM2 */
            kelem2 = kxl[(l4 << 1) + 1] + kxl[(l4 << 1) + 2] - *kelem;
            if (kelem2 == 0) {
                goto L250;
            }
/*  NOW FIND NODE3 - THE NODE THAT WILL BE PART OF THE NEWLY */
/*  FORMED ELEMENT */
            gnxka_(mxnd, &xn[1], &yn[1], &kelem2, nodes, &area, &lxk[5], &nxl[
                    3], &ccw);
            if (d12 > d22) {
                for (i__ = 1; i__ <= 4; ++i__) {
                    if (nodes[i__ - 1] == *n1) {
                        if (i__ == 1) {
                            node3 = nodes[3];
                        } else {
                            node3 = nodes[i__ - 2];
                        }
                        goto L190;
                    }
/* L180: */
                }
                fprintf(stderr,"** PROBLEMS IN LONGEL FINDING NODE3/D11 **");
                *err = TRUE_;
                goto L250;
L190:
                ;
            } else {
                for (i__ = 1; i__ <= 4; ++i__) {
                    if (nodes[i__ - 1] == *n2) {
                        if (i__ == 4) {
                            node3 = nodes[0];
                        } else {
                            node3 = nodes[i__];
                        }
                        goto L210;
                    }
/* L200: */
                }
                fprintf(stderr,"** PROBLEMS IN LONGEL FINDING NODE3/D11 **");
                *err = TRUE_;
                goto L250;
L210:
                ;
            }
/*  NOW FIND L5 */
            for (i__ = 1; i__ <= 4; ++i__) {
                ltest = lxk[i__ + (kelem2 << 2)];
                if (d12 > d22) {
                    if (nxl[(ltest << 1) + 1] == node3 && nxl[(ltest << 1) + 
                            2] == *n1 || nxl[(ltest << 1) + 1] == *n1 && nxl[(
                            ltest << 1) + 2] == node3) {
                        l5 = ltest;
                        goto L230;
                    }
                } else {
                    if (nxl[(ltest << 1) + 1] == node3 && nxl[(ltest << 1) + 
                            2] == *n2 || nxl[(ltest << 1) + 1] == *n2 && nxl[(
                            ltest << 1) + 2] == node3) {
                        l5 = ltest;
                        goto L230;
                    }
                }
/* L220: */
            }
            fprintf(stderr,"** PROBLEMS IN LONGEL FINDING L5 **");
            *err = TRUE_;
            goto L250;
L230:
/*  NOW CHECK TO SEE IF IT MAKES SENSE TO ADD THE EXTRA ELEMENT TO */
/*  IMPROVE AN ELONGATED ONE */
/* Computing 2nd power */
            r__1 = xn[nxl[(l3 << 1) + 1]] - xn[nxl[(l3 << 1) + 2]];
/* Computing 2nd power */
            r__2 = yn[nxl[(l3 << 1) + 1]] - yn[nxl[(l3 << 1) + 2]];
            dl3 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
            r__1 = xn[nxl[(l4 << 1) + 1]] - xn[nxl[(l4 << 1) + 2]];
/* Computing 2nd power */
            r__2 = yn[nxl[(l4 << 1) + 1]] - yn[nxl[(l4 << 1) + 2]];
            dl4 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
            r__1 = xn[nxl[(l5 << 1) + 1]] - xn[nxl[(l5 << 1) + 2]];
/* Computing 2nd power */
            r__2 = yn[nxl[(l5 << 1) + 1]] - yn[nxl[(l5 << 1) + 2]];
            dl5 = sqrt(r__1 * r__1 + r__2 * r__2);
            if (dl3 > dmax__ || dl5 > dl4) {
                goto L250;
            }
/*  ADD THE EXTRA ELEMENT */
            if (*graph) {
                lcolor_("PINK ", (ftnlen)5);
                for (il = 1; il <= 4; ++il) {
                    il1 = nxl[(lxk[il + (*kelem << 2)] << 1) + 1];
                    il2 = nxl[(lxk[il + (*kelem << 2)] << 1) + 2];
                    d2node_(mxnd, &xn[1], &yn[1], &il1, &il2);
                    il1 = nxl[(lxk[il + (kelem2 << 2)] << 1) + 1];
                    il2 = nxl[(lxk[il + (kelem2 << 2)] << 1) + 2];
                    d2node_(mxnd, &xn[1], &yn[1], &il1, &il2);
/* L240: */
                }
                lcolor_("WHITE", (ftnlen)5);
                sflush_();
            }
            if (d12 > d22 && lxn[(*n1 << 2) + 4] != 0) {
                undelm_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &
                        nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], nnn, lll,
                         kkk, navail, iavail, &node2, &node3, n1, node, &l5, &
                        l1, &l4, kelem, &kelem2, noroom, err, graph, video);
                if (*err || *done) {
                    goto L250;
                }
                *done = TRUE_;
            } else if (lxn[(*n2 << 2) + 4] != 0) {
                undelm_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &
                        nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], nnn, lll,
                         kkk, navail, iavail, &node2, node, n2, &node3, &l2, &
                        l5, &l4, &kelem2, kelem, noroom, err, graph, video);
                if (*err || *done) {
                    goto L250;
                }
                *done = TRUE_;
            }
            *kkkadd = *kkk;
        }
    }
L250:
    return 0;
} /* longel_ */

#ifdef __cplusplus
        }
#endif

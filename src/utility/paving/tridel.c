/* tridel.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__3 = 3;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int tridel_(integer *mxnd, integer *mln, real *xn, real *yn, 
        real *zn, integer *nuid, integer *lxk, integer *kxl, integer *nxl, 
        integer *lxn, integer *nnn, integer *lll, integer *kkk, integer *
        navail, integer *iavail, real *angle, integer *lnodes, real *bnsize, 
        integer *nloop, char *dev1, integer *kreg, real *xmin, real *xmax, 
        real *ymin, real *ymax, real *zmin, real *zmax, logical *graph, 
        logical *video, logical *noroom, logical *err, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;
    real r__1, r__2, r__3, r__4, r__5, r__6, r__7, r__8;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal), acos(doublereal);

    /* Local variables */
    static integer i__, j, k[3], l1, l2, l3, n1, n2, n3, n4;
    static real d21, d32, d14, d43, pi, x21, y21, x32, x14, x43, y32, y43, 
            y14;
    static logical ccw;
    static real ang1, ang2, ang3, area, ang12, ang23, ang31;
    static logical done;
    static integer node;
    static logical redo;
    static real dmax__;
    static integer kmax;
    static real dist;
    static integer node1, node3;
    static logical check;
    extern /* Subroutine */ int delem_(integer *, real *, real *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, logical *, 
            logical *, logical *, logical *);
    static integer kelem, nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
            integer *, real *, integer *, integer *, logical *);
    static integer kount;
    static real twopi;
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
            integer *);
    static real theta1, theta2, theta3, theta4, toler1, toler2, toler3, 
            toler4, toler5, toler6;
    static integer kkkadd;
    extern /* Subroutine */ int  adjtri_(integer *, 
            integer *, integer *, real *, real *, real *, integer *, integer *
            , integer *, integer *, integer *, integer *, integer *, integer *
            , integer *, integer *, real *, real *, real *, integer *, 
            integer *, integer *, integer *, real *, real *, real *, real *, 
            real *, real *, integer *, integer *, char *, logical *, logical *
            , logical *, logical *, logical *, logical *, integer *, ftnlen);
    static logical passed;
    extern /* Subroutine */ int filsmo_(integer *, integer *, real *, real *, 
            real *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, real *, integer *, real *, real *
            , real *, real *, real *, real *, char *, integer *, ftnlen), 
            marksm_(integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, logical *), lcolor_(char *, ftnlen), snapit_(integer *), sflush_(), nxkord_(integer *, 
            integer *), trifix_(integer *, integer *, real *, real *, real *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, real *, integer *, 
            real *, integer *, char *, integer *, real *, real *, real *, 
            real *, real *, real *, logical *, logical *, logical *, logical *
            , ftnlen), rplotl_(integer *, real *, real *, real *, integer *, 
            real *, real *, real *, real *, real *, real *, integer *, char *,
             integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE TRIDEL = CHECKS FOR ANY TRIANGULAR SHAPED QUADS ATTACHED */
/*                      TO A THREE NODE ELEMENT AND DELETES THEM WHEN */
/*                      FOUND AND POSSIBLE */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --zn;
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    twopi = pi * (float)2.;
    *err = FALSE_;
    done = FALSE_;
    check = TRUE_;
    ccw = TRUE_;
    kmax = 30;
    kount = 0;
    kkkadd = 0;
L100:
    ++kount;
    if (kount > kmax) {
        goto L180;
    }
L110:
    redo = FALSE_;
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
        if (lxn[(i__ << 2) + 1] > 0 && lxn[(i__ << 2) + 2] > 0 && lxn[(i__ << 
                2) + 4] == 0) {
/*  SEE IF A 2-LINE NODE NEEDS DELETED */
            if (lxn[(i__ << 2) + 3] <= 0) {
                node = i__;
                kelem = kxl[(lxn[(node << 2) + 1] << 1) + 1];
                check = FALSE_;
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &node, err);
                if (*err) {
                    goto L180;
                }
                delem_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[
                        3], &lxn[5], nnn, navail, iavail, &node, &kelem, &
                        node1, &node3, &done, &check, noroom, err);
                if (*noroom || *err) {
                    goto L180;
                }
                if (done) {
                    if (*video) {
                        rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, 
                                xmax, ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
                        snapit_(&c__3);
                    }
                    filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[
                            3], &nxl[3], &lxn[5], lll, nnn, nnn, &lnodes[
                            lnodes_offset], &bnsize[3], nloop, xmin, xmax, 
                            ymin, ymax, zmin, zmax, dev1, kreg, (ftnlen)3);
                    if (*graph || *video) {
                        rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, 
                                xmax, ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
                        if (*video) {
                            snapit_(&c__3);
                        }
                    }
                    redo = TRUE_;
                }
            }
        }
/* L120: */
    }
    if (redo) {
        goto L110;
    }
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
        if (lxn[(i__ << 2) + 1] > 0 && lxn[(i__ << 2) + 2] > 0 && lxn[(i__ << 
                2) + 4] == 0) {
/*  GET THE ATTACHED LINES AND ELEMENTS */
/*  K(1) IS BETWEEN L1 & L2 */
/*  K(2) IS BETWEEN L2 & L3 */
/*  K(3) IS BETWEEN L3 & L1 */
            l1 = lxn[(i__ << 2) + 1];
            l2 = lxn[(i__ << 2) + 2];
            l3 = lxn[(i__ << 2) + 3];
            n1 = nxl[(l1 << 1) + 1] + nxl[(l1 << 1) + 2] - i__;
            n2 = nxl[(l2 << 1) + 1] + nxl[(l2 << 1) + 2] - i__;
            n3 = nxl[(l3 << 1) + 1] + nxl[(l3 << 1) + 2] - i__;
            if (kxl[(l1 << 1) + 1] == kxl[(l2 << 1) + 1] || kxl[(l1 << 1) + 1]
                     == kxl[(l2 << 1) + 2]) {
                k[0] = kxl[(l1 << 1) + 1];
            } else if (kxl[(l1 << 1) + 2] == kxl[(l2 << 1) + 1] || kxl[(l1 << 
                    1) + 2] == kxl[(l2 << 1) + 2]) {
                k[0] = kxl[(l1 << 1) + 2];
            } else {
                *err = TRUE_;
                fprintf(stderr,"** PROBLEMS IN TRIDEL FINDING K(1) **");
                goto L180;
            }
            if (kxl[(l2 << 1) + 1] == kxl[(l3 << 1) + 1] || kxl[(l2 << 1) + 1]
                     == kxl[(l3 << 1) + 2]) {
                k[1] = kxl[(l2 << 1) + 1];
            } else if (kxl[(l2 << 1) + 2] == kxl[(l3 << 1) + 1] || kxl[(l2 << 
                    1) + 2] == kxl[(l3 << 1) + 2]) {
                k[1] = kxl[(l2 << 1) + 2];
            } else {
                *err = TRUE_;
                fprintf(stderr,"** PROBLEMS IN TRIDEL FINDING K(2) **");
                goto L180;
            }
            if (kxl[(l3 << 1) + 1] == kxl[(l1 << 1) + 1] || kxl[(l3 << 1) + 1]
                     == kxl[(l1 << 1) + 2]) {
                k[2] = kxl[(l3 << 1) + 1];
            } else if (kxl[(l3 << 1) + 2] == kxl[(l1 << 1) + 1] || kxl[(l3 << 
                    1) + 2] == kxl[(l1 << 1) + 2]) {
                k[2] = kxl[(l3 << 1) + 2];
            } else {
                *err = TRUE_;
                fprintf(stderr,"** PROBLEMS IN TRIDEL FINDING K(3) **");
                goto L180;
            }
/*  NOW CHECK K(1)'S, K(2)'S, AND K(3)'S ANGLE AT THE LINE JOINT. */
/*  THERE ARE THREE POSSIBILITIES FOR CHANGE: */
/*     1) ANYTHING OVER 175 DEGREES GETS THE CORRESPONDING ELEMENT */
/*        DELETED */
/*     2) ANYTHING OVER 150 AND HOOKED TO ANOTHER 3-LINE NODE GETS */
/*        THE CORRESPONDING ELEMENT DELETED */
/*     3) AN ELONGATED ELEMENT OVER 150 DEGREES GETS A 3 ELEMENT */
/*        REPLACEMENT FOR THE TWO ELEMENTS THERE */
            toler1 = (float)2.9670597;
            toler2 = (float)2.6179939;
            if (*graph && ! (*video)) {
/* Computing MAX */
                r__7 = (r__1 = xn[n1] - xn[i__], dbl_abs(r__1)), r__8 = (r__2 = 
                        xn[n2] - xn[i__], dbl_abs(r__2)), r__7 = dbl_max(r__7,r__8), 
                        r__8 = (r__3 = xn[n3] - xn[i__], dbl_abs(r__3)), r__7 = 
                        dbl_max(r__7,r__8), r__8 = (r__4 = yn[n1] - yn[i__], dbl_abs(
                        r__4)), r__7 = dbl_max(r__7,r__8), r__8 = (r__5 = yn[n2] 
                        - yn[i__], dbl_abs(r__5)), r__7 = dbl_max(r__7,r__8), r__8 = 
                        (r__6 = yn[n3] - yn[i__], dbl_abs(r__6));
                dist = dbl_max(r__7,r__8) * (float)3.;
                *xmin = xn[i__] - dist;
                *xmax = xn[i__] + dist;
                *ymin = yn[i__] - dist;
                *ymax = yn[i__] + dist;
            }
            ang1 = atan2(yn[n1] - yn[i__], xn[n1] - xn[i__]);
            if (ang1 < (float)0.) {
                ang1 += twopi;
            }
            ang2 = atan2(yn[n2] - yn[i__], xn[n2] - xn[i__]);
            if (ang2 < (float)0.) {
                ang2 += twopi;
            }
            ang3 = atan2(yn[n3] - yn[i__], xn[n3] - xn[i__]);
            if (ang3 < (float)0.) {
                ang3 += twopi;
            }
/*  CHECK TO SEE IF THE NODES ARE CLOCKWISE OR COUNTERCLOCKWISE */
/*  (POSITIVE AREA IS CCW) */
            area = (yn[n1] + yn[n3]) * (float).5 * (xn[n3] - xn[n1]) + (yn[n2]
                     + yn[n1]) * (float).5 * (xn[n1] - xn[n2]) + (yn[n3] + yn[
                    n2]) * (float).5 * (xn[n2] - xn[n3]);
            if (area > (float)0.) {
                ang12 = ang2 - ang1;
                ang23 = ang3 - ang2;
                ang31 = ang1 - ang3;
            } else {
                ang12 = ang1 - ang2;
                ang23 = ang2 - ang3;
                ang31 = ang3 - ang1;
            }
            if (ang12 < (float)0.) {
                ang12 += twopi;
            }
            if (ang23 < (float)0.) {
                ang23 += twopi;
            }
            if (ang31 < (float)0.) {
                ang31 += twopi;
            }
            if (*graph) {
                rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, 
                        ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
/*  5 IS PINK; 4 IS BLUE; 3 IS YELLOW; 0 IS BLACK ; 7 IS WHITE; 1 IS RED */
                lcolor_("YELOW", (ftnlen)5);
                d2node_(mxnd, &xn[1], &yn[1], &i__, &n1);
                d2node_(mxnd, &xn[1], &yn[1], &i__, &n2);
                d2node_(mxnd, &xn[1], &yn[1], &i__, &n3);
                lcolor_("WHITE", (ftnlen)5);
                sflush_();
            }
/*  NOW DO THE CHECKS FOR CHANGING THE ELEMENT */
            if (area > (float)0.) {
                adjtri_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &
                        zn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], 
                        nnn, navail, iavail, &i__, k, &ang12, &toler1, &
                        toler2, &n2, &n1, &n3, kreg, xmin, xmax, ymin, ymax, 
                        zmin, zmax, kkk, lll, dev1, &done, &check, graph, 
                        video, noroom, err, &kkkadd, (ftnlen)3);
            } else {
                adjtri_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &
                        zn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], 
                        nnn, navail, iavail, &i__, k, &ang12, &toler1, &
                        toler2, &n1, &n2, &n3, kreg, xmin, xmax, ymin, ymax, 
                        zmin, zmax, kkk, lll, dev1, &done, &check, graph, 
                        video, noroom, err, &kkkadd, (ftnlen)3);
            }
            if (*noroom || *err) {
                goto L180;
            }
            if (done) {
                goto L130;
            }
            if (area > (float)0.) {
                adjtri_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &
                        zn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], 
                        nnn, navail, iavail, &i__, &k[1], &ang23, &toler1, &
                        toler2, &n3, &n2, &n1, kreg, xmin, xmax, ymin, ymax, 
                        zmin, zmax, kkk, lll, dev1, &done, &check, graph, 
                        video, noroom, err, &kkkadd, (ftnlen)3);
            } else {
                adjtri_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &
                        zn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], 
                        nnn, navail, iavail, &i__, &k[1], &ang23, &toler1, &
                        toler2, &n2, &n3, &n1, kreg, xmin, xmax, ymin, ymax, 
                        zmin, zmax, kkk, lll, dev1, &done, &check, graph, 
                        video, noroom, err, &kkkadd, (ftnlen)3);
            }
            if (*noroom || *err) {
                goto L180;
            }
            if (done) {
                goto L130;
            }
            if (area > (float)0.) {
                adjtri_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &
                        zn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], 
                        nnn, navail, iavail, &i__, &k[2], &ang31, &toler1, &
                        toler2, &n1, &n3, &n2, kreg, xmin, xmax, ymin, ymax, 
                        zmin, zmax, kkk, lll, dev1, &done, &check, graph, 
                        video, noroom, err, &kkkadd, (ftnlen)3);
            } else {
                adjtri_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &
                        zn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], 
                        nnn, navail, iavail, &i__, &k[2], &ang31, &toler1, &
                        toler2, &n3, &n1, &n2, kreg, xmin, xmax, ymin, ymax, 
                        zmin, zmax, kkk, lll, dev1, &done, &check, graph, 
                        video, noroom, err, &kkkadd, (ftnlen)3);
            }
            if (*noroom || *err) {
                goto L180;
            }
L130:
            if (done) {
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &n1, err);
                if (*err) {
                    goto L180;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &n2, err);
                if (*err) {
                    goto L180;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &n3, err);
                if (*err) {
                    goto L180;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &i__, err);
                if (*err) {
                    goto L180;
                }
                if (*video) {
                    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax,
                             ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)
                            3);
                    snapit_(&c__3);
                }
                filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &
                        nxl[3], &lxn[5], lll, nnn, nnn, &lnodes[lnodes_offset]
                        , &bnsize[3], nloop, xmin, xmax, ymin, ymax, zmin, 
                        zmax, dev1, kreg, (ftnlen)3);
                if (*graph || *video) {
                    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax,
                             ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)
                            3);
                    if (*video) {
                        snapit_(&c__3);
                    }
                }
                done = FALSE_;
                redo = TRUE_;
                goto L160;
            }
/*  NOW CHECK THE THREE ELEMENTS TO SEE IF AN ELEMENT EXISTS WHICH: */
/*    (1) CONTAINS ONLY 2 OPPOSING "LARGE ANGLE" THREE-LINE NODES */
/*        AND AT LEAST 1 "SMALL ANGLE" FOUR- OR FIVE-LINE NODE */
/*    (2) ONE THAT IS REALLY SQUASHED WITH AT LEAST ONE */
/*        "SMALL ANGLE" FIVE-LINE NODE. */
/*    (3) CONTAINS A "LARGE ANGLE" THREE-LINE NODE AND TWO */
/*        RELATIVELY SMALL FIVE-LINE NODE AND A NORMAL FOUR */
/*        LINE NODE */
/*    (4) CONTAINS TWO "VERY SMALL" ANGLES AND TWO "VERY LARGE" */
/*        ANGLES */
/*    (5) CONTAINS TWO RELATIVELY SMALL ANGLES AND TWO RELATIVELY */
/*        LARGE ANGLES AND IS CONSIDERABLY SMALLER THAN IS */
/*        DICTATED BY THE DESIRED SIZE */
/*  THIS ELEMENT SHOULD BE DELETED. */
            toler3 = (float)1.7453293;
            toler4 = (float)1.5707963;
            toler5 = (float)2.0943951;
            toler6 = (float).9599311;
            for (j = 1; j <= 3; ++j) {
                gnxka_(mxnd, &xn[1], &yn[1], &k[j - 1], nodes, &area, &lxk[5],
                         &nxl[3], &ccw);
                if (i__ != nodes[0] && i__ != nodes[1] && i__ != nodes[2] && 
                        i__ != nodes[3]) {
                    fprintf(stderr,"** PROBLEMS IN TRIDEL - I IS NOT IN ELEMENT K **");
                    *err = TRUE_;
                    goto L180;
                }
/*  ARRANGE NODES SO THE COLLAPSING DIAGONAL IS FROM 1ST TO 3RD NODES */
/*  AND INSURE THAT THE NODE TO BE DELETED IS NOT A BOUNDARY NODE */
                nxkord_(nodes, &i__);
                n1 = nodes[0];
                n2 = nodes[1];
                n3 = nodes[2];
                n4 = nodes[3];
                x21 = xn[n2] - xn[n1];
                x32 = xn[n3] - xn[n2];
                x43 = xn[n4] - xn[n3];
                x14 = xn[n1] - xn[n4];
                y21 = yn[n2] - yn[n1];
                y32 = yn[n3] - yn[n2];
                y43 = yn[n4] - yn[n3];
                y14 = yn[n1] - yn[n4];
/* Computing 2nd power */
                r__1 = x21;
/* Computing 2nd power */
                r__2 = y21;
                d21 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
                r__1 = x32;
/* Computing 2nd power */
                r__2 = y32;
                d32 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
                r__1 = x43;
/* Computing 2nd power */
                r__2 = y43;
                d43 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
                r__1 = x14;
/* Computing 2nd power */
                r__2 = y14;
                d14 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing MAX */
                r__1 = dbl_max(d21,d32), r__1 = dbl_max(r__1,d43);
                dmax__ = dbl_max(r__1,d14) * (float)1.5;
                if (lxn[(n1 << 2) + 3] == 0) {
                    theta1 = pi;
                } else {
                    theta1 = acos(-(x21 * x14 + y21 * y14) / (d21 * d14));
                }
                if (lxn[(n2 << 2) + 3] == 0) {
                    theta2 = pi;
                } else {
                    theta2 = acos(-(x32 * x21 + y32 * y21) / (d32 * d21));
                }
                if (lxn[(n3 << 2) + 3] == 0) {
                    theta3 = pi;
                } else {
                    theta3 = acos(-(x43 * x32 + y43 * y32) / (d43 * d32));
                }
                if (lxn[(n3 << 2) + 3] == 0) {
                    theta4 = pi;
                } else {
                    theta4 = acos(-(x14 * x43 + y14 * y43) / (d14 * d43));
                }
/*  TEST CASE ONE */
                if (lxn[(n1 << 2) + 2] > 0 && lxn[(n3 << 2) + 2] > 0 && lxn[(
                        n3 << 2) + 4] == 0 && lxn[(n2 << 2) + 4] != 0 && lxn[(
                        n4 << 2) + 4] != 0 && (theta1 > toler3 || theta3 > 
                        toler3) && (theta2 < toler4 || theta4 < toler4) && k[
                        j - 1] != kkkadd) {
/*     &            ( (LXN (4, N2) .LT. 0) .OR. */
/*     &            (LXN (4, N4) .LT. 0) ) .AND. */
                    passed = TRUE_;
/*  TEST CASE 2 */
                } else if (lxn[(n1 << 2) + 2] > 0 && lxn[(n3 << 2) + 2] > 0 &&
                         lxn[(n3 << 2) + 4] >= 0 && lxn[(n2 << 2) + 4] != 0 &&
                         lxn[(n4 << 2) + 4] != 0 && (lxn[(n2 << 2) + 4] < 0 ||
                         lxn[(n4 << 2) + 4] < 0) && (theta1 > toler5 || 
                        theta3 > toler5) && (theta2 < toler6 || theta4 < 
                        toler6) && k[j - 1] != kkkadd) {
                    passed = TRUE_;
/*  TEST CASE 3 */
                } else if (lxn[(n1 << 2) + 2] > 0 && lxn[(n3 << 2) + 2] > 0 &&
                         lxn[(n3 << 2) + 4] >= 0 && lxn[(n2 << 2) + 4] < 0 && 
                        lxn[(n4 << 2) + 4] < 0 && (theta1 > toler3 || theta3 
                        > toler3) && (theta2 < toler4 || theta4 < toler4) && 
                        k[j - 1] != kkkadd) {
                    passed = TRUE_;
/*  TEST CASE 4 */
                } else if (lxn[(n1 << 2) + 2] > 0 && lxn[(n3 << 2) + 2] > 0 &&
                         theta1 > toler5 && theta3 > toler5 && theta2 < 
                        toler6 && theta4 < toler6 && k[j - 1] != kkkadd) {
                    passed = TRUE_;
/*  TEST CASE 5 */
                } else if (lxn[(n1 << 2) + 2] > 0 && lxn[(n3 << 2) + 2] > 0 &&
                         theta1 > toler3 && theta3 > toler3 && theta2 < 
                        toler4 && theta4 < toler4 && dmax__ < bnsize[(n1 << 1)
                         + 1] && k[j - 1] != kkkadd) {
                    passed = TRUE_;
                } else {
                    passed = FALSE_;
                }
                if (passed) {
                    if (*graph) {
                        rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, 
                                xmax, ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
                        lcolor_("PINK ", (ftnlen)5);
                        d2node_(mxnd, &xn[1], &yn[1], &n1, &n2);
                        d2node_(mxnd, &xn[1], &yn[1], &n2, &n3);
                        d2node_(mxnd, &xn[1], &yn[1], &n3, &n4);
                        d2node_(mxnd, &xn[1], &yn[1], &n4, &n1);
                        lcolor_("WHITE", (ftnlen)5);
                        sflush_();
                    }
                    node = n1;
                    kelem = k[j - 1];
L140:
                    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                            lnodes[lnodes_offset], &n1, err);
                    if (*err) {
                        goto L180;
                    }
                    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                            lnodes[lnodes_offset], &n2, err);
                    if (*err) {
                        goto L180;
                    }
                    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                            lnodes[lnodes_offset], &n3, err);
                    if (*err) {
                        goto L180;
                    }
                    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                            lnodes[lnodes_offset], &n4, err);
                    if (*err) {
                        goto L180;
                    }
                    delem_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &
                            nxl[3], &lxn[5], nnn, navail, iavail, &node, &
                            kelem, &node1, &node3, &done, &check, noroom, err)
                            ;
                    if (*noroom || *err) {
                        goto L180;
                    }
                    if (done) {
                        if (*video) {
                            rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], 
                                    xmin, xmax, ymin, ymax, zmin, zmax, lll, 
                                    dev1, kreg, (ftnlen)3);
                            snapit_(&c__3);
                        }
                        filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &
                                kxl[3], &nxl[3], &lxn[5], lll, nnn, nnn, &
                                lnodes[lnodes_offset], &bnsize[3], nloop, 
                                xmin, xmax, ymin, ymax, zmin, zmax, dev1, 
                                kreg, (ftnlen)3);
                        if (*graph || *video) {
                            rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], 
                                    xmin, xmax, ymin, ymax, zmin, zmax, lll, 
                                    dev1, kreg, (ftnlen)3);
                            if (*video) {
                                snapit_(&c__3);
                            }
                        }
/*  CHECK TO SEE IF WE HAVE MADE A 2-LINE NODE */
                        if (lxn[(node1 << 2) + 3] <= 0) {
                            node = node1;
                            kelem = kxl[(lxn[(node << 2) + 1] << 1) + 1];
                            check = FALSE_;
                            goto L140;
                        } else if (lxn[(node3 << 2) + 3] <= 0) {
                            node = node3;
                            kelem = kxl[(lxn[(node << 2) + 1] << 1) + 1];
                            check = FALSE_;
                            goto L140;
                        }
                        check = TRUE_;
                        done = FALSE_;
                        redo = TRUE_;
                        goto L160;
                    }
                }
/* L150: */
            }
L160:
            ;
        }
/* L170: */
    }
    trifix_(mxnd, mln, &xn[1], &yn[1], &zn[1], &nuid[1], &lxk[5], &kxl[3], &
            nxl[3], &lxn[5], nnn, lll, kkk, navail, iavail, &angle[1], &
            lnodes[lnodes_offset], &bnsize[3], nloop, dev1, kreg, xmin, xmax, 
            ymin, ymax, zmin, zmax, graph, video, noroom, err, (ftnlen)3);
    if (*noroom || *err) {
        goto L180;
    }
    if (redo) {
        goto L100;
    }
L180:
    return 0;
} /* tridel_ */

#ifdef __cplusplus
        }
#endif

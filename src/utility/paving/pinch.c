/* pinch.f -- translated by f2c (version 20160102).
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
  
/* Common Block Declarations */

struct {
    real timea, timep, timec, timepc, timeaj, times;
} timing_;

#define timing_1 timing_

/* Table of constant values */

static integer c__1 = 1;
static integer c__2 = 2;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int pinch_(integer *mxnd, integer *mxcorn, integer *mln, 
        integer *nuid, real *xn, real *yn, real *zn, integer *lxk, integer *
        kxl, integer *nxl, integer *lxn, real *angle, integer *lnodes, real *
        bnsize, integer *node, integer *nloop, integer *kkkold, integer *
        lllold, integer *nnnold, integer *iavail, integer *navail, logical *
        done, real *xmin, real *xmax, real *ymin, real *ymax, real *zmin, 
        real *zmax, char *dev1, integer *lll, integer *kkk, integer *nnn, 
        integer *lcorn, integer *ncorn, integer *nadj1, integer *nadj2, 
        logical *graph, logical *video, integer *kreg, logical *noroom, 
        logical *err, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1, i__2;
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal);

    /* Local variables */
    static integer i__, j, n0, n1, n2, n6, nc, ii, ll;
    static real pi;
    static integer nl;
    static logical bok;
    static integer nnn2, idif;
    static real fact;
    static integer kneg, kold, lold, knew, lnew, ilow, line1, line2, n1old;
    static real time1, time2;
    static integer idum1, idum2;
    static logical check;
    static real ahold;
    static integer ihigh;
    static logical ddone;
    extern /* Subroutine */ int delem_(integer *, real *, real *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, logical *, 
            logical *, logical *, logical *), wedge_(integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, real *, 
            real *, integer *, real *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, logical *, logical *, logical *, logical *, logical *);
    static real dist01, dist21;
    static integer ngone;
    static logical found, onlyc;
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
            integer *), close2_(integer *, integer *, integer *, real *, real 
            *, real *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, real *, 
            real *, real *, real *, real *, real *, logical *, logical *, 
            char *, integer *, logical *, logical *, ftnlen);
    static real toler1, toler2;
    static integer l1list[20];
    extern /* Subroutine */ int  bpinch_(integer *, 
            integer *, integer *, real *, real *, integer *, integer *, real *
            , integer *, integer *, integer *, integer *, real *, real *, 
            logical *, logical *);
    static logical pwedge;
    extern /* Subroutine */ int getime_(real *), addlxn_(integer *, integer *,
             integer *, integer *, integer *, integer *, integer *, integer *,
             logical *, logical *);
    static logical wedgok, pgraph;
    extern /* Subroutine */ int getcrn_(integer *, integer *, integer *, 
            integer *, integer *, integer *, real *, real *, real *, integer *
            , integer *, integer *, logical *, logical *, logical *, logical *
            );
    static integer nthere;
    extern /* Subroutine */ int dellxn_(integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, logical *, 
            logical *), filsmo_(integer *, integer *, real *, real *, real *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, real *, integer *, real *, real *, real *, 
            real *, real *, real *, char *, integer *, ftnlen), marksm_(
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, logical *), lcolor_(char *, ftnlen), 
            lupang_(integer *, integer *, real *, real *, real *, integer *, 
            integer *, integer *, integer *, integer *, real *, integer *, 
            integer *, integer *, real *, real *, real *, real *, real *, 
            real *, char *, integer *, logical *, ftnlen), snapit_(integer *);
    static logical pposbl;
    extern /* Subroutine */ int getlxn_(integer *, integer *, integer *, 
            integer *, integer *, logical *), sflush_(), rplotl_(integer *, 
            real *, real *, real *, integer *, real *, real *, real *, real *,
             real *, real *, integer *, char *, integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE PINCH = COLLAPSES A CORNER WITH A SMALL ANGLE CLOSED */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --zn;
    --yn;
    --xn;
    --nuid;
    --lcorn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
    getime_(&time1);
    pgraph = FALSE_;
    pwedge = TRUE_;
    found = FALSE_;
    check = FALSE_;
    onlyc = TRUE_;
    *done = FALSE_;
    *err = FALSE_;
/*  SEE IF ONLY 2 NODES ARE LEFT ON THE LOOP */
    if (*nloop == 2) {
        marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
                lnodes_offset], node, err);
        if (*err) {
            goto L210;
        }
        marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
                lnodes_offset], &lnodes[*node * lnodes_dim1 + 3], err);
        if (*err) {
            goto L210;
        }
        close2_(mxnd, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3],
                 &nxl[3], &lxn[5], &lnodes[lnodes_offset], iavail, navail, 
                nnn, lll, node, xmin, xmax, ymin, ymax, zmin, zmax, &pgraph, 
                video, dev1, kreg, noroom, err, (ftnlen)3);
        if (*noroom || *err) {
            goto L210;
        }
        *nloop = 0;
        found = TRUE_;
        *done = TRUE_;
        nnn2 = *nnn;
        getime_(&time2);
        timing_1.timep = timing_1.timep + time2 - time1;
        filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], 
                &lxn[5], lll, nnn, &nnn2, &lnodes[lnodes_offset], &bnsize[3], 
                nloop, xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, (ftnlen)3);
        getime_(&time1);
        if (pgraph || *video) {
            rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, 
                    ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
            if (*video) {
                snapit_(&c__1);
            }
        }
        goto L200;
    }
    if (pgraph) {
        rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, ymax,
                 zmin, zmax, lll, dev1, kreg, (ftnlen)3);
    }
/*  GET THE CORNERS THAT CAN BE ADJUSTED */
    n1old = 0;
L100:
    if (*graph) {
        lcolor_("YELOW", (ftnlen)5);
    }
    getcrn_(mxnd, mxcorn, mln, &lnodes[lnodes_offset], ncorn, &lcorn[1], &
            angle[1], &xn[1], &yn[1], &lxn[5], nloop, node, &onlyc, &pposbl, 
            graph, err);
    if (*ncorn == 2) {
/* Computing MIN */
        i__1 = lnodes[lcorn[1] * lnodes_dim1 + 7], i__2 = lnodes[lcorn[2] * 
                lnodes_dim1 + 7];
        idif = int_min(i__1,i__2);
    } else if (*ncorn == 3) {
/* Computing MIN */
        i__1 = lnodes[lcorn[1] * lnodes_dim1 + 7], i__2 = lnodes[lcorn[2] * 
                lnodes_dim1 + 7], i__1 = int_min(i__1,i__2), i__2 = lnodes[lcorn[
                3] * lnodes_dim1 + 7];
        ilow = int_min(i__1,i__2);
/* Computing MAX */
        i__1 = lnodes[lcorn[1] * lnodes_dim1 + 7], i__2 = lnodes[lcorn[2] * 
                lnodes_dim1 + 7], i__1 = int_max(i__1,i__2), i__2 = lnodes[lcorn[
                3] * lnodes_dim1 + 7];
        ihigh = int_max(i__1,i__2);
    }
    if (*err) {
        goto L210;
    }
    if (*graph) {
        lcolor_("WHITE", (ftnlen)5);
    }
/*  FOR NORMAL REGIONS, */
/*  TOLER1 IS SET AT 20 DEGREES. (A 3 DEGREE IRREGULAR NODE IS FORMED) */
/*  TOLER2 IS SET TO 50 DEGREES. (A 4+ DEGREE IRREGULAR NODE IS HELPED) */
/*  THEY ARE SET FOR AN UNEQUAL SEMICIRCLE TO 30 AND 60 RESPECTIVELY */
/*  THEY ARE SET FOR AN EQUAL SEMICIRCLE TO 35 AND 70 RESPECTIVELY */
/*  THEY ARE SET FOR A 3-2-1 TRIANGLE TO 35 AND 70 RESPECTIVELY */
    if (*ncorn == 2) {
/* Computing MIN */
        i__1 = lnodes[lcorn[1] * lnodes_dim1 + 7], i__2 = lnodes[lcorn[2] * 
                lnodes_dim1 + 7];
        idif = int_min(i__1,i__2);
        if (lnodes[lcorn[1] * lnodes_dim1 + 7] == lnodes[lcorn[2] * 
                lnodes_dim1 + 7]) {
            toler1 = (float).5235988;
            toler2 = (float)1.2217305;
        } else {
            toler1 = (float).6108652;
            toler2 = (float)1.0471976;
        }
    } else if (*ncorn == 3 && ilow == 1 && ihigh == 3) {
        toler1 = (float).6108652;
        toler2 = (float)1.0471976;
    } else {
        toler1 = (float).3490659;
        toler2 = (float).8726646;
    }
/*  NOW MAKE SURE THAT A WEDGE CAN BE ALLOWED */
    if (*nloop <= 4) {
        kneg = 0;
        i__1 = *ncorn;
        for (i__ = 1; i__ <= i__1; ++i__) {
            if (angle[lcorn[i__]] < (float)0.) {
                ++kneg;
            }
/* L110: */
        }
        if (kneg >= 2) {
            wedgok = FALSE_;
        } else {
            wedgok = TRUE_;
        }
    } else {
        wedgok = TRUE_;
    }
/*  NOW SORT THE CORNERS SO THE SMALLEST REMAINING ONE GOES FIRST */
L120:
    j = 0;
    i__1 = *ncorn;
    for (i__ = 1; i__ <= i__1; ++i__) {
        if (lcorn[i__] > 0) {
            if (j == 0) {
                j = i__;
            } else if (angle[lcorn[i__]] < angle[lcorn[j]]) {
                j = i__;
            }
        }
/* L130: */
    }
    if (j > 0) {
        n1 = lcorn[j];
        lcorn[j] = -lcorn[j];
        n0 = lnodes[n1 * lnodes_dim1 + 2];
        n2 = lnodes[n1 * lnodes_dim1 + 3];
/*  CHECK TO MAKE SURE THAT A 1-1-1-1 RECTANGLE ISN'T BEING CLOSED */
        if (*nloop <= 4 && *ncorn >= 4) {
            goto L200;
        }
/*  CHECK TO MAKE SURE THAT A 4 - 1 - 1 TRIANGLE ISN'T BEING CLOSED */
/*         ELSEIF ((NCORN .EQ. 3) .AND. (NLOOP .EQ. 6) .AND. */
/*     &      (ILOW .EQ. 1) .AND. (IHIGH .EQ. 4) ) THEN */
/*            GOTO 200 */
/*         ENDIF */
/*  CHECK TO MAKE SURE THAT THE ANGLE IS ELIGIBLE FOR PINCHING AND */
/*  THAT A CLOSURE DOESN'T FORM A DEGENERATE ELEMENT ALONG THE BOUNDARY */
        bpinch_(mxnd, mln, &lnodes[lnodes_offset], &xn[1], &yn[1], &lxn[5], &
                nxl[3], &angle[1], &n0, &n1, &n2, nloop, &toler1, &toler2, &
                bok, err);
        if (*err) {
            goto L210;
        }
        if (bok) {
            if (*ncorn == 2) {
                --idif;
            }
/*  CHECK TO SEE IF A WEDGE NEEDS TO BE ADDED BEFORE THE THING IS PINCHED */
/* Computing 2nd power */
            r__1 = xn[n1] - xn[n0];
/* Computing 2nd power */
            r__2 = yn[n1] - yn[n0];
            dist01 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
            r__1 = xn[n1] - xn[n2];
/* Computing 2nd power */
            r__2 = yn[n1] - yn[n2];
            dist21 = sqrt(r__1 * r__1 + r__2 * r__2);
            fact = (float)2.5;
            if (wedgok && dist01 > fact * dist21 && kxl[(lnodes[n0 * 
                    lnodes_dim1 + 5] << 1) + 1] > 0) {
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[n1 * lnodes_dim1 + 2], 
                        err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[n1 * 
                        lnodes_dim1 + 2] * lnodes_dim1 + 2], err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[lnodes[n1 * 
                        lnodes_dim1 + 2] * lnodes_dim1 + 2] * lnodes_dim1 + 2]
                        , err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[n1 * lnodes_dim1 + 3], 
                        err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[n1 * 
                        lnodes_dim1 + 3] * lnodes_dim1 + 3], err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[lnodes[n1 * 
                        lnodes_dim1 + 3] * lnodes_dim1 + 3] * lnodes_dim1 + 3]
                        , err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &n1, err);
                if (*err) {
                    goto L210;
                }
                wedge_(mxnd, mln, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5]
                        , &xn[1], &yn[1], &lnodes[lnodes_offset], &bnsize[3], 
                        iavail, navail, lll, kkk, nnn, lllold, nnnold, &n1, &
                        n6, nloop, &pwedge, graph, video, noroom, err);
/*  WATCH FOR THE REPEATING CASE */
                if (n1 == n1old) {
                    bnsize[(n1 << 1) + 2] *= (float)3.;
                    bnsize[(lnodes[n6 * lnodes_dim1 + 3] << 1) + 2] *= (float)
                            3.;
                    bnsize[(n6 << 1) + 2] *= (float)3.;
                }
                n1old = n1;
                if (*noroom || *err) {
                    goto L210;
                }
                if (*video) {
                    snapit_(&c__2);
                }
                if (*node == n1) {
                    *node = lnodes[n2 * lnodes_dim1 + 2];
                }
                if (*nadj1 == n1) {
                    *nadj1 = lnodes[n2 * lnodes_dim1 + 2];
                }
                if (*nadj2 == n1) {
                    *nadj2 = lnodes[n2 * lnodes_dim1 + 2];
                }
                angle[lnodes[n2 * lnodes_dim1 + 2]] = angle[n1];
                n1 = lnodes[n2 * lnodes_dim1 + 2];
                n0 = lnodes[n1 * lnodes_dim1 + 2];
                angle[n1] = pi;
                angle[n0] = pi;
            } else if (wedgok && dist21 > fact * dist01 && lxn[(n2 << 2) + 3] 
                    != 0 && kxl[(lnodes[n1 * lnodes_dim1 + 5] << 1) + 1] > 0) 
                    {
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[n2 * lnodes_dim1 + 2], 
                        err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[n2 * 
                        lnodes_dim1 + 2] * lnodes_dim1 + 2], err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[lnodes[n2 * 
                        lnodes_dim1 + 2] * lnodes_dim1 + 2] * lnodes_dim1 + 2]
                        , err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[n2 * lnodes_dim1 + 3], 
                        err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[n2 * 
                        lnodes_dim1 + 3] * lnodes_dim1 + 3], err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[lnodes[n2 * 
                        lnodes_dim1 + 3] * lnodes_dim1 + 3] * lnodes_dim1 + 3]
                        , err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &n2, err);
                if (*err) {
                    goto L210;
                }
                wedge_(mxnd, mln, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5]
                        , &xn[1], &yn[1], &lnodes[lnodes_offset], &bnsize[3], 
                        iavail, navail, lll, kkk, nnn, lllold, nnnold, &n2, &
                        n6, nloop, &pwedge, graph, video, noroom, err);
                if (*noroom || *err) {
                    goto L210;
                }
                if (*video) {
                    snapit_(&c__2);
                }
                if (*node == n1) {
                    *node = lnodes[n6 * lnodes_dim1 + 3];
                }
                if (*nadj2 == n1) {
                    *nadj2 = lnodes[n6 * lnodes_dim1 + 3];
                }
                ahold = angle[n2];
                n2 = lnodes[n1 * lnodes_dim1 + 3];
                angle[n2] = pi;
                angle[lnodes[n2 * lnodes_dim1 + 3]] = pi;
                angle[lnodes[lnodes[n2 * lnodes_dim1 + 3] * lnodes_dim1 + 3]] 
                        = ahold;
            }
/*  PROCEED WITH THE PINCH */
            line1 = lnodes[n0 * lnodes_dim1 + 5];
            line2 = lnodes[n1 * lnodes_dim1 + 5];
/*  CHECK TO MAKE SURE THAT AT LEAST ONE OF THE LINES */
/*  IS NOT A BOUNDARY LINE AND GET THE NODE TO BE DELETED */
            if (lxn[(n0 << 2) + 2] > 0 || lxn[(n2 << 2) + 2] > 0) {
                found = TRUE_;
                if (lxn[(n0 << 2) + 2] > 0) {
                    ngone = n0;
                    nthere = n2;
                    lnew = line2;
                    lold = line1;
                    lnodes[nthere * lnodes_dim1 + 2] = lnodes[n0 * 
                            lnodes_dim1 + 2];
                    lnodes[lnodes[n0 * lnodes_dim1 + 2] * lnodes_dim1 + 3] = 
                            nthere;
                } else {
                    ngone = n2;
                    nthere = n0;
                    lnew = line1;
                    lold = line2;
                    lnodes[nthere * lnodes_dim1 + 3] = lnodes[n2 * 
                            lnodes_dim1 + 3];
                    lnodes[lnodes[n2 * lnodes_dim1 + 3] * lnodes_dim1 + 2] = 
                            nthere;
                    lnodes[nthere * lnodes_dim1 + 5] = lnodes[n2 * 
                            lnodes_dim1 + 5];
                }
                lnodes[n1 * lnodes_dim1 + 4] = -2;
/*  SEE IF THE NODES BEING USED ARE IN THE CORNER LIST */
/*  IF THEY ARE THEN THOSE CORNERS ARE NEGATED */
                i__1 = *ncorn;
                for (nc = 1; nc <= i__1; ++nc) {
                    if (lcorn[nc] == nthere || lcorn[nc] == ngone) {
                        lcorn[nc] = -(i__2 = lcorn[nc], abs(i__2));
                    }
/* L140: */
                }
/*  DELETE THE OLD LINE AND REDO LINK ARRAYS */
                kold = kxl[(lold << 1) + 1];
                knew = kxl[(lnew << 1) + 1];
                kxl[(lnew << 1) + 1] = knew;
                kxl[(lnew << 1) + 2] = kold;
                kxl[(lold << 1) + 1] = 0;
                kxl[(lold << 1) + 2] = 0;
                if (*video || pgraph) {
                    lcolor_("BLACK", (ftnlen)5);
                    d2node_(mxnd, &xn[1], &yn[1], &nxl[(lold << 1) + 1], &nxl[
                            (lold << 1) + 2]);
                    if (*graph) {
                        lcolor_("WHITE", (ftnlen)5);
                    } else {
                        lcolor_("YELOW", (ftnlen)5);
                    }
                    sflush_();
                }
                nxl[(lold << 1) + 1] = 0;
                nxl[(lold << 1) + 2] = 0;
/*  FIX THE LINES PER ELEMENT ARRAY FOR THE ONE ELEMENT CHANGING */
                if (kold > 0) {
                    for (ii = 1; ii <= 4; ++ii) {
                        if (lxk[ii + (kold << 2)] == lold) {
                            lxk[ii + (kold << 2)] = lnew;
                            goto L160;
                        }
/* L150: */
                    }
                    fprintf(stderr,"** PROBLEMS IN PINCH FIXING ELEMENT **");
                    *err = TRUE_;
                    goto L210;
L160:
                    ;
                }
/*  RECONNECT ALL LINES CONNECTING TO NGONE TO NTHERE */
                getlxn_(mxnd, &lxn[5], &ngone, l1list, &nl, err);
                if (*err) {
                    fprintf(stderr,"** PROBLEMS IN PINCH GETTING NGONELINES **");
                    goto L210;
                }
                i__1 = nl;
                for (ii = 1; ii <= i__1; ++ii) {
                    ll = l1list[ii - 1];
                    if (nxl[(ll << 1) + 1] == ngone) {
                        if (*video || pgraph) {
                            lcolor_("BLACK", (ftnlen)5);
                            d2node_(mxnd, &xn[1], &yn[1], &nxl[(ll << 1) + 1],
                                     &nxl[(ll << 1) + 2]);
                            if (*graph) {
                                lcolor_("WHITE", (ftnlen)5);
                            } else {
                                lcolor_("YELOW", (ftnlen)5);
                            }
                            d2node_(mxnd, &xn[1], &yn[1], &nthere, &nxl[(ll <<
                                     1) + 2]);
                            sflush_();
                        }
                        nxl[(ll << 1) + 1] = nthere;
                    } else if (nxl[(ll << 1) + 2] == ngone) {
                        if (*video || pgraph) {
                            lcolor_("BLACK", (ftnlen)5);
                            d2node_(mxnd, &xn[1], &yn[1], &nxl[(ll << 1) + 1],
                                     &nxl[(ll << 1) + 2]);
                            if (*graph) {
                                lcolor_("WHITE", (ftnlen)5);
                            } else {
                                lcolor_("YELOW", (ftnlen)5);
                            }
                            d2node_(mxnd, &xn[1], &yn[1], &nxl[(ll << 1) + 1],
                                     &nthere);
                            sflush_();
                        }
                        nxl[(ll << 1) + 2] = nthere;
                    }
/* L170: */
                }
/*  FIX LXN ARRAY */
/*  UNHOOK LOLD FROM NGONE AND FROM N1 */
                dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &ngone, &
                        lold, nnn, err, noroom);
                if (*noroom || *err) {
                    fprintf(stderr,"** PROBLEMS IN PINCH DELETING LOLD FROM NGONE **");
                    goto L210;
                }
                dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &n1, &lold, 
                        nnn, err, noroom);
                if (*noroom || *err) {
                    fprintf(stderr,"** PROBLEMS IN PINCH DELETING LOLD FROM N1 **");
                    goto L210;
                }
/*  ADD ALL LINES STILL HOOKED TO NGONE TO THE LIST OF LINES FOR NTHERE */
                i__1 = nl;
                for (ii = 1; ii <= i__1; ++ii) {
                    ll = l1list[ii - 1];
                    if (ll != lold) {
                        addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &
                                nthere, &ll, nnn, err, noroom);
                        if (*noroom || *err) {
                            fprintf(stderr,"** PROBLEMS IN PINCH ADDINGLL TO NTHERE **");
                            goto L210;
                        }
                    }
/* L180: */
                }
/*  DELETE NGONE (UNHOOK EVERYTHING FROM IT) */
                for (ii = 1; ii <= 3; ++ii) {
                    lxn[ii + (ngone << 2)] = 0;
/* L190: */
                }
                lxn[(ngone << 2) + 4] = *iavail;
                *iavail = ngone;
                ++(*navail);
                nuid[ngone] = 0;
                *nloop += -2;
/*  PLOT THE CLOSURE BEFORE SMOOTHING */
                if (*video) {
                    sflush_();
                    snapit_(&c__2);
                }
/*  NOW SEE IF THE CLOSURE HAS PRODUCED A 2-LINE NODE AND */
/*  THUS REQUIRES THAT ONE OF THE ELEMENTS MUST BE SQUASHED */
                if (lxn[(n1 << 2) + 3] == 0 && lxn[(n1 << 2) + 2] > 0) {
                    delem_(mxnd, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &
                            nxl[3], &lxn[5], nnn, navail, iavail, &n1, &kxl[(
                            lxn[(n1 << 2) + 1] << 1) + 1], &idum1, &idum2, &
                            ddone, &check, noroom, err);
                    if (*noroom || *err) {
                        goto L210;
                    }
                    if (*video) {
                        rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, 
                                xmax, ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
                        snapit_(&c__2);
                    }
                }
/*  SEE IF ONLY 2 NODES ARE LEFT ON THE LOOP */
                if (*nloop == 2) {
                    close2_(mxnd, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[
                            5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
                            lnodes_offset], iavail, navail, nnn, lll, &nthere,
                             xmin, xmax, ymin, ymax, zmin, zmax, &pgraph, 
                            video, dev1, kreg, noroom, err, (ftnlen)3);
                    if (*noroom || *err) {
                        goto L210;
                    }
                    *nloop = 0;
                    *done = TRUE_;
                    nnn2 = *nnn;
                } else {
                    nnn2 = 1;
                }
/*  PERFORM THE SMOOTH ON THE MESH */
                getime_(&time2);
                timing_1.timep = timing_1.timep + time2 - time1;
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &n1, err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &nthere, err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[nthere * lnodes_dim1 + 
                        3], err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[nthere * 
                        lnodes_dim1 + 3] * lnodes_dim1 + 3], err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[lnodes[nthere * 
                        lnodes_dim1 + 3] * lnodes_dim1 + 3] * lnodes_dim1 + 3]
                        , err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[nthere * lnodes_dim1 + 
                        2], err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[nthere * 
                        lnodes_dim1 + 2] * lnodes_dim1 + 2], err);
                if (*err) {
                    goto L210;
                }
                marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
                        lnodes[lnodes_offset], &lnodes[lnodes[lnodes[nthere * 
                        lnodes_dim1 + 2] * lnodes_dim1 + 2] * lnodes_dim1 + 2]
                        , err);
                if (*err) {
                    goto L210;
                }
                filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &
                        nxl[3], &lxn[5], lll, nnn, &nnn2, &lnodes[
                        lnodes_offset], &bnsize[3], nloop, xmin, xmax, ymin, 
                        ymax, zmin, zmax, dev1, kreg, (ftnlen)3);
                getime_(&time1);
                if (pgraph || *video) {
                    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax,
                             ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)
                            3);
                    if (*video) {
                        snapit_(&c__2);
                    }
                }
/*  CALCULATE NEW ANGLES */
                if (*nloop > 0) {
                    lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[
                            3], &nxl[3], &lxn[5], nloop, &angle[1], &lnodes[
                            lnodes_offset], &nthere, lll, xmin, xmax, ymin, 
                            ymax, zmin, zmax, dev1, kreg, err, (ftnlen)3);
                }
                if (*err) {
                    goto L210;
                }
                if (*node == ngone || *node == n1) {
                    *node = nthere;
                }
                if (*nadj1 == ngone || *nadj1 == n1) {
                    *nadj1 = nthere;
                }
                if (*nadj2 == ngone || *nadj2 == n1) {
                    *nadj2 = nthere;
                }
                if (*done) {
                    goto L200;
                }
            }
        }
        goto L120;
    }
L200:
/*  NOW GO BACK AND GET THE NEW CORNERS AND TRY AGAIN IF THE FIRST */
/*  TIME WAS SUCCESSFUL */
    if (found && ! (*done)) {
        found = FALSE_;
        goto L100;
    }
/*  NOW PLOT THE NEW BOUNDARY IF A PINCH HAS OCCURRED */
    if (found && *graph && ! pgraph) {
        rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, ymax,
                 zmin, zmax, lll, dev1, kreg, (ftnlen)3);
    }
/*  BE SURE THAT THE LXN ARRAY WILL GET FIXED (FIXLXN) LATER UP */
/*  TO THE CURRENT NNN */
    *nnnold = *nnn;
L210:
    getime_(&time2);
    timing_1.timep = timing_1.timep + time2 - time1;
    return 0;
} /* pinch_ */

#ifdef __cplusplus
        }
#endif

/* pcross.f -- translated by f2c (version 20160102).
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

/* Common Block Declarations */

struct {
    real timea, timep, timec, timepc, timeaj, times;
} timing_;

#define timing_1 timing_

/* Table of constant values */

static integer c__2 = 2;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int pcross_(integer *mxnd, integer *mxcorn, integer *mln, 
        integer *mxloop, integer *maxprm, integer *nuid, real *xn, real *yn, 
        real *zn, integer *lxk, integer *kxl, integer *nxl, integer *lxn, 
        real *angle, integer *lnodes, real *bnsize, integer *linkpr, integer *
        kperim, integer *node, integer *node1, integer *node2, integer *
        kkkold, integer *lllold, integer *nnnold, integer *iavail, integer *
        navail, logical *done, real *xmin, real *xmax, real *ymin, real *ymax,
         real *zmin, real *zmax, char *dev1, integer *lll, integer *kkk, 
        integer *nnn, integer *lcorn, integer *ncorn, integer *nloop, integer 
        *nextn1, integer *kloop, logical *graph, logical *video, integer *
        kreg, logical *noroom, logical *err, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    static real u, w;
    static integer i1, i2, j1, j2, n0, n1, n2, n3;
    static logical bok;
    static integer nnn2;
    extern /* Subroutine */ int sew2_(integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, logical *, logical *);
    static integer ndum;
    static logical done1;
    static real time1, time2;
    extern /* Subroutine */ int pinch_(integer *, integer *, integer *, 
            integer *, real *, real *, real *, integer *, integer *, integer *
            , integer *, real *, integer *, real *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, logical *, 
            real *, real *, real *, real *, real *, real *, char *, integer *,
             integer *, integer *, integer *, integer *, integer *, integer *,
             logical *, logical *, integer *, logical *, logical *, ftnlen);
    static integer kount;
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
            integer *), match2_(integer *, integer *, real *, real *, integer 
            *, integer *, integer *, real *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, logical *, integer *, 
            integer *, real *, real *, integer *, logical *, logical *);
    static integer n1test, n0test, n2test, n3test, kount2;

    static logical lmatch;
    extern /* Subroutine */ int getime_(real *);
    static logical pmatch;
    static integer iperim, jperim;
    extern /* Subroutine */ int filsmo_(integer *, integer *, real *, real *, 
            real *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, real *, integer *, real *, real *
            , real *, real *, real *, real *, char *, integer *, ftnlen), 
            lcolor_(char *, ftnlen), bcross_(integer *, integer *, real *, 
            real *, real *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            logical *, integer *, real *, real *, real *, real *, real *, 
            real *, char *, integer *, logical *, ftnlen), lupang_(integer *, 
            integer *, real *, real *, real *, integer *, integer *, integer *
            , integer *, integer *, real *, integer *, integer *, integer *, 
            real *, real *, real *, real *, real *, real *, char *, integer *,
             logical *, ftnlen), snapit_(integer *);
    static logical lcross;
    extern /* Subroutine */ int intsct_(real *, real *, real *, real *, real *
            , real *, real *, real *, real *, real *, logical *), sflush_(), 
            rplotl_(integer *, real *, real *, real *, integer *, real *, 
            real *, real *, real *, real *, real *, integer *, char *, 
            integer *, ftnlen);
    static integer kountl;

/* *********************************************************************** */
/*  SUBROUTINE PCROSS = CHECKS TO SEE IF ANY PERIMETERS CROSS AND HOOKS */
/*                      THEM TOGETHER IF THEY DO */
/* *********************************************************************** */
/*  FIND THE FIRST OVERLAPPING LINE STARTING AT THE CURRENT NODE */
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
    --nextn1;
    --nloop;
    linkpr -= 4;

    /* Function Body */
    getime_(&time1);
    *err = FALSE_;
    pmatch = TRUE_;
L100:
    if (*node1 == 0) {
        return 0;
    }
    n1 = *node1;
    kount = 0;
L110:
    n0 = lnodes[n1 * lnodes_dim1 + 2];
    n2 = lnodes[n1 * lnodes_dim1 + 3];
    n3 = lnodes[n2 * lnodes_dim1 + 3];
    ++kount;
/*  CHECK FOR COMPLETION */
    if (n1 == *node2 && kount > 1) {
        goto L140;
    } else if (kount > nloop[1] + 1) {
        fprintf(stderr,"** PROBLEMS WITH LOOP CLOSING IN PCROSS **");
        *err = TRUE_;
        goto L140;
    }
/*  LOOP THROUGH ALL THE REMAINING PERIMETERS CHECKING FOR CROSSINGS */
    iperim = linkpr[*kperim * 3 + 2];
L120:
    if (iperim == *kperim) {
        n1 = n2;
        goto L110;
    }
    kount2 = 0;
    n1test = linkpr[iperim * 3 + 1];
L130:
    n0test = lnodes[n1test * lnodes_dim1 + 2];
    n2test = lnodes[n1test * lnodes_dim1 + 3];
    n3test = lnodes[n2test * lnodes_dim1 + 3];
    intsct_(&xn[n1], &yn[n1], &xn[n2], &yn[n2], &xn[n1test], &yn[n1test], &xn[
            n2test], &yn[n2test], &u, &w, &lcross);
    if(!lcross)
      {
        n1test = n2test;
        ++kount2;
        if (n1test == linkpr[iperim * 3 + 1])
          {
            iperim = linkpr[iperim * 3 + 2];
            goto L120;
        } else if (kount2 > linkpr[iperim * 3 + 3]) {
            fprintf(stderr,"** PROBLEMS IN PCROSS WITH UNCLOSED PERIMETER **");
            *err = TRUE_;
            goto L140;
        }
        goto L130;
    }
/*  AN INTERSECTION HAS OCCURRED. */
/*  GET THE BEST SEAM FROM THIS INTERSECTION */
    if (*graph || *video) {
        rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, ymax,
                 zmin, zmax, lll, dev1, kreg, (ftnlen)3);
        if (*video) {
            snapit_(&c__2);
        }
        if (*graph) {
            lcolor_("YELOW", (ftnlen)5);
            d2node_(mxnd, &xn[1], &yn[1], &n1, &n2);
            d2node_(mxnd, &xn[1], &yn[1], &n1test, &n2test);
            lcolor_("WHITE", (ftnlen)5);
            sflush_();
        }
    }
    match2_(mxnd, mln, &xn[1], &yn[1], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &angle[1], &n0, &n1, &n2, &n3, &n0test, &n1test, &
            n2test, &n3test, &i1, &i2, &j1, &j2, &kountl, &lmatch, &linkpr[
            iperim * 3 + 3], node, &u, &w, &nloop[1], &pmatch, err);
    if (*err) {
        goto L140;
    }
    if (*graph) {
        lcolor_("PINK ", (ftnlen)5);
        d2node_(mxnd, &xn[1], &yn[1], &i1, &i2);
        d2node_(mxnd, &xn[1], &yn[1], &j1, &j2);
        lcolor_("WHITE", (ftnlen)5);
        sflush_();
    }
    if (! lmatch) {
        if (n1test == linkpr[iperim * 3 + 1]) {
            iperim = linkpr[iperim * 3 + 2];
            goto L120;
        } else if (kount2 > linkpr[iperim * 3 + 3]) {
            fprintf(stderr,"** PROBLEMS IN PCROSS WITH UNCLOSED PERIMETER **");
            *err = TRUE_;
            goto L140;
        }
        goto L130;
    }
/*  NOW CHECK TO SEE IF THE ATTACHMENT WOULD CAUSE */
/*  LINES ON THE BOUNDARY TO CROSS */
    bcross_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &
            lxn[5], &lnodes[lnodes_offset], &i1, &i2, &j1, &j2, &nloop[1], &
            bok, lll, xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, err, (ftnlen)3);
    if (*err) {
        goto L140;
    }
    if (! bok) {
        n1test = n2test;
        ++kount2;
        if (n1test == linkpr[iperim * 3 + 1]) {
            iperim = linkpr[iperim * 3 + 2];
            goto L120;
        } else if (kount2 > linkpr[iperim * 3 + 3]) {
            fprintf(stderr,"** PROBLEMS IN PCROSS WITH UNCLOSED PERIMETER **");
            *err = TRUE_;
            goto L140;
        }
        goto L130;
    }
/*  NOW THAT THE APPROPRIATE COLLAPSE HAS BEEN FOUND, THE TWO LINES */
/*  MUST BE JOINED AND THE PERIMETER LINKS RESTABLISHED */
    sew2_(mxnd, mln, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], iavail, navail, lll, kkk, nnn, &i1, &i2, &j1, &j2,
             noroom, err);
    if (*noroom || *err) {
        goto L140;
    }
/*  UPDATE THE CURRENT NODE */
    if (j1 == *node) {
        ndum = *node;
        *node = i2;
        if (*node1 == ndum) {
            *node1 = i2;
        }
        if (*node2 == ndum) {
            *node2 = i2;
        }
    } else if (j2 == *node) {
        ndum = *node;
        *node = i1;
        if (*node1 == ndum) {
            *node1 = i1;
        }
        if (*node2 == ndum) {
            *node2 = i1;
        }
    }
    nloop[1] = nloop[1] + linkpr[iperim * 3 + 3] - 2;
    linkpr[*kperim * 3 + 3] = nloop[1];
    jperim = linkpr[iperim * 3 + 2];
    if (jperim == *kperim) {
        *kperim = iperim;
    }
    linkpr[iperim * 3 + 1] = linkpr[jperim * 3 + 1];
    linkpr[iperim * 3 + 2] = linkpr[jperim * 3 + 2];
    linkpr[iperim * 3 + 3] = linkpr[jperim * 3 + 3];
    if (linkpr[*kperim * 3 + 2] == *kperim) {
        linkpr[*kperim * 3 + 2] = 0;
    }
/*  NOW SMOOTH AND PLOT THE CURRENT MESH */
    nnn2 = 1;
    getime_(&time2);
    timing_1.timepc = timing_1.timepc + time2 - time1;
    filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &
            lxn[5], lll, nnn, &nnn2, &lnodes[lnodes_offset], &bnsize[3], &
            nloop[1], xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, (ftnlen)
            3);
    getime_(&time1);
    if (*graph || *video) {
        rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, ymax,
                 zmin, zmax, lll, dev1, kreg, (ftnlen)3);
        if (*video) {
            snapit_(&c__2);
        }
        if (*graph) {
            lcolor_("YELOW", (ftnlen)5);
            d2node_(mxnd, &xn[1], &yn[1], &i1, &i2);
            lcolor_("WHITE", (ftnlen)5);
            sflush_();
        }
    }
/*  NOW TRY TO PINCH THE CONNECTION */
    lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &
            lxn[5], &nloop[1], &angle[1], &lnodes[lnodes_offset], &i2, lll, 
            xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, err, (ftnlen)3);
    if (*err) {
        goto L140;
    }
    getime_(&time2);
    timing_1.timepc = timing_1.timepc + time2 - time1;
    pinch_(mxnd, mxcorn, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[
            3], &nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], &bnsize[
            3], node, &nloop[1], kkkold, lllold, nnnold, iavail, navail, &
            done1, xmin, xmax, ymin, ymax, zmin, zmax, dev1, lll, kkk, nnn, &
            lcorn[1], ncorn, node1, node2, graph, video, kreg, noroom, err, (ftnlen)3);
    if (*noroom || *err) {
        goto L140;
    }
    if (linkpr[*kperim * 3 + 2] != 0) {
        goto L100;
    }
    getime_(&time1);
L140:
    getime_(&time2);
    timing_1.timepc = timing_1.timepc + time2 - time1;
    return 0;
} /* pcross_ */

#ifdef __cplusplus
        }
#endif

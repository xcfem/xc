/* connod.f -- translated by f2c (version 20160102).
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

static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int connod_(integer *mxnd, integer *mln, real *xn, real *yn, 
        integer *nuid, integer *lxk, integer *kxl, integer *nxl, integer *lxn,
         real *angle, integer *lnodes, integer *nnn, integer *kkk, integer *
        lll, integer *nnnold, integer *lllold, integer *n0, integer *n1, 
        integer *n2, integer *node, integer *i__, integer *nloop, integer *
        iavail, integer *navail, logical *graph, logical *video, logical *
        noroom, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;

    /* Local variables */
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
            integer *), addkxl_(integer *, integer *,
             integer *, integer *), marksm_(integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, logical *),
             snapit_(integer *), sflush_(), fixlxn_(integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, logical *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE CONNOD = ADDS A NEW ELEMENT TO AN EXISTING NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    --angle;
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
    *err = FALSE_;
    *nloop += -2;
/*  TAKE CARE OF A COUNTERCLOCKWISE CONNECTION */
    if (lnodes[*n2 * lnodes_dim1 + 3] == *node) {
        lnodes[*n0 * lnodes_dim1 + 3] = *node;
        lnodes[*node * lnodes_dim1 + 2] = *n0;
        lnodes[*n1 * lnodes_dim1 + 4] = -2;
        lnodes[*n2 * lnodes_dim1 + 4] = -2;
        angle[*n1] = (float)0.;
        angle[*n2] = (float)0.;
        ++(*lll);
        ++(*kkk);
        nxl[(*lll << 1) + 1] = *node;
        nxl[(*lll << 1) + 2] = *n0;
/*  MAKE THE NEW ELEMENT */
        lxk[(*kkk << 2) + 1] = lnodes[*n0 * lnodes_dim1 + 5];
        lxk[(*kkk << 2) + 2] = lnodes[*n1 * lnodes_dim1 + 5];
        lxk[(*kkk << 2) + 3] = lnodes[*n2 * lnodes_dim1 + 5];
        lxk[(*kkk << 2) + 4] = *lll;
        addkxl_(mxnd, &kxl[3], kkk, lll);
        addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n0 * lnodes_dim1 + 5]);
        addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n1 * lnodes_dim1 + 5]);
        addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n2 * lnodes_dim1 + 5]);
        lnodes[*n0 * lnodes_dim1 + 5] = *lll;
        if (*graph || *video) {
            d2node_(mxnd, &xn[1], &yn[1], n0, node);
            sflush_();
            if (*video) {
                snapit_(&c__1);
            }
        }
        *i__ = *node;
/*  TAKE CARE OF A CLOCKWISE CONNECTION */
    } else if (lnodes[*n0 * lnodes_dim1 + 2] == *node) {
        lnodes[*node * lnodes_dim1 + 3] = *n2;
        lnodes[*n2 * lnodes_dim1 + 2] = *node;
        lnodes[*n0 * lnodes_dim1 + 4] = -2;
        lnodes[*n1 * lnodes_dim1 + 4] = -2;
        angle[*n0] = (float)0.;
        angle[*n1] = (float)0.;
        ++(*lll);
        ++(*kkk);
        nxl[(*lll << 1) + 1] = *node;
        nxl[(*lll << 1) + 2] = *n2;
/*  MAKE THE NEW ELEMENT */
        lxk[(*kkk << 2) + 1] = lnodes[*node * lnodes_dim1 + 5];
        lxk[(*kkk << 2) + 2] = lnodes[*n0 * lnodes_dim1 + 5];
        lxk[(*kkk << 2) + 3] = lnodes[*n1 * lnodes_dim1 + 5];
        lxk[(*kkk << 2) + 4] = *lll;
        addkxl_(mxnd, &kxl[3], kkk, &lnodes[*node * lnodes_dim1 + 5]);
        addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n0 * lnodes_dim1 + 5]);
        addkxl_(mxnd, &kxl[3], kkk, &lnodes[*n1 * lnodes_dim1 + 5]);
        addkxl_(mxnd, &kxl[3], kkk, lll);
        lnodes[*node * lnodes_dim1 + 5] = *lll;
        if (*graph || *video) {
            d2node_(mxnd, &xn[1], &yn[1], node, n2);
            sflush_();
            if (*video) {
                snapit_(&c__1);
            }
        }
        *i__ = *n2;
    } else {
        fprintf(stderr,"PROBLEM HANDLING CONNECTION IN CONNOD");
        fprintf(stderr,"CONNECTION DOES NOT FORM A SIMPLE CORNER");
        *err = TRUE_;
        goto L100;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], n1, err);
    if (*err) {
        goto L100;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], n2, err);
    if (*err) {
        goto L100;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], node, err);
    if (*err) {
        goto L100;
    }
    fixlxn_(mxnd, &lxn[5], &nxl[3], &nuid[1], navail, iavail, nnn, lll, 
            nnnold, lllold, err, noroom);
    if (*err) {
        goto L100;
    }
    *lllold = *lll;
    *nnnold = *nnn;
L100:
    return 0;
} /* connod_ */

#ifdef __cplusplus
        }
#endif

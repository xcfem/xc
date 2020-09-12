/* wedge.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int wedge_(integer *mxnd, integer *mln, integer *nuid, 
        integer *lxk, integer *kxl, integer *nxl, integer *lxn, real *xn, 
        real *yn, integer *lnodes, real *bnsize, integer *iavail, integer *
        navail, integer *lll, integer *kkk, integer *nnn, integer *lllold, 
        integer *nnnold, integer *n1, integer *n6, integer *nloop, logical *
        pwedge, logical *graph, logical *video, logical *noroom, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;
    real r__1, r__2, r__3, r__4;

    /* Builtin functions */
    //double sqrt(doublereal);

    /* Local variables */
    static integer i__, l1, n0, l3, n2, l2, n4, n5, l4, l5, l6, ll, nl, kl1, 
            kl3, node1, node2, ltest;
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
            integer *);
    static integer l1list[20];
    extern /* Subroutine */ int  addlxn_(integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, logical *, logical *), dellxn_(integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            logical *, logical *), lcolor_(char *, ftnlen), getlxn_(integer *,
             integer *, integer *, integer *, integer *, logical *), sflush_()
            , fixlxn_(integer *, integer *, integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, logical *, 
            logical *);
    static real sizmin;
    static logical twolin;

/* *********************************************************************** */
/*  SUBROUTINE WEDGE = EXPANDS TWO SIDE LINES INTO A CORNER NODE */
/*                      THIS IS REFERRED TO AS A WEDGE. */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    --yn;
    --xn;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *err = FALSE_;
    *nnnold = int_min(*nnn,*nnnold);
    if (lxn[(*n1 << 2) + 3] == 0) {
        twolin = TRUE_;
    } else {
        twolin = FALSE_;
    }
/*  GET ALL THE DEFINITIONS IN ORDER */
    l3 = 0;
    n0 = lnodes[*n1 * lnodes_dim1 + 2];
    n2 = lnodes[*n1 * lnodes_dim1 + 3];
    l1 = lnodes[n0 * lnodes_dim1 + 5];
    l2 = lnodes[*n1 * lnodes_dim1 + 5];
    kl1 = kxl[(l1 << 1) + 1];
/*  FIND L3, N4, AND KL3 */
    if (! twolin) {
        for (i__ = 1; i__ <= 4; ++i__) {
            ltest = lxk[i__ + (kl1 << 2)];
            if (ltest != l1) {
                if (nxl[(ltest << 1) + 1] == *n1) {
                    l3 = ltest;
                    n4 = nxl[(ltest << 1) + 2];
                    goto L110;
                } else if (nxl[(ltest << 1) + 2] == *n1) {
                    l3 = ltest;
                    n4 = nxl[(ltest << 1) + 1];
                    goto L110;
                }
            }
/* L100: */
        }
        fprintf(stderr,"** PROBLEMS IN WEDGE FINDING L3 AND N4 **");
        *err = TRUE_;
        goto L200;
L110:
/*  FIND THE ELEMENT KL3 - THE ELEMENT ON THE OTHER SIDE OF L3 */
        if (kxl[(l3 << 1) + 1] == kl1) {
            kl3 = kxl[(l3 << 1) + 2];
        } else if (kxl[(l3 << 1) + 2] == kl1) {
            kl3 = kxl[(l3 << 1) + 1];
        } else {
            fprintf(stderr,"** PROBLEMS IN WEDGE FINDING KL3 **");
            *err = TRUE_;
            goto L200;
        }
    }
/*  GET ALL THE N1 LINES */
    getlxn_(mxnd, &lxn[5], n1, l1list, &nl, err);
    if (*err) {
        fprintf(stderr,"** PROBLEMS IN WEDGE GETTING N1 LINES **");
        goto L200;
    }
/*  ERASE THE LINES */
    if (*graph || *video) {
        lcolor_("BLACK", (ftnlen)5);
        i__1 = nl;
        for (i__ = 1; i__ <= i__1; ++i__) {
            node1 = nxl[(l1list[i__ - 1] << 1) + 1];
            node2 = nxl[(l1list[i__ - 1] << 1) + 2];
            d2node_(mxnd, &xn[1], &yn[1], &node1, &node2);
/* L120: */
        }
        if (*graph) {
            lcolor_("WHITE", (ftnlen)5);
        } else {
            lcolor_("YELOW", (ftnlen)5);
        }
        sflush_();
    }
/*  NOW THAT ALL THE NECESSARY VARIABLES HAVE BEEN DEFINED, */
/*  START BY DEFINING THE LOCATION OF ALL THE NEW NODES */
    *nnn += 2;
    if (*nnn > *mxnd) {
        *noroom = TRUE_;
        goto L200;
    }
    n5 = *nnn - 1;
    *n6 = *nnn;
    if (*pwedge) {
        xn[n5] = xn[*n1];
        yn[n5] = yn[*n1];
        xn[*n1] = xn[n0] + (xn[n5] - xn[n0]) * (float).33333;
        yn[*n1] = yn[n0] + (yn[n5] - yn[n0]) * (float).33333;
        xn[*n6] = xn[n0] + (xn[n5] - xn[n0]) * (float).66667;
        yn[*n6] = yn[n0] + (yn[n5] - yn[n0]) * (float).66667;
    } else {
        xn[*n6] = xn[n0] + xn[n2] - xn[n4];
        yn[*n6] = yn[n0] + yn[n2] - yn[n4];
        xn[n5] = (xn[*n1] + (xn[n2] - xn[*n1]) / (float)3. + (xn[*n6] + xn[n2]
                ) * (float).5) * (float).5;
        yn[n5] = (yn[*n1] + (yn[n2] - yn[*n1]) / (float)3. + (yn[*n6] + yn[n2]
                ) * (float).5) * (float).5;
        xn[*n1] = (xn[n0] + (xn[*n1] - xn[n0]) * (float).666667 + (xn[*n6] + 
                xn[n0]) * (float).5) * (float).5;
        yn[*n1] = (yn[n0] + (yn[*n1] - yn[n0]) * (float).666667 + (yn[*n6] + 
                yn[n0]) * (float).5) * (float).5;
    }
/*  NOW ADD LINES L4, L5, AND L6 */
    *lll += 3;
    l4 = *lll - 2;
    if (twolin) {
        nxl[(l4 << 1) + 1] = *n1;
        nxl[(l4 << 1) + 2] = n2;
    } else {
        nxl[(l4 << 1) + 1] = n4;
        nxl[(l4 << 1) + 2] = n5;
    }
    l5 = *lll - 1;
    l6 = *lll;
    nxl[(l5 << 1) + 1] = *n1;
    nxl[(l5 << 1) + 2] = *n6;
    nxl[(l6 << 1) + 1] = n5;
    nxl[(l6 << 1) + 2] = *n6;
/*  NOW UPDATE THE LXN ARRAYS */
    for (i__ = 1; i__ <= 4; ++i__) {
        lxn[i__ + (n5 << 2)] = 0;
        lxn[i__ + (*n6 << 2)] = 0;
/* L130: */
    }
    fixlxn_(mxnd, &lxn[5], &nxl[3], &nuid[1], navail, iavail, nnn, lll, 
            nnnold, lllold, noroom, err);
    *lllold = *lll;
    *nnnold = *nnn;
    if (*noroom || *err) {
        goto L200;
    }
/*  UPDATE EXISTING NODES AND THEIR LXN ARRAYS */
/*  - FIRST MAKE SURE THAT N5 TAKES N1'S BOUNDARY STATUS IF PWEDGE IS ON */
    if (*pwedge && lxn[(*n1 << 2) + 2] < 0) {
        lxn[(n5 << 2) + 2] = -(i__1 = lxn[(n5 << 2) + 2], abs(i__1));
    }
/*  MAKE SURE THAT ALL THE LINES THAT WERE ATTACHED TO N1 */
/*  EXCLUDING L1, L3, (STILL HOOKED TO N1) L4 AND L6 (ALREADY */
/*  HOOKED TO N5) ARE NOW ATTACHED TO N5 AND HAVE N5 AS THE */
/*  CORRECT ENDPOINT */
    i__1 = nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
        ll = l1list[i__ - 1];
        if (ll != l1 && ll != l3 && ll != l4 && ll != l6) {
            dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, n1, &ll, nnn, 
                    err, noroom);
            if (*noroom || *err) {
              fprintf(stderr,"** PROBLEMS IN WEDGE UNHOOKING LL FROM N1 **");
                goto L200;
            }
            addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &n5, &ll, nnn, 
                    err, noroom);
            if (*noroom || *err) {
                fprintf(stderr,"** PROBLEMS IN WEDGE HOOKING LL TO N5 **");
                goto L200;
            }
            if (nxl[(ll << 1) + 1] == *n1) {
                nxl[(ll << 1) + 1] = n5;
            } else if (nxl[(ll << 1) + 2] == *n1) {
                nxl[(ll << 1) + 2] = n5;
            } else {
              fprintf(stderr,"** PROBLEMS IN WEDGE CHANGING LL ENDPOINT FROM N1 TO N5 **");
                *err = TRUE_;
                goto L200;
            }
        }
/* L140: */
    }
/*  NOW ADD THE NEW ELEMENT */
    ++(*kkk);
    if (twolin) {
        lxk[(*kkk << 2) + 1] = l4;
        lxk[(*kkk << 2) + 2] = l2;
    } else {
        lxk[(*kkk << 2) + 1] = l3;
        lxk[(*kkk << 2) + 2] = l4;
    }
    lxk[(*kkk << 2) + 3] = l6;
    lxk[(*kkk << 2) + 4] = l5;
/*  NOW FIX THE KXL ARRAY FOR LINE L3 */
    if (! twolin) {
        if (kxl[(l3 << 1) + 1] == kl3) {
            kxl[(l3 << 1) + 1] = *kkk;
        } else if (kxl[(l3 << 1) + 2] == kl3) {
            kxl[(l3 << 1) + 2] = *kkk;
        } else {
            fprintf(stderr,"** PROBLEMS IN WEDGE REPLACING KL3 FOR L3 **");
            *err = TRUE_;
            goto L200;
        }
    }
/*  ADD THE KXL ENTRIES FOR THE NEW LINES */
    if (twolin) {
        kxl[(l4 << 1) + 1] = kl1;
        kxl[(l4 << 1) + 2] = *kkk;
        kxl[(l2 << 1) + 1] = *kkk;
    } else {
        kxl[(l4 << 1) + 1] = *kkk;
        kxl[(l4 << 1) + 2] = kl3;
    }
    kxl[(l5 << 1) + 1] = *kkk;
    kxl[(l6 << 1) + 1] = *kkk;
/*  NOW FIX THE LXK ARRAY FOR THE ELEMENT KL1 IF TWOLIN */
    if (twolin) {
        for (i__ = 1; i__ <= 4; ++i__) {
            if (lxk[i__ + (kl1 << 2)] == l2) {
                lxk[i__ + (kl1 << 2)] = l4;
                goto L160;
            }
/* L150: */
        }
        fprintf(stderr,"** PROBLEMS IN WEDGE REPLACING L2 WITH L4 IN KL1 **");
        *err = TRUE_;
        goto L200;
L160:
/*  OTHERWISE FIX THE LXK ARRAY FOR THE ELEMENT KL3 */
        ;
    } else {
        for (i__ = 1; i__ <= 4; ++i__) {
            if (lxk[i__ + (kl3 << 2)] == l3) {
                lxk[i__ + (kl3 << 2)] = l4;
                goto L180;
            }
/* L170: */
        }
        fprintf(stderr,"** PROBLEMS IN WEDGE REPLACING L3 WITH L4 IN KL3 **");
        *err = TRUE_;
        goto L200;
L180:
        ;
    }
/*  NOW FIX THE LNODES ARRAY */
    lnodes[n5 * lnodes_dim1 + 1] = 0;
    lnodes[*n6 * lnodes_dim1 + 1] = 0;
    lnodes[*n6 * lnodes_dim1 + 2] = *n1;
    lnodes[n5 * lnodes_dim1 + 2] = *n6;
    lnodes[n2 * lnodes_dim1 + 2] = n5;
    lnodes[*n1 * lnodes_dim1 + 3] = *n6;
    lnodes[*n6 * lnodes_dim1 + 3] = n5;
    lnodes[n5 * lnodes_dim1 + 3] = n2;
    lnodes[n5 * lnodes_dim1 + 4] = -1;
    lnodes[*n6 * lnodes_dim1 + 4] = -1;
    lnodes[*n1 * lnodes_dim1 + 5] = l5;
    lnodes[*n6 * lnodes_dim1 + 5] = l6;
    lnodes[n5 * lnodes_dim1 + 5] = l2;
    lnodes[n5 * lnodes_dim1 + 8] = lnodes[*n1 * lnodes_dim1 + 8];
    lnodes[*n6 * lnodes_dim1 + 8] = lnodes[*n1 * lnodes_dim1 + 8];
/*  PUT THE BEGINNING BOUNDARY DISTANCE IN PLACE */
    bnsize[(n5 << 1) + 1] = bnsize[(*n1 << 1) + 1];
    bnsize[(*n6 << 1) + 1] = bnsize[(*n1 << 1) + 1];
    if (bnsize[(*n1 << 1) + 1] > (float)0.) {
/* Computing MIN */
        r__1 = bnsize[(*n1 << 1) + 1] * bnsize[(*n1 << 1) + 2], r__2 = bnsize[
                (n0 << 1) + 1] * bnsize[(n0 << 1) + 2], r__1 = fmin(r__1,r__2),
                 r__2 = bnsize[(n2 << 1) + 1] * bnsize[(n2 << 1) + 2];
        sizmin = dbl_min(r__1,r__2) / bnsize[(*n1 << 1) + 1];
    } else {
/* Computing MIN */
        r__1 = bnsize[(*n1 << 1) + 1] * bnsize[(*n1 << 1) + 2], r__2 = bnsize[
                (n0 << 1) + 1] * bnsize[(n0 << 1) + 2], r__1 = fmin(r__1,r__2),
                 r__2 = bnsize[(n2 << 1) + 1] * bnsize[(n2 << 1) + 2];
/* Computing 2nd power */
        r__3 = xn[*n1] - xn[n2];
/* Computing 2nd power */
        r__4 = yn[*n1] - yn[n2];
        sizmin = dbl_min(r__1,r__2) / sqrt(r__3 * r__3 + r__4 * r__4);
    }
    if (*pwedge) {
        bnsize[(*n6 << 1) + 2] = sizmin;
        bnsize[(n5 << 1) + 2] = sizmin;
        bnsize[(*n1 << 1) + 2] = sizmin;
    } else {
        bnsize[(n5 << 1) + 2] = bnsize[(*n1 << 1) + 2] * (float)1.15;
        bnsize[(*n6 << 1) + 2] = bnsize[(*n1 << 1) + 2];
        bnsize[(*n1 << 1) + 2] *= (float)1.15;
    }
    *nloop += 2;
    *err = FALSE_;
/*  NOW REDRAW THE LINES */
    if (*graph || *video) {
        i__1 = nl;
        for (i__ = 1; i__ <= i__1; ++i__) {
            if (l1list[i__ - 1] != l1 && l1list[i__ - 1] != l3 && l1list[i__ 
                    - 1] != l2) {
                node1 = nxl[(l1list[i__ - 1] << 1) + 1];
                node2 = nxl[(l1list[i__ - 1] << 1) + 2];
                d2node_(mxnd, &xn[1], &yn[1], &node1, &node2);
            }
/* L190: */
        }
        d2node_(mxnd, &xn[1], &yn[1], &n0, n1);
        d2node_(mxnd, &xn[1], &yn[1], n1, n6);
        d2node_(mxnd, &xn[1], &yn[1], &n2, &n5);
        d2node_(mxnd, &xn[1], &yn[1], n6, &n5);
        if (twolin) {
            d2node_(mxnd, &xn[1], &yn[1], n1, &n2);
        } else {
            d2node_(mxnd, &xn[1], &yn[1], n1, &n4);
            d2node_(mxnd, &xn[1], &yn[1], &n4, &n5);
        }
        sflush_();
    }
L200:
    return 0;
} /* wedge_ */

#ifdef __cplusplus
        }
#endif

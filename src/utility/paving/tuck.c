/* tuck.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int tuck_(integer *mxnd, integer *mln, integer *nuid, real *
        xn, real *yn, integer *lxk, integer *kxl, integer *nxl, integer *lxn, 
        integer *lnodes, integer *iavail, integer *navail, integer *lll, 
        integer *kkk, integer *nnn, integer *n1, integer *nloop, logical *
        graph, logical *noroom, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;

    /* Local variables */
    static integer i__, l1, n0, n2, l2, l5, l4, l3, ii, ll, nl, nc1, nc2, kl2,
             kl5, kold, node1, node2, idraw, ltest;
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
            integer *);
    static integer l1list[20];
    extern /* Subroutine */ int  addlxn_(integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, logical *, logical *), dellxn_(integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            logical *, logical *), lcolor_(char *, ftnlen), marksm_(integer *,
             integer *, integer *, integer *, integer *, integer *, integer *,
             integer *, logical *), getlxn_(integer *, integer *, integer *, 
            integer *, integer *, logical *), sflush_();

/* *********************************************************************** */
/*  SUBROUTINE TUCK = COLLAPSES TWO SIDE LINES INTO A ROW END NODE. */
/*                      THIS IS REFERRED TO AS A TUCK. */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --yn;
    --xn;
    --nuid;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *err = FALSE_;
/*  CHECK TO MAKE SURE THAT THE NODE STILL EXISTS */
    if (lxn[(*n1 << 2) + 1] <= 0) {
        *err = TRUE_;
        fprintf(stderr,"** PROBLEMS IN TUCK - N1 DOES NOT EXIST **");
        goto L290;
    }
/*  GET ALL THE DEFINITIONS IN ORDER */
    n0 = lnodes[*n1 * lnodes_dim1 + 2];
    n2 = lnodes[*n1 * lnodes_dim1 + 3];
    l1 = lnodes[n0 * lnodes_dim1 + 5];
    l2 = lnodes[*n1 * lnodes_dim1 + 5];
    kold = kxl[(l1 << 1) + 1];
    kl2 = kxl[(l2 << 1) + 1];
/*  FIND L5 AND NC2 */
    for (i__ = 1; i__ <= 4; ++i__) {
        ltest = lxk[i__ + (kold << 2)];
        if (ltest != l1) {
            if (nxl[(ltest << 1) + 1] == *n1) {
                l5 = ltest;
                nc2 = nxl[(ltest << 1) + 2];
                goto L110;
            } else if (nxl[(ltest << 1) + 2] == *n1) {
                l5 = ltest;
                nc2 = nxl[(ltest << 1) + 1];
                goto L110;
            }
        }
/* L100: */
    }
    fprintf(stderr,"** PROBLEMS IN TUCK FINDING L5 AND NC2 **");
    *err = TRUE_;
    goto L290;
L110:
/*  FIND L4 AND NC1 */
    for (i__ = 1; i__ <= 4; ++i__) {
        ltest = lxk[i__ + (kold << 2)];
        if (ltest != l1 && ltest != l5) {
            if (nxl[(ltest << 1) + 1] == n0) {
                l4 = ltest;
                nc1 = nxl[(ltest << 1) + 2];
                goto L130;
            } else if (nxl[(ltest << 1) + 2] == n0) {
                l4 = ltest;
                nc1 = nxl[(ltest << 1) + 1];
                goto L130;
            }
        }
/* L120: */
    }
    fprintf(stderr,"** PROBLEMS IN TUCK FINDING L4 AND NC1 **");
    *err = TRUE_;
    goto L290;
L130:
/*  FIND L3 */
    for (i__ = 1; i__ <= 4; ++i__) {
        ltest = lxk[i__ + (kold << 2)];
        if (ltest != l1 && ltest != l5 && ltest != l4) {
            l3 = ltest;
            goto L150;
        }
/* L140: */
    }
    fprintf(stderr,"** PROBLEMS IN TUCK FINDING L3 **");
    *err = TRUE_;
    goto L290;
L150:
/*  FIND THE ELEMENT KL5 */
    if (kxl[(l5 << 1) + 1] == kold) {
        kl5 = kxl[(l5 << 1) + 2];
    } else if (kxl[(l5 << 1) + 2] == kold) {
        kl5 = kxl[(l5 << 1) + 1];
    } else {
        fprintf(stderr,"** PROBLEMS IN TUCK FINDING KL5 **");
        *err = TRUE_;
        goto L290;
    }
/*  NOW THAT ALL THE NECESSARY VARIABLES HAVE BEEN DEFINED, */
/*  START BY DELETING LINE L1, L2, AND L5 */
    kxl[(l1 << 1) + 1] = 0;
    kxl[(l1 << 1) + 2] = 0;
    nxl[(l1 << 1) + 1] = 0;
    nxl[(l1 << 1) + 2] = 0;
    kxl[(l2 << 1) + 1] = 0;
    kxl[(l2 << 1) + 2] = 0;
    nxl[(l2 << 1) + 1] = 0;
    nxl[(l2 << 1) + 2] = 0;
    kxl[(l5 << 1) + 1] = 0;
    kxl[(l5 << 1) + 2] = 0;
    nxl[(l5 << 1) + 1] = 0;
    nxl[(l5 << 1) + 2] = 0;
/*  NOW FIX THE KXL ARRAY FOR LINE L3 HAVING KL5 INSTEAD OF KOLD */
    if (kxl[(l3 << 1) + 1] == kold) {
        kxl[(l3 << 1) + 1] = kl5;
    } else if (kxl[(l3 << 1) + 2] == kold) {
        kxl[(l3 << 1) + 2] = kl5;
    } else {
        fprintf(stderr,"** PROBLEMS IN TUCK REPLACING KOLD FOR L3 **");
        *err = TRUE_;
        goto L290;
    }
/*  NOW FIX THE KXL ARRAY FOR LINE L3 HAVING KL5 INSTEAD OF KOLD */
    if (kxl[(l4 << 1) + 1] == kold) {
        kxl[(l4 << 1) + 1] = kl2;
    } else if (kxl[(l4 << 1) + 2] == kold) {
        kxl[(l4 << 1) + 2] = kl2;
    } else {
        fprintf(stderr,"** PROBLEMS IN TUCK REPLACING KOLD FOR L4 **");
        *err = TRUE_;
        goto L290;
    }
/*  FIX THE LINES PER ELEMENT ARRAY FOR ELEMENT KL5 TO REFLECT */
/*  L3 TAKING L5'S PLACE */
    for (i__ = 1; i__ <= 4; ++i__) {
        if (lxk[i__ + (kl5 << 2)] == l5) {
            lxk[i__ + (kl5 << 2)] = l3;
            goto L170;
        }
/* L160: */
    }
    fprintf(stderr,"** PROBLEMS IN TUCK FINDING L5 IN KL5 **");
    *err = TRUE_;
    goto L290;
L170:
/*  FIX THE LINES PER ELEMENT ARRAY FOR ELEMENT KL2 TO REFLECT */
/*  L4 TAKING L2'S PLACE */
    for (i__ = 1; i__ <= 4; ++i__) {
        if (lxk[i__ + (kl2 << 2)] == l2) {
            lxk[i__ + (kl2 << 2)] = l4;
            goto L190;
        }
/* L180: */
    }
    fprintf(stderr,"** PROBLEMS IN TUCK FINDING L2 IN KL2 **");
    *err = TRUE_;
    goto L290;
L190:
/*  RECONNECT ALL LINES CONNECTED TO N1 TO NC1 EXCEPT L5 AND L2 */
    getlxn_(mxnd, &lxn[5], n1, l1list, &nl, err);
    if (*err) {
        fprintf(stderr,"** PROBLEMS IN TUCK GETTING N1 LINES **");
        goto L290;
    }
    if (*graph) {
        lcolor_("BLACK", (ftnlen)5);
    }
    i__1 = nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
        ll = l1list[i__ - 1];
        if (*graph && nxl[(ll << 1) + 1] > 0 && nxl[(ll << 1) + 2] > 0) {
            d2node_(mxnd, &xn[1], &yn[1], &nxl[(ll << 1) + 1], &nxl[(ll << 1) 
                    + 2]);
        }
        if (nxl[(ll << 1) + 1] == *n1) {
            nxl[(ll << 1) + 1] = nc1;
        } else if (nxl[(ll << 1) + 2] == *n1) {
            nxl[(ll << 1) + 2] = nc1;
        }
/* L200: */
    }
    if (*graph) {
        lcolor_("WHITE", (ftnlen)5);
        sflush_();
    }
/*  FIX LXN ARRAY */
/*  UNHOOK L1, L2 AND L5 FROM N1 */
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, n1, &l1, nnn, err, 
            noroom);
    if (*noroom || *err) {
        fprintf(stderr,"** PROBLEMS IN TUCK UNHOOKING L1 FROM N1 **");
        goto L290;
    }
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, n1, &l2, nnn, err, 
            noroom);
    if (*noroom || *err) {
        fprintf(stderr,"** PROBLEMS IN TUCK UNHOOKING L2 FROM N1 **");
        goto L290;
    }
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, n1, &l5, nnn, err, 
            noroom);
    if (*noroom || *err) {
        fprintf(stderr,"** PROBLEMS IN TUCK UNHOOKING L5 FROM N1 **");
        goto L290;
    }
/*  ADD ALL LINES STILL HOOKED TO N1 TO THE LIST OF LINES FOR NC1 */
    i__1 = nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
        ll = l1list[i__ - 1];
        if (ll != l2 && ll != l5 && ll != l1) {
            addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nc1, &ll, nnn, 
                    err, noroom);
            if (*noroom || *err) {
                fprintf(stderr,"** PROBLEMS IN TUCK HOOKING N1' LINES TO NC1 **");
                goto L290;
            }
        }
/* L210: */
    }
/*  DELETE N1 */
    for (i__ = 1; i__ <= 3; ++i__) {
        lxn[i__ + (*n1 << 2)] = 0;
/* L220: */
    }
    lxn[(*n1 << 2) + 4] = *iavail;
    *iavail = *n1;
    ++(*navail);
    nuid[*n1] = 0;
/*  RECONNECT ALL LINES CONNECTED TO N2 TO N0 (EXCEPT L2) */
    getlxn_(mxnd, &lxn[5], &n2, l1list, &nl, err);
    if (*err) {
        fprintf(stderr,"** PROBLEMS IN TUCK GETTING N2 LINES **");
        goto L290;
    }
    if (*graph) {
        lcolor_("BLACK", (ftnlen)5);
    }
    i__1 = nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
        ll = l1list[i__ - 1];
        if (*graph && nxl[(ll << 1) + 1] > 0 && nxl[(ll << 1) + 2] > 0) {
            d2node_(mxnd, &xn[1], &yn[1], &nxl[(ll << 1) + 1], &nxl[(ll << 1) 
                    + 2]);
        }
        if (nxl[(ll << 1) + 1] == n2) {
            nxl[(ll << 1) + 1] = n0;
        } else if (nxl[(ll << 1) + 2] == n2) {
            nxl[(ll << 1) + 2] = n0;
        }
/* L230: */
    }
    if (*graph) {
        lcolor_("WHITE", (ftnlen)5);
        sflush_();
    }
/*  FIX LXN ARRAY */
/*  UNHOOK L2 FROM N2, L1 FROM N0, AND L5 FROM NC2 */
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &n2, &l2, nnn, err, 
            noroom);
    if (*noroom || *err) {
        fprintf(stderr,"** PROBLEMS IN TUCK UNHOOKING L2 FROM N2 **");
        goto L290;
    }
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &n0, &l1, nnn, err, 
            noroom);
    if (*noroom || *err) {
        fprintf(stderr,"** PROBLEMS IN TUCK UNHOOKING L1 FROM N0 **");
        goto L290;
    }
    dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nc2, &l5, nnn, err, 
            noroom);
    if (*noroom || *err) {
        fprintf(stderr,"** PROBLEMS IN TUCK UNHOOKING L1 FROM N0 **");
        goto L290;
    }
/*  ADD ALL LINES STILL HOOKED TO N2 TO THE LIST OF LINES FOR N0 */
    i__1 = nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
        ll = l1list[i__ - 1];
        if (ll != l2) {
            addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &n0, &ll, nnn, 
                    err, noroom);
            if (*noroom || *err) {
                fprintf(stderr,"** PROBLEMS IN TUCK HOOKING N2' LINES TO N0 **");
                goto L290;
            }
        }
/* L240: */
    }
/*  DELETE N2 */
    for (i__ = 1; i__ <= 3; ++i__) {
        lxn[i__ + (n2 << 2)] = 0;
/* L250: */
    }
    lxn[(n2 << 2) + 4] = *iavail;
    *iavail = n2;
    ++(*navail);
    nuid[n2] = 0;
/*  NOW DELETE THE OLD ELEMENT */
    for (i__ = 1; i__ <= 4; ++i__) {
        lxk[i__ + (kold << 2)] = 0;
/* L260: */
    }
/*  NOW FIX THE LNODES ARRAY */
    lnodes[n0 * lnodes_dim1 + 3] = lnodes[n2 * lnodes_dim1 + 3];
    lnodes[lnodes[n2 * lnodes_dim1 + 3] * lnodes_dim1 + 2] = n0;
    lnodes[n0 * lnodes_dim1 + 5] = lnodes[n2 * lnodes_dim1 + 5];
    *nloop += -2;
    *err = FALSE_;
/*  NOW REDRAW THE ELEMENTS */
    if (*graph) {
        lcolor_("BLACK", (ftnlen)5);
        d2node_(mxnd, &xn[1], &yn[1], &n0, n1);
        d2node_(mxnd, &xn[1], &yn[1], &nc2, n1);
        d2node_(mxnd, &xn[1], &yn[1], &n2, n1);
        lcolor_("WHITE", (ftnlen)5);
        getlxn_(mxnd, &lxn[5], &n0, l1list, &nl, err);
        if (*err) {
            goto L290;
        }
        i__1 = nl;
        for (ii = 1; ii <= i__1; ++ii) {
            idraw = l1list[ii - 1];
            node1 = nxl[(idraw << 1) + 1];
            node2 = nxl[(idraw << 1) + 2];
            d2node_(mxnd, &xn[1], &yn[1], &node1, &node2);
/* L270: */
        }
        getlxn_(mxnd, &lxn[5], &nc1, l1list, &nl, err);
        if (*err) {
            goto L290;
        }
        i__1 = nl;
        for (ii = 1; ii <= i__1; ++ii) {
            idraw = l1list[ii - 1];
            node1 = nxl[(idraw << 1) + 1];
            node2 = nxl[(idraw << 1) + 2];
            d2node_(mxnd, &xn[1], &yn[1], &node1, &node2);
/* L280: */
        }
        sflush_();
    }
/*  FLAG NODES FOR SMOOTHING */
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &nc1, err);
    if (*err) {
        goto L290;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &nc2, err);
    if (*err) {
        goto L290;
    }
L290:
    return 0;
} /* tuck_ */

#ifdef __cplusplus
        }
#endif

/* marksm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int marksm_(integer *mxnd, integer *mln, integer *lxk, 
        integer *kxl, integer *nxl, integer *lxn, integer *lnodes, integer *
        node, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1, i__2, i__3;

    /* Local variables */
    static integer i__, j, nl1, nl2, node1, node2, l1list[20], l2list[20];
    extern /* Subroutine */ int  getlxn_(integer *, 
            integer *, integer *, integer *, integer *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE MARKSM = MARKS NODES WITHIN 2 LINE CONNECTIONS FROM NODE */
/*                      FOR SMOOTHING */
/* *********************************************************************** */
    /* Parameter adjustments */
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    if (lxn[(*node << 2) + 1] <= 0) {
        goto L120;
    }
    getlxn_(mxnd, &lxn[5], node, l1list, &nl1, err);
    if (*err) {
        fprintf(stderr,"** PROBLEMS IN MARKSM FINDING LXN **");
        goto L120;
    }
    lnodes[*node * lnodes_dim1 + 4] = -(i__1 = lnodes[*node * lnodes_dim1 + 4]
            , abs(i__1));
    i__1 = nl1;
    for (i__ = 1; i__ <= i__1; ++i__) {
        node2 = nxl[(l1list[i__ - 1] << 1) + 1] + nxl[(l1list[i__ - 1] << 1) 
                + 2] - *node;
        getlxn_(mxnd, &lxn[5], &node2, l2list, &nl2, err);
        if (*err) {
            fprintf(stderr,"** PROBLEMS IN MARKSM FINDING LXN **");
            goto L120;
        }
        lnodes[node2 * lnodes_dim1 + 4] = -(i__2 = lnodes[node2 * lnodes_dim1 
                + 4], abs(i__2));
        i__2 = nl2;
        for (j = 1; j <= i__2; ++j) {
            node1 = nxl[(l2list[j - 1] << 1) + 1] + nxl[(l2list[j - 1] << 1) 
                    + 2] - node2;
            lnodes[node1 * lnodes_dim1 + 4] = -(i__3 = lnodes[node1 * 
                    lnodes_dim1 + 4], abs(i__3));
/* L100: */
        }
/* L110: */
    }
L120:
    return 0;
} /* marksm_ */

#ifdef __cplusplus
        }
#endif

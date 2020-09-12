/* sew2.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int sew2_(integer *mxnd, integer *mln, integer *nuid, 
        integer *lxk, integer *kxl, integer *nxl, integer *lxn, integer *
        lnodes, integer *iavail, integer *navail, integer *lll, integer *kkk, 
        integer *nnn, integer *i1, integer *i2, integer *j1, integer *j2, 
        logical *noroom, logical *err)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;

    /* Local variables */
    static integer ii, ll, nl, kold, knew, lgone, lstay, ngone1, ngone2, 
            l1list[20], nstay1, nstay2;
    extern /* Subroutine */ int  addlxn_(integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, logical *, logical *), dellxn_(integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            logical *, logical *), marksm_(integer *, integer *, integer *, 
            integer *, integer *, integer *, integer *, integer *, logical *),
             getlxn_(integer *, integer *, integer *, integer *, integer *, 
            logical *);

/* *********************************************************************** */
/*  SUBROUTINE SEW2 = COLLAPSES A LOOP INTO TWO POSSIBLE LOOPS */
/* *********************************************************************** */
    /* Parameter adjustments */
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
/*  GET THE APPROPRIATE LINES AND NODES TO BE DELETED */
    if (lxn[(*j1 << 2) + 2] < 0 && lxn[(*j2 << 2) + 2] < 0) {
        lstay = lnodes[*j1 * lnodes_dim1 + 5];
        lgone = lnodes[*i1 * lnodes_dim1 + 5];
        ngone1 = *i1;
        ngone2 = *i2;
        nstay1 = *j2;
        nstay2 = *j1;
    } else if (lxn[(*j1 << 2) + 2] < 0) {
        lstay = lnodes[*i1 * lnodes_dim1 + 5];
        lgone = lnodes[*j1 * lnodes_dim1 + 5];
        ngone1 = *j2;
        ngone2 = *i2;
        nstay1 = *i1;
        nstay2 = *j1;
    } else if (lxn[(*j2 << 2) + 2] < 0) {
        lstay = lnodes[*i1 * lnodes_dim1 + 5];
        lgone = lnodes[*j1 * lnodes_dim1 + 5];
        ngone1 = *i1;
        ngone2 = *j1;
        nstay1 = *j2;
        nstay2 = *i2;
    } else {
        lstay = lnodes[*i1 * lnodes_dim1 + 5];
        lgone = lnodes[*j1 * lnodes_dim1 + 5];
        ngone1 = *j2;
        ngone2 = *j1;
        nstay1 = *i1;
        nstay2 = *i2;
    }
    kold = kxl[(lgone << 1) + 1];
    knew = kxl[(lstay << 1) + 1];
/*  DELETE THE OLD LINE AND REDO LINK ARRAYS */
    if (knew == 0) {
        kxl[(lstay << 1) + 1] = kold;
        kxl[(lstay << 1) + 2] = 0;
    } else {
        kxl[(lstay << 1) + 1] = knew;
        kxl[(lstay << 1) + 2] = kold;
    }
    kxl[(lgone << 1) + 1] = 0;
    kxl[(lgone << 1) + 2] = 0;
    nxl[(lgone << 1) + 1] = 0;
    nxl[(lgone << 1) + 2] = 0;
/*  FIX THE LINES PER ELEMENT ARRAY FOR THE ONE ELEMENT CHANGING */
    if (kold > 0) {
        for (ii = 1; ii <= 4; ++ii) {
            if (lxk[ii + (kold << 2)] == lgone) {
                lxk[ii + (kold << 2)] = lstay;
                goto L110;
            }
/* L100: */
        }
        fprintf(stderr,"** PROBLEMS IN SEW2 FIXING THE CHANGINGELEMENT **");
        *err = TRUE_;
        goto L180;
L110:
        ;
    }
/*  RECONNECT ALL LINES CONNECTING TO NGONE2 TO NSTAY2 */
    getlxn_(mxnd, &lxn[5], &ngone2, l1list, &nl, err);
    if (*err) {
        fprintf(stderr,"** PROBLEMS IN SEW2 FINDING LXN FOR NGONE2 **");
        goto L180;
    }
    i__1 = nl;
    for (ii = 1; ii <= i__1; ++ii) {
        ll = l1list[ii - 1];
        if (nxl[(ll << 1) + 1] == ngone2) {
            nxl[(ll << 1) + 1] = nstay2;
        } else if (nxl[(ll << 1) + 2] == ngone2) {
            nxl[(ll << 1) + 2] = nstay2;
        }
/* L120: */
    }
/*  FIX LXN ARRAY */
/*  UNHOOK LGONE FROM NGONE2 OR NSTAY2 AS NEEDED */
    if (lgone == lnodes[*j1 * lnodes_dim1 + 5]) {
        dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, j1, &lgone, nnn, err,
                 noroom);
        if (*noroom || *err) {
          fprintf(stderr,"** PROBLEMS IN SEW2 DELETING NGONE2 LINES **");
          goto L180;
        }
    } else {
        dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, i2, &lgone, nnn, err,
                 noroom);
        if (*noroom || *err) {
          fprintf(stderr,"** PROBLEMS IN SEW2 DELETING NGONE2 LINES **");
            goto L180;
        }
    }
/*  ADD ALL LINES STILL HOOKED TO NGONE2 TO THE LIST OF LINES FOR NSTAY2 */
    i__1 = nl;
    for (ii = 1; ii <= i__1; ++ii) {
        ll = l1list[ii - 1];
        if (ll != lgone) {
            addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nstay2, &ll, 
                    nnn, err, noroom);
            if (*noroom || *err) {
              fprintf(stderr,"** PROBLEMS IN SEW2 ADDING NSTAY2 LINES **");
                goto L180;
            }
        }
/* L130: */
    }
/*  DELETE NGONE2 (UNHOOK EVERYTHING FROM IT) */
    for (ii = 1; ii <= 3; ++ii) {
        lxn[ii + (ngone2 << 2)] = 0;
/* L140: */
    }
    lxn[(ngone2 << 2) + 4] = *iavail;
    *iavail = ngone2;
    ++(*navail);
    nuid[ngone2] = 0;
/*  RECONNECT ALL LINES CONNECTING TO NGONE1 TO NSTAY1 */
    getlxn_(mxnd, &lxn[5], &ngone1, l1list, &nl, err);
    if (*err) {
        fprintf(stderr,"** PROBLEMS IN SEW2 GETTING NGONE1 LINES **");
        goto L180;
    }
    i__1 = nl;
    for (ii = 1; ii <= i__1; ++ii) {
        ll = l1list[ii - 1];
        if (nxl[(ll << 1) + 1] == ngone1) {
            nxl[(ll << 1) + 1] = nstay1;
        } else if (nxl[(ll << 1) + 2] == ngone1) {
            nxl[(ll << 1) + 2] = nstay1;
        }
/* L150: */
    }
/*  FIX LXN ARRAY */
/*  UNHOOK LGONE FROM NGONE1 OR NSTAY1 AS APPROPRIATE */
    if (lgone == lnodes[*i1 * lnodes_dim1 + 5]) {
        dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, i1, &lgone, nnn, err,
                 noroom);
        if (*noroom || *err) {
          fprintf(stderr,"** PROBLEMS IN SEW2 DELETING NGONE1 LINES **");
            goto L180;
        }
    } else {
        dellxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, j2, &lgone, nnn, err,
                 noroom);
        if (*noroom || *err) {
          fprintf(stderr,"** PROBLEMS IN SEW2 DELETING NGONE1 LINES **");
            goto L180;
        }
    }
/*  ADD ALL LINES STILL HOOKED TO NGONE1 TO THE LIST OF LINES FOR NSTAY1 */
    i__1 = nl;
    for (ii = 1; ii <= i__1; ++ii) {
        ll = l1list[ii - 1];
        if (ll != lgone) {
            addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nstay1, &ll, 
                    nnn, err, noroom);
            if (*noroom || *err) {
              fprintf(stderr,"** PROBLEMS IN SEW2 ADDING NSTAY1 LINES **");
                goto L180;
            }
        }
/* L160: */
    }
/*  DELETE NGONE1 (UNHOOK EVERYTHING FROM IT) */
    for (ii = 1; ii <= 3; ++ii) {
        lxn[ii + (ngone1 << 2)] = 0;
/* L170: */
    }
    lxn[(ngone1 << 2) + 4] = *iavail;
    *iavail = ngone1;
    ++(*navail);
    nuid[ngone1] = 0;
/*  NOW FIX THE LNODES ARRAY FOR BOTH OF THE LOOPS */
    if (ngone2 == *j1) {
        lnodes[nstay2 * lnodes_dim1 + 2] = lnodes[ngone2 * lnodes_dim1 + 2];
        lnodes[lnodes[ngone2 * lnodes_dim1 + 2] * lnodes_dim1 + 3] = nstay2;
    } else {
        lnodes[lnodes[ngone2 * lnodes_dim1 + 3] * lnodes_dim1 + 2] = nstay2;
        lnodes[nstay2 * lnodes_dim1 + 3] = lnodes[ngone2 * lnodes_dim1 + 3];
        lnodes[nstay2 * lnodes_dim1 + 5] = lnodes[ngone2 * lnodes_dim1 + 5];
    }
    if (ngone1 == *j2) {
        lnodes[lnodes[ngone1 * lnodes_dim1 + 3] * lnodes_dim1 + 2] = nstay1;
        lnodes[nstay1 * lnodes_dim1 + 3] = lnodes[ngone1 * lnodes_dim1 + 3];
        lnodes[nstay1 * lnodes_dim1 + 5] = lnodes[ngone1 * lnodes_dim1 + 5];
    } else {
        lnodes[nstay1 * lnodes_dim1 + 2] = lnodes[ngone1 * lnodes_dim1 + 2];
        lnodes[lnodes[ngone1 * lnodes_dim1 + 2] * lnodes_dim1 + 3] = nstay1;
    }
    *i1 = nstay1;
    *i2 = nstay2;
    *j1 = ngone1;
    *j2 = ngone2;
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], i1, err);
    if (*err) {
        goto L180;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &lnodes[*i1 * lnodes_dim1 + 3], err);
    if (*err) {
        goto L180;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &lnodes[lnodes[*i1 * lnodes_dim1 + 3] * 
            lnodes_dim1 + 3], err);
    if (*err) {
        goto L180;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &lnodes[*i1 * lnodes_dim1 + 2], err);
    if (*err) {
        goto L180;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &lnodes[lnodes[*i1 * lnodes_dim1 + 2] * 
            lnodes_dim1 + 2], err);
    if (*err) {
        goto L180;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], i2, err);
    if (*err) {
        goto L180;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &lnodes[*i2 * lnodes_dim1 + 3], err);
    if (*err) {
        goto L180;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &lnodes[lnodes[*i2 * lnodes_dim1 + 3] * 
            lnodes_dim1 + 3], err);
    if (*err) {
        goto L180;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &lnodes[*i2 * lnodes_dim1 + 2], err);
    if (*err) {
        goto L180;
    }
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
            lnodes_offset], &lnodes[lnodes[*i2 * lnodes_dim1 + 2] * 
            lnodes_dim1 + 2], err);
    if (*err) {
        goto L180;
    }
L180:
    return 0;
} /* sew2_ */

#ifdef __cplusplus
        }
#endif

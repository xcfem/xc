/* fixlxn.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int fixlxn_(integer *mxnd, integer *lxn, integer *nxl, 
        integer *nuid, integer *navail, integer *iavail, integer *nnn, 
        integer *lll, integer *nnnold, integer *lllold, logical *err, logical 
        *noroom)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, l, nnn1, iold;
    extern /* Subroutine */ int  addlxn_(integer *, 
            integer *, integer *, integer *, integer *, integer *, integer *, 
            integer *, logical *, logical *);

/* *********************************************************************** */
/*  SUBROUTINE FIXLXN = FIXES THE ADDITIONS TO LXN */
/* *********************************************************************** */
/*     RE-SETUP AVAILABLE LXN-SPACE LINKS */
    /* Parameter adjustments */
    --nuid;
    nxl -= 3;
    lxn -= 5;

    /* Function Body */
    iold = 0;
    *navail = 0;
    i__1 = *nnnold;
    for (i__ = 1; i__ <= i__1; ++i__) {
        if (lxn[(i__ << 2) + 1] == 0) {
            if (iold <= 0) {
                *iavail = i__;
            } else {
                lxn[(iold << 2) + 4] = i__;
            }
            iold = i__;
            ++(*navail);
        }
/* L100: */
    }
    if (iold <= 0) {
        *iavail = *nnn + 1;
    } else {
        lxn[(iold << 2) + 4] = *nnn + 1;
    }
    *navail += *mxnd - *nnn;
    if (*nnn < *mxnd) {
        nnn1 = *nnn + 1;
        i__1 = *mxnd;
        for (i__ = nnn1; i__ <= i__1; ++i__) {
            lxn[(i__ << 2) + 1] = 0;
            lxn[(i__ << 2) + 2] = 0;
            lxn[(i__ << 2) + 3] = 0;
            lxn[(i__ << 2) + 4] = i__ + 1;
/* L110: */
        }
    }
/*     COMPLETE LXN ARRAYS FOR ANY NEW LINES */
    i__1 = *lll;
    for (l = *lllold + 1; l <= i__1; ++l) {
        for (i__ = 1; i__ <= 2; ++i__) {
            addlxn_(mxnd, &lxn[5], &nuid[1], navail, iavail, &nxl[i__ + (l << 
                    1)], &l, nnn, err, noroom);
            if (*err) {
                fprintf(stderr,"ERROR IN FIXLXN - NXL TABLE GENERATION");
                goto L140;
            } else if (*noroom) {
                goto L140;
            }
/* L120: */
        }
/* L130: */
    }
L140:
    return 0;
} /* fixlxn_ */

#ifdef __cplusplus
        }
#endif

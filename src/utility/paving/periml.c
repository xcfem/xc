/* periml.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int periml_(integer *nbnode, integer *mxnd, integer *nper, 
        integer *istart, integer *mln, xc_float *xn, xc_float *yn, xc_float *zn, integer *
        lxk, integer *kxl, integer *nxl, integer *lxn, xc_float *angle, xc_float *
        bnsize, integer *lnodes, integer *lperim, integer *lll, integer *
        lllold, xc_float *xmin, xc_float *xmax, xc_float *ymin, xc_float *ymax, xc_float *zmin, 
        xc_float *zmax, char *dev1, integer *kreg, logical *err, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, i__1;
    xc_float r__1, r__2, r__3, r__4;

    /* Builtin functions */
    

    /* Local variables */
    static integer i__, n0, n2, n1s, iend, node0, node1, node2, kount;
    extern /* Subroutine */ int  lupang_(integer *, 
            integer *, xc_float *, xc_float *, xc_float *, integer *, integer *, integer *
            , integer *, integer *, xc_float *, integer *, integer *, integer *, 
            xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, xc_float *, char *, integer *,
             logical *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE PERIML = LINKS THE PERIMETER OF A REGION TOGETHER FOR */
/*                      THE FILL ROUTINES */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NPER  = NUMBER OF PERIMETER NODES */
/*     ERR   = .TRUE. IF ERRORS WERE ENCOUNTERED */
/*     XN    = GLOBAL X VALUES OF NODES */
/*     YN    = GLOBAL Y VALUES OF NODES */
/*     ZN    = GLOBAL Z VALUES OF NODES */
/*     LXK   = LINES PER ELEMENT */
/*     KXL   = ELEMENTS PER LINE */
/*     NXL   = NODES PER LINE */
/*     LXN   = LINES PER NODE */
/*  NOTE: */
/*     FOR *XN TABLES A NEGATIVE FLAG IN THE FOURTH COLUMN MEANS */
/*     GO TO THAT ROW FOR A CONTINUATION OF THE LIST.  IN THAT ROW */
/*     THE FIRST ELEMENT WILL BE NEGATED TO INDICATE THAT THIS IS */
/*     A CONTINUATION ROW. */
/*     A NEGATIVE FLAG IN THE SECOND COLUMN OF THE LXN ARRAY MEANS */
/*     THAT THIS NODE IS AN EXTERIOR BOUNDARY NODE. */
/* *********************************************************************** */
    /* Parameter adjustments */
    --lperim;
    bnsize -= 3;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --zn;
    --yn;
    --xn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *err = FALSE_;
    *lllold = *lll;
    iend = *nper + *istart - 1;
    i__1 = iend;
    for(i__= *istart; i__<=i__1; ++i__)
      {
        node1 = lperim[i__];
        if (i__ == iend)
          {
            node0 = lperim[i__ - 1];
            node2 = lperim[*istart];
          }
        else if (i__ == *istart)
          {
            node0 = lperim[iend];
            node2 = lperim[i__ + 1];
          }
        else
          {
            node0 = lperim[i__ - 1];
            node2 = lperim[i__ + 1];
          }
        ++(*lll);
/*  FILL UP THE NODES PER LINE ARRAY */
        nxl[(*lll << 1) + 1] = node1;
        nxl[(*lll << 1) + 2] = node2;
/*  FILL UP THE LINES PER NODE ARRAY */
        lxn[(node1 << 2) + 1] = *lll;
        if (i__ == *istart) {
            lxn[(node1 << 2) + 2] = -(iend - *istart + 1 + *lllold);
        } else {
            lxn[(node1 << 2) + 2] = 1 - *lll;
        }
        lxn[(node1 << 2) + 3] = 0;
        lxn[(node1 << 2) + 4] = 0;
/*  THE LNODES ARRAY IS DOCUMENTED IN THE ADDROW ROUTINE */
        lnodes[node1 * lnodes_dim1 + 1] = 0;
        lnodes[node1 * lnodes_dim1 + 2] = node0;
        lnodes[node1 * lnodes_dim1 + 3] = node2;
        lnodes[node1 * lnodes_dim1 + 4] = 1;
        lnodes[node1 * lnodes_dim1 + 5] = *lll;
        lnodes[node1 * lnodes_dim1 + 6] = 0;
        lnodes[node1 * lnodes_dim1 + 7] = 0;
        lnodes[node1 * lnodes_dim1 + 8] = 0;
/* L100: */
    }
/*  SET ALL THE INTERIOR ANGLES */
    lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &
            lxn[5], nper, &angle[1], &lnodes[lnodes_offset], &lperim[*istart],
             lll, xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, err, (ftnlen)3);
    if (*err) {
        goto L120;
    }
/*  SET UP THE CORRECT SIZE ARRAY */
    n1s = lperim[*istart];
    kount = 0;
L110:
    n0 = lnodes[n1s * lnodes_dim1 + 2];
    n2 = lnodes[n1s * lnodes_dim1 + 3];
    n1s = n2;
    if (n1s == lperim[*istart]) {
        goto L120;
    }
    ++kount;
    if (kount > *nper) {
        fprintf(stderr," ** ERROR IN PERIML GETTING NODE SIZES ** \n");
        *err = TRUE_;
        goto L120;
    }
/* Computing 2nd power */
    r__1 = xn[n1s] - xn[n0];
/* Computing 2nd power */
    r__2 = yn[n1s] - yn[n0];
/* Computing 2nd power */
    r__3 = xn[n1s] - xn[n2];
/* Computing 2nd power */
    r__4 = yn[n1s] - yn[n2];
    bnsize[(n1s << 1) + 1] = (sqrt(r__1 * r__1 + r__2 * r__2) + sqrt(r__3 * 
            r__3 + r__4 * r__4)) * (xc_float).5;
    bnsize[(n1s << 1) + 2] = (xc_float)1.;
    goto L110;
L120:
    return 0;
} /* periml_ */

#ifdef __cplusplus
        }
#endif

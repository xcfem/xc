/* getsiz.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getsiz_(real *xnold, real *ynold, integer *nxkold, 
        integer *linkeg, integer *listeg, real *bmesur, integer *mlink, 
        integer *npnold, integer *npeold, integer *nnxk, real *
        rexmin, real *rexmax, real *reymin, real *reymax, integer *idivis, 
        real *sizmin, real *emax, real *emin, real *x, real *y, real *size)
{
    /* System generated locals */
    integer nxkold_dim1, nxkold_offset, i__1;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__;
    static real b1, b2, d1, e1, e2, e3, e4, d2, d3, d4;
    static integer n1, n2, n3, n4;
    static real x1, x2, y1, y2;
    static integer ic, jc;
    static real dt;
    static integer ix, iy;
    static real xi, xm1, xm2;
    static logical bad;
    static real eta;
    static integer kin, iend, icin, jcin;
    static real delx, dely, dtry, xtry, ytry;
    static integer kelem;
    static real reduc;
    static integer index;
    static real ratio, xemin, yemin, xemax, yemax, dtest, error;
    static integer ibegin;
    extern /* Subroutine */ int  dlpara_(real *, real 
            *, real *, real *, real *, real *, logical *);
    static logical inside;
    extern /* Subroutine */ int invmap_(real *, real *, real *, real *, real *
            , real *, real *, real *, real *, real *, real *, real *, logical 
            *);

/* *********************************************************************** */
/*  SUBROUTINE GETSIZ = GETS THE SIZE OF AN ELEMENT EDGE BASED ON THE */
/*                      OLD MESH SIZE AT THE GIVEN X,Y LOCATION AND THE */
/*                      RELATIVE MEASURE OF THE ERROR ESTIMATOR AT THAT */
/*                      LOCATION */
/* *********************************************************************** */
/*  ASSUME A LINEAR REDUCTION FACTOR FROM R0 TO R1 WHERE R0 IS THE */
/*  DESIRED REDUCTION A 0. NORMALIZED ERROR MEASURE AND R1 IS A DESIRED */
/*  REDUCTION AT 1.0 NORMALIZED ERROR MEASURE */
/*  FIND THE ELEMENT THAT THIS POINT FALLS INTO */
    /* Parameter adjustments */
    linkeg -= 3;
    --bmesur;
    --ynold;
    --xnold;
    --listeg;
    nxkold_dim1 = *nnxk;
    nxkold_offset = 1 + nxkold_dim1;
    nxkold -= nxkold_offset;

    /* Function Body */
    delx = (*rexmax - *rexmin) / (doublereal) (*idivis);
    dely = (*reymax - *reymin) / (doublereal) (*idivis);
    ix = (integer) ((*x - *rexmin) / delx) + 1;
    if (*x >= *rexmax) {
        ix = *idivis;
    }
    iy = (integer) ((*reymax - *y) / dely) + 1;
    if (*y <= *reymin) {
        iy = *idivis;
    }
    index = iy * 10 + ix;
    ibegin = linkeg[(index << 1) + 1];
    iend = ibegin + linkeg[(index << 1) + 2] - 1;
    i__1 = iend;
    for (i__ = ibegin; i__ <= i__1; ++i__) {
        kelem = listeg[i__];
        xemin = xnold[nxkold[kelem * nxkold_dim1 + 1]];
        xemax = xnold[nxkold[kelem * nxkold_dim1 + 1]];
        yemin = ynold[nxkold[kelem * nxkold_dim1 + 1]];
        yemax = ynold[nxkold[kelem * nxkold_dim1 + 1]];
        for (ic = 2; ic <= 4; ++ic) {
/* Computing MIN */
            r__1 = xemin, r__2 = xnold[nxkold[ic + kelem * nxkold_dim1]];
            xemin = dbl_min(r__1,r__2);
/* Computing MAX */
            r__1 = xemax, r__2 = xnold[nxkold[ic + kelem * nxkold_dim1]];
            xemax = dbl_max(r__1,r__2);
/* Computing MIN */
            r__1 = yemin, r__2 = ynold[nxkold[ic + kelem * nxkold_dim1]];
            yemin = dbl_min(r__1,r__2);
/* Computing MAX */
            r__1 = yemax, r__2 = ynold[nxkold[ic + kelem * nxkold_dim1]];
            yemax = dbl_max(r__1,r__2);
/* L100: */
        }
        if (*x < xemin || *x > xemax || *y < yemin || *y > yemax) {
            inside = FALSE_;
        } else {
            invmap_(x, y, &xnold[nxkold[kelem * nxkold_dim1 + 1]], &ynold[
                    nxkold[kelem * nxkold_dim1 + 1]], &xnold[nxkold[kelem * 
                    nxkold_dim1 + 2]], &ynold[nxkold[kelem * nxkold_dim1 + 2]]
                    , &xnold[nxkold[kelem * nxkold_dim1 + 3]], &ynold[nxkold[
                    kelem * nxkold_dim1 + 3]], &xnold[nxkold[kelem * 
                    nxkold_dim1 + 4]], &ynold[nxkold[kelem * nxkold_dim1 + 4]]
                    , &xi, &eta, &inside);
        }
        if (inside) {
            kin = kelem;
            goto L170;
        }
/* L110: */
    }
/*  THERE IS A POSSIBILITY THAT THE POINT IS ON AN ARC WHICH IS NOT */
/*  INCLUDED IN THE ORIGINAL MESH - THIS MUST BE CHECKED. */
    dtest = (float)1e15;
    i__1 = iend;
    for (i__ = ibegin; i__ <= i__1; ++i__) {
        kelem = listeg[i__];
        for (ic = 1; ic <= 4; ++ic) {
            jc = ic + 1;
            if (jc == 5) {
                jc = 1;
            }
            x1 = xnold[nxkold[ic + kelem * nxkold_dim1]];
            x2 = xnold[nxkold[jc + kelem * nxkold_dim1]];
            y1 = ynold[nxkold[ic + kelem * nxkold_dim1]];
            y2 = ynold[nxkold[jc + kelem * nxkold_dim1]];
/*  GET THE PARAMETERS FOR THE LINE */
            dlpara_(&x1, &y1, &x2, &y2, &xm1, &b1, &bad);
/*  GET DISTANCE FOR VERTICAL LINE */
            if (bad) {
                dtry = (r__1 = x1 - *x, dbl_abs(r__1));
                xtry = x1;
                ytry = *y;
/*  GET DISTANCE FOR HORIZONTAL LINE */
            } else if (dbl_abs(xm1) < (float)1e-6) {
                dtry = (r__1 = y1 - *y, dbl_abs(r__1));
                xtry = *x;
                ytry = y1;
/*  GET PERPENDICULAR DISTANCE TO ARBITRARY LINE */
            } else {
                xm2 = (float)-1. / xm1;
                b2 = *y - xm2 * *x;
                xtry = (b2 - b1) / (xm1 - xm2);
                ytry = xm1 * xtry + b1;
/* Computing 2nd power */
                r__1 = *x - xtry;
/* Computing 2nd power */
                r__2 = *y - ytry;
                dtry = sqrt(r__1 * r__1 + r__2 * r__2);
            }
/*  CHECK THE INTERSECTION TO MAKE SURE THAT IT CUTS THE LINE SEGMENT */
/*  WE HAVE */
            if (xtry >= dbl_min(x1,x2) && xtry <= dbl_max(x1,x2) && ytry >= dbl_min(y1,
                    y2) && ytry <= dbl_max(y1,y2)) {
/*  NOW GET THE SHORTEST INTERSECTION AND GET NEEDED SIZE VALUE BASED ON */
/*  THE XTRY AND YTRY LOCATION */
                if (dtry < dtest) {
                    dtest = dtry;
                    inside = TRUE_;
/* Computing 2nd power */
                    r__1 = x1 - x2;
/* Computing 2nd power */
                    r__2 = y1 - y2;
                    dt = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
                    r__1 = xtry - x1;
/* Computing 2nd power */
                    r__2 = ytry - y1;
                    d1 = sqrt(r__1 * r__1 + r__2 * r__2);
                    ratio = d1 / dt;
                    if (ic == 1) {
                        xi = ratio;
                        eta = (float)0.;
                    } else if (ic == 2) {
                        xi = (float)0.;
                        eta = ratio;
                    } else if (ic == 3) {
                        xi = (float)1. - ratio;
                        eta = (float)0.;
                    } else {
                        xi = (float)0.;
                        eta = (float)1. - ratio;
                    }
                    kin = kelem;
                    icin = ic;
                    jcin = jc;
                }
            }
/* L120: */
        }
/* L130: */
    }
/*  NOW CHECK THE ELEMENT THAT HAS BEEN FOUND AND MAKE SURE THAT IT IS */
/*  A ELEMENT ALONG THE SIDE OF THE MESH AND THAT THE EDGE CLOSEST IS */
/*  NOT SHARED BY ANY OTHER ELEMENT. */
    if (inside) {
        i__1 = *npeold;
        for (i__ = 1; i__ <= i__1; ++i__) {
            if (i__ != kin) {
                for (ic = 1; ic <= 4; ++ic) {
                    jc = ic + 1;
                    if (jc == 5) {
                        jc = 1;
                    }
                    if (ic == jcin && jc == icin) {
                        fprintf(stderr,"** ERROR WITH ELEMENT SIDE FOUND BEING INTERIOR TO MESH IN GETSIZ **");
                        inside = FALSE_;
                        goto L160;
                    }
/* L140: */
                }
            }
/* L150: */
        }
L160:
        ;
    }
/*  THE ELEMENT HAS BEEN FOUND - NOW INTERPOLATE THE STRESS VALUE FOR */
/*  THIS LEVEL */
L170:
    if (inside) {
        n1 = nxkold[kin * nxkold_dim1 + 1];
        n2 = nxkold[kin * nxkold_dim1 + 2];
        n3 = nxkold[kin * nxkold_dim1 + 3];
        n4 = nxkold[kin * nxkold_dim1 + 4];
        e1 = bmesur[n1];
        e2 = bmesur[n2];
        e3 = bmesur[n3];
        e4 = bmesur[n4];
        error = e1 + (e2 - e1) * xi + (e4 - e1) * eta + (e1 - e2 + e3 - e4) * 
                xi * eta;
/* Computing 2nd power */
        r__1 = xnold[n2] - xnold[n1];
/* Computing 2nd power */
        r__2 = ynold[n2] - ynold[n1];
        d1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
        r__1 = xnold[n3] - xnold[n2];
/* Computing 2nd power */
        r__2 = ynold[n3] - ynold[n2];
        d2 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
        r__1 = xnold[n4] - xnold[n3];
/* Computing 2nd power */
        r__2 = ynold[n4] - ynold[n3];
        d3 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
        r__1 = xnold[n1] - xnold[n4];
/* Computing 2nd power */
        r__2 = ynold[n1] - ynold[n4];
        d4 = sqrt(r__1 * r__1 + r__2 * r__2);
        reduc = *emax - error * *emax + error * *emin;
/* Computing MAX */
/* Computing MIN */
        r__2 = dbl_min(d1,d2), r__2 = dbl_min(r__2,d3);
        r__1 = dbl_min(r__2,d4) * reduc;
        *size = dbl_max(r__1,*sizmin);
    } else {
/*  ERROR HAS OCCURRED IN FINDING THE ELEMENT */
        fprintf(stderr,"** ERROR - ENCLOSING ELEMENT NOT FOUND IN GETSIZ **");
    }
    return 0;
} /* getsiz_ */

#ifdef __cplusplus
        }
#endif

/* addjut.f -- translated by f2c (version 20160102).
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
#include "fastq_cpp.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int addjut_(integer *mxnd, real *xn, real *yn, integer *lxk, 
	integer *kxl, integer *nxl, integer *lxn, real *angle, integer *
	lnodes, real *xnew, real *ynew, integer *nnn, integer *lll, integer *
	nold, integer *nloop, logical *jutted)
{
    static integer i__;

/* *********************************************************************** */
/*  SUBROUTINE ADDJUT = ADDS A NEW NODE JUTTING OUT FROM AN EXISTING */
/*                      NODE */
/* *********************************************************************** */
    /* Parameter adjustments */
    lnodes -= 8;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --yn;
    --xn;

    /* Function Body */
    ++(*nnn);
    xn[*nnn] = *xnew;
    yn[*nnn] = *ynew;
/*  MAKE LXN AND NXL ARRAYS */
/*  ADD THE NEW NODE'S LINES */
    ++(*lll);
    nxl[(*lll << 1) + 1] = *nnn;
    nxl[(*lll << 1) + 2] = *nold;
    for (i__ = 1; i__ <= 4; ++i__) {
	lxn[i__ + (*nnn << 2)] = 0;
/* L100: */
    }
    kxl[(*lll << 1) + 1] = 0;
    kxl[(*lll << 1) + 2] = 0;
/*  REDO THE LNODES ARRAY */
    lnodes[*nnn * 7 + 1] = 0;
    lnodes[*nnn * 7 + 2] = *nold;
    lnodes[*nnn * 7 + 3] = *nold;
    lnodes[*nnn * 7 + 4] = -1;
    lnodes[*nnn * 7 + 5] = *lll;
    lnodes[*nold * 7 + 1] = 0;
    lnodes[*nold * 7 + 3] = *nnn;
    lnodes[*nold * 7 + 5] = *lll;
    *nloop += 2;
    *jutted = TRUE_;
    return 0;
} /* addjut_ */

#ifdef __cplusplus
	}
#endif

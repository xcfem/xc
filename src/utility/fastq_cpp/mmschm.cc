/* mmschm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int mmschm_(integer *nper, integer *kkk, integer *lll, 
	integer *nnn, integer *ml, integer *ms, integer *nspr, integer *
	islist, integer *nint, integer *ifline, integer *nlps, integer *
	illist, integer *linkl, integer *links, integer *mxnper, integer *
	maxprm, integer *max3, integer *mxnd, real *x, real *y, integer *nid, 
	integer *nnps, real *angle, real *xn, real *yn, integer *nuid, 
	integer *lxk, integer *kxl, integer *nxl, integer *lxn, integer *
	iavail, integer *navail, logical *ccw, logical *real__, char *schstr, 
	integer *m1, logical *err, ftnlen schstr_len)
{
    static integer m2;
    static logical norm;
    extern /* Subroutine */ int getm1_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, integer *, real *
	    , integer *, char *, integer *, logical *, logical *, logical *, 
	    logical *, ftnlen), rmesh_(integer *, integer *, real *, real *, 
	    integer *, real *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *), mesage_(char *, ftnlen);
    static integer kkkold, lllold, nnnold;

/* *********************************************************************** */
/*  MMSCHM - "M" MESH SCHEME; CALCULATE A REGULAR RECTANGULAR MESH */
/* *********************************************************************** */
/*  CALCULATE THE BASE OF THE RECTANGLE FOR THE REGION */
    /* Parameter adjustments */
    linkl -= 3;
    --nint;
    links -= 3;
    --illist;
    --nlps;
    --ifline;
    --islist;
    --angle;
    --y;
    --x;
    --nid;
    --nnps;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;

    /* Function Body */
    getm1_(ml, ms, max3, nspr, &islist[1], &nint[1], &ifline[1], &nlps[1], &
	    illist[1], &linkl[3], &links[3], &x[1], &y[1], &nid[1], &nnps[1], 
	    &angle[1], nper, schstr, m1, ccw, &norm, real__, err, (ftnlen)72);
    if (norm) {
	mesage_("FORCED RECTANGLE PRIMITIVE PROCESSING USED", (ftnlen)42);
    } else {
	mesage_("GENERAL RECTANGLE PRIMITIVE PROCESSING USED", (ftnlen)43);
    }
    m2 = *nper / 2 - *m1;
/*  CALCULATE A REGULAR MAPPED "RECTANGULAR" MESH */
    kkkold = *kkk;
    lllold = *lll;
    nnnold = *nnn;
    rmesh_(nper, mxnd, &x[1], &y[1], &nid[1], &xn[1], &yn[1], &nuid[1], &lxk[
	    5], &kxl[3], &nxl[3], &lxn[5], m1, &m2, kkk, &kkkold, nnn, &
	    nnnold, lll, &lllold, iavail, navail, err);
    return 0;
} /* mmschm_ */

#ifdef __cplusplus
	}
#endif

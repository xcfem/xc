/* initdg.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int initdg_(integer *mcom, integer *icom, integer *jcom, 
	char *cin, real *rin, integer *iin, integer *kin, integer *idump, 
	real *xx1, real *yy1, real *scale, real *ct, real *st, real *x1, real 
	*x2, real *y1, real *y2, logical *drwtab, logical *snap, ftnlen 
	cin_len)
{
    static real x, y;
    static integer iz;
    static real xx2, yy2;
    static logical ians;
    extern /* Subroutine */ int dpread_(real *, real *, char *, ftnlen), 
	    mesage_(char *, ftnlen), frefld_(integer *, integer *, char *, 
	    integer *, integer *, integer *, integer *, char *, integer *, 
	    real *, ftnlen, ftnlen), tabint_(real *, real *, real *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, logical *)
	    , pltbel_();
    static integer iostat;
    extern /* Subroutine */ int pltflu_();
    static char button[1];
    extern /* Subroutine */ int intrup_(char *, logical *, integer *, integer 
	    *, integer *, char *, integer *, real *, integer *, ftnlen, 
	    ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE INITDG = INITIALIZES THE DIGITIZING TABLET */
/* *********************************************************************** */
    /* Parameter adjustments */
    --kin;
    --iin;
    --rin;
    cin -= 72;

    /* Function Body */
    iz = 0;
/*  CHECK TO MAKE SURE THAT THE DRAWING IS NOT BEING TOGGLED */
    if (*drwtab) {
	mesage_("DRAWING INITIALIZATION IS ALREADY ACTIVE", (ftnlen)40);
	intrup_("TOGGLE ALL DRAWING INITIALIZATION OFF", &ians, mcom, icom, 
		jcom, cin + 72, &iin[1], &rin[1], &kin[1], (ftnlen)37, (
		ftnlen)72);
	if (ians) {
	    *drwtab = FALSE_;
	    tabint_(x1, x2, y1, y2, ct, st, scale, xx1, yy1, &xx2, &yy2, 
		    drwtab);
	    return 0;
	}
    }
/*  GET THE ZOOM LIMITS */
    mesage_(" ", (ftnlen)1);
    if (*icom > *jcom) {
	frefld_(&iz, &iz, "ENTER DRAWING XMIN, XMAX, YMIN, YMAX:", mcom, &
		iostat, jcom, &kin[1], cin + 72, &iin[1], &rin[1], (ftnlen)37,
		 (ftnlen)72);
	*icom = 1;
    }
    if (*jcom - *icom + 1 >= 4) {
	*snap = TRUE_;
	*x1 = rin[*icom];
	*x2 = rin[*icom + 1];
	*y1 = rin[*icom + 2];
	*y2 = rin[*icom + 3];
	*icom += 4;
    } else {
	mesage_("NOT ENOUGH INFORMATION DEFINED TO SPECIFY DRAWING LIMITS", (
		ftnlen)56);
	mesage_("INITIALIZATION ABORTED", (ftnlen)22);
	mesage_(" ", (ftnlen)1);
	return 0;
    }
/*  GET THE DIGITIZING POINTS */
    mesage_("NOW DIGITIZE THOSE 2 POINTS", (ftnlen)27);
    mesage_("       PUSH \"PUCK - 1\" FOR LOWER LEFT", (ftnlen)37);
    mesage_("       PUSH \"PUCK - 2\" FOR UPPER RIGHT", (ftnlen)38);
    mesage_("       PUSH \"PUCK - E\" TO END", (ftnlen)29);
L100:
    dpread_(&x, &y, button, (ftnlen)1);
    if (*(unsigned char *)button == '1') {
	*xx1 = x;
	*yy1 = y;
	mesage_("LOWER LEFT INPUT", (ftnlen)16);
	goto L100;
    } else if (*(unsigned char *)button == '2') {
	xx2 = x;
	yy2 = y;
	mesage_("UPPER RIGHT INPUT", (ftnlen)17);
	goto L100;
    } else if (*(unsigned char *)button == 'E') {
	pltbel_();
	pltflu_();
    }
    if (yy2 - *yy1 == (float)0. && xx2 - *xx1 == (float)0. || *y2 - *y1 == (
	    float)0. && *x2 - *x1 == (float)0.) {
	mesage_("BAD INITIALIZATION  -  INITIALIZATION ABORTED", (ftnlen)45);
	mesage_(" ", (ftnlen)1);
	pltbel_();
	pltflu_();
	return 0;
    }
    *drwtab = TRUE_;
    tabint_(x1, x2, y1, y2, ct, st, scale, xx1, yy1, &xx2, &yy2, drwtab);
    mesage_("INITIALIZATION COMPLETE", (ftnlen)23);
    mesage_(" ", (ftnlen)1);
    return 0;
} /* initdg_ */

#ifdef __cplusplus
	}
#endif

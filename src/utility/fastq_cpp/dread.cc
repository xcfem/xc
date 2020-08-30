/* dread.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int dread_(real *x, real *y, char *button, real *x1, real *
	y1, real *xx1, real *yy1, real *scale, real *ct, real *st, ftnlen 
	button_len)
{
    static real xnew, ynew;
    extern /* Subroutine */ int dpread_(real *, real *, char *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE DREAD = SETS ALL PARAMETERS UP FOR READING FROM A DIGI-PAD */
/* *********************************************************************** */
/*  SUBROUTINES CALLED: */
/*     DPREAD   = READS INPUT FROM A DIGI-PAD DIGITIZER */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     X      = THE X LOCATION IN USER COORDINATES */
/*     Y      = THE Y LOCATION IN USER COORDINATES */
/*     BUTTON = THE MOUSE BUTTON PUSHED */
/*     X1     = THE X ZERO SHIFT IN USER COORDINATES */
/*     Y1     = THE Y ZERO SHIFT IN USER COORDINATES */
/*     XX1    = THE X ZERO SHIFT IN DIGITIZED COORDINATES */
/*     YY1    = THE Y ZERO SHIFT IN DIGITIZED COORDINATES */
/*     SCALE  = THE SCALE FACTOR FROM DIGITIZED TO USER COORDINATES */
/*     CT     = THE COSINE OF THE ANGLE OF THE DRAWING ON THE PAD */
/*     ST     = THE SINE OF THE ANGLE OF THE DRAWING ON THE PAD */
/*     XNEW   = THE NEW DIGITIZED X VALUE BEFORE TRANSFORMATIONS */
/*     YNEW   = THE NEW DIGITIZED Y VALUE BEFORE TRANSFORMATIONS */
/*     NCB    = THE NUMBER OF BUTTONS ON THE MOUSE  (BIT-PAD-ONE) */
/*     DEL    = THE DELTA DISTANCE BETWEEN ACCEPTABLE POINTS  (TALOS) */
/* *********************************************************************** */
    dpread_(&xnew, &ynew, button, (ftnlen)1);
    xnew -= *xx1;
    ynew -= *yy1;
    *x = *x1 + *scale * (*ct * xnew + *st * ynew);
    *y = *y1 + *scale * (-(*st) * xnew + *ct * ynew);
    return 0;
} /* dread_ */

#ifdef __cplusplus
	}
#endif

/* usrsym.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/* Common Block Declarations */

struct {
    char cdebug[8];
} debugc_;

#define debugc_1 debugc_

struct {
    integer idebug;
} debugn_;

#define debugn_1 debugn_

/* Table of constant values */

static integer c__1 = 1;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int usrsym_(char *isytyp, logical *is3dim, real *x0, real *
	y0, real *z0, real *evar, real *scale, ftnlen isytyp_len)
{
    /* System generated locals */
    real r__1, r__2, r__3, r__4;

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen);
    double cos(doublereal), sin(doublereal);

    /* Local variables */
    static real sx0, sy0;
    extern /* Subroutine */ int mpd2vc_(integer *, real *, real *, real *, 
	    real *), mpd2sy_(integer *, real *, real *, char *, ftnlen);

/* ======================================================================= */
/*   --*** USRSYM *** (DETOUR) Plot element symbol (slip/angle version) */
/*   --   Written by Amy Gilkey - revised 06/15/87 */
/*   --   D. P. Flanagan, 12/08/83 */
/*   -- */
/*   --If the element variable name begins with 'CRACK', USRSYM displays */
/*   --an 'X' to indicate a sliding joint or an 'O' to indicate an opening */
/*   --joint (Slip plot). */
/*   -- */
/*   --If the element variable name begins with 'ANGLE', USRSYM draws a */
/*   --line segment at a specified angle (Angle plot). A '*' is drawn to */
/*   --indicate rubblization. */
/*   -- */
/*   --Parameters: */
/*   --   ISYTYP - IN - the symbol type (as in MODTYP of /DETOPT/) */
/*   --   IS3DIM - IN - true iff 3D versus 2D */
/*   --   X0, Y0, Z0 - IN - the element center coordinates */
/*   --   EVAR - IN - the element variable */
/*   --      Crack angle for angle plot: */
/*   --         >360 indicates no crack */
/*   --      Crack state indicator for slip plot: */
/*   --         -1 = joint sliding */
/*   --          0 = joint stationary */
/*   --         +1 = joint opening */
/*   --   SCALE - IN - the scale factor for crack angle, */
/*   --      default is device to window conversion */
    if (s_cmp(isytyp, "ANGLE", isytyp_len, (ftnlen)5) == 0) {
/*      --Angle plot */
	if (*evar > (float)360.) {
	    return 0;
	} else if (*evar < (float)-360.) {
/*         --Indicate multiple failures */
#if NeedsDoubleEscape
	    mpd2sy_(&c__1, x0, y0, "\\CX", (ftnlen)3);
	    mpd2sy_(&c__1, x0, y0, "\\CCS", (ftnlen)4);
#else
	    mpd2sy_(&c__1, x0, y0, "CX", (ftnlen)2);
	    mpd2sy_(&c__1, x0, y0, "CCS", (ftnlen)3);
#endif
	} else {
/*         --Indicate crack angle */
	    sx0 = *scale * (float).01 * cos(*evar * (float).0174532925);
	    sy0 = *scale * (float).01 * sin(*evar * (float).0174532925);
	    r__1 = *x0 - sx0;
	    r__2 = *y0 - sy0;
	    r__3 = *x0 + sx0;
	    r__4 = *y0 + sy0;
	    mpd2vc_(&c__1, &r__1, &r__2, &r__3, &r__4);
	}
    } else if (s_cmp(isytyp, "CRACK", isytyp_len, (ftnlen)5) == 0) {
/*      --Slip plot */
	if (*evar == (float)0.) {
	    return 0;
	} else if (*evar < (float)0.) {
/*         --Indicate sliding joint */
#if NeedsDoubleEscape
	    mpd2sy_(&c__1, x0, y0, "\\CX", (ftnlen)3);
#else
	    mpd2sy_(&c__1, x0, y0, "CX", (ftnlen)2);
#endif
	} else {
/*         --Indicate opening joint */
#if NeedsDoubleEscape
	    mpd2sy_(&c__1, x0, y0, "\\CCI", (ftnlen)4);
#else
	    mpd2sy_(&c__1, x0, y0, "CCI", (ftnlen)3);
#endif
	}
    } else if (s_cmp(debugc_1.cdebug, "SYMBOL", (ftnlen)8, (ftnlen)6) == 0) {
#if NeedsDoubleEscape
	mpd2sy_(&c__1, x0, y0, "\\cx", (ftnlen)3);
#else
	mpd2sy_(&c__1, x0, y0, "cx", (ftnlen)2);
#endif
    }
    return 0;
} /* usrsym_ */

#ifdef __cplusplus
	}
#endif

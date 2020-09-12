/* symbol.f -- translated by f2c (version 20160102).
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

/* Common Block Declarations */

struct {
    integer ndim, numnp, numel, nelblk, nvarhi, nvargl, nvarnp, nvarel, 
	    nvarns, nvarss, nsteps, nstepw, nshl;
} dbnums_;

#define dbnums_1 dbnums_

struct {
    logical is3dim;
    integer nnpsur, numnpf, llnset;
} d3nums_;

#define d3nums_1 d3nums_

struct {
    real vecscl;
} etcopt_;

#define etcopt_1 etcopt_

struct {
    real dtw, vwscl;
} devdat_;

#define devdat_1 devdat_

/* Table of constant values */

static integer c__2 = 2;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int symbol_bl__(char *isytyp, real *varfac, integer *lenf, 
	integer *nlnkf, logical *hidef, real *xf, real *yf, real *zf, logical 
	*isvok, integer *blkcol, integer *idelb, ftnlen isytyp_len)
{
    /* System generated locals */
    integer i__1, i__2;
    real r__1;

    /* Local variables */
    static integer ifac, ielb;
    static logical ldum;
    static real sscl, symsz;
    extern logical grabrt_();
    extern /* Subroutine */ int ugrcol_(integer *, integer *), pltflu_();
    extern logical pltgtt_(integer *, real *), pltstt_(integer *, real *);
    extern /* Subroutine */ int usrsym_(char *, logical *, real *, real *, 
	    real *, real *, real *, ftnlen);

/* ======================================================================= */
/*   --*** SYMBOL *** (DETOUR) Plot element symbol */
/*   --   Written by Amy Gilkey, revised 10/12/87 */
/*   --   D. P. Flanagan, 12/08/83 */
/*   -- */
/*   --SYMBOL drives the user symbol interface for element variables. */
/*   --It processes each element by element block, computing scaling factors */
/*   --and element information, then calling the user symbol routine. */
/*   --Only elements of selected element blocks are drawn. */
/*   -- */
/*   --Parameters: */
/*   --   ISYTYP - IN - the symbol type (as in MODTYP of /DETOPT/) */
/*   --   VARFAC - IN - the face variable values */
/*   --   LENF - IN - the cumulative face counts by element block */
/*   --   NLNKF - IN - the number of nodes per face */
/*   --   HIDEF(i) - IN - true iff face i is hidden (3D only) */
/*   --   XF, YF, ZF - IN - the face center coordinates */
/*   --   ISVOK - IN - ISVOK(i) is true iff the symbol variables are defined */
/*   --      for element block i */
/*   --   BLKCOL - IN - the user selected colors of the element blocks. */
/*   --                    BLKCOL(0) = 1 if the user defined material */
/*   --                                colors should be used in mesh plots. */
/*   --                              = -1 if program selected colors should */
/*   --                                be used. */
/*   --                    BLKCOL(i) = the user selected color of element */
/*   --                               block i: */
/*   --                                  -2 - no color selected by user. */
/*   --                                  -1 - black */
/*   --                                   0 - white */
/*   --                                   1 - red */
/*   --                                   2 - green */
/*   --                                   3 - yellow */
/*   --                                   4 - blue */
/*   --                                   5 - cyan */
/*   --                                   6 - magenta */
/*   --   * - return statement if the cancel function is active */
/*   -- */
/*   --Common Variables: */
/*   --   Uses NELBLK of /DBNUMS/ */
/*   --   Uses IS3DIM of /D3NUMS/ */
/*   --   Uses VECSCL of /ETCOPT/ */
/*   --   Uses DTW, VWSCL of /DEVDAT/ */
/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/*      --NDIM - the number of coordinates per node */
/*      --NUMNP - the number of nodes */
/*      --NUMEL - the number of elements */
/*      --NELBLK - the number of element blocks */
/*      --NVARGL - the number of global variables */
/*      --NVARNP - the number of nodal variables */
/*      --NVAREL - the number of element variables */
/*      --NSTEPS - the number of database time steps */
/*      --NSTEPW - the number of whole (versus history) database time steps */
/*      --nshl   - the number of shell elements */
    /* Parameter adjustments */
    --idelb;
    --isvok;
    --zf;
    --yf;
    --xf;
    --hidef;
    --nlnkf;
    --varfac;

    /* Function Body */
    sscl = etcopt_1.vecscl * devdat_1.dtw;
/*   --Set symbol size, must be reset before exit */
    ldum = pltgtt_(&c__2, &symsz);
    r__1 = etcopt_1.vecscl * symsz;
    ldum = pltstt_(&c__2, &r__1);
    i__1 = dbnums_1.nelblk;
    for (ielb = 1; ielb <= i__1; ++ielb) {
	if (isvok[ielb]) {
/*         --Set the symbol color */
/*            CALL UGRCOL (IDELB(IELB), BLKCOL) */
	    ugrcol_(&ielb, blkcol);
	    i__2 = lenf[ielb];
	    for (ifac = lenf[ielb - 1] + 1; ifac <= i__2; ++ifac) {
		if (d3nums_1.is3dim) {
		    if (hidef[ifac]) {
			goto L100;
		    }
		}
		if (grabrt_()) {
		    goto L120;
		}
/*            --Call symbol routine */
		usrsym_(isytyp, &d3nums_1.is3dim, &xf[ifac], &yf[ifac], &zf[
			ifac], &varfac[ifac], &sscl, isytyp_len);
L100:
		;
	    }
	    pltflu_();
	}
/* L110: */
    }
/*   --Reset symbol size */
    ldum = pltstt_(&c__2, &symsz);
    return 0;
L120:
/*   --Reset symbol size */
    ldum = pltstt_(&c__2, &symsz);
    return 1;
} /* symbol_bl__ */

#ifdef __cplusplus
	}
#endif

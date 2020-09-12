/* ugrcol.f -- translated by f2c (version 20160102).
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
    char devnam[6], devcod[16];
} grpcoc_;

#define grpcoc_1 grpcoc_

struct {
    integer icurdv;
    logical ishard, devok[2], talkok[2];
    integer nsnap[2], ifont[2];
    logical softch[2], autopl[2];
    integer maxcol[2], numcol[4]	/* was [2][2] */, mapalt[2], mapuse[2]
	    ;
} grpcom_;

#define grpcom_1 grpcom_

struct {
    integer ndim, numnp, numel, nelblk, nvarhi, nvargl, nvarnp, nvarel, 
	    nvarns, nvarss, nsteps, nstepw, nshl;
} dbnums_;

#define dbnums_1 dbnums_

struct {
    integer lstmap, lstdev, lstcol, iblack, iwhite;
} grcol_;

#define grcol_1 grcol_

struct {
    logical bcolch;
} bcolr_;

#define bcolr_1 bcolr_

struct {
    integer idfor, idfort, idbck, idbckt;
    real colfor;
} plcol2_;

#define plcol2_1 plcol2_

struct {
    real colmap[11];
} cmap_;

#define cmap_1 cmap_

struct {
    char collst[352];
} clst_;

#define clst_1 clst_

/* Table of constant values */

static integer c__1 = 1;
static integer c__6 = 6;
static integer c__44 = 44;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int ugrcol_(integer *indx, integer *blkcol)
{
    /* Initialized data */

    static integer lstd2 = 0;
    static char blcolr[8+1] = "BLACK   ";
    static char whcolr[8+1] = "WHITE   ";

    /* System generated locals */
    integer i__1;
    real r__1;

    /* Local variables */
    static integer i__, ncol;
    static logical ldum;
    static real rblack;
    static integer frecol[9];
    extern logical pltstg_(integer *, real *), pltstd_(integer *, real *), 
	    plticl_(char *, real *, ftnlen);
    static logical colspc[9];
    static integer numfre;
    static real rwhite, rcolor;
    static integer icolor, nblnsp;

/* ======================================================================= */
/*   --*** UGRCOL *** (BLOT) Set color (PLT) */
/*   --   Written by John Glick - 11/29/88 */
/*   -- */
/*   --UGRCOL is similar to the GRCOLR routine of GRPLIB in that it */
/*   --sets the color of lines depending on the passed index. */
/*   --The colors are chosen differently than GRCOLR, however. */
/*   --If appropriate, the BLKCOL array is used to select the color. */
/*   -- */
/*   -- */
/*   --Parameters: */
/*   --   INDX - IN - the color index or block id: */
/*   --      If = -1, black is chosen as the color. */
/*   --      If = 0, the foreground color is chosen as the color. */
/*   --              If the foreground color is not available on */
/*   --              the current device, white is chosen. */
/*   --      If > 0, it is assumed to be an element block identifier. */
/*   --              If BLKCOL(INDX) > -2, then BLKCOL(INDX) is the */
/*   --              color index chosen (if it is a supported color on */
/*   --              the current device). */
/*   --   BLKCOL - IN - the user selected colors of the element blocks. */
/*   --                    BLKCOL(0) = 1 if the user defined material */
/*   --                                colors should be used in mesh plots. */
/*   --                              = -1 if program selected colors should */
/*   --                                not be used. */
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
/*   -- */
/*   --Common Variables: */
/*   --   Uses ICURDV, MAXCOL, NUMCOL, MAPUSE of /GRPCOM/ */
/*   --Routines Called: */
/*   --   PLTICL - (PLTLIB) Get color index from name */
/*   --   PLTSTD - (PLTLIB) Set device parameter */
/*   --      1 = (KCOLOR) set color */
/*   --   PLTSTG - (PLTLIB) Set graph parameter */
/*   --      6 = (KCOLIN) set line color for PLTGRH lines */
/*   --      44 = (KCOSYM) set symbol color for PLTGRH lines */
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
/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/*      --LSTMAP - the last color map (saved so that change in */
/*      --   color map can be detected) */
/*      --LSTDEV - the last current device */
/*      --LSTCOL - the last color set */
/*      --IBLACK, IWHITE - the color indices for black and white */
/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/*     --MXTVAR - the maximum number of plot variables */
/* -- MXNAME - the maximum allowed size for an entity or variable name... */
/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/*        -- COLMAP -- Color indexes of currently mapped colors */
/*        -- COLLST -- character names of currently mapped colors */
    if (grpcom_1.icurdv != grcol_1.lstdev) {
	grcol_1.lstdev = grpcom_1.icurdv;
	grcol_1.lstmap = 0;
/*      --Reset last color */
	grcol_1.lstcol = -999;
/*      --Set device black and white */
	if (grpcom_1.maxcol[grpcom_1.icurdv - 1] > 0) {
	    ldum = plticl_(whcolr, &rwhite, (ftnlen)8);
	    grcol_1.iwhite = (integer) rwhite;
	    ldum = plticl_(blcolr, &rblack, (ftnlen)8);
	    grcol_1.iblack = (integer) rblack;
	} else {
	    grcol_1.iblack = 0;
	    grcol_1.iwhite = 1;
	}
    } else if (grpcom_1.mapuse[grpcom_1.icurdv - 1] != grcol_1.lstmap) {
/*      --Reset last color */
	grcol_1.lstcol = -999;
	grcol_1.lstmap = grpcom_1.mapuse[grpcom_1.icurdv - 1];
    }
    if (grpcom_1.icurdv != lstd2) {
/*           Get mapping of colors */
	for (i__ = 1; i__ <= 9; ++i__) {
	    if (plticl_(clst_1.collst + (i__ + 1 << 5), &rcolor, (ftnlen)32)) 
		    {
		cmap_1.colmap[i__ - 1] = rcolor;
	    } else {
		cmap_1.colmap[i__ - 1] = (float)-1.;
	    }
/* L100: */
	}
    }
    if (bcolr_1.bcolch || grpcom_1.icurdv != lstd2) {
	bcolr_1.bcolch = FALSE_;
	for (i__ = 1; i__ <= 9; ++i__) {
	    colspc[i__ - 1] = FALSE_;
/* L110: */
	}
	if (blkcol[0] == 1) {
	    i__1 = dbnums_1.nelblk;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (blkcol[i__] > 0 && cmap_1.colmap[blkcol[i__] - 1] != (
			float)-1.) {
		    colspc[blkcol[i__] - 1] = TRUE_;
		}
/* L120: */
	    }
	}
	numfre = 0;
	for (i__ = 1; i__ <= 9; ++i__) {
	    if (! colspc[i__ - 1] && cmap_1.colmap[i__ - 1] != (float)-1.) {
		++numfre;
		frecol[numfre - 1] = i__;
	    }
/* L130: */
	}
    }
    if (grpcom_1.icurdv != lstd2) {
	lstd2 = grpcom_1.icurdv;
    }
    ncol = grpcom_1.numcol[grpcom_1.mapuse[grpcom_1.icurdv - 1] + (
	    grpcom_1.icurdv << 1) - 2];
    if (blkcol[0] == 1) {
	if (*indx == -1) {
	    icolor = grcol_1.iblack;
	} else if (*indx == 0) {
	    plcol2_1.idfort = plcol2_1.idfor;
	    if (plcol2_1.idfor == 1) {
		icolor = grcol_1.iblack;
	    } else if (plcol2_1.idfor == 2) {
		icolor = grcol_1.iwhite;
	    } else if (cmap_1.colmap[plcol2_1.idfor - 3] >= (float)0.) {
		icolor = (integer) cmap_1.colmap[plcol2_1.idfor - 3];
	    } else {
		icolor = grcol_1.iwhite;
		plcol2_1.idfort = 2;
	    }
	    plcol2_1.colfor = (real) icolor;
	} else if (blkcol[*indx] > 0 && cmap_1.colmap[blkcol[*indx] - 1] != (
		float)-1.) {
	    icolor = (integer) cmap_1.colmap[blkcol[*indx] - 1];
	} else if (blkcol[*indx] == 0) {
	    icolor = grcol_1.iwhite;
	} else if (blkcol[*indx] == -1) {
	    icolor = grcol_1.iblack;
	} else {
	    if (numfre > 0) {
		nblnsp = 0;
		i__1 = *indx;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    if (blkcol[i__] == -2) {
			++nblnsp;
		    } else if (blkcol[i__] > 0) {
			if (cmap_1.colmap[blkcol[i__] - 1] == (float)-1.) {
			    ++nblnsp;
			}
		    }
/* L140: */
		}
		icolor = (integer) cmap_1.colmap[frecol[(nblnsp - 1) % numfre]
			 - 1];
	    } else {
		plcol2_1.idfort = plcol2_1.idfor;
		if (plcol2_1.idfor == 1) {
		    icolor = grcol_1.iblack;
		} else if (plcol2_1.idfor == 2) {
		    icolor = grcol_1.iwhite;
		} else if (cmap_1.colmap[plcol2_1.idfor - 3] >= (float)0.) {
		    icolor = (integer) cmap_1.colmap[plcol2_1.idfor - 3];
		} else {
		    icolor = grcol_1.iwhite;
		    plcol2_1.idfort = 2;
		}
		plcol2_1.colfor = (real) icolor;
	    }
	}
    } else {
	if (*indx > 0 && ncol > 0) {
/* ... This is a temporary kludge by GDS for Ultrix - */
	    icolor = (*indx - 1) % ncol + 1;
/*            ICOLOR = MOD (INDX-1, NCOL) */
	    if (grpcom_1.mapuse[grpcom_1.icurdv - 1] == 0) {
		if (icolor >= grcol_1.iblack) {
		    ++icolor;
		}
		if (icolor >= grcol_1.iwhite) {
		    ++icolor;
		}
		if (icolor > grcol_1.iblack && icolor < grcol_1.iwhite) {
		    icolor = (integer) cmap_1.colmap[icolor - 1];
		}
	    } else {
		icolor += 8;
	    }
	} else if (*indx == -1) {
	    icolor = grcol_1.iblack;
	} else {
	    plcol2_1.idfort = plcol2_1.idfor;
	    if (plcol2_1.idfor == 1) {
		icolor = grcol_1.iblack;
	    } else if (plcol2_1.idfor == 2) {
		icolor = grcol_1.iwhite;
	    } else if (cmap_1.colmap[plcol2_1.idfor - 3] >= (float)0.) {
		icolor = (integer) cmap_1.colmap[plcol2_1.idfor - 3];
	    } else {
		icolor = grcol_1.iwhite;
		plcol2_1.idfort = 2;
	    }
	    plcol2_1.colfor = (real) icolor;
	}
    }
    if (icolor != grcol_1.lstcol) {
	r__1 = (real) icolor;
	ldum = pltstd_(&c__1, &r__1);
	r__1 = (real) icolor;
	ldum = pltstg_(&c__6, &r__1);
	r__1 = (real) icolor;
	ldum = pltstg_(&c__44, &r__1);
	grcol_1.lstcol = icolor;
    }
    return 0;
} /* ugrcol_ */

#ifdef __cplusplus
	}
#endif

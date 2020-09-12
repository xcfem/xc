/* grsnap.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__201 = 201;
static integer c__0 = 0;
static integer c__203 = 203;
static integer c__1 = 1;
static integer c__200 = 200;
static integer c__2 = 2;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int grsnap_(char *cmd, integer *indev, ftnlen cmd_len)
{
    /* Initialized data */

    static char idsnap[8*2+1] = "                ";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, idev;
    static real rbuf[2];
    extern /* Subroutine */ int vdnwpg_(), pltflu_();

/* ======================================================================= */
/*   --*** GRSNAP *** (GRPLIB) Perform movie snap operations */
/*   --   Written by Amy Gilkey, revised 01/25/88 */
/*   -- */
/*   --GRSNAP sets up a device for snapping multiple frames for movies. */
/*   --This code is very device-dependent. */
/*   -- */
/*   --At the present time, the only device that may be snapped is a camera */
/*   --(attached to a terminal) or a Dicomed. */
/*   -- */
/*   --An attached camera device must be pre-initialized and NSNAP must */
/*   --be set non-negative (zero ok). */
/*   -- */
/*   --Parameters: */
/*   --   CMD - IN - the snap operation: */
/*   --      QUERY = initialize device code, nsnap < 0 if cannot snap frames */
/*   --      INIT  = initialize device to snap nsnap frames */
/*   --      ABORT = abort plot */
/*   --      START = start plot */
/*   --      STOP  = end plot and snap frames */
/*   --      EXIT  = deactivate device */
/*   --   INDEV - IN - the device; 0 for current device (only QUERY allowed */
/*   --      for non-current device) */
/*   -- */
/*   --Common Variables: */
/*   --   Uses ICURDV, DEVCOD, NSNAP of /GRPCOM/ */
/*   --Routines Called: */
/*   --   VDESCP - (VDI) Send escape sequence to device */
/*      --IDSNAP - an artificial device code related to the device code */
    if (*indev != 1 && *indev != 2) {
	idev = grpcom_1.icurdv;
    } else {
	idev = *indev;
    }
/*   --Initialize device code */
    if (s_cmp(idsnap + (idev - 1 << 3), " ", (ftnlen)8, (ftnlen)1) == 0) {
	s_copy(idsnap + (idev - 1 << 3), "NONE", (ftnlen)8, (ftnlen)4);
	if (grpcom_1.nsnap[idev - 1] >= 0) {
	    s_copy(idsnap + (idev - 1 << 3), "CAMERA", (ftnlen)8, (ftnlen)6);
	} else if (s_cmp(grpcoc_1.devcod + (idev - 1 << 3), "DICOMED", (
		ftnlen)8, (ftnlen)7) == 0) {
	    s_copy(idsnap + (idev - 1 << 3), grpcoc_1.devcod + (idev - 1 << 3)
		    , (ftnlen)8, (ftnlen)8);
	}
/*      --Initialize the device */
	if (s_cmp(idsnap + (idev - 1 << 3), "CAMERA", (ftnlen)8, (ftnlen)6) ==
		 0) {
/*         --Initialized before the routine was called */
	} else if (s_cmp(idsnap + (idev - 1 << 3), "DICOMED", (ftnlen)8, (
		ftnlen)7) == 0) {
	}
    }
/*   --Set nsnap = -999 if cannot snap device */
    if (s_cmp(idsnap + (idev - 1 << 3), "NONE", (ftnlen)8, (ftnlen)4) == 0) {
	grpcom_1.nsnap[idev - 1] = -999;
    }
    if (s_cmp(cmd, "QUERY", cmd_len, (ftnlen)5) != 0) {
/*      --Skip if not current device */
	if (idev != grpcom_1.icurdv) {
	    goto L110;
	}
/*      --Skip if snaps are not requested on device */
	if (grpcom_1.nsnap[grpcom_1.icurdv - 1] <= 0) {
	    goto L110;
	}
	pltflu_();
	if (s_cmp(cmd, "INIT", cmd_len, (ftnlen)4) == 0) {
/*         --Initialize number of snaps */
	    if (s_cmp(idsnap + (idev - 1 << 3), "CAMERA", (ftnlen)8, (ftnlen)
		    6) == 0) {
	    } else if (s_cmp(idsnap + (idev - 1 << 3), "DICOMED", (ftnlen)8, (
		    ftnlen)7) == 0) {
	    }
	} else if (s_cmp(cmd, "ABORT", cmd_len, (ftnlen)5) == 0) {
/*         --Abort plot */
	    if (s_cmp(idsnap + (idev - 1 << 3), "CAMERA", (ftnlen)8, (ftnlen)
		    6) == 0) {
	    } else if (s_cmp(idsnap + (idev - 1 << 3), "DICOMED", (ftnlen)8, (
		    ftnlen)7) == 0) {
/*            --Close segment and delete all segments */
		rbuf[0] = 0;
		vdescp_(&c__201, &c__0, rbuf);
		vdescp_(&c__203, &c__1, rbuf);
	    }
	} else if (s_cmp(cmd, "START", cmd_len, (ftnlen)5) == 0) {
/*         --Start plot */
	    if (s_cmp(idsnap + (idev - 1 << 3), "CAMERA", (ftnlen)8, (ftnlen)
		    6) == 0) {
	    } else if (s_cmp(idsnap + (idev - 1 << 3), "DICOMED", (ftnlen)8, (
		    ftnlen)7) == 0) {
/*            --Open segment */
		rbuf[0] = 1;
		rbuf[1] = 1;
		vdescp_(&c__200, &c__2, rbuf);
	    }
	} else if (s_cmp(cmd, "STOP", cmd_len, (ftnlen)4) == 0) {
/*         --End plot and snap frames */
	    if (s_cmp(idsnap + (idev - 1 << 3), "CAMERA", (ftnlen)8, (ftnlen)
		    6) == 0) {
/*            --Snap n frames */
	    } else if (s_cmp(idsnap + (idev - 1 << 3), "DICOMED", (ftnlen)8, (
		    ftnlen)7) == 0) {
/*            --Close segment */
		rbuf[0] = 0;
		vdescp_(&c__201, &c__0, rbuf);
/*            --Snap n-1 frames (plot segment with newpage) */
		i__1 = grpcom_1.nsnap[grpcom_1.icurdv - 1] - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    vdnwpg_();
/* L100: */
		}
/*            --Delete all segments */
		rbuf[0] = 0;
		vdescp_(&c__203, &c__1, rbuf);
	    }
	} else if (s_cmp(cmd, "EXIT", cmd_len, (ftnlen)4) == 0) {
/*         --Deactivate device */
	    if (s_cmp(idsnap + (idev - 1 << 3), "CAMERA", (ftnlen)8, (ftnlen)
		    6) == 0) {
	    } else if (s_cmp(idsnap + (idev - 1 << 3), "DICOMED", (ftnlen)8, (
		    ftnlen)7) == 0) {
	    }
	} else {
	    goto L110;
	}
    }
L110:
    return 0;
} /* grsnap_ */

#ifdef __cplusplus
	}
#endif

/* vdicps.f -- translated by f2c (version 20160102).
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
    real xinch, yinch;
} vcmodr_;

#define vcmodr_1 vcmodr_

struct {
    integer ivect;
} vcvec1_;

#define vcvec1_1 vcvec1_

struct {
    real xndcmx, yndcmx;
} vcndcm_;

#define vcndcm_1 vcndcm_

struct {
    real vector[7];
} vcattr_;

#define vcattr_1 vcattr_

struct {
    real xcp, ycp;
} vccrps_;

#define vccrps_1 vccrps_

struct {
    integer kwrtfl, krdfl, koutfl, kinfl, kwrdsz, kbytel, kcpw, kbaud, kcomtp;
} vcpstc_;

#define vcpstc_1 vcpstc_

struct {
    real xscale, yscale;
} vcscal_;

#define vcscal_1 vcscal_

struct {
    integer xpad, ypad, xdevic, ydevic;
} vcddim_;

#define vcddim_1 vcddim_

struct {
    integer kidsiz, kjobid[4], kusrsz, kusrid[4], kszrou, kjrout[4], ksecur, 
	    kjtime[4], kjdate[4], machin[3], maclen;
} vcjob_;

#define vcjob_1 vcjob_

struct {
    real dev[33];
} devcap_;

#define devcap_1 devcap_

struct {
    integer pgform, patno, border;
} vcescp_;

#define vcescp_1 vcescp_

struct {
    real pstmlw;
    integer kpstbg, kpstci;
} vcpsta_;

#define vcpsta_1 vcpsta_

struct {
    integer mopoly, mocolr;
} vcpstb_;

#define vcpstb_1 vcpstb_

struct {
    integer kempty;
} vcpstd_;

#define vcpstd_1 vcpstd_

struct {
    real pcols[768]	/* was [3][256] */;
} pcolst_;

#define pcolst_1 pcolst_

struct {
    integer totpag;
} vcpage_;

#define vcpage_1 vcpage_

struct {
    char coord[20], lstcrd[20];
} vcvec2_;

#define vcvec2_1 vcvec2_

struct {
    char bltans[2];
} blotans_;

#define blotans_1 blotans_

/* Table of constant values */

static integer c__721 = 721;
static integer c__5 = 5;
static integer c__720 = 720;
static integer c__0 = 0;
static integer c__38 = 38;
static integer c__40 = 40;
static integer c__24 = 24;
static integer c__16 = 16;
static integer c__28 = 28;
static integer c__13 = 13;
static integer c__35 = 35;
static integer c__14 = 14;
static integer c__27 = 27;
static integer c__32 = 32;
static integer c__50 = 50;
static integer c__54 = 54;
static integer c__31 = 31;
static integer c__33 = 33;
static integer c__39 = 39;
static integer c__10 = 10;
static integer c__21 = 21;
static integer c__4 = 4;
static integer c__8 = 8;
static integer c__11 = 11;
static integer c__6 = 6;
static integer c__17 = 17;
static integer c__1 = 1;
static integer c__726 = 726;
static integer c__723 = 723;
static integer c__725 = 725;
static integer c__724 = 724;
static integer c__727 = 727;
static integer c__2 = 2;
static integer c__401 = 401;
static integer c__9 = 9;
static integer c__3 = 3;
static integer c__19 = 19;
static integer c__802 = 802;
static integer c__12 = 12;
static integer c__7 = 7;
static integer c__212 = 212;
static integer c__213 = 213;
static integer c__208 = 208;
static integer c__22 = 22;

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* Subroutine */ int viinit_(real *aspect, integer *justif)
{
    /* System generated locals */
    address a__1[8];
    integer i__1, i__2[8];

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);
    integer i_nint(real *);

    /* Local variables */
    static integer ic, ik;
    static real temp;
    static char coord[20];
    extern /* Subroutine */ int vbdev_(char *, ftnlen);
    static integer dummy[1];
    static real aspec1;
    extern /* Subroutine */ int vdiqd9_(real *, real *), psti2c_(integer *, 
	    integer *, char *, ftnlen);
    static integer justi1;
    static real coldef[3], daspec;
    extern /* Subroutine */ int pstbbg_();
    static char xcoord[4], ycoord[4];
    extern /* Subroutine */ int vberrh_(integer *, integer *), pstsel_(char *,
	     ftnlen), vdmoni_(integer *), pstofs_(integer *), pstini_(), 
	    pstbuf_(integer *, char *, ftnlen), vdstco_(integer *, integer *, 
	    real *, integer *), vbvect_(integer *, real *, real *), vdstlw_(
	    real *);
    static real xunits, yunits;
    extern /* Subroutine */ int vdstfc_(integer *);

/*     VDI-PostScript driver - B&W and COLOR versions */
/*     Adapted for all systems by S.L.Thompson */
/*     Original code from D.Campbell and J.LONG */
/*     vdi device numbers are */
/*                                                   device number */
/*   black & white, batch, no poly fill                  799.1 */
/*   black & white, interactive, no poly                 799.2 */
/*   black & white, batch, poly fill                     799.3 */
/*   black & white, interactive, poly fill               799.4 */
/*   color, batch                                        799.5 */
/*   color, interactive                                  799.6 */
/*   color, batch, black-white interchange               799.7 */
/*   color, interactive, black-white interchange         799.8 */
/*   color, batch, black background                      799.9 */
/*   color, interactive, black background                799.11 */
/*                                                 last mod 6/20/90 slt */
/*     Note that there are several parameters to set depending on how */
/*     the package is to be used. Most are in routine pstsel routine */
/*     which is called at the first of this routine (viinit.) Two other */
/*     parameters (xinch,yinch) are set in this routine and vdiqd9. */
/*     This code is for BOTH color and black & white systems. */
/*     Flag is set for mode in pstsel. */
/*     Device can be set with escape call before call to vdinit. */
/*     Otherwise, code will interactively ask for device type. */
/*     There is also an escape flag for landscape or portrait format. */
/*     This deck was generated from a qms driver and still has the */
/*     qms comments in places. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VIINIT           -Initialize SVDI.  postscript device */
/* D.L. CAMPBELL    -1-DEC-1986 */
/* J.P. LONG        -9-NOV-1987 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   (postscript) */
/* ENTRY CONDITIONS -ASPECT = real ratio of X dimension to Y dimension. */
/*                   Range >0.0.  Default: 0. (device dependent). */
/*                   JUSTIF = integer justification of NDC space on the */
/*                   device.  Range 0-9.  Default: 0 (device dependent.) */
/* CALLS            -VBERRH,VDSTCS,VDSTLW,VIMOVA */
/* EXIT CONDITIONS  -XNDCMX,YNDCMX = real NDC maximum valid values(as */
/*                   constrained by ASPECT). */
/*                   VECTOR = real array of attribute values(all device */
/*                   dependent except VECTOR(4)=0.0). */
/* NARRATIVE        -This must be the first SVDI call made.  All */
/*                   attribute values, the color table, and current */
/*                   position are set to appropriate defaults for the */
/*                   device.  All necessary input device initialization */
/*                   is done.  The screen is cleared or paper advanced */
/*                   if necessary to guarantee a blank view surface for */
/*                   drawing on. */
/*                   ASPECT specifies the ratio of the X dimension to the */
/*                   Y dimension .  Maximum NDC values (at least one of */
/*                   which will be 1.0) are computed to give the ASPECT */
/*                   specified.  The default ASPECT (0.0) is device */
/*                   dependent and equals the aspect ratio of the */
/*                   physical device, except for variable aspect devices */
/*                   (such as drum plotters) which are assigned a default */
/*                   aspect of 1.0.  The NDC rectangle is scaled until */
/*                   one dimension fills the corresponding dimension of */
/*                   the device. */
/*                   JUSTIF determines where the rectangle is located on */
/*                   the device as diagrammed below: */
/*                            --------- */
/*                           | 7  8  9 | */
/*                           | 4  5  6 | */
/*                           | 1  2  3 | */
/*                            --------- */
/*                   For example, JUSTIF = 7 indicates NDC space will be */
/*                   upper left justified on the device. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC */
/*     Set parameters for type of usage. */
/*     Two settings are coded - one for square field of view */
/*     and one for full field of view. */
/*     If VDIQDC is called before vdinit, full field of view is selected. */
/*     Otherwise, square is used. */
/*     size of full view */
/*     size of square view */
/*     PARAMETER (XINCHO=7.5) */
/*     PARAMETER (YINCHO=7.5) */
/* - INCLUDE PSTSQUAR */
/*     size of square view window */
/*     parameters set to get same size plot as imagen and qms b&w. */
/*      PARAMETER (XINCHO=7.4412525) */
/*      PARAMETER (YINCHO=7.4412525) */
/* - */
/* CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC */
/* MAXIMUM VALID NDC VALUES. (DEVICE-INDEPENDENT) */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* CURRENT POSITION. */
/* COMPUTER DEPENDENT COMMON VARIABLES AND CONSTANTS. */
/* SCALE FACTORS FOR NDC TO DC MAPPING. (LXY,HC1) */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* DC DIMENSIONS OF OFFSETS AND PICTURE. (LXY,HC1) */
/* JOB ID INFORMATION. (HC1, DIC) */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ESCAPE FLAGS */
/* PATNO AND BORDER USED BY VIPOLY FOR FILL PATTERN AND BORDER ON/OFF; */
/* DEFAULT COMPLETE FILL WITH BORDER. PLC. */
/*     pstmlw controls minimum line width */
/*     kpstbg controls background coloring */
/*            = 0,  not colored (white ground from paper) */
/*            = 1,  colored black */
/*     kpstci controls black-white interchange (colors 0 & 7 only) */
/*            = 0,  no interchange */
/*            = 1,  colors interchanged */
/*     mopoly controls polygon fill =0, on ;  =1, off */
/*     mocolr controls color =0, on ;  =1, off */
/*     vcpstd variables control what to do with empty frames with */
/*     command is received to dump data to output */
/*        kempty=0,  frame is void - do not draw */
/*              >0,  frame has data - draw it */
    devcap_1.dev[0] = (float)0.;
    devcap_1.dev[1] = (float)1.;
    devcap_1.dev[2] = (float)1.;
    devcap_1.dev[3] = (float)1.;
    devcap_1.dev[4] = (float)15.;
    devcap_1.dev[5] = (float)2.;
    devcap_1.dev[6] = (float)0.;
    devcap_1.dev[7] = (float)0.;
    devcap_1.dev[8] = (float)0.;
    devcap_1.dev[9] = (float)0.;
    devcap_1.dev[10] = (float)0.;
    devcap_1.dev[11] = (float)0.;
    devcap_1.dev[12] = (float)0.;
    devcap_1.dev[13] = (float)0.;
    devcap_1.dev[14] = (float)7230.;
    devcap_1.dev[15] = (float)5040.;
    devcap_1.dev[16] = (float)254.;
    devcap_1.dev[17] = (float)178.;
    devcap_1.dev[18] = (float)4.;
    devcap_1.dev[19] = (float)10.;
    devcap_1.dev[20] = (float)84.;
    devcap_1.dev[21] = (float)0.;
    devcap_1.dev[22] = (float)0.;
    devcap_1.dev[23] = (float)3.;
    devcap_1.dev[24] = (float)99999.;
    devcap_1.dev[25] = (float)0.;
    devcap_1.dev[26] = (float)1.;
    devcap_1.dev[27] = (float)0.;
    devcap_1.dev[28] = (float)0.;
    devcap_1.dev[29] = (float)5e3;
    devcap_1.dev[30] = (float)750.;
    devcap_1.dev[31] = (float)0.;
    devcap_1.dev[32] = (float)1.;
/* SET DEFAULT ATTRIBUTE VALUES.  ALL ARE DEVICE DEPENDENT EXCEPT */
/* VECTOR(4)=0.0. */
/* .. following removed since should be in block data... */
/*      DATA VECTOR /0.,7.,1.,0.,.06255,.01,.0/ */
/*     VECTOR(1)=FOREGROUND COLOR - BLACK */
/*           (2)=BACKGROUND COLOR - WHITE */
/*           (3)=INTENSITY        - MAXIMUM */
/*           (4)=LINE STYLE       - SOLID */
/*           (5)=LINE WIDTH       - ABOUT 1/72 INCHES */
/*           (6)=CHARACTER BOX Y  - ABOUT 1/10 INCHES */
/*           (7)=CHARACTER BOX X  - 5/7 OF BOX-Y */
    vcattr_1.vector[0] = (float)0.;
    vcattr_1.vector[1] = (float)7.;
    vcattr_1.vector[2] = (float)1.;
    vcattr_1.vector[3] = (float)0.;
    vcattr_1.vector[4] = (float).06255;
    vcattr_1.vector[5] = (float).01;
    vcattr_1.vector[6] = (float)0.;
/* PROTECT INPUT PARAMETERS FROM BEING CHANGED. */
    aspec1 = *aspect;
    justi1 = *justif;
    vcpstd_1.kempty = 0;
    vcescp_1.pgform = 0;
    vcescp_1.patno = 20;
    vcescp_1.border = 1;
    vccrps_1.xcp = (float)0.;
    vccrps_1.ycp = (float)0.;
/* CHECK FOR VALID ASPECT.  IF(ASPECT.LT.0.0) THEN CALL VBERRH(721,5), */
/* AND USE DEFAULT ASPECT. */
    if (*aspect < (float)0.) {
	vberrh_(&c__721, &c__5);
	aspec1 = (float)0.;
    }
/* CHECK FOR VALID JUSTIF.  IF(JUSTIF.LT.0 .OR. JUSTIF.GT.9) THEN */
/* CALL VBERRH(720,5), AND USE DEFAULT JUSTIF. */
    if (*justif < 0 || *justif > 9) {
	vberrh_(&c__720, &c__5);
	justi1 = 0;
    }
/* SCALE NDC UNITS TO DEVICE UNITS. */
/* FOR QMS, THE PHYSICAL PLOT SURFACE IS XINCH X YINCH (10.x7.5). */
/*    DEVICE COORDINATES ARE KEPT IN 1/723 INCH TO GAIN SIMPLICITY */
/*    IN ASSEMBLING CHARACTER STRINGS WITH FLOATING NUMBERS */
/*    ACCURATE TO TENTHS OF DEVICE UNITS */
/* THE DEVICE ASPECT IS XINCH/YINCH */
/* BUT WE MUST MAP THE ASPECT SPECIFIED IN DC INTO THIS */
/* ADDRESSABILITY,USING AS MUCH OF THE SPACE AS POSSIBLE. */
    vcmodr_1.xinch = (float)10.;
    vcmodr_1.yinch = (float)7.5;
/*     test for rscors post or direct mode. Use 7.5x7.5 for direct */
/*     and 10.0x7.5 for post */
/*     if VDIQDC has already been called, we are in post mode; */
/*     otherwise in direct mode */
    vdiqd9_(&vcmodr_1.xinch, &vcmodr_1.yinch);
/*                                  CHECK PAGE FORMAT - IF PORTRAIT, */
/*                                     THEN SWITCH THINGS AROUND */
    if (vcescp_1.pgform == 1) {
	temp = vcmodr_1.xinch;
	vcmodr_1.xinch = vcmodr_1.yinch;
	vcmodr_1.yinch = temp;
	temp = devcap_1.dev[14];
	devcap_1.dev[14] = devcap_1.dev[15];
	devcap_1.dev[15] = temp;
	temp = devcap_1.dev[16];
	devcap_1.dev[16] = devcap_1.dev[17];
	devcap_1.dev[17] = temp;
    }
    xunits = vcmodr_1.xinch * (float)723.;
    yunits = vcmodr_1.yinch * (float)723.;
    daspec = xunits / yunits;
/* DEFAULT ASPECT = 1., DEFAULT JUSTIF = 1. */
    if (aspec1 == (float)0.) {
	aspec1 = daspec;
    }
    if (justi1 == 0) {
	justi1 = 1;
    }
    if (aspec1 >= daspec) {
/* THEN X DIMENSION IS FILLED. */
	vcddim_1.xdevic = xunits;
	vcddim_1.ydevic = xunits / aspec1;
	vcddim_1.xpad = 0;
/* FIGURE HOW MUCH Y PADDING IS NEEDED. */
	if (justi1 <= 3) {
	    vcddim_1.ypad = 0;
	} else if (justi1 <= 6) {
	    vcddim_1.ypad = (yunits - vcddim_1.ydevic) / 2;
	} else {
	    vcddim_1.ypad = yunits - vcddim_1.ydevic;
	}
    } else {
/* ELSE Y DIMENSION IS FILLED. */
	vcddim_1.xdevic = yunits * aspec1;
	vcddim_1.ydevic = yunits;
	vcddim_1.ypad = 0;
/* FIGURE HOW MUCH X PADDING IS NEEDED. */
	if (justi1 == 3 || justi1 == 6 || justi1 == 9) {
	    vcddim_1.xpad = xunits - vcddim_1.xdevic;
	} else if (justi1 == 2 || justi1 == 5 || justi1 == 8) {
	    vcddim_1.xpad = (xunits - vcddim_1.xdevic) / 2;
	} else {
	    vcddim_1.xpad = 0;
	}
    }
/* FIGURE MAXIMUM NDC VALUES XNDCMX AND YNDCMX. */
    if (aspec1 >= daspec) {
	vcndcm_1.xndcmx = dmin((float)1.,aspec1);
	vcndcm_1.yndcmx = vcndcm_1.xndcmx / aspec1;
    } else {
	vcndcm_1.xndcmx = aspec1;
	vcndcm_1.yndcmx = (float)1.;
    }
/* SET SCALE FACTORS FOR NDC-TO-DEVICE MAPPING. */
    vcscal_1.xscale = (doublereal) vcddim_1.xdevic / vcndcm_1.xndcmx;
    vcscal_1.yscale = (doublereal) vcddim_1.ydevic / vcndcm_1.yndcmx;
    if (vcescp_1.pgform > 0) {
	vcddim_1.ypad += (float)280.;
	vcddim_1.xpad += (float)360.;
    } else {
	vcddim_1.xpad += (float)280.;
	vcddim_1.ypad += (float)-180.;
    }
    pstsel_(" ", (ftnlen)1);
/*  SET UP MONITORING INFORMATION */
    vbdev_("V PST   ", (ftnlen)8);
    vdmoni_(&c__0);
    vcvec1_1.ivect = 0;
/* OPEN OUTPUT FILE */
    pstofs_(&vcpstc_1.koutfl);
/* INITIALIZE the printer */
    pstini_();
    pstbuf_(&c__38, "%%Title: Graphics SVDI PostScript File", (ftnlen)38);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__40, "%%Creator: SNL SEACAS SVDI Driver -- cps", (ftnlen)40);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__24, "%%Orientation: Landscape", (ftnlen)24);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__16, "%%Pages: (atend)", (ftnlen)16);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__28, "%%BoundingBox: 36 30 574 750", (ftnlen)28);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__24, "%%DocumentFonts: Courier", (ftnlen)24);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__13, "%%EndComments", (ftnlen)13);
    pstbuf_(&c__0, " ", (ftnlen)1);
    if (vcpstb_1.mocolr == 0) {
	pstbuf_(&c__35, "% this file contains color commands", (ftnlen)35);
	pstbuf_(&c__0, " ", (ftnlen)1);
/*       default user dictionary is too small to contain color */
/*       definition commands - make a larger one. */
	pstbuf_(&c__14, "300 dict begin", (ftnlen)14);
	pstbuf_(&c__0, " ", (ftnlen)1);
    }
    pstbuf_(&c__27, "/y {/Courier findfont} def ", (ftnlen)27);
    pstbuf_(&c__27, "/x {scalefont setfont} def ", (ftnlen)27);
    pstbuf_(&c__32, "/m {moveto} def /l {lineto} def ", (ftnlen)32);
    pstbuf_(&c__50, "/c {closepath} def /v {save} def /r {restore} def ", (
	    ftnlen)50);
    pstbuf_(&c__54, "/f {eofill} def /s {stroke} def /w {setlinewidth} def ", 
	    (ftnlen)54);
    pstbuf_(&c__31, "/h {setdash} def /t {show} def ", (ftnlen)31);
    pstbuf_(&c__33, "/d {gsave} def /u {grestore} def ", (ftnlen)33);
    if (devcap_1.dev[22] >= (float)799.75 && devcap_1.dev[22] <= (float)
	    799.85) {
	pstbuf_(&c__39, "/q {exch pop exch dup 2 exp 1 exch sub ", (ftnlen)39)
		;
	pstbuf_(&c__39, "setgray add /brsum exch def brsum 0 le ", (ftnlen)39)
		;
	pstbuf_(&c__40, "{ 0 setgray } if brsum 2 ge{ 1 setgray }", (ftnlen)
		40);
	pstbuf_(&c__10, " if } def ", (ftnlen)10);
    } else if (devcap_1.dev[22] >= (float)799.05 && devcap_1.dev[22] <= (
	    float)799.15) {
	pstbuf_(&c__28, "/q {exch pop exch dup 2 exp ", (ftnlen)28);
	pstbuf_(&c__39, "setgray add /brsum exch def brsum 0 le ", (ftnlen)39)
		;
	pstbuf_(&c__40, "{ 0 setgray } if brsum 2 ge{ 1 setgray }", (ftnlen)
		40);
	pstbuf_(&c__10, " if } def ", (ftnlen)10);
    } else {
	pstbuf_(&c__21, "/q {setrgbcolor} def ", (ftnlen)21);
    }
    pstbuf_(&c__14, "1 setlinejoin ", (ftnlen)14);
    pstbuf_(&c__0, " ", (ftnlen)1);
/*                                       SET PAGE FORMAT (LANDSCAPE/PORTRAIT) */
    if (vcescp_1.pgform == 0) {
	pstbuf_(&c__4, "/o {", (ftnlen)4);
	pstbuf_(&c__10, "90 rotate ", (ftnlen)10);
	psti2c_(&c__0, &c__4, xcoord, (ftnlen)4);
	i__1 = (integer) (vcddim_1.ydevic + vcddim_1.ydevic * (float)3. / (
		float)32.);
	psti2c_(&i__1, &c__4, ycoord, (ftnlen)4);
/* Writing concatenation */
	i__2[0] = 1, a__1[0] = " ";
	i__2[1] = 3, a__1[1] = xcoord;
	i__2[2] = 1, a__1[2] = ".";
	i__2[3] = 1, a__1[3] = xcoord + 3;
	i__2[4] = 2, a__1[4] = " -";
	i__2[5] = 3, a__1[5] = ycoord;
	i__2[6] = 1, a__1[6] = ".";
	i__2[7] = 1, a__1[7] = ycoord + 3;
	s_cat(coord, a__1, i__2, &c__8, (ftnlen)20);
	pstbuf_(&c__13, coord, (ftnlen)20);
	pstbuf_(&c__11, " translate ", (ftnlen)11);
	pstbuf_(&c__6, "} def ", (ftnlen)6);
	vcddim_1.ypad = -vcddim_1.ypad;
    } else {
	pstbuf_(&c__17, "/o {newpath} def ", (ftnlen)17);
    }
    pstbuf_(&c__35, "/p {showpage} def 1 setlinecap v o ", (ftnlen)35);
/*     check for color or black & white mode */
    if (vcpstb_1.mocolr == 0) {
/*       color is on */
/*       define some kind of color table */
	for (ic = 0; ic <= 7; ++ic) {
	    coldef[0] = (float)0.;
	    coldef[1] = (float)0.;
	    coldef[2] = (float)0.;
	    if (ic == 1) {
		coldef[0] = (float)1.;
	    } else if (ic == 2) {
		coldef[1] = (float)1.;
	    } else if (ic == 3) {
		coldef[0] = (float)1.;
		coldef[1] = (float)1.;
	    } else if (ic == 4) {
		coldef[2] = (float)1.;
	    } else if (ic == 5) {
		coldef[0] = (float)1.;
		coldef[2] = (float)1.;
	    } else if (ic == 6) {
		coldef[1] = (float)1.;
		coldef[2] = (float)1.;
	    } else if (ic == 7) {
		coldef[0] = (float)1.;
		coldef[1] = (float)1.;
		coldef[2] = (float)1.;
	    }
	    for (ik = 0; ik <= 255; ik += 8) {
		dummy[0] = ic + ik;
		vdstco_(&c__1, dummy, coldef, &c__0);
		if (ic == 0) {
		    coldef[0] = (float).2;
		    coldef[1] = (float).2;
		    coldef[2] = (float).2;
		}
/* L115: */
	    }
/* L120: */
	}
    }
    vcattr_1.vector[0] = (float)7.;
    vcattr_1.vector[1] = (float)0.;
/*     define the postscript current position */
    vbvect_(&c__0, &vccrps_1.xcp, &vccrps_1.ycp);
/*     shade background if appropriate */
    if (vcpsta_1.kpstbg != 0) {
	pstbbg_();
	vcpstd_1.kempty = 0;
    }
/* INIT LINE WIDTH,CHARACTER SIZE */
    vdstlw_(&vcattr_1.vector[4]);
/*      CALL VDSTCS(VECTOR(6)) */
    i__1 = i_nint(vcattr_1.vector);
    vdstfc_(&i__1);
    pstbuf_(&c__0, " ", (ftnlen)1);
    return 0;
} /* viinit_ */

/* Subroutine */ int vdiqdc_0_(int n__, integer *index, real *value, real *
	xinch, real *yinch)
{
    /* Initialized data */

    static integer nocall = 0;

    /* Builtin functions */
    integer i_nint(real *);

    /* Local variables */
    extern /* Subroutine */ int vberrh_(integer *, integer *), pstsel_(char *,
	     ftnlen);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDIQDC           -Inquire Device Capabilities. */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS -INDEX = integer capability number.  Range 1-33. */
/* CALLS            - */
/* EXIT CONDITIONS  -VALUE = real value of the capability indicated by */
/*                   INDEX. */
/* NARRATIVE        -Return values of various device capabilities.  INDEX */
/*                   is the integer capability number (as given below), */
/*                   and the real value is returned in VALUE. */
/*     1. Erasability */
/*        0.  None (hard copy) */
/*        1.  Screen (Tektronix 4010) */
/*        2.  Point or Line SLOW (Plasma) */
/*        3.  Point or Line MEDIUM (Refresh -Serially connected) */
/*        4.  Point or Line FAST (Refresh -Direct connected) */
/*        5.  (1) and some (3) (Tektronix 4014 with write-thru mode) */
/*     2. Scan Type */
/*        0.  Vector */
/*        1.  Raster */
/*        2.  Matrix (Plasma) */
/*     3. Intensities (1-N) */
/*     4. Colors (1-N)  This is the number of colors that can be */
/*                      displayed at one time and may be less than the */
/*                      total number of colors the device can produce. */
/*     5. Line Widths (1-N) */
/*     6. Line Styles (0-N) A bit pattern indicating which of the 5 */
/*                      non-solid line styles are supported in the */
/*                      device.  Bits 4,3,2,1, and 0 correspond to line */
/*                      styles medium dash, long dash, short dash, dot */
/*                      dash, and dot. (0 - device has no hardware line */
/*                      styles - simulate.) */
/*     7. Character Sizes (0-N)  (0 - device has no hardware - simulate) */
/*     8. Number of Locator Devices */
/*     9. Number of Valuator Devices */
/*    10. Number of Button Devices */
/*    11. Number of Keyboard Devices */
/*    12. Number of Stroke Devices */
/*    13. Input */
/*        0. none */
/*        1. synchronous only - program requests input, then the user */
/*           supplies it. */
/*        2. synchronous and asynchronous - synchronous is the same */
/*           as in (1) above.  Asynchronous means the user can provide */
/*           input at any time; this input is then saved by the system */
/*           in an event queue until the program calls for it. */
/*    14. Input Timing */
/*        0. no timeout supported */
/*        1. unreliable timing */
/*        2. timeout with reliable timing */
/*    15. X Dimension of View Surface in Device Coordinates */
/*    16. Y Dimension of View Surface in Device Coordinates */
/*    17. X Dimension of View Surface in Physical Units (mm) (0 if */
/*        undefined).  If this dimension is variable (as for drum */
/*        plotters), it should be set equal to the Y dimension to */
/*        guarantee a device aspect ratio of 1.0. */
/*    18. Y Dimension of View Surface in Physical Units (mm) (0 if */
/*        undefined). */
/*    19. Smallest Line Width (DC) at default intensity */
/*    20. Smallest Point (DC) at default intensity */
/*    21. Smallest Character Size (DC) */
/*    22. Header and Trailer Frames Required (0=no,1=yes) */
/*    23. Device Identifier */
/*        1.  TK4 - Tektronix 4014 */
/*        1.1 TK6 - Tektronix 4016 */
/*        1.2 TEK - Tektronix 4010, 4012 */
/*        1.3 TP2 - Tektronix 4662 */
/*        1.4 TP8 - Tektronix 4662 with 8 pen option */
/*        1.5 T14 - Tektronix 4114 */
/*        1.6 T13 - Tektronix 4113 */
/*        1.7 T05 - TEKTRONIX 4105 */
/*        1.8 T07 - TEKTRONIX 4107 */
/*        1.9 T15 - TEKTRONIX 4115 */
/*        2.1 16C - Dicomed 16mm color Movies */
/*        2.2 16B - Dicomed 16mm black and white movies */
/*        2.3 35C - Dicomed 35mm color slides */
/*        2.31 3MC - Dicomed 35mm movie color */
/*        2.4 35B - Dicomed 35mm black and white slides */
/*        2.41 3MB - Dicomed 35mm movie black and white */
/*        2.5 35A - Dicomed 35mm Aperture Card */
/*        2.6 24L - Dicomed 24X Fiche */
/*        2.7 48L - Dicomed 48X Fiche */
/*        2.8 CSQ - Dicomed Color Full Frame(square aspect ratio) */
/*        2.9 BSQ - Dicomed Black and White Full Frame(square aspect) */
/*        3   R94 - Ramtek 9400 */
/*        4.  T27 - Tektronix 4027 */
/*        4.1 T25 - Tektronix 4025 */
/*        5.  ALP - Alphanumeric Terminal */
/*        6.  HC1 - Remote Hard Copy */
/*        7.  LXY - Printronix */
/*        8.  TST - Test Driver, Print VDI calls made. */
/*        9.  V25 - Digital VT125 */
/*       10.  AED - AED 512 */
/*       10.1 AE7 - AED 767 */
/*       10.2 AE1 - AED 1024 */
/*       11.  MET - SVDI Metafile */
/*       12.  HPP - Hewlett Packard Printer/Plotter 2671G */
/*       12.1 H75 - HP 7580 */
/*       12.2 H72 - HP 7221C OR T */
/*       12.3 H74 - HP 7475A */
/*       14.  RET - Retrographics */
/*       15.  AP5 - Aps 5 Phototypesetter */
/*       16.  JP7 - JUPITER 7 */
/*       16.1 JP1 - Jupiter 1024 */
/*       17.  GER - Gerber 4400 Photoplotter */
/*       18.  XYN - XYNETICS */
/*       20.  PS3 - E & S Picture System 300 */
/*       21.  QMS - QMS LASER PRINTER */
/*       22.  C51 - CALCOMP 1051 DRUM PLOTTER */
/*       23.  R25 - RASTER TECHNOLOGIES MODEL ONE/25 */
/*       24.  QLF - QCR large format (8 x 10) */
/*       24.1 Q35 - QCR 35mm format */
/*       25.  T45 - Tektronix 4510 Rasterizer */
/*    24. Polygon support level */
/*        0.  no support */
/*        1.  fills convex polygons */
/*        2.  fills simple polygons (may be concave but not */
/*            self-crossing) */
/*        3.  full complex polygon fill support */
/*    25. Maximum number of points in a polygon (99999. if infinite) */
/*    26. Setable color table */
/*        0.  no */
/*        1.  yes */
/*    27. Device color palette size (1-N) - the number of different */
/*            colors a device can produce (may be more than the device */
/*            can display simultaneously) */
/*    28. Direct color space size (0-N) - the number of colors */
/*            available via direct RGB color specification */
/*            (displayable simultaneously) */
/*    29. Vector verses Raster VDI */
/*        0.  SVDI */
/*        1.  SVDI+Raster */
/*    30. Maximum character height (DC) */
/*    31. Maximum line width (DC) */
/*    32. Color verses monochrome (greyscale) device */
/*        0.  monochrome */
/*        1.  color */
/*    33. Device pixel aspect - the ratio of the spacing of device */
/*            pixels in x divided by the spacing in y (1 for square */
/*            pixels) */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* ESCAPE FLAGS */
/* PATNO AND BORDER USED BY VIPOLY FOR FILL PATTERN AND BORDER ON/OFF; */
/* DEFAULT COMPLETE FILL WITH BORDER. PLC. */
/*     mopoly controls polygon fill =0, on ;  =1, off */
/*     mocolr controls color =0, on ;  =1, off */
/* - INCLUDE PSTFULL */
/*     size of full view window */
/*     parameters set to get same size plot as imagen and qms b&w. */
/* - */
/* INITIALIZE THE DEVICE CAPABILITIES VECTOR. */
    switch(n__) {
	case 1: goto L_vdiqd9;
	}

/*     If device is 0, call to reset */
    if (i_nint(&devcap_1.dev[22]) == 0) {
	pstsel_(" ", (ftnlen)1);
    }
/* CHECK FOR VALID INDEX. */
    if (*index < 1 || *index > 33) {
	vberrh_(&c__726, &c__5);
	goto L999;
    }
/* RETURN INDEXED VALUE. */
    *value = devcap_1.dev[*index - 1];
    if (*index == 23) {
	nocall = 1;
    }
L999:
    return 0;
/* ********************************************************************** */

L_vdiqd9:
/*     This is an added entry for rscors version of pst driver to */
/*     tell if direct or post mode operation. If post mode, vdiqdc */
/*     is called before vdinit to get terminal type. In direct mode */
/*     it is never called to get type. */
    if (nocall != 0) {
/*       XINCH=10.0 */
/*       YINCH=7.5 */
	*xinch = (float)9.92167;
	*yinch = (float)7.4412525;
    }
    return 0;
} /* vdiqdc_ */

/* Subroutine */ int vdiqdc_(integer *index, real *value)
{
    return vdiqdc_0_(0, index, value, (real *)0, (real *)0);
    }

/* Subroutine */ int vdiqd9_(real *xinch, real *yinch)
{
    return vdiqdc_0_(1, (integer *)0, (real *)0, xinch, yinch);
    }

/* Subroutine */ int vberrh_(integer *errnum, integer *errsev)
{
    /* Builtin functions */
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    extern /* Subroutine */ int vdloge_(integer *, integer *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VBERRH           -Error Handler. */
/* R.W.Simons       -08APR81 */
/*                   30SEP81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -ERRNUM = integer error number. */
/*                   ERRSEV = integer severity code.  If > 12, error is */
/*                   fatal. */
/* CALLS            -VDLOGE. */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -An error will normally cause an error message to */
/*                   be printed on the error output device and possible */
/*                   termination of the program, unless a routine VBERRH */
/*                   is supplied by the user.  This routine will replace */
/*                   the default VBERRH provided by the system.  The */
/*                   system supplied VBERRH calls VDLOGE before */
/*                   returning.  All versions of VBERRH, whether user- */
/*                   supplied or default, must STOP on any error severity */
/*                   greater than 12. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* REPORT THE ERROR USING VDLOGE. */
    vdloge_(errnum, errsev);
/* CHECK FOR FATAL ERROR. */
    if (*errsev > 12) {
	s_stop("", (ftnlen)0);
    }
    return 0;
} /* vberrh_ */

/* Subroutine */ int vdgnam_0_(int n__, char *name__, integer *koutfl, 
	integer *koutff, integer *kk, ftnlen name_len)
{
    /* Initialized data */

    static char pstnam[132+1] = "vdicps.ps                                  \
                                                                            \
             ";
    static integer istat = 0;

    /* Format strings */
    static char fmt_203[] = "(//,\002 ERROR OPENING PST OUTPUT FILE UNIT \
=\002,i8,/,1x,a)";

    /* System generated locals */
    integer i__1;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer i_len(char *, ftnlen);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer f_open(olist *), s_wsfe(cilist *), do_fio(integer *, char *, 
	    ftnlen), e_wsfe();
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer f_clos(cllist *);

    /* Local variables */
    static integer i__, iend, length, istart;

    /* Fortran I/O blocks */
    static cilist io___21 = { 0, 6, 0, fmt_203, 0 };


/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDGNAM           -Name the graphics output file */
/* P.L.Crotty       -OCT88 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -NAME = character string; < 80 characters */
/* CALLS */
/* EXIT CONDITIONS  -output graphics file is assigned the name NAME */
/* NARRATIVE        -This subroutine associates a file name with */
/*                   the graphics output file (KOUTFL). If this */
/*                   routine is not called, a system dependent */
/*                   default name is used.  VDGNAM must be called */
/*                   before VDINIT. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
    switch(n__) {
	case 1: goto L_pstofs;
	case 2: goto L_pstcfs;
	}

/* Computing MIN */
    i__1 = i_len(name__, name_len);
    length = min(i__1,132);
/* Strip off any leading blanks */
    istart = 0;
    i__1 = length;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (*(unsigned char *)&name__[i__ - 1] != ' ') {
	    istart = i__;
	    goto L11;
	}
/* L10: */
    }
L11:
/* Strip off trailing blanks */
    iend = 0;
    if (istart > 0) {
	for (i__ = length; i__ >= 1; --i__) {
	    if (*(unsigned char *)&name__[i__ - 1] != ' ') {
		iend = i__;
		goto L21;
	    }
/* L20: */
	}
    }
L21:
    s_copy(pstnam, name__ + (istart - 1), (ftnlen)132, iend - (istart - 1));
    return 0;
/* CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC */

L_pstofs:
    if (istat == 0) {
	o__1.oerr = 1;
	o__1.ounit = *koutfl;
	o__1.ofnmlen = 132;
	o__1.ofnm = pstnam;
	o__1.orl = 0;
	o__1.osta = "UNKNOWN";
	o__1.oacc = 0;
	o__1.ofm = "FORMATTED";
	o__1.oblnk = 0;
	istat = f_open(&o__1);
	if (istat != 0) {
	    goto L202;
	}
	istat = 1;
    }
    goto L210;
L202:
    s_wsfe(&io___21);
    do_fio(&c__1, (char *)&istat, (ftnlen)sizeof(integer));
    do_fio(&c__1, pstnam, (ftnlen)128);
    e_wsfe();
    s_stop("NOOPEN", (ftnlen)6);
L210:
    return 0;
/* CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC */

L_pstcfs:
    if (istat != 0) {
	cl__1.cerr = 1;
	cl__1.cunit = *koutff;
	cl__1.csta = 0;
	i__1 = f_clos(&cl__1);
	if (i__1 != 0) {
	    goto L303;
	}
L303:
	istat = 0;
    }
    return 0;
} /* vdgnam_ */

/* Subroutine */ int vdgnam_(char *name__, ftnlen name_len)
{
    return vdgnam_0_(0, name__, (integer *)0, (integer *)0, (integer *)0, 
	    name_len);
    }

/* Subroutine */ int pstofs_(integer *koutfl)
{
    return vdgnam_0_(1, (char *)0, koutfl, (integer *)0, (integer *)0, (
	    ftnint)0);
    }

/* Subroutine */ int pstcfs_(integer *koutff, integer *kk)
{
    return vdgnam_0_(2, (char *)0, (integer *)0, koutff, kk, (ftnint)0);
    }

/* Subroutine */ int vdinit_(real *aspect, integer *justif)
{
    extern /* Subroutine */ int pstjob_(), viinit_(real *, integer *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDINIT           -Initialize SVDI. */
/* R.W.Simons       -08APR81 */
/*                   30SEP81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -ASPECT = real ratio of X dimension to Y dimension. */
/*                   Range >0.0.  Default: 0.0 (device dependent). */
/*                   JUSTIF = integer justification of NDC space on the */
/*                   device.  Range 0-9.  Default: 0 (device dependent). */
/* CALLS            -PSTJOB, VBERRH, VIINIT. */
/* EXIT CONDITIONS  -XNDCMX,YNDCMX = real NDC maximum valid values. */
/*                   VECTOR = real array of default attribute values (all */
/*                   device-dependent except VECTOR(4)=0.0). */
/* NARRATIVE        -This must be the first SVDI call made.  All */
/*                   attribute values, the color table, and current */
/*                   position are set to appropriate defaults for the */
/*                   device.  All necessary input device initialization */
/*                   is done.  The screen is cleared or paper advanced */
/*                   if necessary to guarantee a blank view surface for */
/*                   drawing. */
/*                   ASPECT specifies the ratio of the X dimension to the */
/*                   Y dimension.  Maximum NDC values (at least one of */
/*                   which will be 1.0) are computed to give the ASPECT */
/*                   specified.  The default ASPECT (0.0) is device */
/*                   dependent and equals the aspect ratio of the */
/*                   physical device, except for variable aspect devices */
/*                   (such as drum plotters) which are assigned a default */
/*                   aspect of 1.0.  The NDC rectangle is scaled until */
/*                   one dimension fills the corresponding dimension of */
/*                   the device. */
/*                   JUSTIF determines where the rectangle is located on */
/*                   the device as diagrammed below: */
/*                            --------- */
/*                           | 7  8  9 | */
/*                           | 4  5  6 | */
/*                           | 1  2  3 | */
/*                            --------- */
/*                   For example, JUSTIF = 7 indicates NDC space will be */
/*                   upper left justified on the device. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* JOB ID INFORMATION. (HC1, DIC) */
/* COMPUTER DEPENDENT COMMON VARIABLES AND CONSTANTS. */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/*     set common variables */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
    vcpstc_1.kwrtfl = 6;
    vcpstc_1.krdfl = 0;
    vcpstc_1.koutfl = 77;
    vcpstc_1.kinfl = 5;
    vcpstc_1.kwrdsz = 0;
    vcpstc_1.kbytel = 0;
    vcpstc_1.kcpw = 0;
    vcpstc_1.kbaud = 0;
    vcpstc_1.kcomtp = 0;
/* CHECK FOR VALID CLASSIFICATION. Because of output format ignore. */
    pstjob_();
/*     IF(KSECUR.NE.0) THEN */
/*        CALL VBERRH(957,13) */
/*     END IF */
/* THIS IS JUST A DUMMY ROUTINE WHICH DOES NOTHING BUT CALL VIINIT. */
/* THIS ORGANIZATION FACILITATES ADDING SECURITY MARKINGS TO SVDI. */
    viinit_(aspect, justif);
    return 0;
} /* vdinit_ */

/* Subroutine */ int vdiqnd_(real *xndc, real *yndc)
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDIQND           -Inquire NDC Space. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -XNDCMX,YNDCMX = real maximum valid NDC values. */
/* CALLS            - */
/* EXIT CONDITIONS  -XNDC,YNDC = real maximum valid NDC values (XNDCMX, */
/*                   YNDCMX). */
/* NARRATIVE        -Return the maximum NDC values as set to realize the */
/*                   aspect defined by VDINIT. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* MAXIMUM VALID NDC VALUES. (DEVICE-INDEPENDENT) */
/* RETURN THE MAXIMUM VALID NDC VALUES. */
    *xndc = vcndcm_1.xndcmx;
    *yndc = vcndcm_1.yndcmx;
    return 0;
} /* vdiqnd_ */

/* Subroutine */ int vdiqos_(real *attarr)
{
    static integer i__;

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDIQOS           -Inquire Output Status (of Attributes). */
/* K.M. ERICKSON    -14 NOV 80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -VECTOR = real array of current attribute values. */
/* CALLS            - */
/* EXIT CONDITIONS  -ATTARR = real array of current attribute value */
/*                   (VECTOR). */
/* NARRATIVE        -Return the current attribute values in ATTARR as */
/*                   given below. */
/*                   ATTARR(1)=Foreground Color */
/*                         (2)=Background Color */
/*                         (3)=Intensity */
/*                         (4)=Line Style */
/*                         (5)=Line Width */
/*                         (6)=Character Box Y */
/*                         (7)=Character Box X */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
    /* Parameter adjustments */
    --attarr;

    /* Function Body */
    for (i__ = 1; i__ <= 7; ++i__) {
	attarr[i__] = vcattr_1.vector[i__ - 1];
/* L100: */
    }
    return 0;
} /* vdiqos_ */

/* Subroutine */ int vdlina_(real *x, real *y)
{
    extern /* Subroutine */ int vilina_(real *, real *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDLINA           -Line Absolute. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -X,Y = real NDC position. */
/* CALLS            -VILINA. */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Draw a line from current position to absolute NDC */
/*                   position X,Y and update current position. */
/*                   Attributes foreground color, intensity, line style, */
/*                   and line width apply. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* THIS IS JUST A DUMMY ROUTINE WHICH DOES NOTHING BUT CALL VILINA. */
/* THIS ORGANIZATION FACILITATES ADDING SECURITY MARKINGS TO SVDI. */
    vilina_(x, y);
    return 0;
} /* vdlina_ */

/* Subroutine */ int vdloge_(integer *errnum, integer *errsev)
{
    /* Format strings */
    static char fmt_10[] = "(\002 SVDI ERROR NUMBER \002,i5,\002   SEVERITY \
CODE \002,i5)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    extern /* Subroutine */ int vdbufl_();

    /* Fortran I/O blocks */
    static cilist io___23 = { 0, 0, 0, fmt_10, 0 };


/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDLOGE           -Log Error. */
/* R.W.Simons       -08APR81 */
/* K.M.Erickson     -8OCT84 - add buffer flush */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -ERRNUM = integer error number. */
/*                   ERRSEV = integer error severity. */
/* CALLS            -PSTTBK, VDBUFL */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Report error with message to user and possibly */
/*                   terminate job depending on severity.  Notice that */
/*                   by judicious use of the error routines (see VBERRH) */
/*                   it is possible to write very "nice" error routines */
/*                   that, for example, only report the first two */
/*                   occurrences of a particular error, or terminate */
/*                   if more than 10 errors of a particular severity */
/*                   occur. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* COMPUTER DEPENDENT COMMON VARIABLES AND CONSTANTS. */
/* flush buffer before we do a write */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
    vdbufl_();
/* WRITE THE ERROR TO THE LISTING. */
    io___23.ciunit = vcpstc_1.kwrtfl;
    s_wsfe(&io___23);
    do_fio(&c__1, (char *)&(*errnum), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*errsev), (ftnlen)sizeof(integer));
    e_wsfe();
/* TRACEBACK. */
/* sam  CALL PSTTBK */
    return 0;
} /* vdloge_ */

/* Subroutine */ int vdmoni_0_(int n__, integer *istate, char *c1, char *c2, 
	ftnlen c1_len, ftnlen c2_len)
{
    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDMONI           -Logs Usage Information.. */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -ISTATE = 0 - initialization */
/*                            1 - new page */
/*                            2 - terminate */
/* CALLS */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -For ISTATE=0, job information is initialized, and */
/*                   timers are initialized called by VIINIT. */
/*                   ISTATE=1 will increment a common block page */
/*                   counter called by VINWPG. */
/*                   ISTATE=2 is called by VITERM and will cause */
/*                   the collected usage monitoring information to */
/*                   be written to a file. */
/*                   Contains entry points VBPKG which will has */
/*                   an 8 character parameter which will set a common */
/*                   block variable specifying the package being used. */
/*                   Entry point VBDEV has an 8 character parameter */
/*                   which will set a common block variable specifying */
/*                   the device being used. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/*     dummy routine */
    switch(n__) {
	case 1: goto L_vbpkg;
	case 2: goto L_vbdev;
	case 3: goto L_vbiqpk;
	case 4: goto L_vbiqdv;
	}

    return 0;
/* Usage Monitoring Information */

L_vbpkg:
    return 0;

L_vbdev:
    return 0;

L_vbiqpk:
    s_copy(c1, " ", c1_len, (ftnlen)1);
    return 0;

L_vbiqdv:
    s_copy(c2, " ", c2_len, (ftnlen)1);
    return 0;
} /* vdmoni_ */

/* Subroutine */ int vdmoni_(integer *istate)
{
    return vdmoni_0_(0, istate, (char *)0, (char *)0, (ftnint)0, (ftnint)0);
    }

/* Subroutine */ int vbpkg_(char *c1, ftnlen c1_len)
{
    return vdmoni_0_(1, (integer *)0, c1, (char *)0, c1_len, (ftnint)0);
    }

/* Subroutine */ int vbdev_(char *c2, ftnlen c2_len)
{
    return vdmoni_0_(2, (integer *)0, (char *)0, c2, (ftnint)0, c2_len);
    }

/* Subroutine */ int vbiqpk_(char *c1, ftnlen c1_len)
{
    return vdmoni_0_(3, (integer *)0, c1, (char *)0, c1_len, (ftnint)0);
    }

/* Subroutine */ int vbiqdv_(char *c2, ftnlen c2_len)
{
    return vdmoni_0_(4, (integer *)0, (char *)0, c2, (ftnint)0, c2_len);
    }

/* Subroutine */ int vdmova_(real *x, real *y)
{
    extern /* Subroutine */ int vimova_(real *, real *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDMOVA           -Move Absolute. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -X,Y = real NDC position. */
/* CALLS            -VIMOVA */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Set current position to absolute NDC position X,Y. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* THIS IS JUST A DUMMY ROUTINE WHICH DOES NOTHING BUT CALL VIMOVA. */
/* THIS ORGANIZATION FACILITATES ADDING SECURITY MARKINGS TO SVDI. */
    vimova_(x, y);
    return 0;
} /* vdmova_ */

/* Subroutine */ int vdnwpg_()
{
    extern /* Subroutine */ int vinwpg_();

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDNWPG           -New Page. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS - */
/* CALLS            -VINWPG. */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Physically advance the medium or clear the screen, */
/*                   whichever is appropriate.  Also flood the screen */
/*                   with the background color on devices that support */
/*                   this function. The current position is not changed. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* THIS IS JUST A DUMMY ROUTINE WHICH DOES NOTHING BUT CALL VINWPG. */
/* THIS ORGANIZATION FACILITATES ADDING SECURITY MARKINGS TO SVDI. */
    vinwpg_();
    return 0;
} /* vdnwpg_ */

/* Subroutine */ int vdpnta_(real *x, real *y)
{
    extern /* Subroutine */ int vipnta_(real *, real *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDPNTA           -Point Absolute. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -X,Y = real NDC position. */
/* CALLS            -VIPNTA. */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Set current position to absolute NDC position X,Y */
/*                   and put a visible point there.  Attributes */
/*                   foreground color and intensity apply. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* THIS IS JUST A DUMMY ROUTINE WHICH DOES NOTHING BUT CALL VIPNTA. */
/* THIS ORGANIZATION FACILITATES ADDING SECURITY MARKINGS TO SVDI. */
    vipnta_(x, y);
    return 0;
} /* vdpnta_ */

/* Subroutine */ int vdpoly_(real *xarray, real *yarray, integer *npts)
{
    extern /* Subroutine */ int vipoly_(real *, real *, integer *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDPOLY           -POLYGON FILL ROUTINE */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS -XARRAY-ARRAY OF X VALUES OF THE POLYGON */
/*                   YARRAY-CORRESPONDING ARRAY OF Y VALUES */
/*                   NPTS- NUMBER OF POINTS IN (XARRAY,YARRAY) */
/* CALLS            -VIPOLY */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -The polygon defined by XARRAY,YARRAY will be drawn */
/*                   and filled (constrained by any limitations of the */
/*                   physical device -- see below).  No checking will be */
/*                   done -- all points will be passed to the device. */
/*                   Current foreground color is used and the polygon */
/*                   boundary is drawn using the solid line style. */
/*                   VDI will close the polygon (i.e. the last point */
/*                   will be connected to the first). */
/*                   The level of support for this primitive is device- */
/*                   dependent.  The level of support is categorized */
/*                   as follows: */
/*                     Level 0 -- no polygon fill.  Only the polygon */
/*                        boundary is drawn. */
/*                     Level 1 -- the device fills convex polygons. */
/*                     Level 2 -- the device fills simple polygons (may */
/*                        be concave but not self-crossing) */
/*                     Level 3 -- full support for complex polygons (may */
/*                        be self-crossing). In general, the interior of */
/*                        a complex polygon is defined by the set of points */
/*                        such that, for each point, when an imaginary line */
/*                        is drawn to that point from a point far outside */
/*                        the polygon, that line intersects the polygon */
/*                        boundary an odd number of times. */
/*                   Note that the level of support for a particular device */
/*                   can be inquired using the function VDIQDC. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* THIS IS JUST A DUMMY ROUTINE WHICH DOES NOTHING BUT CALL VIPOLY. */
/* THIS ORGANIZATION FACILITATES ADDING SECURITY MARKINGS TO SVDI. */
/*     mopoly controls polygon fill =0, on ;  =1, off */
/*     mocolr controls color =0, on ;  =1, off */
    /* Parameter adjustments */
    --yarray;
    --xarray;

    /* Function Body */
    if (vcpstb_1.mopoly == 0) {
	vipoly_(&xarray[1], &yarray[1], npts);
    }
    return 0;
} /* vdpoly_ */

/* Subroutine */ int vdstos_(real *attarr)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer i_nint(real *);

    /* Local variables */
    extern /* Subroutine */ int vdstbc_(integer *), vdstfc_(integer *), 
	    vdstin_(real *), vdstls_(integer *), vdstlw_(real *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDSTOS           -Set Output Status (of Attributes). */
/* K.M. ERICKSON    -14 NOV 80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -ATTARR = real array of attribute values. */
/* CALLS            -VDSTBC,VDSTCS,VDSTFC,VDSTIN,VDSTLS,VDSTLW */
/* EXIT CONDITIONS  -VECTOR = real updated attribute values (ATTARR). */
/* NARRATIVE        -Set the attribute values from ATTARR as given below. */
/*                   ATTARR(1)=Foreground Color */
/*                         (2)=Background Color */
/*                         (3)=Intensity */
/*                         (4)=Line Style */
/*                         (5)=Line Width */
/*                         (6)=Character Box Y */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* CALL EACH OF THE INDIVIDUAL ATTRIBUTE SETTING ROUTINES. */
/* CHECK FOR VALIDITY OF INPUT VALUES WILL BE DONE IN EACH INDIVIDUAL */
/* ROUTINE. */
    /* Parameter adjustments */
    --attarr;

    /* Function Body */
    i__1 = i_nint(&attarr[1]);
    vdstfc_(&i__1);
    i__1 = i_nint(&attarr[2]);
    vdstbc_(&i__1);
    vdstin_(&attarr[3]);
    i__1 = i_nint(&attarr[4]);
    vdstls_(&i__1);
    vdstlw_(&attarr[5]);
/*      CALL VDSTCS(ATTARR(6)) */
    return 0;
} /* vdstos_ */

/* Subroutine */ int vdterm_()
{
    extern /* Subroutine */ int viterm_();

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDTERM           -Terminate SVDI. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS - */
/* CALLS            -VITERM. */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Terminate the SVDI by flushing buffers, etc.  This */
/*                   should be the last SVDI call made. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* THIS IS JUST A DUMMY ROUTINE WHICH DOES NOTHING BUT CALL VITERM. */
/* THIS ORGANIZATION FACILITATES ADDING SECURITY MARKINGS TO SVDI. */
    viterm_();
    return 0;
} /* vdterm_ */

/* Subroutine */ int vdtext_(integer *length, integer *chars)
{
    extern /* Subroutine */ int vitext_(integer *, integer *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDTEXT           -Text from Array. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   Device-independent. */
/* ENTRY CONDITIONS -LENGTH = integer number of characters in CHARS. */
/*                   Range 1-136. */
/*                   CHARS = integer array of ASCII characters, one */
/*                   character per array element, right justified. */
/*                   Range 8,10,32-126. */
/* CALLS            -VITEXT. */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Draw LENGTH characters in CHARS, starting at */
/*                   current position and update current position to */
/*                   the point after the last character box where the */
/*                   next character would begin.  Current position */
/*                   indicates the lower left corner of the first */
/*                   character box.  Only printable characters (32-126 */
/*                   decimal) and backspace and linefeed are allowed. */
/*                   All values in this range must produce "reasonable" */
/*                   output; mapping lower case to upper case letters is */
/*                   considered reasonable.  Attributes foreground color, */
/*                   background color, intensity, and  character size */
/*                   apply. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* THIS IS JUST A DUMMY ROUTINE WHICH DOES NOTHING BUT CALL VITEXT. */
/* THIS ORGANIZATION FACILITATES ADDING SECURITY NARKINGS TO SVDI. */
    /* Parameter adjustments */
    --chars;

    /* Function Body */
    vitext_(length, &chars[1]);
    return 0;
} /* vdtext_ */

/* Subroutine */ int vdfram_(integer *itype)
{
    extern /* Subroutine */ int vifram_(integer *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDFRAM           - Draw header or trailer frame */
/* P. Watterberg    - 27 Aug 81 */
/* ENVIRONMENT      -Computer-independent, system-independent, FORTRAN 77 */
/* ENTRY CONDITIONS - ITYPE = 0   for header frame */
/*                          = 1   for trailer frame */
/* CALLS            - VIFRAM */
/* EXIT CONDITIONS  - */
/* NARRATIVE        - Calls vifram to get time and date from the */
/*                    system via the computer-dependent routine PSTTOD(entry */
/*                    point in PSTJOB) and writes it on an identification frame. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
    vifram_(itype);
    return 0;
} /* vdfram_ */

/* Subroutine */ int vifram_(integer *itype)
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VIFRAM           - Draw header or trailer frame */
/* P. Watterberg    - 27 Aug 81 */
/* ENVIRONMENT      -Computer-independent, system-independent, FORTRAN 77 */
/* ENTRY CONDITIONS - ITYPE = 0   for header frame */
/*                          = 1   for trailer frame */
/* CALLS            - */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -NULL ROUTINE */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
    return 0;
} /* vifram_ */

/* Subroutine */ int vdaabu_(integer *btnnum)
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDAABU           -Await Any Button. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Batch Devices. */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  -BTNNUM = integer number of the button pressed. */
/*                   Range 1 to a device dependent maximum which must be */
/*                   at least 8. */
/* NARRATIVE        -When a button has been pressed, its integer button */
/*                   number is returned in BTNNUM.  This function flushes */
/*                   the button buffer, if any.  This function flushes */
/*                   the output buffers before doing input. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* COMPUTER DEPENDENT COMMON VARIABLES AND CONSTANTS. */
/* BATCH DEVICES DON'T NEED TO FLUSH BUFFERS. */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
    *btnnum = 32;
    return 0;
} /* vdaabu_ */

/* Subroutine */ int vdabgl_(integer *btnnum, real *x, real *y)
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDABGL           -Await Button, Get Locator. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Batch Devices. */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  -BTNNUM = integer number of the button pressed. */
/*                   Range 1 to a device dependent maximum that must be */
/*                   at least 8. */
/*                   X,Y = real NDC position of the locator. */
/* NARRATIVE        -Wait until a button is hit, then return the number */
/*                   of the button in BTNNUM and the NDC value of the */
/*                   locator in X,Y.  This function flushes the output */
/*                   buffers before doing input. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* COMPUTER DEPENDENT COMMON VARIABLES AND CONSTANTS. */
/* BATCH DEVICES DON'T NEED TO FLUSH BUFFERS. */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
    *btnnum = 32;
    *x = (float)0.;
    *y = (float)0.;
    return 0;
} /* vdabgl_ */

/* Subroutine */ int vdakgl_(integer *char__, real *x, real *y)
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDAKGL           -Await Keyboard, Get Locator. */
/* R.W.SIMONS       -02DEC80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Batch Devices. */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  -CHAR = integer ASCII character input from the */
/*                   keyboard, right-justified, zero fill.  Range 32-126. */
/*                   X,Y = real NDC position of the locator. */
/* NARRATIVE        -Wait until a key is hit, then return the character */
/*                   entered in CHAR and the NDC value of the locator */
/*                   in X,Y.  If the character entered does not fall in */
/*                   the range 32-126, a blank(32) is returned in CHAR. */
/*                   This function flushes the output buffers before */
/*                   doing input. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/*     dummy routine */
    *char__ = 32;
    *x = (float)0.;
    *y = (float)0.;
    return 0;
} /* vdakgl_ */

/* Subroutine */ int vdaloc_(real *x, real *y)
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDALOC           -Await Locator. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Batch Devices. */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  -X,Y = real NDC position of the locator. */
/* NARRATIVE        -Wait until the locator is positioned, then return */
/*                   the NDC value of the locator in X,Y.  The fact that */
/*                   the locator is positioned can be signaled in a */
/*                   variety of device dependent ways, such as clicking */
/*                   the switch in a tablet pen, hitting a button, or */
/*                   hitting a key on the keyboard.  Any information */
/*                   contained in this signal is ignored by this */
/*                   function, as only the locator position is returned. */
/*                   This function flushes the output buffers before */
/*                   doing input. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* COMPUTER DEPENDENT COMMON VARIABLES AND CONSTANTS. */
/* BATCH DEVICES DON'T NEED TO FLUSH BUFFERS. */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
    *x = (float)0.;
    *y = (float)0.;
    return 0;
} /* vdaloc_ */

/* Subroutine */ int vdbell_()
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDBELL           -Ring Bell */
/* R.W.SIMONS       -02DEC80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Batch Devices. */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Ring user's bell to get his attention.  This */
/*                   function is ignored by batch devices. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* THIS FUNCTION IS IGNORED BY BATCH DEVICES. */
    return 0;
} /* vdbell_ */

/* Subroutine */ int vdbufl_()
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDBUFL           -Buffer Flush. */
/* R.W.Simons       -19DEC80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Batch Devices. */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Assure that the picture is up-to-date by flushing */
/*                   buffers if necessary.  Also prepare the device to */
/*                   operate in alphanumeric (as opposed to graphic) */
/*                   mode.  This is necessary on some devices so that */
/*                   alphanumeric data from FORTRAN I/O won't be */
/*                   misinterpreted as graphic data. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* THIS FUNCTION IS IGNORED BY BATCH DEVICES. */
    return 0;
} /* vdbufl_ */

/* Subroutine */ int vdstla_(real *locx, real *locy)
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDSTLA           -Set Initial Locator Position. */
/* R.W.Simons       -08APR81 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Batch Devices. */
/* ENTRY CONDITIONS -LOCX,LOCY = real NDC position that the locator is */
/*                   initilaized to. */
/* CALLS            - */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Set the initial locator position (light pen tracking */
/*                   cross, for example) each time this function is */
/*                   called. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* BATCH DEVICES IGNORE THIS FUNCTION. */
    return 0;
} /* vdstla_ */

/* Subroutine */ int vdwait_()
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDWAIT           -Wait for User. */
/* R.W.SIMONS       -02DEC80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Batch Devices. */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Wait for the user to view the screen and signal he */
/*                   is done, normally by hitting any key.  This function */
/*                   flushes the output buffers before doing input. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* BATCH DEVICES IGNORE THIS COMMAND. */
    return 0;
} /* vdwait_ */

/* Subroutine */ int vdiqco_(integer *num, integer *index, real *clrary, 
	integer *clrmod)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    static integer i__, indexn;
    extern /* Subroutine */ int vberrh_(integer *, integer *);

/*     C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */

/*     VDIQCO           -Inquire Color Table. */

/*     R.W.Simons       -08APR81 */
/*     H. S. LAUSON      29MAY86 - changed for current HLS interpretation */

/*     ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*     All Black and White Devices. (LXY, HC1, ALP) */

/*     ENTRY CONDITIONS -NUM = integer number of color indexes to inquire. */
/*     Range 1-256. */
/*     INDEX = integer array of indexes to inquire.  Range */
/*     0-255. */
/*     CLRMOD = integer color model to be used.  Range 0,1. */

/*     CALLS            -VBERRH */

/*     EXIT CONDITIONS  -CLRARY = real array of 3 by NUM elements returning */
/*     the values of the components of the indexes inquired. */
/*     Range for RGB: red 0.0-1.0 */
/*     green 0.0-1.0 */
/*     blue 0.0-1.0 */
/*     Range for HLS: hue 0.0-360.0 */
/*     lightness 0.0-1.0 */
/*     saturation 0.0-1.0 */

/*     NARRATIVE        -Inquire one or more color table entries.  NUM and */
/*     INDEX specify how many and which indexes are being */
/*     inquired.  CLRMOD specifies which color model */
/*     (0=RGB, 1=HLS) should be used in constructing values */
/*     to return in CLRARY.  A device which does not */
/*     support a color table index specified will */
/*     return -1.0 in the first element of the CLRARY value */
/*     for that index. */

/*     C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */



/*     CHECK FOR VALID NUM. */
    /* Parameter adjustments */
    clrary -= 4;
    --index;

    /* Function Body */
    if (*num < 1 || *num > 256) {
	vberrh_(&c__723, &c__5);
	goto L999;
    }

/*     CHECK FOR VALID CLRMOD. */
    if (*clrmod != 0 && *clrmod != 1) {
	vberrh_(&c__725, &c__5);
	goto L999;
    }

    if (*clrmod != 0) {
	s_stop("HLS COLORS NOT SUPPORTED", (ftnlen)24);
    }

/*     CHECK FOR VALID INDEXES. */
    i__1 = *num;
    for (i__ = 1; i__ <= i__1; ++i__) {
	indexn = index[i__];
	if (indexn < 0 || indexn > 255) {
	    vberrh_(&c__724, &c__5);
	    goto L100;
	}
	clrary[i__ * 3 + 1] = pcolst_1.pcols[indexn * 3 - 3];
	clrary[i__ * 3 + 2] = pcolst_1.pcols[indexn * 3 - 2];
	clrary[i__ * 3 + 3] = pcolst_1.pcols[indexn * 3 - 1];
L100:
	;
    }

L999:
    return 0;
} /* vdiqco_ */

/* Subroutine */ int vdiqcp_(real *x, real *y)
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDIQCP           -Inquire Where Current Position Is. */
/* R.W.Simons       -02DEC80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Devices that support a software CP. */
/*                   (AP5,GER,H50,HC1,HCB,HPP,I10,I30,LXY,QCR,QMS,XYN) */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  -X,Y = real NDC position. */
/* NARRATIVE        -Return the value of current position. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* CURRENT POSITION. */
/* ASSIGN THE CP TO X,Y. */
    *x = vccrps_1.xcp;
    *y = vccrps_1.ycp;
    return 0;
} /* vdiqcp_ */

/* Subroutine */ int vdstbc_(integer *color)
{
    extern /* Subroutine */ int vberrh_(integer *, integer *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDSTBC           -Set Background Color. */
/* R.W.Simons       -05DEC80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Devices with a constant white background. (LXY, */
/*                   HC1, ALP) */
/* ENTRY CONDITIONS -COLOR = integer color table index. Range 0-255. */
/*                   Default: device dependent, in range 0-7. */
/* CALLS            -VBERRH */
/* EXIT CONDITIONS  -VECTOR(2) = real updated background color (COLOR). */
/* NARRATIVE        -Set the background color for following VDNWPG or */
/*                   TEXT primitives for devices supporting these */
/*                   features.  For example, many raster devices support */
/*                   both an overall background color and a background */
/*                   for character drawing(e.g.,red letters on a green */
/*                   box). */
/*                   All devices must support at least a single device */
/*                   dependent value in the range 0-7. */
/*                   If an unsupported value is specified, set to */
/*                   default. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* CHECK FOR VALID COLOR. */
    if (*color < 0 || *color > 255) {
	vberrh_(&c__724, &c__5);
	goto L999;
    }
/* ONLY THE SINGLE BACKGROUND COLOR 7 (WHITE) IS SUPPORTED, */
/* SO NO ACTION IS NECESSARY. */
    vcattr_1.vector[1] = (real) (*color);
L999:
    return 0;
} /* vdstbc_ */

/* Subroutine */ int vdstco_(integer *num, integer *index, real *clrary, 
	integer *clrmod)
{
    /* Format strings */
    static char fmt_300[] = "(f5.3,2f6.3,\002 q}\002)";

    /* System generated locals */
    address a__1[4];
    integer i__1, i__2, i__3[4];
    char ch__1[32];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi();
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen),
	     s_stop(char *, ftnlen);

    /* Local variables */
    static integer i__, ic, nnn;
    static real clrar1, clrar2, clrar3;
    static char kolind[6], kolcom[20];
    static integer indexn;
    extern /* Subroutine */ int vberrh_(integer *, integer *), pstbuf_(
	    integer *, char *, ftnlen);

    /* Fortran I/O blocks */
    static icilist io___33 = { 1, kolind+2, 0, "(I1)", 1, 1 };
    static icilist io___35 = { 1, kolind+2, 0, "(I2)", 2, 1 };
    static icilist io___36 = { 1, kolind+2, 0, "(I3)", 3, 1 };
    static icilist io___38 = { 1, kolcom, 0, fmt_300, 20, 1 };


/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDSTCO           -Set Color Table. */
/* R.W.SIMONS       -02DEC80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Black and White Devices. (LXY, HC1, ALP) */
/* ENTRY CONDITIONS -NUM = integer number of color indexes to be set. */
/*                   Range 1-256. */
/*                   INDEX = integer array of indexes to be set.  Range */
/*                   0-255. */
/*                   CLRARY = real array of 3 by NUM elements specifying */
/*                   the values of the components of the index to be */
/*                   set. */
/*                   Range for RGB: red 0.0-1.0 */
/*                                  green 0.0-1.0 */
/*                                  blue 0.0-1.0 */
/*                   Range for HLS: hue 0.0-360.0 */
/*                                  lightness 0.0-1.0 */
/*                                  saturation 0.0-1.0 */
/*                   Default: */
/*                   Index  Color  RGB Values */
/*                     0    black   0.,0.,0. */
/*                     1    red     1.,0.,0. */
/*                     2    green   0.,1.,0. */
/*                     3    yellow  1.,1.,0. */
/*                     4    blue    0.,0.,1. */
/*                     5    magenta 1.,0.,1. */
/*                     6    cyan    0.,1.,1. */
/*                     7    white   1.,1.,1. */
/*                   CLRMOD = integer color model being used.  Range 0,1. */
/*                   Default: 0 (RGB). */
/* CALLS            -VBERRH */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Set one or more color table entries.  This is a */
/*                   dynamic setting, if the device will support it. */
/*                   "Dynamic" neans that primitives which have already */
/*                   been drawn will change their appearance when a */
/*                   dynamic setting is changed.  INDEX is the */
/*                   position (or array of positions) in the table */
/*                   (0-255).  CLRARY is a three-element vector (or 3 by */
/*                   NUM array) with the fractions (0.-1.) of RGB or */
/*                   values (0.0-360.0, 0.0-1.0, 0.0-1.0) of HLS. */
/*                   A translator for HLS to RGB will be used from */
/*                   GSPC79.  CLRMOD specifies which color model is being */
/*                   used (0=RGB, 1=HLS). */
/*                   All devices must support at least a single device */
/*                   dependent value for each of red, green, and blue and */
/*                   the corresponding values for hue, lightness, and */
/*                   saturation.  If unsupported values are specified, */
/*                   set to the closest supported values. */
/*                   All devices must support both RGB and HLS color */
/*                   models. */
/*                   All devices must support at least a single device */
/*                   dependent INDEX value in the range 0-7.  If an */
/*                   unsupported value is specified, it should be ignored. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/*     ARRAY TO CONTAIN COMPLETE COLOR TABLE */
/*     mopoly controls polygon fill =0, on ;  =1, off */
/*     mocolr controls color =0, on ;  =1, off */
/* CHECK FOR VALID NUM. */
    /* Parameter adjustments */
    clrary -= 4;
    --index;

    /* Function Body */
    if (*num < 1 || *num > 256) {
	vberrh_(&c__723, &c__5);
	goto L999;
    }
/* CHECK FOR VALID CLRMOD. */
    if (*clrmod != 0 && *clrmod != 1) {
	vberrh_(&c__725, &c__5);
	goto L999;
    }
/* CHECK FOR VALID INDEXES. */
    i__1 = *num;
    for (i__ = 1; i__ <= i__1; ++i__) {
	indexn = index[i__];
	if (indexn < 0 || indexn > 255) {
	    vberrh_(&c__724, &c__5);
	    goto L100;
	}
/* CHECK FOR VALID CLRARY. */
	clrar1 = clrary[i__ * 3 + 1];
	clrar2 = clrary[i__ * 3 + 2];
	clrar3 = clrary[i__ * 3 + 3];
	if (*clrmod == 0) {
	    if (clrar1 < (float)0. || clrar1 > (float)1. || clrar2 < (float)
		    0. || clrar2 > (float)1. || clrar3 < (float)0. || clrar3 
		    > (float)1.) {
		vberrh_(&c__727, &c__5);
		goto L100;
	    }
/* 256 INDEXES ARE SUPPORTED: */
	    for (ic = 1; ic <= 3; ++ic) {
		pcolst_1.pcols[ic + (indexn + 1) * 3 - 4] = clrary[ic + i__ * 
			3];
	    }
/*           define symbol for color reference */
	    if (vcpstb_1.mocolr != 0) {
		goto L390;
	    }
/*           if a set of vectors was in process, issue stroke command */
/*           to draw them - then start a new path. */
	    if (vcvec1_1.ivect != 0) {
		pstbuf_(&c__2, "s ", (ftnlen)2);
		vcvec1_1.ivect = 0;
	    }
	    pstbuf_(&c__0, " ", (ftnlen)1);
	    pstbuf_(&c__2, "r ", (ftnlen)2);
	    s_copy(kolind, "/c", (ftnlen)6, (ftnlen)2);
	    if (indexn <= 9) {
		i__2 = s_wsfi(&io___33);
		if (i__2 != 0) {
		    goto L310;
		}
		i__2 = do_fio(&c__1, (char *)&indexn, (ftnlen)sizeof(integer))
			;
		if (i__2 != 0) {
		    goto L310;
		}
		i__2 = e_wsfi();
		if (i__2 != 0) {
		    goto L310;
		}
		nnn = 4;
	    } else if (indexn <= 99) {
		i__2 = s_wsfi(&io___35);
		if (i__2 != 0) {
		    goto L310;
		}
		i__2 = do_fio(&c__1, (char *)&indexn, (ftnlen)sizeof(integer))
			;
		if (i__2 != 0) {
		    goto L310;
		}
		i__2 = e_wsfi();
		if (i__2 != 0) {
		    goto L310;
		}
		nnn = 5;
	    } else {
		i__2 = s_wsfi(&io___36);
		if (i__2 != 0) {
		    goto L310;
		}
		i__2 = do_fio(&c__1, (char *)&indexn, (ftnlen)sizeof(integer))
			;
		if (i__2 != 0) {
		    goto L310;
		}
		i__2 = e_wsfi();
		if (i__2 != 0) {
		    goto L310;
		}
		nnn = 6;
	    }
	    i__2 = s_wsfi(&io___38);
	    if (i__2 != 0) {
		goto L310;
	    }
	    for (ic = 1; ic <= 3; ++ic) {
		i__2 = do_fio(&c__1, (char *)&pcolst_1.pcols[ic + (indexn + 1)
			 * 3 - 4], (ftnlen)sizeof(real));
		if (i__2 != 0) {
		    goto L310;
		}
	    }
	    i__2 = e_wsfi();
	    if (i__2 != 0) {
		goto L310;
	    }
	    i__2 = nnn + 26;
/* Writing concatenation */
	    i__3[0] = nnn, a__1[0] = kolind;
	    i__3[1] = 1, a__1[1] = "{";
	    i__3[2] = 20, a__1[2] = kolcom;
	    i__3[3] = 5, a__1[3] = " def ";
	    s_cat(ch__1, a__1, i__3, &c__4, (ftnlen)32);
	    pstbuf_(&i__2, ch__1, nnn + 26);
L310:
/*           save and restore can not be in same line - why? */
	    pstbuf_(&c__0, " ", (ftnlen)1);
	    pstbuf_(&c__1, "v", (ftnlen)1);
	    pstbuf_(&c__0, " ", (ftnlen)1);
L390:
	    ;
	} else {
	    if (clrar1 < (float)0. || clrar1 > (float)360. || clrar2 < (float)
		    0. || clrar2 > (float)1. || clrar3 < (float)0. || clrar3 
		    > (float)1.) {
		vberrh_(&c__727, &c__5);
		goto L100;
	    }
/* 256 INDEXES ARE SUPPORTED: */
	    s_stop("HLS COLORS NOT AVAILABLE", (ftnlen)24);
	}
L100:
	;
    }
L999:
    return 0;
} /* vdstco_ */

/* Subroutine */ int vdstfc_(integer *color)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi();

    /* Local variables */
    static integer nnn, kolor;
    static char kolind[5];
    extern /* Subroutine */ int vberrh_(integer *, integer *), pstbuf_(
	    integer *, char *, ftnlen);

    /* Fortran I/O blocks */
    static icilist io___41 = { 1, kolind+1, 0, "(I1)", 1, 1 };
    static icilist io___43 = { 1, kolind+1, 0, "(I2)", 2, 1 };
    static icilist io___44 = { 1, kolind+1, 0, "(I3)", 3, 1 };


/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDSTFC           -Set Foreground Color. */
/* R.W.Simons       -05DEC80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Devices with a constant black foreground. (LXY, */
/*                   HC1) */
/* ENTRY CONDITIONS -COLOR = integer color table index . Range 0-255. */
/*                   Default is device dependent, in range 0-7. */
/* CALLS            -VBERRH */
/* EXIT CONDITIONS  -VECTOR(1) = real updated foreground color (COLOR). */
/* NARRATIVE        -Set the foreground color index, i.e., the color */
/*                   table index used for drawing future primitives. */
/*                   Color is an integer from 0-255 which is used as an */
/*                   index into the color table (see VDSTCO). */
/*                   All devices must support at least a single device */
/*                   dependent value in the range 0-7. */
/*                   If an unsupported value is specified, set to */
/*                   default. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/*     ARRAY TO CONTAIN COMPLETE COLOR TABLE */
/*     pstmlw controls minimum line width */
/*     kpstbg controls background coloring */
/*            = 0,  not colored (white ground from paper) */
/*            = 1,  colored black */
/*     kpstci controls black-white interchange (colors 0 & 7 only) */
/*            = 0,  no interchange */
/*            = 1,  colors interchanged */
/*     mopoly controls polygon fill =0, on ;  =1, off */
/*     mocolr controls color =0, on ;  =1, off */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* CHECK FOR VALID COLOR. */
    if (*color < 0 || *color > 255) {
	vberrh_(&c__724, &c__5);
	goto L999;
    }
    vcattr_1.vector[0] = (real) (*color);
    if (vcpstb_1.mocolr == 0) {
/*       draw any vectors in stack before changing colors */
	if (vcvec1_1.ivect != 0) {
	    pstbuf_(&c__4, "s r ", (ftnlen)4);
	    pstbuf_(&c__0, " ", (ftnlen)1);
	    pstbuf_(&c__4, "v o ", (ftnlen)4);
	    pstbuf_(&c__0, " ", (ftnlen)1);
	    vcvec1_1.ivect = 0;
	}
/*       code using symbols */
	s_copy(kolind, "c", (ftnlen)5, (ftnlen)1);
	if (*color <= 9) {
	    kolor = *color;
/*         test for interchange of colors 0 and 7 */
	    if (vcpsta_1.kpstci != 0) {
		if (kolor == 7) {
		    kolor = 0;
		} else if (kolor == 0) {
		    kolor = 7;
		}
	    }
	    i__1 = s_wsfi(&io___41);
	    if (i__1 != 0) {
		goto L999;
	    }
	    i__1 = do_fio(&c__1, (char *)&kolor, (ftnlen)sizeof(integer));
	    if (i__1 != 0) {
		goto L999;
	    }
	    i__1 = e_wsfi();
	    if (i__1 != 0) {
		goto L999;
	    }
	    nnn = 3;
	} else if (*color <= 99) {
	    i__1 = s_wsfi(&io___43);
	    if (i__1 != 0) {
		goto L999;
	    }
	    i__1 = do_fio(&c__1, (char *)&(*color), (ftnlen)sizeof(integer));
	    if (i__1 != 0) {
		goto L999;
	    }
	    i__1 = e_wsfi();
	    if (i__1 != 0) {
		goto L999;
	    }
	    nnn = 4;
	} else {
	    i__1 = s_wsfi(&io___44);
	    if (i__1 != 0) {
		goto L999;
	    }
	    i__1 = do_fio(&c__1, (char *)&(*color), (ftnlen)sizeof(integer));
	    if (i__1 != 0) {
		goto L999;
	    }
	    i__1 = e_wsfi();
	    if (i__1 != 0) {
		goto L999;
	    }
	    nnn = 5;
	}
	pstbuf_(&nnn, kolind, nnn);
    }
L999:
    return 0;
} /* vdstfc_ */

/* Subroutine */ int vdstin_(real *inten)
{
    extern /* Subroutine */ int vberrh_(integer *, integer *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDSTIN           -Set Intensity. */
/* R.W.Simons       -05DEC80 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Single Intensity Devices. (LXY, HC1) */
/* ENTRY CONDITIONS -INTEN = real intensity of the image of an output */
/*                   primitive.  Range 0.-1.  Default: device dependent. */
/* CALLS            - */
/* EXIT CONDITIONS  -VECTOR(3) = real updated intensity (INTEN). */
/* NARRATIVE        -Set the intensity value indicated for future */
/*                   primitives.  Intensity is a real value between 0 */
/*                   (not visible) and 1 (maximum).  Intensities are */
/*                   monotonically increasing within this range. */
/*                   All devices must support at least a single value: */
/*                   1.0.  If an unsupported value is specified, set to */
/*                   the closest supported intensity. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* CHECK FOR VALID INTEN. */
    if (*inten < (float)0. || *inten > (float)1.) {
	vberrh_(&c__401, &c__5);
	goto L999;
    }
/* ONLY THE SINGLE INTENSITY 1.0 (MAXIMUM) IS SUPPORTED, */
/* SO NO ACTION IS NECESSARY. */
L999:
    return 0;
} /* vdstin_ */

/* Subroutine */ int viterm_()
{
    /* System generated locals */
    address a__1[2];
    integer i__1, i__2[2];
    char ch__1[19];

    /* Builtin functions */
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi();
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen), s_cat(char *,
	     char **, integer *, integer *, ftnlen);

    /* Local variables */
    static char kpage[10];
    extern /* Subroutine */ int vdmoni_(integer *), pstcfs_(integer *, 
	    integer *), pstbuf_(integer *, char *, ftnlen), vinwpg_();

    /* Fortran I/O blocks */
    static icilist io___46 = { 1, kpage, 0, "(I10)", 10, 1 };


/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VITERM           -TERMINATE. */
/* D.L. CAMPBELL    -1-DEC-1986 */
/* J.P. LONG        -9-NOV-1987 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Terminate graphics device.  Close output file. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* COMPUTER DEPENDENT COMMON VARIABLES AND CONSTANTS. */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/*     mopoly controls polygon fill =0, on ;  =1, off */
/*     mocolr controls color =0, on ;  =1, off */
/*  put out the last page and restore postscript environment so */
/*                            nothing is left on the stack */
    vinwpg_();
    pstbuf_(&c__2, "r ", (ftnlen)2);
/* FLUSH BUFFER */
    pstbuf_(&c__0, " ", (ftnlen)1);
/*     write end of data message */
    i__1 = s_wsfi(&io___46);
    if (i__1 != 0) {
	goto L345;
    }
    i__1 = do_fio(&c__1, (char *)&vcpage_1.totpag, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L345;
    }
    i__1 = e_wsfi();
    if (i__1 != 0) {
	goto L345;
    }
    goto L349;
L345:
    s_copy(kpage, " ???", (ftnlen)10, (ftnlen)4);
L349:
    pstbuf_(&c__9, "%%Trailer", (ftnlen)9);
    pstbuf_(&c__0, " ", (ftnlen)1);
    if (vcpstb_1.mocolr == 0) {
	pstbuf_(&c__3, "end", (ftnlen)3);
	pstbuf_(&c__0, " ", (ftnlen)1);
    }
/* Writing concatenation */
    i__2[0] = 9, a__1[0] = "%%Pages: ";
    i__2[1] = 10, a__1[1] = kpage;
    s_cat(ch__1, a__1, i__2, &c__2, (ftnlen)19);
    pstbuf_(&c__19, ch__1, (ftnlen)19);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__5, "%%EOF", (ftnlen)5);
    pstbuf_(&c__0, " ", (ftnlen)1);
/* CLOSE OUTPUT FILE */
    pstcfs_(&vcpstc_1.koutfl, &c__1);
    vdmoni_(&c__2);
    return 0;
} /* viterm_ */

/* Subroutine */ int vimova_(real *x, real *y)
{
    extern /* Subroutine */ int vbvect_(integer *, real *, real *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VIMOVA           -Move Absolute. */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS -X,Y = real NDC position. */
/* CALLS            - */
/* EXIT CONDITIONS  -XCP,YCP = real updated current position. (X,Y) */
/* NARRATIVE        -Set current position to absolute NDC position X,Y. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* move */
    vbvect_(&c__0, x, y);
    return 0;
} /* vimova_ */

/* Subroutine */ int vipnta_(real *x, real *y)
{
    extern /* Subroutine */ int vilina_(real *, real *), vimova_(real *, real 
	    *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VIPNTA           -Point Absolute. */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS -X,Y = real NDC position. */
/* CALLS            -VIMOVA,VILINA */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Set current position to absolute NDC position X,Y */
/*                   and put a visible point there.  Attributes */
/*                   foreground color and intensity apply. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
    vimova_(x, y);
    vilina_(x, y);
    return 0;
} /* vipnta_ */

/* Subroutine */ int vipoly_(real *xarray, real *yarray, integer *npts)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer i_nint(real *);

    /* Local variables */
    static integer i__;
    static real wsave;
    extern /* Subroutine */ int vilina_(real *, real *), vberrh_(integer *, 
	    integer *), vdstfc_(integer *), vdmova_(real *, real *), vimova_(
	    real *, real *), pstbuf_(integer *, char *, ftnlen), vdstlw_(real 
	    *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VIPOLY           -POLYGON FILL ROUTINE */
/* D.L. CAMPBELL    -1-DEC-1986 */
/* J.P. LONG        -9-NOV-1987 */
/* ENVIRONMENT      -Fortran77, QMS */
/* ENTRY CONDITIONS -XARRAY-ARRAY OF X VALUES OF THE POLYGON */
/*                   YARRAY-CORRESPONDING ARRAY OF Y VALUES */
/*                   NPTS- NUMBER OF POINTS IN (XARRAY,YARRAY) */
/* CALLS            - */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -The polygon defined by XARRAY,YARRAY will be drawn */
/*                   and filled (constrained by any limitations of the */
/*                   physical device -- see below).  No checking will be */
/*                   done -- all points will be passed to the device. */
/*                   Current foreground color is used and the polygon */
/*                   boundary is drawn using the solid line style. */
/*                   VDI will close the polygon (i.e. the last point */
/*                   will be connected to the first). */
/*                   The level of support for this primitive is device- */
/*                   dependent.  The level of support is categorized */
/*                   as follows: */
/*                     Level 0 -- no polygon fill.  Only the polygon */
/*                        boundary is drawn. */
/*                     Level 1 -- the device fills convex polygons. */
/*                     Level 2 -- the device fills simple polygons (may */
/*                        be concave but not self-crossing) */
/*                     Level 3 -- full support for complex polygons (may */
/*                        be self-crossing). In general, the interior of */
/*                        a complex polygon is defined by the set of points */
/*                        such that, for each point, when an imaginary line */
/*                        is drawn to that point from a point far outside */
/*                        the polygon, that line intersects the polygon */
/*                        boundary an odd number of times. */
/*                   Note that the level of support for a particular device */
/*                   can be inquired using the function VDIQDC. */
/* ******************************************************************************* */
/*                   The level for this device is level 2. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* ESCAPE FLAGS */
/* PATNO AND BORDER USED BY VIPOLY FOR PATTERN FILL AND BORDER ON/OFF. DEFAULT */
/* COMPLETE FILL AND BORDER ON */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/*     mopoly controls polygon fill =0, on ;  =1, off */
/*     mocolr controls color =0, on ;  =1, off */
/* CHECK FOR VALID N */
    /* Parameter adjustments */
    --yarray;
    --xarray;

    /* Function Body */
    if (*npts < 1 || *npts > 1490) {
	vberrh_(&c__802, &c__5);
	goto L999;
    }
/* IF A SET OF VECTORS WAS IN PROCESS, ISSUE STROKE COMMAND TO DRAW THEM */
/* Start a new path. */
    if (vcvec1_1.ivect != 0) {
	pstbuf_(&c__2, "s ", (ftnlen)2);
	vcvec1_1.ivect = 0;
    }
    pstbuf_(&c__2, "r ", (ftnlen)2);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__4, "v o ", (ftnlen)4);
    wsave = vcattr_1.vector[4];
    pstbuf_(&c__4, "0 w ", (ftnlen)4);
/*     CALL VDSTLW(0.) */
    if (vcpstb_1.mocolr == 0) {
	i__1 = i_nint(vcattr_1.vector);
	vdstfc_(&i__1);
    }
    pstbuf_(&c__0, " ", (ftnlen)1);
/* DRAW POLYGON VECTORS */
/* MOVE TO FIRST POINT */
    vimova_(&xarray[1], &yarray[1]);
/* CALL VDLINA TO DRAW POINTS FROM 1ST POINT TO NTH POINT */
    i__1 = *npts;
    for (i__ = 2; i__ <= i__1; ++i__) {
	vilina_(&xarray[i__], &yarray[i__]);
/* L100: */
    }
/* THEN DRAW A LINE TO THE FIRST POINT TO CLOSE THE POLYGON */
    vilina_(&xarray[1], &yarray[1]);
/* CLOSE THE POLYGON, GRAPHICS SAVE, FILL IT, GRAPHICS RESTORE, STROKE */
/*    TO PROVIDE THE SAME FILLED AREA AS IF IT WERE FILLED WITH VECTORS */
/*    THEN RESTORE AND SAVE POSTSCRIPT ENVIRONMENT TO AVOID INPUT BUFFER OVERFLOW */
    pstbuf_(&c__12, "c d f u s r ", (ftnlen)12);
    vcvec1_1.ivect = 0;
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__4, "v o ", (ftnlen)4);
    vdstlw_(&wsave);
/* ... if color is on (mocolr = 0), then font is set in vdstfc */
    if (vcpstb_1.mocolr == 0) {
	i__1 = i_nint(vcattr_1.vector);
	vdstfc_(&i__1);
    } else {
/*         CALL VDSTCS(VECTOR(6)) */
    }
    pstbuf_(&c__0, " ", (ftnlen)1);
/* INIT THE CURRENT POSITION WITHIN POSTSCRIPT */
    vdmova_(&xarray[*npts], &yarray[*npts]);
    vcvec1_1.ivect = 0;
L999:
    return 0;
} /* vipoly_ */

/* Subroutine */ int vinwpg_()
{
    /* Initialized data */

    static integer npage = 0;

    /* System generated locals */
    address a__1[4];
    integer i__1, i__2[4];
    char ch__1[31];

    /* Builtin functions */
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi();
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen), s_cat(char *,
	     char **, integer *, integer *, ftnlen);
    integer i_nint(real *);

    /* Local variables */
    static char kpage[10];
    extern /* Subroutine */ int pstbbg_(), vdstfc_(integer *), vdmoni_(
	    integer *), pstbuf_(integer *, char *, ftnlen), vdstlw_(real *);

    /* Fortran I/O blocks */
    static icilist io___51 = { 1, kpage, 0, "(I10)", 10, 1 };


/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VINWPG           -New Page. */
/* D.L. CAMPBELL    -1-DEC-1986 */
/* J.P. LONG        -9-NOV-1987 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Physically advance the medium or clear the screen, */
/*                   whichever is appropriate.  Also flood the screen */
/*                   with the background color on devices that support */
/*                   this.  The current position is not changed. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/*     vcpstd variables control what to do with empty frames with */
/*     command is received to dump data to output */
/*        kempty=0,  frame is void - do not draw */
/*              >0,  frame has data - draw it */
/*     pstmlw controls minimum line width */
/*     kpstbg controls background coloring */
/*            = 0,  not colored (white ground from paper) */
/*            = 1,  colored black */
/*     kpstci controls black-white interchange (colors 0 & 7 only) */
/*            = 0,  no interchange */
/*            = 1,  colors interchanged */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/*        check for void page draw request */
/*        if nothing is on page, skip request */
    ++npage;
    vcpage_1.totpag = npage;
    i__1 = s_wsfi(&io___51);
    if (i__1 != 0) {
	goto L345;
    }
    i__1 = do_fio(&c__1, (char *)&npage, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L345;
    }
    i__1 = e_wsfi();
    if (i__1 != 0) {
	goto L345;
    }
    goto L349;
L345:
    s_copy(kpage, " ???", (ftnlen)10, (ftnlen)4);
L349:
    if (vcpstd_1.kempty == 0) {
	goto L350;
    }
/*  stroke the path in case there are any vectors and show text */
    pstbuf_(&c__2, "s ", (ftnlen)2);
    vcvec1_1.ivect = 0;
/*   showpage and restore postscript environment to avoid buffer overflow */
/*            flush buffer because save and restore won't work back-to-back */
    pstbuf_(&c__4, "p r ", (ftnlen)4);
    pstbuf_(&c__0, " ", (ftnlen)1);
/*     comment frame number in output file */
/* Writing concatenation */
    i__2[0] = 9, a__1[0] = "%%Page: \"";
    i__2[1] = 10, a__1[1] = kpage;
    i__2[2] = 2, a__1[2] = "\" ";
    i__2[3] = 10, a__1[3] = kpage;
    s_cat(ch__1, a__1, i__2, &c__4, (ftnlen)31);
    pstbuf_(&c__31, ch__1, (ftnlen)31);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__28, "%%PageOrientation: Landscape", (ftnlen)28);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__32, "%%PageBoundingBox: 36 30 574 750", (ftnlen)32);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__4, "v o ", (ftnlen)4);
    vdmoni_(&c__1);
/*     shade background if appropriate */
    if (vcpsta_1.kpstbg != 0) {
	pstbbg_();
    }
    goto L370;
/*     void frame -- First Page */
L350:
    pstbuf_(&c__2, "r ", (ftnlen)2);
    pstbuf_(&c__0, " ", (ftnlen)1);
/* Writing concatenation */
    i__2[0] = 9, a__1[0] = "%%Page: \"";
    i__2[1] = 10, a__1[1] = kpage;
    i__2[2] = 2, a__1[2] = "\" ";
    i__2[3] = 10, a__1[3] = kpage;
    s_cat(ch__1, a__1, i__2, &c__4, (ftnlen)31);
    pstbuf_(&c__31, ch__1, (ftnlen)31);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__28, "%%PageOrientation: Landscape", (ftnlen)28);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__32, "%%PageBoundingBox: 36 30 574 750", (ftnlen)32);
    pstbuf_(&c__0, " ", (ftnlen)1);
    pstbuf_(&c__4, "v o ", (ftnlen)4);
L370:
    vdstlw_(&vcattr_1.vector[4]);
/*      CALL VDSTCS(VECTOR(6)) */
    i__1 = i_nint(vcattr_1.vector);
    vdstfc_(&i__1);
    pstbuf_(&c__0, " ", (ftnlen)1);
    vcpstd_1.kempty = 0;
    return 0;
} /* vinwpg_ */

/* Subroutine */ int vdescp_(integer *escpcd, integer *n, real *args)
{
    extern /* Subroutine */ int vberrh_(integer *, integer *), pstsel_(char *,
	     ftnlen);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDESCP           -Escape Code Routine. */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS -ESCPCD = integer escape function code. */
/*                   N = integer number of arguments in ARG.  RANGE >=0. */
/*                   ARGS = real array of arguments for the escape */
/*                   function specified. */
/* CALLS            - */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -Invoke the nonstandard, device-dependent function */
/*                   ESCPCD.  N is the number of arguments used by this */
/*                   function and ARGS is a real array containing those */
/*                   arguments.  Unsupported values of ESCPCD are */
/*                   ignored, not causing an error. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* COMPUTER DEPENDENT COMMON VARIABLES AND CONSTANTS. */
/* SCALE FACTORS FOR NDC TO DC MAPPING. (LXY,HC1) */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* DC DIMENSIONS OF OFFSETS AND PICTURE. (LXY,HC1) */
/* USED BY VIPOLY FOR PATTERN FILL AND BORDER ON/OFF. DEFAULT COMPLETE FILL */
/* AND BORDER ON. PLC. */
/* CHECK FOR VALID N. */
    /* Parameter adjustments */
    --args;

    /* Function Body */
    if (*n < 0) {
	vberrh_(&c__802, &c__5);
	goto L999;
    }
/* 2100 - PAGE FORMAT (0=LANDSCAPE,1=PORTRAIT) */
    if (*escpcd == 2100) {
	if (args[1] == (float)0.) {
	    vcescp_1.pgform = 0;
	} else {
	    vcescp_1.pgform = 1;
	}
/*     set output format */
    } else if (*escpcd == 2101) {
	pstsel_("1", (ftnlen)1);
    } else if (*escpcd == 2102) {
	pstsel_("2", (ftnlen)1);
    } else if (*escpcd == 2103) {
	pstsel_("3", (ftnlen)1);
    } else if (*escpcd == 2104) {
	pstsel_("4", (ftnlen)1);
    } else if (*escpcd == 2105) {
	pstsel_("5", (ftnlen)1);
    } else if (*escpcd == 2106) {
	pstsel_("6", (ftnlen)1);
    } else if (*escpcd == 2107) {
	pstsel_("7", (ftnlen)1);
    } else if (*escpcd == 2108) {
	pstsel_("8", (ftnlen)1);
    } else if (*escpcd == 2109) {
	pstsel_("9", (ftnlen)1);
    } else if (*escpcd == 2110) {
	pstsel_("10", (ftnlen)2);
    }
L999:
    return 0;
} /* vdescp_ */

/* Subroutine */ int vilina_0_(int n__, real *x, real *y)
{
    extern /* Subroutine */ int vbvect_(integer *, real *, real *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VILINA */
/* D.L. CAMPBELL    -1-DEC-1986 */
/* J.P. LONG        -9-NOV-1987 */
/* ENVIRONMENT      -DEVICE DEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS -X,Y REAL NDC COORDINATES */
/* CALLS            - */
/* EXIT CONDITIONS  -CURRENT POSITION IS SET */
/* NARRATIVE */
/*                   LINE-DRAW A LINE FROM CP TO ABSOLUTE NDC POSITION X,Y */
/*                        AND UPDATE CP . ATTRIBUTES COLOR,INTEN,LINSTY AND */
/*                        LINWTH APPLY. */
/*        OTHER VARIABLES: */
/*                    XCP,YCP-NDC COORDINATES */
/* *************************************************************************** */
/*     vcpstd variables control what to do with empty frames with */
/*     command is received to dump data to output */
/*        kempty=0,  frame is void - do not draw */
/*              >0,  frame has data - draw it */
/* draw */
    switch(n__) {
	case 1: goto L_vblina;
	}


L_vblina:
    vbvect_(&c__1, x, y);
    vcpstd_1.kempty = 1;
    return 0;
} /* vilina_ */

/* Subroutine */ int vilina_(real *x, real *y)
{
    return vilina_0_(0, x, y);
    }

/* Subroutine */ int vblina_(real *x, real *y)
{
    return vilina_0_(1, x, y);
    }

/* Subroutine */ int vbvect_(integer *ipen, real *x, real *y)
{
    /* System generated locals */
    address a__1[7], a__2[2];
    integer i__1[7], i__2[2], i__3;

    /* Builtin functions */
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen),
	     s_copy(char *, char *, ftnlen, ftnlen);
    integer i_nint(real *);

    /* Local variables */
    static integer ixdc, iydc;
    static real xoff, yoff;
    static char ctemp[20];
    extern /* Subroutine */ int psti2c_(integer *, integer *, char *, ftnlen),
	     vdstfc_(integer *);
    static char xcoord[4], ycoord[4];
    extern /* Subroutine */ int pstbuf_(integer *, char *, ftnlen), vdstlw_(
	    real *);

/* **************************************************** */
/* vbvect - do move or draw to x,y (depending on ipen) */
/*     ipen = 0 for move, 1 for draw */
/*     x,y = NDC coordinates to be moved/drawn to */
/* ****************************************************** */
/*     mopoly controls polygon fill =0, on ;  =1, off */
/*     mocolr controls color =0, on ;  =1, off */
/* SCALE FACTORS FOR NDC TO DC MAPPING. (LXY,HC1) */
/* DC DIMENSIONS OF OFFSETS AND PICTURE. (LXY,HC1) */
/* CURRENT POSITION. */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* compute new point in dev. coord. */
/*     convert to floating offsets */
    xoff = (real) vcddim_1.xpad;
    yoff = (real) vcddim_1.ypad;
    ixdc = *x * vcscal_1.xscale + xoff;
    iydc = *y * vcscal_1.yscale + yoff;
/*        write(xcoord,'(i5)')ixdc */
/*        write(ycoord,'(i5)')iydc */
/*                                ...include both x,y */
    psti2c_(&ixdc, &c__4, xcoord, (ftnlen)4);
    psti2c_(&iydc, &c__4, ycoord, (ftnlen)4);
/* Writing concatenation */
    i__1[0] = 3, a__1[0] = xcoord;
    i__1[1] = 1, a__1[1] = ".";
    i__1[2] = 1, a__1[2] = xcoord + 3;
    i__1[3] = 1, a__1[3] = " ";
    i__1[4] = 3, a__1[4] = ycoord;
    i__1[5] = 1, a__1[5] = ".";
    i__1[6] = 1, a__1[6] = ycoord + 3;
    s_cat(vcvec2_1.coord, a__1, i__1, &c__7, (ftnlen)20);
/* pack up move/draw command, send it down */
/*      if (lstcrd(1:11) .ne. coord(1:11)) then */
    if (*ipen == 0) {
/* Writing concatenation */
	i__2[0] = 11, a__2[0] = vcvec2_1.coord;
	i__2[1] = 3, a__2[1] = " m ";
	s_cat(ctemp, a__2, i__2, &c__2, (ftnlen)20);
    } else {
/* Writing concatenation */
	i__2[0] = 11, a__2[0] = vcvec2_1.coord;
	i__2[1] = 3, a__2[1] = " l ";
	s_cat(ctemp, a__2, i__2, &c__2, (ftnlen)20);
    }
    pstbuf_(&c__14, ctemp, (ftnlen)20);
/*     ...count the coordinate pair */
    ++vcvec1_1.ivect;
/*      end if */
    s_copy(vcvec2_1.lstcrd, vcvec2_1.coord, (ftnlen)11, (ftnlen)11);
/*  stroke the path if we are approaching the 1500-coord pair limit */
/*                also restore and save postscript environment to avoid */
/*                input buffer overflow (must have a c/r between restore */
/*                and save) */
    if (vcvec1_1.ivect > 1400) {
	s_copy(vcvec2_1.lstcrd, "           ", (ftnlen)11, (ftnlen)11);
	pstbuf_(&c__4, "s r ", (ftnlen)4);
	pstbuf_(&c__0, " ", (ftnlen)1);
	pstbuf_(&c__4, "v o ", (ftnlen)4);
	vdstlw_(&vcattr_1.vector[4]);
	if (vcpstb_1.mocolr == 0) {
	    i__3 = i_nint(vcattr_1.vector);
	    vdstfc_(&i__3);
	}
/*        ...reset the vector count - vdstls (called by vdstlw) */
/*                 reinitted the current posn */
	vcvec1_1.ivect = 1;
    }
/* UPDATE CURRENT POSITION */
    vccrps_1.xcp = *x;
    vccrps_1.ycp = *y;
    return 0;
} /* vbvect_ */

/* Subroutine */ int vitext_(integer *lengt1, integer *chars)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__;
    static real dx, dy;
    static char str[3], ctemp[150];
    extern /* Subroutine */ int psta2c_(integer *, char *, ftnlen);
    static integer length;
    extern /* Subroutine */ int vberrh_(integer *, integer *), vbvect_(
	    integer *, real *, real *), pstbuf_(integer *, char *, ftnlen), 
	    vdstcs_(real *);
    static integer lenout;

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VITEXT           - Text from Array. */
/* P. Watterberg    - 24 MAR 81 */
/* J. P. LONG       -  3 DEC 87 */
/* ENVIRONMENT      - COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS - LENGT1 = integer number of characters in CHARS. */
/*                    Range 1-136. */
/*                    CHARS = integer array of ASCII characters, one */
/*                    character per array element, right justified. */
/*                    Range 8,10,32-126. */
/* CALLS            - vbout */
/* EXIT CONDITIONS  - XCP,YCP = integer updated current position (at the end */
/*                    of the string). */
/* NARRATIVE        - Draw LENGT1 characters in CHARS, starting at */
/*                    current position and update current position to */
/*                    the point after the last character box where the */
/*                    next character would begin.  Current position */
/*                    indicates the lower left corner of the first */
/*                    character box.  Only printable characters (32-126 */
/*                    decimal) and backspace and linefeed are allowed. */
/*                    All values in this range must produce "reasonable" */
/*                    output; mapping lower case to upper case letters is */
/*                    considered reasonable.  Attributes foreground color, */
/*                    background color, intensity, and character size */
/*                    apply. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* CURRENT POSITION. */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* SCALE FACTORS FOR NDC TO DC MAPPING. (LXY,HC1) */
/* DC DIMENSIONS OF OFFSETS AND PICTURE. (LXY,HC1) */
/*     vcpstd variables control what to do with empty frames with */
/*     command is received to dump data to output */
/*        kempty=0,  frame is void - do not draw */
/*              >0,  frame has data - draw it */
/*          check for valid length. */
    /* Parameter adjustments */
    --chars;

    /* Function Body */
    vdstcs_(&vcattr_1.vector[5]);
    vcpstd_1.kempty = 1;
    length = *lengt1;
    if (length < 1) {
	vberrh_(&c__212, &c__5);
	goto L999;
    }
/*          if(length.gt.136) then call vberrh(213,5), and use the */
/*          maximum length of 136. */
    if (length > 136) {
	vberrh_(&c__213, &c__5);
	length = 136;
    }
    s_copy(ctemp, "(", (ftnlen)150, (ftnlen)1);
    lenout = 1;
/*          loop through length characters. */
    i__1 = length;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*          check for valid chars. */
/*          ignore control characters, except for: */
/*          8 is backspace */
/*          10 is linefeed */
/*          13 is carriage return */
	if (chars[i__] < 32 || chars[i__] > 126) {
	    if (chars[i__] == 8) {
		dx = -vcattr_1.vector[6];
		dy = (float)0.;
	    } else if (chars[i__] == 10) {
		dx = (float)0.;
		dy = -vcattr_1.vector[5];
	    } else if (chars[i__] == 13) {
		dx = vcddim_1.xpad - vccrps_1.xcp;
		dy = (float)0.;
	    } else {
		dx = (float)0.;
		dy = (float)0.;
		vberrh_(&c__208, &c__5);
		goto L100;
	    }
/*           finish the string, emulate the control char, and start a new one */
/*           send the buffered chars to the printer if there are any */
	    if (lenout != 1) {
		i__2 = lenout;
		s_copy(ctemp + i__2, ") t ", 150 - i__2, (ftnlen)4);
		lenout += 4;
		pstbuf_(&lenout, ctemp, (ftnlen)150);
/*              reset the cp from the characters */
		vccrps_1.xcp += (lenout - 5) * vcattr_1.vector[6];
	    }
/*           calculate the new current position after the control char */
	    vccrps_1.xcp += dx;
	    vccrps_1.ycp += dy;
	    vbvect_(&c__0, &vccrps_1.xcp, &vccrps_1.ycp);
/*           start a new string */
	    s_copy(ctemp, "(", (ftnlen)150, (ftnlen)1);
	    lenout = 1;
	} else {
/*           Char value is 32-126 inclusive.  Put \ before these: */
/*              92 is \ */
/*              40 is ( */
/*              41 is ) */
	    if (chars[i__] == 40 || chars[i__] == 41 || chars[i__] == 92) {
		i__2 = lenout;
		s_copy(ctemp + i__2, "\\", 150 - i__2, (ftnlen)1);
		++lenout;
	    }
/*           now pack the chars into the buffer */
	    psta2c_(&chars[i__], str, (ftnlen)3);
	    i__2 = lenout;
	    s_copy(ctemp + i__2, str, 150 - i__2, (ftnlen)1);
	    ++lenout;
	}
L100:
	;
    }
/*          send the chars to the printer */
    i__1 = lenout;
    s_copy(ctemp + i__1, ") t ", 150 - i__1, (ftnlen)4);
    lenout += 4;
    pstbuf_(&lenout, ctemp, (ftnlen)150);
/*          reset the cp from the characters */
    vccrps_1.xcp += (lenout - 5) * vcattr_1.vector[6];
L999:
    return 0;
} /* vitext_ */

/* Subroutine */ int vdstls_0_(int n__, integer *linsty)
{
    /* System generated locals */
    address a__1[5], a__2[9], a__3[2];
    integer i__1[5], i__2[9], i__3[2];
    real r__1;

    /* Builtin functions */
    integer i_nint(real *);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static real lw;
    static integer ill, jll, igap;
    static char strg[3], strl[3], strs[3], ctemp[30];
    extern /* Subroutine */ int psti2c_(integer *, integer *, char *, ftnlen),
	     vberrh_(integer *, integer *), pstbuf_(integer *, char *, ftnlen)
	    ;

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDSTLS           -Set Line Style. */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS -LINSTY = integer linestyle of line drawing output */
/*                   primitives.  Range 0-5.  Default:0. */
/* CALLS            - */
/* EXIT CONDITIONS  -VECTOR(4) = real updated line style (LINSTY). */
/* NARRATIVE        -Set the style of line as below.  This applies only */
/*                   to line drawing primitives.  The line styles are: */
/*                          0 - solid */
/*                          1 - dotted */
/*                          2 - dot dash */
/*                          3 - short dash */
/*                          4 - long dash */
/*                          5 - medium dash */
/*                   All devices must support at least the values 0 and */
/*                   5.  If an unsupported value is specified, set to 5. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* SCALE FACTORS FOR NDC TO DC MAPPING. (LXY,HC1) */
/* CURRENT POSITION. */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*      REAL VECTOR(7) */
/*     COMMON /VCATTR/ VECTOR */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* CHECK FOR VALID LINSTY. */
    switch(n__) {
	case 1: goto L_vbstls;
	}


L_vbstls:
    if (*linsty < 0 || *linsty > 5) {
	vberrh_(&c__401, &c__5);
	vcattr_1.vector[3] = (float)0.;
	goto L999;
    }
    if (vcvec1_1.ivect != 0) {
	pstbuf_(&c__2, "s ", (ftnlen)2);
	vcvec1_1.ivect = 0;
    }
/* GENERATE THE LINESTYLE COMMANDS */
    if (*linsty == 0) {
	pstbuf_(&c__7, "[] 0 h ", (ftnlen)7);
    }
/* calculate the linewidth -- it's needed below in every case */
/*        actual xscale is xscale*.1; linewidth=1 => .01 in NDC */
    lw = vcattr_1.vector[4];
    lw = vcscal_1.xscale * vcattr_1.vector[4] * (float).001;
/*        a linewidth of zero isn't good with postscript */
    if (lw < (float)1.) {
	lw = (float)1.;
    }
/*     from here on, set up patterns that depend on the linewidth and */
/*          the extra length added to the line segment */
/*          by the hemispherical end cap */
    if (*linsty == 1) {
	r__1 = lw * (float).5;
	ill = i_nint(&r__1);
	r__1 = lw * (float)3.;
	igap = i_nint(&r__1);
	psti2c_(&ill, &c__3, strl, (ftnlen)3);
	psti2c_(&igap, &c__3, strg, (ftnlen)3);
/* Writing concatenation */
	i__1[0] = 1, a__1[0] = "[";
	i__1[1] = 3, a__1[1] = strl;
	i__1[2] = 1, a__1[2] = " ";
	i__1[3] = 3, a__1[3] = strg;
	i__1[4] = 6, a__1[4] = "] 0 h ";
	s_cat(ctemp, a__1, i__1, &c__5, (ftnlen)30);
	pstbuf_(&c__14, ctemp, (ftnlen)30);
    } else if (*linsty == 2) {
	r__1 = lw * (float)18.;
	ill = i_nint(&r__1);
	r__1 = lw * (float)1.5;
	jll = i_nint(&r__1);
	r__1 = lw * (float)3.;
	igap = i_nint(&r__1);
	psti2c_(&ill, &c__3, strl, (ftnlen)3);
	psti2c_(&jll, &c__3, strs, (ftnlen)3);
	psti2c_(&igap, &c__3, strg, (ftnlen)3);
/* Writing concatenation */
	i__2[0] = 1, a__2[0] = "[";
	i__2[1] = 3, a__2[1] = strs;
	i__2[2] = 1, a__2[2] = " ";
	i__2[3] = 3, a__2[3] = strg;
	i__2[4] = 1, a__2[4] = " ";
	i__2[5] = 3, a__2[5] = strl;
	i__2[6] = 1, a__2[6] = " ";
	i__2[7] = 3, a__2[7] = strg;
	i__2[8] = 6, a__2[8] = "] 0 h ";
	s_cat(ctemp, a__2, i__2, &c__9, (ftnlen)30);
	pstbuf_(&c__22, ctemp, (ftnlen)30);
/*         call pstbuf(14,'[2 2 6 2] 0 h ') */
    } else if (*linsty == 3) {
	r__1 = lw * (float)6.;
	ill = i_nint(&r__1);
	r__1 = lw * (float)7.;
	igap = i_nint(&r__1);
	psti2c_(&ill, &c__3, strl, (ftnlen)3);
	psti2c_(&igap, &c__3, strg, (ftnlen)3);
/* Writing concatenation */
	i__1[0] = 1, a__1[0] = "[";
	i__1[1] = 3, a__1[1] = strl;
	i__1[2] = 1, a__1[2] = " ";
	i__1[3] = 3, a__1[3] = strg;
	i__1[4] = 6, a__1[4] = "] 0 h ";
	s_cat(ctemp, a__1, i__1, &c__5, (ftnlen)30);
	pstbuf_(&c__14, ctemp, (ftnlen)30);
/*         call pstbuf(8,'[4] 0 h ') */
    } else if (*linsty == 4) {
	r__1 = lw * (float)24.;
	ill = i_nint(&r__1);
	r__1 = lw * (float)18.;
	igap = i_nint(&r__1);
	psti2c_(&ill, &c__3, strl, (ftnlen)3);
	psti2c_(&igap, &c__3, strg, (ftnlen)3);
/* Writing concatenation */
	i__1[0] = 1, a__1[0] = "[";
	i__1[1] = 3, a__1[1] = strl;
	i__1[2] = 1, a__1[2] = " ";
	i__1[3] = 3, a__1[3] = strg;
	i__1[4] = 6, a__1[4] = "] 0 h ";
	s_cat(ctemp, a__1, i__1, &c__5, (ftnlen)30);
	pstbuf_(&c__14, ctemp, (ftnlen)30);
/*         call pstbuf(8,'[8] 0 h ') */
    } else if (*linsty == 5) {
	r__1 = lw * (float)12.;
	ill = i_nint(&r__1);
	r__1 = lw * (float)10.;
	igap = i_nint(&r__1);
	psti2c_(&ill, &c__3, strl, (ftnlen)3);
	psti2c_(&igap, &c__3, strg, (ftnlen)3);
/* Writing concatenation */
	i__1[0] = 1, a__1[0] = "[";
	i__1[1] = 3, a__1[1] = strl;
	i__1[2] = 1, a__1[2] = " ";
	i__1[3] = 3, a__1[3] = strg;
	i__1[4] = 6, a__1[4] = "] 0 h ";
	s_cat(ctemp, a__1, i__1, &c__5, (ftnlen)30);
	pstbuf_(&c__14, ctemp, (ftnlen)30);
    }
/*     redefine the postscript current position */
/*     the code below is equivalent to */
/*      call vbvect(0,xcp,ycp) */
/*     but can't do it because vbvect calls vdstlw which calls this routine */
/* Writing concatenation */
    i__3[0] = 11, a__3[0] = vcvec2_1.coord;
    i__3[1] = 3, a__3[1] = " m ";
    s_cat(ctemp, a__3, i__3, &c__2, (ftnlen)30);
    pstbuf_(&c__14, ctemp, (ftnlen)30);
    vcattr_1.vector[3] = (real) (*linsty);
L999:
    return 0;
} /* vdstls_ */

/* Subroutine */ int vdstls_(integer *linsty)
{
    return vdstls_0_(0, linsty);
    }

/* Subroutine */ int vbstls_(integer *linsty)
{
    return vdstls_0_(1, linsty);
    }

/* Subroutine */ int vdstcs_(real *ysize)
{
    /* System generated locals */
    address a__1[3];
    integer i__1[3];
    real r__1;

    /* Builtin functions */
    integer i_nint(real *);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static char str[4], ctemp[10];
    extern /* Subroutine */ int psti2c_(integer *, integer *, char *, ftnlen);
    static real ysize1;
    extern /* Subroutine */ int vberrh_(integer *, integer *), pstbuf_(
	    integer *, char *, ftnlen);
    static integer iysize;

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDSTCS           -Set Character Size. */
/* R.W.Simons       -05DEC80 */
/* J. P. LONG       -03 DEC 87 */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/*                   All Devices that support only software characters. */
/*                   (LXY, HC1) */
/* ENTRY CONDITIONS -YSIZE = real Y dimension of the character box in NDC */
/*                   space.  Range 0.-1.  Default: device dependent, */
/*                   typically the smallest hardware size. */
/* CALLS            - */
/* EXIT CONDITIONS  -VECTOR(6) = real updated character box Y (YSIZE). */
/*                   VECTOR(7) = real updated character box X. */
/* NARRATIVE        -Set the character size for text primitives.  Size */
/*                   is given by YSIZE as the Y dimension of the */
/*                   character box.  The SVDI will assign the X dimension */
/*                   of the character box and X and Y character size */
/*                   within the box according to the font used.  Applies */
/*                   only to text primitives. */
/*                   All devices must support at least a single device */
/*                   dependent value that is the default.  If an */
/*                   unsupported value is specified, set to the largest */
/*                   supported character size that does not exceed the */
/*                   specified size. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* SCALE FACTORS FOR NDC TO DC MAPPING. (LXY,HC1) */
/* CHECK FOR VALID YSIZE. */
    if (*ysize < (float)0. || *ysize > (float)1.) {
	vberrh_(&c__401, &c__5);
	goto L999;
    }
/* PROTECT INPUT PARAMETER FROM BEING CHANGED. */
    ysize1 = *ysize;
/* DON'T ALLOW VALUES BELOW THE MINIMUM "HARDWARE" SIZE. */
    if (ysize1 < (float).01) {
	ysize1 = (float).01;
    }
/* VALUES ESTABLISHED HERE ARE USED BY VBSIM IN SIMULATING CHARACTERS. */
/* ALWAYS USE A CHARACTER ASPECT RATIO OF 5/7. */
    vcattr_1.vector[5] = ysize1;
    vcattr_1.vector[6] = ysize1 * (float)5. / (float)7.;
/* convert the character size into device coords */
    r__1 = vcscal_1.xscale * ysize1;
    iysize = i_nint(&r__1);
/* output the postscript command */
    psti2c_(&iysize, &c__4, str, (ftnlen)4);
/*     iysize is in tenths of device units */
/* Writing concatenation */
    i__1[0] = 2, a__1[0] = "y ";
    i__1[1] = 3, a__1[1] = str;
    i__1[2] = 3, a__1[2] = " x ";
    s_cat(ctemp, a__1, i__1, &c__3, (ftnlen)10);
    pstbuf_(&c__8, ctemp, (ftnlen)10);
L999:
    return 0;
} /* vdstcs_ */

/* Subroutine */ int vdstlw_(real *linwth)
{
    /* System generated locals */
    address a__1[5], a__2[4];
    integer i__1[5], i__2[4];
    real r__1;

    /* Builtin functions */
    integer i_nint(real *);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static real lw;
    static integer ilw;
    static char str[5], ctemp[19];
    static real width;
    extern /* Subroutine */ int psti2c_(integer *, integer *, char *, ftnlen),
	     vberrh_(integer *, integer *), pstbuf_(integer *, char *, ftnlen)
	    , vbstls_(integer *);
    static integer linsty;

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDSTLW           -Set Line Width. */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS -LINWTH = real line width of line drawing output */
/*                   primitives.  Range 0.-1.  Default: device dependent. */
/* CALLS            - */
/* EXIT CONDITIONS  -VECTOR(5) = real updated line width (LINWTH). */
/* NARRATIVE        -Set the relative width of an output line.  Values */
/*                   are 0.-1. with 1. being .01 in NDC space. */
/*                   All devices must support at least a single device */
/*                   dependent value that is the default.  If an */
/*                   unsupported value is specified, set to the closest */
/*                   supported line width. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* COMPUTER DEPENDENT COMMON VARIABLES AND CONSTANTS. */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
/* SCALE FACTORS FOR NDC TO DC MAPPING. (LXY,HC1) */
/*     pstmlw controls minimum line width */
/*     kpstbg controls background coloring */
/*            = 0,  not colored (white ground from paper) */
/*            = 1,  colored black */
/*     kpstci controls black-white interchange (colors 0 & 7 only) */
/*            = 0,  no interchange */
/*            = 1,  colors interchanged */
/* CHECK FOR VALID LINWTH. */
    if (*linwth < (float)0. || *linwth > (float)1.) {
	vberrh_(&c__401, &c__5);
	goto L999;
    }
/*     test user define minimum */
    width = dmax(vcpsta_1.pstmlw,*linwth);
/* CONVERT LINE-WIDTH TO NDC */
    lw = width * (float).005;
/* CONVERT WIDTH TO DEVICE COORDINATES AND ADD A DIGIT; NEED IT TO HUNDREDTHS */
    r__1 = vcscal_1.xscale * lw * (float)10.;
    ilw = i_nint(&r__1);
/*     A LINEWIDTH OF ZERO WORKS ONLY PART OF THE TIME */
    if (ilw < 10) {
	ilw = 10;
    }
/* SET LINE WIDTH */
    psti2c_(&ilw, &c__5, str, (ftnlen)5);
    if (vcvec1_1.ivect != 0) {
/* Writing concatenation */
	i__1[0] = 2, a__1[0] = "s ";
	i__1[1] = 3, a__1[1] = str;
	i__1[2] = 1, a__1[2] = ".";
	i__1[3] = 2, a__1[3] = str + 3;
	i__1[4] = 3, a__1[4] = " w ";
	s_cat(ctemp, a__1, i__1, &c__5, (ftnlen)19);
	pstbuf_(&c__11, ctemp, (ftnlen)19);
	vcvec1_1.ivect = 0;
    } else {
/* Writing concatenation */
	i__2[0] = 3, a__2[0] = str;
	i__2[1] = 1, a__2[1] = ".";
	i__2[2] = 2, a__2[2] = str + 3;
	i__2[3] = 3, a__2[3] = " w ";
	s_cat(ctemp, a__2, i__2, &c__4, (ftnlen)19);
	pstbuf_(&c__9, ctemp, (ftnlen)19);
    }
    vcattr_1.vector[4] = width;
/*     since linestyle uses the linewidth in setting the pattern, call it */
    linsty = vcattr_1.vector[3];
    vbstls_(&linsty);
L999:
    return 0;
} /* vdstlw_ */

/* Subroutine */ int vdiqes_(integer *escpcd, integer *support)
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* VDIQES           -Inquire Escape. */
/* ENVIRONMENT      -COMPUTER-INDEPENDENT, SYSTEM-INDEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS -ESCPCD = integer escape function code. */
/* CALLS            - */
/* EXIT CONDITIONS  -SUPPRT = integer level of support for the escape */
/*                   function specified.  Range 0,1,2. */
/* NARRATIVE        -An integer value indicating 2=hardware supported, */
/*                   1=software supported, 0=unsupported is returned in */
/*                   SUPPORT for the escape function ESCPCD. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
    if (*escpcd == 2100) {
	*support = 2;
    } else if (*escpcd >= 2101 && *escpcd <= 2110) {
	*support = 2;
/* ELSE THERE IS NO SUPPORT OF ANY OTHER ESCAPE CODES */
    } else {
	*support = 0;
    }
    return 0;
} /* vdiqes_ */

/* Subroutine */ int pstbuf_(integer *nchrs, char *out, ftnlen out_len)
{
    /* Initialized data */

    static integer icnt = 1;
    static integer charln = 130;
    static integer lstchr = -1;
    static char lstout[132+1] = "                                           \
                                                                            \
             ";

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static char cbuf[130];
    static integer remain;

    /* Fortran I/O blocks */
    static cilist io___90 = { 0, 0, 0, "(A)", 0 };
    static cilist io___91 = { 0, 0, 0, "(A)", 0 };


/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* PSTBUF           -Output PostScript data */
/* C. D. Brown      -DEC 1986 (Adapted from QMSBUF) */
/* ENVIRONMENT      -COMPUTER/DEVICE DEPENDENT, FORTRAN 77 */
/* ENTRY CONDITIONS -NCHRS = integer number of characters in OUT. */
/*                          = 0 means flush the buffer. */
/*                   OUT = character string of input data */
/*                   KOUTFL = integer number of the graphics output file. */
/* CALLS            - */
/* EXIT CONDITIONS  - */
/* NARRATIVE        -The data in OUT is buffered for output to KOUTFL. */
/*                   The buffer is output when it is "full" or a buffer */
/*                   flush is requested by specifying NCHRS<=0. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* COMPUTER DEPENDENT COMMON VARIABLES AND CONSTANTS. */
/* **NOTE: BUFFER SIZE (IN BITS) MUST BE AN EXACT MULTIPLE OF 8 (8-BIT DATA */
/*  MUST END EXACTLY AT WORD BOUNDARY) */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/*                                        CHARLN=BUFFER SIZE IN CHARS */
/* ...Check that last output string does not match current output GDS */
    if (lstchr == *nchrs) {
	if (s_cmp(lstout, out, lstchr, (*nchrs)) == 0) {
	    return 0;
	}
    }
    lstchr = *nchrs;
    s_copy(lstout, out, (*nchrs), (*nchrs));
/* COMPUTE REMAINING AVAILABLE CHARACTERS IN BUFFER */
    remain = charln - icnt + 1;
/* CHECK FOR BUFFER FLUSH REQUEST OR NOT ENOUGH ROOM IN BUFFER. */
    if (*nchrs <= 0 || *nchrs > remain) {
/*                                        TEST IF THERE'S ANYTHING TO FLUSH. */
	if (icnt > 1) {
/*                                   PAD TO END OF RECORD AND OUTPUT THE BUFFER. */
	    if (icnt <= charln) {
		s_copy(cbuf + (icnt - 1), " ", charln - (icnt - 1), (ftnlen)1)
			;
		io___90.ciunit = vcpstc_1.koutfl;
		s_wsfe(&io___90);
		do_fio(&c__1, cbuf, icnt);
		e_wsfe();
	    } else {
		io___91.ciunit = vcpstc_1.koutfl;
		s_wsfe(&io___91);
		do_fio(&c__1, cbuf, (ftnlen)130);
		e_wsfe();
	    }
	    icnt = 1;
	}
    }
/* ADD TO BUFFER */
    if (*nchrs > 0) {
	s_copy(cbuf + (icnt - 1), out, icnt + *nchrs - 1 - (icnt - 1), (*
		nchrs));
	icnt += *nchrs;
    }
    return 0;
} /* pstbuf_ */

/* Subroutine */ int psta2c_(integer *asci, char *charac, ftnlen charac_len)
{
    /* System generated locals */
    char ch__1[1];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* PSTA2C           - CONVERT FROM ASCII TO CHARACTER */
/* P. Watterberg    - 19 Jan 1982 */
/* ENVIRONMENT      - computer dependent, system dependent, fortran 77 */
/* ENTRY CONDITIONS - ASCI is an integer representing an ascii character */
/* CALLS            - */
/* EXIT CONDITIONS  - CHARAC is the character represented by ASCI */
/* NARRATIVE        - */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
    *(unsigned char *)&ch__1[0] = *asci;
    s_copy(charac, ch__1, charac_len, (ftnlen)1);
    return 0;
} /* psta2c_ */

/* Subroutine */ int psti2c_(integer *int__, integer *ndigit, char *istr, 
	ftnlen istr_len)
{
    /* Initialized data */

    static char ka[1*10+1] = "0123456789";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer i_len(char *, ftnlen);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, j, k, nd, int1, ndig1, length;

/*     C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */

/*     PSTI2C           - convert positive integer to decimal character */
/*     string equivalent */

/*     ENVIRONMENT      - COMPUTER-INdependent */

/*     ENTRY CONDITIONS - int = positive integer to be converted */
/*     ndigit = number of digits to be produced in string */
/*     form (pad left with zeros) */
/*     istr = character string of at least ndigit characters */

/*     CALLS            - */

/*     EXIT CONDITIONS  - istr contains decimal-string equivalent of int */
/*     (ndigits left-justified in istr) */

/*     NARRATIVE        - This routine modified 10/89  S.L.Thompson */

/*     C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */

/*     check input parameters */
    int1 = max(*int__,0);
    length = i_len(istr, istr_len);
/* Computing MAX */
    i__1 = 1, i__2 = min(length,*ndigit);
    ndig1 = max(i__1,i__2);
    s_copy(istr, "00000000000000000000000000000000000000000", istr_len, (
	    ftnlen)41);
    nd = length;
    i__1 = ndig1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	j = int1 / 10;
	k = int1 - j * 10;
	*(unsigned char *)&istr[nd - 1] = *(unsigned char *)&ka[k];
	--nd;
	int1 = j;
    }
    return 0;
} /* psti2c_ */

/* Subroutine */ int pstbbg_()
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer i_nint(real *);

    /* Local variables */
    static real x[4], y[4];
    extern /* Subroutine */ int vdstfc_(integer *);
    static integer kolsav;
    extern /* Subroutine */ int vipoly_(real *, real *, integer *);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/*     Color background black for white paper device. */
/*     Should only be called from vdnwpg and viinit. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/*     pstmlw controls minimum line width */
/*     kpstbg controls background coloring */
/*            = 0,  not colored (white ground from paper) */
/*            = 1,  colored black */
/*     kpstci controls black-white interchange (colors 0 & 7 only) */
/*            = 0,  no interchange */
/*            = 1,  colors interchanged */
/*     mopoly controls polygon fill =0, on ;  =1, off */
/*     mocolr controls color =0, on ;  =1, off */
/* CURRENT ATTRIBUTE VALUES. (DEVICE-INDEPENDENT) */
/*     VECTOR(1)=FOREGROUND COLOR */
/*           (2)=BACKGROUND COLOR */
/*           (3)=INTENSITY */
/*           (4)=LINE STYLE */
/*           (5)=LINE WIDTH */
/*           (6)=CHARACTER BOX Y */
/*           (7)=CHARACTER BOX X */
    if (vcpstb_1.mopoly == 0) {
	if (vcescp_1.pgform == 0) {
	    x[0] = (float)0.;
	    x[1] = (float)0.;
	    x[2] = (float).99999;
	    x[3] = (float).99999;
	    y[0] = (float)0.;
	    y[1] = (float).75;
	    y[2] = (float).75;
	    y[3] = (float)0.;
	} else {
	    x[0] = (float)0.;
	    x[1] = (float)0.;
	    x[2] = (float).75;
	    x[3] = (float).75;
	    y[0] = (float)0.;
	    y[1] = (float).99999;
	    y[2] = (float).99999;
	    y[3] = (float)0.;
	}
	kolsav = i_nint(vcattr_1.vector);
	i__1 = i_nint(&vcattr_1.vector[1]);
	vdstfc_(&i__1);
	vipoly_(x, y, &c__4);
	vdstfc_(&kolsav);
    }
    return 0;
} /* pstbbg_ */

/* Subroutine */ int pstjob_()
{
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* PSTJOB           - GET JOB ID AND ROUTING INFORMATION */
/* ENVIRONMENT      - COMPUTER-DEPENDENT FOR CTSS */
/* ENTRY CONDITIONS - */
/* CALLS            - */
/* EXIT CONDITIONS  - KJTIME - TIME HOLLERITH STRING */
/*                    KJDATE - DATE HOLLERITH STRING */
/*                    KUSRID - USER IDENTIFICATION */
/*                    KJROUT - ROUTING INFORMATION */
/* NARRATIVE        - THIS ROUTINE INQUIRES THE SYSTEM TO FIND THE ABOVE */
/*                    INFORMATION.  THE INFO IS PACKED INTO THE ARRAYS AS */
/*                    HOLLERITH (INTERNAL DISPLAY CODE) STRINGS.  A TERMI */
/*                    CHARACTER "\" IS APPENDED TO EACH STRING SO THE CAL */
/*                    ROUTINE CAN FIND THE END IF FOR SOME REASON THE LEN */
/*                    VARIABLES ARE NOT SUFFICIENT. */
/*  None of functions are used in pst driver */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/* JOB ID INFORMATION. (HC1, DIC) */
/*  FOR SECURITY MARKINGS, CTSS CODES NEED TO MAP TO THESE SILLY */
/*  OLD SCOPE SECURITY CODES */
/*      SCOPE 3 CODE */
/*         0    UNCL */
/*         1    UNDEFINED */
/*         2    UNDEFINED */
/*         3    PARD */
/*         4    C */
/*         5    CNSI */
/*         6    CFRD */
/*         7    CRD */
/*         8    S */
/*         9    SNSI */
/*        10    SFRD */
/*        11    SRD */
/*     GET CLASSIFICATION LEVEL */
/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
    vcjob_1.ksecur = 0;
/*     GET USER ID */
    vcjob_1.kusrsz = 8;
    vcjob_1.kusrid[0] = 0;
    vcjob_1.kusrid[1] = 0;
    vcjob_1.kusrid[2] = 0;
    vcjob_1.kusrid[3] = 0;
/*     GET JOB ID AND USERS NAME */
    vcjob_1.kjobid[0] = 0;
    vcjob_1.kjobid[1] = 0;
    vcjob_1.kjobid[2] = 0;
    vcjob_1.kjobid[3] = 0;
    vcjob_1.kidsiz = 24;
/*     GET BOX NUMBER */
    vcjob_1.kszrou = 777;
    vcjob_1.kjrout[0] = 0;
    vcjob_1.kjrout[1] = 0;
    vcjob_1.kjrout[2] = 0;
    vcjob_1.kjrout[3] = 0;
/* GET MACHINE ID */
    vcjob_1.machin[0] = 0;
    vcjob_1.machin[1] = 0;
    vcjob_1.machin[2] = 0;
    vcjob_1.maclen = 1;
/* GET THE TIME AND DATE */
    vcjob_1.kjtime[0] = 0;
    vcjob_1.kjtime[1] = 0;
    vcjob_1.kjtime[2] = 0;
    vcjob_1.kjdate[0] = 0;
    vcjob_1.kjdate[1] = 0;
    vcjob_1.kjdate[2] = 0;
    return 0;
} /* pstjob_ */

/* Subroutine */ int pstsel_(char *karg, ftnlen karg_len)
{
    /* Initialized data */

    static integer ionce = 0;

    /* Format strings */
    static char fmt_10[] = "(/,\002 This VDI PostScript driver has seven opt\
ions.\002,/,\002     1. black & white, no polygon fill\002,/,\002     3. bla\
ck & white, polygon fill\002,/,\002     5. color,\002,/,\002     7. color, b\
lack-white interchange\002,/,\002     8. gray-scale, black-white interchang\
e\002,/,\002     9. color, black background\002,/,\002    10. gray-scale, bl\
ack background\002,/,\002 Enter option number\002)";
    static char fmt_40[] = "(\002 Bad input - defaulting to 7\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_cmp(char *, char *, ftnlen, ftnlen), s_wsfe(cilist *), e_wsfe(),
	     s_rsfe(cilist *), do_fio(integer *, char *, ftnlen), e_rsfe();

    /* Local variables */
    static char arg[2], ans[2];

    /* Fortran I/O blocks */
    static cilist io___105 = { 0, 6, 0, fmt_10, 0 };
    static cilist io___106 = { 1, 5, 0, "(A)", 0 };
    static cilist io___108 = { 0, 6, 0, fmt_40, 0 };


/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/*     Select type of desired output. Four options are */
/*                                                      device number */
/*   1. black & white, batch, no poly fill                  799.1 */
/*   2. black & white, interactive, no poly                 799.2 */
/*   3. black & white, batch, poly fill                     799.3 */
/*   4. black & white, interactive, poly fill               799.4 */
/*   5. color, batch                                        799.5 */
/*   6. color, interactive                                  799.6 */
/*   7. color, batch, black-white interchange               799.7 */
/*   8. color, interactive, black-white interchange         799.8 */
/*   9. color, batch, black background                      799.9 */
/*   10.color, interactive, black background                799.11 */
/*     A second function of this routine is to set the minimum line */
/*     width. For most systems the minimum width line is too narrow. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/*     pstmlw controls minimum line width */
/*     kpstbg controls background coloring */
/*            = 0,  not colored (white ground from paper) */
/*            = 1,  colored black */
/*     kpstci controls black-white interchange (colors 0 & 7 only) */
/*            = 0,  no interchange */
/*            = 1,  colors interchanged */
/*     mopoly controls polygon fill =0, on ;  =1, off */
/*     mocolr controls color =0, on ;  =1, off */
    s_copy(arg, karg, (ftnlen)2, karg_len);
    if (ionce == 0) {
	vcpsta_1.kpstbg = 0;
	vcpsta_1.kpstci = 0;
	ionce = 1;
	if (s_cmp(arg, " ", (ftnlen)2, (ftnlen)1) == 0 && s_cmp(
		blotans_1.bltans, " ", (ftnlen)2, (ftnlen)1) == 0) {
	    s_wsfe(&io___105);
	    e_wsfe();
	    i__1 = s_rsfe(&io___106);
	    if (i__1 != 0) {
		goto L30;
	    }
	    i__1 = do_fio(&c__1, ans, (ftnlen)2);
	    if (i__1 != 0) {
		goto L30;
	    }
	    i__1 = e_rsfe();
	    if (i__1 != 0) {
		goto L30;
	    }
	} else {
	    if (s_cmp(arg, " ", (ftnlen)2, (ftnlen)1) != 0) {
		s_copy(ans, arg, (ftnlen)2, (ftnlen)2);
	    } else if (s_cmp(blotans_1.bltans, " ", (ftnlen)2, (ftnlen)1) != 
		    0) {
		s_copy(ans, blotans_1.bltans, (ftnlen)2, (ftnlen)2);
	    }
	}
	if (s_cmp(ans, "6", (ftnlen)2, (ftnlen)1) == 0) {
	    devcap_1.dev[3] = (float)256.;
	    devcap_1.dev[22] = (float)799.6;
	    devcap_1.dev[25] = (float)1.;
	    devcap_1.dev[26] = (float)256.;
	    devcap_1.dev[31] = (float)1.;
	    vcpstb_1.mopoly = 0;
	    vcpstb_1.mocolr = 0;
	} else if (s_cmp(ans, "5", (ftnlen)2, (ftnlen)1) == 0) {
	    devcap_1.dev[3] = (float)256.;
	    devcap_1.dev[22] = (float)799.5;
	    devcap_1.dev[25] = (float)1.;
	    devcap_1.dev[26] = (float)256.;
	    devcap_1.dev[31] = (float)1.;
	    vcpstb_1.mopoly = 0;
	    vcpstb_1.mocolr = 0;
	} else if (s_cmp(ans, "2", (ftnlen)2, (ftnlen)1) == 0) {
	    devcap_1.dev[3] = (float)1.;
	    devcap_1.dev[22] = (float)799.2;
	    devcap_1.dev[25] = (float)1.;
	    devcap_1.dev[26] = (float)1.;
	    devcap_1.dev[31] = (float)0.;
	    vcpstb_1.mopoly = 1;
	    vcpstb_1.mocolr = 1;
	} else if (s_cmp(ans, "1", (ftnlen)2, (ftnlen)1) == 0) {
	    devcap_1.dev[3] = (float)1.;
	    devcap_1.dev[22] = (float)799.1;
	    devcap_1.dev[25] = (float)1.;
	    devcap_1.dev[26] = (float)1.;
	    devcap_1.dev[31] = (float)0.;
	    vcpstb_1.mopoly = 1;
	    vcpstb_1.mocolr = 1;
	} else if (s_cmp(ans, "4", (ftnlen)2, (ftnlen)1) == 0) {
	    devcap_1.dev[3] = (float)1.;
	    devcap_1.dev[22] = (float)799.4;
	    devcap_1.dev[25] = (float)1.;
	    devcap_1.dev[26] = (float)1.;
	    devcap_1.dev[31] = (float)0.;
	    vcpstb_1.mopoly = 0;
	    vcpstb_1.mocolr = 1;
	} else if (s_cmp(ans, "3", (ftnlen)2, (ftnlen)1) == 0) {
	    devcap_1.dev[3] = (float)1.;
	    devcap_1.dev[22] = (float)799.3;
	    devcap_1.dev[25] = (float)1.;
	    devcap_1.dev[26] = (float)1.;
	    devcap_1.dev[31] = (float)0.;
	    vcpstb_1.mopoly = 0;
	    vcpstb_1.mocolr = 1;
	} else if (s_cmp(ans, "7", (ftnlen)2, (ftnlen)1) == 0) {
	    devcap_1.dev[3] = (float)256.;
	    devcap_1.dev[22] = (float)799.7;
	    devcap_1.dev[25] = (float)1.;
	    devcap_1.dev[26] = (float)256.;
	    devcap_1.dev[31] = (float)1.;
	    vcpstb_1.mopoly = 0;
	    vcpstb_1.mocolr = 0;
	    vcpsta_1.kpstci = 1;
	} else if (s_cmp(ans, "8", (ftnlen)2, (ftnlen)1) == 0) {
	    devcap_1.dev[3] = (float)256.;
	    devcap_1.dev[22] = (float)799.8;
	    devcap_1.dev[25] = (float)1.;
	    devcap_1.dev[26] = (float)256.;
	    devcap_1.dev[31] = (float)1.;
	    vcpstb_1.mopoly = 0;
	    vcpstb_1.mocolr = 0;
	    vcpsta_1.kpstci = 1;
	} else if (s_cmp(ans, "9", (ftnlen)2, (ftnlen)1) == 0) {
	    devcap_1.dev[3] = (float)256.;
	    devcap_1.dev[22] = (float)799.9;
	    devcap_1.dev[25] = (float)1.;
	    devcap_1.dev[26] = (float)256.;
	    devcap_1.dev[31] = (float)1.;
	    vcpstb_1.mopoly = 0;
	    vcpstb_1.mocolr = 0;
	    vcpsta_1.kpstbg = 1;
	} else if (s_cmp(ans, "10", (ftnlen)2, (ftnlen)2) == 0) {
	    devcap_1.dev[3] = (float)256.;
	    devcap_1.dev[22] = (float)799.1;
	    devcap_1.dev[25] = (float)1.;
	    devcap_1.dev[26] = (float)256.;
	    devcap_1.dev[31] = (float)1.;
	    vcpstb_1.mopoly = 0;
	    vcpstb_1.mocolr = 0;
	    vcpsta_1.kpstbg = 1;
	} else {
	    goto L30;
	}
	goto L50;
L30:
	s_wsfe(&io___108);
	e_wsfe();
	devcap_1.dev[3] = (float)256.;
	devcap_1.dev[22] = (float)799.7;
	devcap_1.dev[25] = (float)1.;
	devcap_1.dev[26] = (float)256.;
	devcap_1.dev[31] = (float)1.;
	vcpstb_1.mopoly = 0;
	vcpstb_1.mocolr = 0;
	vcpsta_1.kpstci = 1;
	s_copy(arg, " ", (ftnlen)2, (ftnlen)1);
L50:
/* - INCLUDE PSTMLW */
/*       set minimum line width (range 0 to 1) */
	vcpsta_1.pstmlw = (float).025;
/* - */
    }
    return 0;
} /* pstsel_ */

/* Subroutine */ int pstini_()
{
    /* Initialized data */

    static integer kway = 0;

    extern /* Subroutine */ int pstbuf_(integer *, char *, ftnlen);

/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
/*     Generate any system dependent records which must be at the first */
/*     of PostScript output file. For example, a SUN laser printer */
/*     requires the first record of the file to be %! for the file */
/*     recognized as a PostScript file. This routine writes these */
/*     initial records. */
/* C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C C */
    if (kway == 0) {
	kway = 1;
/*       generate first records in output file */
/* - INCLUDE PSTHEAD */
/*       the following is for a SUN UNIX system */
/*       record is a comment except for sun lpr */
	pstbuf_(&c__14, "%!PS-Adobe-2.0", (ftnlen)14);
/*       clear line buffer */
	pstbuf_(&c__0, " ", (ftnlen)1);
/* - */
    }
    return 0;
} /* pstini_ */

#ifdef __cplusplus
	}
#endif

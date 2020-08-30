/* tablet.f -- translated by f2c (version 20160102).
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

/* Table of constant values */

static integer c__1 = 1;
static integer c__2 = 2;
static integer c__14 = 14;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int tablet_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *msnap, integer *mcom, integer *icom, integer *jcom, char 
	*cin, real *rin, integer *iin, integer *kin, integer *idump, integer *
	n, integer *ipoint, real *coor, integer *ipboun, integer *iline, 
	integer *ltype, integer *nint, real *factor, integer *lcon, integer *
	ilboun, integer *isboun, integer *iside, integer *nlps, integer *
	ifline, integer *illist, integer *iregn, integer *imat, integer *nspr,
	 integer *ifside, integer *islist, integer *irpb, integer *ipbf, 
	integer *nppf, integer *ifpb, integer *listpb, integer *ilbf, integer 
	*nlpf, integer *iflb, integer *listlb, integer *isbf, integer *nspf, 
	integer *ifsb, integer *listsb, integer *linkp, integer *linkl, 
	integer *links, integer *linkr, integer *linkm, integer *linkpb, 
	integer *linklb, integer *linksb, real *rextrm, integer *iholdp, 
	integer *iholdl, integer *iholds, integer *iholdr, integer *iholdm, 
	integer *ihold2, integer *ihold3, integer *iwtpbf, integer *iwtlbf, 
	integer *iwtsbf, integer *irgflg, char *title, logical *noroom, 
	logical *drwtab, real *xx1, real *yy1, real *scale, real *ct, real *
	st, real *x1, real *x2, real *y1, real *y2, logical *alpha, char *
	dev1, logical *snap, real *snapdx, integer *nsnap, logical *vaxvms, 
	logical *tbzoom, logical *axist, logical *wrote, logical *batch, char 
	*versn, real *time1, ftnlen cin_len, ftnlen title_len, ftnlen 
	dev1_len, ftnlen versn_len)
{
    /* Format strings */
    static char fmt_10010[] = "(\002 THE TABLET (AND PLOTTING) LIMITS ARE DE\
FAULTED TO:\002,/\002         XMIN: \002,g14.7,/,\002         XMAX: \002,g14\
.7,/,\002         YMIN: \002,g14.7,/,\002         YMAX: \002,g14.7)";
    static char fmt_10020[] = "(\002 FOR THE DEFAULT GRID, THE MINIMUM X I\
S: \002,g14.7,/,\002                       THE MAXIMUM X IS: \002,g14.7,/\
,\002                          THE X STEP IS: \002,g14.7,/,\002             \
          THE MINIMUM Y IS: \002,g14.7,/,\002                       THE MAXI\
MUM Y IS: \002,g14.7,/,\002                          THE Y STEP IS: \002,g14\
.7)";
    static char fmt_10000[] = "(\002 THE LAST SUCCESSFUL \002,a1,\002 INPUT \
WAS: \002,g14.7)";

    /* System generated locals */
    integer i__1;
    real r__1, r__2, r__3, r__4;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(), 
	    s_cmp(char *, char *, ftnlen, ftnlen), s_wsfi(icilist *), e_wsfi()
	    , s_rsfi(icilist *), e_rsfi();
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__;
    static real x11, y11, x22, y22;
    static integer iz;
    static real xx2, yy2;
    static integer len;
    static logical err;
    static real sdx, sdy;
    static logical ians;
    static real xmin, xmax, ymin, ymax, x1old, x2old, y1old, y2old, xmin1, 
	    ymin1, xmax1, ymax1, xmin2, xmax2, ymin2, ymax2;
    extern /* Subroutine */ int digit_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, char *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, char *, logical *, real *, real *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, real *, 
	    real *, real *, real *, real *, real *, real *, logical *, char *,
	     logical *, real *, integer *, logical *, logical *, ftnlen, 
	    ftnlen, ftnlen);
    static integer index;
    static logical drawn;
    extern /* Subroutine */ int fexit_(logical *, integer *, integer *, 
	    integer *, char *, integer *, real *, integer *, real *, logical *
	    , char *, ftnlen, ftnlen), spawn_(logical *), mesage_(char *, 
	    ftnlen), frefld_(integer *, integer *, char *, integer *, integer 
	    *, integer *, integer *, char *, integer *, real *, ftnlen, 
	    ftnlen), addsnp_(integer *, real *, integer *, integer *, real *, 
	    logical *), tabint_(real *, real *, real *, real *, real *, real *
	    , real *, real *, real *, real *, real *, logical *), initdg_(
	    integer *, integer *, integer *, char *, real *, integer *, 
	    integer *, integer *, real *, real *, real *, real *, real *, 
	    real *, real *, real *, real *, logical *, logical *, ftnlen), 
	    getext_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, real *, real *, 
	    real *);
    static integer iostat;
    static char intrnl[8];
    extern /* Subroutine */ int strlng_(char *, integer *, ftnlen), unisnp_(
	    integer *, real *, integer *, integer *, real *, real *, real *), 
	    snapxy_(integer *, integer *, integer *, integer *, real *, 
	    integer *, real *, integer *), intrup_(char *, logical *, integer 
	    *, integer *, integer *, char *, integer *, real *, integer *, 
	    ftnlen, ftnlen), zoomlt_(integer *, integer *, integer *, char *, 
	    real *, integer *, integer *, integer *, logical *, logical *, 
	    char *, real *, real *, real *, real *, real *, real *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, real *, 
	    real *, ftnlen, ftnlen), help_fq__(integer *);

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___17 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___22 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___23 = { 0, 6, 0, fmt_10000, 0 };
    static icilist io___33 = { 0, intrnl, 0, "(E8.1)", 8, 1 };
    static icilist io___34 = { 0, intrnl, 0, "(E8.1)", 8, 1 };
    static icilist io___35 = { 0, intrnl, 0, "(E8.1)", 8, 1 };
    static icilist io___36 = { 0, intrnl, 0, "(E8.1)", 8, 1 };
    static cilist io___37 = { 0, 6, 0, fmt_10020, 0 };


/* *********************************************************************** */
/*  SUBROUTINE TABLET = SUBROUTINE TO CONTROL DIGITIZE FUNCTIONS */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     FASTQ = A PROGRAM TO QUICKLY PREPARE QMESH INPUT */
/* *********************************************************************** */
/*  SUBROUTINES CALLED: */
/*     DREAD  = SETS ALL PARAMETERS UP FOR READING FROM A DIGI-PAD */
/*     DPREAD = READS INPUT FROM A DIGI-PAD DIGITIZER */
/*     CLOSE  = FINDS THE CLOSEST EXISTING POINT TO THE MOUSE */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     IANS   = LOGICAL RESPONSE FROM YES-NO QUESTION */
/*     TITLE  = MESH TITLE */
/*     DRWTAB = .TRUE. IF THE TABLET IS INITIALIZED TO A DRAWING */
/*     XX1    = DIGITIZATION PAD X COORDINATE OF POINT 1 (PAD INIT) */
/*     YY1    = DIGITIZATION PAD Y COORDINATE OF POINT 1 (PAD INIT) */
/*     X1     = USER X COORDINATE OF POINT 1 (PAD INIT) */
/*     Y1     = USER Y COORDINATE OF POINT 1 (PAD INIT) */
/*     X2     = USER X COORDINATE OF POINT 2 (PAD INIT) */
/*     Y2     = USER Y COORDINATE OF POINT 2 (PAD INIT) */
/*     X      = THE X LOCATION IN USER COORDINATES */
/*     Y      = THE Y LOCATION IN USER COORDINATES */
/*     SCALE  = THE SCALE FACTOR FROM DIGITIZED TO USER COORDINATES */
/*     CT     = THE COSINE OF THE ANGLE OF THE DRAWING ON THE PAD */
/*     ST     = THE SINE OF THE ANGLE OF THE DRAWING ON THE PAD */
/*     CHANGE = .TRUE. IF THE POINT USED IS THE CLOSEST POINT ONLY */
/*     SLIDE  = .TRUE. IF THE NEXT POINT IS TO HAVE THE CLOSEST POINT'S */
/*              COORDINATES, BUT NEW NUMBERING (SLIDE LINE USE) */
/*     NOROOM = .TRUE. IF THE AMOUNT OF DATA EXCEEDS DIMENSIONED LIMITS */
/* *********************************************************************** */
    /* Parameter adjustments */
    iwtpbf -= 4;
    iholdp -= 3;
    linkpb -= 3;
    linkp -= 3;
    listpb -= 3;
    --ifpb;
    --nppf;
    --ipbf;
    --ipboun;
    coor -= 3;
    --ipoint;
    iwtsbf -= 4;
    iwtlbf -= 4;
    ihold3 -= 3;
    ihold2 -= 3;
    --iholdl;
    linksb -= 3;
    linklb -= 3;
    linkl -= 3;
    listsb -= 3;
    --ifsb;
    --nspf;
    --isbf;
    listlb -= 3;
    --iflb;
    --nlpf;
    --ilbf;
    --isboun;
    --ilboun;
    lcon -= 4;
    --factor;
    --nint;
    --ltype;
    --iline;
    iholds -= 3;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --iside;
    --irgflg;
    iholdm -= 3;
    iholdr -= 3;
    rextrm -= 5;
    linkm -= 3;
    linkr -= 3;
    --irpb;
    --islist;
    --ifside;
    --nspr;
    --imat;
    --iregn;
    snapdx -= 3;
    --kin;
    --iin;
    --rin;
    cin -= 72;
    --n;
    --nsnap;

    /* Function Body */
    drawn = FALSE_;
/*  GET THE BODY EXTREMES */
    getext_(mp, ml, ms, mr, &n[1], &ipoint[1], &coor[3], &iline[1], &ltype[1],
	     &lcon[4], &nlps[1], &ifline[1], &illist[1], &nspr[1], &ifside[1],
	     &islist[1], &linkp[3], &linkl[3], &links[3], &linkr[3], &rextrm[
	    5], &xmin1, &xmax1, &ymin1, &ymax1);
/*  GET THE DEFAULT ZOOM AND GRID DEFINITIONS IF NOTHING HAS BEEN DEFINED */
    if (! (*tbzoom)) {
/*  SET THE BODY EXTREMES AS THE ZOOM EXTREMES */
	*x1 = xmin1;
	*x2 = xmax1;
	*y1 = ymin1;
	*y2 = ymax1;
	s_wsfe(&io___6);
	do_fio(&c__1, (char *)&(*x1), (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&(*x2), (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&(*y1), (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&(*y2), (ftnlen)sizeof(real));
	e_wsfe();
/*  GET THE DEFAULT TABLET INITIALIZATION */
	tabint_(x1, x2, y1, y2, ct, st, scale, xx1, yy1, &xx2, &yy2, drwtab);
	*tbzoom = TRUE_;
    } else {
	x1old = *x1;
	x2old = *x2;
	y1old = *y1;
	y2old = *y2;
    }
/*  GET THE DEFAULT GRID IF NO GRID IS DEFINED */
    if (*snap && (nsnap[1] < 2 || nsnap[2] < 2)) {
	snapxy_(mp, msnap, &n[1], &ipoint[1], &coor[3], &linkp[3], &snapdx[3],
		 &nsnap[1]);
    }
/*  ENTER DIGITIZING OPTION */
L100:
    if (*icom > *jcom) {
	mesage_(" ", (ftnlen)1);
	frefld_(&iz, &iz, "ENTER TABLET OPTION: ", mcom, &iostat, jcom, &kin[
		1], cin + 72, &iin[1], &rin[1], (ftnlen)21, (ftnlen)72);
	*icom = 1;
    }
/*  SPAWN A PROCESS */
    if (s_cmp(cin + *icom * 72, "SP", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(cin 
	    + *icom * 72, "sp", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	spawn_(vaxvms);
/*  SET THE SNAP FLAG ON */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'S' || *(unsigned char *)
	    &cin[*icom * 72] == 's') {
	++(*icom);
	if (*snap) {
	    *snap = FALSE_;
	    mesage_(" ", (ftnlen)1);
	    mesage_("SNAP COORDINATE DIGITIZAITON - DISABLED", (ftnlen)39);
	} else {
	    *snap = TRUE_;
	    mesage_(" ", (ftnlen)1);
	    mesage_("SNAP COORDINATE DIGITIZAITON - ENABLED", (ftnlen)38);
	    if (nsnap[1] < 2 || nsnap[2] < 2) {
		mesage_("PROPOSED SNAP GRID SPACING", (ftnlen)26);
		sdx = (*x2 - *x1) / (float)20.;
		sdy = (*y2 - *y1) / (float)15.;
		s_wsfe(&io___17);
		r__1 = *x1 - sdx;
		do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
		r__2 = *x2 + sdx;
		do_fio(&c__1, (char *)&r__2, (ftnlen)sizeof(real));
		do_fio(&c__1, (char *)&sdx, (ftnlen)sizeof(real));
		r__3 = *y1 - sdx;
		do_fio(&c__1, (char *)&r__3, (ftnlen)sizeof(real));
		r__4 = *y2 + sdx;
		do_fio(&c__1, (char *)&r__4, (ftnlen)sizeof(real));
		do_fio(&c__1, (char *)&sdy, (ftnlen)sizeof(real));
		e_wsfe();
		intrup_("IS THIS EXCEPTABLE", &ians, mcom, icom, jcom, cin + 
			72, &iin[1], &rin[1], &kin[1], (ftnlen)18, (ftnlen)72)
			;
		if (ians) {
		    index = 1;
		    r__1 = *x1 - sdx;
		    r__2 = *x2 + sdx;
		    unisnp_(msnap, &snapdx[3], &nsnap[1], &index, &r__1, &
			    r__2, &sdx);
		    index = 2;
		    r__1 = *y1 - sdy;
		    r__2 = *y2 + sdy;
		    unisnp_(msnap, &snapdx[3], &nsnap[1], &index, &r__1, &
			    r__2, &sdy);
		} else {
		    mesage_("PLEASE DEFINE THE GRID USING UNIFORM, UX, UY, X\
, OR Y OPTIONS", (ftnlen)61);
		}
	    }
	}
/*  SHOW THE BUTTON DEFINITIONS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'A' || *(unsigned char *)
	    &cin[*icom * 72] == 'a') {
	++(*icom);
	if (*axist) {
	    *axist = FALSE_;
	    mesage_("AXIS DRAWING - OFF", (ftnlen)18);
	} else {
	    *axist = TRUE_;
	    mesage_("AXIS DRAWING - ON", (ftnlen)17);
	}
/*  SHOW THE BUTTON DEFINITIONS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'B' || *(unsigned char *)
	    &cin[*icom * 72] == 'b') {
	++(*icom);
	help_fq__(&c__2);
/*  ADD UNIFORM Y SNAP GRID SPACINGS */
    } else if (s_cmp(cin + *icom * 72, "UY", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "uy", (ftnlen)1, (ftnlen)2) == 0) {
	++(*icom);
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, "ENTER GRID YMIN, YMAX, AND Y GRID SPACING:", 
		    mcom, &iostat, jcom, &kin[1], cin + 72, &iin[1], &rin[1], 
		    (ftnlen)42, (ftnlen)72);
	    *icom = 1;
	}
	if (*jcom - *icom + 1 >= 3) {
	    index = 2;
	    unisnp_(msnap, &snapdx[3], &nsnap[1], &index, &rin[*icom], &rin[*
		    icom + 1], &rin[*icom + 2]);
	    *icom += 3;
	    *snap = TRUE_;
	} else {
	    mesage_("NOT ENOUGH INFORMATION DEFINED TO ENTER UNIFORM Y GRID", 
		    (ftnlen)54);
	    mesage_("NO ADDITIONAL Y GRID DEFINED", (ftnlen)28);
	}
/*  ADD UNIFORM X SNAP GRID SPACINGS */
    } else if (s_cmp(cin + *icom * 72, "UX", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ux", (ftnlen)1, (ftnlen)2) == 0) {
	++(*icom);
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, "ENTER GRID XMIN, XMAX, AND X GRID SPACING:", 
		    mcom, &iostat, jcom, &kin[1], cin + 72, &iin[1], &rin[1], 
		    (ftnlen)42, (ftnlen)72);
	    *icom = 1;
	}
	if (*jcom - *icom + 1 >= 3) {
	    index = 1;
	    unisnp_(msnap, &snapdx[3], &nsnap[1], &index, &rin[*icom], &rin[*
		    icom + 1], &rin[*icom + 2]);
	    *icom += 3;
	    *snap = TRUE_;
	} else {
	    mesage_("NOT ENOUGH INFORMATION DEFINED TO ENTER UNIFORM X GRID", 
		    (ftnlen)54);
	    mesage_("NO ADDITIONAL X GRID DEFINED", (ftnlen)28);
	}
/*  ADD UNIFORM SNAP GRID SPACINGS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'U' || *(unsigned char *)
	    &cin[*icom * 72] == 'u') {
	++(*icom);
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, "ENTER GRID XMIN, XMAX, YMIN, YMAX, AND GRID S\
PACING:", mcom, &iostat, jcom, &kin[1], cin + 72, &iin[1], &rin[1], (ftnlen)
		    52, (ftnlen)72);
	    *icom = 1;
	}
	if (*jcom - *icom + 1 >= 5) {
	    *snap = TRUE_;
	    index = 1;
	    unisnp_(msnap, &snapdx[3], &nsnap[1], &index, &rin[*icom], &rin[*
		    icom + 1], &rin[*icom + 4]);
	    index = 2;
	    unisnp_(msnap, &snapdx[3], &nsnap[1], &index, &rin[*icom + 2], &
		    rin[*icom + 3], &rin[*icom + 4]);
	    *icom += 5;
	} else {
	    mesage_("NOT ENOUGH INFORMATION DEFINED TO ENTER UNIFORM GRID", (
		    ftnlen)52);
	    mesage_("NO GRID DEFINED", (ftnlen)15);
	}
/*  CLEAR ALL X GRID DEFINITIONS */
    } else if (s_cmp(cin + *icom * 72, "XC", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "xc", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	nsnap[1] = 0;
	mesage_("ALL X SNAP GRID DEFINITIONS HAVE BEEN CLEARED", (ftnlen)45);
	mesage_(" ", (ftnlen)1);
/*  ADD X SNAP GRID SPACINGS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'X' || *(unsigned char *)
	    &cin[*icom * 72] == 'x') {
	++(*icom);
	if (*icom > *jcom) {
	    mesage_("ENTER X GRID VALUES IN ANY ORDER:", (ftnlen)33);
	    mesage_("SEPERATE VALUES BY DELIMITERS OR RETURN KEY", (ftnlen)43)
		    ;
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	}
L110:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	if (*jcom >= *icom) {
	    *snap = TRUE_;
	    index = 1;
	    i__1 = *jcom;
	    for (i__ = *icom; i__ <= i__1; ++i__) {
		if (kin[i__] > 0) {
		    ++(*icom);
		    addsnp_(msnap, &snapdx[3], &nsnap[1], &index, &rin[i__], &
			    err);
		    if (err) {
			s_wsfe(&io___22);
			do_fio(&c__1, "X", (ftnlen)1);
			do_fio(&c__1, (char *)&rin[i__ - 1], (ftnlen)sizeof(
				real));
			e_wsfe();
			goto L130;
		    }
		} else {
		    goto L130;
		}
/* L120: */
	    }
	    goto L110;
L130:
	    ;
	}
/*  CLEAR ALL Y GRID DEFINITIONS */
    } else if (s_cmp(cin + *icom * 72, "YC", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "yc", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	nsnap[2] = 0;
	mesage_("ALL Y SNAP GRID DEFINITIONS HAVE BEEN CLEARED", (ftnlen)45);
	mesage_(" ", (ftnlen)1);
/*  ADD Y SNAP GRID SPACINGS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'Y' || *(unsigned char *)
	    &cin[*icom * 72] == 'y') {
	++(*icom);
	if (*icom > *jcom) {
	    mesage_("ENTER Y GRID VALUES IN ANY ORDER:", (ftnlen)33);
	    mesage_("SEPERATE VALUES BY DELIMITERS OR RETURN KEY", (ftnlen)43)
		    ;
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	}
L140:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	if (*jcom >= *icom) {
	    *snap = TRUE_;
	    index = 2;
	    i__1 = *jcom;
	    for (i__ = *icom; i__ <= i__1; ++i__) {
		if (kin[i__] > 0) {
		    ++(*icom);
		    addsnp_(msnap, &snapdx[3], &nsnap[1], &index, &rin[i__], &
			    err);
		    if (err) {
			s_wsfe(&io___23);
			do_fio(&c__1, "Y", (ftnlen)1);
			do_fio(&c__1, (char *)&rin[i__ - 1], (ftnlen)sizeof(
				real));
			e_wsfe();
			goto L160;
		    }
		} else {
		    goto L160;
		}
/* L150: */
	    }
	    goto L140;
L160:
	    ;
	}
/*  SET ZOOM LIMITS FOR PLOTTING */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned char *)
	    &cin[*icom * 72] == 'p') {
	++(*icom);
	mesage_("GRID POINTS ADDED FOR ALL POINT COORDINATE VALUES", (ftnlen)
		49);
	snapxy_(mp, msnap, &n[1], &ipoint[1], &coor[3], &linkp[3], &snapdx[3],
		 &nsnap[1]);
	*snap = TRUE_;
/*  SET ZOOM LIMITS FOR PLOTTING */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'Z' || *(unsigned char *)
	    &cin[*icom * 72] == 'z') {
	++(*icom);
	zoomlt_(mcom, icom, jcom, cin + 72, &rin[1], &iin[1], &kin[1], idump, 
		&drawn, alpha, dev1, x1, x2, y1, y2, &x11, &x22, &y11, &y22, &
		xmin1, &xmax1, &ymin1, &ymax1, &xmin, &xmax, &ymin, &ymax, (
		ftnlen)72, (ftnlen)3);
	drawn = FALSE_;
	if (*drwtab) {
	    mesage_("TABLET EXTREMES REMAIN TIED TO DRAWING INITIALIZATION", (
		    ftnlen)53);
	    mesage_("SCREEN PLOTTING ZOOM CHANGED", (ftnlen)28);
	    x1old = xmin;
	    x2old = xmax;
	    y1old = ymin;
	    y2old = ymax;
	} else {
	    *x1 = xmin;
	    *x2 = xmax;
	    *y1 = ymin;
	    *y2 = ymax;
	    tabint_(x1, x2, y1, y2, ct, st, scale, xx1, yy1, &xx2, &yy2, 
		    drwtab);
	    mesage_("SCREEN PLOTTING ZOOM CHANGED", (ftnlen)28);
	    mesage_("TABLET EXTREMES RESET TO ZOOM LIMITS", (ftnlen)36);
	}
/*  INITIALIZE DIGITIZING PAD */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'I' || *(unsigned char *)
	    &cin[*icom * 72] == 'i') {
	++(*icom);
	initdg_(mcom, icom, jcom, cin + 72, &rin[1], &iin[1], &kin[1], idump, 
		xx1, yy1, scale, ct, st, x1, x2, y1, y2, drwtab, snap, (
		ftnlen)72);
	if (*drwtab) {
	    x1old = *x1;
	    x2old = *x2;
	    y1old = *y1;
	    y2old = *y2;
	    xmin1 = *x1;
	    xmax1 = *x2;
	    ymin1 = *y1;
	    ymax1 = *y2;
	}
/*  CLEAR ALL GRID DEFINITIONS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'C' || *(unsigned char *)
	    &cin[*icom * 72] == 'c') {
	++(*icom);
	nsnap[1] = 0;
	nsnap[2] = 0;
	mesage_("ALL SNAP GRID DEFINITIONS HAVE BEEN CLEARED", (ftnlen)43);
	mesage_(" ", (ftnlen)1);
/*  DIGITIZING OPTION */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'D' || *(unsigned char *)
	    &cin[*icom * 72] == 'd') {
	++(*icom);
/*  GENERATE A DEFAULT SNAP GRID IF NEEDED */
	if (*snap && (nsnap[1] < 2 || nsnap[2] < 2)) {
	    nsnap[1] = 0;
	    nsnap[2] = 0;
	    mesage_("PROPOSED SNAP GRID SPACING", (ftnlen)26);
	    sdx = (*x2 - *x1) / (float)20.;
	    s_wsfi(&io___33);
	    do_fio(&c__1, (char *)&sdx, (ftnlen)sizeof(real));
	    e_wsfi();
	    s_rsfi(&io___34);
	    do_fio(&c__1, (char *)&sdx, (ftnlen)sizeof(real));
	    e_rsfi();
	    sdy = (*y2 - *y1) / (float)15.;
	    s_wsfi(&io___35);
	    do_fio(&c__1, (char *)&sdy, (ftnlen)sizeof(real));
	    e_wsfi();
	    s_rsfi(&io___36);
	    do_fio(&c__1, (char *)&sdy, (ftnlen)sizeof(real));
	    e_rsfi();
	    sdx = dmax(sdx,sdy);
	    sdy = sdx;
	    s_wsfe(&io___37);
	    r__1 = *x1 - sdx;
	    do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	    r__2 = *x2 + sdx;
	    do_fio(&c__1, (char *)&r__2, (ftnlen)sizeof(real));
	    do_fio(&c__1, (char *)&sdx, (ftnlen)sizeof(real));
	    r__3 = *y1 - sdx;
	    do_fio(&c__1, (char *)&r__3, (ftnlen)sizeof(real));
	    r__4 = *y2 + sdx;
	    do_fio(&c__1, (char *)&r__4, (ftnlen)sizeof(real));
	    do_fio(&c__1, (char *)&sdy, (ftnlen)sizeof(real));
	    e_wsfe();
	    intrup_("IS THIS EXCEPTABLE", &ians, mcom, icom, jcom, cin + 72, &
		    iin[1], &rin[1], &kin[1], (ftnlen)18, (ftnlen)72);
	    if (ians) {
		index = 1;
		r__1 = *x1 - sdx;
		r__2 = *x2 + sdx;
		unisnp_(msnap, &snapdx[3], &nsnap[1], &index, &r__1, &r__2, &
			sdx);
		index = 2;
		r__1 = *y1 - sdy;
		r__2 = *y2 + sdy;
		unisnp_(msnap, &snapdx[3], &nsnap[1], &index, &r__1, &r__2, &
			sdy);
	    } else {
		intrup_("WOULD YOU CARE TO CONTINUE DIGITIZING WITHOUT GRID \
SNAP", &ians, mcom, icom, jcom, cin + 72, &iin[1], &rin[1], &kin[1], (ftnlen)
			55, (ftnlen)72);
		if (ians) {
		    *snap = FALSE_;
		} else {
		    mesage_("PLEASE DEFINE THE GRID USING UNIFORM, UX, UY, X\
, OR Y OPTIONS", (ftnlen)61);
		    goto L100;
		}
	    }
	}
/*  NOW ENTER THE MOUSE CONTROL */
	digit_(mp, ml, ms, mr, msnap, mcom, icom, jcom, cin + 72, &rin[1], &
		iin[1], &kin[1], idump, &n[1], &ipoint[1], &coor[3], &ipboun[
		1], &iline[1], &ltype[1], &nint[1], &factor[1], &lcon[4], &
		ilboun[1], &isboun[1], &iside[1], &nlps[1], &ifline[1], &
		illist[1], &iregn[1], &imat[1], &nspr[1], &ifside[1], &islist[
		1], &irpb[1], &ipbf[1], &nppf[1], &ifpb[1], &listpb[3], &ilbf[
		1], &nlpf[1], &iflb[1], &listlb[3], &isbf[1], &nspf[1], &ifsb[
		1], &listsb[3], &linkp[3], &linkl[3], &links[3], &linkr[3], &
		linkm[3], &linkpb[3], &linklb[3], &linksb[3], &iholdp[3], &
		iholdl[1], &iholds[3], &iholdr[3], &iholdm[3], &ihold2[3], &
		ihold3[3], &iwtpbf[4], &iwtlbf[4], &iwtsbf[4], &irgflg[1], 
		title, noroom, xx1, yy1, scale, ct, st, x1, x2, y1, y2, &x11, 
		&x22, &y11, &y22, &xmin1, &xmax1, &ymin1, &ymax1, &xmin2, &
		xmax2, &ymin2, &ymax2, &x1old, &x2old, &y1old, &y2old, alpha, 
		dev1, snap, &snapdx[3], &nsnap[1], drwtab, axist, (ftnlen)72, 
		(ftnlen)72, (ftnlen)3);
	drawn = TRUE_;
	*wrote = FALSE_;
/*  GO GET MORE ROOM IF NEEDED AND GO STRAIGHT BACK INTO DIGITIZING */
	if (*noroom) {
	    *jcom = 1;
	    *icom = 1;
	    s_copy(cin + 72, "DIG", (ftnlen)72, (ftnlen)3);
	    return 0;
	}
/*  RETURN FROM DIGITIZING */
    } else if (*(unsigned char *)&cin[*icom * 72] == ' ') {
	++(*icom);
	return 0;
/*  EXIT OPTION - EXITS FASTQ */
    } else if (s_cmp(cin + *icom * 72, "EX", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ex", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	strlng_(cin + *icom * 72, &len, (ftnlen)72);
	if (len > 1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'X' || len > 
		1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'x') {
	    help_fq__(&c__14);
	} else {
	    fexit_(wrote, mcom, icom, jcom, cin + 72, &iin[1], &rin[1], &kin[
		    1], time1, batch, versn, (ftnlen)72, (ftnlen)9);
	}
/*  WRITE OUT THE HELP MESSAGE */
    } else {
	++(*icom);
	help_fq__(&c__14);
    }
    goto L100;
} /* tablet_ */

#ifdef __cplusplus
	}
#endif

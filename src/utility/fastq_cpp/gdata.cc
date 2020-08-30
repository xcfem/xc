/* gdata.f -- translated by f2c (version 20160102).
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
static integer c__10002 = 10002;
static integer c__0 = 0;
static integer c__10001 = 10001;
static integer c__5 = 5;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int gdata_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *msc, integer *mcom, integer *icom, integer *jcom, char *
	cin, real *rin, integer *iin, integer *kin, integer *idump, integer *
	n, integer *ipoint, real *coor, integer *ipboun, integer *iline, 
	integer *ltype, integer *nint, real *factor, integer *lcon, integer *
	ilboun, integer *isboun, integer *iside, integer *nlps, integer *
	ifline, integer *illist, integer *ibarst, integer *jmat, integer *
	jcent, integer *nlpb, integer *jfline, integer *jllist, integer *
	iregn, integer *imat, integer *nspr, integer *ifside, integer *islist,
	 integer *linkp, integer *linkl, integer *links, integer *linkb, 
	integer *linkr, integer *linksc, real *rextrm, real *rsize, char *
	scheme, char *defsch, real *defsiz, char *title, logical *labp, 
	logical *labl, logical *labr, logical *axisd, logical *labmd, logical 
	*labi, logical *labf, logical *labpb, logical *lablb, logical *labsbd,
	 logical *labsc, logical *labsz, logical *full, integer *idev, 
	logical *alpha, char *dev1, logical *vaxvms, char *versn, logical *
	wrote, real *time1, logical *hardpl, logical *batch, ftnlen cin_len, 
	ftnlen scheme_len, ftnlen defsch_len, ftnlen title_len, ftnlen 
	dev1_len, ftnlen versn_len)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1, r__2;

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, j, k, l, i1, i2;
    static real r1, x1, y1, x2, y2;
    static integer ii, jj, kk, ll, iz;
    static real xx1, xx2, yy1, yy2;
    static integer iii[1], len;
    static logical flag__;
    static real xmin, xmax, ymin;
    static logical test;
    static real ymax, xmin1, ymin1, xmax1, ymax1;
    extern /* Subroutine */ int flagd_(integer *, integer *, integer *, 
	    integer *, logical *), check_(integer *, integer *, integer *), 
	    pdata_(integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, char *, char *, real *, 
	    real *, integer *, logical *, logical *, logical *, logical *, 
	    logical *, logical *, logical *, logical *, logical *, logical *, 
	    logical *, logical *, logical *, char *, real *, real *, real *, 
	    real *, real *, real *, real *, real *, char *, char *, ftnlen, 
	    ftnlen, ftnlen, ftnlen, ftnlen), geti12_(integer *, integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    integer *, ftnlen), dline_(integer *, integer *, real *, integer *
	    , integer *, integer *, integer *, integer *, integer *, logical *
	    , real *, real *, logical *, logical *, real *, real *, real *, 
	    real *);
    static logical drawn, found;
    static real hxmin, hymin, hxmax, hymax;
    extern /* Subroutine */ int fexit_(logical *, integer *, integer *, 
	    integer *, char *, integer *, real *, integer *, real *, logical *
	    , char *, ftnlen, ftnlen), spawn_(logical *);
    static integer kaval2;
    extern /* Subroutine */ int geti1r_(integer *, integer *, integer *, char 
	    *, integer *, real *, integer *, integer *, real *, integer *, 
	    ftnlen);
    static logical addlnk;
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen), infact_(integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    logical *);
    static logical getmax;
    static integer ifound;
    extern /* Subroutine */ int vdescp_(integer *, integer *, integer *), 
	    vdiqes_(integer *, integer *), getext_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, real *, real *, real *);
    static logical goplot;
    static integer iostat;
    extern /* Subroutine */ int inintr_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *), strlng_(
	    char *, integer *, ftnlen);
    static logical numplt;
    extern /* Subroutine */ int zoomlt_(integer *, integer *, integer *, char 
	    *, real *, integer *, integer *, integer *, logical *, logical *, 
	    char *, real *, real *, real *, real *, real *, real *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, real *, 
	    real *, ftnlen, ftnlen), ltsort_(integer *, integer *, integer *, 
	    integer *, logical *), help_fq__(integer *);

/* *********************************************************************** */
/*  GDATA = SUBROUTINE TO INPUT LIGHT TABLE POINTS */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     FASTQ = A PROGRAM TO QUICKLY PREPARE QMESH INPUT */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    --ipboun;
    coor -= 3;
    --ipoint;
    linkl -= 3;
    --isboun;
    --ilboun;
    lcon -= 4;
    --factor;
    --nint;
    --ltype;
    --iline;
    linkb -= 3;
    links -= 3;
    --jllist;
    --jfline;
    --nlpb;
    --jcent;
    --jmat;
    --ibarst;
    --illist;
    --ifline;
    --nlps;
    --iside;
    --rsize;
    rextrm -= 5;
    linksc -= 3;
    linkr -= 3;
    --islist;
    --ifside;
    --nspr;
    --imat;
    --iregn;
    scheme -= 72;
    --kin;
    --iin;
    --rin;
    cin -= 72;
    --n;
    --idev;

    /* Function Body */
    iz = 0;
    drawn = FALSE_;
    addlnk = FALSE_;
    getmax = FALSE_;
    test = FALSE_;
    numplt = FALSE_;
/*  FLAG ALL THE DATA TO BE PLOTTED */
    flag__ = TRUE_;
    flagd_(mp, &n[18], &linkp[3], &ipoint[1], &flag__);
    flagd_(ml, &n[19], &linkl[3], &iline[1], &flag__);
    flagd_(mr, &n[22], &linkr[3], &iregn[1], &flag__);
/*  GET THE REGION AND BODY EXTREMES */
    getext_(mp, ml, ms, mr, &n[1], &ipoint[1], &coor[3], &iline[1], &ltype[1],
	     &lcon[4], &nlps[1], &ifline[1], &illist[1], &nspr[1], &ifside[1],
	     &islist[1], &linkp[3], &linkl[3], &links[3], &linkr[3], &rextrm[
	    5], &xmin, &xmax, &ymin, &ymax);
    xmin1 = xmin;
    xmax1 = xmax;
    ymin1 = ymin;
    ymax1 = ymax;
    hxmin = xmin;
    hymin = ymin;
    hxmax = xmax;
    hymax = ymax;
/*  ENTER GRAPHICS OPTION */
L100:
    if (*icom > *jcom) {
	mesage_(" ", (ftnlen)1);
	frefld_(&iz, &iz, "ENTER GRAPHICS OPTION: ", mcom, &iostat, jcom, &
		kin[1], cin + 72, &iin[1], &rin[1], (ftnlen)23, (ftnlen)72);
	*icom = 1;
    }
/*  TOGGLE DRAWING OF THE AXIS */
    if (*(unsigned char *)&cin[*icom * 72] == 'A' || *(unsigned char *)&cin[*
	    icom * 72] == 'a') {
	++(*icom);
	if (*axisd) {
	    *axisd = FALSE_;
	    mesage_("AXIS DRAWING - OFF", (ftnlen)18);
	} else {
	    *axisd = TRUE_;
	    mesage_("AXIS DRAWING - ON", (ftnlen)17);
	}
/*  TOGGLE THE FACTOR NUMBERS */
    } else if (s_cmp(cin + *icom * 72, "FA", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "fa", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*labf) {
	    *labf = FALSE_;
	    mesage_("FACTOR LABELS - OFF", (ftnlen)19);
	} else {
	    *labf = TRUE_;
	    mesage_("FACTOR LABELS - ON", (ftnlen)18);
	}
/*  TOGGLE THE FULL DISPLAY OF PROPERTIES */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'F' || *(unsigned char *)
	    &cin[*icom * 72] == 'f') {
	++(*icom);
	if (*full) {
	    *full = FALSE_;
	    mesage_("FULL DISPLAY OF PROPERTIES - OFF", (ftnlen)32);
	} else {
	    *full = TRUE_;
	    mesage_("FULL DISPLAY OF PROPERTIES - ON", (ftnlen)31);
	}
/*  TOGGLE THE SCHEME DISPLAY */
    } else if (s_cmp(cin + *icom * 72, "SC", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "sc", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*labsc) {
	    *labsc = FALSE_;
	    mesage_("SCHEME LABELS - OFF", (ftnlen)19);
	} else {
	    *labsc = TRUE_;
	    mesage_("SCHEME LABELS - ON", (ftnlen)18);
	}
/*  TOGGLE THE SCHEME DISPLAY */
    } else if (s_cmp(cin + *icom * 72, "SI", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "si", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*labsz) {
	    *labsz = FALSE_;
	    mesage_("ELEMENT SIZE LABELS - OFF", (ftnlen)25);
	} else {
	    *labsz = TRUE_;
	    mesage_("ELEMENT SIZE LABELS - ON", (ftnlen)24);
	}
/*  TOGGLE THE MATERIAL NUMBERS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'M' || *(unsigned char *)
	    &cin[*icom * 72] == 'm') {
	++(*icom);
	if (*labmd) {
	    *labmd = FALSE_;
	    mesage_("MATERIAL LABELS - OFF", (ftnlen)21);
	} else {
	    *labmd = TRUE_;
	    mesage_("MATERIAL LABELS - ON", (ftnlen)20);
	}
/*  ENTER LINE INTERVALS */
    } else if (s_cmp(cin + *icom * 72, "II", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ii", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*icom > *jcom) {
	    mesage_("ENTER LINE INTERVALS IN THE FOLLOWING FORMAT:", (ftnlen)
		    45);
	    mesage_("[ LINE NO.  (OR NEG SIDE NO.),  INTERVALS ]", (ftnlen)43)
		    ;
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	}
L110:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	iii[0] = i1;
	if (ifound > 0) {
	    inintr_(ml, ms, &c__1, &i2, iii, &n[19], &n[20], &nint[1], &nlps[
		    1], &ifline[1], &illist[1], &linkl[3], &links[3], &addlnk)
		    ;
	    goto L110;
	}
/*  ENTER LINE FACTORS */
    } else if (s_cmp(cin + *icom * 72, "IF", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "if", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*icom > *jcom) {
	    mesage_("ENTER LINE FACTORS IN THE FOLLOWING FORMAT:", (ftnlen)43)
		    ;
	    mesage_("[ LINE NO.  (OR NEG. SIDE NO., ) FACTOR ]", (ftnlen)41);
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	}
L120:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti1r_(mcom, icom, jcom, cin + 72, &iin[1], &rin[1], &kin[1], &i1, &
		r1, &ifound, (ftnlen)72);
	iii[0] = i1;
	if (ifound > 0) {
	    infact_(ml, ms, &c__1, &r1, iii, &n[19], &n[20], &factor[1], &
		    nlps[1], &ifline[1], &illist[1], &linkl[3], &links[3], &
		    addlnk);
	    goto L120;
	}
/*  TOGGLE THE INTERVAL NUMBERS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'I' || *(unsigned char *)
	    &cin[*icom * 72] == 'i') {
	++(*icom);
	if (*labi) {
	    *labi = FALSE_;
	    mesage_("INTERVAL LABELS - OFF", (ftnlen)21);
	} else {
	    *labi = TRUE_;
	    mesage_("INTERVAL LABELS - ON", (ftnlen)20);
	}
/*  TOGGLE THE NODE BOUNDARY NUMBERS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'N' || *(unsigned char *)
	    &cin[*icom * 72] == 'n') {
	++(*icom);
	if (*lablb) {
	    *lablb = FALSE_;
	    mesage_("LINE BOUNDARY LABELS - OFF", (ftnlen)26);
	} else {
	    *lablb = TRUE_;
	    mesage_("LINE BOUNDARY LABELS - ON", (ftnlen)25);
	}
/*  FLAG LINES TO BE PROCESSED */
    } else if (s_cmp(cin + *icom * 72, "LP", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "lp", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	hxmin = xmin;
	hymin = ymin;
	hxmax = xmax;
	hymax = ymax;
	getmax = TRUE_;
	found = FALSE_;
	flag__ = FALSE_;
	flagd_(mp, &n[18], &linkp[3], &ipoint[1], &flag__);
	flagd_(ml, &n[19], &linkl[3], &iline[1], &flag__);
	flagd_(mr, &n[22], &linkr[3], &iregn[1], &flag__);
	goplot = FALSE_;
	mesage_("PLOT LINES FROM <I1> TO <I2>", (ftnlen)28);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L130:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    if (i1 > 0) {
		check_(&i1, &i2, &n[19]);
/*  FLAG ALL DATA ASSOCIATED WITH THE LINES */
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(ml, &linkl[3], &i__, &kk, &addlnk);
		    if (kk > 0) {
			goplot = TRUE_;
			for (l = 1; l <= 3; ++l) {
			    if (lcon[l + kk * 3] > 0) {
				ltsort_(mp, &linkp[3], &lcon[l + kk * 3], &ll,
					 &addlnk);
				if (ll > 0) {
				    ipoint[ll] = -(i__2 = ipoint[ll], abs(
					    i__2));
				    if (! found) {
					xmax = coor[(ll << 1) + 1];
					xmin = coor[(ll << 1) + 1];
					ymax = coor[(ll << 1) + 2];
					ymin = coor[(ll << 1) + 2];
					found = TRUE_;
				    }
				}
			    }
/* L140: */
			}
			dline_(mp, ml, &coor[3], &linkp[3], &iline[kk], &
				ltype[kk], &lcon[kk * 3 + 1], &lcon[kk * 3 + 
				2], &lcon[kk * 3 + 3], &numplt, &x1, &y1, &
				test, &getmax, &xmin, &xmax, &ymin, &ymax);
			iline[kk] = -(i__2 = iline[kk], abs(i__2));
		    }
/* L150: */
		}
		goto L130;
	    }
	}
	getmax = FALSE_;
	if (goplot) {
/*  PLOT THE LINE DATA THAT HAS BEEN FLAGGED */
	    if (*alpha) {
		mesage_("NO PLOTTING POSSIBLE ON ALPHANUMERIC TERMINAL", (
			ftnlen)45);
	    } else {
		pdata_(mp, ml, mr, msc, &ipoint[1], &coor[3], &ipboun[1], &
			iline[1], &ltype[1], &nint[1], &lcon[4], &factor[1], &
			ilboun[1], &isboun[1], &iregn[1], &imat[1], &linkp[3],
			 &linkl[3], &linkr[3], &linksc[3], &rsize[1], scheme 
			+ 72, defsch, defsiz, &rextrm[5], &n[1], labp, labl, 
			labr, full, labmd, labi, labf, labpb, lablb, labsbd, 
			labsc, labsz, axisd, title, &xmin, &xmax, &ymin, &
			ymax, &xx1, &yy1, &xx2, &yy2, dev1, versn, (ftnlen)72,
			 (ftnlen)72, (ftnlen)72, (ftnlen)3, (ftnlen)9);
		drawn = TRUE_;
	    }
	} else {
	    xmin = hxmin;
	    ymin = hymin;
	    xmax = hxmax;
	    ymax = hymax;
	}
	xmin1 = xmin;
	ymin1 = ymin;
	xmax1 = xmax;
	ymax1 = ymax;
/*  TOGGLE THE LINE NUMBERS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'L' || *(unsigned char *)
	    &cin[*icom * 72] == 'l') {
	++(*icom);
	if (*labl) {
	    *labl = FALSE_;
	    mesage_("LINE LABELS - OFF", (ftnlen)17);
	} else {
	    *labl = TRUE_;
	    mesage_("LINE LABELS - ON", (ftnlen)16);
	}
/*  TOGGLE THE POINT BOUNDARY NUMBERS */
    } else if (s_cmp(cin + *icom * 72, "PB", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "pb", (ftnlen)1, (ftnlen)2) == 0) {
	++(*icom);
	if (*labpb) {
	    *labpb = FALSE_;
	    mesage_("POINT BOUNDARY LABELS - OFF", (ftnlen)27);
	} else {
	    *labpb = TRUE_;
	    mesage_("POINT BOUNDARY LABELS - ON", (ftnlen)26);
	}
/*  TOGGLE THE SIDE BOUNDARY NUMBERS */
    } else if (s_cmp(cin + *icom * 72, "EB", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "eb", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*labsbd) {
	    *labsbd = FALSE_;
	    mesage_("ELEMENT BOUNDARY LABELS - OFF", (ftnlen)29);
	} else {
	    *labsbd = TRUE_;
	    mesage_("ELEMENT BOUNDARY LABELS - ON", (ftnlen)28);
	}
/*  TOGGLE THE POINT NUMBERS */
    } else if (s_cmp(cin + *icom * 72, "PO", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "po", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*labp) {
	    *labp = FALSE_;
	    mesage_("POINT LABELS - OFF", (ftnlen)18);
	} else {
	    *labp = TRUE_;
	    mesage_("POINT LABELS - ON", (ftnlen)17);
	}
/*  TOGGLE THE REGION NUMBERS */
    } else if (s_cmp(cin + *icom * 72, "RE", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "re", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*labr) {
	    *labr = FALSE_;
	    mesage_("REGION LABELS - OFF", (ftnlen)19);
	} else {
	    *labr = TRUE_;
	    mesage_("REGION LABELS - ON", (ftnlen)18);
	}
/*  SPAWN A PROCESS */
    } else if (s_cmp(cin + *icom * 72, "SPA", (ftnlen)3, (ftnlen)3) == 0 || 
	    s_cmp(cin + *icom * 72, "spa", (ftnlen)3, (ftnlen)3) == 0) {
	++(*icom);
	spawn_(vaxvms);
/*  FLAG SIDES TO BE PROCESSED */
    } else if (s_cmp(cin + *icom * 72, "SP", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "sp", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	hxmin = xmin;
	hymin = ymin;
	hxmax = xmax;
	hymax = ymax;
	xmin = (float)1e5;
	ymin = (float)1e5;
	xmax = (float)-1e5;
	ymax = (float)-1e5;
	found = FALSE_;
	getmax = TRUE_;
	flag__ = FALSE_;
	flagd_(mp, &n[18], &linkp[3], &ipoint[1], &flag__);
	flagd_(ml, &n[19], &linkl[3], &iline[1], &flag__);
	flagd_(mr, &n[22], &linkr[3], &iregn[1], &flag__);
	goplot = FALSE_;
	mesage_("PLOT SIDES FROM <I1> TO <I2>", (ftnlen)28);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L160:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    if (i1 > 0) {
		check_(&i1, &i2, &n[20]);
/*  FLAG ALL DATA ASSOCIATED WITH THE SIDES */
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(ms, &links[3], &i__, &jj, &addlnk);
		    if (jj > 0) {
			goplot = TRUE_;
			i__2 = ifline[jj] + nlps[jj] - 1;
			for (k = ifline[jj]; k <= i__2; ++k) {
			    ltsort_(ml, &linkl[3], &illist[k], &kk, &addlnk);
			    if (kk > 0) {
				dline_(mp, ml, &coor[3], &linkp[3], &iline[kk]
					, &ltype[kk], &lcon[kk * 3 + 1], &
					lcon[kk * 3 + 2], &lcon[kk * 3 + 3], &
					numplt, &x1, &y1, &test, &getmax, &
					xmin, &xmax, &ymin, &ymax);
				iline[kk] = -(i__3 = iline[kk], abs(i__3));
				for (l = 1; l <= 3; ++l) {
				    if (lcon[l + kk * 3] > 0) {
					ltsort_(mp, &linkp[3], &lcon[l + kk * 
						3], &ll, &addlnk);
					if (ll > 0) {
					    ipoint[ll] = -(i__3 = ipoint[ll], 
						    abs(i__3));
					    if (! found) {
			  xmax = coor[(ll << 1) + 1];
			  xmin = coor[(ll << 1) + 1];
			  ymax = coor[(ll << 1) + 2];
			  ymin = coor[(ll << 1) + 2];
			  found = TRUE_;
					    }
					}
				    }
/* L170: */
				}
			    }
/* L180: */
			}
		    }
/* L190: */
		}
		goto L160;
	    }
	}
	getmax = FALSE_;
	if (goplot) {
/*  PLOT THE SIDE DATA THAT HAS BEEN FLAGGED */
	    if (*alpha) {
		mesage_("NO PLOTTING POSSIBLE ON ALPHANUMERIC TERMINAL", (
			ftnlen)45);
	    } else {
		pdata_(mp, ml, mr, msc, &ipoint[1], &coor[3], &ipboun[1], &
			iline[1], &ltype[1], &nint[1], &lcon[4], &factor[1], &
			ilboun[1], &isboun[1], &iregn[1], &imat[1], &linkp[3],
			 &linkl[3], &linkr[3], &linksc[3], &rsize[1], scheme 
			+ 72, defsch, defsiz, &rextrm[5], &n[1], labp, labl, 
			labr, full, labmd, labi, labf, labpb, lablb, labsbd, 
			labsc, labsz, axisd, title, &xmin, &xmax, &ymin, &
			ymax, &xx1, &yy1, &xx2, &yy2, dev1, versn, (ftnlen)72,
			 (ftnlen)72, (ftnlen)72, (ftnlen)3, (ftnlen)9);
		drawn = TRUE_;
	    }
	} else {
	    xmin = hxmin;
	    ymin = hymin;
	    xmax = hxmax;
	    ymax = hymax;
	}
	xmin1 = xmin;
	ymin1 = ymin;
	xmax1 = xmax;
	ymax1 = ymax;
/*  SHOW STATUS OF ALL TOGGLES */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'S' || *(unsigned char *)
	    &cin[*icom * 72] == 's') {
	++(*icom);
	mesage_(" ", (ftnlen)1);
	mesage_("THE CURRENT STATUS OF ALL PLOTTING TOGGLES IS:", (ftnlen)46);
	if (*axisd) {
	    mesage_("   AXIS PLOTTING                - ON", (ftnlen)36);
	} else {
	    mesage_("   AXIS PLOTTING                - OFF", (ftnlen)37);
	}
	if (*labp) {
	    mesage_("   LABELING OF POINTS           - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF POINTS           - OFF", (ftnlen)37);
	}
	if (*labpb) {
	    mesage_("   LABELING OF POINBC FLAGS     - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF POINBC FLAGS     - OFF", (ftnlen)37);
	}
	if (*labl) {
	    mesage_("   LABELING OF LINES            - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF LINES            - OFF", (ftnlen)37);
	}
	if (*labi) {
	    mesage_("   LABELING OF LINE INTERVALS   - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF LINE INTERVALS   - OFF", (ftnlen)37);
	}
	if (*labf) {
	    mesage_("   LABELING OF LINE FACTORS     - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF LINE FACTORS     - OFF", (ftnlen)37);
	}
	if (*lablb) {
	    mesage_("   LABELING OF NODEBC FLAGS     - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF NODEBC FLAGS     - OFF", (ftnlen)37);
	}
	if (*labsbd) {
	    mesage_("   LABELING OF ELEMBC FLAGS     - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF ELEMBC FLAGS     - OFF", (ftnlen)37);
	}
	if (*labr) {
	    mesage_("   LABELING OF REGIONS          - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF REGIONS          - OFF", (ftnlen)37);
	}
	if (*labmd) {
	    mesage_("   LABELING OF BLOCK ID  (MAT)   - ON", (ftnlen)37);
	} else {
	    mesage_("   LABELING OF BLOCK ID  (MAT)   - OFF", (ftnlen)38);
	}
	if (*labsc) {
	    mesage_("   LABELING OF REGION SCHEMES   - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF REGION SCHEMES   - OFF", (ftnlen)37);
	}
	if (*labsz) {
	    mesage_("   LABELING OF REGION ELEM SIZE - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF REGION ELEM SIZE - OFF", (ftnlen)37);
	}
	if (*full) {
	    mesage_("   FULL LABELING OF PROPERTIES  - ON", (ftnlen)36);
	} else {
	    mesage_("   FULL LABELING OF PROPERTIES  - OFF", (ftnlen)37);
	}
	mesage_("*-------------------- NOTE -------------------*", (ftnlen)47)
		;
	mesage_("    PLOTTING ORDER AT POINTS IS:               ", (ftnlen)47)
		;
	mesage_("        POINT NO./POINBC FLAG                  ", (ftnlen)47)
		;
	mesage_("    PLOTTING ORDER AT LINE CENTERS IS:         ", (ftnlen)47)
		;
	mesage_("       LINE NO./INTERVALS/FACTORS/NODEBC/ELEMBC", (ftnlen)47)
		;
	mesage_("    PLOTTING ORDER AT REGION CENTERS IS:       ", (ftnlen)47)
		;
	mesage_("        REGION NO./BLOCK ID NO./SCHEME         ", (ftnlen)47)
		;
	mesage_("*-------------------- NOTE -------------------*", (ftnlen)47)
		;
/*  FLAG REGIONS TO BE PROCESSED */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'R' || *(unsigned char *)
	    &cin[*icom * 72] == 'r') {
	++(*icom);
	hxmin = xmin;
	hymin = ymin;
	hxmax = xmax;
	hymax = ymax;
	xmin = (float)1e5;
	ymin = (float)1e5;
	xmax = (float)-1e5;
	ymax = (float)-1e5;
	flag__ = FALSE_;
	flagd_(mp, &n[18], &linkp[3], &ipoint[1], &flag__);
	flagd_(ml, &n[19], &linkl[3], &iline[1], &flag__);
	flagd_(mr, &n[21], &linkb[3], &ibarst[1], &flag__);
	flagd_(mr, &n[22], &linkr[3], &iregn[1], &flag__);
	goplot = FALSE_;
	mesage_("PLOT REGIONS FROM <I1> TO <I2>", (ftnlen)30);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L200:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    if (i1 > 0) {
		check_(&i1, &i2, &n[22]);
/*  FLAG ALL DATA ASSOCIATED WITH THE REGIONS */
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(mr, &linkr[3], &i__, &ii, &addlnk);
		    if (ii > 0) {
/*  FIND THE MAXIMUM AND MINIMUM */
/* Computing MIN */
			r__1 = xmin, r__2 = rextrm[(ii << 2) + 1];
			xmin = dmin(r__1,r__2);
/* Computing MAX */
			r__1 = xmax, r__2 = rextrm[(ii << 2) + 2];
			xmax = dmax(r__1,r__2);
/* Computing MIN */
			r__1 = ymin, r__2 = rextrm[(ii << 2) + 3];
			ymin = dmin(r__1,r__2);
/* Computing MAX */
			r__1 = ymax, r__2 = rextrm[(ii << 2) + 4];
			ymax = dmax(r__1,r__2);
			goplot = TRUE_;
			iregn[ii] = -(i__2 = iregn[ii], abs(i__2));
			i__2 = ifside[ii] + nspr[ii] - 1;
			for (j = ifside[ii]; j <= i__2; ++j) {
/*  FLAG SIDE DATA */
			    if (islist[j] > 0) {
				ltsort_(ms, &links[3], &islist[j], &jj, &
					addlnk);
				if (jj > 0) {
				    i__3 = ifline[jj] + nlps[jj] - 1;
				    for (k = ifline[jj]; k <= i__3; ++k) {
					ltsort_(ml, &linkl[3], &illist[k], &
						kk, &addlnk);
					if (kk > 0) {
					    iline[kk] = -(i__4 = iline[kk], 
						    abs(i__4));
					    for (l = 1; l <= 3; ++l) {
			  if (lcon[l + kk * 3] > 0) {
			      ltsort_(mp, &linkp[3], &lcon[l + kk * 3], &ll, &
				      addlnk);
			      if (ll > 0) {
				  ipoint[ll] = -(i__4 = ipoint[ll], abs(i__4))
					  ;
			      }
			  }
/* L210: */
					    }
					}
/* L220: */
				    }
				}
/*  FLAG LINE DATA */
			    } else {
				jj = (i__3 = islist[j], abs(i__3));
				ltsort_(ml, &linkl[3], &jj, &kk, &addlnk);
				if (kk > 0) {
				    iline[kk] = -(i__3 = iline[kk], abs(i__3))
					    ;
				    for (l = 1; l <= 3; ++l) {
					if (lcon[l + kk * 3] > 0) {
					    ltsort_(mp, &linkp[3], &lcon[l + 
						    kk * 3], &ll, &addlnk);
					    if (ll > 0) {
			  ipoint[ll] = -(i__3 = ipoint[ll], abs(i__3));
					    }
					}
/* L230: */
				    }
				}
			    }
/* L240: */
			}
		    }
/* L250: */
		}
		goto L200;
	    }
	}
	if (goplot) {
/*  PLOT THE REGION DATA THAT HAS BEEN FLAGGED */
	    if (*alpha) {
		mesage_("NO PLOTTING POSSIBLE ON ALPHA-NUMERIC TERMINAL", (
			ftnlen)46);
	    } else {
		pdata_(mp, ml, mr, msc, &ipoint[1], &coor[3], &ipboun[1], &
			iline[1], &ltype[1], &nint[1], &lcon[4], &factor[1], &
			ilboun[1], &isboun[1], &iregn[1], &imat[1], &linkp[3],
			 &linkl[3], &linkr[3], &linksc[3], &rsize[1], scheme 
			+ 72, defsch, defsiz, &rextrm[5], &n[1], labp, labl, 
			labr, full, labmd, labi, labf, labpb, lablb, labsbd, 
			labsc, labsz, axisd, title, &xmin, &xmax, &ymin, &
			ymax, &xx1, &yy1, &xx2, &yy2, dev1, versn, (ftnlen)72,
			 (ftnlen)72, (ftnlen)72, (ftnlen)3, (ftnlen)9);
		drawn = TRUE_;
	    }
	} else {
	    xmin = hxmin;
	    ymin = hymin;
	    xmax = hxmax;
	    ymax = hymax;
	}
	xmin1 = xmin;
	ymin1 = ymin;
	xmax1 = xmax;
	ymax1 = ymax;
/*  FLAG BARSETS TO BE PLOTTED */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'B' || *(unsigned char *)
	    &cin[*icom * 72] == 'b') {
	++(*icom);
	hxmin = xmin;
	hymin = ymin;
	hxmax = xmax;
	hymax = ymax;
	xmin = (float)1e5;
	ymin = (float)1e5;
	xmax = (float)-1e5;
	ymax = (float)-1e5;
	flag__ = FALSE_;
	flagd_(mp, &n[18], &linkp[3], &ipoint[1], &flag__);
	flagd_(ml, &n[19], &linkl[3], &iline[1], &flag__);
	flagd_(mr, &n[21], &linkb[3], &ibarst[1], &flag__);
	flagd_(mr, &n[22], &linkr[3], &iregn[1], &flag__);
	goplot = FALSE_;
	mesage_("PLOT BARSETS FROM <I1> TO <I2>", (ftnlen)30);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L260:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    if (i1 > 0) {
		check_(&i1, &i2, &n[21]);
/*  FLAG ALL LINES ASSOCIATED WITH THE BARSETS */
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(ms, &linkb[3], &i__, &ii, &addlnk);
		    if (ii > 0) {
			goplot = TRUE_;
			ibarst[ii] = -(i__2 = ibarst[ii], abs(i__2));
			i__2 = jfline[ii] + nlpb[ii] - 1;
			for (j = jfline[ii]; j <= i__2; ++j) {
			    jj = (i__3 = jllist[j], abs(i__3));
			    ltsort_(ml, &linkl[3], &jj, &kk, &addlnk);
			    if (kk > 0) {
				iline[kk] = -(i__3 = iline[kk], abs(i__3));
				for (l = 1; l <= 3; ++l) {
				    if (lcon[l + kk * 3] > 0) {
					ltsort_(mp, &linkp[3], &lcon[l + kk * 
						3], &ll, &addlnk);
					if (ll > 0) {
					    ipoint[ll] = -(i__3 = ipoint[ll], 
						    abs(i__3));
/* Computing MIN */
					    r__1 = xmin, r__2 = coor[(ll << 1)
						     + 1];
					    xmin = dmin(r__1,r__2);
/* Computing MAX */
					    r__1 = xmax, r__2 = coor[(ll << 1)
						     + 1];
					    xmax = dmax(r__1,r__2);
/* Computing MIN */
					    r__1 = ymin, r__2 = coor[(ll << 1)
						     + 2];
					    ymin = dmin(r__1,r__2);
/* Computing MAX */
					    r__1 = ymax, r__2 = coor[(ll << 1)
						     + 2];
					    ymax = dmax(r__1,r__2);
					}
				    }
/* L270: */
				}
			    }
/* L280: */
			}
		    }
/* L290: */
		}
		goto L260;
	    }
	}
	if (goplot) {
/*  PLOT THE BARSET DATA THAT HAS BEEN FLAGGED */
	    if (*alpha) {
		mesage_("NO PLOTTING POSSIBLE ON ALPHANUMERIC TERMINAL", (
			ftnlen)45);
	    } else {
		pdata_(mp, ml, mr, msc, &ipoint[1], &coor[3], &ipboun[1], &
			iline[1], &ltype[1], &nint[1], &lcon[4], &factor[1], &
			ilboun[1], &isboun[1], &iregn[1], &imat[1], &linkp[3],
			 &linkl[3], &linkr[3], &linksc[3], &rsize[1], scheme 
			+ 72, defsch, defsiz, &rextrm[5], &n[1], labp, labl, 
			labr, full, labmd, labi, labf, labpb, lablb, labsbd, 
			labsc, labsz, axisd, title, &xmin, &xmax, &ymin, &
			ymax, &xx1, &yy1, &xx2, &yy2, dev1, versn, (ftnlen)72,
			 (ftnlen)72, (ftnlen)72, (ftnlen)3, (ftnlen)9);
		drawn = TRUE_;
	    }
	} else {
	    xmin = hxmin;
	    ymin = hymin;
	    xmax = hxmax;
	    ymax = hymax;
	}
	xmin1 = xmin;
	ymin1 = ymin;
	xmax1 = xmax;
	ymax1 = ymax;
/*  ENTER ZOOM LOCATION */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'Z' || *(unsigned char *)
	    &cin[*icom * 72] == 'z') {
	++(*icom);
	zoomlt_(mcom, icom, jcom, cin + 72, &rin[1], &iin[1], &kin[1], idump, 
		&drawn, alpha, dev1, &x1, &x2, &y1, &y2, &xx1, &xx2, &yy1, &
		yy2, &xmin1, &xmax1, &ymin1, &ymax1, &xmin, &xmax, &ymin, &
		ymax, (ftnlen)72, (ftnlen)3);
	drawn = FALSE_;
/*  RETURN FROM DATA PLOTTING */
    } else if (*(unsigned char *)&cin[*icom * 72] == ' ') {
	++(*icom);
	flag__ = FALSE_;
	flagd_(mp, &n[18], &linkp[3], &ipoint[1], &flag__);
	flagd_(ml, &n[19], &linkl[3], &iline[1], &flag__);
	flagd_(ms, &n[21], &linkb[3], &ibarst[1], &flag__);
	flagd_(mr, &n[22], &linkr[3], &iregn[1], &flag__);
	return 0;
/* GENERATE A HARDCOPY QMS PLOT */
    } else if ((*(unsigned char *)&cin[*icom * 72] == 'H' || s_cmp(cin + *
	    icom * 72, "h", (ftnlen)2, (ftnlen)1) == 0) && *(unsigned char *)&
	    cin[*icom * 72 + 1] != 'E' && *(unsigned char *)&cin[*icom * 72 + 
	    1] != 'e') {
	++(*icom);
	vdiqes_(&c__10002, &kaval2);
	if (kaval2 == 1) {
	    if (! (*alpha)) {
		vdescp_(&c__10002, &c__0, &c__0);
	    }
	    pdata_(mp, ml, mr, msc, &ipoint[1], &coor[3], &ipboun[1], &iline[
		    1], &ltype[1], &nint[1], &lcon[4], &factor[1], &ilboun[1],
		     &isboun[1], &iregn[1], &imat[1], &linkp[3], &linkl[3], &
		    linkr[3], &linksc[3], &rsize[1], scheme + 72, defsch, 
		    defsiz, &rextrm[5], &n[1], labp, labl, labr, full, labmd, 
		    labi, labf, labpb, lablb, labsbd, labsc, labsz, axisd, 
		    title, &xmin, &xmax, &ymin, &ymax, &xx1, &yy1, &xx2, &yy2,
		     "XXX", versn, (ftnlen)72, (ftnlen)72, (ftnlen)72, (
		    ftnlen)3, (ftnlen)9);
	    if (! (*alpha)) {
		vdescp_(&c__10001, &c__0, &c__0);
	    }
	    mesage_("HARDCOPY PLOT GENERATED", (ftnlen)23);
	    *hardpl = TRUE_;
	} else {
	    mesage_("HARDCOPY DEVICE NOT AVAILABLE", (ftnlen)29);
	}
/*  PLOT THE CURRENT ACTIVE ITEMS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned char *)
	    &cin[*icom * 72] == 'p') {
	++(*icom);
	if (*alpha) {
	    mesage_("NO PLOTTING POSSIBLE ON ALPHANUMERIC TERMINAL", (ftnlen)
		    45);
	} else {
	    pdata_(mp, ml, mr, msc, &ipoint[1], &coor[3], &ipboun[1], &iline[
		    1], &ltype[1], &nint[1], &lcon[4], &factor[1], &ilboun[1],
		     &isboun[1], &iregn[1], &imat[1], &linkp[3], &linkl[3], &
		    linkr[3], &linksc[3], &rsize[1], scheme + 72, defsch, 
		    defsiz, &rextrm[5], &n[1], labp, labl, labr, full, labmd, 
		    labi, labf, labpb, lablb, labsbd, labsc, labsz, axisd, 
		    title, &xmin, &xmax, &ymin, &ymax, &xx1, &yy1, &xx2, &yy2,
		     dev1, versn, (ftnlen)72, (ftnlen)72, (ftnlen)72, (ftnlen)
		    3, (ftnlen)9);
	    drawn = TRUE_;
	}
/*  EXIT OPTION - EXITS FASTQ */
    } else if (s_cmp(cin + *icom * 72, "EX", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ex", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	strlng_(cin + *icom * 72, &len, (ftnlen)72);
	if (len > 1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'X' || len > 
		1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'x') {
	    help_fq__(&c__5);
	} else {
	    fexit_(wrote, mcom, icom, jcom, cin + 72, &iin[1], &rin[1], &kin[
		    1], time1, batch, versn, (ftnlen)72, (ftnlen)9);
	}
	goto L100;
/*  WRITE OUT THE HELP MESSAGE */
    } else {
	++(*icom);
	help_fq__(&c__5);
    }
    goto L100;
} /* gdata_ */

#ifdef __cplusplus
	}
#endif

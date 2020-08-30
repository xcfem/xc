/* gmesh.f -- translated by f2c (version 20160102).
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
static integer c__11 = 11;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int gmesh_(integer *npnode, integer *npelem, integer *mxnflg,
	 integer *mxsflg, integer *npnbc, integer *npsbc, integer *maxkxn, 
	integer *mr, integer *npregn, integer *mcom, integer *icom, integer *
	jcom, char *cin, real *rin, integer *iin, integer *kin, integer *nnn, 
	integer *kkk, integer *nummat, integer *nnxk, integer *ipart, integer 
	*nodes, integer *nnflg, integer *nnptr, integer *nsflg, integer *
	nvptr, integer *nvlen, integer *nsiden, integer *mapdxg, real *xn, 
	real *yn, integer *nxk, integer *mat, integer *ilook, integer *mapgxd,
	 real *centk, integer *matmap, real *wtnode, real *wtside, integer *
	nbcnod, integer *nnlist, integer *nbcsid, integer *nvlist, char *
	title, integer *idump, logical *axis, logical *areacg, logical *labe, 
	logical *labo, logical *labn, logical *labnb, logical *labsb, logical 
	*labm, logical *labw, integer *idev, logical *alpha, char *dev1, 
	logical *eight, logical *nine, logical *vaxvms, char *versn, logical *
	wrote, real *time1, logical *hardpl, logical *batch, ftnlen cin_len, 
	ftnlen title_len, ftnlen dev1_len, ftnlen versn_len)
{
    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2;

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, i1, i2;
    extern /* Subroutine */ int minmax_fq__(integer *, integer *, real *, 
	    real *, real *, real *, real *, real *);
    static real x1, x2, y1, y2;
    static integer im, iz;
    static real xx1, xx2, yy1, yy2;
    static logical old;
    static integer len;
    static real xmin, ymin, xmax, ymax, xmin1, ymin1, xmax1, ymax1;
    extern /* Subroutine */ int check_(integer *, integer *, integer *), 
	    flagk_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *), geti12_(integer *, integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    integer *, ftnlen);
    static logical drawn;
    extern /* Subroutine */ int pmesh_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, integer *, integer *, integer *, integer *, real *
	    , real *, logical *, logical *, logical *, logical *, logical *, 
	    logical *, logical *, logical *, logical *, real *, integer *, 
	    real *, real *, real *, real *, real *, real *, real *, real *, 
	    char *, char *, logical *, logical *, char *, logical *, ftnlen, 
	    ftnlen, ftnlen), fexit_(logical *, integer *, integer *, integer *
	    , char *, integer *, real *, integer *, real *, logical *, char *,
	     ftnlen, ftnlen), spawn_(logical *), mnmxk_(integer *, integer *, 
	    integer *, integer *, real *, real *, real *, integer *, real *, 
	    real *, real *, real *);
    static integer kaval2;
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen);
    static logical setflg;
    static integer ifound;
    extern /* Subroutine */ int vdescp_(integer *, integer *, integer *), 
	    vdiqes_(integer *, integer *);
    static logical regplt;
    static integer iostat;
    static logical goplot;
    extern /* Subroutine */ int pltflu_(), strlng_(char *, integer *, ftnlen),
	     zoomlt_(integer *, integer *, integer *, char *, real *, integer 
	    *, integer *, integer *, logical *, logical *, char *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, ftnlen, 
	    ftnlen), help_fq__(integer *);

/* *********************************************************************** */
/*  SUBROUTINE GMESH = SETS UP GRAPHICS FOR THE GENERATED MESH */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     MESH = ALLOWS MESH GENERATION AND DISPLAY */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     TITLE  = MESH TITLE */
/*     LABE   = .TRUE. IF ELEMENT NUMBERS ARE TO BE PLOTTED */
/*     LABN   = .TRUE. IF NODE NUMBERS ARE TO BE PLOTTED */
/*     LABNB  = .TRUE. IF NODE BOUNDARY NUMBERS ARE TO BE PLOTTED */
/*     LABSB  = .TRUE. IF ELEMENT BOUNDARY NUMBERS ARE TO BE PLOTTED */
/*     LABM   = .TRUE. IF MATERIAL NUMBERS ARE TO BE PLOTTED */
/*     AXIS   = .TRUE. IF THE AXIS IS TO BE DRAWN */
/*     AREACG = .TRUE. IF THE AREA AND C.G. ARE CALCULATED AND THE C.G. */
/*              IS DISPLAYED */
/* *********************************************************************** */
    /* Parameter adjustments */
    --mapgxd;
    --yn;
    --xn;
    --mapdxg;
    centk -= 3;
    --mat;
    --nnptr;
    --nnflg;
    --nvlen;
    --nvptr;
    --nsflg;
    --wtnode;
    --nodes;
    --wtside;
    --nsiden;
    matmap -= 4;
    ipart -= 4;
    --kin;
    --iin;
    --rin;
    cin -= 72;
    --ilook;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;
    --idev;

    /* Function Body */
    iz = 0;
/*  CALCULATE THE CENTER OF EACH ELEMENT FOR CLIPPING CONSIDERATIONS */
    i__1 = *kkk;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nxk[i__ * nxk_dim1 + 3] == 0) {
	    centk[(i__ << 1) + 1] = (xn[nxk[i__ * nxk_dim1 + 1]] + xn[nxk[i__ 
		    * nxk_dim1 + 2]]) * (float).5;
	    centk[(i__ << 1) + 2] = (yn[nxk[i__ * nxk_dim1 + 1]] + yn[nxk[i__ 
		    * nxk_dim1 + 2]]) * (float).5;
	} else if (nxk[i__ * nxk_dim1 + 4] == 0) {
	    centk[(i__ << 1) + 1] = (xn[nxk[i__ * nxk_dim1 + 1]] + xn[nxk[i__ 
		    * nxk_dim1 + 2]] + xn[nxk[i__ * nxk_dim1 + 3]]) * (float)
		    .33333;
	    centk[(i__ << 1) + 2] = (yn[nxk[i__ * nxk_dim1 + 1]] + yn[nxk[i__ 
		    * nxk_dim1 + 2]] + yn[nxk[i__ * nxk_dim1 + 3]]) * (float)
		    .33333;
	} else if (*eight || *nine) {
	    centk[(i__ << 1) + 1] = (xn[nxk[i__ * nxk_dim1 + 1]] + xn[nxk[i__ 
		    * nxk_dim1 + 5]]) * (float).5;
	    centk[(i__ << 1) + 2] = (yn[nxk[i__ * nxk_dim1 + 1]] + yn[nxk[i__ 
		    * nxk_dim1 + 5]]) * (float).5;
	} else {
	    centk[(i__ << 1) + 1] = (xn[nxk[i__ * nxk_dim1 + 1]] + xn[nxk[i__ 
		    * nxk_dim1 + 3]]) * (float).5;
	    centk[(i__ << 1) + 2] = (yn[nxk[i__ * nxk_dim1 + 1]] + yn[nxk[i__ 
		    * nxk_dim1 + 3]]) * (float).5;
	}
/* L100: */
    }
    drawn = FALSE_;
/*  FIND THE BODY MIN AND MAX */
    minmax_fq__(npnode, nnn, &xn[1], &yn[1], &xmin, &xmax, &ymin, &ymax);
    xmin1 = xmin;
    xmax1 = xmax;
    ymin1 = ymin;
    ymax1 = ymax;
/*  ENTER GRAPHICS OPTION */
L110:
    if (*icom > *jcom) {
	mesage_(" ", (ftnlen)1);
	frefld_(&iz, &iz, "ENTER MESH GRAPHICS OPTION: ", mcom, &iostat, jcom,
		 &kin[1], cin + 72, &iin[1], &rin[1], (ftnlen)28, (ftnlen)72);
	*icom = 1;
    }
/*  TOGGLE DRAWING OF THE AXIS */
    if (*(unsigned char *)&cin[*icom * 72] == 'A' || *(unsigned char *)&cin[*
	    icom * 72] == 'a') {
	++(*icom);
	if (*axis) {
	    *axis = FALSE_;
	    mesage_("AXIS DRAWING - OFF", (ftnlen)18);
	} else {
	    *axis = TRUE_;
	    mesage_("AXIS DRAWING - ON", (ftnlen)17);
	}
/*  TOGGLE CALCULATION OF AREA AND C.G. */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'C' || *(unsigned char *)
	    &cin[*icom * 72] == 'c') {
	++(*icom);
	if (*areacg) {
	    *areacg = FALSE_;
	    mesage_("AREA AND C.G. REPORT - OFF", (ftnlen)26);
	} else {
	    *areacg = TRUE_;
	    mesage_("AREA AND C.G. REPORT - ON", (ftnlen)25);
	}
/*  TOGGLE NODAL BOUNDARY DISPLAY */
    } else if (s_cmp(cin + *icom * 72, "NB", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "nb", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*labnb) {
	    *labnb = FALSE_;
	    mesage_("NODAL BOUNDARY DISPLAY - OFF", (ftnlen)28);
	} else {
	    *labnb = TRUE_;
	    mesage_("NODAL BOUNDARY DISPLAY - ON", (ftnlen)27);
	}
/*  TOGGLE ELEMENT SIDE BOUNDARY DISPLAY */
    } else if (s_cmp(cin + *icom * 72, "EB", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "eb", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*labsb) {
	    *labsb = FALSE_;
	    mesage_("ELEMENT SIDE BOUNDARY DISPLAY - OFF", (ftnlen)35);
	} else {
	    *labsb = TRUE_;
	    mesage_("ELEMENT SIDE BOUNDARY DISPLAY - ON", (ftnlen)34);
	}
/*  TOGGLE WEIGHTING FACTOR DISPLAY */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'W' || *(unsigned char *)
	    &cin[*icom * 72] == 'w') {
	++(*icom);
	if (*labw) {
	    *labw = FALSE_;
	    mesage_("BOUNDARY WEIGHTING DISPLAY - OFF", (ftnlen)32);
	} else {
	    *labw = TRUE_;
	    mesage_("BOUNDARY WEIGHTING DISPLAY - ON", (ftnlen)31);
	}
/*  TOGGLE ELEMENT NUMBERING */
    } else if (s_cmp(cin + *icom * 72, "EN", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "en", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*labe) {
	    *labe = FALSE_;
	    mesage_("ELEMENT NUMBERS - OFF", (ftnlen)21);
	} else {
	    *labe = TRUE_;
	    mesage_("ELEMENT NUMBERS - ON", (ftnlen)20);
	    *labo = FALSE_;
	}
/*  TOGGLE NODE NUMBERING */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'N' || *(unsigned char *)
	    &cin[*icom * 72] == 'n') {
	++(*icom);
	if (*labn) {
	    *labn = FALSE_;
	    mesage_("NODE NUMBERS - OFF", (ftnlen)18);
	} else {
	    *labn = TRUE_;
	    mesage_("NODE NUMBERS - ON", (ftnlen)17);
	}
/*  TOGGLE MATERIAL NUMBER DISPLAY */
    } else if (s_cmp(cin + *icom * 72, "MN", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "mn", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*labm) {
	    *labm = FALSE_;
	    mesage_("MATERIAL NUMBERING - OFF", (ftnlen)24);
	} else {
	    *labm = TRUE_;
	    mesage_("MATERIAL NUMBERING - ON", (ftnlen)23);
	}
/*  TOGGLE OPTIMIZED ORDER DISPLAY */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'O' || *(unsigned char *)
	    &cin[*icom * 72] == 'o') {
	++(*icom);
	if (*labo) {
	    *labo = FALSE_;
	    mesage_("OPTIMIZED ORDER NUMBERING - OFF", (ftnlen)31);
	} else {
	    *labo = TRUE_;
	    mesage_("OPTIMIZER ORDER NUMBERING - ON", (ftnlen)30);
	    *labe = FALSE_;
	}
/*  PLOT ALL ACTIVE ELEMENTS  (ZOOM STILL APPLIES) */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned char *)
	    &cin[*icom * 72] == 'p') {
	++(*icom);
	if (*alpha) {
	    mesage_("NO PLOTTING POSSIBLE ON ALPHANUMERIC TERMINAL", (ftnlen)
		    45);
	} else {
	    pmesh_(npnode, npelem, mxnflg, mxsflg, npnbc, npsbc, maxkxn, nnn, 
		    kkk, nnxk, nbcnod, nbcsid, nnlist, nvlist, &nodes[1], &
		    nsiden[1], &nnflg[1], &nnptr[1], &nsflg[1], &nvptr[1], &
		    nvlen[1], &xn[1], &yn[1], &nxk[nxk_offset], &mat[1], &
		    mapdxg[1], &mapgxd[1], &wtnode[1], &wtside[1], axis, 
		    areacg, labe, labo, labn, labnb, labsb, labm, labw, &
		    centk[3], &ilook[1], &xmin, &xmax, &ymin, &ymax, &xx1, &
		    xx2, &yy1, &yy2, title, dev1, eight, nine, versn, vaxvms, 
		    (ftnlen)72, (ftnlen)3, (ftnlen)9);
	    drawn = TRUE_;
	}
/*  PLOT A LIMITED NUMBER OF ELEMENTS BY ELEMENT NUMBER */
    } else if (s_cmp(cin + *icom * 72, "EP", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ep", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	setflg = FALSE_;
	if (*labo) {
	    old = FALSE_;
	} else {
	    old = TRUE_;
	}
	flagk_(npelem, nnxk, &nxk[nxk_offset], &mapdxg[1], &c__1, kkk, &
		setflg, &old);
	setflg = TRUE_;
	goplot = FALSE_;
	mesage_("PLOT ELEMENTS FROM <I1> TO <I2>", (ftnlen)31);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L120:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    if (i1 > 0) {
		check_(&i1, &i2, kkk);
		flagk_(npelem, nnxk, &nxk[nxk_offset], &mapdxg[1], &i1, &i2, &
			setflg, &old);
		goplot = TRUE_;
		goto L120;
	    }
	}
	if (goplot) {
	    if (*alpha) {
		mesage_("NO PLOTTING POSSIBLE ON ALPHANUMERIC  TERMINAL", (
			ftnlen)46);
	    } else {
		mnmxk_(npelem, npnode, nnxk, &nxk[nxk_offset], &xn[1], &yn[1],
			 &centk[3], kkk, &xmin, &xmax, &ymin, &ymax);
		pmesh_(npnode, npelem, mxnflg, mxsflg, npnbc, npsbc, maxkxn, 
			nnn, kkk, nnxk, nbcnod, nbcsid, nnlist, nvlist, &
			nodes[1], &nsiden[1], &nnflg[1], &nnptr[1], &nsflg[1],
			 &nvptr[1], &nvlen[1], &xn[1], &yn[1], &nxk[
			nxk_offset], &mat[1], &mapdxg[1], &mapgxd[1], &wtnode[
			1], &wtside[1], axis, areacg, labe, labo, labn, labnb,
			 labsb, labm, labw, &centk[3], &ilook[1], &xmin, &
			xmax, &ymin, &ymax, &xx1, &xx2, &yy1, &yy2, title, 
			dev1, eight, nine, versn, vaxvms, (ftnlen)72, (ftnlen)
			3, (ftnlen)9);
		drawn = TRUE_;
	    }
	}
/*  PLOT ELEMENTS BY REGION (S) OR BARSET (S) CHOSEN */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'R' || *(unsigned char *)
	    &cin[*icom * 72] == 'r' || *(unsigned char *)&cin[*icom * 72] == 
	    'B' || *(unsigned char *)&cin[*icom * 72] == 'b') {
	setflg = FALSE_;
	old = TRUE_;
	flagk_(npelem, nnxk, &nxk[nxk_offset], &mapdxg[1], &c__1, kkk, &
		setflg, &old);
	setflg = TRUE_;
	goplot = FALSE_;
	if (*(unsigned char *)&cin[*icom * 72] == 'R' || *(unsigned char *)&
		cin[*icom * 72] == 'r') {
	    mesage_("PLOT REGIONS FROM <I1> TO <I2>", (ftnlen)30);
	    regplt = TRUE_;
	} else {
	    mesage_("PLOT BARSETS FROM <I1> TO <I2>", (ftnlen)30);
	    regplt = FALSE_;
	}
	++(*icom);
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
		if (i2 < i1) {
		    i2 = i1;
		}
		i__1 = *npregn;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    if (ipart[i__ * 3 + 1] >= i1 && ipart[i__ * 3 + 1] <= i2) 
			    {
			if (regplt && nxk[ipart[i__ * 3 + 2] * nxk_dim1 + 4] 
				> 0 || nxk[ipart[i__ * 3 + 2] * nxk_dim1 + 4] 
				== 0) {
			    flagk_(npelem, nnxk, &nxk[nxk_offset], &mapdxg[1],
				     &ipart[i__ * 3 + 2], &ipart[i__ * 3 + 3],
				     &setflg, &old);
			    goplot = TRUE_;
			}
		    }
/* L140: */
		}
		goto L130;
	    }
	}
	if (goplot) {
	    if (*alpha) {
		mesage_("NO PLOTTING POSSIBLE ON ALPHANUMERIC TERMINAL", (
			ftnlen)45);
	    } else {
		mnmxk_(npelem, npnode, nnxk, &nxk[nxk_offset], &xn[1], &yn[1],
			 &centk[3], kkk, &xmin, &xmax, &ymin, &ymax);
		pmesh_(npnode, npelem, mxnflg, mxsflg, npnbc, npsbc, maxkxn, 
			nnn, kkk, nnxk, nbcnod, nbcsid, nnlist, nvlist, &
			nodes[1], &nsiden[1], &nnflg[1], &nnptr[1], &nsflg[1],
			 &nvptr[1], &nvlen[1], &xn[1], &yn[1], &nxk[
			nxk_offset], &mat[1], &mapdxg[1], &mapgxd[1], &wtnode[
			1], &wtside[1], axis, areacg, labe, labo, labn, labnb,
			 labsb, labm, labw, &centk[3], &ilook[1], &xmin, &
			xmax, &ymin, &ymax, &xx1, &xx2, &yy1, &yy2, title, 
			dev1, eight, nine, versn, vaxvms, (ftnlen)72, (ftnlen)
			3, (ftnlen)9);
		drawn = TRUE_;
		xmin1 = xmin;
		xmax1 = xmax;
		ymin1 = ymin;
		ymax1 = ymax;
	    }
	}
/*  PLOT ELEMENTS BY MATERIAL NUMBER */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'M' || *(unsigned char *)
	    &cin[*icom * 72] == 'm') {
	++(*icom);
	setflg = FALSE_;
	old = TRUE_;
	flagk_(npelem, nnxk, &nxk[nxk_offset], &mapdxg[1], &c__1, kkk, &
		setflg, &old);
	goplot = FALSE_;
	setflg = TRUE_;
	mesage_("PLOT MATERIALS FROM <I1> TO <I2>", (ftnlen)32);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L150:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    if (i1 > 0) {
		if (i2 < i1) {
		    i2 = i1;
		}
		i__1 = i2;
		for (im = i1; im <= i__1; ++im) {
		    i__2 = *nummat;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			if (im == matmap[i__ * 3 + 1]) {
			    flagk_(npelem, nnxk, &nxk[nxk_offset], &mapdxg[1],
				     &matmap[i__ * 3 + 2], &matmap[i__ * 3 + 
				    3], &setflg, &old);
			    goplot = TRUE_;
			}
/* L160: */
		    }
/* L170: */
		}
		goto L150;
	    }
	}
	if (goplot) {
	    if (*alpha) {
		mesage_("NO PLOTTING POSSIBLE ON ALPHANUMERIC TERMINAL", (
			ftnlen)45);
	    } else {
		mnmxk_(npelem, npnode, nnxk, &nxk[nxk_offset], &xn[1], &yn[1],
			 &centk[3], kkk, &xmin, &xmax, &ymin, &ymax);
		pmesh_(npnode, npelem, mxnflg, mxsflg, npnbc, npsbc, maxkxn, 
			nnn, kkk, nnxk, nbcnod, nbcsid, nnlist, nvlist, &
			nodes[1], &nsiden[1], &nnflg[1], &nnptr[1], &nsflg[1],
			 &nvptr[1], &nvlen[1], &xn[1], &yn[1], &nxk[
			nxk_offset], &mat[1], &mapdxg[1], &mapgxd[1], &wtnode[
			1], &wtside[1], axis, areacg, labe, labo, labn, labnb,
			 labsb, labm, labw, &centk[3], &ilook[1], &xmin, &
			xmax, &ymin, &ymax, &xx1, &xx2, &yy1, &yy2, title, 
			dev1, eight, nine, versn, vaxvms, (ftnlen)72, (ftnlen)
			3, (ftnlen)9);
		drawn = TRUE_;
	    }
	}
/*  SPAWN A PROCESS */
    } else if (s_cmp(cin + *icom * 72, "SP", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "sp", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	spawn_(vaxvms);
/*  SHOW STATUS OF ALL TOGGLES */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'S' || *(unsigned char *)
	    &cin[*icom * 72] == 's') {
	++(*icom);
	mesage_(" ", (ftnlen)1);
	mesage_("THE CURRENT STATUS OF ALL PLOTTING TOGGLES IS:", (ftnlen)46);
	if (*axis) {
	    mesage_("   AXIS PLOTTING                - ON", (ftnlen)36);
	} else {
	    mesage_("   AXIS PLOTTING                - OFF", (ftnlen)37);
	}
	if (*areacg) {
	    mesage_("   AREA AND C.G. REPORT         - ON", (ftnlen)36);
	} else {
	    mesage_("   AREA AND C.G. REPORT         - OFF", (ftnlen)37);
	}
	if (*labn) {
	    mesage_("   LABELING OF NODES            -  ON", (ftnlen)37);
	} else {
	    mesage_("   LABELING OF NODES            -  OFF", (ftnlen)38);
	}
	if (*labnb) {
	    mesage_("   LABELING OF NODAL BOUND.     -  ON", (ftnlen)37);
	} else {
	    mesage_("   LABELING OF NODAL BOUND.     -  OFF", (ftnlen)38);
	}
	if (*labw) {
	    mesage_("   LABELING OF NODE WEIGHTING   -  ON", (ftnlen)37);
	} else {
	    mesage_("   LABELING OF NODE WEIGHTING   -  OFF", (ftnlen)38);
	}
	if (*labe) {
	    mesage_("   LABELING OF ELEMENTS         -  ON", (ftnlen)37);
	} else {
	    mesage_("   LABELING OF ELEMENTS         -  OFF", (ftnlen)38);
	}
	if (*labsb) {
	    mesage_("   LABELING OF ELEM SIDE BOUND. - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF ELEM SIDE BOUND. - OFF", (ftnlen)37);
	}
	if (*labm) {
	    mesage_("   LABELING OF BLOCK ID  (MAT)  - ON", (ftnlen)36);
	} else {
	    mesage_("   LABELING OF BLOCK ID  (MAT)  - OFF", (ftnlen)37);
	}
	mesage_(" ", (ftnlen)1);
	mesage_("*----------------- NOTE -----------------*", (ftnlen)42);
	mesage_("    PLOTTING ORDER AT NODES IS:           ", (ftnlen)42);
	mesage_("        NODE NO./NODAL BOUND. FLAG/WEIGHT ", (ftnlen)42);
	mesage_("    PLOTTING ORDER AT ELEMENT CENTER IS:  ", (ftnlen)42);
	mesage_("        ELEMENT NO./BLOCK ID  (MAT) NO.    ", (ftnlen)43);
	mesage_("*----------------- NOTE -----------------*", (ftnlen)42);
/*  GET A QMS PLOT FILE OF THE CURRENT SCREEN */
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
	    pmesh_(npnode, npelem, mxnflg, mxsflg, npnbc, npsbc, maxkxn, nnn, 
		    kkk, nnxk, nbcnod, nbcsid, nnlist, nvlist, &nodes[1], &
		    nsiden[1], &nnflg[1], &nnptr[1], &nsflg[1], &nvptr[1], &
		    nvlen[1], &xn[1], &yn[1], &nxk[nxk_offset], &mat[1], &
		    mapdxg[1], &mapgxd[1], &wtnode[1], &wtside[1], axis, 
		    areacg, labe, labo, labn, labnb, labsb, labm, labw, &
		    centk[3], &ilook[1], &xmin, &xmax, &ymin, &ymax, &xx1, &
		    xx2, &yy1, &yy2, title, "XXX", eight, nine, versn, vaxvms,
		     (ftnlen)72, (ftnlen)3, (ftnlen)9);
	    if (! (*alpha)) {
		pltflu_();
		vdescp_(&c__10001, &c__0, &c__0);
	    }
	    mesage_("HARDCOPY PLOT GENERATED", (ftnlen)23);
	    *hardpl = TRUE_;
	} else {
	    mesage_("HARDCOPY DEVICE NOT AVAILABLE", (ftnlen)29);
	}
/*  ENTER ZOOM LOCATION */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'Z' || *(unsigned char *)
	    &cin[*icom * 72] == 'z') {
	++(*icom);
	zoomlt_(mcom, icom, jcom, cin + 72, &rin[1], &iin[1], &kin[1], idump, 
		&drawn, alpha, dev1, &x1, &x2, &y1, &y2, &xx1, &xx2, &yy1, &
		yy2, &xmin1, &xmax1, &ymin1, &ymax1, &xmin, &xmax, &ymin, &
		ymax, (ftnlen)72, (ftnlen)3);
	drawn = FALSE_;
/*  EXIT OPTION - EXITS FASTQ */
    } else if (s_cmp(cin + *icom * 72, "EX", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ex", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	strlng_(cin + *icom * 72, &len, (ftnlen)72);
	if (len > 1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'X' || len > 
		1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'x') {
	    help_fq__(&c__11);
	} else {
	    fexit_(wrote, mcom, icom, jcom, cin + 72, &iin[1], &rin[1], &kin[
		    1], time1, batch, versn, (ftnlen)72, (ftnlen)9);
	}
	goto L110;
/*  RETURN TO MESH ROUTINE */
    } else if (*(unsigned char *)&cin[*icom * 72] == ' ') {
	++(*icom);
	i__1 = *kkk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nxk[i__ * nxk_dim1 + 1] = (i__2 = nxk[i__ * nxk_dim1 + 1], abs(
		    i__2));
/* L180: */
	}
	return 0;
/*  GET HELP MESSAGE */
    } else {
	++(*icom);
	help_fq__(&c__11);
    }
    goto L110;
} /* gmesh_ */

#ifdef __cplusplus
	}
#endif

/* paving.f -- translated by f2c (version 20160102).
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
    real timea, timep, timec, timepc, timeaj, times;
} timing_;

#define timing_1 timing_

/* Table of constant values */

static integer c__10 = 10;
static integer c__1024 = 1024;
static integer c__10240 = 10240;
static integer c__20 = 20;
static logical c_true = TRUE_;
static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int paving_(integer *nbnode, integer *nprm, integer *mln, 
	integer *iptper, integer *numper, integer *lperim, real *xn, real *yn,
	 real *zn, integer *iexk, integer *inxe, integer *nnn, integer *lll, 
	integer *kkk, integer *mxnd, real *angle, real *bnsize, integer *
	lnodes, integer *linkpr, integer *nperim, integer *lxk, integer *kxl, 
	integer *nxl, integer *lxn, integer *nuid, integer *iavail, integer *
	navail, logical *graph, logical *timer, logical *video, real *defsiz, 
	logical *sizeit, char *dev1, integer *kreg, logical *batch, logical *
	noroom, logical *err, real *amesur, real *xnold, real *ynold, integer 
	*nxkold, integer *mmpold, integer *linkeg, integer *listeg, real *
	bmesur, integer *mlink, integer *nprold, integer *npnold, integer *
	npeold, integer *nnxk, logical *remesh, real *rexmin, real *rexmax, 
	real *reymin, real *reymax, integer *idivis, real *sizmin, real *emax,
	 real *emin, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, nxkold_dim1, nxkold_offset, i__1, 
	    i__2;
    real r__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, n0, n1, nnn2, nend;
    static logical done;
    static real xmin, xmax, ymin, ymax, zmin, zmax;
    static integer nadj1, nadj2, node1;
    static real time1, time2, xmin1, ymin1, xmax1, ymax1, zmin1, zmax1;
    static integer icomb[10240]	/* was [10][1024] */;
    extern /* Subroutine */ int pinch_(integer *, integer *, integer *, 
	    integer *, real *, real *, real *, integer *, integer *, integer *
	    , integer *, real *, integer *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    real *, real *, real *, real *, real *, real *, char *, integer *,
	     integer *, integer *, integer *, integer *, integer *, integer *,
	     logical *, logical *, integer *, logical *, logical *, ftnlen);
    static integer lcorn[10], ncorn, kloop, nloop[20], itype[1024], iuppr;
    extern /* Subroutine */ int close4_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *), close6_(integer *, 
	    integer *, integer *, integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, real *, real *, real *, real *, char *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *, logical *, logical *, logical *, real *, real *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, logical *, real *, real *, real *, real *, 
	    integer *, real *, real *, real *, ftnlen);
    static integer nextn1[20];
    static logical adjted;
    extern /* Subroutine */ int mesage_(char *, ftnlen), getime_(real *), 
	    ringbl_();
    static integer kkkold;
    extern /* Subroutine */ int addrow_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, real *, integer *
	    , integer *, integer *, integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, real *, real *, real *, char *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *, integer *, logical *, logical *, 
	    logical *, logical *, real *, real *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    logical *, real *, real *, real *, real *, integer *, real *, 
	    real *, real *, ftnlen);
    extern logical cpubrk_(logical *);
    extern /* Subroutine */ int periml_(integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, real *, integer *, integer *
	    , integer *, integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, real *, real *, real *, real *, real *, 
	    real *, char *, integer *, logical *, ftnlen);
    static integer lllold, itnper, nnnold;
    extern /* Subroutine */ int rplotl_(integer *, real *, real *, real *, 
	    integer *, real *, real *, real *, real *, real *, real *, 
	    integer *, char *, integer *, ftnlen);
    static integer kperim;
    extern /* Subroutine */ int getrow_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, real *, real *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *, real *, real *, real *, real *, real *, real *, char *,
	     integer *, logical *, integer *, logical *, logical *, ftnlen), 
	    filsmo_(integer *, integer *, real *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    real *, real *, char *, integer *, ftnlen), sflush_(), adjrow_(
	    integer *, integer *, integer *, real *, real *, real *, integer *
	    , integer *, integer *, integer *, real *, real *, integer *, 
	    integer *, integer *, integer *, real *, real *, real *, real *, 
	    real *, real *, char *, integer *, integer *, integer *, integer *
	    , integer *, integer *, integer *, integer *, integer *, logical *
	    , logical *, integer *, real *, logical *, logical *, logical *, 
	    ftnlen), pcross_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, real *, integer *, integer *
	    , integer *, integer *, real *, integer *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, real *, real *, real *
	    , real *, real *, real *, char *, integer *, integer *, integer *,
	     integer *, integer *, integer *, integer *, integer *, logical *,
	     logical *, integer *, logical *, logical *, ftnlen), colaps_(
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, integer *, integer *, integer *, integer *, real *
	    , integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, real *, real *, real *, real *, 
	    real *, real *, char *, integer *, integer *, integer *, integer *
	    , integer *, integer *, integer *, integer *, logical *, logical *
	    , integer *, logical *, logical *, ftnlen), flmnmx_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, integer *, integer *, real *, real *, real *, real *, 
	    logical *), tridel_(integer *, integer *, real *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    real *, integer *, char *, integer *, real *, real *, real *, 
	    real *, real *, real *, logical *, logical *, logical *, logical *
	    , ftnlen);

    /* Fortran I/O blocks */
    static cilist io___39 = { 0, 6, 0, " (A, F10.5)", 0 };
    static cilist io___40 = { 0, 6, 0, " (A, F10.5)", 0 };
    static cilist io___41 = { 0, 6, 0, " (A, F10.5)", 0 };
    static cilist io___42 = { 0, 6, 0, " (A, F10.5)", 0 };
    static cilist io___43 = { 0, 6, 0, " (A, F10.5)", 0 };
    static cilist io___44 = { 0, 6, 0, " (A, F10.5)", 0 };
    static cilist io___45 = { 0, 6, 0, " (A, F10.5)", 0 };
    static cilist io___46 = { 0, 6, 0, " (A, F10.5)", 0 };
    static cilist io___47 = { 0, 6, 0, " (A, F10.5)", 0 };


/* *********************************************************************** */
/*  SUBROUTINE PAVING = A SUBROUTINE TO PAVE A REGION GIVEN THE INITIAL */
/*                      BOUNDARY AS A LIST OF NODES. */
/* *********************************************************************** */
/*  EXTERNAL VARIABLES: */
/*     NBNODE = NUMBER OF NODES ON THE INITIAL BOUNDARY */
/*     NPRM   = NUMBER OF SEPARATE PERIMETERS IN THE BOUNDARY */
/*              (THERE IS ONE OUTSIDE PERIMETER AND ONE PERIMETER FOR */
/*              EACH HOLE IN THE BOUNDARY) */
/*     MLN    = NUMBER OF ATTRIBUTES NEEDED IN LNODES ARRAY.  THIS */
/*              NUMBER SHOULD BE PASSED IN AS EIGHT (8) CURRENTLY. */
/*     IPTPER = INTEGER ARRAY OF POINTERS INTO THE BNODE ARRAY. */
/*              EACH POINTER INDICATES THE BEGINNING NODE FOR THAT */
/*              PERIMETER IN LPERIM */
/*     NUMPER = INTEGER ARRAY CONTAINING THE NUMBER OF NODES IN EACH */
/*              OF THE PERIMETERS */
/*     LPERIM = LIST OF PERIMETER NODES */
/*     X      = REAL ARRAY OF X VALUES OF NODES DIMENSIONED TO MXND */
/*     Y      = REAL ARRAY OF Y VALUES OF NODES DIMENSIONED TO MXND */
/*     Z      = REAL ARRAY OF Z VALUES OF NODES DIMENSIONED TO MXND */
/*     IEXK   = INTEGER ARRAY OF EDGES ATTACHED TO EACH ELEMENT */
/*              DIMENSIONED AS (4, MXND) */
/*     INXE   = INTEGER ARRAY OF NODES ATTACHED TO EACH EDGE */
/*            = DIMENSIONED AS (2, MXND) */
/*     NNODE  = NUMBER OF NODES IN THE FINAL MESH */
/*     NEDGE  = NUMBER OF EDGES IN THE FINAL MESH */
/*     NELEM  = NUMBER OF ELEMENTS IN THE FINAL MESH */
/*     MAXND  = MAXIMUM NUMBER OF NODES EXPECTED IN THE MESH */
/*              (IF THIS IS EXCEEDED, NOROOM IS RETURNED AS .TRUE.) */
/*     RWORK1 = REAL ARRAY FOR WORKING SPACE IN PAVING - DIMENSIONED */
/*              TO (MXND) - THIS BECOMES THE ANGLE ARRAY */
/*     RWORK2 = REAL ARRAY FOR WORKING SPACE IN PAVING - DIMENSIONED */
/*              TO (MXND * 2) - THIS BECOMES THE BNSIZE ARRAY */
/*     IWORK3 = INTEGER ARRAY FOR WORKING SPACE IN PAVING - DIMENSIONED */
/*              TO (MXND * 8) - THIS BECOMES THE LNODES ARRAY */
/*     IWORK4 = INTEGER ARRAY FOR WORKING SPACE IN PAVING - DIMENSIONED */
/*              TO (NPRM * 3) - THIS BECOMES THE LINKPR ARRAY */
/*     IWORK5 = INTEGER ARRAY FOR WORKING SPACE IN PAVING - DIMENSIONED */
/*              TO (NPRM) - THIS BECOMES THE NPERIM ARRAY */
/*     IWORK6 = INTEGER ARRAY FOR WORKING SPACE IN PAVING - DIMENSIONED */
/*              TO (MXND * 4) - THIS BECOMES THE LXK ARRAY */
/*     IWORK7 = INTEGER ARRAY FOR WORKING SPACE IN PAVING - DIMENSIONED */
/*              TO (MXND * 6) - THIS BECOMES THE KXL ARRAY */
/*     IWORK8 = INTEGER ARRAY FOR WORKING SPACE IN PAVING - DIMENSIONED */
/*              TO (MXND * 6) - THIS BECOMES THE NXL ARRAY */
/*     IWORK9 = INTEGER ARRAY FOR WORKING SPACE IN PAVING - DIMENSIONED */
/*              TO (MXND * 4) - THIS BECOMES THE LXN ARRAY */
/*     IWORK10 = INTEGER ARRAY FOR WORKING SPACE IN PAVING - DIMENSIONED */
/*              TO (MXND) - THIS BECOMES THE NUID ARRAY */
/*     IDUM1  = A DUMMY INTEGER PARAMETER NEEDED BY FASTQ -  THIS BECOMES */
/*              IAVAIL */
/*     IDUM2  = A DUMMY INTEGER PARAMETER NEEDED BY FASTQ -  THIS BECOMES */
/*              NAVAIL */
/*     GRAPH  = .TRUE. IF PLOTTING AT EACH STAGE IS DESIRED */
/*     TIMER  = .TRUE. IF A TIMING REPORT IS DESIRED */
/*     VIDEO  = .TRUE. IF A VIDEO ANIMATION SEQUENCE PLOT IS DESIRED */
/*     DEFSIZ = THE DEFAULT SIZE OF THE ELEMENTS IN THIS REGION */
/*              (SET IT TO ZERO IF YOU DON'T KNOW WHAT ELSE TO DO.) */
/*     SIZEIT = .TRUE. IF A SIZING FUNCTION IS TO BE USED WITH PAVING */
/*     DEV1   = A CHARACTER VARIABLE OF LENGTH 3 DESCRIBING THE */
/*              PLOTTING DEVICE BEING USED. */
/*     KREG   = THE REGION NUMBER BEING PROCESSED (FOR PLOTTING ID) */
/*     BATCH  = .TRUE. IF THE PROGRAM IS BEING RUN WITHOUT */
/*              GRAPHICS CAPABILITIES */
/*     NOROOM = .TRUE. IF ARRAY SIZES BUILT ACCORDING TO MAXND ARE */
/*              EXCEEDED (MORE SPACE IS NEEDED) */
/*     ERR    = .TRUE. IF AN ERROR OCCURS DURING PAVING OR IF NOROOM */
/*              IS .TRUE. */
/*     AMESUR = THE NODAL ERROR MEASURE VARIABLE (USED IN ADAPTIVE MESHING) */
/*     XNOLD  = THE OLD XN ARRAY FOR THE OLD MESH (USED IN ADAPTIVE MESHING) */
/*     YNOLD  = THE OLD YN ARRAY FOR THE OLD MESH (USED IN ADAPTIVE MESHING) */
/*     NXKOLD = THE OLD CONNECTIVITY ARRAY OF THE OLD MESH */
/*              (USED IN ADAPTIVE MESHING) */
/*     MMPOLD = THE OLD MATERIAL MAP ARRAY (USED IN ADAPTIVE MESHING) */
/*     LINKEG = THE LINKING ARRAY MAPPING ELEMENTS TO A SEARCH GRID */
/*     LISTEG = THE LIST OF ELEMENT THAT THE LINK POINTS TO */
/*     MLINK  = THE MAXIMUM SPACE NEEDED FOR THE SEARCH GRID LINK (LINKEG) */
/*     NPROLD = THE NUMBER OF PROCESSED REGIONS IN THE OLD MESH */
/*     NPNOLD = THE NUMBER OF PROCESSED NODES IN THE OLD MESH */
/*     NPEOLD = THE NUMBER OF PROCESSED ELEMENTS IN THE OLD MESH */
/*     NNXK   = THE NUMBER OF NODES PER ELEMENT IN THE OLD MESH */
/*     REMESH = .TRUE. IF AN ADAPTIVE MESHING IS REQUESTED */
/*     REXMIN = MIN X FOR THE OLD MESH */
/*     REXMAX = MAX X FOR THE OLD MESH */
/*     REYMIN = MIN Y FOR THE OLD MESH */
/*     REYMAX = MAX Y FOR THE OLD MESH */
/*     IDIVIS = NUMBER OF DIVISIONS IN THE SEARCH GRID LINK */
/* *********************************************************************** */
/*  INTERNAL VARIABLES: */
/*     ANGLE  = ARRAY OF REALS FOR STORING BOUNDARY NODE ANGLES. */
/*     BNSIZE = ARRAY OF REALS FOR STORING ELEMENT SIZE PROPAGATION INFO. */
/*     LNODES = ARRAY OF INTEGERS FOR STORING BOUNDARY NODE INFORMATION. */
/*              IN THE LNODES ARRAY, */
/*              THE CORNER STATUS IS STORED IN LNODES (1, N1): */
/*                  0 = NOT DECIDED */
/*                  1 = ROW END */
/*                  3 = ROW SIDE */
/*                  5 = ROW CORNER */
/*                  7 = ROW REVERSAL */
/*              THE PRECEDING NODE IN LNODES (2, N1), */
/*              THE NEXT NODE IN LNODES (3, N1), */
/*              THE INTERIOR/EXTERIOR STATUS OF NODE IS IN LNODES (4, N1). */
/*                  1 = EXTERIOR OR ON THE BOUNDARY OF THE MESH */
/*                      (NEGATED FOR SMOOTHING) */
/*                  2 = INTERIOR TO THE MESH (NEGATED FOR SMOOTHING) */
/*              THE NEXT COUNTERCLOCKWISE LINE IS STORED IN LNODES (5, N1). */
/*              THE ANGLE STATUS OF LNODES IS STORED IN (6, N1), */
/*                  1 = ROW END ONLY */
/*                  2 = ROW END OR SIDE */
/*                  3 = ROW SIDE ONLY */
/*                  4 = ROW SIDE OR ROW CORNER */
/*                  5 = ROW CORNER ONLY */
/*                  6 = ROW CORNER OR REVERSAL */
/*                  7 = ROW REVERSAL ONLY */
/*              THE NUMBER OF NODES TO THE NEXT CORNER IS STORED IN (7, N1). */
/*              THE DEPTH OF THE ROW OF THIS NODE IS STORED IN (8, N1) */
/*     LINKPR = ARRAY FOR STORING LINKS TO PERIMETERS. */
/*     LXK    = LINES PER ELEMENT */
/*     KXL    = ELEMENTS PER LINE */
/*     NXL    = NODES PER LINE */
/*     LXN    = LINES PER NODE */
/*              NOTE: */
/*                 FOR *XN TABLES A NEGATIVE FLAG IN THE FOURTH COLUMN MEANS */
/*                 GO TO THAT ROW FOR A CONTINUATION OF THE LIST.  IN THAT ROW */
/*                 THE FIRST ELEMENT WILL BE NEGATED TO INDICATE THAT THIS IS */
/*                 A CONTINUATION ROW. */
/*                 A NEGATIVE FLAG IN THE SECOND COLUMN OF THE LXN ARRAY MEANS */
/*                 THAT THIS NODE IS AN EXTERIOR BOUNDARY NODE. */
/* *********************************************************************** */
/*  MXPICK MUST BE SET AT (2 ** MXCORN) */
    /* Parameter adjustments */
    --lperim;
    --nperim;
    linkpr -= 4;
    --numper;
    --iptper;
    --nuid;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    bnsize -= 3;
    --angle;
    inxe -= 3;
    iexk -= 5;
    --zn;
    --yn;
    --xn;
    linkeg -= 3;
    mmpold -= 4;
    --bmesur;
    --ynold;
    --xnold;
    --listeg;
    --amesur;
    nxkold_dim1 = *nnxk;
    nxkold_offset = 1 + nxkold_dim1;
    nxkold -= nxkold_offset;

    /* Function Body */
    if (*remesh) {
	*sizeit = TRUE_;
    }
    timing_1.timea = (float)0.;
    timing_1.timep = (float)0.;
    timing_1.timec = (float)0.;
    timing_1.timepc = (float)0.;
    timing_1.timeaj = (float)0.;
    timing_1.times = (float)0.;
    getime_(&time1);
    *err = FALSE_;
    done = FALSE_;
/*  ZERO ALL THE LINK ARRAYS */
    i__1 = *mxnd;
    for (i__ = 1; i__ <= i__1; ++i__) {
	for (j = 1; j <= 4; ++j) {
	    lxk[j + (i__ << 2)] = 0;
	    lxn[j + (i__ << 2)] = 0;
/* L100: */
	}
/* L110: */
    }
    i__1 = *mxnd;
    for (i__ = *nnn + 1; i__ <= i__1; ++i__) {
	nuid[i__] = 0;
/* L120: */
    }
    i__1 = *mxnd * 3;
    for (i__ = 1; i__ <= i__1; ++i__) {
	for (j = 1; j <= 2; ++j) {
	    kxl[j + (i__ << 1)] = 0;
	    nxl[j + (i__ << 1)] = 0;
/* L130: */
	}
/* L140: */
    }
/*  ZERO THE LOOP COUNTING AND CONNECTING ARRAYS */
    for (i__ = 1; i__ <= 20; ++i__) {
	nloop[i__ - 1] = 0;
	nextn1[i__ - 1] = 0;
/* L150: */
    }
    i__1 = *nprm;
    for (i__ = 1; i__ <= i__1; ++i__) {
	linkpr[i__ * 3 + 1] = 0;
	linkpr[i__ * 3 + 2] = 0;
	linkpr[i__ * 3 + 3] = 0;
/* L160: */
    }
/*  FIND THE EXTREMES OF THE PERIMETERS */
    xmin = xn[iptper[1]];
    xmax = xn[iptper[1]];
    ymin = yn[iptper[1]];
    ymax = yn[iptper[1]];
    zmin = zn[iptper[1]];
    zmax = zn[iptper[1]];
    i__1 = *nprm;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = iptper[i__] + numper[i__] - 1;
	for (j = iptper[i__]; j <= i__2; ++j) {
	    node1 = lperim[j];
/* Computing MIN */
	    r__1 = xn[node1];
	    xmin = dmin(r__1,xmin);
/* Computing MAX */
	    r__1 = xn[node1];
	    xmax = dmax(r__1,xmax);
/* Computing MIN */
	    r__1 = yn[node1];
	    ymin = dmin(r__1,ymin);
/* Computing MAX */
	    r__1 = yn[node1];
	    ymax = dmax(r__1,ymax);
/* Computing MIN */
	    r__1 = zn[node1];
	    zmin = dmin(r__1,zmin);
/* Computing MAX */
	    r__1 = zn[node1];
	    zmax = dmax(r__1,zmax);
/* L170: */
	}
/* L180: */
    }
/*  LINK ALL THE NODES IN THE ORIGINAL PERIMETERS TOGETHER */
    i__1 = *nprm;
    for (i__ = 1; i__ <= i__1; ++i__) {
	periml_(nbnode, mxnd, &numper[i__], &iptper[i__], mln, &xn[1], &yn[1],
		 &zn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &angle[1], &
		bnsize[3], &lnodes[lnodes_offset], &lperim[1], lll, &lllold, &
		xmin, &xmax, &ymin, &ymax, &zmin, &zmax, dev1, kreg, err, (
		ftnlen)3);
	if (*err) {
	    goto L310;
	}
	linkpr[i__ * 3 + 1] = lperim[iptper[i__]];
	if (i__ > 1) {
	    linkpr[(i__ - 1) * 3 + 2] = i__;
	    linkpr[i__ * 3 + 2] = 1;
	} else {
	    linkpr[i__ * 3 + 2] = 0;
	}
	linkpr[i__ * 3 + 3] = numper[i__];
	nperim[1] = numper[i__];
/* L190: */
    }
    itnper = *nbnode;
/*  LINK UP THE REST OF THE LXN ARRAY */
    nnnold = *nnn;
    lllold = *lll;
    *iavail = *nnn + 1;
    *navail = *mxnd - *nnn;
    i__1 = *mxnd;
    for (i__ = *iavail; i__ <= i__1; ++i__) {
	lxn[(i__ << 2) + 1] = 0;
	lxn[(i__ << 2) + 2] = 0;
	lxn[(i__ << 2) + 3] = 0;
	lxn[(i__ << 2) + 4] = i__ + 1;
/* L200: */
    }
/*  PLOT THE INITIAL BOUNDARIES */
    if (*graph) {
	rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], &xmin, &xmax, &ymin, &
		ymax, &zmin, &zmax, lll, dev1, kreg, (ftnlen)3);
    }
    xmin1 = xmin;
    xmax1 = xmax;
    ymin1 = ymin;
    ymax1 = ymax;
    zmin1 = zmin;
    zmax1 = zmax;
/*  CHECK INPUT FOR ODDNESS */
    if (itnper / 2 << 1 != itnper) {
	mesage_("IN PAVING, NO. OF PERIMETER NODES IS ODD", (ftnlen)40);
	*err = TRUE_;
	goto L310;
    }
/*  NOW BEGIN TO LOOP THROUGH THE INTERIOR NODE LIST */
/*  FILLING ROWS WITH ELEMENTS */
    n1 = linkpr[4];
    n0 = lnodes[n1 * lnodes_dim1 + 2];
    kloop = 1;
    kperim = 1;
    nloop[0] = numper[1];
L210:
/*  SEE IF IT IS TIME TO SWITCH TO THE NEXT PERIMETER */
/*  BY WHETHER THE CURRENT N0 IS INTERIOR OR NOT */
    if ((i__1 = lnodes[n0 * lnodes_dim1 + 4], abs(i__1)) == 2) {
	if (linkpr[kperim * 3 + 2] != 0) {
	    linkpr[kperim * 3 + 3] = nloop[0];
	    linkpr[kperim * 3 + 1] = n1;
	    kperim = linkpr[kperim * 3 + 2];
	    n1 = linkpr[kperim * 3 + 1];
	    nloop[0] = linkpr[kperim * 3 + 3];
	    n0 = lnodes[n1 * lnodes_dim1 + 2];
	} else {
	    n0 = lnodes[n1 * lnodes_dim1 + 2];
	}
    }
/*  NOW GET THE BEST CORNERS FOR THE NEXT ROW */
    getrow_(mxnd, &c__10, &c__1024, mln, &nuid[1], &lxk[5], &kxl[3], &nxl[3], 
	    &lxn[5], &lnodes[lnodes_offset], &ncorn, lcorn, &bnsize[3], &
	    angle[1], &xn[1], &yn[1], &zn[1], icomb, itype, nloop, &n1, &nend,
	     iavail, navail, lll, kkk, nnn, graph, video, &xmin, &xmax, &ymin,
	     &ymax, &zmin, &zmax, dev1, kreg, sizeit, &linkpr[kperim * 3 + 2],
	     noroom, err, (ftnlen)3);
    if (*noroom || *err) {
	goto L310;
    }
/*  CHECK TO SEE IF WE ARE DONE WITH ONLY A QUAD LEFT */
/*  (AND THAT THE LOOP IS NOT AN INTERIOR HOLE) */
    if (nloop[0] == 4 && linkpr[kperim * 3 + 2] == 0) {
	close4_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[n1 * lnodes_dim1 + 2], &n1, &lnodes[
		n1 * lnodes_dim1 + 3], &lnodes[lnodes[n1 * lnodes_dim1 + 3] * 
		lnodes_dim1 + 3], kkk, err);
	if (*err) {
	    goto L310;
	}
	filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], 
		&lxn[5], lll, nnn, nnn, &lnodes[lnodes_offset], &bnsize[3], 
		nloop, &xmin, &xmax, &ymin, &ymax, &zmin, &zmax, dev1, kreg, (
		ftnlen)3);
	if (*graph) {
	    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], &xmin, &xmax, &
		    ymin, &ymax, &zmin, &zmax, lll, dev1, kreg, (ftnlen)3);
	}
	goto L240;
/*  CHECK TO SEE IF WE ARE DONE WITH ONLY 6 NODES LEFT */
    } else if (nloop[0] == 6 && linkpr[kperim * 3 + 2] == 0) {
	close6_(mxnd, &c__10, mln, &nuid[1], &xn[1], &yn[1], &lxk[5], &kxl[3],
		 &nxl[3], &lxn[5], &angle[1], &bnsize[3], &lnodes[
		lnodes_offset], &n1, nloop, &kkkold, &lllold, &nnnold, navail,
		 iavail, &done, &xmin, &xmax, &ymin, &ymax, dev1, lll, kkk, 
		nnn, lcorn, &ncorn, graph, video, sizeit, noroom, err, &xnold[
		1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], &listeg[1],
		 &bmesur[1], mlink, npnold, npeold, nnxk, remesh, rexmin, 
		rexmax, reymin, reymax, idivis, sizmin, emax, emin, (ftnlen)3)
		;
	if (*noroom || *err) {
	    goto L310;
	}
	filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], 
		&lxn[5], lll, nnn, nnn, &lnodes[lnodes_offset], &bnsize[3], 
		nloop, &xmin, &xmax, &ymin, &ymax, &zmin, &zmax, dev1, kreg, (
		ftnlen)3);
	if (*graph) {
	    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], &xmin, &xmax, &
		    ymin, &ymax, &zmin, &zmax, lll, dev1, kreg, (ftnlen)3);
	    sflush_();
	}
	goto L240;
    }
/*  GENERATE A NEW ROW OF ELEMENTS */
    addrow_(mxnd, &c__10240, &c__20, mln, nprm, &nuid[1], &xn[1], &yn[1], &zn[
	    1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &angle[1], &bnsize[3], &
	    lnodes[lnodes_offset], &n1, &nend, nloop, nextn1, &linkpr[4], &
	    kperim, &kkkold, &lllold, &nnnold, iavail, navail, &xmin, &xmax, &
	    ymin, &ymax, &zmin, &zmax, dev1, lll, kkk, nnn, &nnn2, &nadj1, &
	    nadj2, icomb, &kloop, graph, video, kreg, &done, sizeit, noroom, 
	    err, &xnold[1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3], &
	    listeg[1], &bmesur[1], mlink, npnold, npeold, nnxk, remesh, 
	    rexmin, rexmax, reymin, reymax, idivis, sizmin, emax, emin, (
	    ftnlen)3);
    if (*noroom || *err) {
	goto L310;
    }
    if (done) {
	goto L240;
    }
/*  TRY COLLAPSING CORNERS WITH SMALL ANGLES AFTER A ROW HAS BEEN */
/*  COMPLETED - NOTE THAT THE ICOMB ARRAY IS SENT TO PINCH IN PLACE */
/*  OF THE LCORN ARRAY FOR MORE CORNER PROCESSING CAPABILITIES */
L220:
    pinch_(mxnd, &c__10240, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[5], &
	    kxl[3], &nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], &
	    bnsize[3], &n1, nloop, &kkkold, &lllold, &nnnold, iavail, navail, 
	    &done, &xmin, &xmax, &ymin, &ymax, &zmin, &zmax, dev1, lll, kkk, 
	    nnn, icomb, &ncorn, &nadj1, &nadj2, graph, video, kreg, noroom, 
	    err, (ftnlen)3);
    if (*noroom || *err) {
	goto L310;
    }
    if (done) {
	goto L240;
    }
/* C */
/* C  CHECK TO SEE IF ANY ISOLATED ELEMENTS HAVE BEEN FORMED AND */
/* C  TAKE CARE OF THEM IF THEY HAVE */
/* C */
/*      IF (NLOOP(1) .GT. 6) THEN */
/*         CALL ISOEL (MXND, MLN, NUID, XN, YN, ZN, LXK, KXL, NXL, LXN, */
/*     &      LNODES, ANGLE, BNSIZE, IAVAIL, NAVAIL, LLL, KKK, NNN, N1, */
/*     &      NLOOP (1), XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX, DEV1, KREG, */
/*     &      ISOELM, GRAPH, VIDEO, NOROOM, ERR) */
/*         IF ((NOROOM) .OR. (ERR)) GOTO 220 */
/*         IF (ISOELM) GOTO 180 */
/*      ENDIF */
/*  ADJUST THE NEW ROW BY TAKING TUCKS OR INSERTING WEDGES AS NEEDED */
    if (nadj1 > 0 && nadj2 > 0 && nloop[0] > 4) {
	adjrow_(mxnd, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3],
		 &nxl[3], &lxn[5], &angle[1], &bnsize[3], &lnodes[
		lnodes_offset], nloop, iavail, navail, &xmin, &xmax, &ymin, &
		ymax, &zmin, &zmax, dev1, lll, kkk, nnn, &lllold, &nnnold, &
		n1, &nadj1, &nadj2, &nnn2, graph, video, kreg, defsiz, &
		adjted, noroom, err, (ftnlen)3);
	if (*noroom || *err) {
	    goto L310;
	}
	if (adjted) {
	    goto L220;
	}
    }
/*  CHECK TO SEE IF ANY OF THE CONCURRENT PERIMETERS OVERLAP */
    if (linkpr[kperim * 3 + 2] != 0) {
	linkpr[kperim * 3 + 3] = nloop[0];
	pcross_(mxnd, &c__10240, mln, &c__20, nprm, &nuid[1], &xn[1], &yn[1], 
		&zn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &angle[1], &
		lnodes[lnodes_offset], &bnsize[3], &linkpr[4], &kperim, &n1, &
		nadj1, &nadj2, &kkkold, &lllold, &nnnold, iavail, navail, &
		done, &xmin, &xmax, &ymin, &ymax, &zmin, &zmax, dev1, lll, 
		kkk, nnn, icomb, &ncorn, nloop, nextn1, &kloop, graph, video, 
		kreg, noroom, err, (ftnlen)3);
	if (*noroom || *err) {
	    goto L310;
	}
    }
/*  TRY COLLAPSING OVERLAPPING SIDES TO FORM TWO LOOPS OUT OF THE */
/*  CURRENT SINGLE LOOP - NOTE THAT THE ICOMB ARRAY IS SENT AS */
/*  WHEN CALLING PINCH IN PLACE OF THE LCORN ARRAY */
L230:
    if (nloop[0] > 6) {
	colaps_(mxnd, &c__10240, mln, &c__20, &nuid[1], &xn[1], &yn[1], &zn[1]
		, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &angle[1], &lnodes[
		lnodes_offset], &bnsize[3], &n1, &kkkold, &lllold, &nnnold, 
		iavail, navail, &done, &xmin, &xmax, &ymin, &ymax, &zmin, &
		zmax, dev1, lll, kkk, nnn, icomb, &ncorn, nloop, nextn1, &
		kloop, graph, video, kreg, noroom, err, (ftnlen)3);
	if (*noroom || *err) {
	    goto L310;
	}
	if (done) {
	    goto L240;
	}
    }
/*  ADJUST THE ZOOMS TO FIT THE NEW AREA */
    if (*graph || cpubrk_(&c_true)) {
	linkpr[kperim * 3 + 3] = nloop[0];
	flmnmx_(mxnd, mln, nprm, &linkpr[4], &kperim, &lnodes[lnodes_offset], 
		&xn[1], &yn[1], nloop, &n1, &xmin, &xmax, &ymin, &ymax, err);
	if (*err) {
	    goto L310;
	}
	rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], &xmin, &xmax, &ymin, &
		ymax, &zmin, &zmax, lll, dev1, kreg, (ftnlen)3);
    }
    goto L210;
/*  CHECK TO MAKE SURE THAT OTHER LOOPS ARE NOT REMAINING TO BE FILLED */
L240:
    if (kloop > 1) {
	n1 = nextn1[0];
	i__1 = kloop - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nloop[i__ - 1] = nloop[i__];
	    nextn1[i__ - 1] = nextn1[i__];
/* L250: */
	}
	nloop[kloop - 1] = 0;
	nextn1[kloop - 1] = 0;
	--kloop;
/*  ADJUST THE ZOOMS TO FIT THE NEW AREA */
	if (*graph) {
	    flmnmx_(mxnd, mln, nprm, &linkpr[4], &kperim, &lnodes[
		    lnodes_offset], &xn[1], &yn[1], nloop, &n1, &xmin, &xmax, 
		    &ymin, &ymax, err);
	    if (*err) {
		goto L310;
	    }
	    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], &xmin, &xmax, &
		    ymin, &ymax, &zmin, &zmax, lll, dev1, kreg, (ftnlen)3);
	}
	done = FALSE_;
/*  ENTER THE FILL LOOP WHERE IT CAN CHECK TO SEE IF ANY CROSSINGS */
/*  ALREADY EXIST IN THIS LOOP */
	goto L230;
    }
/*  THE FILL HAS BEEN COMPLETED - NOW FIX UP ANY BAD SPOTS */
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	lnodes[i__ * lnodes_dim1 + 4] = (i__2 = lnodes[i__ * lnodes_dim1 + 4],
		 abs(i__2));
/* L260: */
    }
    filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &
	    lxn[5], lll, nnn, nnn, &lnodes[lnodes_offset], &bnsize[3], nloop, 
	    &xmin, &xmax, &ymin, &ymax, &zmin, &zmax, dev1, kreg, (ftnlen)3);
    tridel_(mxnd, mln, &xn[1], &yn[1], &zn[1], &nuid[1], &lxk[5], &kxl[3], &
	    nxl[3], &lxn[5], nnn, lll, kkk, navail, iavail, &angle[1], &
	    lnodes[lnodes_offset], &bnsize[3], nloop, dev1, kreg, &xmin, &
	    xmax, &ymin, &ymax, &zmin, &zmax, graph, video, noroom, err, (
	    ftnlen)3);
    if (*noroom || *err) {
	goto L310;
    }
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	lnodes[i__ * lnodes_dim1 + 4] = -(i__2 = lnodes[i__ * lnodes_dim1 + 4]
		, abs(i__2));
/* L270: */
    }
    filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &
	    lxn[5], lll, nnn, nnn, &lnodes[lnodes_offset], &bnsize[3], nloop, 
	    &xmin, &xmax, &ymin, &ymax, &zmin, &zmax, dev1, kreg, (ftnlen)3);
/*  SUCCESSFUL EXIT */
    if (*graph) {
	rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], &xmin1, &xmax1, &ymin1,
		 &ymax1, &zmin1, &zmax1, lll, dev1, kreg, (ftnlen)3);
    }
    iuppr = min(*lll,*mxnd);
    i__1 = iuppr;
    for (i__ = 1; i__ <= i__1; ++i__) {
	for (j = 1; j <= 4; ++j) {
	    iexk[j + (i__ << 2)] = lxk[j + (i__ << 2)];
/* L280: */
	}
/* L300: */
    }
/* Computing MIN */
    i__1 = *lll, i__2 = *mxnd * 3;
    iuppr = min(i__1,i__2);
    i__1 = iuppr;
    for (i__ = 1; i__ <= i__1; ++i__) {
	for (j = 1; j <= 2; ++j) {
	    inxe[j + (i__ << 1)] = nxl[j + (i__ << 1)];
/* L290: */
	}
/* L301: */
    }
/*  EXIT WITH ERROR */
L310:
    if (*err && ! (*batch)) {
	rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], &xmin1, &xmax1, &ymin1,
		 &ymax1, &zmin1, &zmax1, lll, dev1, kreg, (ftnlen)3);
	ringbl_();
	sflush_();
    }
    if (*timer) {
	getime_(&time2);
	s_wsfe(&io___39);
	do_fio(&c__1, "  CPU SECONDS USED: ", (ftnlen)20);
	r__1 = time2 - time1;
	do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	e_wsfe();
	s_wsfe(&io___40);
	do_fio(&c__1, "            ADDROW: ", (ftnlen)20);
	do_fio(&c__1, (char *)&timing_1.timea, (ftnlen)sizeof(real));
	e_wsfe();
	s_wsfe(&io___41);
	do_fio(&c__1, "             PINCH: ", (ftnlen)20);
	do_fio(&c__1, (char *)&timing_1.timep, (ftnlen)sizeof(real));
	e_wsfe();
	s_wsfe(&io___42);
	do_fio(&c__1, "            COLAPS: ", (ftnlen)20);
	do_fio(&c__1, (char *)&timing_1.timec, (ftnlen)sizeof(real));
	e_wsfe();
	s_wsfe(&io___43);
	do_fio(&c__1, "            PCROSS: ", (ftnlen)20);
	do_fio(&c__1, (char *)&timing_1.timepc, (ftnlen)sizeof(real));
	e_wsfe();
	s_wsfe(&io___44);
	do_fio(&c__1, "            ADJROW: ", (ftnlen)20);
	do_fio(&c__1, (char *)&timing_1.timeaj, (ftnlen)sizeof(real));
	e_wsfe();
	s_wsfe(&io___45);
	do_fio(&c__1, "            SMOOTH: ", (ftnlen)20);
	do_fio(&c__1, (char *)&timing_1.times, (ftnlen)sizeof(real));
	e_wsfe();
	s_wsfe(&io___46);
	do_fio(&c__1, "     MISCELLANEOUS: ", (ftnlen)20);
	r__1 = time2 - time1 - timing_1.timea - timing_1.timep - 
		timing_1.timec - timing_1.timepc - timing_1.timeaj - 
		timing_1.times;
	do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	e_wsfe();
	s_wsfe(&io___47);
	do_fio(&c__1, "         %  SMOOTH: ", (ftnlen)20);
	r__1 = timing_1.times * (float)100. / (time2 - time1);
	do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	e_wsfe();
    }
    return 0;
} /* paving_ */

#ifdef __cplusplus
	}
#endif

/* fastq.f -- translated by f2c (version 20160102).
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

static integer c_n4 = -4;
static integer c_n3 = -3;
static integer c__75 = 75;
static integer c__9 = 9;
static integer c__1 = 1;
static integer c_n1 = -1;
static integer c_n2 = -2;
static integer c__10001 = 10001;
static integer c__10002 = 10002;
static integer c__10003 = 10003;
static integer c__0 = 0;
static integer c__2 = 2;
static real c_b75 = (float)160.;
static integer c_n99 = -99;
static integer c__6 = 6;
static integer c__50 = 50;
static integer c__60 = 60;
static integer c__4 = 4;
static integer c__100 = 100;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Main program */ int MAIN__()
{
    /* Format strings */
    static char fmt_10000[] = "(\002 \002,\002ERROR OPENING FILE: \002,a)";

    /* System generated locals */
    integer i__1, i__2, i__3;
    doublereal d__1;
    icilist ici__1;
    olist o__1;
    cllist cl__1;
    alist al__1;
    static real equiv_0[1];

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(), s_rsfi(icilist *), do_fio(integer *, char *, ftnlen), 
	    e_rsfi();
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer i_dnnt(doublereal *), s_wsfe(cilist *), e_wsfe(), f_open(olist *),
	     f_rew(alist *), f_clos(cllist *);

    /* Local variables */
    static char datatype[8];
    static logical exodusii;
#define a (equiv_0)
    static integer i__, k[67], l, n[29];
    static real x1, x2, y1, y2;
#define ia ((integer *)equiv_0)
    static real ct;
    static integer ml, ln, mp, mr, ms, iz;
    static real st, xx1, yy1;
    static char cin[72*50];
    static integer len, iin[50], kin[50];
    static real rin[50];
    static char dev1[3], dev2[3];
    static logical labe, labf, labi, labl, labm, labn, labo, labp;
    static char hard[8];
    static logical labr;
    static char date[8];
    static integer idau, mode;
    static logical labw;
    static integer idev[2], icom;
    static logical nine, ians;
    static integer klen, jcom;
    static char time[8];
    extern /* Subroutine */ int mesh_(real *, integer *, integer *, integer *,
	     integer *, integer *, integer *, integer *, integer *, integer *,
	     integer *, char *, real *, integer *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, integer *, integer *, integer *, char *, char *, char *
	    , real *, char *, logical *, integer *, logical *, char *, 
	    logical *, logical *, logical *, logical *, logical *, char *, 
	    logical *, logical *, logical *, logical *, logical *, logical *, 
	    logical *, logical *, logical *, logical *, real *, logical *, 
	    logical *, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen)
	    ;
    static logical snap, full, axis;
    static integer kcsu, nerr;
    static char soft[8];
    extern /* Subroutine */ int list_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, char *, 
	    real *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    char *, char *, char *, real *, char *, logical *, logical *, 
	    logical *, logical *, logical *, logical *, real *, char *, 
	    logical *, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen);
    static integer knsu, itry;
    static real time1;
    static logical lablb, labnb, labmd, labpb, batch;
    extern /* Subroutine */ int gdata_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, char *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, real *, char *, char *, real *, char *, logical *, 
	    logical *, logical *, logical *, logical *, logical *, logical *, 
	    logical *, logical *, logical *, logical *, logical *, logical *, 
	    integer *, logical *, char *, logical *, char *, logical *, real *
	    , logical *, logical *, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen, 
	    ftnlen);
    static logical labsb, labsc, alpha;
    static char fname[2048];
    static real scale;
    extern /* Subroutine */ int ltadd_(integer *, integer *, integer *, 
	    integer *, integer *);
    static logical merge, eight;
    static integer ischm[60];
    static logical three, axisd;
    static integer mlold;
    static logical labsz;
    static integer mpold, mused, nsnap[2];
    static char title[72];
    static integer idump, msold, mrold;
    extern /* Subroutine */ int keyin_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    char *, real *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    char *, char *, char *, real *, char *, logical *, logical *, 
	    logical *, logical *, logical *, logical *, logical *, real *, 
	    char *, logical *, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen)
	    , rdfsq_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    char *, char *, char *, real *, char *, logical *, logical *, 
	    logical *, logical *, logical *, logical *, real *, integer *, 
	    real *, logical *, logical *, ftnlen, ftnlen, ftnlen, ftnlen);
    static real ratio;
    static logical optim, axist;
    static integer iunit;
    extern /* Subroutine */ int ltnew_(integer *, integer *), spawn_(logical *
	    );
    static logical start;
    static char versn[10];
    extern /* Subroutine */ int fexit_(logical *, integer *, integer *, 
	    integer *, char *, integer *, real *, integer *, real *, logical *
	    , char *, ftnlen, ftnlen);
    static logical wrote;
    extern /* Subroutine */ int wrfsq_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, char *, real *, integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, char *, char *, char *, real *, char *, 
	    logical *, logical *, logical *, logical *, logical *, real *, 
	    integer *, logical *, logical *, ftnlen, ftnlen, ftnlen, ftnlen, 
	    ftnlen);
    static integer kaval1, kaval2;
    static logical areacg;
    extern /* Subroutine */ int mddebg_(integer *);
    static logical labsbd;
    static char memdbg[8], defsch[72];
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen);
    static char scheme[72*60];
    extern /* Subroutine */ int mdfill_(integer *);
    static logical hardpl;
    extern /* Subroutine */ int exdate_(char *, ftnlen), tablet_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, char *, real *, integer *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, char *, 
	    logical *, logical *, real *, real *, real *, real *, real *, 
	    real *, real *, real *, real *, logical *, char *, logical *, 
	    real *, integer *, logical *, logical *, logical *, logical *, 
	    logical *, char *, real *, ftnlen, ftnlen, ftnlen, ftnlen), 
	    exname_(integer *, char *, integer *, ftnlen), delfsq_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, char *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, char *, char *, logical *, logical *, 
	    logical *, real *, logical *, char *, ftnlen, ftnlen, ftnlen, 
	    ftnlen), mdlong_(char *, integer *, integer *, ftnlen);
    static logical drwtab;
    static real defsiz;
    extern /* Subroutine */ int vdescp_(integer *, integer *, integer *), 
	    mdinit_(real *);
    static char number[80*60];
    extern /* Subroutine */ int mderor_(integer *), extime_(char *, ftnlen), 
	    exparm_(char *, char *, integer *, integer *, integer *, integer *
	    , ftnlen, ftnlen);
    static real snapdx[200]	/* was [2][100] */;
    extern /* Subroutine */ int vdiqes_(integer *, integer *), mdstat_(
	    integer *, integer *);
    static logical barwrt;
    static integer iostat;
    extern /* Subroutine */ int strait_(integer *, integer *, integer *, 
	    integer *, integer *, char *, real *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    ftnlen), exupcs_(char *, ftnlen), excpus_(real *);
    static logical noroom, tbzoom, regwrt;
    extern /* Subroutine */ int pltint_(), mdrsrv_(char *, integer *, integer 
	    *, ftnlen), strlng_(char *, integer *, ftnlen), inqstr_(char *, 
	    char *, ftnlen, ftnlen), intrup_(char *, logical *, integer *, 
	    integer *, integer *, char *, integer *, real *, integer *, 
	    ftnlen, ftnlen);
    static logical vaxvms;
    extern /* Subroutine */ int pltstv_(integer *, real *), help_fq__(integer 
	    *);

    /* Fortran I/O blocks */
    static cilist io___67 = { 0, 6, 0, 0, 0 };
    static cilist io___68 = { 0, 6, 0, 0, 0 };
    static cilist io___69 = { 0, 6, 0, 0, 0 };
    static cilist io___70 = { 0, 6, 0, 0, 0 };
    static cilist io___71 = { 0, 6, 0, 0, 0 };
    static cilist io___72 = { 0, 6, 0, 0, 0 };
    static cilist io___73 = { 0, 6, 0, 0, 0 };
    static cilist io___74 = { 0, 6, 0, 0, 0 };
    static cilist io___75 = { 0, 6, 0, 0, 0 };
    static cilist io___76 = { 0, 6, 0, 0, 0 };
    static cilist io___115 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  FASTQ = A PROGRAM TO QUICKLY GENERATE QUADRALATERAL MESHES */
/* *********************************************************************** */
/*                       WRITTEN AND MAINTAINED BY */
/*                             TED D. BLACKER */
/*                             DIVISION 1523 */
/*                              VERSION 1.4X */
/* *********************************************************************** */
/*                          USES WORK PREVIOUSLY */
/*                              COMPLETED BY */
/*                            RONDALL E. JONES */
/*                             DIVISION 2644 */
/*                     (QMESH, RENUM, AND QNUM CODES) */
/* *********************************************************************** */
/*  NOTE:  FASTQ CALLS SEVERAL GRAPHICS ROUTINES FROM THE PLT PLOT */
/*         PACKAGE, AS WELL AS A NUMBER OF UTILITY ROUTINES FROM */
/*         THE DEPARTMENT 1520 SUPES LIBRARY OF ROUTINES.  OF PRIME */
/*         USE IS THE FREE FIELD READER ROUTINES AND THE DYNAMIC */
/*         MEMORY ALLOCATION ROUTINES. */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     IANS   = LOGICAL RESPONSE FROM YES-NO QUESTION */
/*     TITLE  = MESH TITLE */
/*     DRWTAB = .TRUE. IF THE DIGITIZER HAS BEEN INITIALIZED TO A DRAWING */
/*     TBZOOM = .TRUE. IF THE DIGITIZER ZOOM HAS BEEN SET */
/*     WROTE  = .FALSE. IF THE DATA HAS BEEN CHANGED SINCE THE LAST WRITE */
/*     K      = POINTER ARRAY TO DIMENSION A */
/*     A      = LARGE ARRAY FOR VARIABLE DIMENSIONING */
/*              NOTE: FOR DYNAMIC VARIABLE DIMENSIONING, THE ROUTINES */
/*              WILL WORK AS CONFIGURED - THE SWITCH TO NORMAL VARIABLE */
/*              DIMENSIONING IS NOTED IN THE CODE AS IT IS NEEDED. */
/*              DYNAMIC VARIABLE DIMENSIONING IS NOT MACHINE INDEPENDENT. */
/*     MERGE  = .TRUE. IF TWO DATA FILES ARE TO BE MERGED */
/*     NOROOM = .TRUE. IF MORE ROOM IS NEEDED TO INPUT THE DATA */
/*     BATCH  = .TRUE. IF THE PROGRAM IS BEING RUN IN BATCH MODE */
/*     START  = .TRUE. IF THE PROGRAM IS JUST STARTING - TRY A READ FIRST */
/*     VAXVMS = .TRUE. IF THE INSTALLATION IS ON A VAX/VMS MACHINE */
/*              (IT IS ASSUMED THAT VAXVMS WILL HAVE MULTIPLE VIRTUAL */
/*              DEVICE DRIVERS AVAILABLE - MVDI) */
/* *********************************************************************** */
/*  NOTE:  IF DYNAMIC VARIABLE DIMENSIONING IS NOT BEING USED, THIS */
/*         PARAMETER STATEMENT SHOULD BE EXPANDED TO THE FORM: */
/*      PARAMETER (MP = 100, ML = 100, MS = 50, MR = 30, MSC = 30, MA = 4) */
/*         IF LARGER DIMENSIONS ARE DESIRED, MP, ML, MS, AND MR CAN */
/*         BE INCREASED ACCORDINGLY. */
/*         ALSO, THE VARIABLE A SHOULD BE DIMENSIONED AS: */
/*            DIMENSION A(MP*17 + ML*31 + MS*10 + MR*15) */
/*  INITIALIZE VARIABLES */
/* ... By default, write exodusII format. If the environment variable */
/*     EXT04 is set to 'EXODUSII', write exodusII format.  If EXT04 */
/*     is set to 'GENESIS', write exodusI format */
    exname_(&c_n4, datatype, &klen, (ftnlen)8);
    exupcs_(datatype, klen);
    if (s_cmp(datatype, "EXODUSII", (ftnlen)8, (ftnlen)8) == 0) {
	exodusii = TRUE_;
    } else if (s_cmp(datatype, "GENESIS", (ftnlen)7, (ftnlen)7) == 0) {
	exodusii = FALSE_;
    } else {
	exodusii = TRUE_;
    }
    excpus_(&time1);
    s_copy(title, " ", (ftnlen)72, (ftnlen)1);
    s_copy(defsch, "M", (ftnlen)72, (ftnlen)1);
    s_copy(versn, "FASTQ 3.22", (ftnlen)10, (ftnlen)10);
    defsiz = (float)0.;
    snap = TRUE_;
    tbzoom = FALSE_;
    merge = FALSE_;
    nsnap[0] = 0;
    nsnap[1] = 0;
    for (i__ = 1; i__ <= 100; ++i__) {
	snapdx[(i__ << 1) - 2] = (float)0.;
	snapdx[(i__ << 1) - 1] = (float)0.;
/* L100: */
    }
    three = FALSE_;
    eight = FALSE_;
    nine = FALSE_;
    optim = FALSE_;
/*  GET THE CURRENT SYSTEM PARAMETERS AND SET MODE FOR RUNNING */
    exparm_(hard, soft, &mode, &kcsu, &knsu, &idau, (ftnlen)8, (ftnlen)8);
/* **               FILE MODE */
    exname_(&c_n3, dev2, &klen, (ftnlen)3);
    if (s_cmp(dev2, "BAT", (ftnlen)3, (ftnlen)3) == 0) {
	batch = TRUE_;
	alpha = TRUE_;
	start = FALSE_;
	s_copy(cin, "READ", (ftnlen)72, (ftnlen)4);
	s_copy(cin + 72, "MESH", (ftnlen)72, (ftnlen)4);
	s_copy(cin + 144, "PROCESS", (ftnlen)72, (ftnlen)7);
	s_copy(cin + 216, "WRITE", (ftnlen)72, (ftnlen)5);
	s_copy(cin + 288, "EXIT", (ftnlen)72, (ftnlen)4);
	icom = 1;
	jcom = 5;
    } else {
	batch = FALSE_;
	alpha = FALSE_;
	start = TRUE_;
	s_copy(cin, "READ", (ftnlen)72, (ftnlen)4);
	icom = 1;
	jcom = 1;
    }
    if (s_cmp(soft, "VMS", (ftnlen)3, (ftnlen)3) == 0) {
	vaxvms = TRUE_;
    } else {
	vaxvms = FALSE_;
    }
    if (s_cmp(hard, "CRAY", (ftnlen)4, (ftnlen)4) == 0 && s_cmp(soft, "UNI", (
	    ftnlen)3, (ftnlen)3) != 0) {
/*  WE MUST NOW INPUT THE DEVICE IN AN ADHOCK MANNER FROM THE CRAY */
	exname_(&c__75, fname, &len, (ftnlen)2048);
	s_copy(dev1, fname, (ftnlen)3, (ftnlen)3);
	exupcs_(dev1, (ftnlen)3);
    }
/* *********************************************************************** */
    s_copy(title, " ", (ftnlen)72, (ftnlen)1);
    s_copy(defsch, "M", (ftnlen)72, (ftnlen)1);
    wrote = TRUE_;
    drwtab = FALSE_;
    hardpl = FALSE_;
    for (i__ = 1; i__ <= 29; ++i__) {
	n[i__ - 1] = 0;
/* L110: */
    }
/*  SET UP THE DEFAULT LABELING FOR DATA PLOTTING */
    areacg = FALSE_;
    axis = FALSE_;
    axisd = FALSE_;
    axist = FALSE_;
    full = FALSE_;
    labp = TRUE_;
    labl = TRUE_;
    labr = TRUE_;
    labmd = FALSE_;
    labi = FALSE_;
    labf = FALSE_;
    labpb = FALSE_;
    lablb = FALSE_;
    labsbd = FALSE_;
    labsc = FALSE_;
    labsz = FALSE_;
    labn = FALSE_;
    labe = FALSE_;
    labo = FALSE_;
    labnb = FALSE_;
    labsb = FALSE_;
    labm = FALSE_;
    labw = FALSE_;
/*  PRINT GREETING AND TRACE */
    mesage_(" ", (ftnlen)1);
    mesage_("WELCOME TO FASTQ:", (ftnlen)17);
    exdate_(date, (ftnlen)8);
    extime_(time, (ftnlen)8);
    s_wsle(&io___67);
    do_lio(&c__9, &c__1, "            DATE: ", (ftnlen)18);
    do_lio(&c__9, &c__1, date, (ftnlen)8);
    e_wsle();
    s_wsle(&io___68);
    do_lio(&c__9, &c__1, "            TIME: ", (ftnlen)18);
    do_lio(&c__9, &c__1, time, (ftnlen)8);
    e_wsle();
    s_wsle(&io___69);
    do_lio(&c__9, &c__1, "         VERSION: ", (ftnlen)18);
    do_lio(&c__9, &c__1, versn, (ftnlen)10);
    e_wsle();
    if (exodusii) {
	s_wsle(&io___70);
	do_lio(&c__9, &c__1, "   Output Format: ExodusII", (ftnlen)26);
	e_wsle();
    } else {
	s_wsle(&io___71);
	do_lio(&c__9, &c__1, "   Output Format: Genesis/ExodusI", (ftnlen)33);
	e_wsle();
    }
    mesage_(" ", (ftnlen)1);
    s_wsle(&io___72);
    do_lio(&c__9, &c__1, "+++                Copyright 2014 NTESS           \
        +++", (ftnlen)61);
    e_wsle();
    s_wsle(&io___73);
    do_lio(&c__9, &c__1, "+++ The U.S. Government retains a limited license \
in this +++", (ftnlen)61);
    e_wsle();
    s_wsle(&io___74);
    do_lio(&c__9, &c__1, "+++    software as prescribed in AL 88-1 and AL 91\
-7.     +++", (ftnlen)61);
    e_wsle();
    s_wsle(&io___75);
    do_lio(&c__9, &c__1, "+++ Export of this program may require a license f\
rom the +++", (ftnlen)61);
    e_wsle();
    s_wsle(&io___76);
    do_lio(&c__9, &c__1, "+++               United States Government        \
        +++", (ftnlen)61);
    e_wsle();
/*  IF THE CODE IS BEING RUN ON THE VAX INTERACTIVELY, */
/*  GET WHICH DEVICE IS BEING USED */
/*  AND SET UP THE MULTIPLE DEVICE OUTPUT ROUTINES */
    if (vaxvms && ! batch) {
	exname_(&c_n1, dev1, &len, (ftnlen)3);
	exname_(&c_n2, dev2, &len, (ftnlen)3);
	vdiqes_(&c__10001, &kaval1);
	vdiqes_(&c__10002, &kaval2);
	if (kaval1 != 1) {
	    alpha = TRUE_;
	    mesage_("TERMINAL PLOTTING DEVICE NOT AVAILABLE", (ftnlen)38);
	} else {
	    alpha = FALSE_;
	}
	if (kaval2 != 1) {
	    mesage_("HARDCOPY DEVICE NOT AVAILABLE", (ftnlen)29);
	}
    }
    if (! batch && ! alpha) {
	vdescp_(&c__10003, &c__0, &c__0);
	pltint_();
	vdescp_(&c__10001, &c__0, &c__0);
	pltstv_(&c__2, &c_b75);
    }
/*  SET UP THE DUMP LOCATION FOR THE LOG FILE */
    idump = 0;
/* ----------------------------------------------------------------------- */
/*  THE NEXT SERIES OF STATEMENTS MUST BE TAKEN OUT IF NOT USING */
/*  DYNAMIC VARIABLE DIMENSIONING */
/*  SET UP THE INITIAL POINTER ARRAY SYSTEM */
    mp = 1000;
    ml = 1000;
    ms = 1000;
    mr = 1000;
/*  INITIALIZE THE DYNAMIC DIMENSIONING ROUTINES */
    mdinit_(a);
    mdfill_(&c__0);
/* ... See if supes memory debugging desired */
/*     If EXT99 Environment variable set, turn on supes memory debugging */
/*     The numeric value of the variable is used as the unit to write */
/*     debug information to. */
    exname_(&c_n99, memdbg, &l, (ftnlen)8);
    if (l >= 1) {
	ici__1.icierr = 1;
	ici__1.iciend = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = l;
	ici__1.iciunit = memdbg;
	ici__1.icifmt = "(I8)";
	i__1 = s_rsfi(&ici__1);
	if (i__1 != 0) {
	    goto L20;
	}
	i__1 = do_fio(&c__1, (char *)&iunit, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L20;
	}
	i__1 = e_rsfi();
	if (i__1 != 0) {
	    goto L20;
	}
	mddebg_(&iunit);
    }
L20:
/*  GET INITIAL SPACE IN ARRAY A */
    mdrsrv_("IPOINT", k, &mp, (ftnlen)6);
    i__1 = mp << 1;
    mdrsrv_("COOR", &k[1], &i__1, (ftnlen)4);
    mdrsrv_("IPBOUN", &k[2], &mp, (ftnlen)6);
    mdrsrv_("ILINE", &k[3], &ml, (ftnlen)5);
    mdrsrv_("LTYPE", &k[4], &ml, (ftnlen)5);
    mdrsrv_("NINT", &k[5], &ml, (ftnlen)4);
    mdrsrv_("FACTOR", &k[6], &ml, (ftnlen)6);
    i__1 = ml * 3;
    mdrsrv_("LCON", &k[7], &i__1, (ftnlen)4);
    mdrsrv_("ILBOUN", &k[8], &ml, (ftnlen)6);
    mdrsrv_("ISBOUN", &k[9], &ml, (ftnlen)6);
    mdrsrv_("ISIDE", &k[10], &ms, (ftnlen)5);
    mdrsrv_("NLPS", &k[11], &ms, (ftnlen)4);
    mdrsrv_("IFLINE", &k[12], &ms, (ftnlen)6);
    i__1 = ms * 3;
    mdrsrv_("ILLIST", &k[13], &i__1, (ftnlen)6);
    mdrsrv_("IBARST", &k[14], &ms, (ftnlen)6);
    mdrsrv_("JMAT", &k[15], &ms, (ftnlen)4);
    mdrsrv_("JCENT", &k[16], &ms, (ftnlen)5);
    mdrsrv_("NLPB", &k[17], &ms, (ftnlen)4);
    mdrsrv_("JFLINE", &k[18], &ms, (ftnlen)6);
    i__1 = ms * 3;
    mdrsrv_("JLLIST", &k[19], &i__1, (ftnlen)6);
    mdrsrv_("IREGN", &k[20], &mr, (ftnlen)5);
    mdrsrv_("IMAT", &k[21], &mr, (ftnlen)4);
    mdrsrv_("NSPR", &k[22], &mr, (ftnlen)4);
    mdrsrv_("IFSIDE", &k[23], &mr, (ftnlen)6);
    i__1 = mr << 2;
    mdrsrv_("ISLIST", &k[24], &i__1, (ftnlen)6);
    mdrsrv_("IRPB", &k[25], &mr, (ftnlen)4);
    mdrsrv_("IPBF", &k[26], &mp, (ftnlen)4);
    mdrsrv_("NPPF", &k[27], &mp, (ftnlen)4);
    mdrsrv_("IFPB", &k[28], &mp, (ftnlen)4);
    i__1 = mp << 1;
    mdrsrv_("LISTPB", &k[29], &i__1, (ftnlen)6);
    mdrsrv_("ILBF", &k[30], &ml, (ftnlen)4);
    mdrsrv_("NLPF", &k[31], &ml, (ftnlen)4);
    mdrsrv_("IFLB", &k[32], &ml, (ftnlen)4);
    i__1 = ml << 1;
    mdrsrv_("LISTLB", &k[33], &i__1, (ftnlen)6);
    mdrsrv_("ISBF", &k[34], &ml, (ftnlen)4);
    mdrsrv_("NSPF", &k[35], &ml, (ftnlen)4);
    mdrsrv_("IFSB", &k[36], &ml, (ftnlen)4);
    i__1 = ml << 1;
    mdrsrv_("LISTSB", &k[37], &i__1, (ftnlen)6);
    i__1 = mr + ms << 2;
    mdrsrv_("ATTRIB", &k[38], &i__1, (ftnlen)6);
    i__1 = mp << 1;
    mdrsrv_("LINKP", &k[39], &i__1, (ftnlen)5);
    i__1 = ml << 1;
    mdrsrv_("LINKL", &k[40], &i__1, (ftnlen)5);
    i__1 = ms << 1;
    mdrsrv_("LINKS", &k[41], &i__1, (ftnlen)5);
    i__1 = ms << 1;
    mdrsrv_("LINKB", &k[42], &i__1, (ftnlen)5);
    i__1 = mr << 1;
    mdrsrv_("LINKR", &k[43], &i__1, (ftnlen)5);
    i__1 = ms + mr << 1;
    mdrsrv_("LINKM", &k[44], &i__1, (ftnlen)5);
    i__1 = mr << 1;
    mdrsrv_("LINKSC", &k[45], &i__1, (ftnlen)6);
    i__1 = mp << 1;
    mdrsrv_("LINKPB", &k[46], &i__1, (ftnlen)6);
    i__1 = ml << 1;
    mdrsrv_("LINKLB", &k[47], &i__1, (ftnlen)6);
    i__1 = ml << 1;
    mdrsrv_("LINKSB", &k[48], &i__1, (ftnlen)6);
    i__1 = mr << 2;
    mdrsrv_("REXTRM", &k[49], &i__1, (ftnlen)6);
    i__1 = mp << 1;
    mdrsrv_("IHOLDP", &k[50], &i__1, (ftnlen)6);
    i__1 = ml << 1;
    mdrsrv_("IHOLDL", &k[51], &i__1, (ftnlen)6);
    i__1 = ms << 1;
    mdrsrv_("IHOLDS", &k[52], &i__1, (ftnlen)6);
    i__1 = ms << 1;
    mdrsrv_("IHOLDB", &k[53], &i__1, (ftnlen)6);
    i__1 = mr << 1;
    mdrsrv_("IHOLDR", &k[54], &i__1, (ftnlen)6);
    i__1 = ms + mr << 1;
    mdrsrv_("IHOLDM", &k[55], &i__1, (ftnlen)6);
    i__1 = mp << 1;
    mdrsrv_("IHOLD1", &k[56], &i__1, (ftnlen)6);
    i__1 = ml << 1;
    mdrsrv_("IHOLD2", &k[57], &i__1, (ftnlen)6);
    i__1 = ml << 1;
    mdrsrv_("IHOLD3", &k[58], &i__1, (ftnlen)6);
    i__1 = mp * 3;
    mdrsrv_("IWTPBF", &k[59], &i__1, (ftnlen)6);
    i__1 = ml * 3;
    mdrsrv_("IWTLBF", &k[60], &i__1, (ftnlen)6);
    i__1 = ml * 3;
    mdrsrv_("IWTSBF", &k[61], &i__1, (ftnlen)6);
    mdrsrv_("RSIZE", &k[62], &mr, (ftnlen)5);
    mdrsrv_("IFHOLE", &k[63], &mr, (ftnlen)6);
    mdrsrv_("NHPR", &k[64], &mr, (ftnlen)4);
    mdrsrv_("IHLIST", &k[65], &mr, (ftnlen)6);
    mdrsrv_("IRGFLG", &k[66], &mr, (ftnlen)6);
    mdstat_(&nerr, &mused);
    if (nerr > 0) {
	mderor_(&c__6);
	s_stop(" ", (ftnlen)1);
    }
/*  THIS ENDS THE SECTION THAT NEEDS TO BE REMOVED IF NOT USING */
/*  DYNAMIC VARIABLE DIMENSIONING.  AS A REPLACEMENT, THE POINTERS */
/*  MUST BE HARD WIRED INTO THE PROGRAM.  THIS WOULD BE HANDLED IN THE */
/*  FOLLOWING PATTERN OF STATEMENTS: */
/*        K(1) = 1 */
/*        K(2) = K(1) + MP      !NOTE - MP IS THE DIMENSION FOR IPOINT, ETC. */
/*        K(3) = K(2) + MP*2 */
/*        K(4) = K(3) + MP */
/*        K(5) = K(4) + ML */
/*         .... */
/*        K(67) = K(66) + MR */
/* ----------------------------------------------------------------------- */
/*  ZERO THE LINK ARRAYS */
    ltnew_(&mp, &ia[k[39] - 1]);
    ltnew_(&ml, &ia[k[40] - 1]);
    ltnew_(&ms, &ia[k[41] - 1]);
    ltnew_(&ms, &ia[k[42] - 1]);
    ltnew_(&mr, &ia[k[43] - 1]);
    i__1 = ms + mr;
    ltnew_(&i__1, &ia[k[44] - 1]);
    ltnew_(&mr, &ia[k[45] - 1]);
    ltnew_(&mp, &ia[k[46] - 1]);
    ltnew_(&ml, &ia[k[47] - 1]);
    ltnew_(&ml, &ia[k[48] - 1]);
/*  ENTER FASTQ MAIN OPTION */
    iz = 0;
L120:
    if (! batch && icom > jcom) {
	mesage_(" ", (ftnlen)1);
	frefld_(&iz, &iz, "ENTER OPTION: ", &c__50, &iostat, &jcom, kin, cin, 
		iin, rin, (ftnlen)14, (ftnlen)72);
	icom = 1;
    }
/*  GRAPHICS OPTION - PLOTS FASTQ DATA */
    if (*(unsigned char *)&cin[(icom - 1) * 72] == 'G' || *(unsigned char *)&
	    cin[(icom - 1) * 72] == 'g') {
	++icom;
	gdata_(&mp, &ml, &ms, &mr, &c__60, &c__50, &icom, &jcom, cin, rin, 
		iin, kin, &idump, n, &ia[k[0] - 1], &a[k[1] - 1], &ia[k[2] - 
		1], &ia[k[3] - 1], &ia[k[4] - 1], &ia[k[5] - 1], &a[k[6] - 1],
		 &ia[k[7] - 1], &ia[k[8] - 1], &ia[k[9] - 1], &ia[k[10] - 1], 
		&ia[k[11] - 1], &ia[k[12] - 1], &ia[k[13] - 1], &ia[k[14] - 1]
		, &ia[k[15] - 1], &ia[k[16] - 1], &ia[k[17] - 1], &ia[k[18] - 
		1], &ia[k[19] - 1], &ia[k[20] - 1], &ia[k[21] - 1], &ia[k[22] 
		- 1], &ia[k[23] - 1], &ia[k[24] - 1], &ia[k[39] - 1], &ia[k[
		40] - 1], &ia[k[41] - 1], &ia[k[42] - 1], &ia[k[43] - 1], &ia[
		k[45] - 1], &a[k[49] - 1], &a[k[62] - 1], scheme, defsch, &
		defsiz, title, &labp, &labl, &labr, &axisd, &labmd, &labi, &
		labf, &labpb, &lablb, &labsbd, &labsc, &labsz, &full, idev, &
		alpha, dev1, &vaxvms, versn, &wrote, &time1, &hardpl, &batch, 
		(ftnlen)72, (ftnlen)72, (ftnlen)72, (ftnlen)72, (ftnlen)3, (
		ftnlen)10);
/*  DELETE OPTION - DELETES FASTQ DATA */
    } else if (*(unsigned char *)&cin[(icom - 1) * 72] == 'D' || *(unsigned 
	    char *)&cin[(icom - 1) * 72] == 'd') {
	++icom;
	delfsq_(&mp, &ml, &ms, &mr, &c__60, &c__50, &icom, &jcom, cin, rin, 
		iin, kin, n, &ia[k[2] - 1], &ia[k[8] - 1], &ia[k[9] - 1], &ia[
		k[11] - 1], &ia[k[12] - 1], &ia[k[13] - 1], &ia[k[22] - 1], &
		ia[k[23] - 1], &ia[k[24] - 1], &ia[k[25] - 1], &ia[k[26] - 1],
		 &ia[k[27] - 1], &ia[k[28] - 1], &ia[k[29] - 1], &ia[k[30] - 
		1], &ia[k[31] - 1], &ia[k[32] - 1], &ia[k[33] - 1], &ia[k[34] 
		- 1], &ia[k[35] - 1], &ia[k[36] - 1], &ia[k[37] - 1], &ia[k[
		39] - 1], &ia[k[40] - 1], &ia[k[41] - 1], &ia[k[42] - 1], &ia[
		k[43] - 1], &ia[k[45] - 1], &ia[k[46] - 1], &ia[k[47] - 1], &
		ia[k[48] - 1], &ia[k[59] - 1], &ia[k[60] - 1], &ia[k[61] - 1],
		 &ia[k[63] - 1], &ia[k[64] - 1], &ia[k[65] - 1], &ia[k[66] - 
		1], number, defsch, &optim, &vaxvms, &wrote, &time1, &batch, 
		versn, (ftnlen)72, (ftnlen)80, (ftnlen)72, (ftnlen)10);
	wrote = FALSE_;
/*  FLUSH OPTION - ERASES ALL DATA */
    } else if (*(unsigned char *)&cin[(icom - 1) * 72] == 'F' || *(unsigned 
	    char *)&cin[(icom - 1) * 72] == 'f') {
	++icom;
	intrup_("THIS OPTION ERASES ALL DATA - CONTINUE ANYWAY", &ians, &
		c__50, &icom, &jcom, cin, iin, rin, kin, (ftnlen)45, (ftnlen)
		72);
	if (ians) {
	    s_copy(title, " ", (ftnlen)72, (ftnlen)1);
	    for (i__ = 1; i__ <= 29; ++i__) {
		n[i__ - 1] = 0;
/* L130: */
	    }
	    nsnap[0] = 0;
	    nsnap[1] = 0;
	    for (i__ = 1; i__ <= 100; ++i__) {
		snapdx[(i__ << 1) - 2] = (float)0.;
		snapdx[(i__ << 1) - 1] = (float)0.;
/* L140: */
	    }
	    tbzoom = FALSE_;
	    ltnew_(&mp, &ia[k[39] - 1]);
	    ltnew_(&ml, &ia[k[40] - 1]);
	    ltnew_(&ms, &ia[k[41] - 1]);
	    ltnew_(&ms, &ia[k[42] - 1]);
	    ltnew_(&mr, &ia[k[43] - 1]);
	    i__1 = ms + mr;
	    ltnew_(&i__1, &ia[k[44] - 1]);
	    ltnew_(&mr, &ia[k[45] - 1]);
	    ltnew_(&mp, &ia[k[46] - 1]);
	    ltnew_(&ml, &ia[k[47] - 1]);
	    ltnew_(&ml, &ia[k[48] - 1]);
	}
/*  MESH OPTION - BEGINS MESH PROCESSING */
    } else if (*(unsigned char *)&cin[(icom - 1) * 72] == 'M' || *(unsigned 
	    char *)&cin[(icom - 1) * 72] == 'm') {
	++icom;
	mesh_(a, ia, &mp, &ml, &ms, &mr, &c__60, &c__4, &c__50, &icom, &jcom, 
		cin, rin, iin, kin, &idump, n, &ia[k[0] - 1], &a[k[1] - 1], &
		ia[k[2] - 1], &ia[k[3] - 1], &ia[k[4] - 1], &ia[k[5] - 1], &a[
		k[6] - 1], &ia[k[7] - 1], &ia[k[8] - 1], &ia[k[9] - 1], &ia[k[
		10] - 1], &ia[k[11] - 1], &ia[k[12] - 1], &ia[k[13] - 1], &ia[
		k[14] - 1], &ia[k[15] - 1], &ia[k[16] - 1], &ia[k[17] - 1], &
		ia[k[18] - 1], &ia[k[19] - 1], &ia[k[20] - 1], &ia[k[21] - 1],
		 &ia[k[22] - 1], &ia[k[23] - 1], &ia[k[24] - 1], &ia[k[25] - 
		1], &ia[k[26] - 1], &ia[k[27] - 1], &ia[k[28] - 1], &ia[k[29] 
		- 1], &ia[k[30] - 1], &ia[k[31] - 1], &ia[k[32] - 1], &ia[k[
		33] - 1], &ia[k[34] - 1], &ia[k[35] - 1], &ia[k[36] - 1], &ia[
		k[37] - 1], &ia[k[39] - 1], &ia[k[40] - 1], &ia[k[41] - 1], &
		ia[k[42] - 1], &ia[k[43] - 1], &ia[k[44] - 1], &ia[k[45] - 1],
		 &ia[k[46] - 1], &ia[k[47] - 1], &ia[k[48] - 1], &ia[k[59] - 
		1], &ia[k[60] - 1], &ia[k[61] - 1], &a[k[62] - 1], &ia[k[63] 
		- 1], &ia[k[64] - 1], &ia[k[65] - 1], &ia[k[66] - 1], ischm, 
		scheme, number, defsch, &defsiz, title, &optim, idev, &alpha, 
		dev1, &three, &eight, &nine, &batch, &vaxvms, versn, &axis, &
		areacg, &labn, &labe, &labo, &labnb, &labsb, &labm, &labw, &
		wrote, &time1, &hardpl, &exodusii, (ftnlen)72, (ftnlen)72, (
		ftnlen)80, (ftnlen)72, (ftnlen)72, (ftnlen)3, (ftnlen)10);
/*  SPAWN A PROCESS */
    } else if (s_cmp(cin + (icom - 1) * 72, "SP", (ftnlen)2, (ftnlen)2) == 0 
	    || s_cmp(cin + (icom - 1) * 72, "sp", (ftnlen)2, (ftnlen)2) == 0) 
	    {
	++icom;
	spawn_(&vaxvms);
/*  STRAIGHTEN OPTION - STRAIGHTEN LINES IN THE X OR Y DIRECTION */
    } else if (*(unsigned char *)&cin[(icom - 1) * 72] == 'S' || s_cmp(cin + (
	    icom - 1) * 72, "s", (ftnlen)2, (ftnlen)1) == 0) {
	++icom;
	strait_(&mp, &ml, &c__50, &icom, &jcom, cin, rin, iin, kin, &idump, n,
		 &a[k[1] - 1], &ia[k[7] - 1], &ia[k[39] - 1], &ia[k[40] - 1], 
		(ftnlen)72);
/*  TABLET DIGITIZE OPTION - DIGITIZE THE GEOMETRY */
    } else if (*(unsigned char *)&cin[(icom - 1) * 72] == 'T' || *(unsigned 
	    char *)&cin[(icom - 1) * 72] == 't') {
	++icom;
	merge = FALSE_;
L150:
	tablet_(&mp, &ml, &ms, &mr, &c__100, &c__50, &icom, &jcom, cin, rin, 
		iin, kin, &idump, n, &ia[k[0] - 1], &a[k[1] - 1], &ia[k[2] - 
		1], &ia[k[3] - 1], &ia[k[4] - 1], &ia[k[5] - 1], &a[k[6] - 1],
		 &ia[k[7] - 1], &ia[k[8] - 1], &ia[k[9] - 1], &ia[k[10] - 1], 
		&ia[k[11] - 1], &ia[k[12] - 1], &ia[k[13] - 1], &ia[k[20] - 1]
		, &ia[k[21] - 1], &ia[k[22] - 1], &ia[k[23] - 1], &ia[k[24] - 
		1], &ia[k[25] - 1], &ia[k[26] - 1], &ia[k[27] - 1], &ia[k[28] 
		- 1], &ia[k[29] - 1], &ia[k[30] - 1], &ia[k[31] - 1], &ia[k[
		32] - 1], &ia[k[33] - 1], &ia[k[34] - 1], &ia[k[35] - 1], &ia[
		k[36] - 1], &ia[k[37] - 1], &ia[k[39] - 1], &ia[k[40] - 1], &
		ia[k[41] - 1], &ia[k[43] - 1], &ia[k[44] - 1], &ia[k[46] - 1],
		 &ia[k[47] - 1], &ia[k[48] - 1], &a[k[49] - 1], &ia[k[50] - 1]
		, &ia[k[51] - 1], &ia[k[52] - 1], &ia[k[54] - 1], &ia[k[55] - 
		1], &ia[k[57] - 1], &ia[k[58] - 1], &ia[k[59] - 1], &ia[k[60] 
		- 1], &ia[k[61] - 1], &ia[k[66] - 1], title, &noroom, &drwtab,
		 &xx1, &yy1, &scale, &ct, &st, &x1, &x2, &y1, &y2, &alpha, 
		dev1, &snap, snapdx, nsnap, &vaxvms, &tbzoom, &axist, &wrote, 
		&batch, versn, &time1, (ftnlen)72, (ftnlen)72, (ftnlen)3, (
		ftnlen)10);
/*  EXTEND THE MEMORY AND CONTINUE IF USING DYNAMIC VARIABLE DIMENSIONING. */
/*  IN CONVERTING TO NORMAL VARIABLE DIMENSIONING, THE EXTEND MEMORY LINES */
/*  MUST BE TAKEN OUT, AND AN EXIT OF THE PROGRAM INPUT.  THEN THE */
/*  PARAMETER STATEMENT CONTAINING MP, ML, MS, AND MR MUST BE INCREASED TO */
/*  INCREASE DIMESIONING. */
	if (noroom) {
	    mpold = mp;
	    mlold = ml;
	    msold = ms;
	    mrold = mr;
	    d__1 = (doublereal) mp * (float)1.5000001;
	    mp = i_dnnt(&d__1);
	    d__1 = (doublereal) ml * (float)1.5000001;
	    ml = i_dnnt(&d__1);
	    d__1 = (doublereal) ms * (float)1.5000001;
	    ms = i_dnnt(&d__1);
	    d__1 = (doublereal) mr * (float)1.5000001;
	    mr = i_dnnt(&d__1);
	    mdlong_("IPOINT", k, &mp, (ftnlen)6);
	    i__1 = mp << 1;
	    mdlong_("COOR", &k[1], &i__1, (ftnlen)4);
	    mdlong_("IPBOUN", &k[2], &mp, (ftnlen)6);
	    mdlong_("ILINE", &k[3], &ml, (ftnlen)5);
	    mdlong_("LTYPE", &k[4], &ml, (ftnlen)5);
	    mdlong_("NINT", &k[5], &ml, (ftnlen)4);
	    mdlong_("FACTOR", &k[6], &ml, (ftnlen)6);
	    i__1 = ml * 3;
	    mdlong_("LCON", &k[7], &i__1, (ftnlen)4);
	    mdlong_("ILBOUN", &k[8], &ml, (ftnlen)6);
	    mdlong_("ISBOUN", &k[9], &ml, (ftnlen)6);
	    mdlong_("ISIDE", &k[10], &ms, (ftnlen)5);
	    mdlong_("NLPS", &k[11], &ms, (ftnlen)4);
	    mdlong_("IFLINE", &k[12], &ms, (ftnlen)6);
	    i__1 = ms * 3;
	    mdlong_("ILLIST", &k[13], &i__1, (ftnlen)6);
	    mdlong_("IBARST", &k[14], &ms, (ftnlen)6);
	    mdlong_("JMAT", &k[15], &ms, (ftnlen)4);
	    mdlong_("JCENT", &k[16], &ms, (ftnlen)5);
	    mdlong_("NLPB", &k[17], &ms, (ftnlen)4);
	    mdlong_("JFLINE", &k[18], &ms, (ftnlen)6);
	    i__1 = ms * 3;
	    mdlong_("JLLIST", &k[19], &i__1, (ftnlen)6);
	    mdlong_("IREGN", &k[20], &mr, (ftnlen)5);
	    mdlong_("IMAT", &k[21], &mr, (ftnlen)4);
	    mdlong_("NSPR", &k[22], &mr, (ftnlen)4);
	    mdlong_("IFSIDE", &k[23], &mr, (ftnlen)6);
	    i__1 = mr << 2;
	    mdlong_("ISLIST", &k[24], &i__1, (ftnlen)6);
	    mdlong_("IRPB", &k[25], &mr, (ftnlen)4);
	    mdlong_("IPBF", &k[26], &mp, (ftnlen)4);
	    mdlong_("NPPF", &k[27], &mp, (ftnlen)4);
	    mdlong_("IFPB", &k[28], &mp, (ftnlen)4);
	    i__1 = mp << 1;
	    mdlong_("LISTPB", &k[29], &i__1, (ftnlen)6);
	    mdlong_("ILBF", &k[30], &ml, (ftnlen)4);
	    mdlong_("NLPF", &k[31], &ml, (ftnlen)4);
	    mdlong_("IFLB", &k[32], &ml, (ftnlen)4);
	    i__1 = ml << 1;
	    mdlong_("LISTLB", &k[33], &i__1, (ftnlen)6);
	    mdlong_("ISBF", &k[34], &ml, (ftnlen)4);
	    mdlong_("NSPF", &k[35], &ml, (ftnlen)4);
	    mdlong_("IFSB", &k[36], &ml, (ftnlen)4);
	    i__1 = ml << 1;
	    mdlong_("LISTSB", &k[37], &i__1, (ftnlen)6);
	    i__1 = mr + ms << 2;
	    mdlong_("ATTRIB", &k[38], &i__1, (ftnlen)6);
	    i__1 = mp << 1;
	    mdlong_("LINKP", &k[39], &i__1, (ftnlen)5);
	    i__1 = ml << 1;
	    mdlong_("LINKL", &k[40], &i__1, (ftnlen)5);
	    i__1 = ms << 1;
	    mdlong_("LINKS", &k[41], &i__1, (ftnlen)5);
	    i__1 = ms << 1;
	    mdlong_("LINKB", &k[42], &i__1, (ftnlen)5);
	    i__1 = mr << 1;
	    mdlong_("LINKR", &k[43], &i__1, (ftnlen)5);
	    i__1 = ms + mr << 1;
	    mdlong_("LINKM", &k[44], &i__1, (ftnlen)5);
	    i__1 = mr << 1;
	    mdlong_("LINKSC", &k[45], &i__1, (ftnlen)6);
	    i__1 = mp << 1;
	    mdlong_("LINKPB", &k[46], &i__1, (ftnlen)6);
	    i__1 = ml << 1;
	    mdlong_("LINKLB", &k[47], &i__1, (ftnlen)6);
	    i__1 = ml << 1;
	    mdlong_("LINKSB", &k[48], &i__1, (ftnlen)6);
	    i__1 = mr << 2;
	    mdlong_("REXTRM", &k[49], &i__1, (ftnlen)6);
	    i__1 = mp << 1;
	    mdlong_("IHOLDP", &k[50], &i__1, (ftnlen)6);
	    i__1 = ml << 1;
	    mdlong_("IHOLDL", &k[51], &i__1, (ftnlen)6);
	    i__1 = ms << 1;
	    mdlong_("IHOLDS", &k[52], &i__1, (ftnlen)6);
	    i__1 = ms << 1;
	    mdlong_("IHOLDB", &k[53], &i__1, (ftnlen)6);
	    i__1 = mr << 1;
	    mdlong_("IHOLDR", &k[54], &i__1, (ftnlen)6);
	    i__1 = ms + mr << 1;
	    mdlong_("IHOLDM", &k[55], &i__1, (ftnlen)6);
	    i__1 = mp << 1;
	    mdlong_("IHOLD1", &k[56], &i__1, (ftnlen)6);
	    i__1 = ml << 1;
	    mdlong_("IHOLD2", &k[57], &i__1, (ftnlen)6);
	    i__1 = ml << 1;
	    mdlong_("IHOLD3", &k[58], &i__1, (ftnlen)6);
	    i__1 = mp * 3;
	    mdlong_("IWTPBF", &k[59], &i__1, (ftnlen)6);
	    i__1 = ml * 3;
	    mdlong_("IWTLBF", &k[60], &i__1, (ftnlen)6);
	    i__1 = ml * 3;
	    mdlong_("IWTSBF", &k[61], &i__1, (ftnlen)6);
	    mdlong_("RSIZE", &k[62], &mr, (ftnlen)5);
	    mdlong_("IFHOLE", &k[63], &mr, (ftnlen)6);
	    mdlong_("NHPR", &k[64], &mr, (ftnlen)4);
	    mdlong_("IHLIST", &k[65], &mr, (ftnlen)6);
	    mdlong_("IRGFLG", &k[66], &mr, (ftnlen)6);
	    mdstat_(&nerr, &mused);
	    if (nerr > 0) {
		mderor_(&c__6);
		s_stop(" ", (ftnlen)1);
	    }
/*  RESORT THE LINK ARRAYS */
	    ltnew_(&ml, &ia[k[50] - 1]);
	    ltadd_(&ml, &mlold, n, &ia[k[39] - 1], &ia[k[50] - 1]);
	    ltnew_(&ml, &ia[k[50] - 1]);
	    ltnew_(&ml, &ia[k[51] - 1]);
	    ltadd_(&ml, &mlold, &n[1], &ia[k[40] - 1], &ia[k[51] - 1]);
	    ltnew_(&ml, &ia[k[51] - 1]);
	    ltnew_(&ms, &ia[k[52] - 1]);
	    ltadd_(&ms, &msold, &n[2], &ia[k[41] - 1], &ia[k[52] - 1]);
	    ltnew_(&ms, &ia[k[52] - 1]);
	    ltnew_(&ms, &ia[k[53] - 1]);
	    ltadd_(&ms, &msold, &n[4], &ia[k[42] - 1], &ia[k[53] - 1]);
	    ltnew_(&ms, &ia[k[53] - 1]);
	    ltnew_(&mr, &ia[k[54] - 1]);
	    ltadd_(&mr, &mrold, &n[6], &ia[k[43] - 1], &ia[k[54] - 1]);
	    i__1 = ms + mr;
	    ltnew_(&i__1, &ia[k[55] - 1]);
	    i__1 = ms + mr;
	    i__2 = msold + mrold;
	    i__3 = msold + mrold;
	    ltadd_(&i__1, &i__2, &i__3, &ia[k[44] - 1], &ia[k[55] - 1]);
	    i__1 = ms + mr;
	    ltnew_(&i__1, &ia[k[55] - 1]);
	    ltnew_(&mr, &ia[k[54] - 1]);
	    ltadd_(&mr, &mrold, &n[7], &ia[k[45] - 1], &ia[k[54] - 1]);
	    ltnew_(&mr, &ia[k[54] - 1]);
	    ltnew_(&mp, &ia[k[56] - 1]);
	    ltadd_(&mp, &mpold, &n[10], &ia[k[46] - 1], &ia[k[56] - 1]);
	    ltnew_(&mp, &ia[k[56] - 1]);
	    ltnew_(&ml, &ia[k[57] - 1]);
	    ltadd_(&ml, &mlold, &n[12], &ia[k[47] - 1], &ia[k[57] - 1]);
	    ltnew_(&ml, &ia[k[57] - 1]);
	    ltnew_(&ml, &ia[k[58] - 1]);
	    ltadd_(&ml, &mlold, &n[14], &ia[k[48] - 1], &ia[k[58] - 1]);
	    mesage_("DIGITIZATION CAN NOW BE CONTINUED", (ftnlen)33);
	    goto L150;
	}
	wrote = FALSE_;
/*  KEY-IN OPTION - TYPE IN THE DATA FROM THE KEYBOARD */
    } else if (*(unsigned char *)&cin[(icom - 1) * 72] == 'K' || *(unsigned 
	    char *)&cin[(icom - 1) * 72] == 'k') {
	++icom;
	merge = FALSE_;
	wrote = FALSE_;
L160:
	keyin_(&mp, &ml, &ms, &mr, &c__60, &c__4, &c__50, &icom, &jcom, cin, 
		rin, iin, kin, &idump, n, &ia[k[0] - 1], &a[k[1] - 1], &ia[k[
		2] - 1], &ia[k[3] - 1], &ia[k[4] - 1], &ia[k[5] - 1], &a[k[6] 
		- 1], &ia[k[7] - 1], &ia[k[8] - 1], &ia[k[9] - 1], &ia[k[10] 
		- 1], &ia[k[11] - 1], &ia[k[12] - 1], &ia[k[13] - 1], &ia[k[
		14] - 1], &ia[k[15] - 1], &ia[k[16] - 1], &ia[k[17] - 1], &ia[
		k[18] - 1], &ia[k[19] - 1], &ia[k[20] - 1], &ia[k[21] - 1], &
		ia[k[22] - 1], &ia[k[23] - 1], &ia[k[24] - 1], &ia[k[25] - 1],
		 &ia[k[26] - 1], &ia[k[27] - 1], &ia[k[28] - 1], &ia[k[29] - 
		1], &ia[k[30] - 1], &ia[k[31] - 1], &ia[k[32] - 1], &ia[k[33] 
		- 1], &ia[k[34] - 1], &ia[k[35] - 1], &ia[k[36] - 1], &ia[k[
		37] - 1], &ia[k[39] - 1], &ia[k[40] - 1], &ia[k[41] - 1], &ia[
		k[42] - 1], &ia[k[43] - 1], &ia[k[44] - 1], &ia[k[45] - 1], &
		ia[k[46] - 1], &ia[k[47] - 1], &ia[k[48] - 1], &ia[k[50] - 1],
		 &ia[k[51] - 1], &ia[k[52] - 1], &ia[k[53] - 1], &ia[k[54] - 
		1], &ia[k[55] - 1], &ia[k[56] - 1], &ia[k[57] - 1], &ia[k[58] 
		- 1], &ia[k[59] - 1], &ia[k[60] - 1], &ia[k[61] - 1], &a[k[62]
		 - 1], &ia[k[63] - 1], &ia[k[64] - 1], &ia[k[65] - 1], &ia[k[
		66] - 1], ischm, scheme, number, defsch, &defsiz, title, &
		optim, &three, &eight, &nine, &noroom, &vaxvms, &wrote, &
		time1, versn, &batch, (ftnlen)72, (ftnlen)72, (ftnlen)80, (
		ftnlen)72, (ftnlen)72, (ftnlen)10);
	if (noroom) {
	    mpold = mp;
	    mlold = ml;
	    msold = ms;
	    mrold = mr;
	    d__1 = (doublereal) mp * (float)1.5000001;
	    mp = i_dnnt(&d__1);
	    d__1 = (doublereal) ml * (float)1.5000001;
	    ml = i_dnnt(&d__1);
	    d__1 = (doublereal) ms * (float)1.5000001;
	    ms = i_dnnt(&d__1);
	    d__1 = (doublereal) mr * (float)1.5000001;
	    mr = i_dnnt(&d__1);
	    mdlong_("IPOINT", k, &mp, (ftnlen)6);
	    i__1 = mp << 1;
	    mdlong_("COOR", &k[1], &i__1, (ftnlen)4);
	    mdlong_("IPBOUN", &k[2], &mp, (ftnlen)6);
	    mdlong_("ILINE", &k[3], &ml, (ftnlen)5);
	    mdlong_("LTYPE", &k[4], &ml, (ftnlen)5);
	    mdlong_("NINT", &k[5], &ml, (ftnlen)4);
	    mdlong_("FACTOR", &k[6], &ml, (ftnlen)6);
	    i__1 = ml * 3;
	    mdlong_("LCON", &k[7], &i__1, (ftnlen)4);
	    mdlong_("ILBOUN", &k[8], &ml, (ftnlen)6);
	    mdlong_("ISBOUN", &k[9], &ml, (ftnlen)6);
	    mdlong_("ISIDE", &k[10], &ms, (ftnlen)5);
	    mdlong_("NLPS", &k[11], &ms, (ftnlen)4);
	    mdlong_("IFLINE", &k[12], &ms, (ftnlen)6);
	    i__1 = ms * 3;
	    mdlong_("ILLIST", &k[13], &i__1, (ftnlen)6);
	    mdlong_("IBARST", &k[14], &ms, (ftnlen)6);
	    mdlong_("JMAT", &k[15], &ms, (ftnlen)4);
	    mdlong_("JCENT", &k[16], &ms, (ftnlen)5);
	    mdlong_("NLPB", &k[17], &ms, (ftnlen)4);
	    mdlong_("JFLINE", &k[18], &ms, (ftnlen)6);
	    i__1 = ms * 3;
	    mdlong_("JLLIST", &k[19], &i__1, (ftnlen)6);
	    mdlong_("IREGN", &k[20], &mr, (ftnlen)5);
	    mdlong_("IMAT", &k[21], &mr, (ftnlen)4);
	    mdlong_("NSPR", &k[22], &mr, (ftnlen)4);
	    mdlong_("IFSIDE", &k[23], &mr, (ftnlen)6);
	    i__1 = mr << 2;
	    mdlong_("ISLIST", &k[24], &i__1, (ftnlen)6);
	    mdlong_("IRPB", &k[25], &mr, (ftnlen)4);
	    mdlong_("IPBF", &k[26], &mp, (ftnlen)4);
	    mdlong_("NPPF", &k[27], &mp, (ftnlen)4);
	    mdlong_("IFPB", &k[28], &mp, (ftnlen)4);
	    i__1 = mp << 1;
	    mdlong_("LISTPB", &k[29], &i__1, (ftnlen)6);
	    mdlong_("ILBF", &k[30], &ml, (ftnlen)4);
	    mdlong_("NLPF", &k[31], &ml, (ftnlen)4);
	    mdlong_("IFLB", &k[32], &ml, (ftnlen)4);
	    i__1 = ml << 1;
	    mdlong_("LISTLB", &k[33], &i__1, (ftnlen)6);
	    mdlong_("ISBF", &k[34], &ml, (ftnlen)4);
	    mdlong_("NSPF", &k[35], &ml, (ftnlen)4);
	    mdlong_("IFSB", &k[36], &ml, (ftnlen)4);
	    i__1 = ml << 1;
	    mdlong_("LISTSB", &k[37], &i__1, (ftnlen)6);
	    i__1 = mr + ms << 2;
	    mdlong_("ATTRIB", &k[38], &i__1, (ftnlen)6);
	    i__1 = mp << 1;
	    mdlong_("LINKP", &k[39], &i__1, (ftnlen)5);
	    i__1 = ml << 1;
	    mdlong_("LINKL", &k[40], &i__1, (ftnlen)5);
	    i__1 = ms << 1;
	    mdlong_("LINKS", &k[41], &i__1, (ftnlen)5);
	    i__1 = ms << 1;
	    mdlong_("LINKB", &k[42], &i__1, (ftnlen)5);
	    i__1 = mr << 1;
	    mdlong_("LINKR", &k[43], &i__1, (ftnlen)5);
	    i__1 = ms + mr << 1;
	    mdlong_("LINKM", &k[44], &i__1, (ftnlen)5);
	    i__1 = mr << 1;
	    mdlong_("LINKSC", &k[45], &i__1, (ftnlen)6);
	    i__1 = mp << 1;
	    mdlong_("LINKPB", &k[46], &i__1, (ftnlen)6);
	    i__1 = ml << 1;
	    mdlong_("LINKLB", &k[47], &i__1, (ftnlen)6);
	    i__1 = ml << 1;
	    mdlong_("LINKSB", &k[48], &i__1, (ftnlen)6);
	    i__1 = mr << 2;
	    mdlong_("REXTRM", &k[49], &i__1, (ftnlen)6);
	    i__1 = mp << 1;
	    mdlong_("IHOLDP", &k[50], &i__1, (ftnlen)6);
	    i__1 = ml << 1;
	    mdlong_("IHOLDL", &k[51], &i__1, (ftnlen)6);
	    i__1 = ms << 1;
	    mdlong_("IHOLDS", &k[52], &i__1, (ftnlen)6);
	    i__1 = ms << 1;
	    mdlong_("IHOLDB", &k[53], &i__1, (ftnlen)6);
	    i__1 = mr << 1;
	    mdlong_("IHOLDR", &k[54], &i__1, (ftnlen)6);
	    i__1 = ms + mr << 1;
	    mdlong_("IHOLDM", &k[55], &i__1, (ftnlen)6);
	    i__1 = mp << 1;
	    mdlong_("IHOLD1", &k[56], &i__1, (ftnlen)6);
	    i__1 = ml << 1;
	    mdlong_("IHOLD2", &k[57], &i__1, (ftnlen)6);
	    i__1 = ml << 1;
	    mdlong_("IHOLD3", &k[58], &i__1, (ftnlen)6);
	    i__1 = mp * 3;
	    mdlong_("IWTPBF", &k[59], &i__1, (ftnlen)6);
	    i__1 = ml * 3;
	    mdlong_("IWTLBF", &k[60], &i__1, (ftnlen)6);
	    i__1 = ml * 3;
	    mdlong_("IWTSBF", &k[61], &i__1, (ftnlen)6);
	    mdlong_("RSIZE", &k[62], &mr, (ftnlen)5);
	    mdlong_("IFHOLE", &k[63], &mr, (ftnlen)6);
	    mdlong_("NHPR", &k[64], &mr, (ftnlen)4);
	    mdlong_("IHLIST", &k[65], &mr, (ftnlen)6);
	    mdlong_("IRGFLG", &k[66], &mr, (ftnlen)6);
	    mdstat_(&nerr, &mused);
	    if (nerr > 0) {
		mderor_(&c__6);
		s_stop(" ", (ftnlen)1);
	    }
/*  RESORT THE LINK ARRAYS */
	    ltnew_(&ml, &ia[k[50] - 1]);
	    ltadd_(&ml, &mlold, n, &ia[k[39] - 1], &ia[k[50] - 1]);
	    ltnew_(&ml, &ia[k[50] - 1]);
	    ltnew_(&ml, &ia[k[51] - 1]);
	    ltadd_(&ml, &mlold, &n[1], &ia[k[40] - 1], &ia[k[51] - 1]);
	    ltnew_(&ml, &ia[k[51] - 1]);
	    ltnew_(&ms, &ia[k[52] - 1]);
	    ltadd_(&ms, &msold, &n[2], &ia[k[41] - 1], &ia[k[52] - 1]);
	    ltnew_(&ms, &ia[k[52] - 1]);
	    ltnew_(&ms, &ia[k[53] - 1]);
	    ltadd_(&ms, &msold, &n[4], &ia[k[42] - 1], &ia[k[53] - 1]);
	    ltnew_(&ms, &ia[k[53] - 1]);
	    ltnew_(&mr, &ia[k[54] - 1]);
	    ltadd_(&mr, &mrold, &n[6], &ia[k[43] - 1], &ia[k[54] - 1]);
	    i__1 = ms + mr;
	    ltnew_(&i__1, &ia[k[55] - 1]);
	    i__1 = ms + mr;
	    i__2 = msold + mrold;
	    i__3 = msold + mrold;
	    ltadd_(&i__1, &i__2, &i__3, &ia[k[44] - 1], &ia[k[55] - 1]);
	    i__1 = ms + mr;
	    ltnew_(&i__1, &ia[k[55] - 1]);
	    ltnew_(&mr, &ia[k[54] - 1]);
	    ltadd_(&mr, &mrold, &n[7], &ia[k[45] - 1], &ia[k[54] - 1]);
	    ltnew_(&mr, &ia[k[54] - 1]);
	    ltnew_(&mp, &ia[k[56] - 1]);
	    ltadd_(&mp, &mpold, &n[10], &ia[k[46] - 1], &ia[k[56] - 1]);
	    ltnew_(&mp, &ia[k[56] - 1]);
	    ltnew_(&ml, &ia[k[57] - 1]);
	    ltadd_(&ml, &mlold, &n[12], &ia[k[47] - 1], &ia[k[57] - 1]);
	    ltnew_(&ml, &ia[k[57] - 1]);
	    ltnew_(&ml, &ia[k[58] - 1]);
	    ltadd_(&ml, &mlold, &n[14], &ia[k[48] - 1], &ia[k[58] - 1]);
	    mesage_("KEYIN OPTION CAN NOW BE CONTINUED", (ftnlen)33);
	    goto L160;
	}
/*  LIST OPTION - LISTS FASTQ DATA */
    } else if (*(unsigned char *)&cin[(icom - 1) * 72] == 'L' || *(unsigned 
	    char *)&cin[(icom - 1) * 72] == 'l') {
	++icom;
	list_(&mp, &ml, &ms, &mr, &c__60, &c__50, &icom, &jcom, cin, rin, iin,
		 kin, n, &ia[k[0] - 1], &a[k[1] - 1], &ia[k[2] - 1], &ia[k[3] 
		- 1], &ia[k[4] - 1], &ia[k[5] - 1], &a[k[6] - 1], &ia[k[7] - 
		1], &ia[k[8] - 1], &ia[k[9] - 1], &ia[k[10] - 1], &ia[k[11] - 
		1], &ia[k[12] - 1], &ia[k[13] - 1], &ia[k[14] - 1], &ia[k[15] 
		- 1], &ia[k[16] - 1], &ia[k[17] - 1], &ia[k[18] - 1], &ia[k[
		19] - 1], &ia[k[20] - 1], &ia[k[21] - 1], &ia[k[22] - 1], &ia[
		k[23] - 1], &ia[k[24] - 1], &ia[k[25] - 1], &ia[k[26] - 1], &
		ia[k[27] - 1], &ia[k[28] - 1], &ia[k[29] - 1], &ia[k[30] - 1],
		 &ia[k[31] - 1], &ia[k[32] - 1], &ia[k[33] - 1], &ia[k[34] - 
		1], &ia[k[35] - 1], &ia[k[36] - 1], &ia[k[37] - 1], &ia[k[39] 
		- 1], &ia[k[40] - 1], &ia[k[41] - 1], &ia[k[42] - 1], &ia[k[
		43] - 1], &ia[k[45] - 1], &ia[k[46] - 1], &ia[k[47] - 1], &ia[
		k[48] - 1], &ia[k[59] - 1], &ia[k[60] - 1], &ia[k[61] - 1], &
		a[k[62] - 1], &ia[k[63] - 1], &ia[k[64] - 1], &ia[k[65] - 1], 
		&ia[k[66] - 1], ischm, scheme, number, defsch, &defsiz, title,
		 &optim, &three, &eight, &nine, &vaxvms, &wrote, &time1, 
		versn, &batch, (ftnlen)72, (ftnlen)72, (ftnlen)80, (ftnlen)72,
		 (ftnlen)72, (ftnlen)10);
/*  READ OPTION - READS FASTQ DATA */
    } else if ((*(unsigned char *)&cin[(icom - 1) * 72] == 'R' || *(unsigned 
	    char *)&cin[(icom - 1) * 72] == 'r') && *(unsigned char *)&cin[(
	    icom - 1) * 72 + 1] != 'P' && *(unsigned char *)&cin[(icom - 1) * 
	    72 + 1] != 'p' && *(unsigned char *)&cin[(icom - 1) * 72 + 1] != 
	    'W' && *(unsigned char *)&cin[(icom - 1) * 72 + 1] != 'w') {
	++icom;
	if (n[0] > 0 || n[1] > 0) {
/*  CHECK TO SEE IF A FASTQ DATA MERGING IS DESIRED */
	    intrup_("MERGE FILE WITH EXISTING DATA", &merge, &c__50, &icom, &
		    jcom, cin, iin, rin, kin, (ftnlen)29, (ftnlen)72);
	    if (merge) {
		ltnew_(&mp, &ia[k[50] - 1]);
		ltnew_(&ml, &ia[k[51] - 1]);
		ltnew_(&ms, &ia[k[52] - 1]);
		ltnew_(&ms, &ia[k[53] - 1]);
		ltnew_(&mr, &ia[k[54] - 1]);
		i__1 = ms + mr;
		ltnew_(&i__1, &ia[k[55] - 1]);
		ltnew_(&mp, &ia[k[56] - 1]);
		ltnew_(&ml, &ia[k[57] - 1]);
		ltnew_(&ml, &ia[k[58] - 1]);
	    } else {
		if (! wrote) {
		    mesage_("CONTINUATION WILL OVERWRITE OLD DATA", (ftnlen)
			    36);
		    intrup_("DO YOU WISH TO CONTINUE", &ians, &c__50, &icom, &
			    jcom, cin, iin, rin, kin, (ftnlen)23, (ftnlen)72);
		    if (! ians) {
			goto L120;
		    }
		}
		for (i__ = 1; i__ <= 29; ++i__) {
		    n[i__ - 1] = 0;
/* L170: */
		}
		ltnew_(&mp, &ia[k[39] - 1]);
		ltnew_(&ml, &ia[k[40] - 1]);
		ltnew_(&ms, &ia[k[41] - 1]);
		ltnew_(&ms, &ia[k[42] - 1]);
		ltnew_(&mr, &ia[k[43] - 1]);
		i__1 = ms + mr;
		ltnew_(&i__1, &ia[k[44] - 1]);
		ltnew_(&mr, &ia[k[45] - 1]);
		ltnew_(&mp, &ia[k[46] - 1]);
		ltnew_(&ml, &ia[k[47] - 1]);
		ltnew_(&ml, &ia[k[48] - 1]);
	    }
	}
	iunit = 1;
	itry = 0;
L180:
	if (itry > 0) {
	    strlng_(fname, &len, (ftnlen)2048);
	    if (s_cmp(fname, "FOR001", (ftnlen)6, (ftnlen)6) != 0) {
		s_wsfe(&io___115);
		do_fio(&c__1, fname, len);
		e_wsfe();
	    }
	}
	++itry;
	if (itry <= 3 && ! batch || batch && itry <= 1 || start && ! batch) {
	    iunit = 1;
	    if (batch) {
		idump = 6;
		merge = FALSE_;
		s_copy(cin, "MESH", (ftnlen)72, (ftnlen)4);
		s_copy(cin + 72, "PROC", (ftnlen)72, (ftnlen)4);
		s_copy(cin + 144, "WRITE", (ftnlen)72, (ftnlen)5);
		s_copy(cin + 216, "EXIT", (ftnlen)72, (ftnlen)4);
		icom = 1;
		jcom = 4;
		exname_(&iunit, fname, &ln, (ftnlen)2048);
		o__1.oerr = 1;
		o__1.ounit = iunit;
		o__1.ofnmlen = ln;
		o__1.ofnm = fname;
		o__1.orl = 0;
		o__1.osta = "OLD";
		o__1.oacc = 0;
		o__1.ofm = 0;
		o__1.oblnk = 0;
		i__1 = f_open(&o__1);
		if (i__1 != 0) {
		    goto L180;
		}
	    } else if (start) {
		start = FALSE_;
		exname_(&iunit, fname, &ln, (ftnlen)2048);
		if (! vaxvms && s_cmp(fname, "tty", (ftnlen)2048, (ftnlen)3) 
			== 0) {
		    goto L120;
		}
		o__1.oerr = 1;
		o__1.ounit = iunit;
		o__1.ofnmlen = ln;
		o__1.ofnm = fname;
		o__1.orl = 0;
		o__1.osta = "OLD";
		o__1.oacc = 0;
		o__1.ofm = 0;
		o__1.oblnk = 0;
		i__1 = f_open(&o__1);
		if (i__1 != 0) {
		    goto L120;
		}
	    } else {
		if (icom <= jcom) {
		    s_copy(fname, cin + (icom - 1) * 72, (ftnlen)2048, (
			    ftnlen)72);
		    ++icom;
		} else {
		    inqstr_("INPUT FILE: ", fname, (ftnlen)12, (ftnlen)2048);
		}
		o__1.oerr = 1;
		o__1.ounit = iunit;
		o__1.ofnmlen = 2048;
		o__1.ofnm = fname;
		o__1.orl = 0;
		o__1.osta = "OLD";
		o__1.oacc = 0;
		o__1.ofm = 0;
		o__1.oblnk = 0;
		i__1 = f_open(&o__1);
		if (i__1 != 0) {
		    goto L180;
		}
		idump = 0;
	    }
L190:
	    al__1.aerr = 0;
	    al__1.aunit = iunit;
	    f_rew(&al__1);
	    rdfsq_(&mp, &ml, &ms, &mr, &c__100, &c__60, &c__4, &iunit, &idump,
		     n, &ia[k[0] - 1], &a[k[1] - 1], &ia[k[2] - 1], &ia[k[3] 
		    - 1], &ia[k[4] - 1], &ia[k[5] - 1], &a[k[6] - 1], &ia[k[7]
		     - 1], &ia[k[8] - 1], &ia[k[9] - 1], &ia[k[10] - 1], &ia[
		    k[11] - 1], &ia[k[12] - 1], &ia[k[13] - 1], &ia[k[14] - 1]
		    , &ia[k[15] - 1], &ia[k[16] - 1], &ia[k[17] - 1], &ia[k[
		    18] - 1], &ia[k[19] - 1], &ia[k[20] - 1], &ia[k[21] - 1], 
		    &ia[k[22] - 1], &ia[k[23] - 1], &ia[k[24] - 1], &ia[k[25] 
		    - 1], &ia[k[26] - 1], &ia[k[27] - 1], &ia[k[28] - 1], &ia[
		    k[29] - 1], &ia[k[30] - 1], &ia[k[31] - 1], &ia[k[32] - 1]
		    , &ia[k[33] - 1], &ia[k[34] - 1], &ia[k[35] - 1], &ia[k[
		    36] - 1], &ia[k[37] - 1], &a[k[38] - 1], &ia[k[39] - 1], &
		    ia[k[40] - 1], &ia[k[41] - 1], &ia[k[42] - 1], &ia[k[43] 
		    - 1], &ia[k[44] - 1], &ia[k[45] - 1], &ia[k[46] - 1], &ia[
		    k[47] - 1], &ia[k[48] - 1], &ia[k[50] - 1], &ia[k[51] - 1]
		    , &ia[k[52] - 1], &ia[k[53] - 1], &ia[k[54] - 1], &ia[k[
		    55] - 1], &ia[k[56] - 1], &ia[k[57] - 1], &ia[k[58] - 1], 
		    &ia[k[59] - 1], &ia[k[60] - 1], &ia[k[61] - 1], &a[k[62] 
		    - 1], &ia[k[63] - 1], &ia[k[64] - 1], &ia[k[65] - 1], &ia[
		    k[66] - 1], ischm, scheme, number, defsch, &defsiz, title,
		     &optim, &merge, &three, &eight, &nine, &snap, snapdx, 
		    nsnap, &ratio, &noroom, &exodusii, (ftnlen)72, (ftnlen)80,
		     (ftnlen)72, (ftnlen)72);
	    if (noroom) {
		mpold = mp;
		mlold = ml;
		msold = ms;
		mrold = mr;
		d__1 = (doublereal) mp * ratio;
		mp = i_dnnt(&d__1);
		d__1 = (doublereal) ml * ratio;
		ml = i_dnnt(&d__1);
		d__1 = (doublereal) ms * ratio;
		ms = i_dnnt(&d__1);
		d__1 = (doublereal) mr * ratio;
		mr = i_dnnt(&d__1);
		mdlong_("IPOINT", k, &mp, (ftnlen)6);
		i__1 = mp << 1;
		mdlong_("COOR", &k[1], &i__1, (ftnlen)4);
		mdlong_("IPBOUN", &k[2], &mp, (ftnlen)6);
		mdlong_("ILINE", &k[3], &ml, (ftnlen)5);
		mdlong_("LTYPE", &k[4], &ml, (ftnlen)5);
		mdlong_("NINT", &k[5], &ml, (ftnlen)4);
		mdlong_("FACTOR", &k[6], &ml, (ftnlen)6);
		i__1 = ml * 3;
		mdlong_("LCON", &k[7], &i__1, (ftnlen)4);
		mdlong_("ILBOUN", &k[8], &ml, (ftnlen)6);
		mdlong_("ISBOUN", &k[9], &ml, (ftnlen)6);
		mdlong_("ISIDE", &k[10], &ms, (ftnlen)5);
		mdlong_("NLPS", &k[11], &ms, (ftnlen)4);
		mdlong_("IFLINE", &k[12], &ms, (ftnlen)6);
		i__1 = ms * 3;
		mdlong_("ILLIST", &k[13], &i__1, (ftnlen)6);
		mdlong_("IBARST", &k[14], &ms, (ftnlen)6);
		mdlong_("JMAT", &k[15], &ms, (ftnlen)4);
		mdlong_("JCENT", &k[16], &ms, (ftnlen)5);
		mdlong_("NLPB", &k[17], &ms, (ftnlen)4);
		mdlong_("JFLINE", &k[18], &ms, (ftnlen)6);
		i__1 = ms * 3;
		mdlong_("JLLIST", &k[19], &i__1, (ftnlen)6);
		mdlong_("IREGN", &k[20], &mr, (ftnlen)5);
		mdlong_("IMAT", &k[21], &mr, (ftnlen)4);
		mdlong_("NSPR", &k[22], &mr, (ftnlen)4);
		mdlong_("IFSIDE", &k[23], &mr, (ftnlen)6);
		i__1 = mr << 2;
		mdlong_("ISLIST", &k[24], &i__1, (ftnlen)6);
		mdlong_("IRPB", &k[25], &mr, (ftnlen)4);
		mdlong_("IPBF", &k[26], &mp, (ftnlen)4);
		mdlong_("NPPF", &k[27], &mp, (ftnlen)4);
		mdlong_("IFPB", &k[28], &mp, (ftnlen)4);
		i__1 = mp << 1;
		mdlong_("LISTPB", &k[29], &i__1, (ftnlen)6);
		mdlong_("ILBF", &k[30], &ml, (ftnlen)4);
		mdlong_("NLPF", &k[31], &ml, (ftnlen)4);
		mdlong_("IFLB", &k[32], &ml, (ftnlen)4);
		i__1 = ml << 1;
		mdlong_("LISTLB", &k[33], &i__1, (ftnlen)6);
		mdlong_("ISBF", &k[34], &ml, (ftnlen)4);
		mdlong_("NSPF", &k[35], &ml, (ftnlen)4);
		mdlong_("IFSB", &k[36], &ml, (ftnlen)4);
		i__1 = ml << 1;
		mdlong_("LISTSB", &k[37], &i__1, (ftnlen)6);
		i__1 = mr + ms << 2;
		mdlong_("ATTRIB", &k[38], &i__1, (ftnlen)6);
		i__1 = mp << 1;
		mdlong_("LINKP", &k[39], &i__1, (ftnlen)5);
		i__1 = ml << 1;
		mdlong_("LINKL", &k[40], &i__1, (ftnlen)5);
		i__1 = ms << 1;
		mdlong_("LINKS", &k[41], &i__1, (ftnlen)5);
		i__1 = ms << 1;
		mdlong_("LINKB", &k[42], &i__1, (ftnlen)5);
		i__1 = mr << 1;
		mdlong_("LINKR", &k[43], &i__1, (ftnlen)5);
		i__1 = ms + mr << 1;
		mdlong_("LINKM", &k[44], &i__1, (ftnlen)5);
		i__1 = mr << 1;
		mdlong_("LINKSC", &k[45], &i__1, (ftnlen)6);
		i__1 = mp << 1;
		mdlong_("LINKPB", &k[46], &i__1, (ftnlen)6);
		i__1 = ml << 1;
		mdlong_("LINKLB", &k[47], &i__1, (ftnlen)6);
		i__1 = ml << 1;
		mdlong_("LINKSB", &k[48], &i__1, (ftnlen)6);
		i__1 = mr << 2;
		mdlong_("REXTRM", &k[49], &i__1, (ftnlen)6);
		i__1 = mp << 1;
		mdlong_("IHOLDP", &k[50], &i__1, (ftnlen)6);
		i__1 = ml << 1;
		mdlong_("IHOLDL", &k[51], &i__1, (ftnlen)6);
		i__1 = ms << 1;
		mdlong_("IHOLDS", &k[52], &i__1, (ftnlen)6);
		i__1 = ms << 1;
		mdlong_("IHOLDB", &k[53], &i__1, (ftnlen)6);
		i__1 = mr << 1;
		mdlong_("IHOLDR", &k[54], &i__1, (ftnlen)6);
		i__1 = ms + mr << 1;
		mdlong_("IHOLDM", &k[55], &i__1, (ftnlen)6);
		i__1 = mp << 1;
		mdlong_("IHOLD1", &k[56], &i__1, (ftnlen)6);
		i__1 = ml << 1;
		mdlong_("IHOLD2", &k[57], &i__1, (ftnlen)6);
		i__1 = ml << 1;
		mdlong_("IHOLD3", &k[58], &i__1, (ftnlen)6);
		i__1 = mp * 3;
		mdlong_("IWTPBF", &k[59], &i__1, (ftnlen)6);
		i__1 = ml * 3;
		mdlong_("IWTLBF", &k[60], &i__1, (ftnlen)6);
		i__1 = ml * 3;
		mdlong_("IWTSBF", &k[61], &i__1, (ftnlen)6);
		mdlong_("RSIZE", &k[62], &mr, (ftnlen)5);
		mdlong_("IFHOLE", &k[63], &mr, (ftnlen)6);
		mdlong_("NHPR", &k[64], &mr, (ftnlen)4);
		mdlong_("IHLIST", &k[65], &mr, (ftnlen)6);
		mdlong_("IRGFLG", &k[66], &mr, (ftnlen)6);
		mdstat_(&nerr, &mused);
		if (nerr > 0) {
		    mderor_(&c__6);
		    s_stop(" ", (ftnlen)1);
		}
/*  RESORT THE LINK ARRAYS */
		ltnew_(&ml, &ia[k[50] - 1]);
		ltadd_(&ml, &mlold, n, &ia[k[39] - 1], &ia[k[50] - 1]);
		ltnew_(&ml, &ia[k[50] - 1]);
		ltnew_(&ml, &ia[k[51] - 1]);
		ltadd_(&ml, &mlold, &n[1], &ia[k[40] - 1], &ia[k[51] - 1]);
		ltnew_(&ml, &ia[k[51] - 1]);
		ltnew_(&ms, &ia[k[52] - 1]);
		ltadd_(&ms, &msold, &n[2], &ia[k[41] - 1], &ia[k[52] - 1]);
		ltnew_(&ms, &ia[k[52] - 1]);
		ltnew_(&ms, &ia[k[53] - 1]);
		ltadd_(&ms, &msold, &n[4], &ia[k[42] - 1], &ia[k[53] - 1]);
		ltnew_(&ms, &ia[k[53] - 1]);
		ltnew_(&mr, &ia[k[54] - 1]);
		ltadd_(&mr, &mrold, &n[6], &ia[k[43] - 1], &ia[k[54] - 1]);
		i__1 = ms + mr;
		ltnew_(&i__1, &ia[k[55] - 1]);
		i__1 = ms + mr;
		i__2 = msold + mrold;
		i__3 = msold + mrold;
		ltadd_(&i__1, &i__2, &i__3, &ia[k[44] - 1], &ia[k[55] - 1]);
		i__1 = ms + mr;
		ltnew_(&i__1, &ia[k[55] - 1]);
		ltnew_(&mr, &ia[k[54] - 1]);
		ltadd_(&mr, &mrold, &n[7], &ia[k[45] - 1], &ia[k[54] - 1]);
		ltnew_(&mr, &ia[k[54] - 1]);
		ltnew_(&mp, &ia[k[56] - 1]);
		ltadd_(&mp, &mpold, &n[10], &ia[k[46] - 1], &ia[k[56] - 1]);
		ltnew_(&mp, &ia[k[56] - 1]);
		ltnew_(&ml, &ia[k[57] - 1]);
		ltadd_(&ml, &mlold, &n[12], &ia[k[47] - 1], &ia[k[57] - 1]);
		ltnew_(&ml, &ia[k[57] - 1]);
		ltnew_(&ml, &ia[k[58] - 1]);
		ltadd_(&ml, &mlold, &n[14], &ia[k[48] - 1], &ia[k[58] - 1]);
		mesage_("FILE WILL NOW BE READ AGAIN AS NEW INPUT", (ftnlen)
			40);
		goto L190;
	    }
	    tbzoom = FALSE_;
	    al__1.aerr = 0;
	    al__1.aunit = iunit;
	    f_rew(&al__1);
	    cl__1.cerr = 0;
	    cl__1.cunit = iunit;
	    cl__1.csta = 0;
	    f_clos(&cl__1);
	}
/*  EXIT OPTION - EXITS FASTQ */
    } else if (s_cmp(cin + (icom - 1) * 72, "EX", (ftnlen)2, (ftnlen)2) == 0 
	    || s_cmp(cin + (icom - 1) * 72, "ex", (ftnlen)2, (ftnlen)2) == 0) 
	    {
	++icom;
	strlng_(cin + (icom - 1) * 72, &len, (ftnlen)72);
	if (len > 1 && *(unsigned char *)&cin[(icom - 1) * 72 + 1] != 'X' || 
		len > 1 && *(unsigned char *)&cin[(icom - 1) * 72 + 1] != 'x')
		 {
	    help_fq__(&c__6);
	} else {
	    fexit_(&wrote, &c__50, &icom, &jcom, cin, iin, rin, kin, &time1, &
		    batch, versn, (ftnlen)72, (ftnlen)10);
	}
	goto L120;
/*  WRITE OPTION - WRITES A FASTQ DATA FILE */
    } else if (*(unsigned char *)&cin[(icom - 1) * 72] == 'W' || *(unsigned 
	    char *)&cin[(icom - 1) * 72] == 'w' || s_cmp(cin + (icom - 1) * 
	    72, "BW", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(cin + (icom - 1) * 
	    72, "bw", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(cin + (icom - 1) * 
	    72, "RW", (ftnlen)2, (ftnlen)2) == 0 || s_cmp(cin + (icom - 1) * 
	    72, "rw", (ftnlen)2, (ftnlen)2) == 0) {
	if (s_cmp(cin + (icom - 1) * 72, "RW", (ftnlen)2, (ftnlen)2) == 0 || 
		s_cmp(cin + (icom - 1) * 72, "rw", (ftnlen)2, (ftnlen)2) == 0)
		 {
	    regwrt = TRUE_;
	    barwrt = FALSE_;
	} else if (s_cmp(cin + (icom - 1) * 72, "BW", (ftnlen)2, (ftnlen)2) ==
		 0 || s_cmp(cin + (icom - 1) * 72, "bw", (ftnlen)2, (ftnlen)2)
		 == 0) {
	    regwrt = FALSE_;
	    barwrt = TRUE_;
	} else {
	    regwrt = FALSE_;
	    barwrt = FALSE_;
	}
	++icom;
	iunit = 1;
L200:
	if (icom <= jcom) {
	    s_copy(fname, cin + (icom - 1) * 72, (ftnlen)2048, (ftnlen)72);
	    ++icom;
	} else {
	    inqstr_("FASTQ DATA FILE: ", fname, (ftnlen)17, (ftnlen)2048);
	}
	o__1.oerr = 1;
	o__1.ounit = iunit;
	o__1.ofnmlen = 2048;
	o__1.ofnm = fname;
	o__1.orl = 0;
	o__1.osta = "NEW";
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	i__1 = f_open(&o__1);
	if (i__1 != 0) {
	    goto L200;
	}
	wrfsq_(&iunit, &mp, &ml, &ms, &mr, &c__100, &c__60, &c__50, &icom, &
		jcom, cin, rin, iin, kin, n, &ia[k[0] - 1], &a[k[1] - 1], &ia[
		k[2] - 1], &ia[k[3] - 1], &ia[k[4] - 1], &ia[k[5] - 1], &a[k[
		6] - 1], &ia[k[7] - 1], &ia[k[8] - 1], &ia[k[9] - 1], &ia[k[
		10] - 1], &ia[k[11] - 1], &ia[k[12] - 1], &ia[k[13] - 1], &ia[
		k[14] - 1], &ia[k[15] - 1], &ia[k[16] - 1], &ia[k[17] - 1], &
		ia[k[18] - 1], &ia[k[19] - 1], &ia[k[20] - 1], &ia[k[21] - 1],
		 &ia[k[22] - 1], &ia[k[23] - 1], &ia[k[24] - 1], &ia[k[25] - 
		1], &ia[k[26] - 1], &ia[k[27] - 1], &ia[k[28] - 1], &ia[k[29] 
		- 1], &ia[k[30] - 1], &ia[k[31] - 1], &ia[k[32] - 1], &ia[k[
		33] - 1], &ia[k[34] - 1], &ia[k[35] - 1], &ia[k[36] - 1], &ia[
		k[37] - 1], &ia[k[39] - 1], &ia[k[40] - 1], &ia[k[41] - 1], &
		ia[k[42] - 1], &ia[k[43] - 1], &ia[k[45] - 1], &ia[k[46] - 1],
		 &ia[k[47] - 1], &ia[k[48] - 1], &ia[k[59] - 1], &ia[k[60] - 
		1], &ia[k[61] - 1], &a[k[62] - 1], &ia[k[63] - 1], &ia[k[64] 
		- 1], &ia[k[65] - 1], &ia[k[66] - 1], ischm, scheme, number, 
		defsch, &defsiz, title, &optim, &three, &eight, &nine, &snap, 
		snapdx, nsnap, &regwrt, &barwrt, (ftnlen)72, (ftnlen)72, (
		ftnlen)80, (ftnlen)72, (ftnlen)72);
	wrote = TRUE_;
	cl__1.cerr = 0;
	cl__1.cunit = iunit;
	cl__1.csta = 0;
	f_clos(&cl__1);
/*  GET THE APPROPRIATE HELP MESAGE */
    } else {
	++icom;
	help_fq__(&c__1);
    }
    goto L120;
} /* MAIN__ */

#undef ia
#undef a


/* Main program alias */ int fastq_ () { MAIN__ (); return 0; }
#ifdef __cplusplus
	}
#endif

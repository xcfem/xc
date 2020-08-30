/* keyin.f -- translated by f2c (version 20160102).
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
static integer c__9 = 9;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int keyin_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *msc, integer *ma, integer *mcom, integer *icom, integer *
	jcom, char *cin, real *rin, integer *iin, integer *kin, integer *
	idump, integer *n, integer *ipoint, real *coor, integer *ipboun, 
	integer *iline, integer *ltype, integer *nint, real *factor, integer *
	lcon, integer *ilboun, integer *isboun, integer *iside, integer *nlps,
	 integer *ifline, integer *illist, integer *ibarst, integer *jmat, 
	integer *jcent, integer *nlpb, integer *jfline, integer *jllist, 
	integer *iregn, integer *imat, integer *nspr, integer *ifside, 
	integer *islist, integer *irpb, integer *ipbf, integer *nppf, integer 
	*ifpb, integer *listpb, integer *ilbf, integer *nlpf, integer *iflb, 
	integer *listlb, integer *isbf, integer *nspf, integer *ifsb, integer 
	*listsb, integer *linkp, integer *linkl, integer *links, integer *
	linkb, integer *linkr, integer *linkm, integer *linksc, integer *
	linkpb, integer *linklb, integer *linksb, integer *iholdp, integer *
	iholdl, integer *iholds, integer *iholdb, integer *iholdr, integer *
	iholdm, integer *ihold1, integer *ihold2, integer *ihold3, integer *
	iwtpbf, integer *iwtlbf, integer *iwtsbf, real *rsize, integer *
	ifhole, integer *nhpr, integer *ihlist, integer *irgflg, integer *
	ischm, char *scheme, char *number, char *defsch, real *defsiz, char *
	title, logical *optim, logical *three, logical *eight, logical *nine, 
	logical *noroom, logical *vaxvms, logical *wrote, real *time1, char *
	versn, logical *batch, ftnlen cin_len, ftnlen scheme_len, ftnlen 
	number_len, ftnlen defsch_len, ftnlen title_len, ftnlen versn_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 A POINT NO. OF:\002,i7,\002 IS NOT ALLO\
WED\002,/,\002 THIS POINT WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10010[] = "(\002 A LINE NO. OF:\002,i7,\002 IS NOT ALLOW\
ED\002,/,\002 THIS LINE WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10020[] = "(\002 FOR LINE NO.:\002,i5,\002 NOT ENOUGH IN\
FORMATION IS \002//\002SUPPLIED\002,/,\002 THIS LINE WILL NOT BE INPUT INTO \
DATABASE\002)";
    static char fmt_10030[] = "(\002 REGION NO:\002,i5,\002 IS NOT IN THE DA\
TABASE\002,/,\002 THUS NO SIZE CAN BE ENTERED\002)";
    static char fmt_10040[] = "(\002 A SIDE NO. OF:\002,i7,\002 IS NOT ALLOW\
ED\002,/,\002 THIS SIDE WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10070[] = "(\002 FOR SIDE NO.:\002,i5,\002 NOT ENOUGH IN\
FORMATION IS \002//\002SUPPLIED\002,/,\002 THIS SIDE WILL NOT BE INPUT INTO \
DATABASE\002)";
    static char fmt_10080[] = "(\002 REGION NO:\002,i5,\002 IS NOT IN THE DA\
TABASE\002,/,\002 THUS NO HOLE CAN BE ENTERED\002)";
    static char fmt_10090[] = "(\002 REGION:\002,i5,\002 HAS LESS THAN ONE H\
OLE\002,/,\002 THE HOLE FOR THIS REGION WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10050[] = "(\002 A BAR SET NO. OF:\002,i7,\002 IS NOT AL\
LOWED\002,/,\002 THIS BAR SET WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10060[] = "(\002 FOR BAR SET NO.:\002,i5,/,\002 NOT ENOU\
GH INFORMATION IS SUPPLIED\002,/,\002 THIS BAR SET WILL NOT BE INPUT INTO DA\
TABASE\002)";
    static char fmt_10150[] = "(\002 A REGION NO. OF:\002,i7,\002 IS NOT ALL\
OWED\002,/,\002 THE SCHEME FOR THIS REGION WILL NOT BE INPUT INTO DATABAS\
E\002)";
    static char fmt_10160[] = "(\002 FOR THE SCHEME FOR REGION NO.:\002,i5\
,\002 NOT ENOUGH INFORMATION IS SUPPLIED\002,/,\002 THIS SCHEME WILL NOT BE \
INPUT INTO DATABASE\002)";
    static char fmt_10100[] = "(\002 WEIGHTING OF \002,a,\002 FLAG:\002,i5\
,\002 NOT POSSIBLE - \002//\002FLAG NOT FOUND\002)";
    static char fmt_10110[] = "(\002 A REGION NO. OF:\002,i7,\002 IS NOT ALL\
OWED\002,/,\002 THIS REGION WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10120[] = "(\002 FOR REGION NO.:\002,i5,\002 NOT ENOUGH \
INFORMATION IS \002//\002SUPPLIED\002,/,\002 THIS REGION WILL NOT BE INPUT I\
NTO DATABASE\002)";
    static char fmt_10130[] = "(\002 A GROUP NO. OF:\002,i7,\002 IS NOT ALLO\
WED\002,/,\002 THIS GROUP WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10140[] = "(\002 FOR GROUP NO.:\002,i5,\002 NOT ENOUGH I\
NFORMATION IS \002//\002SUPPLIED\002,/,\002 THIS GROUP WILL NOT BE INPUT INT\
O DATABASE\002)";
    static char fmt_10170[] = "(\002 REGION NO:\002,i5,\002 IS NOT IN THE DA\
TABASE\002,/,\002 THUS NO MATERIAL NUMBER CAN BE ENTERED\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(), 
	    s_cmp(char *, char *, ftnlen, ftnlen);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, j, i1, i2;
    static real r1;
    static integer jj, it, iz, iii[1], len, jin[80];
    static char hold[72];
    static integer nold[29];
    static logical ians;
    static integer irsz;
    extern /* Subroutine */ int geti12_(integer *, integer *, integer *, char 
	    *, integer *, integer *, integer *, integer *, integer *, ftnlen);
    static integer nold10;
    static logical merge;
    static integer nold24, jhold;
    extern /* Subroutine */ int fexit_(logical *, integer *, integer *, 
	    integer *, char *, integer *, real *, integer *, real *, logical *
	    , char *, ftnlen, ftnlen);
    static integer izero;
    extern /* Subroutine */ int spawn_(logical *);
    static integer iunit, ipntr, jpntr, nhold1, nhold2, nhold3;
    extern /* Subroutine */ int geti1r_(integer *, integer *, integer *, char 
	    *, integer *, real *, integer *, integer *, real *, integer *, 
	    ftnlen);
    static logical addold, addlnk;
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen);
    static integer nholdb;
    static logical dolink, sideok;
    static integer iostat;
    static logical newnum;
    static integer ifound;
    extern /* Subroutine */ int inpoin_(integer *, integer *, integer *, 
	    integer *, real *, real *, integer *, integer *, integer *, real *
	    , integer *, integer *, logical *, logical *);
    static integer nholdp;
    extern /* Subroutine */ int inline_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, logical *, 
	    logical *);
    static integer nholdl;
    extern /* Subroutine */ int inside_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *);
    static integer nholds, jjpntr;
    extern /* Subroutine */ int inhole_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, logical *), inbrst_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, logical *);
    static integer nholdm;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *), intrup_(char *, logical *, integer *, 
	    integer *, integer *, char *, integer *, real *, integer *, 
	    ftnlen, ftnlen), inbody_(integer *, integer *, integer *, integer 
	    *, integer *, logical *, logical *), inschm_(integer *, integer *,
	     integer *, integer *, integer *, char *, integer *, char *, 
	    integer *, char *, logical *, logical *, ftnlen, ftnlen, ftnlen), 
	    inboun_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, logical *, logical *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *), inrenm_(integer *, 
	    integer *, char *, real *, integer *, integer *, char *, logical *
	    , ftnlen, ftnlen), inregn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *);
    static integer nholdr;
    extern /* Subroutine */ int ingrpn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *), getinp_(integer *, integer *, 
	    char *, char *, integer *, ftnlen, ftnlen), inintr_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *), infact_(integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, integer *, logical *), strlng_(char *, 
	    integer *, ftnlen), linkbc_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, logical *), help_fq__(integer *);

    /* Fortran I/O blocks */
    static cilist io___10 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___15 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___20 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___21 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___22 = { 0, 6, 0, fmt_10070, 0 };
    static cilist io___25 = { 0, 6, 0, fmt_10080, 0 };
    static cilist io___26 = { 0, 6, 0, fmt_10090, 0 };
    static cilist io___27 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___28 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___35 = { 0, 6, 0, fmt_10150, 0 };
    static cilist io___36 = { 0, 6, 0, fmt_10160, 0 };
    static cilist io___44 = { 0, 6, 0, fmt_10100, 0 };
    static cilist io___45 = { 0, 6, 0, fmt_10100, 0 };
    static cilist io___46 = { 0, 6, 0, fmt_10100, 0 };
    static cilist io___48 = { 0, 6, 0, fmt_10110, 0 };
    static cilist io___49 = { 0, 6, 0, fmt_10120, 0 };
    static cilist io___51 = { 0, 6, 0, fmt_10130, 0 };
    static cilist io___52 = { 0, 6, 0, fmt_10140, 0 };
    static cilist io___58 = { 0, 6, 0, fmt_10170, 0 };


/* *********************************************************************** */
/*  SUBROUTINE KEYIN = INPUTS MESH DEFINITIONS FROM THE KEYBOARD */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     FASTQ = A PROGRAM TO QUICKLY GENERATE QUADRILATERAL MESHES */
/* *********************************************************************** */
    /* Parameter adjustments */
    iwtpbf -= 4;
    ihold1 -= 3;
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
    iholdl -= 3;
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
    iholdb -= 3;
    iholds -= 3;
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
    --irgflg;
    --ihlist;
    --nhpr;
    --ifhole;
    --rsize;
    iholdm -= 3;
    iholdr -= 3;
    linksc -= 3;
    linkm -= 3;
    linkr -= 3;
    --irpb;
    --islist;
    --ifside;
    --nspr;
    --imat;
    --iregn;
    number -= 80;
    scheme -= 72;
    --ischm;
    --kin;
    --iin;
    --rin;
    cin -= 72;
    --n;

    /* Function Body */
    iz = 0;
    merge = FALSE_;
    dolink = TRUE_;
    *noroom = FALSE_;
    addlnk = FALSE_;
    for (i__ = 1; i__ <= 29; ++i__) {
	nold[i__ - 1] = n[i__];
/* L100: */
    }
L110:
    if (*icom > *jcom) {
	mesage_(" ", (ftnlen)1);
	frefld_(&iz, &iz, "ENTER KEYIN OPTION: ", mcom, &iostat, jcom, &kin[1]
		, cin + 72, &iin[1], &rin[1], (ftnlen)20, (ftnlen)72);
	*icom = 1;
    }
/*  INPUT A POINT INTO THE DATABASE */
    if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned char *)&cin[*
	    icom * 72] == 'p') {
	++(*icom);
	mesage_("ENTER POINT DATA IN THE FOLLOWING FORMAT:", (ftnlen)41);
	mesage_("[ POINT NO., X, Y ]", (ftnlen)19);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	*icom = *jcom + 1;
L120:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0) {
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___10);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    addlnk = FALSE_;
	    ltsort_(mp, &linkp[3], &jj, &ipntr, &addlnk);
	    inpoin_(mp, &n[1], &n[18], &jj, &rin[2], &rin[3], &nholdp, &
		    iholdp[3], &ipoint[1], &coor[3], &ipboun[1], &linkp[3], &
		    merge, noroom);
	    if (*noroom) {
		goto L400;
	    }
/*  REPLACE THE FLAGS OF A REDEFINED POINT */
	    if (ipntr > 0) {
		ltsort_(mp, &linkp[3], &jj, &jpntr, &addlnk);
		ipboun[jpntr] = ipboun[ipntr];
	    }
	    goto L120;
	}
/*  ENTER A LINE INTO THE DATABASE */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'L' || *(unsigned char *)
	    &cin[*icom * 72] == 'l') {
	++(*icom);
	if (*icom > *jcom) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("THE FOLLOWING LINE TYPES ARE AVAILABLE:", (ftnlen)39);
	    mesage_("     S*TRAIGHT = STRAIGHT LINE", (ftnlen)30);
	    mesage_("     CI*RCULAR = CIRCULAR CCW ARC ABOUT A CENTER", (
		    ftnlen)48);
	    mesage_("     3*CIRCULAR = CIRCULAR ARC WITH 3RD ARC POINT", (
		    ftnlen)49);
	    mesage_("     R*CIRCULAR = CIRCULAR ARC WITH RADIUS", (ftnlen)42);
	    mesage_("     E*LIPSE    = CCW ELIPSE ABOUT A CENTER", (ftnlen)43)
		    ;
	    mesage_("     CO*RNER    = 2 LINE SEGMENTS JOINED", (ftnlen)40);
	    mesage_("     P*ARABOLA  = PARABOLIC SHAPED LINE", (ftnlen)39);
	    frefld_(&iz, &iz, "WHICH LINE TYPE WOULD YOU LIKE TO ENTER:", 
		    mcom, &iostat, jcom, &kin[1], cin + 72, &iin[1], &rin[1], 
		    (ftnlen)40, (ftnlen)72);
	    *icom = 1;
	}
	if (*(unsigned char *)&cin[*icom * 72] == 'S' || *(unsigned char *)&
		cin[*icom * 72] == 's') {
	    ++(*icom);
	    mesage_("ENTER STRAIGHT LINE DATA IN THE FOLLOWING FORMAT:", (
		    ftnlen)49);
	    mesage_("[ LINE NO., POINT 1, POINT 2, NO. INTERVALS, FACTOR ]", (
		    ftnlen)53);
	    it = 1;
	} else if (s_cmp(cin + *icom * 72, "CI", (ftnlen)2, (ftnlen)2) == 0 ||
		 s_cmp(cin + *icom * 72, "ci", (ftnlen)2, (ftnlen)2) == 0) {
	    ++(*icom);
	    mesage_("NOTE: IF A CW ARC IS DESIRED, ENTER", (ftnlen)35);
	    mesage_("      THE CENTER POINT AS NEGATIVE", (ftnlen)34);
	    mesage_("ENTER CIRCULAR ARC LINE DATA IN THE FOLLOWING FORMAT:", (
		    ftnlen)53);
	    mesage_("[ LINE NO., POINT 1, POINT 2, CENTER, NO. INTERVALS, FA\
CTOR ]", (ftnlen)61);
	    it = 3;
	} else if (*(unsigned char *)&cin[*icom * 72] == '3') {
	    ++(*icom);
	    mesage_("ENTER THIRD POINT ARC DATA IN THE FOLLOWING FORMAT:", (
		    ftnlen)51);
	    mesage_("[ LINE NO., POINT 1, POINT 2, POINT 3, NO. INTERVALS, F\
ACTOR ]", (ftnlen)62);
	    it = 4;
	} else if (*(unsigned char *)&cin[*icom * 72] == 'R' || *(unsigned 
		char *)&cin[*icom * 72] == 'r') {
	    ++(*icom);
	    mesage_("NOTE: THE RADIUS IS ASSUMED TO BE CONTAINED", (ftnlen)43)
		    ;
	    mesage_("      IN THE X COORDINATE OF POINT 3. THE", (ftnlen)41);
	    mesage_("      ARC CENTER IS ASSUMED TO THE LEFT OF A", (ftnlen)
		    44);
	    mesage_("      LINE FROM POINT 1 TO POINT 2 (OPPOSITE", (ftnlen)
		    44);
	    mesage_("      IF POINT 3 IS ENTERED NEGATIVE).", (ftnlen)38);
	    mesage_("ENTER CIRCULAR ARC W/RADIUS DATA IN THE FOLLOWING FORMA\
T:", (ftnlen)57);
	    mesage_("[ LINE NO., POINT 1, POINT 2, CENTER, NO. INTERVALS, FA\
CTOR ]", (ftnlen)61);
	    it = 7;
	} else if (*(unsigned char *)&cin[*icom * 72] == 'E' || *(unsigned 
		char *)&cin[*icom * 72] == 'e') {
	    ++(*icom);
	    mesage_("NOTE: THE TWO POINTS ON THE ELIPSE CANNOT BE COLINEAR W\
ITH THE", (ftnlen)62);
	    mesage_("      CENTER POINT IN THIS DEFINITION.", (ftnlen)38);
	    mesage_("NOTE: IF A CW ARC IS DESIRED, ENTER", (ftnlen)35);
	    mesage_("      THE CENTER POINT AS NEGATIVE.", (ftnlen)35);
	    mesage_("ENTER ELIPSE ABOUT A CENTER DATA IN THE FOLLOWING FORMA\
T:", (ftnlen)57);
	    mesage_("[ LINE NO., POINT 1, POINT 2, POINT 3, NO. INTERVALS, F\
ACTOR ]", (ftnlen)62);
	    it = 7;
	} else if (s_cmp(cin + *icom * 72, "CO", (ftnlen)2, (ftnlen)2) == 0 ||
		 s_cmp(cin + *icom * 72, "co", (ftnlen)2, (ftnlen)2) == 0) {
	    ++(*icom);
	    mesage_("NOTE: A CORNER LINE CONTAINS TO STRAIGHT LINE", (ftnlen)
		    45);
	    mesage_("      SEGMENTS JOINED AT POINT 3", (ftnlen)32);
	    mesage_("ENTER CORNER LINE DATA IN THE FOLLOWING FORMAT:", (
		    ftnlen)47);
	    mesage_("[ LINE NO., POINT 1, POINT 2, POINT 3, NO. INTERVALS, F\
ACTOR ]", (ftnlen)62);
	    it = 2;
	} else if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned 
		char *)&cin[*icom * 72] == 'p') {
	    ++(*icom);
	    mesage_("NOTE: POINT 3 IS THE TIP OF THE PARABOLA, AND", (ftnlen)
		    45);
	    mesage_("      POINT 1 AND POINT 2 MUST BE EQUAL ARC", (ftnlen)43)
		    ;
	    mesage_("      LENGTHS AWAY (ISOCELES TRIANGLE)", (ftnlen)38);
	    mesage_("ENTER PARABOLIC LINE DATA IN THE FOLLOWING FORMAT:", (
		    ftnlen)50);
	    mesage_("[ LINE NO., POINT 1, POINT 2, POINT 3, NO. INTERVALS, F\
ACTOR ]", (ftnlen)62);
	    it = 5;
	} else {
	    ++(*icom);
	    goto L110;
	}
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	*icom = *jcom + 1;
L130:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0) {
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___15);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L130;
	    } else if (it == 1 && ifound < 3 || it != 1 && ifound < 4) {
		s_wsfe(&io___16);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L130;
	    }
	    izero = 0;
	    addlnk = FALSE_;
	    ltsort_(ml, &linkl[3], &jj, &ipntr, &addlnk);
	    if (it == 1) {
		inline_(ml, &n[2], &n[19], &jj, &it, &iin[2], &iin[3], &izero,
			 &iin[4], &rin[5], &nholdl, &iholdl[3], &iline[1], &
			ltype[1], &nint[1], &factor[1], &lcon[4], &ilboun[1], 
			&isboun[1], &linkl[3], &merge, noroom);
	    } else {
		inline_(ml, &n[2], &n[19], &jj, &it, &iin[2], &iin[3], &iin[4]
			, &iin[5], &rin[6], &nholdl, &iholdl[3], &iline[1], &
			ltype[1], &nint[1], &factor[1], &lcon[4], &ilboun[1], 
			&isboun[1], &linkl[3], &merge, noroom);
	    }
	    if (*noroom) {
		goto L400;
	    }
/*  LINK UP THE OLD FLAGS TO THE NEW LINE */
	    if (ipntr > 0) {
		addlnk = FALSE_;
		ltsort_(ml, &linkl[3], &jj, &jpntr, &addlnk);
		ilboun[jpntr] = ilboun[ipntr];
		isboun[jpntr] = isboun[ipntr];
	    }
	    goto L130;
	}
/*  ENTER A REGION INTERVAL SIZE INTO THE DATABASE */
    } else if (s_cmp(cin + *icom * 72, "SIZ", (ftnlen)3, (ftnlen)3) == 0 || 
	    s_cmp(cin + *icom * 72, "siz", (ftnlen)3, (ftnlen)3) == 0) {
	++(*icom);
	mesage_("NOTE: ENTER A DEFAULT SIZE BY SPECIFYING", (ftnlen)40);
	mesage_("      A SIZE WITH NO REGIONS.", (ftnlen)29);
	mesage_("ENTER REGION SIZE DATA IN THE FOLLOWING FORMAT:", (ftnlen)47)
		;
	mesage_("[ SIZE, REGION 1, REGION 2, ..., REGION N ]", (ftnlen)43);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	*icom = *jcom + 1;
L140:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0) {
	    if (ifound < 2) {
		*defsiz = rin[1];
	    } else {
		addlnk = FALSE_;
		i__1 = ifound;
		for (irsz = 2; irsz <= i__1; ++irsz) {
		    ltsort_(mr, &linkr[3], &iin[irsz], &jj, &addlnk);
		    if (jj >= 0) {
			rsize[jj] = dmax(rin[1],(float)0.);
		    } else {
			s_wsfe(&io___20);
			do_fio(&c__1, (char *)&iin[irsz], (ftnlen)sizeof(
				integer));
			e_wsfe();
		    }
/* L150: */
		}
	    }
	    goto L140;
	}
/*  ENTER A SIDE INTO THE DATABASE */
    } else if (s_cmp(cin + *icom * 72, "SI", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "si", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	mesage_("ENTER SIDE DATA IN THE FOLLOWING FORMAT:", (ftnlen)40);
	mesage_("[ SIDE NO., LINE 1, LINE 2, ..., LINE N ]", (ftnlen)41);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	*icom = *jcom + 1;
L160:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0) {
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___21);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L160;
	    } else if (ifound < 2) {
		s_wsfe(&io___22);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L160;
	    }
	    i__1 = ifound - 1;
	    inside_(ms, &n[3], &n[4], &n[20], &jj, &iin[2], &i__1, &iside[1], 
		    &nlps[1], &ifline[1], &illist[1], &links[3], &nholds, &
		    iholds[3], &merge, noroom);
	    if (*noroom) {
		goto L400;
	    }
	    goto L160;
	}
/*  ENTER A HOLE INTO THE DATABASE */
    } else if (s_cmp(cin + *icom * 72, "HO", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ho", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	mesage_("ENTER HOLE DATA IN THE FOLLOWING FORMAT:", (ftnlen)40);
	mesage_("[ REGION NO., HOLE 1, HOLE 2, ..., HOLE N ]", (ftnlen)43);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	*icom = *jcom + 1;
L170:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0) {
	    jj = iin[1];
	    addlnk = FALSE_;
	    ltsort_(mr, &linkr[3], &jj, &jjpntr, &addlnk);
	    addlnk = TRUE_;
	    if (jjpntr <= 0) {
		s_wsfe(&io___25);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L170;
	    } else if (ifound < 1) {
		s_wsfe(&io___26);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L170;
	    }
	    i__1 = ifound - 1;
	    inhole_(mr, &n[7], &n[29], &jjpntr, &iin[2], &i__1, &ifhole[1], &
		    nhpr[1], &ihlist[1], &merge, noroom);
	    if (*noroom) {
		goto L400;
	    }
	    goto L170;
	}
/*  ENTER A BARSET INTO THE DATABASE */
    } else if (s_cmp(cin + *icom * 72, "BA", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ba", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	mesage_("ENTER BAR SET DATA IN THE FOLLOWING FORMAT:", (ftnlen)43);
	mesage_("[ BAR SET NO., MAT NO., REFR. PNT., LINE 1, LINE 2, ..., LI\
NE N ]", (ftnlen)65);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	*icom = *jcom + 1;
L180:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0) {
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___27);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L180;
	    } else if (ifound < 4) {
		s_wsfe(&io___28);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L180;
	    }
	    i__1 = ifound - 3;
	    inbrst_(ms, mr, &n[5], &n[6], &n[21], &n[23], &jj, &iin[2], &iin[
		    3], &iin[4], &i__1, &ibarst[1], &jmat[1], &jcent[1], &
		    nlpb[1], &jfline[1], &jllist[1], &linkb[3], &linkm[3], &
		    nholdm, &iholdm[3], &nholdb, &iholdb[3], &merge, noroom);
	    if (*noroom) {
		goto L400;
	    }
	    goto L180;
	}
/*  INPUT A BODY DEFINITION INTO THE DATABASE */
    } else if (s_cmp(cin + *icom * 72, "BOD", (ftnlen)3, (ftnlen)3) == 0 || 
	    s_cmp(cin + *icom * 72, "bod", (ftnlen)3, (ftnlen)3) == 0) {
	++(*icom);
	mesage_("NOTE: THE BODY CAN BE MADE UP OF ANY", (ftnlen)36);
	mesage_("      COMBINATION OF REGIONS AND BAR SETS.", (ftnlen)42);
	mesage_("      ENTER BAR SETS AS NEGATIVE REGIONS.", (ftnlen)41);
	mesage_("ENTER REGIONS (& BAR SETS) IN THE BODY IN THE FOLLOWING FOR\
MAT", (ftnlen)62);
	mesage_("[ REGION 1, REGION 2, ..., REGION N ]", (ftnlen)37);
	mesage_("HIT A RETURN TO END INPUT", (ftnlen)25);
	*icom = *jcom + 1;
L190:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0 && ifound <= 80) {
	    if (n[9] > 0) {
		i__1 = ifound;
		for (j = 1; j <= i__1; ++j) {
		    jin[j - 1] = iin[j];
/* L200: */
		}
		intrup_("REPLACE THE CURRENT BODY DEFINITION", &ians, mcom, 
			icom, jcom, cin + 72, &iin[1], &rin[1], &kin[1], (
			ftnlen)35, (ftnlen)72);
		i__1 = ifound;
		for (j = 1; j <= i__1; ++j) {
		    iin[j] = jin[j - 1];
/* L210: */
		}
		if (ians) {
		    addold = FALSE_;
		} else {
		    addold = TRUE_;
		}
	    }
	    inbody_(mr, &n[9], &iin[1], &ifound, &irpb[1], &addold, noroom);
	    if (*noroom) {
		goto L400;
	    }
	    goto L190;
	} else if (ifound > 80) {
	    mesage_("TOO MANY BODIES BEING INPUT A ONCE - TRY AGAIN", (ftnlen)
		    46);
	    goto L190;
	}
/*  SPAWN A PROCESS */
    } else if (s_cmp(cin + *icom * 72, "SP", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "sp", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	spawn_(vaxvms);
/*  INPUT A SCHEME INTO THE DATABASE */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'S' || *(unsigned char *)
	    &cin[*icom * 72] == 's') {
	++(*icom);
	mesage_("NOTE: ENTER A DEFAULT SCHEME BY SPECIFYING", (ftnlen)42);
	mesage_("      THE REGION NUMBER AS ZERO", (ftnlen)31);
	mesage_("ENTER A SCHEME IN THE FOLLOWING FORMAT:", (ftnlen)39);
	mesage_("[ REGION NO., SCHEME ]", (ftnlen)22);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	*icom = *jcom + 1;
L220:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0) {
	    jj = iin[1];
	    if (jj >= 10000) {
		s_wsfe(&io___35);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L220;
	    } else if (ifound < 2) {
		s_wsfe(&io___36);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L220;
	    }
	    nold10 = n[10];
	    nold24 = n[24];
	    inschm_(mr, msc, &n[10], &n[24], &jj, cin + 144, &ischm[1], 
		    scheme + 72, &linksc[3], defsch, noroom, &dolink, (ftnlen)
		    72, (ftnlen)72, (ftnlen)72);
	    if (*noroom) {
		n[10] = nold10;
		n[24] = nold24;
		mesage_("************************************", (ftnlen)36);
		mesage_("NOT ENOUGH ROOM FOR SCHEME CARD", (ftnlen)31);
		mesage_("NO DYNAMIC DIMENSIONING INCREASES", (ftnlen)33);
		mesage_("AVAILABLE FOR CHARACTER STRINGS", (ftnlen)31);
		mesage_("SCHEME INPUT IS THUS IGNORED", (ftnlen)28);
		mesage_("************************************", (ftnlen)36);
	    }
	    goto L220;
	}
/*  INPUT A BOUNDARY INTO THE DATABASE */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'B' || *(unsigned char *)
	    &cin[*icom * 72] == 'b') {
	++(*icom);
	if (*icom > *jcom) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("THE FOLLOWING BOUNDARY FLAGS ARE AVAILABLE:", (ftnlen)43)
		    ;
	    mesage_("        P*OINT FLAGS    - FOR NODES AT POINTS", (ftnlen)
		    45);
	    mesage_("        N*ODE FLAGS     - FOR NODES ON A BOUNDARY", (
		    ftnlen)49);
	    mesage_("        E*LEMENT FLAGS  - FOR ELEMENT SIDES ON A BOUNDA\
RY", (ftnlen)57);
	    frefld_(&iz, &iz, "WHICH BOUNDARY FLAG WOULD YOU LIKE TO ENTER: ",
		     mcom, &iostat, jcom, &kin[1], cin + 72, &iin[1], &rin[1],
		     (ftnlen)45, (ftnlen)72);
	    *icom = 1;
	}
/*  INPUT A POINT BOUNDARY INTO THE DATABASE */
	if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned char *)&
		cin[*icom * 72] == 'p') {
	    ++(*icom);
	    mesage_("INPUT POINT BOUNDARY FLAG DATA IN THE FOLLOWING FORMAT:",
		     (ftnlen)55);
	    mesage_("[ FLAG NO., POINT 1, POINT 2, ..., POINT N ]", (ftnlen)
		    44);
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	    *icom = *jcom + 1;
L230:
	    frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, 
		    &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    if (ifound > 0) {
		addold = FALSE_;
		i__1 = ifound - 1;
		inboun_(mp, &iin[1], &i__1, &iin[2], &n[25], &n[11], &n[12], &
			nold[10], &merge, noroom, &newnum, &nhold1, &ihold1[3]
			, &ipbf[1], &nppf[1], &ifpb[1], &listpb[3], &linkpb[3]
			, &iwtpbf[4], &jhold, &addold);
		if (*noroom) {
		    goto L400;
		}
		goto L230;
	    }
/*  INPUT A NODE BOUNDARY INTO THE DATABASE */
	} else if (*(unsigned char *)&cin[*icom * 72] == 'N' || *(unsigned 
		char *)&cin[*icom * 72] == 'n') {
	    ++(*icom);
	    mesage_("INPUT NODE BOUNDARY FLAG DATA IN THE FOLLOWING FORMAT:", 
		    (ftnlen)54);
	    mesage_("[ FLAG NO., LINE (OR NEG. SIDE) 1, LINE (OR NEG. SIDE) \
2, ...]", (ftnlen)62);
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	    *icom = *jcom + 1;
L240:
	    frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, 
		    &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    if (ifound > 0) {
		addold = FALSE_;
		i__1 = ifound - 1;
		inboun_(ml, &iin[1], &i__1, &iin[2], &n[26], &n[13], &n[14], &
			nold[12], &merge, noroom, &newnum, &nhold2, &ihold2[3]
			, &ilbf[1], &nlpf[1], &iflb[1], &listlb[3], &linklb[3]
			, &iwtlbf[4], &jhold, &addold);
		if (*noroom) {
		    goto L400;
		}
		goto L240;
	    }
/*  INPUT AN ELEMENT BOUNDARY INTO THE DATABASE */
	} else if (*(unsigned char *)&cin[*icom * 72] == 'E' || *(unsigned 
		char *)&cin[*icom * 72] == 'e') {
	    ++(*icom);
	    mesage_("INPUT ELEMENT BOUNDARY FLAG DATA IN THE FOLLOWING FORMA\
T:", (ftnlen)57);
	    mesage_("[ FLAG NO., LINE (OR NEG. SIDE) 1, LINE (OR NEG. SIDE) \
2, ...]", (ftnlen)62);
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	    *icom = *jcom + 1;
L250:
	    frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, 
		    &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    if (ifound > 0) {
		addold = FALSE_;
		i__1 = ifound - 1;
		inboun_(ml, &iin[1], &i__1, &iin[2], &n[27], &n[15], &n[16], &
			nold[14], &merge, noroom, &newnum, &nhold3, &ihold3[3]
			, &isbf[1], &nspf[1], &ifsb[1], &listsb[3], &linksb[3]
			, &iwtsbf[4], &jhold, &addold);
		if (*noroom) {
		    goto L400;
		}
		goto L250;
	    }
	}
/*  INPUT A BOUNDARY FLAG WEIGHTING INTO THE DATABASE */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'W' || *(unsigned char *)
	    &cin[*icom * 72] == 'w') {
	++(*icom);
	if (*icom > *jcom) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("THE FOLLOWING BOUNDARY FLAGS CAN BE WEIGHTED:", (ftnlen)
		    45);
	    mesage_("        P*OINT FLAGS    - FOR NODES AT POINTS", (ftnlen)
		    45);
	    mesage_("        N*ODE FLAGS     - FOR NODES ON A BOUNDARY", (
		    ftnlen)49);
	    mesage_("        E*LEMENT FLAGS  - FOR ELEMENT SIDES ON A BOUNDA\
RY", (ftnlen)57);
	    frefld_(&iz, &iz, "WHICH BOUNDARY FLAG WOULD YOU LIKE TO WEIGHT: "
		    , mcom, &iostat, jcom, &kin[1], cin + 72, &iin[1], &rin[1]
		    , (ftnlen)46, (ftnlen)72);
	    *icom = 1;
	}
/*  INPUT A POINT BOUNDARY FLAG WEIGHT INTO THE DATABASE */
	if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned char *)&
		cin[*icom * 72] == 'p') {
	    ++(*icom);
	    mesage_("INPUT POINT BOUNDARY FLAG WEIGHTS IN THE FOLLOWING FORM\
AT:", (ftnlen)58);
	    mesage_("[ FLAG NO., WEIGHTING POINT, BOUNDARY POINT ]", (ftnlen)
		    45);
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	    *icom = *jcom + 1;
L260:
	    frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, 
		    &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    if (ifound > 0) {
		ltsort_(mp, &linkpb[3], &iin[1], &jj, &addlnk);
		if (jj > 0) {
		    iwtpbf[jj * 3 + 1] = iin[2];
		    iwtpbf[jj * 3 + 2] = iin[3];
		    iwtpbf[jj * 3 + 3] = 0;
		} else {
		    s_wsfe(&io___44);
		    do_fio(&c__1, "POINT", (ftnlen)5);
		    do_fio(&c__1, (char *)&iin[1], (ftnlen)sizeof(integer));
		    e_wsfe();
		}
		goto L260;
	    }
/*  INPUT A NODE BOUNDARY WEIGHT INTO THE DATABASE */
	} else if (*(unsigned char *)&cin[*icom * 72] == 'N' || *(unsigned 
		char *)&cin[*icom * 72] == 'n') {
	    ++(*icom);
	    mesage_("INPUT NODE BOUNDARY FLAG WEIGHTS IN THE FOLLOWING FORMA\
T:", (ftnlen)57);
	    mesage_("[ FLAG NO., WEIGHTING SIDE (OR NEG. LINE) NO., BEGINNIN\
G POINT NO., ", (ftnlen)68);
	    mesage_("  BEGINNING LINE NO. (OPTIONAL) ]", (ftnlen)33);
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	    *icom = *jcom + 1;
L270:
	    frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, 
		    &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    if (ifound > 0) {
		ltsort_(ml, &linklb[3], &iin[1], &jj, &addlnk);
		if (jj > 0) {
		    iwtlbf[jj * 3 + 1] = iin[2];
		    iwtlbf[jj * 3 + 2] = iin[3];
		    iwtlbf[jj * 3 + 3] = iin[4];
		} else {
		    s_wsfe(&io___45);
		    do_fio(&c__1, "NODE", (ftnlen)4);
		    do_fio(&c__1, (char *)&iin[1], (ftnlen)sizeof(integer));
		    e_wsfe();
		}
		goto L270;
	    }
/*  INPUT AN ELEMENT BOUNDARY WEIGHT INTO THE DATABASE */
	} else if (*(unsigned char *)&cin[*icom * 72] == 'E' || *(unsigned 
		char *)&cin[*icom * 72] == 'e') {
	    ++(*icom);
	    mesage_("INPUT ELEMENT BOUNDARY FLAG WEIGHTS IN THE FOLLOWING FO\
RMAT:", (ftnlen)60);
	    mesage_("[ FLAG NO., WEIGHTING SIDE (OR NEG. LINE) NO.,         \
BEGINNING POINT NO., ", (ftnlen)76);
	    mesage_("  BEGINNING LINE NO. (OPTIONAL) ]", (ftnlen)33);
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	    *icom = *jcom + 1;
L280:
	    frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, 
		    &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    if (ifound > 0) {
		ltsort_(ml, &linksb[3], &iin[1], &jj, &addlnk);
		if (jj > 0) {
		    iwtsbf[jj * 3 + 1] = iin[2];
		    iwtsbf[jj * 3 + 2] = iin[3];
		    iwtsbf[jj * 3 + 3] = iin[4];
		} else {
		    s_wsfe(&io___46);
		    do_fio(&c__1, "ELEMENT", (ftnlen)7);
		    do_fio(&c__1, (char *)&iin[1], (ftnlen)sizeof(integer));
		    e_wsfe();
		}
		goto L280;
	    }
	}
/*  TOGGLE THE BANDWIDTH OPTIMIZATION FLAG */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'O' || *(unsigned char *)
	    &cin[*icom * 72] == 'o') {
	++(*icom);
	if (*optim) {
	    *optim = FALSE_;
	    mesage_("BANDWIDTH OPTIMIZER DISABLED", (ftnlen)28);
	} else {
	    *optim = TRUE_;
	    mesage_("BANDWIDTH OPTIMIZER ENABLED", (ftnlen)27);
	}
/*  FLAG THE BANDWIDTH OPTIMIZATION ROUTINES ON, AND READ A RENUM CARD */
    } else if (s_cmp(cin + *icom * 72, "REN", (ftnlen)3, (ftnlen)3) == 0 || 
	    s_cmp(cin + *icom * 72, "ren", (ftnlen)3, (ftnlen)3) == 0) {
	++(*icom);
	*optim = TRUE_;
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING RENUMBERING OPTIONS ARE AVAILABLE:", (ftnlen)
		48);
	mesage_("        P*-L-P = POINT LINE POINT STARTING LIST", (ftnlen)47)
		;
	mesage_("        X*, Y   = X, Y STARTING LOCATION", (ftnlen)40);
	mesage_("        N*UID  = NODE UNIQUE ID STARTING LIST", (ftnlen)45);
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, "WHICH RENUMBER OPTION WOULD YOU LIKE TO ENTER\
: ", mcom, &iostat, jcom, &kin[1], cin + 72, &iin[1], &rin[1], (ftnlen)47, (
		    ftnlen)72);
	    *icom = 1;
	}
/*  ENTER A POINT-LINE-POINT RENUM CARD */
	if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned char *)&
		cin[*icom * 72] == 'p') {
	    ++(*icom);
	    s_copy(hold, "P-L-P", (ftnlen)72, (ftnlen)5);
	    mesage_("ENTER A POINT-LINE-POINT CARD IN THE FOLLOWING FORMAT", (
		    ftnlen)53);
	    mesage_("[ ENTER POINT, LINE, POINT, LINE, ... ]", (ftnlen)39);
	    mesage_("HIT A RETURN TO END INPUT", (ftnlen)25);
	    *icom = *jcom + 1;
L290:
	    frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, 
		    &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    if (ifound >= 1) {
		inrenm_(msc, &n[28], hold, &rin[1], &iin[1], &ifound, number 
			+ 80, noroom, (ftnlen)72, (ftnlen)80);
		if (*noroom) {
		    goto L400;
		}
		goto L290;
	    }
/*  ENTER A X, Y LOCATION RENUM CARD */
	} else if (*(unsigned char *)&cin[*icom * 72] == 'X' || *(unsigned 
		char *)&cin[*icom * 72] == 'x') {
	    ++(*icom);
	    s_copy(hold, "X-Y  ", (ftnlen)72, (ftnlen)5);
	    mesage_("ENTER X, Y PAIRS IN THE FOLLOWING FORMAT", (ftnlen)40);
	    mesage_("[ X , Y ]", (ftnlen)9);
	    mesage_("HIT A RETURN TO END INPUT", (ftnlen)25);
	    *icom = *jcom + 1;
L300:
	    frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, 
		    &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    if (ifound >= 1) {
		inrenm_(msc, &n[28], hold, &rin[1], &iin[1], &ifound, number 
			+ 80, noroom, (ftnlen)72, (ftnlen)80);
		if (*noroom) {
		    goto L400;
		}
		goto L300;
	    }
/*  ENTER A NODE UNIQUE ID RENUM CARD */
	} else if (*(unsigned char *)&cin[*icom * 72] == 'N' || *(unsigned 
		char *)&cin[*icom * 72] == 'n') {
	    ++(*icom);
	    s_copy(hold, "NODE ", (ftnlen)72, (ftnlen)5);
	    mesage_("NOTE: NODE UNIQUE ID NUMBERS (NUID) ARE", (ftnlen)39);
	    mesage_("      DEFINED IN THE DOCUMENTATION", (ftnlen)34);
	    mesage_("ENTER NUID CARDS IN THE FOLLOWING FORMAT:", (ftnlen)41);
	    mesage_("[ NUID 1, NUID 2, ..., NUID N ]", (ftnlen)31);
	    mesage_("HIT A RETURN TO END INPUT", (ftnlen)25);
	    *icom = *jcom + 1;
L310:
	    frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, 
		    &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    if (ifound >= 1) {
		inrenm_(msc, &n[28], hold, &rin[1], &iin[1], &ifound, number 
			+ 80, noroom, (ftnlen)72, (ftnlen)80);
		if (*noroom) {
		    goto L400;
		}
		goto L310;
	    }
	}
/*  ENTER A REGION INTO THE DATABASE */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'R' || *(unsigned char *)
	    &cin[*icom * 72] == 'r') {
	++(*icom);
	intrup_("ARE YOU USING SIDES IN DEFINING REGIONS", &ians, mcom, icom, 
		jcom, cin + 72, &iin[1], &rin[1], &kin[1], (ftnlen)39, (
		ftnlen)72);
	if (ians) {
	    mesage_("NOTE: ENTER ANY LINES NEEDED IN THE REGION AS", (ftnlen)
		    45);
	    mesage_("      A NEGATIVE SIDE.", (ftnlen)22);
	    mesage_("ENTER REGION DATA IN THE FOLLOWING FORMAT:", (ftnlen)42);
	    mesage_("[ REGION NO., MATERIAL NO., SIDE 1, SIDE 2, ..., SIDE N\
 ]", (ftnlen)57);
	} else {
	    mesage_("ENTER REGION DATA IN THE FOLLOWING FORMAT:", (ftnlen)42);
	    mesage_("[ REGION NO., MATERIAL NO., LINE 1, LINE 2, ..., LINE N\
 ]", (ftnlen)57);
	}
	*icom = *jcom + 1;
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L320:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0) {
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___48);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L320;
	    } else if (ifound < 3) {
		s_wsfe(&io___49);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L320;
	    }
	    if (! ians) {
		i__1 = ifound;
		for (j = 3; j <= i__1; ++j) {
		    iin[j] = -iin[j];
/* L330: */
		}
	    }
	    i__1 = ifound - 2;
	    inregn_(ms, mr, &n[7], &n[8], &n[22], &n[23], &jj, &iin[2], &iin[
		    3], &i__1, &iregn[1], &imat[1], &nspr[1], &ifside[1], &
		    islist[1], &linkr[3], &linkm[3], &nholdr, &iholdr[3], &
		    nholdm, &iholdm[3], &irgflg[1], &merge, noroom);
	    if (*noroom) {
		goto L400;
	    }
	    addlnk = FALSE_;
	    ltsort_(mr, &linkr[3], &iin[1], &jjpntr, &addlnk);
	    rsize[jjpntr] = (float)0.;
	    goto L320;
	}
/*  ENTER A GROUP OF REGIONS INTO THE DATABASE */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'G' || *(unsigned char *)
	    &cin[*icom * 72] == 'g') {
	++(*icom);
	mesage_("ENTER GROUP OF REGIONS IN THE FOLLOWING FORMAT:", (ftnlen)47)
		;
	mesage_("[ GROUP NO., REGION 1, REGION 2, ..., REGION N ]", (ftnlen)
		48);
	*icom = *jcom + 1;
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L340:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0) {
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___51);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L340;
	    } else if (ifound < 2) {
		s_wsfe(&io___52);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L340;
	    }
	    i__1 = ifound - 1;
	    ingrpn_(ms, mr, &n[7], &n[8], &n[22], &jj, &iin[2], &i__1, &iregn[
		    1], &nspr[1], &ifside[1], &islist[1], &linkr[3], &nholdr, 
		    &iholdr[3], &irgflg[1], &merge, noroom);
	    if (*noroom) {
		goto L400;
	    }
	    goto L340;
	}
/*  ENTER A TITLE */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'T' || *(unsigned char *)
	    &cin[*icom * 72] == 't') {
	++(*icom);
	if (*icom > *jcom) {
	    getinp_(&iunit, idump, "TITLE: ", title, &iostat, (ftnlen)7, (
		    ftnlen)72);
	}
/*  ENTER LINE INTERVALS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'I' || *(unsigned char *)
	    &cin[*icom * 72] == 'i') {
	++(*icom);
	if (*icom > *jcom) {
	    mesage_("ENTER LINE INTERVALS IN THE FOLLOWING FORMAT:", (ftnlen)
		    45);
	    mesage_("[ LINE NO. (OR NEG SIDE NO.), INTERVALS ]", (ftnlen)41);
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	}
L350:
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
	    goto L350;
	}
/*  ENTER LINE FACTORS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'F' || *(unsigned char *)
	    &cin[*icom * 72] == 'f') {
	++(*icom);
	if (*icom > *jcom) {
	    mesage_("ENTER LINE FACTORS IN THE FOLLOWING FORMAT:", (ftnlen)43)
		    ;
	    mesage_("[ LINE NO. (OR NEG. SIDE NO., ) FACTOR ]", (ftnlen)40);
	    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	}
L360:
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
	    goto L360;
	}
/*  ENTER MATERIAL NUMBERS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'M' || *(unsigned char *)
	    &cin[*icom * 72] == 'm') {
	++(*icom);
	mesage_("ENTER REGION MATERIALS IN THE FOLLOWING FORMAT:", (ftnlen)47)
		;
	mesage_("[ REGION NO., MATERIAL NO. ]", (ftnlen)28);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
	*icom = *jcom + 1;
L370:
	frefld_(&iz, &iz, ">", mcom, &iostat, &ifound, &kin[1], cin + 72, &
		iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	if (ifound > 0) {
	    jj = iin[1];
	    ltsort_(mr, &linkr[3], &jj, &ipntr, &addlnk);
	    if (jj > n[22] || ipntr <= 0) {
		s_wsfe(&io___58);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		imat[ipntr] = iin[2];
	    }
	    goto L370;
	}
/*  FLAG THE THREE-NODE ELEMENT OPTION */
    } else if (s_cmp(cin + *icom * 72, "TH", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "th", (ftnlen)1, (ftnlen)2) == 0) {
	++(*icom);
	if (*three) {
	    *three = FALSE_;
	    mesage_("THREE NODE ELEMENT GENERATION - OFF", (ftnlen)35);
	} else {
	    *three = TRUE_;
	    mesage_("THREE NODE ELEMENT GENERATION - ON", (ftnlen)34);
	}
/*  FLAG THE EIGHT-NODE ELEMENT OPTION */
    } else if (s_cmp(cin + *icom * 72, "EI", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ei", (ftnlen)1, (ftnlen)2) == 0) {
	++(*icom);
	if (*eight) {
	    *eight = FALSE_;
	    mesage_("EIGHT NODE ELEMENT GENERATION - OFF", (ftnlen)35);
	} else {
	    *eight = TRUE_;
	    *nine = FALSE_;
	    mesage_("EIGHT NODE ELEMENT GENERATION - ON", (ftnlen)34);
	}
/*  FLAG THE NINE-NODE ELEMENT OPTION */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'N' || *(unsigned char *)
	    &cin[*icom * 72] == 'n') {
	++(*icom);
	if (*nine) {
	    *nine = FALSE_;
	    mesage_("NINE NODE ELEMENT GENERATION - OFF", (ftnlen)34);
	} else {
	    *nine = TRUE_;
	    *eight = FALSE_;
	    mesage_("NINE NODE ELEMENT GENERATION - ON", (ftnlen)33);
	}
/*  EXIT OPTION - EXITS FASTQ */
    } else if (s_cmp(cin + *icom * 72, "EX", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ex", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	strlng_(cin + *icom * 72, &len, (ftnlen)72);
	if (len > 1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'X' || len > 
		1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'x') {
	    help_fq__(&c__9);
	} else {
	    fexit_(wrote, mcom, icom, jcom, cin + 72, &iin[1], &rin[1], &kin[
		    1], time1, batch, versn, (ftnlen)72, (ftnlen)9);
	}
	goto L110;
/*  LINK ALL NEW DATA AS NEEDED, AND RETURN FROM THE KEYIN OPTION */
    } else if (*(unsigned char *)&cin[*icom * 72] == ' ') {
	++(*icom);
/*  LINK UP THE POINTS AND LINES TO THEIR ASSOCIATED FLAGS */
	sideok = FALSE_;
	i__1 = nold[10] + 1;
	linkbc_(mp, ms, &i__1, &n[11], &n[1], &n[25], &n[11], &n[12], &n[20], 
		&ipbf[1], &ifpb[1], &nppf[1], &listpb[3], &nlps[1], &ifline[1]
		, &illist[1], &ipboun[1], &linkpb[3], &iwtpbf[4], &linkp[3], &
		links[3], &sideok, noroom);
	if (*noroom) {
	    goto L400;
	}
	sideok = TRUE_;
	i__1 = nold[12] + 1;
	linkbc_(ml, ms, &i__1, &n[13], &n[2], &n[26], &n[13], &n[14], &n[20], 
		&ilbf[1], &iflb[1], &nlpf[1], &listlb[3], &nlps[1], &ifline[1]
		, &illist[1], &ilboun[1], &linklb[3], &iwtlbf[4], &linkl[3], &
		links[3], &sideok, noroom);
	if (*noroom) {
	    goto L400;
	}
	i__1 = nold[14] + 1;
	linkbc_(ml, ms, &i__1, &n[15], &n[2], &n[27], &n[15], &n[16], &n[20], 
		&isbf[1], &ifsb[1], &nspf[1], &listsb[3], &nlps[1], &ifline[1]
		, &illist[1], &isboun[1], &linksb[3], &iwtsbf[4], &linkl[3], &
		links[3], &sideok, noroom);
	if (*noroom) {
	    goto L400;
	}
/*  IF NO BODY CARDS HAVE BEEN READ, ASSUME THE BODY IS ALL THE */
/*  REGIONS AND ALL THE BAR SETS */
	if (n[9] == nold[8]) {
	    addold = TRUE_;
	    ifound = 1;
	    i__1 = n[5];
	    for (i__ = nold[4] + 1; i__ <= i__1; ++i__) {
		ltsort_(ms, &linkb[3], &ibarst[i__], &ipntr, &addlnk);
		if (ipntr == i__) {
		    iii[0] = -ibarst[i__];
		    inbody_(mr, &n[9], iii, &ifound, &irpb[1], &addold, 
			    noroom);
		    if (*noroom) {
			goto L400;
		    }
		}
/* L380: */
	    }
	    i__1 = n[7];
	    for (i__ = nold[6] + 1; i__ <= i__1; ++i__) {
		ltsort_(mr, &linkr[3], &iregn[i__], &ipntr, &addlnk);
		if (ipntr == i__) {
		    inbody_(mr, &n[9], &iregn[i__], &ifound, &irpb[1], &
			    addold, noroom);
		    if (*noroom) {
			goto L400;
		    }
		}
/* L390: */
	    }
	}
	return 0;
    } else {
	++(*icom);
	help_fq__(&c__9);
    }
    goto L110;
/*  MORE ROOM IN DIMENSIONS NEEDED */
L400:
    mesage_(" ", (ftnlen)1);
    mesage_("DIMENSIONS MUST BE INCREASED - PLEASE WAIT", (ftnlen)42);
    for (i__ = 1; i__ <= 29; ++i__) {
	n[i__] = nold[i__ - 1];
/* L410: */
    }
    *noroom = TRUE_;
    return 0;
} /* keyin_ */

#ifdef __cplusplus
	}
#endif

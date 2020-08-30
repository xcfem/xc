/* rdfsq.f -- translated by f2c (version 20160102).
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

static integer c__1000 = 1000;
static integer c__1 = 1;
static integer c__2 = 2;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int rdfsq_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *msnap, integer *msc, integer *ma, integer *iunit, 
	integer *idump, integer *n, integer *ipoint, real *coor, integer *
	ipboun, integer *iline, integer *ltype, integer *nint, real *factor, 
	integer *lcon, integer *ilboun, integer *isboun, integer *iside, 
	integer *nlps, integer *ifline, integer *illist, integer *ibarst, 
	integer *jmat, integer *jcent, integer *nlpb, integer *jfline, 
	integer *jllist, integer *iregn, integer *imat, integer *nspr, 
	integer *ifside, integer *islist, integer *irpb, integer *ipbf, 
	integer *nppf, integer *ifpb, integer *listpb, integer *ilbf, integer 
	*nlpf, integer *iflb, integer *listlb, integer *isbf, integer *nspf, 
	integer *ifsb, integer *listsb, real *attrib, integer *linkp, integer 
	*linkl, integer *links, integer *linkb, integer *linkr, integer *
	linkm, integer *linksc, integer *linkpb, integer *linklb, integer *
	linksb, integer *iholdp, integer *iholdl, integer *iholds, integer *
	iholdb, integer *iholdr, integer *iholdm, integer *ihold1, integer *
	ihold2, integer *ihold3, integer *iwtpbf, integer *iwtlbf, integer *
	iwtsbf, real *rsize, integer *ifhole, integer *nhpr, integer *ihlist, 
	integer *irgflg, integer *ischm, char *scheme, char *number, char *
	defsch, real *defsiz, char *title, logical *optim, logical *merge, 
	logical *three, logical *eight, logical *nine, logical *snap, real *
	snapdx, integer *nsnap, real *ratio, logical *noroom, logical *
	exodusii, ftnlen scheme_len, ftnlen number_len, ftnlen defsch_len, 
	ftnlen title_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 A POINT NO. OF:\002,i7,\002 IS NOT ALLO\
WED\002,/,\002 THIS POINT WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10010[] = "(\002 A LINE NO. OF:\002,i7,\002 IS NOT ALLOW\
ED\002,/,\002 THIS LINE WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10020[] = "(\002 FOR LINE NO.:\002,i5,\002 NOT ENOUGH IN\
FORMATION IS \002,\002SUPPLIED\002,/,\002 THIS LINE WILL NOT BE INPUT INTO D\
ATABASE\002)";
    static char fmt_10030[] = "(\002 LINE TYPE \"\002,a4,\002\" FOR LINE:\
\002,i5,\002 IS ILLEGAL.\002,/,\002 LINE TYPE CHANGED TO \"STRAIGHT\"\002)";
    static char fmt_10040[] = "(\002 A SIDE NO. OF:\002,i7,\002 IS NOT ALLOW\
ED\002,/,\002 THIS SIDE WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10050[] = "(\002 FOR SIDE NO.:\002,i5,\002 NOT ENOUGH IN\
FORMATION IS \002,\002SUPPLIED\002,/,\002 THIS SIDE WILL NOT BE INPUT INTO D\
ATABASE\002)";
    static char fmt_10060[] = "(\002 A REGION NO. OF:\002,i7,\002 IS NOT ALL\
OWED\002,/,\002 THIS REGION WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10080[] = "(\002 FOR REGION NO.:\002,i5,\002 NOT ENOUGH \
INFORMATION IS \002,\002SUPPLIED\002/,\002 THIS REGION WILL NOT BE INPUT INT\
O DATABASE\002)";
    static char fmt_10090[] = "(\002 A REGION NO. OF:\002,i7,\002 IS NOT IN \
THE DATABASE\002,/,\002 THE HOLES FOR THIS REGION WILL NOT BE INPUT INTO DAT\
ABASE\002)";
    static char fmt_10100[] = "(\002 FOR HOLE REGION NO.:\002,i5,\002 NOT EN\
OUGH INFORMATION \002,\002IS SUPPLIED\002/,\002 THE HOLES FOR THIS REGION WI\
LL NOT BE INPUT \002,\002INTO DATABASE\002)";
    static char fmt_10110[] = "(\002 A REGION NO. OF:\002,i7,\002 IS NOT ALL\
OWED\002,/,\002 THE SCHEME FOR THIS REGION WILL NOT BE INPUT INTO DATABAS\
E\002)";
    static char fmt_10120[] = "(\002 THE DEFAULT SCHEME HAS NOT BEEN SPECIFI\
ED ADEQUATELY\002,/,\002 THIS DEFAULT WILL NOT BE INPUT INTO THE DATABASE\
\002)";
    static char fmt_10130[] = "(\002 FOR THE SCHEME FOR REGION NO.:\002,i5\
,\002 NOT ENOUGH INFORMATION IS SUPPLIED\002,/,\002 THIS SCHEME WILL NOT BE \
INPUT INTO DATABASE\002)";
    static char fmt_10140[] = "(\002 FOR INTERVALS TO BE INPUT ON LINES IN S\
IDE: \002,i5,/,\002 NOT ENOUGH INFORMATION IS SUPPLIED\002,/,\002 THESE INTE\
RVALS WILL NOT BE ASSIGNED\002)";
    static char fmt_10150[] = "(\002 FOR INTERVALS TO BE INPUT ON LINE: \002\
,i5,/,\002 NOT ENOUGH INFORMATION IS SUPPLIED\002,/,\002 THIS INTERVAL WILL \
NOT BE ASSIGNED\002)";
    static char fmt_10160[] = "(\002 FOR FACTORS TO BE INPUT ON LINES IN SID\
E: \002,i5,/,\002 NOT ENOUGH INFORMATION IS SUPPLIED\002,/,\002 THESE INTERV\
ALS WILL NOT BE ASSIGNED\002)";
    static char fmt_10170[] = "(\002 FOR FACTORS TO BE INPUT ON LINE: \002,i\
5,/,\002 NOT ENOUGH INFORMATION IS SUPPLIED\002,/,\002 THIS INTERVAL WILL NO\
T BE ASSIGNED\002)";
    static char fmt_10070[] = "(\002 REGION NO:\002,i5,\002 IS NOT IN THE DA\
TABASE\002,/,\002 THUS NO SIZE CAN BE ENTERED\002)";
    static char fmt_10200[] = "(\002 FOR POINBC NO.:\002,i5,\002 NOT ENOUGH \
INFORMATION IS \002,\002SUPPLIED\002,/,\002 THIS POINBC WILL NOT BE INPUT IN\
TO DATABASE\002)";
    static char fmt_10210[] = "(\002 OLD POINBC NO:\002,i5,\002 TO NEW POINB\
C NO:\002,i5)";
    static char fmt_10220[] = "(\002 FOR NODEBC NO.:\002,i5,\002 NOT ENOUGH \
INFORMATION IS \002,\002SUPPLIED\002,/,\002 THIS NODEBC WILL NOT BE INPUT IN\
TO DATABASE\002)";
    static char fmt_10230[] = "(\002 OLD NODEBC NO:\002,i5,\002 TO NEW NODEB\
C NO:\002,i5)";
    static char fmt_10240[] = "(\002 FOR ELEMBC NO.:\002,i5,\002 NOT ENOUGH \
INFORMATION IS \002,\002SUPPLIED\002,/,\002 THIS ELEMBC WILL NOT BE INPUT IN\
TO DATABASE\002)";
    static char fmt_10250[] = "(\002 OLD ELEMBC NO:\002,i5,\002 TO NEW ELEMB\
C NO:\002,i5)";
    static char fmt_10260[] = "(\002 WEIGHTING OF \002,a5,\002 FLAG:\002,i5\
,\002 NOT POSSIBLE\002,/,\002FLAG NOT FOUND\002)";
    static char fmt_10270[] = "(\002 WEIGHTING OF \002,a4,\002 FLAG:\002,i5\
,\002 NOT POSSIBLE\002,/,\002FLAG NOT FOUND\002)";
    static char fmt_10280[] = "(\002 WEIGHTING TYPE OF \002,a5,\002 CANNOT B\
E ENTERED\002)";
    static char fmt_10290[] = "(\002 NOT ENOUGH INFORMATION SUPPLIED WITH CA\
RD:  \002,a)";
    static char fmt_10300[] = "(\002 TITLE: \002,a)";
    static char fmt_10310[] = "(\002 \002,i5,\002 NEW POINTS READ   -    TOT\
AL POINTS:\002,i5)";
    static char fmt_10320[] = "(\002 \002,i5,\002 NEW LINES READ    -     TO\
TAL LINES:\002,i5)";
    static char fmt_10330[] = "(\002 \002,i5,\002 NEW SIDES READ    -     TO\
TAL SIDES:\002,i5)";
    static char fmt_10340[] = "(\002 \002,i5,\002 NEW BAR SETS READ -  TOTAL\
 BAR SETS:\002,i5)";
    static char fmt_10350[] = "(\002 \002,i5,\002 NEW REGIONS READ  -   TOTA\
L REGIONS:\002,i5)";
    static char fmt_10360[] = "(\002 \002,i5,\002 NEW SCHEMES READ  -   TOTA\
L SCHEMES:\002,i5)";
    static char fmt_10370[] = "(\002 \002,i5,\002 NEW POINBCS READ  -   TOTA\
L POINBCS:\002,i5)";
    static char fmt_10380[] = "(\002 \002,i5,\002 NEW NODEBCS READ  -   TOTA\
L NODEBCS:\002,i5)";
    static char fmt_10390[] = "(\002 \002,i5,\002 NEW ELEMBCS READ  -   TOTA\
L ELEMBCS:\002,i5)";
    static char fmt_10400[] = "(\002 \002,i5,\002 NEW RENUMS READ   -    TOT\
AL RENUMS:\002,i5)";
    static char fmt_10410[] = "(\002 \002,i5,\002 NEW HOLES READ    -     TO\
TAL HOLES:\002,i5)";
    static char fmt_10420[] = "(\002   NUMBER OF POINTS READ:\002,i5)";
    static char fmt_10430[] = "(\002    NUMBER OF LINES READ:\002,i5)";
    static char fmt_10440[] = "(\002    NUMBER OF SIDES READ:\002,i5)";
    static char fmt_10450[] = "(\002 NUMBER OF BAR SETS READ:\002,i5)";
    static char fmt_10460[] = "(\002  NUMBER OF REGIONS READ:\002,i5)";
    static char fmt_10480[] = "(\002  NUMBER OF SCHEMES READ:\002,i5)";
    static char fmt_10490[] = "(\002  NUMBER OF POINBCS READ:\002,i5)";
    static char fmt_10500[] = "(\002  NUMBER OF NODEBCS READ:\002,i5)";
    static char fmt_10510[] = "(\002  NUMBER OF ELEMBCS READ:\002,i5)";
    static char fmt_10520[] = "(\002   NUMBER OF RENUMS READ:\002,i5)";
    static char fmt_10470[] = "(\002    NUMBER OF HOLES READ:\002,i5)";

    /* System generated locals */
    integer i__1, i__2, i__3;
    doublereal d__1, d__2;
    alist al__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_cmp(char *, char *, ftnlen, ftnlen), s_wsfe(cilist *), do_fio(
	    integer *, char *, ftnlen), e_wsfe(), f_rew(alist *);

    /* Local variables */
    static integer i__, j, jj, kk;
    static char cin[72*1000];
    static integer iii[1], len, iin[1000], kin[1000];
    static real rin[1000];
    static logical err;
    static integer lend;
    static char hold[72];
    static integer nold[29], idum, irsz, ltyp, nold10, lhold, nold24, jhold, 
	    jjnew, newmp, newml, newms, ipntr, newmr, nhold1, nhold2, nhold3;
    static logical addold, addlnk;
    extern /* Subroutine */ int frefld_(integer *, integer *, char *, integer 
	    *, integer *, integer *, integer *, char *, integer *, real *, 
	    ftnlen, ftnlen), mesage_(char *, ftnlen);
    static logical nodata, dolink, newnum, sideok;
    static integer nholdp, nholdl, nholds, nholdb, nholdr, nholdm, iostat, 
	    ifound;
    extern /* Subroutine */ int strlng_(char *, integer *, ftnlen), getinp_(
	    integer *, integer *, char *, char *, integer *, ftnlen, ftnlen), 
	    strcut_(char *, ftnlen), inpoin_(integer *, integer *, integer *, 
	    integer *, real *, real *, integer *, integer *, integer *, real *
	    , integer *, integer *, logical *, logical *), inline_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, logical *, logical *), inside_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, logical *), inbrst_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, logical *), inregn_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, logical *), ltsort_(
	    integer *, integer *, integer *, integer *, logical *);
    static integer jjpntr;
    extern /* Subroutine */ int ingrpn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *), inhole_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *), inschm_(integer *, integer *, 
	    integer *, integer *, integer *, char *, integer *, char *, 
	    integer *, char *, logical *, logical *, ftnlen, ftnlen, ftnlen), 
	    inintr_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *), infact_(integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     inbody_(integer *, integer *, integer *, integer *, integer *, 
	    logical *, logical *), inboun_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *, 
	    logical *, logical *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     inrenm_(integer *, integer *, char *, real *, integer *, integer 
	    *, char *, logical *, ftnlen, ftnlen), ingrid_(integer *, real *, 
	    integer *, integer *, real *, integer *, logical *), linkbc_(
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, logical *);

    /* Fortran I/O blocks */
    static cilist io___26 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___27 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___28 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___30 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___31 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___32 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___33 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___34 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___35 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___36 = { 0, 6, 0, fmt_10080, 0 };
    static cilist io___38 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___39 = { 0, 6, 0, fmt_10080, 0 };
    static cilist io___41 = { 0, 6, 0, fmt_10090, 0 };
    static cilist io___42 = { 0, 6, 0, fmt_10100, 0 };
    static cilist io___43 = { 0, 6, 0, fmt_10110, 0 };
    static cilist io___44 = { 0, 6, 0, fmt_10120, 0 };
    static cilist io___45 = { 0, 6, 0, fmt_10130, 0 };
    static cilist io___49 = { 0, 6, 0, fmt_10140, 0 };
    static cilist io___50 = { 0, 6, 0, fmt_10150, 0 };
    static cilist io___51 = { 0, 6, 0, fmt_10160, 0 };
    static cilist io___52 = { 0, 6, 0, fmt_10170, 0 };
    static cilist io___54 = { 0, 6, 0, fmt_10070, 0 };
    static cilist io___55 = { 0, 6, 0, fmt_10200, 0 };
    static cilist io___59 = { 0, 6, 0, fmt_10210, 0 };
    static cilist io___60 = { 0, 6, 0, fmt_10220, 0 };
    static cilist io___61 = { 0, 6, 0, fmt_10230, 0 };
    static cilist io___62 = { 0, 6, 0, fmt_10240, 0 };
    static cilist io___63 = { 0, 6, 0, fmt_10250, 0 };
    static cilist io___64 = { 0, 6, 0, fmt_10260, 0 };
    static cilist io___65 = { 0, 6, 0, fmt_10270, 0 };
    static cilist io___66 = { 0, 6, 0, fmt_10270, 0 };
    static cilist io___67 = { 0, 6, 0, fmt_10280, 0 };
    static cilist io___69 = { 0, 6, 0, fmt_10290, 0 };
    static cilist io___71 = { 0, 6, 0, fmt_10290, 0 };
    static cilist io___76 = { 0, 6, 0, fmt_10300, 0 };
    static cilist io___77 = { 0, 6, 0, fmt_10310, 0 };
    static cilist io___78 = { 0, 6, 0, fmt_10320, 0 };
    static cilist io___79 = { 0, 6, 0, fmt_10330, 0 };
    static cilist io___80 = { 0, 6, 0, fmt_10340, 0 };
    static cilist io___81 = { 0, 6, 0, fmt_10350, 0 };
    static cilist io___82 = { 0, 6, 0, fmt_10360, 0 };
    static cilist io___83 = { 0, 6, 0, fmt_10370, 0 };
    static cilist io___84 = { 0, 6, 0, fmt_10380, 0 };
    static cilist io___85 = { 0, 6, 0, fmt_10390, 0 };
    static cilist io___86 = { 0, 6, 0, fmt_10400, 0 };
    static cilist io___87 = { 0, 6, 0, fmt_10410, 0 };
    static cilist io___88 = { 0, 6, 0, fmt_10420, 0 };
    static cilist io___89 = { 0, 6, 0, fmt_10430, 0 };
    static cilist io___90 = { 0, 6, 0, fmt_10440, 0 };
    static cilist io___91 = { 0, 6, 0, fmt_10450, 0 };
    static cilist io___92 = { 0, 6, 0, fmt_10460, 0 };
    static cilist io___93 = { 0, 6, 0, fmt_10480, 0 };
    static cilist io___94 = { 0, 6, 0, fmt_10490, 0 };
    static cilist io___95 = { 0, 6, 0, fmt_10500, 0 };
    static cilist io___96 = { 0, 6, 0, fmt_10510, 0 };
    static cilist io___97 = { 0, 6, 0, fmt_10520, 0 };
    static cilist io___98 = { 0, 6, 0, fmt_10470, 0 };


/* *********************************************************************** */
/*  SUBROUTINE RDFSQ =  READS AND/OR MERGES FASTQ CARD FILE(S) */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     FASTQ  =  A PROGRAM TO QUICKLY PREPARE FASTQ INPUT */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     TITLE   =  MESH TITLE */
/*     IHOLDP  =  AN ARRAY TO HOLD THE POINTS DURING RENUMBERING */
/*     IHOLDL  =  AN ARRAY TO HOLD THE LINES DURING RENUMBERING */
/*     IHOLDS  =  AN ARRAY TO HOLD THE SIDES DURING RENUMBERING */
/*     IHOLDR  =  AN ARRAY TO HOLD THE REGIONS DURING RENUMBERING */
/*     DUMB     =  DUMMY VARIABLE WHERE THE DATA IS READ IN */
/*     OPTIM   =  .TRUE. IF THE MESH IS TO BE OPTIMIZED */
/*     MERGE   =  .TRUE. IF THE DATA IS TO BE MERGED WITH EXISTING DATA */
/*     NOROOM  =  .TRUE. IF THE AMOUNT OF DATA EXCEEDS DIMENSIONED LIMITS */
/*     NODATA  =  .TRUE. IF NO DATA HAS BEEN READ FROM THE FILE */
/* *********************************************************************** */
/*  SET UP THE INITIALIZATION OF VARIABLES */
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
    snapdx -= 3;
    number -= 80;
    scheme -= 72;
    --ischm;
    --attrib;
    --n;
    --nsnap;

    /* Function Body */
    for (i__ = 1; i__ <= 29; ++i__) {
	nold[i__ - 1] = n[i__];
/* L100: */
    }
    lhold = 0;
    nholdp = 0;
    nholdl = 0;
    nholds = 0;
    nholdb = 0;
    nholdr = 0;
    nholdm = 0;
    nhold1 = 0;
    nhold2 = 0;
    nhold3 = 0;
    *three = FALSE_;
    *eight = FALSE_;
    *nine = FALSE_;
    *optim = FALSE_;
    *noroom = FALSE_;
    addold = TRUE_;
    nodata = TRUE_;
    addlnk = TRUE_;
    s_copy(defsch, "M", (ftnlen)72, (ftnlen)1);
/*  READ THE INPUT CARDS AND SORT AS NEEDED */
    i__1 = *mp + (*ml << 1) + *ms + (*mr << 1);
    for (i__ = 1; i__ <= i__1; ++i__) {
	frefld_(iunit, idump, " ", &c__1000, &iostat, &ifound, kin, cin, iin, 
		rin, (ftnlen)1, (ftnlen)72);
/*  CHECK FOR THE END OF THE FILE OR FOR AN ERROR */
	if (iostat < 0) {
	    goto L140;
	}
/*  INPUT THE TITLE */
	if (s_cmp(cin, "TITLE", (ftnlen)5, (ftnlen)5) == 0) {
	    nodata = FALSE_;
	    if (*merge) {
		strlng_(title, &len, (ftnlen)72);
		if (len <= 70) {
		    lhold = len;
		    i__2 = len;
		    s_copy(title + i__2, ":", len + 1 - i__2, (ftnlen)1);
		    getinp_(iunit, idump, " ", hold, &iostat, (ftnlen)1, (
			    ftnlen)72);
		    strcut_(hold, (ftnlen)72);
		    lend = 71 - len;
		    i__2 = len + 1;
		    s_copy(title + i__2, hold, 72 - i__2, lend);
		}
	    } else {
		getinp_(iunit, idump, " ", title, &iostat, (ftnlen)1, (ftnlen)
			72);
	    }
	    strcut_(title, (ftnlen)72);
	    strlng_(title, &len, (ftnlen)72);
/*  INPUT A POINT INTO THE DATABASE */
	} else if (s_cmp(cin, "POINT", (ftnlen)5, (ftnlen)5) == 0) {
	    nodata = FALSE_;
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___26);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    inpoin_(mp, &n[1], &n[18], &jj, &rin[2], &rin[3], &nholdp, &
		    iholdp[3], &ipoint[1], &coor[3], &ipboun[1], &linkp[3], 
		    merge, noroom);
	    if (*noroom) {
		goto L310;
	    }
/*  INPUT A LINE INTO THE DATABASE */
	} else if (s_cmp(cin, "LINE ", (ftnlen)5, (ftnlen)5) == 0) {
	    nodata = FALSE_;
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___27);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    } else if (ifound < 5) {
		s_wsfe(&io___28);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    if (s_cmp(cin + 144, "STR ", (ftnlen)4, (ftnlen)4) == 0 || s_cmp(
		    cin + 144, "    ", (ftnlen)4, (ftnlen)4) == 0) {
		ltyp = 1;
	    } else if (s_cmp(cin + 144, "CIRC", (ftnlen)4, (ftnlen)4) == 0) {
		ltyp = 3;
	    } else if (s_cmp(cin + 144, "CIRM", (ftnlen)4, (ftnlen)4) == 0) {
		ltyp = 4;
	    } else if (s_cmp(cin + 144, "CIRR", (ftnlen)4, (ftnlen)4) == 0) {
		ltyp = 6;
	    } else if (s_cmp(cin + 144, "PARA", (ftnlen)4, (ftnlen)4) == 0) {
		ltyp = 5;
	    } else if (s_cmp(cin + 144, "CORN", (ftnlen)4, (ftnlen)4) == 0) {
		ltyp = 2;
	    } else if (s_cmp(cin + 144, "ELIP", (ftnlen)4, (ftnlen)4) == 0) {
		ltyp = 7;
	    } else if (s_cmp(cin + 144, "ELPR", (ftnlen)4, (ftnlen)4) == 0) {
		ltyp = 8;
	    } else {
		ltyp = 1;
		s_wsfe(&io___30);
		do_fio(&c__1, cin + 144, (ftnlen)4);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    inline_(ml, &n[2], &n[19], &jj, &ltyp, &iin[3], &iin[4], &iin[5], 
		    &iin[6], &rin[7], &nholdl, &iholdl[3], &iline[1], &ltype[
		    1], &nint[1], &factor[1], &lcon[4], &ilboun[1], &isboun[1]
		    , &linkl[3], merge, noroom);
	    if (*noroom) {
		goto L310;
	    }
/*  INPUT A SIDE INTO THE DATABASE */
	} else if (s_cmp(cin, "SIDE ", (ftnlen)5, (ftnlen)5) == 0) {
	    nodata = FALSE_;
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___31);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    } else if (ifound < 3) {
		s_wsfe(&io___32);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    i__2 = ifound - 2;
	    inside_(ms, &n[3], &n[4], &n[20], &jj, &iin[2], &i__2, &iside[1], 
		    &nlps[1], &ifline[1], &illist[1], &links[3], &nholds, &
		    iholds[3], merge, noroom);
	    if (*noroom) {
		goto L310;
	    }
/*  INPUT A BAR SET INTO THE DATABASE */
	} else if (s_cmp(cin, "BARSET", (ftnlen)6, (ftnlen)6) == 0) {
	    nodata = FALSE_;
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___33);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    } else if (ifound < 5) {
		s_wsfe(&io___34);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    i__2 = ifound - 4;
	    inbrst_(ms, mr, &n[5], &n[6], &n[21], &n[23], &jj, &iin[2], &iin[
		    3], &iin[4], &i__2, &ibarst[1], &jmat[1], &jcent[1], &
		    nlpb[1], &jfline[1], &jllist[1], &linkb[3], &linkm[3], &
		    nholdm, &iholdm[3], &nholdb, &iholdb[3], merge, noroom);
	    if (*noroom) {
		goto L310;
	    }
/*  INPUT A REGION INTO THE DATABASE */
	} else if (s_cmp(cin, "REGION", (ftnlen)6, (ftnlen)6) == 0) {
	    nodata = FALSE_;
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___35);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    } else if (ifound < 4) {
		s_wsfe(&io___36);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    i__2 = ifound - 3;
	    inregn_(ms, mr, &n[7], &n[8], &n[22], &n[23], &jj, &iin[2], &iin[
		    3], &i__2, &iregn[1], &imat[1], &nspr[1], &ifside[1], &
		    islist[1], &linkr[3], &linkm[3], &nholdr, &iholdr[3], &
		    nholdm, &iholdm[3], &irgflg[1], merge, noroom);
	    addlnk = FALSE_;
	    ltsort_(mr, &linkr[3], &iin[1], &jjpntr, &addlnk);
	    addlnk = TRUE_;
	    rsize[jjpntr] = (float)0.;
	    nhpr[jjpntr] = 0;
	    if (*noroom) {
		goto L310;
	    }
/*  INPUT A GROUP INTO THE DATABASE */
	} else if (s_cmp(cin, "GROUP ", (ftnlen)6, (ftnlen)6) == 0) {
	    nodata = FALSE_;
	    jj = iin[1];
	    if (jj <= 0 || jj >= 10000) {
		s_wsfe(&io___38);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    } else if (ifound < 3) {
		s_wsfe(&io___39);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    i__2 = ifound - 2;
	    ingrpn_(ms, mr, &n[7], &n[8], &n[22], &jj, &iin[2], &i__2, &iregn[
		    1], &nspr[1], &ifside[1], &islist[1], &linkr[3], &nholdr, 
		    &iholdr[3], &irgflg[1], merge, noroom);
	    if (*noroom) {
		goto L310;
	    }
/*  INPUT A REGION'S HOLES INTO THE DATABASE */
	} else if (s_cmp(cin, "HOLE  ", (ftnlen)6, (ftnlen)6) == 0) {
	    nodata = FALSE_;
	    jj = iin[1];
	    addlnk = FALSE_;
	    ltsort_(mr, &linkr[3], &jj, &jjpntr, &addlnk);
	    if (*merge && jjpntr > 0) {
		ltsort_(mr, &iholdr[3], &jj, &jjnew, &addlnk);
		if (jjnew > 0) {
		    ltsort_(mr, &linkr[3], &jjnew, &jjpntr, &addlnk);
		}
	    }
	    addlnk = TRUE_;
	    if (jjpntr <= 0) {
		s_wsfe(&io___41);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    } else if (ifound < 3) {
		s_wsfe(&io___42);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    i__2 = ifound - 2;
	    inhole_(mr, &n[7], &n[29], &jjpntr, &iin[2], &i__2, &ifhole[1], &
		    nhpr[1], &ihlist[1], merge, noroom);
	    if (*noroom) {
		goto L310;
	    }
/*  INPUT A SCHEME INTO THE DATABASE */
	} else if (s_cmp(cin, "SCHEME", (ftnlen)6, (ftnlen)6) == 0) {
	    nodata = FALSE_;
	    jj = iin[1];
	    if (jj >= 10000) {
		s_wsfe(&io___43);
		do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    } else if (ifound < 3) {
		if (jj == 0) {
		    s_wsfe(&io___44);
		    e_wsfe();
		} else {
		    s_wsfe(&io___45);
		    do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
		goto L120;
	    }
	    dolink = FALSE_;
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
		mesage_("SCHEME CARD IS THUS IGNORED", (ftnlen)27);
		mesage_("************************************", (ftnlen)36);
	    }
/*  INPUT INTERVALS FOR SIDES OR LINES */
	} else if (s_cmp(cin, "INT", (ftnlen)3, (ftnlen)3) == 0) {
	    nodata = FALSE_;
	    jj = iin[2];
	    if (ifound < 3) {
		if (jj < 0) {
		    s_wsfe(&io___49);
		    i__2 = -jj;
		    do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
		    e_wsfe();
		} else {
		    s_wsfe(&io___50);
		    do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
		goto L120;
	    }
	    addlnk = FALSE_;
	    i__2 = ifound - 2;
	    inintr_(ml, ms, &i__2, &iin[1], &iin[2], &n[19], &n[20], &nint[1],
		     &nlps[1], &ifline[1], &illist[1], &linkl[3], &links[3], &
		    addlnk);
	    addlnk = TRUE_;
/*  INPUT FACTORS FOR SIDES OR LINES */
	} else if (s_cmp(cin, "FAC", (ftnlen)3, (ftnlen)3) == 0) {
	    nodata = FALSE_;
	    jj = iin[2];
	    if (ifound < 3) {
		if (jj < 0) {
		    s_wsfe(&io___51);
		    i__2 = -jj;
		    do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
		    e_wsfe();
		} else {
		    s_wsfe(&io___52);
		    do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
		goto L120;
	    }
	    addlnk = FALSE_;
	    i__2 = ifound - 2;
	    infact_(ml, ms, &i__2, &rin[1], &iin[2], &n[19], &n[20], &factor[
		    1], &nlps[1], &ifline[1], &illist[1], &linkl[3], &links[3]
		    , &addlnk);
	    addlnk = TRUE_;
/*  INPUT A REGION'S INTERVAL SIZE INTO THE DATABASE */
	} else if (s_cmp(cin, "SIZE ", (ftnlen)5, (ftnlen)5) == 0) {
	    nodata = FALSE_;
	    if (ifound < 3) {
		*defsiz = dmax(rin[1],(float)0.);
	    } else {
		addlnk = FALSE_;
		i__2 = ifound;
		for (irsz = 3; irsz <= i__2; ++irsz) {
		    ltsort_(mr, &linkr[3], &iin[irsz - 1], &jj, &addlnk);
		    if (jj > 0) {
			rsize[jj] = dmax(rin[1],(float)0.);
		    } else {
			s_wsfe(&io___54);
			do_fio(&c__1, (char *)&iin[irsz - 1], (ftnlen)sizeof(
				integer));
			e_wsfe();
		    }
/* L110: */
		}
		addlnk = TRUE_;
	    }
/*  INPUT A BODY DEFINITION INTO THE DATABASE */
	} else if (s_cmp(cin, "BODY", (ftnlen)4, (ftnlen)4) == 0) {
	    nodata = FALSE_;
	    if (ifound > 1) {
		i__2 = ifound - 1;
		inbody_(mr, &n[9], &iin[1], &i__2, &irpb[1], &addold, noroom);
		if (*noroom) {
		    goto L310;
		}
	    }
/*  INPUT A POINT BOUNDARY INTO THE DATABASE */
	} else if (s_cmp(cin, "POINBC", (ftnlen)6, (ftnlen)6) == 0) {
	    if (ifound < 3) {
		s_wsfe(&io___55);
		do_fio(&c__1, (char *)&iin[1], (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    nodata = FALSE_;
	    i__2 = ifound - 2;
	    inboun_(mp, &iin[1], &i__2, &iin[2], &n[25], &n[11], &n[12], &
		    nold[10], merge, noroom, &newnum, &nhold1, &ihold1[3], &
		    ipbf[1], &nppf[1], &ifpb[1], &listpb[3], &linkpb[3], &
		    iwtpbf[4], &jhold, &addold);
	    if (*noroom) {
		goto L310;
	    }
	    if (newnum) {
		addlnk = FALSE_;
		ltsort_(mp, &ihold1[3], &jhold, &ipntr, &addlnk);
		s_wsfe(&io___59);
		do_fio(&c__1, (char *)&jhold, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ipntr, (ftnlen)sizeof(integer));
		e_wsfe();
		addlnk = TRUE_;
	    }
/*  INPUT A LINE BOUNDARY INTO THE DATABASE */
	} else if (s_cmp(cin, "NODEBC", (ftnlen)6, (ftnlen)6) == 0 || s_cmp(
		cin, "LINEBC", (ftnlen)6, (ftnlen)6) == 0) {
	    if (ifound < 3) {
		s_wsfe(&io___60);
		do_fio(&c__1, (char *)&iin[1], (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    nodata = FALSE_;
	    i__2 = ifound - 2;
	    inboun_(ml, &iin[1], &i__2, &iin[2], &n[26], &n[13], &n[14], &
		    nold[12], merge, noroom, &newnum, &nhold2, &ihold2[3], &
		    ilbf[1], &nlpf[1], &iflb[1], &listlb[3], &linklb[3], &
		    iwtlbf[4], &jhold, &addold);
	    if (*noroom) {
		goto L310;
	    }
	    if (newnum) {
		addlnk = FALSE_;
		ltsort_(ml, &ihold2[3], &jhold, &ipntr, &addlnk);
		s_wsfe(&io___61);
		do_fio(&c__1, (char *)&jhold, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ipntr, (ftnlen)sizeof(integer));
		e_wsfe();
		addlnk = TRUE_;
	    }
/*  INPUT A SIDE BOUNDARY INTO THE DATABASE */
	} else if (s_cmp(cin, "ELEMBC", (ftnlen)6, (ftnlen)6) == 0 || s_cmp(
		cin, "SIDEBC", (ftnlen)6, (ftnlen)6) == 0) {
	    if (ifound < 3) {
		s_wsfe(&io___62);
		do_fio(&c__1, (char *)&iin[1], (ftnlen)sizeof(integer));
		e_wsfe();
		goto L120;
	    }
	    nodata = FALSE_;
	    i__2 = ifound - 2;
	    inboun_(ml, &iin[1], &i__2, &iin[2], &n[27], &n[15], &n[16], &
		    nold[14], merge, noroom, &newnum, &nhold3, &ihold3[3], &
		    isbf[1], &nspf[1], &ifsb[1], &listsb[3], &linksb[3], &
		    iwtsbf[4], &jhold, &addold);
	    if (*noroom) {
		goto L310;
	    }
	    if (newnum) {
		addlnk = FALSE_;
		ltsort_(ml, &ihold3[3], &jhold, &ipntr, &addlnk);
		s_wsfe(&io___63);
		do_fio(&c__1, (char *)&jhold, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ipntr, (ftnlen)sizeof(integer));
		e_wsfe();
		addlnk = TRUE_;
	    }
/*  INPUT A FLAG WEIGHTING INTO THE DATABASE */
	} else if (s_cmp(cin, "WEIGHT", (ftnlen)6, (ftnlen)6) == 0) {
	    addlnk = FALSE_;
	    nodata = FALSE_;
/*  GET THE FLAG TYPE */
	    if (*(unsigned char *)&cin[72] == 'P') {
		ltsort_(mp, &linkpb[3], &iin[2], &jj, &addlnk);
		if (jj > 0) {
		    iwtpbf[jj * 3 + 1] = iin[3];
		    iwtpbf[jj * 3 + 2] = iin[4];
		    iwtpbf[jj * 3 + 3] = 0;
		} else {
		    s_wsfe(&io___64);
		    do_fio(&c__1, "POINT", (ftnlen)5);
		    do_fio(&c__1, (char *)&iin[2], (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    } else if (*(unsigned char *)&cin[72] == 'L') {
		ltsort_(ml, &linklb[3], &iin[2], &jj, &addlnk);
		if (jj > 0) {
		    iwtlbf[jj * 3 + 1] = iin[3];
		    iwtlbf[jj * 3 + 2] = iin[4];
		    iwtlbf[jj * 3 + 3] = iin[5];
		} else {
		    s_wsfe(&io___65);
		    do_fio(&c__1, "LINE", (ftnlen)4);
		    do_fio(&c__1, (char *)&iin[2], (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    } else if (*(unsigned char *)&cin[72] == 'S') {
		ltsort_(ml, &linksb[3], &iin[2], &jj, &addlnk);
		if (jj > 0) {
		    iwtsbf[jj * 3 + 1] = iin[3];
		    iwtsbf[jj * 3 + 2] = iin[4];
		    iwtsbf[jj * 3 + 3] = iin[5];
		} else {
		    s_wsfe(&io___66);
		    do_fio(&c__1, "SIDE", (ftnlen)4);
		    do_fio(&c__1, (char *)&iin[2], (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    } else {
/*  NO FLAG TYPE HAS BEEN RECOGNIZED */
		s_wsfe(&io___67);
		do_fio(&c__1, cin + 72, (ftnlen)5);
		e_wsfe();
	    }
	    addlnk = TRUE_;
/*  FLAG THE BANDWIDTH OPTIMIZATION ROUTINES ON, AND READ A RENUM CARD */
	} else if (s_cmp(cin, "RENUM", (ftnlen)5, (ftnlen)5) == 0) {
	    nodata = FALSE_;
	    *optim = TRUE_;
	    if (ifound > 2) {
		s_copy(hold, cin + 72, (ftnlen)72, (ftnlen)72);
		idum = ifound - 2;
		inrenm_(msc, &n[28], hold, &rin[2], &iin[2], &idum, number + 
			80, noroom, (ftnlen)72, (ftnlen)80);
		if (*noroom) {
		    mesage_("************************************", (ftnlen)
			    36);
		    mesage_("NOT ENOUGH ROOM FOR RENUMBERING CARD", (ftnlen)
			    36);
		    mesage_("NO DYNAMIC DIMENSIONING INCREASES", (ftnlen)33);
		    mesage_("AVAILABLE FOR CHARACTER STRINGS", (ftnlen)31);
		    mesage_("RENUMBERING CARD IS THUS IGNORED", (ftnlen)32);
		    mesage_("************************************", (ftnlen)
			    36);
		}
	    } else if (ifound == 2) {
		mesage_("RENUM CARD READ WITHOUT ANY DATA", (ftnlen)32);
		mesage_("DEFAULT RENUM PROCESSING WILL BE USED", (ftnlen)37);
	    }
/*  Write Database in exodusII format */
	} else if (s_cmp(cin, "X2", (ftnlen)2, (ftnlen)2) == 0) {
	    nodata = FALSE_;
	    *exodusii = TRUE_;
/*  Write Database in exodusI/genesis format */
	} else if (s_cmp(cin, "X1", (ftnlen)2, (ftnlen)2) == 0) {
	    nodata = FALSE_;
	    *exodusii = FALSE_;
/*  FLAG THE GENERATION OF THREE NODE ELEMENTS */
	} else if (s_cmp(cin, "THREE", (ftnlen)5, (ftnlen)5) == 0) {
	    nodata = FALSE_;
	    *three = TRUE_;
/*  FLAG THE GENERATION OF EIGHT NODE ELEMENTS */
	} else if (s_cmp(cin, "EIGHT", (ftnlen)5, (ftnlen)5) == 0) {
	    nodata = FALSE_;
	    *eight = TRUE_;
	    *nine = FALSE_;
/*  FLAG THE GENERATION OF NINE NODE ELEMENTS */
	} else if (s_cmp(cin, "NINE", (ftnlen)4, (ftnlen)4) == 0) {
	    nodata = FALSE_;
	    *nine = TRUE_;
	    *eight = FALSE_;
/*  INPUT SNAP-TO-GRID FLAG */
	} else if (s_cmp(cin, "SNAP", (ftnlen)4, (ftnlen)4) == 0) {
	    nodata = FALSE_;
	    *snap = s_cmp(cin + 72, "ON", (ftnlen)2, (ftnlen)2) == 0;
/*  INPUT X-GRID LINES */
	} else if (s_cmp(cin, "XGRI", (ftnlen)4, (ftnlen)4) == 0) {
	    if (ifound < 2) {
		s_wsfe(&io___69);
		do_fio(&c__1, "XGRID", (ftnlen)5);
		e_wsfe();
		goto L120;
	    }
	    nodata = FALSE_;
	    i__2 = ifound - 1;
	    ingrid_(msnap, &snapdx[3], &nsnap[1], &c__1, &rin[1], &i__2, &err)
		    ;
/*  INPUT Y-GRID LINES */
	} else if (s_cmp(cin, "YGRI", (ftnlen)4, (ftnlen)4) == 0) {
	    if (ifound < 2) {
		s_wsfe(&io___71);
		do_fio(&c__1, "YGRID", (ftnlen)5);
		e_wsfe();
		goto L120;
	    }
	    nodata = FALSE_;
	    i__2 = ifound - 1;
	    ingrid_(msnap, &snapdx[3], &nsnap[1], &c__2, &rin[1], &i__2, &err)
		    ;
/*  END OF DATA */
	} else if (s_cmp(cin, "EXIT", (ftnlen)4, (ftnlen)4) == 0) {
	    nodata = FALSE_;
	    goto L150;
	}
L120:
/* L130: */
	;
    }
L140:
    mesage_("FILE END ENCOUNTERED BEFORE -EXIT- CARD WAS FOUND", (ftnlen)49);
    mesage_("POSSIBLE PROBLEM IN FILE", (ftnlen)24);
/*  RENUMBER THE CARDS IF MERGING */
L150:
    addlnk = FALSE_;
    if (*merge) {
/*  RENUMBER THE POINTS CONTAINED IN THE LINE, AND POINT BOUNDARY CARDS */
	i__1 = n[2];
	for (i__ = nold[1] + 1; i__ <= i__1; ++i__) {
	    for (j = 1; j <= 3; ++j) {
		ltsort_(mp, &iholdp[3], &lcon[j + i__ * 3], &ipntr, &addlnk);
		if (lcon[j + i__ * 3] <= nholdp && ipntr > 0) {
		    lcon[j + i__ * 3] = ipntr;
		}
/* L160: */
	    }
/* L170: */
	}
	i__1 = n[12];
	for (i__ = nold[11] + 1; i__ <= i__1; ++i__) {
	    ltsort_(mp, &iholdp[3], &listpb[(i__ << 1) + 1], &ipntr, &addlnk);
	    if (listpb[(i__ << 1) + 1] <= nholdp && ipntr > 0) {
		listpb[(i__ << 1) + 1] = ipntr;
	    }
/* L180: */
	}
/*  RENUMBER THE LINES CONTAINED IN THE SIDE, BAR SET, REGION, */
/*  LINE BOUNDARY, AND SIDE BOUNDARY CARDS */
	i__1 = n[4];
	for (i__ = nold[3] + 1; i__ <= i__1; ++i__) {
	    ltsort_(ml, &iholdl[3], &illist[i__], &ipntr, &addlnk);
	    if (illist[i__] <= nholdl && ipntr > 0) {
		illist[i__] = ipntr;
	    }
/* L190: */
	}
	i__1 = n[6];
	for (i__ = nold[5] + 1; i__ <= i__1; ++i__) {
	    ltsort_(ml, &iholdl[3], &jllist[i__], &ipntr, &addlnk);
	    if (jllist[i__] <= nholdl && ipntr > 0) {
		jllist[i__] = ipntr;
	    }
/* L200: */
	}
	i__1 = n[8];
	for (i__ = nold[7] + 1; i__ <= i__1; ++i__) {
	    if (islist[i__] < 0) {
		kk = (i__2 = islist[i__], abs(i__2));
		ltsort_(ml, &iholdl[3], &kk, &ipntr, &addlnk);
		if (kk <= nholdl && ipntr > 0) {
		    islist[i__] = -ipntr;
		}
	    }
/* L210: */
	}
	i__1 = n[14];
	for (i__ = nold[13] + 1; i__ <= i__1; ++i__) {
	    ltsort_(ml, &iholdl[3], &listlb[(i__ << 1) + 1], &ipntr, &addlnk);
	    if (listlb[(i__ << 1) + 1] <= nholdl && ipntr > 0) {
		listlb[(i__ << 1) + 1] = ipntr;
	    }
/* L220: */
	}
	i__1 = n[16];
	for (i__ = nold[15] + 1; i__ <= i__1; ++i__) {
	    ltsort_(ml, &iholdl[3], &listsb[(i__ << 1) + 1], &ipntr, &addlnk);
	    if (listsb[(i__ << 1) + 1] <= nholdl && ipntr > 0) {
		listsb[(i__ << 1) + 1] = ipntr;
	    }
/* L230: */
	}
/*  RENUMBER THE SIDES CONTAINED IN THE REGION CARDS */
	i__1 = n[8];
	for (i__ = nold[7] + 1; i__ <= i__1; ++i__) {
	    if (islist[i__] > 0) {
		ltsort_(ms, &iholds[3], &islist[i__], &ipntr, &addlnk);
		if (islist[i__] <= nholds && ipntr > 0) {
		    islist[i__] = ipntr;
		}
	    }
/* L240: */
	}
/*  RENUMBER THE REGIONS CONTAINED IN THE HOLE CARDS */
	i__1 = n[29];
	for (i__ = nold[28] + 1; i__ <= i__1; ++i__) {
	    if (ihlist[i__] > 0) {
		ltsort_(mr, &iholdr[3], &ihlist[i__], &ipntr, &addlnk);
		if (ihlist[i__] <= nholdr && ipntr > 0) {
		    ihlist[i__] = ipntr;
		}
	    }
/* L250: */
	}
/*  RENUMBER THE BAR SETS AND REGIONS CONTAINED IN THE BODY, */
/*  AND THE REGIONS CONTAINED IN THE SCHEME CARDS */
	i__1 = n[9];
	for (i__ = nold[8] + 1; i__ <= i__1; ++i__) {
	    if (irpb[i__] > 0) {
		ltsort_(mr, &iholdr[3], &irpb[i__], &ipntr, &addlnk);
		if (irpb[i__] <= nholdr && ipntr > 0) {
		    irpb[i__] = ipntr;
		}
	    } else if (irpb[i__] < 0) {
		i__3 = (i__2 = irpb[i__], abs(i__2));
		ltsort_(ms, &iholdb[3], &i__3, &ipntr, &addlnk);
		if ((i__2 = irpb[i__], abs(i__2)) <= nholdb && ipntr > 0) {
		    irpb[i__] = -ipntr;
		}
	    }
/* L260: */
	}
	i__1 = n[10];
	for (i__ = nold[9] + 1; i__ <= i__1; ++i__) {
	    ltsort_(mr, &iholdr[3], &ischm[i__], &ipntr, &addlnk);
	    if (ischm[i__] <= nholdr && ipntr > 0) {
		ischm[i__] = ipntr;
	    }
/* L270: */
	}
    }
/*  LINK THE SCHEME CARDS */
    addlnk = TRUE_;
    i__1 = n[10];
    for (i__ = nold[9] + 1; i__ <= i__1; ++i__) {
	if (ischm[i__] > n[24]) {
	    n[24] = ischm[i__];
	}
	ltsort_(mr, &linksc[3], &ischm[i__], &i__, &addlnk);
/* L280: */
    }
/*  LINK UP THE POINTS AND LINES TO THEIR ASSOCIATED FLAGS */
    sideok = FALSE_;
    i__1 = nold[10] + 1;
    linkbc_(mp, ms, &i__1, &n[11], &n[1], &n[25], &n[11], &n[12], &n[20], &
	    ipbf[1], &ifpb[1], &nppf[1], &listpb[3], &nlps[1], &ifline[1], &
	    illist[1], &ipboun[1], &linkpb[3], &iwtpbf[4], &linkp[3], &links[
	    3], &sideok, noroom);
    if (*noroom) {
	goto L310;
    }
    sideok = TRUE_;
    i__1 = nold[12] + 1;
    linkbc_(ml, ms, &i__1, &n[13], &n[2], &n[26], &n[13], &n[14], &n[20], &
	    ilbf[1], &iflb[1], &nlpf[1], &listlb[3], &nlps[1], &ifline[1], &
	    illist[1], &ilboun[1], &linklb[3], &iwtlbf[4], &linkl[3], &links[
	    3], &sideok, noroom);
    if (*noroom) {
	goto L310;
    }
    i__1 = nold[14] + 1;
    linkbc_(ml, ms, &i__1, &n[15], &n[2], &n[27], &n[15], &n[16], &n[20], &
	    isbf[1], &ifsb[1], &nspf[1], &listsb[3], &nlps[1], &ifline[1], &
	    illist[1], &isboun[1], &linksb[3], &iwtsbf[4], &linkl[3], &links[
	    3], &sideok, noroom);
    if (*noroom) {
	goto L310;
    }
/*  IF NO BODY CARDS HAVE BEEN READ, ASSUME THE BODY IS ALL THE REGIONS */
/*  AND ALL THE BAR SETS */
    addlnk = FALSE_;
    if (n[9] == nold[8]) {
	ifound = 1;
	i__1 = n[5];
	for (i__ = nold[4] + 1; i__ <= i__1; ++i__) {
	    ltsort_(ms, &linkb[3], &ibarst[i__], &ipntr, &addlnk);
	    if (ipntr == i__) {
		iii[0] = -ibarst[i__];
		inbody_(mr, &n[9], iii, &ifound, &irpb[1], &addold, noroom);
		if (*noroom) {
		    goto L310;
		}
	    }
/* L290: */
	}
	i__1 = n[7];
	for (i__ = nold[6] + 1; i__ <= i__1; ++i__) {
	    ltsort_(mr, &linkr[3], &iregn[i__], &ipntr, &addlnk);
	    if (ipntr == i__) {
		inbody_(mr, &n[9], &iregn[i__], &ifound, &irpb[1], &addold, 
			noroom);
		if (*noroom) {
		    goto L310;
		}
	    }
/* L300: */
	}
    }
/*  SUCCESSFUL COMPLETION - WRITE SUMMARY OF SUCCESSFUL READS */
    if (nodata) {
	mesage_(" ", (ftnlen)1);
	mesage_(" *----------------------------------------- - *", (ftnlen)47)
		;
	mesage_(" NO FASTQ DATA HAS BEEN FOUND IN CURRENT FILE", (ftnlen)45);
	mesage_(" *----------------------------------------- - *", (ftnlen)47)
		;
	return 0;
    }
    mesage_(" ", (ftnlen)1);
    mesage_("FILE SUCCESSFULLY READ", (ftnlen)22);
    strlng_(title, &len, (ftnlen)72);
    s_wsfe(&io___76);
    do_fio(&c__1, title, len);
    e_wsfe();
    if (*merge) {
	if (n[1] > 0) {
	    s_wsfe(&io___77);
	    i__1 = n[1] - nold[0];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[1], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[2] > 0) {
	    s_wsfe(&io___78);
	    i__1 = n[2] - nold[1];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[2], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[3] > 0) {
	    s_wsfe(&io___79);
	    i__1 = n[3] - nold[2];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[3], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[5] > 0) {
	    s_wsfe(&io___80);
	    i__1 = n[5] - nold[4];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[5], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[7] > 0) {
	    s_wsfe(&io___81);
	    i__1 = n[7] - nold[6];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[7], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[10] > 0) {
	    s_wsfe(&io___82);
	    i__1 = n[10] - nold[9];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[10], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[11] > 0) {
	    s_wsfe(&io___83);
	    i__1 = n[11] - nold[10];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[11], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[13] > 0) {
	    s_wsfe(&io___84);
	    i__1 = n[13] - nold[12];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[13], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[15] > 0) {
	    s_wsfe(&io___85);
	    i__1 = n[15] - nold[14];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[15], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[28] > 0) {
	    s_wsfe(&io___86);
	    i__1 = n[28] - nold[27];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[28], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[29] > 0) {
	    s_wsfe(&io___87);
	    i__1 = n[29] - nold[28];
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&n[29], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
    } else {
	if (n[1] > 0) {
	    s_wsfe(&io___88);
	    do_fio(&c__1, (char *)&n[1], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[2] > 0) {
	    s_wsfe(&io___89);
	    do_fio(&c__1, (char *)&n[2], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[3] > 0) {
	    s_wsfe(&io___90);
	    do_fio(&c__1, (char *)&n[3], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[5] > 0) {
	    s_wsfe(&io___91);
	    do_fio(&c__1, (char *)&n[5], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[7] > 0) {
	    s_wsfe(&io___92);
	    do_fio(&c__1, (char *)&n[7], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[10] > 0) {
	    s_wsfe(&io___93);
	    do_fio(&c__1, (char *)&n[10], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[11] > 0) {
	    s_wsfe(&io___94);
	    do_fio(&c__1, (char *)&n[11], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[13] > 0) {
	    s_wsfe(&io___95);
	    do_fio(&c__1, (char *)&n[13], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[15] > 0) {
	    s_wsfe(&io___96);
	    do_fio(&c__1, (char *)&n[15], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[28] > 0) {
	    s_wsfe(&io___97);
	    do_fio(&c__1, (char *)&n[28], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (n[29] > 0) {
	    s_wsfe(&io___98);
	    do_fio(&c__1, (char *)&n[29], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
    }
    return 0;
/*  MORE ROOM IN DIMENSIONS NEEDED */
L310:
    mesage_(" ", (ftnlen)1);
    mesage_("DIMENSIONS MUST BE INCREASED - PLEASE WAIT", (ftnlen)42);
    for (i__ = 1; i__ <= 29; ++i__) {
	n[i__] = nold[i__ - 1];
/* L320: */
    }
    if (lhold == 0) {
	s_copy(title, " ", (ftnlen)72, (ftnlen)1);
    } else {
	i__1 = lhold;
	s_copy(title + i__1, " ", 72 - i__1, (ftnlen)1);
    }
    *noroom = TRUE_;
/*  FIND OUT HOW MUCH ROOM IS NEEDED */
    al__1.aerr = 0;
    al__1.aunit = *iunit;
    f_rew(&al__1);
    newmp = 0;
    newml = 0;
    newms = 0;
    newmr = 0;
L330:
    frefld_(iunit, idump, " ", &c__1000, &iostat, &ifound, kin, cin, iin, rin,
	     (ftnlen)1, (ftnlen)72);
/*  CHECK FOR THE END OF THE FILE OR FOR AN ERROR */
    if (iostat < 0) {
	goto L340;
    }
/*  COUNT THE CARDS FOR NEEDED DIMENSIONING */
    if (s_cmp(cin, "POINT", (ftnlen)5, (ftnlen)5) == 0) {
	++newmp;
    } else if (s_cmp(cin, "LINE ", (ftnlen)5, (ftnlen)5) == 0) {
	++newml;
    } else if (s_cmp(cin, "SIDE ", (ftnlen)5, (ftnlen)5) == 0) {
	++newms;
    } else if (s_cmp(cin, "REGION", (ftnlen)6, (ftnlen)6) == 0) {
	++newmr;
    }
    goto L330;
/*  GET THE LARGEST RATIO OF NEEDED/CURRENT */
L340:
/* Computing MAX */
    d__1 = (doublereal) newmp / (doublereal) (*mp), d__2 = (doublereal) newml 
	    / (doublereal) (*ml), d__1 = max(d__1,d__2), d__2 = (doublereal) 
	    newms / (doublereal) (*ms), d__1 = max(d__1,d__2), d__2 = (
	    doublereal) newmr / (doublereal) (*mr), d__1 = max(d__1,d__2);
    *ratio = (real) max(d__1,1.5000001) * (float)1.1;
    return 0;
} /* rdfsq_ */

#ifdef __cplusplus
	}
#endif

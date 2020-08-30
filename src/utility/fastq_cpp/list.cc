/* list.f -- translated by f2c (version 20160102).
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
static integer c__4 = 4;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int list_(integer *mp, integer *ml, integer *ms, integer *mr,
	 integer *msc, integer *mcom, integer *icom, integer *jcom, char *cin,
	 real *rin, integer *iin, integer *kin, integer *n, integer *ipoint, 
	real *coor, integer *ipboun, integer *iline, integer *ltype, integer *
	nint, real *factor, integer *lcon, integer *ilboun, integer *isboun, 
	integer *iside, integer *nlps, integer *ifline, integer *illist, 
	integer *ibarst, integer *jmat, integer *jcent, integer *nlpb, 
	integer *jfline, integer *jllist, integer *iregn, integer *imat, 
	integer *nspr, integer *ifside, integer *islist, integer *irpb, 
	integer *ipbf, integer *nppf, integer *ifpb, integer *listpb, integer 
	*ilbf, integer *nlpf, integer *iflb, integer *listlb, integer *isbf, 
	integer *nspf, integer *ifsb, integer *listsb, integer *linkp, 
	integer *linkl, integer *links, integer *linkb, integer *linkr, 
	integer *linksc, integer *linkpb, integer *linklb, integer *linksb, 
	integer *iwtpbf, integer *iwtlbf, integer *iwtsbf, real *rsize, 
	integer *ifhole, integer *nhpr, integer *ihlist, integer *irgflg, 
	integer *ischm, char *scheme, char *number, char *defsch, real *
	defsiz, char *title, logical *optim, logical *three, logical *eight, 
	logical *nine, logical *vaxvms, logical *wrote, real *time1, char *
	versn, logical *batch, ftnlen cin_len, ftnlen scheme_len, ftnlen 
	number_len, ftnlen defsch_len, ftnlen title_len, ftnlen versn_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002  POINT            X(R)               Y(\
Z)        BOUNDARY\002,/,\002   NO.         COORDINATE         COORDINATE   \
     FLAG\002,/,\002  -----       ------------      -------------      -----\
---\002)";
    static char fmt_10010[] = "(1x,i5,3x,f15.4,4x,f15.4,4x,i5)";
    static char fmt_10020[] = "(\002   LINE  BEGINNING   ENDING   CENTER  NO\
. OF   NODE BC  \002,\002ELEM BC\002,/,\002    NO.    NODE       NODE     NO\
DE  INTERVALS   FLAG   \002,\002  FLAG    FACTOR\002,/,\002  -----  --------\
-   ------   ------ --------- -------- \002,\002-------- --------\002)";
    static char fmt_10040[] = "(1x,3(i5,4x),\002  -----\002,2x,3(i5,4x),f10.\
5)";
    static char fmt_10030[] = "(1x,7(i5,4x),f10.5)";
    static char fmt_10090[] = "(\002  SIDE\002,/,\002   NO.               SI\
DE/LINE LISTING\002,/,\002 ------  -----------------------------------------\
-----\002)";
    static char fmt_10110[] = "(i5,5x,11i6)";
    static char fmt_10120[] = "(10x,11i6)";
    static char fmt_10170[] = "(\002  FOR\002,/,\002 REGION                 \
 SCHEME\002,/,\002 ------  ----------------------------------------------\
\002)";
    static char fmt_10190[] = "(1x,i5,2x,a72)";
    static char fmt_10180[] = "(\002 DEFLT: \002,a72)";
    static char fmt_10130[] = "(\002  BAR SET  MAT.  REFR     BAR SET LIN\
E\002,/,\002    NO.    NO.   NODE        LISTING\002,/,\002 -------- ------ \
------ \002,\002-----------------------------------\002)";
    static char fmt_10140[] = "(i6,2x,i5,2x,i5,2x,11i5)";
    static char fmt_10150[] = "(22x,11i5)";
    static char fmt_10220[] = "(\002 CARD           RENUMBERING CARD\002,/\
,\002  NO.\002,/,\002 -----  -------------------------------------------\002)"
	    ;
    static char fmt_10230[] = "(1x,i5,2x,a72)";
    static char fmt_10240[] = "(8x,a8)";
    static char fmt_10050[] = "(\002 REGION  MAT.  INTERVAL    REGION\002,/\
,\002   NO.    NO.    SIZE      SCHEME       REGION SIDE/\002,\002LINE LISTI\
NG\002,/,\002 ------  ---- ---------- -----------  \002,\002----------------\
--------------------\002)";
    static char fmt_10060[] = "(i5,2x,i5,2x,f10.5,2x,a10,2x,7i6)";
    static char fmt_10070[] = "(38x,9i6)";
    static char fmt_10080[] = "(\002 GROUP\002/,\002   NO.                 R\
EGION LISTING\002,/,\002 ------  -------------------------------------------\
---\002)";
    static char fmt_10100[] = "(\002 REGION\002,/,\002   NO.              HO\
LE REGION LISTING\002,/,\002 ------  ---------------------------------------\
-------\002)";
    static char fmt_10200[] = "(\002     REGIONS IN THE BODY\002,/,\002 ----\
------------------------------------------------\002)";
    static char fmt_10210[] = "(1x,13i6)";

    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Builtin functions */
    integer s_wsfe(cilist *), e_wsfe(), do_fio(integer *, char *, ftnlen), 
	    s_cmp(char *, char *, ftnlen, ftnlen);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, j, k, l, i1, i2, k1, k2, j1, j2, iz, len;
    static logical ians;
    extern /* Subroutine */ int check_(integer *, integer *, integer *), 
	    geti12_(integer *, integer *, integer *, char *, integer *, 
	    integer *, integer *, integer *, integer *, ftnlen), fexit_(
	    logical *, integer *, integer *, integer *, char *, integer *, 
	    real *, integer *, real *, logical *, char *, ftnlen, ftnlen), 
	    spawn_(logical *);
    static char dummy[10];
    static integer ipntr;
    static char choice[7];
    static logical addlnk;
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen);
    static integer ifound, iostat;
    extern /* Subroutine */ int listbf_(integer *, integer *, char *, integer 
	    *, integer *, integer *, integer *, integer *, integer *, ftnlen);
    static logical lgroup;
    extern /* Subroutine */ int strlng_(char *, integer *, ftnlen);
    static real rrsize;
    extern /* Subroutine */ int intrup_(char *, logical *, integer *, integer 
	    *, integer *, char *, integer *, real *, integer *, ftnlen, 
	    ftnlen), ltsort_(integer *, integer *, integer *, integer *, 
	    logical *), help_fq__(integer *);

    /* Fortran I/O blocks */
    static cilist io___8 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___11 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___12 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___13 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___15 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_10090, 0 };
    static cilist io___19 = { 0, 6, 0, fmt_10110, 0 };
    static cilist io___20 = { 0, 6, 0, fmt_10120, 0 };
    static cilist io___21 = { 0, 6, 0, fmt_10170, 0 };
    static cilist io___22 = { 0, 6, 0, fmt_10190, 0 };
    static cilist io___23 = { 0, 6, 0, fmt_10180, 0 };
    static cilist io___24 = { 0, 6, 0, fmt_10130, 0 };
    static cilist io___25 = { 0, 6, 0, fmt_10140, 0 };
    static cilist io___26 = { 0, 6, 0, fmt_10150, 0 };
    static cilist io___27 = { 0, 6, 0, fmt_10220, 0 };
    static cilist io___28 = { 0, 6, 0, fmt_10230, 0 };
    static cilist io___29 = { 0, 6, 0, fmt_10240, 0 };
    static cilist io___31 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___35 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___36 = { 0, 6, 0, fmt_10070, 0 };
    static cilist io___37 = { 0, 6, 0, fmt_10080, 0 };
    static cilist io___38 = { 0, 6, 0, fmt_10110, 0 };
    static cilist io___39 = { 0, 6, 0, fmt_10120, 0 };
    static cilist io___40 = { 0, 6, 0, fmt_10100, 0 };
    static cilist io___41 = { 0, 6, 0, fmt_10110, 0 };
    static cilist io___42 = { 0, 6, 0, fmt_10120, 0 };
    static cilist io___44 = { 0, 6, 0, fmt_10200, 0 };
    static cilist io___46 = { 0, 6, 0, fmt_10210, 0 };
    static cilist io___48 = { 0, 6, 0, fmt_10210, 0 };


/* *********************************************************************** */
/*  SUBROUTINE LIST = LISTS POINTS, LINES, REGIONS, SCHEMES, AND BOUNDARY */
/*                    DEFINITIONS */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     FASTQ = A PROGRAM TO QUICKLY PREPARE QMESH INPUT */
/* *********************************************************************** */
/*  SUBROUTINES CALLED: */
/*     CHECK  = CHECKS 2 VALUES FOR BEING OUT OF PRESCRIBED BOUNDS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     IANS   = LOGICAL RESPONSE FROM YES-NO QUESTION */
/* *********************************************************************** */
    /* Parameter adjustments */
    iwtpbf -= 4;
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
    linksc -= 3;
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
    addlnk = FALSE_;
    *batch = FALSE_;
L100:
    if (*icom > *jcom) {
	mesage_(" ", (ftnlen)1);
	frefld_(&iz, &iz, "ENTER LIST OPTION: ", mcom, &iostat, jcom, &kin[1],
		 cin + 72, &iin[1], &rin[1], (ftnlen)19, (ftnlen)72);
	*icom = 1;
    }
/*  LIST OUT THE POINTS */
    if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned char *)&cin[*
	    icom * 72] == 'p') {
	++(*icom);
	if (n[1] > 0) {
	    mesage_(" ", (ftnlen)1);
	    intrup_("LIST ALL POINTS", &ians, mcom, icom, jcom, cin + 72, &
		    iin[1], &rin[1], &kin[1], (ftnlen)15, (ftnlen)72);
	    if (ians) {
		i1 = 1;
		i2 = n[18];
	    } else {
		mesage_("LIST POINTS <I1> THROUGH <I2>:", (ftnlen)30);
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[18]);
		} else {
		    goto L120;
		}
	    }
	    s_wsfe(&io___8);
	    e_wsfe();
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		ltsort_(mp, &linkp[3], &i__, &k, &addlnk);
		if (k > 0) {
		    s_wsfe(&io___11);
		    do_fio(&c__1, (char *)&ipoint[k], (ftnlen)sizeof(integer))
			    ;
		    do_fio(&c__1, (char *)&coor[(k << 1) + 1], (ftnlen)sizeof(
			    real));
		    do_fio(&c__1, (char *)&coor[(k << 1) + 2], (ftnlen)sizeof(
			    real));
		    do_fio(&c__1, (char *)&ipboun[k], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		}
/* L110: */
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*-----------------------------------*", (ftnlen)37);
	    mesage_("* NO POINTS IN THE CURRENT DATABASE *", (ftnlen)37);
	    mesage_("*-----------------------------------*", (ftnlen)37);
	}
L120:
/*  LIST OUT THE LINES */
	;
    } else if (*(unsigned char *)&cin[*icom * 72] == 'L' || *(unsigned char *)
	    &cin[*icom * 72] == 'l') {
	++(*icom);
	if (n[2] > 0) {
	    mesage_(" ", (ftnlen)1);
	    intrup_("LIST ALL LINES", &ians, mcom, icom, jcom, cin + 72, &iin[
		    1], &rin[1], &kin[1], (ftnlen)14, (ftnlen)72);
	    if (ians) {
		i1 = 1;
		i2 = n[19];
	    } else {
		mesage_("LIST LINES <I1> THROUGH <I2>:", (ftnlen)29);
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[19]);
		} else {
		    goto L140;
		}
	    }
	    s_wsfe(&io___12);
	    e_wsfe();
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		ltsort_(ml, &linkl[3], &i__, &k, &addlnk);
		if (k > 0) {
		    if (ltype[k] == 1) {
			s_wsfe(&io___13);
			do_fio(&c__1, (char *)&iline[k], (ftnlen)sizeof(
				integer));
			for (l = 1; l <= 2; ++l) {
			    do_fio(&c__1, (char *)&lcon[l + k * 3], (ftnlen)
				    sizeof(integer));
			}
			do_fio(&c__1, (char *)&nint[k], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&ilboun[k], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&isboun[k], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&factor[k], (ftnlen)sizeof(real)
				);
			e_wsfe();
		    } else {
			s_wsfe(&io___15);
			do_fio(&c__1, (char *)&iline[k], (ftnlen)sizeof(
				integer));
			for (l = 1; l <= 3; ++l) {
			    do_fio(&c__1, (char *)&lcon[l + k * 3], (ftnlen)
				    sizeof(integer));
			}
			do_fio(&c__1, (char *)&nint[k], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&ilboun[k], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&isboun[k], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&factor[k], (ftnlen)sizeof(real)
				);
			e_wsfe();
		    }
		}
/* L130: */
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*----------------------------------*", (ftnlen)36);
	    mesage_("* NO LINES IN THE CURRENT DATABASE *", (ftnlen)36);
	    mesage_("*----------------------------------*", (ftnlen)36);
	}
L140:
/*  LIST OUT THE SIDES */
	;
    } else if (s_cmp(cin + *icom * 72, "SI", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "si", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (n[3] > 0) {
	    mesage_(" ", (ftnlen)1);
	    intrup_("LIST ALL SIDES", &ians, mcom, icom, jcom, cin + 72, &iin[
		    1], &rin[1], &kin[1], (ftnlen)14, (ftnlen)72);
	    if (ians) {
		i1 = 1;
		i2 = n[20];
	    } else {
		mesage_("LIST SIDES <I1> THROUGH <I2>:", (ftnlen)29);
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[20]);
		} else {
		    goto L170;
		}
	    }
	    s_wsfe(&io___16);
	    e_wsfe();
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		ltsort_(ms, &links[3], &i__, &k, &addlnk);
		if (k > 0) {
		    k1 = ifline[k];
L150:
/* Computing MIN */
		    i__2 = k1 + 10, i__3 = ifline[k] + nlps[k] - 1;
		    k2 = min(i__2,i__3);
		    if (k1 == ifline[k]) {
			s_wsfe(&io___19);
			do_fio(&c__1, (char *)&iside[k], (ftnlen)sizeof(
				integer));
			i__2 = k2;
			for (l = k1; l <= i__2; ++l) {
			    do_fio(&c__1, (char *)&illist[l], (ftnlen)sizeof(
				    integer));
			}
			e_wsfe();
		    } else {
			s_wsfe(&io___20);
			i__2 = k2;
			for (l = k1; l <= i__2; ++l) {
			    do_fio(&c__1, (char *)&illist[l], (ftnlen)sizeof(
				    integer));
			}
			e_wsfe();
		    }
		    if (k2 < ifline[k] + nlps[k] - 1) {
			k1 = k2 + 1;
			goto L150;
		    }
		}
/* L160: */
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*----------------------------------*", (ftnlen)36);
	    mesage_("* NO SIDES IN THE CURRENT DATABASE *", (ftnlen)36);
	    mesage_("*----------------------------------*", (ftnlen)36);
	}
L170:
/*  SPAWN A PROCESS */
	;
    } else if (s_cmp(cin + *icom * 72, "SP", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "sp", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	spawn_(vaxvms);
/*  LIST OUT SCHEMES */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'S' || *(unsigned char *)
	    &cin[*icom * 72] == 's') {
	++(*icom);
	if (n[10] > 0) {
	    mesage_(" ", (ftnlen)1);
	    intrup_("LIST ALL SCHEMES", &ians, mcom, icom, jcom, cin + 72, &
		    iin[1], &rin[1], &kin[1], (ftnlen)16, (ftnlen)72);
	    if (ians) {
		i1 = 1;
		i2 = n[24];
	    } else {
		mesage_("LIST SCHEMES <I1> THROUGH <I2>:", (ftnlen)31);
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[24]);
		} else {
		    goto L190;
		}
	    }
	    mesage_(" ", (ftnlen)1);
	    s_wsfe(&io___21);
	    e_wsfe();
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		ltsort_(mr, &linksc[3], &i__, &k, &addlnk);
		if (k > 0) {
		    s_wsfe(&io___22);
		    do_fio(&c__1, (char *)&ischm[k], (ftnlen)sizeof(integer));
		    do_fio(&c__1, scheme + k * 72, (ftnlen)72);
		    e_wsfe();
		}
/* L180: */
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*---------------------------------------------*", (
		    ftnlen)47);
	    mesage_("* ONLY DEFAULT SCHEME IN THE CURRENT DATABASE *", (
		    ftnlen)47);
	    mesage_("*---------------------------------------------*", (
		    ftnlen)47);
	    s_wsfe(&io___23);
	    do_fio(&c__1, defsch, (ftnlen)72);
	    e_wsfe();
	    mesage_(" ", (ftnlen)1);
	}
L190:
/*  LIST OUT THE BAR SETS */
	;
    } else if (s_cmp(cin + *icom * 72, "BA", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ba", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (n[5] > 0) {
	    mesage_(" ", (ftnlen)1);
	    intrup_("LIST ALL BAR SETS", &ians, mcom, icom, jcom, cin + 72, &
		    iin[1], &rin[1], &kin[1], (ftnlen)17, (ftnlen)72);
	    if (ians) {
		i1 = 1;
		i2 = n[21];
	    } else {
		mesage_("LIST BAR SETS <I1> THROUGH <I2>:", (ftnlen)32);
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[21]);
		} else {
		    goto L220;
		}
	    }
	    mesage_(" ", (ftnlen)1);
	    s_wsfe(&io___24);
	    e_wsfe();
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		ltsort_(ms, &linkb[3], &i__, &k, &addlnk);
		if (k > 0) {
		    k1 = jfline[k];
L200:
/* Computing MIN */
		    i__2 = k1 + 10, i__3 = jfline[k] + nlpb[k] - 1;
		    k2 = min(i__2,i__3);
		    if (k1 == jfline[k]) {
			s_wsfe(&io___25);
			do_fio(&c__1, (char *)&ibarst[k], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&jmat[k], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&jcent[k], (ftnlen)sizeof(
				integer));
			i__2 = k2;
			for (l = k1; l <= i__2; ++l) {
			    do_fio(&c__1, (char *)&jllist[l], (ftnlen)sizeof(
				    integer));
			}
			e_wsfe();
		    } else {
			s_wsfe(&io___26);
			i__2 = k2;
			for (l = k1; l <= i__2; ++l) {
			    do_fio(&c__1, (char *)&jllist[l], (ftnlen)sizeof(
				    integer));
			}
			e_wsfe();
		    }
		    if (k2 < jfline[i__] + nlpb[k] - 1) {
			k1 = k2 + 1;
			goto L200;
		    }
		}
/* L210: */
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*-------------------------------------*", (ftnlen)39);
	    mesage_("* NO BAR SETS IN THE CURRENT DATABASE *", (ftnlen)39);
	    mesage_("*-------------------------------------*", (ftnlen)39);
	}
L220:
/*  LIST OUT THE RENUMBERING CARDS */
	;
    } else if (s_cmp(cin + *icom * 72, "REN", (ftnlen)3, (ftnlen)3) == 0 || 
	    s_cmp(cin + *icom * 72, "ren", (ftnlen)3, (ftnlen)3) == 0) {
	++(*icom);
	if (n[28] > 0) {
	    mesage_(" ", (ftnlen)1);
	    i1 = 1;
	    i2 = n[28];
	    mesage_(" ", (ftnlen)1);
	    s_wsfe(&io___27);
	    e_wsfe();
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		s_wsfe(&io___28);
		do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
		do_fio(&c__1, number + i__ * 80, (ftnlen)72);
		e_wsfe();
		if (s_cmp(number + (i__ * 80 + 72), "        ", (ftnlen)8, (
			ftnlen)8) != 0) {
		    s_wsfe(&io___29);
		    do_fio(&c__1, number + (i__ * 80 + 72), (ftnlen)8);
		    e_wsfe();
		}
/* L230: */
	    }
	} else if (*optim) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*------------------------------------------*", (ftnlen)
		    44);
	    mesage_("* NO RENUMBER CARDS - OPTIMIZATION ENABLED *", (ftnlen)
		    44);
	    mesage_("*------------------------------------------*", (ftnlen)
		    44);
	    mesage_(" ", (ftnlen)1);
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*-------------------------------------------*", (ftnlen)
		    45);
	    mesage_("* NO RENUMBER CARDS - OPTIMIZATION DISABLED *", (ftnlen)
		    45);
	    mesage_("*-------------------------------------------*", (ftnlen)
		    45);
	    mesage_(" ", (ftnlen)1);
	}
/*  LIST OUT THE REGIONS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'R' || *(unsigned char *)
	    &cin[*icom * 72] == 'r') {
	++(*icom);
	lgroup = TRUE_;
	i__1 = n[7];
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (irgflg[i__] <= -1) {
		lgroup = FALSE_;
		goto L250;
	    }
/* L240: */
	}
L250:
	if (! lgroup) {
	    mesage_(" ", (ftnlen)1);
	    intrup_("LIST ALL REGIONS", &ians, mcom, icom, jcom, cin + 72, &
		    iin[1], &rin[1], &kin[1], (ftnlen)16, (ftnlen)72);
	    if (ians) {
		i1 = 1;
		i2 = n[22];
	    } else {
		mesage_("LIST REGIONS <I1> THROUGH <I2>:", (ftnlen)31);
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[22]);
		} else {
		    goto L280;
		}
	    }
	    mesage_(" ", (ftnlen)1);
	    s_wsfe(&io___31);
	    e_wsfe();
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		ltsort_(mr, &linkr[3], &i__, &k, &addlnk);
		if (k > 0 && irgflg[k] <= 0) {
		    if (rsize[k] > (float)0.) {
			rrsize = rsize[k];
		    } else {
			rrsize = *defsiz;
		    }
		    ltsort_(mr, &linksc[3], &k, &ipntr, &addlnk);
		    if (n[24] >= i__ && ipntr > 0) {
			s_copy(dummy, scheme + ipntr * 72, (ftnlen)10, (
				ftnlen)10);
		    } else {
			s_copy(dummy, defsch, (ftnlen)10, (ftnlen)10);
		    }
		    k1 = ifside[k];
L260:
/* Computing MIN */
		    i__2 = k1 + 4, i__3 = ifside[k] + nspr[k] - 1;
		    k2 = min(i__2,i__3);
		    if (k1 == ifside[k]) {
			s_wsfe(&io___35);
			do_fio(&c__1, (char *)&iregn[k], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&imat[k], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&rrsize, (ftnlen)sizeof(real));
			do_fio(&c__1, dummy, (ftnlen)10);
			i__2 = k2;
			for (l = k1; l <= i__2; ++l) {
			    do_fio(&c__1, (char *)&islist[l], (ftnlen)sizeof(
				    integer));
			}
			e_wsfe();
		    } else {
			s_wsfe(&io___36);
			i__2 = k2;
			for (l = k1; l <= i__2; ++l) {
			    do_fio(&c__1, (char *)&islist[l], (ftnlen)sizeof(
				    integer));
			}
			e_wsfe();
		    }
		    if (k2 < ifside[k] + nspr[k] - 1) {
			k1 = k2 + 1;
			goto L260;
		    }
		}
/* L270: */
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*------------------------------------*", (ftnlen)38);
	    mesage_("* NO REGIONS IN THE CURRENT DATABASE *", (ftnlen)38);
	    mesage_("*------------------------------------*", (ftnlen)38);
	}
L280:
/*  LIST OUT THE GROUPS */
	;
    } else if (*(unsigned char *)&cin[*icom * 72] == 'G' || *(unsigned char *)
	    &cin[*icom * 72] == 'g') {
	++(*icom);
	lgroup = FALSE_;
	i__1 = n[7];
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (irgflg[i__] >= 1) {
		lgroup = TRUE_;
		goto L300;
	    }
/* L290: */
	}
L300:
	if (lgroup) {
	    mesage_(" ", (ftnlen)1);
	    intrup_("LIST ALL GROUPS", &ians, mcom, icom, jcom, cin + 72, &
		    iin[1], &rin[1], &kin[1], (ftnlen)15, (ftnlen)72);
	    if (ians) {
		i1 = 1;
		i2 = n[22];
	    } else {
		mesage_("LIST GROUPS <I1> THROUGH <I2>:", (ftnlen)30);
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[22]);
		} else {
		    goto L330;
		}
	    }
	    mesage_(" ", (ftnlen)1);
	    s_wsfe(&io___37);
	    e_wsfe();
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		ltsort_(mr, &linkr[3], &i__, &k, &addlnk);
		if (k > 0 && irgflg[k] >= 1) {
		    k1 = ifside[k];
L310:
/* Computing MIN */
		    i__2 = k1 + 10, i__3 = ifside[k] + nspr[k] - 1;
		    k2 = min(i__2,i__3);
		    if (k1 == ifside[k]) {
			s_wsfe(&io___38);
			do_fio(&c__1, (char *)&iregn[k], (ftnlen)sizeof(
				integer));
			i__2 = k2;
			for (l = k1; l <= i__2; ++l) {
			    do_fio(&c__1, (char *)&islist[l], (ftnlen)sizeof(
				    integer));
			}
			e_wsfe();
		    } else {
			s_wsfe(&io___39);
			i__2 = k2;
			for (l = k1; l <= i__2; ++l) {
			    do_fio(&c__1, (char *)&islist[l], (ftnlen)sizeof(
				    integer));
			}
			e_wsfe();
		    }
		    if (k2 < ifside[k] + nspr[k] - 1) {
			k1 = k2 + 1;
			goto L310;
		    }
		}
/* L320: */
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*-----------------------------------*", (ftnlen)37);
	    mesage_("* NO GROUPS IN THE CURRENT DATABASE *", (ftnlen)37);
	    mesage_("*-----------------------------------*", (ftnlen)37);
	}
L330:
/*  LIST OUT THE HOLES */
	;
    } else if (s_cmp(cin + *icom * 72, "HO", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ho", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (n[29] > 0) {
	    mesage_(" ", (ftnlen)1);
	    intrup_("LIST ALL REGIONS WITH HOLES", &ians, mcom, icom, jcom, 
		    cin + 72, &iin[1], &rin[1], &kin[1], (ftnlen)27, (ftnlen)
		    72);
	    if (ians) {
		i1 = 1;
		i2 = n[7];
	    } else {
		mesage_("LIST HOLES IN REGIONS <I1> THROUGH <I2>:", (ftnlen)
			40);
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[7]);
		} else {
		    goto L360;
		}
	    }
	    mesage_(" ", (ftnlen)1);
	    s_wsfe(&io___40);
	    e_wsfe();
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		ltsort_(mr, &linkr[3], &i__, &k, &addlnk);
		if (k > 0) {
		    k1 = ifhole[k];
L340:
/* Computing MIN */
		    i__2 = k1 + 10, i__3 = ifhole[k] + nhpr[k] - 1;
		    k2 = min(i__2,i__3);
		    if (k2 >= k1) {
			if (k1 == ifhole[k]) {
			    s_wsfe(&io___41);
			    do_fio(&c__1, (char *)&iregn[k], (ftnlen)sizeof(
				    integer));
			    i__2 = k2;
			    for (l = k1; l <= i__2; ++l) {
				do_fio(&c__1, (char *)&ihlist[l], (ftnlen)
					sizeof(integer));
			    }
			    e_wsfe();
			} else {
			    s_wsfe(&io___42);
			    i__2 = k2;
			    for (l = k1; l <= i__2; ++l) {
				do_fio(&c__1, (char *)&ihlist[l], (ftnlen)
					sizeof(integer));
			    }
			    e_wsfe();
			}
			if (k2 < ifhole[k] + nhpr[k] - 1) {
			    k1 = k2 + 1;
			    goto L340;
			}
		    }
		}
/* L350: */
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*----------------------------------*", (ftnlen)36);
	    mesage_("* NO HOLES IN THE CURRENT DATABASE *", (ftnlen)36);
	    mesage_("*----------------------------------*", (ftnlen)36);
	}
L360:
/*  LIST OUT THE REGIONS IN THE BODY */
	;
    } else if (s_cmp(cin + *icom * 72, "BOD", (ftnlen)3, (ftnlen)3) == 0 || 
	    s_cmp(cin + *icom * 72, "bod", (ftnlen)3, (ftnlen)3) == 0) {
	++(*icom);
	j1 = 1;
	s_wsfe(&io___44);
	e_wsfe();
L370:
	if (n[9] - j1 + 1 > 13) {
	    j2 = j1 + 12;
	    s_wsfe(&io___46);
	    i__1 = j2;
	    for (j = j1; j <= i__1; ++j) {
		do_fio(&c__1, (char *)&irpb[j], (ftnlen)sizeof(integer));
	    }
	    e_wsfe();
	    j1 = j2 + 1;
	    goto L370;
	}
	s_wsfe(&io___48);
	i__1 = n[9];
	for (j = j1; j <= i__1; ++j) {
	    do_fio(&c__1, (char *)&irpb[j], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    } else if (*(unsigned char *)&cin[*icom * 72] == ' ') {
	++(*icom);
	mesage_(" ", (ftnlen)1);
	return 0;
/*  LIST OUT BOUNDARY CONDITIONS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'B' || *(unsigned char *)
	    &cin[*icom * 72] == 'b') {
	++(*icom);
	mesage_(" ", (ftnlen)1);
	s_copy(choice, "POINT  ", (ftnlen)7, (ftnlen)7);
	listbf_(mp, &n[25], choice, &linkpb[3], &ipbf[1], &nppf[1], &ifpb[1], 
		&listpb[3], &iwtpbf[4], (ftnlen)7);
	s_copy(choice, "NODE   ", (ftnlen)7, (ftnlen)7);
	listbf_(ml, &n[26], choice, &linklb[3], &ilbf[1], &nlpf[1], &iflb[1], 
		&listlb[3], &iwtlbf[4], (ftnlen)7);
	s_copy(choice, "ELEMENT", (ftnlen)7, (ftnlen)7);
	listbf_(ml, &n[27], choice, &linksb[3], &isbf[1], &nspf[1], &ifsb[1], 
		&listsb[3], &iwtsbf[4], (ftnlen)7);
/*  LIST OUT THE THREE NODE QUAD GENERATION FLAG */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'T' || *(unsigned char *)
	    &cin[*icom * 72] == 't') {
	++(*icom);
	if (*three) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*--------------------------------------*", (ftnlen)40);
	    mesage_("* THREE NODE BAR GENERATION - ENABLED  *", (ftnlen)40);
	    mesage_("*--------------------------------------*", (ftnlen)40);
	    mesage_(" ", (ftnlen)1);
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*---------------------------------------*", (ftnlen)41);
	    mesage_("* THREE NODE BAR GENERATION - DISABLED  *", (ftnlen)41);
	    mesage_("*---------------------------------------*", (ftnlen)41);
	    mesage_(" ", (ftnlen)1);
	}
/*  LIST OUT THE EIGHT NODE QUAD GENERATION FLAG */
    } else if (s_cmp(cin + *icom * 72, "EI", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ei", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (*eight) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*--------------------------------------*", (ftnlen)40);
	    mesage_("* EIGHT NODE QUAD GENERATION - ENABLED *", (ftnlen)40);
	    mesage_("*--------------------------------------*", (ftnlen)40);
	    mesage_(" ", (ftnlen)1);
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*---------------------------------------*", (ftnlen)41);
	    mesage_("* EIGHT NODE QUAD GENERATION - DISABLED *", (ftnlen)41);
	    mesage_("*---------------------------------------*", (ftnlen)41);
	    mesage_(" ", (ftnlen)1);
	}
/*  LIST OUT THE NINE NODE QUAD GENERATION FLAG */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'N' || *(unsigned char *)
	    &cin[*icom * 72] == 'n') {
	++(*icom);
	if (*nine) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*-------------------------------------*", (ftnlen)39);
	    mesage_("* NINE NODE QUAD GENERATION - ENABLED *", (ftnlen)39);
	    mesage_("*-------------------------------------*", (ftnlen)39);
	    mesage_(" ", (ftnlen)1);
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*--------------------------------------*", (ftnlen)40);
	    mesage_("* NINE NODE QUAD GENERATION - DISABLED *", (ftnlen)40);
	    mesage_("*--------------------------------------*", (ftnlen)40);
	    mesage_(" ", (ftnlen)1);
	}
/*  EXIT OPTION - EXITS FASTQ */
    } else if (s_cmp(cin + *icom * 72, "EX", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ex", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	strlng_(cin + *icom * 72, &len, (ftnlen)72);
	if (len > 1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'X' || len > 
		1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'x') {
	    help_fq__(&c__4);
	} else {
	    fexit_(wrote, mcom, icom, jcom, cin + 72, &iin[1], &rin[1], &kin[
		    1], time1, batch, versn, (ftnlen)72, (ftnlen)9);
	}
	goto L100;
/*  PRINT HELP MESAGE */
    } else {
	++(*icom);
	help_fq__(&c__4);
    }
    goto L100;
} /* list_ */

#ifdef __cplusplus
	}
#endif

/* chkwt.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int chkwt_(integer *mp, integer *ml, integer *ms, integer *
	mlist, integer *mflag, integer *nnuid, integer *mxlps, integer *linkp,
	 integer *linkl, integer *links, integer *nuid, integer *iflg, 
	integer *ilen, integer *iptr, integer *nlist, integer *iflag, integer 
	*ltype, integer *lcon, integer *nlps, integer *ifline, integer *
	illist, real *coor, integer *jpoint, integer *jside, integer *iloc, 
	integer *jloc, integer *nix, integer *ilist, real *xlist, logical *
	addlnk, logical *ispnt, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 FLAG NUMBER:\002,i5,\002 HAS NOT BEEN U\
SED IN GENERATING\002,/,\002 THIS MESH - THUS NO WEIGHTING IS POSSIBLE\002)";
    static char fmt_10010[] = "(\002 THE BEGINNING POINT:\002,i5,\002 FOR FL\
AG:\002,i5,\002 IS NOT DEFINED \002,/,\002 NO BOUNDARY FLAG WEIGHTING WITH T\
HIS POINT POSSIBLE\002)";
    static char fmt_10020[] = "(\002 THE BEGINNING POINT:\002,i5,\002 IS NOT\
 IN THE FLAG\002,/,\002 NO BOUNDARY FLAG WEIGHTING POSSIBLE WITH THIS FLAG/P\
OINT \002//\002SET.\002)";
    static char fmt_10030[] = "(\002 THE WEIGHTING POINT:\002,i5,\002 FOR FL\
AG:\002,i5,\002 IS NOT DEFINED\002,/,\002 NO BOUNDARY FLAG WEIGHTING WITH TH\
IS POINT POSSIBLE\002)";
    static char fmt_10040[] = "(\002 THE WEIGHTING LINE:\002,i5,\002 FOR FLA\
G:\002,i5,\002 IS NOT DEFINED\002,/,\002 NO BOUNDARY FLAG WEIGHTING WITH THI\
S LINE POSSIBLE\002)";
    static char fmt_10050[] = "(\002 THE WEIGHTING SIDE:\002,i5,\002 FOR FLA\
G:\002,i5,\002 IS NOT DEFINED\002,/,\002 NO BOUNDARY FLAG WEIGHTING WITH THI\
S SIDE POSSIBLE\002)";
    static char fmt_10060[] = "(\002 FOR SIDE:\002,i5,\002 LINE:\002,i5,\002\
 DOES NOT EXIST\002,/,\002 NO BOUNDARY FLAG WEIGHTING WITH THIS SIDE POSSIBLE\
\002)";
    static char fmt_10070[] = "(\002 ON LINE:\002,i5,\002 IN SIDE:\002,i5\
,\002 POINT:\002,i5,\002 DOES NOT EXIST\002,/,\002 NO BOUNDARY FLAG WEIGHTIN\
G WITH THIS SIDE POSSIBLE\002)";
    static char fmt_10080[] = "(\002 ON LINE:\002,i5,\002 POINT:\002,i5,\002\
 DOES NOT EXIST\002,/,\002 NO BOUNDARY FLAG WEIGHTING WITH THIS LINE POSSIBLE\
\002)";
    static char fmt_10090[] = "(\002 SIDE:\002,i5,\002 IS NOT MONOTONICALLY \
INCREASING IN X\002,/,\002 NO BOUNDARY FLAG WEIGHTING WITH THIS SIDE POSSIBLE\
\002)";
    static char fmt_10100[] = "(\002 LINE:\002,i5,\002 DOES NOT CONNECT PROP\
ERLY IN SIDE:\002,i5,/,\002 NO BOUNDARY FLAG WEIGHTING WITH THIS SIDE POSSIB\
LE\002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, m, j1, j2, k1, k2, l1, l2, kk, kp, ip1, ip2, ip3, 
	    k2p, k1p, l1p, l2p, kpp, ipntr, ipntr1, ipntr2, ipntr3, jlpntr, 
	    jppntr, jspntr;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___4 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___7 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___9 = { 0, 6, 0, fmt_10030, 0 };
    static cilist io___11 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___13 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___23 = { 0, 6, 0, fmt_10070, 0 };
    static cilist io___24 = { 0, 6, 0, fmt_10080, 0 };
    static cilist io___25 = { 0, 6, 0, fmt_10070, 0 };
    static cilist io___26 = { 0, 6, 0, fmt_10080, 0 };
    static cilist io___27 = { 0, 6, 0, fmt_10070, 0 };
    static cilist io___28 = { 0, 6, 0, fmt_10080, 0 };
    static cilist io___29 = { 0, 6, 0, fmt_10090, 0 };
    static cilist io___37 = { 0, 6, 0, fmt_10100, 0 };
    static cilist io___41 = { 0, 6, 0, fmt_10100, 0 };
    static cilist io___43 = { 0, 6, 0, fmt_10090, 0 };


/* *********************************************************************** */
/*  SUBROUTINE CHKWT = CHECKS THE WEIGHTING COMBINATION FOR FLAGS TO */
/*                     MAKE SURE THE DEFINITION IS VALID.  IT RETURNS */
/*                     THE LOCATION OF THE BEGINNING POINT IN THE */
/*                     FLAG NODE LIST AS JLOC.  THE ARRAY ILIST AND */
/*                     XLIST ARE ALSO SET UP AS X VALUES OF THE SIDE */
/*                     AS IT MOVES FORWARD. */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     ADDWT = ADDS THE WEIGHTING FACTORS TO ANY NODES WITH */
/*             FLAGS CONTAINING WEIGHTS */
/* *********************************************************************** */
    /* Parameter adjustments */
    coor -= 3;
    linkp -= 3;
    lcon -= 4;
    --ltype;
    linkl -= 3;
    --illist;
    --ifline;
    --nlps;
    links -= 3;
    --nlist;
    --iptr;
    --ilen;
    --iflg;
    --nuid;
    --xlist;
    --ilist;

    /* Function Body */
    *err = TRUE_;
/*  CHECK TO MAKE SURE THE FLAG HAS NODES GENERATED ALONG IT */
    i__1 = *mflag;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (*iflag == iflg[i__]) {
	    *iloc = i__;
	    goto L110;
	}
/* L100: */
    }
    s_wsfe(&io___2);
    do_fio(&c__1, (char *)&(*iflag), (ftnlen)sizeof(integer));
    e_wsfe();
    return 0;
L110:
/*  CHECK TO MAKE SURE THE BEGINNING POINT IS DEFINED */
    ltsort_(mp, &linkp[3], jpoint, &jppntr, addlnk);
    if (jppntr <= 0) {
	s_wsfe(&io___4);
	do_fio(&c__1, (char *)&(*jpoint), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*iflag), (ftnlen)sizeof(integer));
	e_wsfe();
	return 0;
    }
/*  CHECK TO MAKE SURE THE NODE AT THE BEGINNING POINT IS IN THE LIST */
/*  OF NODES FOR THE FLAG  -  RETURN THIS LIST LOCATION IF IT IS THERE */
    *jloc = 0;
    j1 = iptr[*iloc];
    j2 = iptr[*iloc] + ilen[*iloc] - 1;
    i__1 = j2;
    for (i__ = j1; i__ <= i__1; ++i__) {
	if (nuid[nlist[i__]] == *jpoint) {
	    *jloc = i__;
	}
/* L120: */
    }
    if (*jloc == 0) {
	s_wsfe(&io___7);
	do_fio(&c__1, (char *)&(*jpoint), (ftnlen)sizeof(integer));
	e_wsfe();
	return 0;
    }
/*  CHECK TO MAKE SURE THE SIDE OR LINE OR POINT IS DEFINED */
    if (*ispnt) {
	ltsort_(mp, &linkp[3], jside, &ipntr, addlnk);
	if (ipntr <= 0) {
	    s_wsfe(&io___9);
	    do_fio(&c__1, (char *)&(*jside), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*iflag), (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	return 0;
    } else if (*jside < 0) {
	i__1 = abs(*jside);
	ltsort_(ml, &linkl[3], &i__1, &jlpntr, addlnk);
	if (jlpntr <= 0) {
	    s_wsfe(&io___11);
	    i__1 = abs(*jside);
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*iflag), (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
    } else {
	ltsort_(ms, &links[3], jside, &jspntr, addlnk);
	if (jspntr <= 0) {
	    s_wsfe(&io___13);
	    do_fio(&c__1, (char *)&(*jside), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*iflag), (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
    }
/*  CHECK TO MAKE SURE SIDE'S LINE DEFINITIONS ARE ALL THERE */
/*  AND ARE IN MONOTONICALLY INCREASING X ORDER */
    if (*jside > 0) {
	j1 = ifline[jspntr];
	j2 = ifline[jspntr] + nlps[jspntr] - 1;
    } else {
	j1 = 1;
	j2 = 1;
    }
    *nix = j2 - j1 + 2;
    i__1 = j2;
    for (j = j1; j <= i__1; ++j) {
	if (*jside > 0) {
	    kk = illist[j];
	    ltsort_(ml, &linkl[3], &kk, &ipntr, addlnk);
	    if (kk <= 0 || ipntr <= 0) {
		s_wsfe(&io___16);
		do_fio(&c__1, (char *)&(*jside), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		e_wsfe();
		return 0;
	    }
	} else {
	    kk = abs(*jside);
	    ipntr = jlpntr;
	}
	ilist[j - j1 + 1] = ipntr;
/*  CHECK TO MAKE SURE LINE'S POINT DEFINITIONS ARE ALL THERE */
	ip1 = lcon[ipntr * 3 + 1];
	ip2 = lcon[ipntr * 3 + 2];
	ip3 = (i__2 = lcon[ipntr * 3 + 3], abs(i__2));
	ltsort_(mp, &linkp[3], &ip1, &ipntr1, addlnk);
	ltsort_(mp, &linkp[3], &ip2, &ipntr2, addlnk);
	if (ip3 != 0) {
	    i__2 = abs(ip3);
	    ltsort_(mp, &linkp[3], &i__2, &ipntr3, addlnk);
	} else {
	    ip3 = 0;
	}
	if (ip1 <= 0 || ipntr1 <= 0) {
	    if (*jside > 0) {
		s_wsfe(&io___23);
		do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jside), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ip1, (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		s_wsfe(&io___24);
		do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ip1, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    return 0;
	} else if (ip2 <= 0 || ipntr2 <= 0) {
	    if (*jside > 0) {
		s_wsfe(&io___25);
		do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jside), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ip2, (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		s_wsfe(&io___26);
		do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ip2, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    return 0;
	} else if (ltype[ipntr] != 1 && (ip3 == 0 || ipntr3 <= 0)) {
	    if (*jside > 0) {
		s_wsfe(&io___27);
		do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jside), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ip3, (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		s_wsfe(&io___28);
		do_fio(&c__1, (char *)&kk, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&ip3, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    return 0;
	}
/* L130: */
    }
/*  GET THE XLIST VALUES SET UP FOR THIS SIDE */
    if (*nix == 2) {
	ltsort_(mp, &linkp[3], &lcon[ilist[1] * 3 + 1], &ip1, addlnk);
	ltsort_(mp, &linkp[3], &lcon[ilist[1] * 3 + 2], &ip2, addlnk);
	if (coor[(ip1 << 1) + 1] < coor[(ip2 << 1) + 1]) {
	    xlist[1] = coor[(ip1 << 1) + 1];
	    xlist[2] = coor[(ip2 << 1) + 1];
	} else if (coor[(ip1 << 1) + 1] > coor[(ip2 << 1) + 1]) {
	    xlist[1] = coor[(ip2 << 1) + 1];
	    xlist[2] = coor[(ip1 << 1) + 1];
	} else {
	    s_wsfe(&io___29);
	    do_fio(&c__1, (char *)&(*jside), (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
    } else {
/*  DEFINE VALUE OF KP,  THE BEGINNING CONNECTIVITY POINT */
	k1 = lcon[ilist[1] * 3 + 1];
	k2 = lcon[ilist[1] * 3 + 2];
	l1 = lcon[ilist[2] * 3 + 1];
	l2 = lcon[ilist[2] * 3 + 2];
	kp = 0;
	if (k1 == l1 || k1 == l2) {
	    kp = k1;
	    ltsort_(mp, &linkp[3], &k2, &k2p, addlnk);
	    xlist[1] = coor[(k2p << 1) + 1];
	} else if (k2 == l1 || k2 == l2) {
	    kp = k2;
	    ltsort_(mp, &linkp[3], &k1, &k1p, addlnk);
	    xlist[1] = coor[(k1p << 1) + 1];
	} else {
	    s_wsfe(&io___37);
	    do_fio(&c__1, (char *)&illist[j1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*jside), (ftnlen)sizeof(integer));
	    e_wsfe();
	    return 0;
	}
/*  NOWLOOP THROUGH THE REST OF THE LINES TO GET THE XLIST */
	i__1 = *nix - 1;
	for (m = 2; m <= i__1; ++m) {
	    l1 = lcon[ilist[m] * 3 + 1];
	    l2 = lcon[ilist[m] * 3 + 2];
	    if (kp == l1) {
		kp = l2;
		ltsort_(mp, &linkp[3], &l1, &l1p, addlnk);
		xlist[m] = coor[(l1p << 1) + 1];
	    } else if (kp == l2) {
		kp = l1;
		ltsort_(mp, &linkp[3], &l2, &l2p, addlnk);
		xlist[m] = coor[(l2p << 1) + 1];
	    } else {
		s_wsfe(&io___41);
		do_fio(&c__1, (char *)&illist[j1 + m - 2], (ftnlen)sizeof(
			integer));
		do_fio(&c__1, (char *)&(*jside), (ftnlen)sizeof(integer));
		e_wsfe();
		return 0;
	    }
/* L140: */
	}
	ltsort_(mp, &linkp[3], &kp, &kpp, addlnk);
	xlist[*nix] = coor[(kpp << 1) + 1];
/*  NOWCHECK TO MAKE SURE THE LINES ARE INCREASING MONOTONICALLY IN X */
	i__1 = *nix;
	for (m = 2; m <= i__1; ++m) {
	    if (xlist[m] < xlist[m - 1]) {
		s_wsfe(&io___43);
		do_fio(&c__1, (char *)&(*jside), (ftnlen)sizeof(integer));
		e_wsfe();
		return 0;
	    }
/* L150: */
	}
    }
    *err = FALSE_;
    return 0;
} /* chkwt_ */

#ifdef __cplusplus
	}
#endif

/* llist.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int llist_(integer *ms, integer *ml, integer *maxnl, integer 
	*ns, integer *nl, integer *knum, integer *listl, integer *iline, 
	integer *iside, integer *nlps, integer *ifline, integer *illist, 
	integer *lcon, integer *islist, integer *links, integer *linkl, 
	logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 IN REGION\002,i5,\002, SIDE\002,i5,\002\
 DOES NOT CONNECT TO\002,/,\002 THE PREVIOUS SECTION OF THE PERIMETER\002)";
    static char fmt_10010[] = "(\002 IN REGION\002,i5,\002, LINE\002,i5,\002\
 DOES NOT CONNECT TO\002,/,\002 THE PREVIOUS SECTION OF THE PERIMETER\002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, i1, i2, k1, k2, k3, k4, j1, j2, j3, j4, is, ks, is2, 
	    ifl, ill, new__, ifl1, ifl2, ill1, ill2, ipntr;
    static logical addlnk;
    extern /* Subroutine */ int irever_(integer *, integer *), ltsort_(
	    integer *, integer *, integer *, integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___21 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___22 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___26 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___27 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE LLIST = PRODUCE LIST OF LINES FOR REGION */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     PERIM  = GENERATES PERIMETER OF THE REGION */
/* *********************************************************************** */
/*     PRODUCE THE LIST OF  (PHYSICAL INDICES OF) LINES FOR  (PHYSICAL) */
/*     REGION KREG.  THIS LIST IS  (LISTL (I), I=1, NL). */
/*     *BACKWARDS* SIDES WILL BE REVERSED. */
/*     IT IS ASSUMED LINES ARE PROPERLY LISTED IN ORDER ON SIDE CARDS. */
/*     IF THEY ARE NOT,  PERIM WILL DIAGNOSE IT. */
/*     ERR = .TRUE. IF ERRORS WERE ENCOUNTERED. */
/* *********************************************************************** */
    /* Parameter adjustments */
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --iside;
    linkl -= 3;
    lcon -= 4;
    --iline;
    --listl;
    --islist;

    /* Function Body */
    *err = TRUE_;
    addlnk = FALSE_;
    is = islist[1];
/*  FIRST SIDE */
    if (is == 0) {
	return 0;
    } else if (is < 0) {
	ifl1 = abs(is);
	ill1 = abs(is);
	new__ = 1;
	listl[new__] = ifl1;
    } else {
	ltsort_(ms, &links[3], &is, &ipntr, &addlnk);
	i1 = ifline[ipntr];
	i2 = i1 + nlps[ipntr] - 1;
	ifl1 = illist[i1];
	ill1 = illist[i2];
	new__ = 0;
	i__1 = i2;
	for (i__ = i1; i__ <= i__1; ++i__) {
	    ++new__;
	    listl[new__] = illist[i__];
/* L100: */
	}
    }
    if (*ns <= 1) {
	*nl = new__;
	*err = FALSE_;
	return 0;
    }
/*     SECOND SIDE */
    is2 = islist[2];
    if (is2 == 0) {
	return 0;
    } else if (is2 < 0) {
	ifl2 = abs(is2);
	ill2 = abs(is2);
    } else {
	ltsort_(ms, &links[3], &is2, &ipntr, &addlnk);
	i1 = ifline[ipntr];
	i2 = i1 + nlps[ipntr] - 1;
	ifl2 = illist[i1];
	ill2 = illist[i2];
    }
/*  DECIDE WHICH END OF SIDE ONE IS THE STARTING POINT */
    ltsort_(ml, &linkl[3], &ifl2, &ipntr, &addlnk);
    k1 = lcon[ipntr * 3 + 1];
    k2 = lcon[ipntr * 3 + 2];
    ltsort_(ml, &linkl[3], &ill2, &ipntr, &addlnk);
    k3 = lcon[ipntr * 3 + 1];
    k4 = lcon[ipntr * 3 + 2];
    ltsort_(ml, &linkl[3], &ifl1, &ipntr, &addlnk);
    j1 = lcon[ipntr * 3 + 1];
    j2 = lcon[ipntr * 3 + 2];
    ltsort_(ml, &linkl[3], &ill1, &ipntr, &addlnk);
    j3 = lcon[ipntr * 3 + 1];
    j4 = lcon[ipntr * 3 + 2];
/*  FIRST SIDE IN PROPER ORDER */
    if (j3 == k1 || j3 == k2 || j3 == k3 || j3 == k4 || j4 == k1 || j4 == k2 
	    || j4 == k3 || j4 == k4) {
/*  FIRST SIDE NEEDS REVERSED */
    } else if (j1 == k1 || j1 == k2 || j1 == k3 || j1 == k4 || j2 == k1 || j2 
	    == k2 || j2 == k3 || j2 == k4) {
	irever_(&listl[1], &new__);
/*  CONNECTIVITY DOES NOT EXIST */
    } else {
	if (is2 > 0) {
	    ltsort_(ms, &links[3], &is2, &ipntr, &addlnk);
	    s_wsfe(&io___21);
	    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&iside[ipntr], (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    i__1 = abs(is2);
	    ltsort_(ml, &linkl[3], &i__1, &ipntr, &addlnk);
	    s_wsfe(&io___22);
	    do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&iline[ipntr], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	return 0;
    }
    *nl = new__;
    i__1 = *ns;
    for (ks = 2; ks <= i__1; ++ks) {
	i__ = listl[*nl];
	ltsort_(ml, &linkl[3], &i__, &ipntr, &addlnk);
	j1 = lcon[ipntr * 3 + 1];
	j2 = lcon[ipntr * 3 + 2];
	is = islist[ks];
/*     ADD NEW LINES TO LIST */
	if (is == 0) {
	    return 0;
	} else if (is < 0) {
	    ifl = abs(is);
	    ill = abs(is);
	    new__ = *nl + 1;
	    listl[new__] = abs(is);
	} else {
	    ltsort_(ms, &links[3], &is, &ipntr, &addlnk);
	    i1 = ifline[ipntr];
	    i2 = i1 + nlps[ipntr] - 1;
	    ifl = illist[i1];
	    ill = illist[i2];
	    new__ = *nl;
	    i__2 = i2;
	    for (i__ = i1; i__ <= i__2; ++i__) {
		++new__;
		listl[new__] = illist[i__];
/* L110: */
	    }
	}
/*     DETERMINE WHETHER THIS SIDE IS BACKWARDS */
	ltsort_(ml, &linkl[3], &ifl, &ipntr, &addlnk);
	k1 = lcon[ipntr * 3 + 1];
	k2 = lcon[ipntr * 3 + 2];
	ltsort_(ml, &linkl[3], &ill, &ipntr, &addlnk);
	k3 = lcon[ipntr * 3 + 1];
	k4 = lcon[ipntr * 3 + 2];
/*  THIS SIDE IS IN PROPER ORDER */
	if (j1 == k1 || j1 == k2 || j2 == k1 || j2 == k2) {
/*  THIS SIDE NEEDS REVERSING */
	} else if (j1 == k3 || j1 == k4 || j2 == k3 || j2 == k4) {
	    ltsort_(ms, &links[3], &is, &ipntr, &addlnk);
	    irever_(&listl[*nl + 1], &nlps[ipntr]);
/*  CONNECTIVITY DOES NOT EXIST */
	} else {
	    if (is > 0) {
		ltsort_(ms, &links[3], &is, &ipntr, &addlnk);
		s_wsfe(&io___26);
		do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iside[ipntr], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		i__2 = abs(is);
		ltsort_(ml, &linkl[3], &i__2, &ipntr, &addlnk);
		s_wsfe(&io___27);
		do_fio(&c__1, (char *)&(*knum), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iline[ipntr], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    return 0;
	}
	*nl = new__;
/* L120: */
    }
/*  SUCCESSFUL LINE LIST GENERATION */
    *err = FALSE_;
    return 0;
} /* llist_ */

#ifdef __cplusplus
	}
#endif

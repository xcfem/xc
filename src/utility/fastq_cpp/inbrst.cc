/* inbrst.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inbrst_(integer *ms, integer *mr, integer *n5, integer *
	n6, integer *n21, integer *n23, integer *jj, integer *imtrl, integer *
	jc, integer *iin, integer *ifound, integer *ibarst, integer *jmat, 
	integer *jcent, integer *nlpb, integer *jfline, integer *jllist, 
	integer *linkb, integer *linkm, integer *nholdm, integer *iholdm, 
	integer *nholdb, integer *iholdb, logical *merge, logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002   OLD BAR SET NO:\002,i5,\002   TO NEW \
BAR SET NO:\002,i5)";
    static char fmt_10010[] = "(\002 BAR SET:\002,i5,\002 HAS LESS THAN ONE \
LINE\002,/,\002 THIS BAR SET WILL NOT BE INPUT INTO DATABASE\002)";
    static char fmt_10020[] = "(\002 MATERIAL:\002,i5,\002 FOR BAR SET:\002,\
i5,\002 HAS BEEN DESIGNATED\002,/,\002 AS A REGION  (4 NODE ELEMENT) MATERIA\
L.\002,/,\002 ELEMENTS WITH 2 AND 4 NODES CANNOT SHARE MATERIAL ID'S\002,/\
,\002 THIS BAR SET WILL NOT BE INPUT INTO DATABASE\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, n22, iz, jhold, ipntr;
    static logical addlnk;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static integer iminus;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___9 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___10 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___11 = { 0, 6, 0, fmt_10020, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INBRST = INPUTS A BAR SET INTO THE DATABASE */
/* *********************************************************************** */
    /* Parameter adjustments */
    iholdb -= 3;
    linkb -= 3;
    --jllist;
    --jfline;
    --nlpb;
    --jcent;
    --jmat;
    --ibarst;
    iholdm -= 3;
    linkm -= 3;
    --iin;

    /* Function Body */
    iz = 0;
    *noroom = TRUE_;
    n22 = 0;
/*  ZERO OUT THE LINK ARRAY IF NEEDED */
    if (*jj > *n21) {
	*n21 = *jj;
/*  FIND THE CORRECT BAR SET NUMBER IF MERGING */
/*  SET UP POINTERS FOR MERGING DATA */
    } else if (*merge) {
	jhold = *jj;
	ltsort_(ms, &linkb[3], jj, &ipntr, &addlnk);
	if (ipntr > 0) {
	    if (jhold > *nholdb) {
		*nholdb = jhold;
	    }
	    ltsort_(ms, &iholdb[3], &jhold, &ipntr, &addlnk);
	    if (ipntr > 0) {
		*jj = ipntr;
	    } else {
		*jj = n22 + 1;
		n22 = *jj;
		s_wsfe(&io___6);
		do_fio(&c__1, (char *)&jhold, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jj), (ftnlen)sizeof(integer));
		e_wsfe();
		addlnk = TRUE_;
		ltsort_(ms, &iholdb[3], &jhold, jj, &addlnk);
	    }
	}
    }
/*  INPUT THE BAR SET DATA INTO THE DATABASE */
    ++(*n5);
    j = *n5;
    if (j > *ms) {
	return 0;
    }
    addlnk = TRUE_;
    ltsort_(ms, &linkb[3], jj, &j, &addlnk);
    ibarst[j] = *jj;
    jcent[j] = *jc;
    jfline[j] = *n6 + 1;
    i__1 = *ifound;
    for (i__ = 1; i__ <= i__1; ++i__) {
	*jj = iin[i__];
	if (*jj == 0) {
	    goto L110;
	}
	++(*n6);
	if (*n6 > *ms * 3) {
	    return 0;
	}
	jllist[*n6] = *jj;
/* L100: */
    }
L110:
    nlpb[j] = *n6 - jfline[j] + 1;
    if (nlpb[j] < 1) {
	s_wsfe(&io___9);
	do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	e_wsfe();
	ltsort_(ms, &linkb[3], jj, &iz, &addlnk);
    }
    addlnk = FALSE_;
/*  LINK UP THE MATERIAL */
/*  ZERO THE LINK ARRAY IF NEEDED */
    if (*imtrl > *n23) {
	*n23 = *imtrl;
/*  SET UP POINTERS FOR MERGING DATA */
    } else if (*merge) {
	jhold = *imtrl;
	i__1 = *ms + *mr;
	ltsort_(&i__1, &linkm[3], imtrl, &ipntr, &addlnk);
	if (ipntr != 0) {
	    if (jhold > *nholdm) {
		*nholdm = jhold;
	    }
	    addlnk = FALSE_;
	    i__1 = *ms + *mr;
	    ltsort_(&i__1, &iholdm[3], &jhold, &ipntr, &addlnk);
	    if (ipntr > 0) {
		*imtrl = ipntr;
	    } else {
		*imtrl = *n23 + 1;
		*n23 = *imtrl;
		s_wsfe(&io___10);
		do_fio(&c__1, (char *)&jhold, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imtrl), (ftnlen)sizeof(integer));
		e_wsfe();
		addlnk = TRUE_;
		i__1 = *ms + *mr;
		ltsort_(&i__1, &iholdm[3], &jhold, imtrl, &addlnk);
	    }
	}
    }
/*  ADD THE MATERIAL INTO THE DATABASE */
    *noroom = FALSE_;
    addlnk = FALSE_;
    i__1 = *ms + *mr;
    ltsort_(&i__1, &linkm[3], imtrl, &ipntr, &addlnk);
    addlnk = TRUE_;
    if (ipntr > 0) {
	mesage_(" ", (ftnlen)1);
	s_wsfe(&io___11);
	do_fio(&c__1, (char *)&(*imtrl), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ibarst[j], (ftnlen)sizeof(integer));
	e_wsfe();
	ltsort_(ms, &linkb[3], &ibarst[j], &iz, &addlnk);
	return 0;
    } else if (ipntr == 0) {
	iminus = -1;
	i__1 = *ms + *mr;
	ltsort_(&i__1, &linkm[3], imtrl, &iminus, &addlnk);
    }
    jmat[j] = *imtrl;
    return 0;
} /* inbrst_ */

#ifdef __cplusplus
	}
#endif

/* inboun.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int inboun_(integer *mdim, integer *jj, integer *ifound, 
	integer *iin, integer *n1, integer *n2, integer *n3, integer *n2old, 
	logical *merge, logical *noroom, logical *newnum, integer *nhold, 
	integer *ihold, integer *iflag, integer *inum, integer *ifirst, 
	integer *list, integer *link, integer *iwt, integer *jhold, logical *
	addold)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 A FLAG NO. OF:\002,i7,\002 IS NOT ALLOW\
ED\002,/,\002 THIS FLAG WILL NOT BE INPUT INTO DATABASE\002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, k, iz, iold, ipntr, kount, nlist1;
    static logical addlnk;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static integer nplace, ikount;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INBOUN = INPUTS AND LINKS BOUNDARY FLAG INFORMATION */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     READ = READS AND/OR MERGES QMESH CARD FILE(S) */
/*   LINKBC = LINKS BOUNDARY FLAGS TO ENTITIES */
/* *********************************************************************** */
    /* Parameter adjustments */
    iwt -= 4;
    link -= 3;
    list -= 3;
    --ifirst;
    --inum;
    --iflag;
    ihold -= 3;
    --iin;

    /* Function Body */
    *newnum = FALSE_;
    *noroom = FALSE_;
    iz = 0;
    iold = 0;
    if (*jj <= 0 || *jj > 10000) {
	s_wsfe(&io___3);
	do_fio(&c__1, (char *)&(*jj), (ftnlen)sizeof(integer));
	e_wsfe();
	return 0;
    }
    *noroom = TRUE_;
/*  GET THE CORRECT FLAG ID */
    if (*jj > *n1) {
	*n1 = *jj;
    } else if (*merge) {
	*jhold = *jj;
	addlnk = FALSE_;
	ltsort_(mdim, &link[3], jj, &ipntr, &addlnk);
	if (ipntr > 0) {
	    if (*jhold > *nhold) {
		*nhold = *jhold;
	    }
	    ltsort_(mdim, &ihold[3], jhold, &ipntr, &addlnk);
	    if (ipntr > 0) {
		*jj = ipntr;
	    } else {
		*jj = *n1 + 1;
		*n1 = *jj;
		*newnum = TRUE_;
		addlnk = TRUE_;
		ltsort_(mdim, &link[3], jj, &iz, &addlnk);
		ltsort_(mdim, &ihold[3], jhold, jj, &addlnk);
	    }
	}
    }
    if (*n2 + 1 > *mdim) {
	return 0;
    }
/*  GET THE OLD LOCATION OF THE FLAG IF IT IS THERE */
    addlnk = FALSE_;
    ltsort_(mdim, &link[3], jj, &iold, &addlnk);
/*  IF THE FLAG CURRENTLY EXISTS,  SHIFT THE FLAG DATA TO THE END */
/*  OF THE CURRENT FLAG LIST,  TO FACILITATE ADDING MORE ENTRIES */
/*  TO THE OLD FLAG */
    if (iold > 0) {
/*  SHIFT THE OLD DEFINITION TO THE END OF THE LIST */
	iflag[*n2 + 1] = iflag[iold];
	inum[*n2 + 1] = inum[iold];
	iwt[(*n2 + 1) * 3 + 1] = iwt[iold * 3 + 1];
	iwt[(*n2 + 1) * 3 + 2] = iwt[iold * 3 + 2];
	iwt[(*n2 + 1) * 3 + 3] = iwt[iold * 3 + 3];
	ifirst[*n2 + 1] = *n3 + 1;
	if (iold < *n2) {
	    kount = ifirst[iold + 1] - ifirst[iold];
	} else if (iold == *n2) {
	    kount = inum[iold];
	} else {
	    mesage_("IN INBOUN, ERROR REORDERING FLAG LIST", (ftnlen)37);
	    return 0;
	}
	nlist1 = ifirst[iold];
	nplace = *n3;
	i__1 = nlist1 + inum[iold] - 1;
	for (i__ = nlist1; i__ <= i__1; ++i__) {
	    ++nplace;
	    if (nplace > *mdim) {
		return 0;
	    }
	    list[(nplace << 1) + 1] = list[(i__ << 1) + 1];
	    list[(nplace << 1) + 2] = list[(i__ << 1) + 2];
/* L100: */
	}
/*  SLIDE ALL TRAILING FLAGS OVER TO FILL THE GAP IN THE LIST */
/*  RESORT THE POINTER ARRAY AS THE LIST FILLS AND STEP N2OLD */
/*  DOWN A NOTCH SO THESE WILL BE RELINKED IF NECESSARY */
	if (*n2old > 0) {
	    --(*n2old);
	}
	addlnk = TRUE_;
	i__1 = *n2;
	for (i__ = iold; i__ <= i__1; ++i__) {
	    iflag[i__] = iflag[i__ + 1];
	    inum[i__] = inum[i__ + 1];
	    ifirst[i__] = ifirst[i__ + 1] - kount;
	    iwt[i__ * 3 + 1] = iwt[(i__ + 1) * 3 + 1];
	    iwt[i__ * 3 + 2] = iwt[(i__ + 1) * 3 + 2];
	    iwt[i__ * 3 + 3] = iwt[(i__ + 1) * 3 + 3];
	    ltsort_(mdim, &link[3], &iflag[i__], &i__, &addlnk);
/* L110: */
	}
	*n3 = ifirst[*n2] + inum[*n2] - 1;
	i__1 = *n3;
	for (i__ = nlist1; i__ <= i__1; ++i__) {
	    ikount = i__ + kount;
	    list[(i__ << 1) + 1] = list[(ikount << 1) + 1];
	    list[(i__ << 1) + 2] = list[(ikount << 1) + 2];
/* L120: */
	}
    } else {
/*  LINK UP THE FLAG IN THE NEXT OPEN SLOT */
	addlnk = TRUE_;
	++(*n2);
	ltsort_(mdim, &link[3], jj, n2, &addlnk);
	iflag[*n2] = *jj;
	ifirst[*n2] = *n3 + 1;
	iwt[*n2 * 3 + 1] = 0;
	iwt[*n2 * 3 + 2] = 0;
	iwt[*n2 * 3 + 3] = 0;
    }
/*  READ IN THE NEW FLAG LIST */
    i__1 = *ifound;
    for (i__ = 1; i__ <= i__1; ++i__) {
	*jj = iin[i__];
	if (*jj == 0) {
	    goto L160;
	}
/*  CHECK TO MAKE SURE THIS ENTITY IS NOT ALREADY IN THE LIST */
	i__2 = *n3;
	for (k = ifirst[*n2]; k <= i__2; ++k) {
	    if (list[(k << 1) + 1] == *jj) {
		goto L140;
	    }
/* L130: */
	}
	++(*n3);
	if (*n3 > *mdim) {
	    return 0;
	}
	list[(*n3 << 1) + 1] = *jj;
	list[(*n3 << 1) + 2] = 0;
L140:
/* L150: */
	;
    }
L160:
    inum[*n2] = *n3 - ifirst[*n2] + 1;
    *noroom = FALSE_;
    return 0;
} /* inboun_ */

#ifdef __cplusplus
	}
#endif

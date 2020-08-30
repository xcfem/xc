/* linkbc.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int linkbc_(integer *mdim, integer *ms, integer *i1, integer 
	*i2, integer *nboun, integer *n1, integer *n2, integer *n3, integer *
	n20, integer *iflag, integer *iflist, integer *neps, integer *list, 
	integer *nlps, integer *ifline, integer *illist, integer *iboun, 
	integer *linkf, integer *iwt, integer *linke, integer *links, logical 
	*sideok, logical *noroom)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 SIDE NO:\002,i5,\002 IS NOT IN THE DATA\
BASE\002,/,\002 THUS NO BOUNDARY FLAGS CAN BE ENTERED ALONG THIS SIDE\002)";

    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, k, j1, j2, ii, jj, iz, n2old;
    static logical merge;
    static integer ipntr, iflag1;
    static logical addold, addlnk;
    extern /* Subroutine */ int linken_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *), inboun_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *, logical *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *);
    static logical newnum;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___13 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE LINKBC = LINKS UP ALL BOUNDARY FLAG LISTS */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     READ = READS AND/OR MERGES FASTQ FILE(S) */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     I1     = THE FIRST FLAG TO BE LINKED */
/*     I2     = THE LAST FLAG TO BE LINKED */
/*     IFLAG  = THE ARRAY OF FLAGS */
/*     IFLIST = THE FIRST ENTITY IN LIST TO BE ASSOCIATED WITH A FLAG */
/*     NEPS   = THE NUMBER OF ENTITIES IN LIST THAT GO WITH A FLAG */
/*     LIST   = THE LIST OF ENTITIES */
/*     LINK   = THE LINK TO THE FLAG LIST */
/*     IBOUN  = THE LINK FROM THE ENTITY TO THE FLAGS */
/*     MDIM   = THE DIMENSIONING PARAMETER FOR THE LIST */
/*     SIDEOK = .FALSE. IF IT IS NOT POSSIBLE TO EXPAND SIDES  (POINBC'S) */
/* *********************************************************************** */
    /* Parameter adjustments */
    linke -= 3;
    iwt -= 4;
    linkf -= 3;
    --iboun;
    list -= 3;
    --neps;
    --iflist;
    --iflag;
    links -= 3;
    --illist;
    --ifline;
    --nlps;

    /* Function Body */
    iz = 0;
    addlnk = FALSE_;
    merge = FALSE_;
    *noroom = FALSE_;
    if (*sideok) {
/*  EXPAND ALL THE SIDES (SETS) TO THEIR RESPECT LINES (ENTITIES) */
	i__1 = *i2;
	for (i__ = *i1; i__ <= i__1; ++i__) {
L100:
	    ltsort_(mdim, &linkf[3], &iflag[i__], &ii, &addlnk);
	    if (ii > 0) {
/*  THE FLAG HAS BEEN FOUND */
		iflag1 = iflag[ii];
		j1 = iflist[ii];
		j2 = j1 + neps[ii] - 1;
		i__2 = j2;
		for (j = j1; j <= i__2; ++j) {
		    jj = list[(j << 1) + 1];
		    if (jj < 0) {
/* REMOVE THE SIDE FROM THE FLAG LIST */
			--neps[ii];
			i__3 = j2 - 1;
			for (k = j; k <= i__3; ++k) {
			    list[(k << 1) + 1] = list[(k + 1 << 1) + 1];
			    list[(k << 1) + 2] = list[(k + 1 << 1) + 2];
/* L110: */
			}
/*  IF THE SIDE EXISTS,  REPLACE IT WITH THE LINES IT REPRESENTS */
			jj = -jj;
			ltsort_(ms, &links[3], &jj, &ipntr, &addlnk);
			if (jj > *n20 || ipntr <= 0) {
			    s_wsfe(&io___13);
			    do_fio(&c__1, (char *)&jj, (ftnlen)sizeof(integer)
				    );
			    e_wsfe();
			} else {
			    inboun_(mdim, &iflag1, &nlps[ipntr], &illist[
				    ifline[ipntr]], n1, n2, n3, &n2old, &
				    merge, noroom, &newnum, &iz, &linkf[3], &
				    iflag[1], &neps[1], &iflist[1], &list[3], 
				    &linkf[3], &iwt[4], &iz, &addold);
			    if (*noroom) {
				return 0;
			    }
/*  NOW,  SEE IF THERE ARE ANY SIDES IN THE NEW I'TH FLAG'S LIST */
/*  NOTE THAT THE ONE FIXED HAS NOW BEEN ROTATED TO THE END OF THE LIST. */
			    goto L100;
			}
		    }
/* L120: */
		}
	    }
/* L130: */
	}
    }
/*  ALL POSSIBLE SIDE EXPANSION HAS OCCURRED */
/*  NOW LINK UP ALL THE LINES */
    i__1 = *i2;
    for (i__ = *i1; i__ <= i__1; ++i__) {
	ltsort_(mdim, &linkf[3], &iflag[i__], &ii, &addlnk);
	if (ii > 0) {
/*  THE FLAG HAS BEEN FOUND */
	    iflag1 = iflag[ii];
	    j1 = iflist[ii];
	    j2 = j1 + neps[ii] - 1;
	    i__2 = j2;
	    for (j = j1; j <= i__2; ++j) {
		jj = list[(j << 1) + 1];
		if (jj > 0) {
		    linken_(mdim, &jj, &iflag1, &iflag[1], &iflist[1], &neps[
			    1], &list[3], &linkf[3], &linke[3], &iboun[1], &
			    addlnk);
		} else {
		    mesage_("PROBLEMS ELIMINATING SIDES IN LINKBC", (ftnlen)
			    36);
		}
/* L140: */
	    }
	} else {
	    i__2 = *nboun;
	    for (j = 1; j <= i__2; ++j) {
		if (iboun[j] == iflag[i__]) {
		    iboun[j] = 0;
		}
/* L150: */
	    }
	}
/* L160: */
    }
    return 0;
} /* linkbc_ */

#ifdef __cplusplus
	}
#endif

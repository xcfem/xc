/* linken.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int linken_(integer *mdim, integer *jj, integer *iflag1, 
	integer *iflag, integer *iflist, integer *neps, integer *list, 
	integer *linkf, integer *linke, integer *iboun, logical *addlnk)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 FLAG: \002,i5,\002  ENTITY ATTEMPTED\
:\002,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer k, l, m, m1, m2, ipntr, iflag2;
    extern /* Subroutine */ int ltsort_(integer *, 
	    integer *, integer *, integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE LINKEN = LINKS ENTITIES IN BOUNDARY FLAG LISTS */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     LINKBC = LINKS UP ALL BOUNDARY FLAG LISTS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     IFLAG  = THE ARRAY OF FLAGS */
/*     IFLIST = THE FIRST ENTITY IN LIST TO BE ASSOCIATED WITH A FLAG */
/*     NEPS   = THE NUMBER OF ENTITIES IN LIST THAT GO WITH A FLAG */
/*     LIST   = THE LIST OF ENTITIES */
/*     LINK   = THE LINK TO THE FLAG LIST */
/*     IBOUN  = THE LINK FROM THE ENTITY TO THE FLAGS */
/*     MDIM   = THE DIMENSIONING PARAMETER FOR THE LIST */
/* *********************************************************************** */
    /* Parameter adjustments */
    --iboun;
    linke -= 3;
    linkf -= 3;
    list -= 3;
    --neps;
    --iflist;
    --iflag;

    /* Function Body */
    ltsort_(mdim, &linke[3], jj, &l, addlnk);
    if (l <= 0) {
	mesage_("BOUNDARY CONDITION LINK ATTEMPTED", (ftnlen)33);
	mesage_("TO A NONEXISTENT ENTITY", (ftnlen)23);
	s_wsfe(&io___2);
	do_fio(&c__1, (char *)&(*iflag1), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*jj), (ftnlen)sizeof(integer));
	e_wsfe();
	mesage_("CHECK POINBC,  LINEBC,  OR SIDEBC (S)", (ftnlen)37);
	mesage_(" ", (ftnlen)1);
    } else if (iboun[l] <= 0) {
	iboun[l] = *iflag1;
    } else {
	k = iboun[l];
L100:
	ltsort_(mdim, &linkf[3], &k, &ipntr, addlnk);
	iflag2 = iflag[ipntr];
	if (iflag2 == *iflag1) {
	    goto L120;
	}
	m1 = iflist[ipntr];
	m2 = m1 + neps[ipntr] - 1;
	i__1 = m2;
	for (m = m1; m <= i__1; ++m) {
	    if (list[(m << 1) + 1] == *jj) {
		if (list[(m << 1) + 2] > 0) {
		    k = list[(m << 1) + 2];
		    goto L100;
		} else {
		    list[(m << 1) + 2] = *iflag1;
		    goto L120;
		}
	    }
/* L110: */
	}
	mesage_("PROBLEM LINKING BOUNDARY FLAG TABLES", (ftnlen)36);
    }
L120:
    return 0;
} /* linken_ */

#ifdef __cplusplus
	}
#endif

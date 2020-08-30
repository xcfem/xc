/* chcond.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int chcond_(integer *nper, integer *nsa, real *smang, 
	integer *index, integer *ifirst, integer *n1, integer *n2, integer *
	n3, integer *n4, logical *check)
{
    static integer n5, nsa2, naux[10];
    extern /* Subroutine */ int sortia_(integer *, integer *, integer *, 
	    integer *);

/* ********************************************************************* */
/*  SUBROUTINE CHCOND = THIS SUBROUTINE CHECKS IF THE "NSA" ANGLES */
/*                       SATISFIES THE CONDITIONS. */
/* ********************************************************************* */
/*  VARIABLES  IN: NPER .... NUMBER OF POINTS IN THE REGION */
/*                 NSA ..... NUMBER OF SOUGHT SMALLEST ANGLES */
/*                 SMANG ... ARRAY OF SMALLEST ANGLES */
/*               INDEX ... POINTERS INTO THE ANGLE ARRAY */
/*           OUT: IFIRST... POINTER TO THE FIRST VERTEX */
/*                 Mi ...... INTERVALS FOR THE PENTAGON REGION */
/*                 CHECK ... .EQ. TRUE IF IT SATISFIES THE CONDITIONS */
/*  CALL BY: PICKM5.FOR */
/* WRITTEN BY: HORACIO RECALDE                          DATE:FEB 15, 1988 */
/* ************************************************************************ */
    /* Parameter adjustments */
    --index;
    --smang;

    /* Function Body */
    nsa2 = *nsa / 2;
/* --- SORT THE INDEX ARRAY TO FIND THE 'NSA2' SMALLEST ANGLES */
    sortia_(nsa, &index[1], &nsa2, naux);
    *ifirst = naux[0];
    *n1 = naux[1] - naux[0];
    *n2 = naux[2] - naux[1];
    *n3 = naux[3] - naux[2];
    *n4 = naux[4] - naux[3];
    n5 = *nper - *n1 - *n2 - *n3 - *n4;
/* --- CHECK COMPATIBILITY EQUATIONS */
    if (*n1 + *n2 + *n3 < *n4 + n5 + 2 || *n2 + *n3 + *n4 < n5 + *n1 + 2 || *
	    n3 + *n4 + n5 < *n1 + *n2 + 2 || *n4 + n5 + *n1 < *n2 + *n3 + 2 ||
	     n5 + *n1 + *n2 < *n3 + *n4 + 2) {
	*check = FALSE_;
    } else {
	*check = TRUE_;
    }
    return 0;
} /* chcond_ */

#ifdef __cplusplus
	}
#endif

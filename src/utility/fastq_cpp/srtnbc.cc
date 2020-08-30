/* srtnbc.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int srtnbc_(integer *mxnflg, integer *npnbc, integer *nnn, 
	integer *nnflg, integer *nnlen, integer *nnptr, integer *nodes, 
	integer *lstnbc, integer *ihere, integer *nnnbc, integer *nbcnod, 
	integer *nnlist)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    static integer i__, ihold;
    static logical found, enter;
    static integer itest, istart;

/* *********************************************************************** */
/*  SUBROUTINE SRTNBC = SORTS THE LIST OF NODAL BOUNDARY FLAGS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     IHERE  = AN ATTENDANCE ARRAY TO SEE IF A NODE HAS BEEN FLAGGED */
/*     NNFLG  = THE ARRAY OF FLAG VALUES */
/*     NNLEN  = NUMBER OF NODES IN THE NODE LIST ASSOCIATED WITH EACH */
/*              FLAG */
/*     NNPTR  = POINTER TO THE FIRST NODE IN LIST FOR EACH FLAG */
/*     NODES  = THE NODE LIST */
/*     NNN    = THE NUMBER OF NODES IN THE MESH */
/*     MXNFLG = THE NUMBER OF ENTRIES IN THE BOUNDARY LIST */
/*     ENTER  = .TRUE. IF THE FOLLOWING NODES ARE TO BE CHECKED "HERE" */
/*     FOUND  = .TRUE. IF A NEW UNIQUE FLAG HAS BEEN FOUND */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nnptr;
    --nnlen;
    --nnflg;
    --lstnbc;
    --nodes;
    --ihere;

    /* Function Body */
    *nnlist = 0;
    ihold = 1;
    *nbcnod = 0;
L100:
    istart = ihold;
    ihold = *nnnbc;
    enter = FALSE_;
    found = FALSE_;
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ihere[i__] = 0;
/* L110: */
    }
    i__1 = *nnnbc;
    for (i__ = istart; i__ <= i__1; ++i__) {
	if (lstnbc[i__] < 0) {
	    if (found) {
		if (enter) {
/* Computing MIN */
		    i__2 = ihold, i__3 = i__ - 1;
		    ihold = min(i__2,i__3);
		}
		itest = (i__2 = lstnbc[i__], abs(i__2));
		if (itest == nnflg[*nbcnod]) {
		    enter = TRUE_;
		    lstnbc[i__] = 0;
		} else {
		    enter = FALSE_;
		}
	    } else {
		found = TRUE_;
		enter = TRUE_;
		++(*nbcnod);
		nnflg[*nbcnod] = (i__2 = lstnbc[i__], abs(i__2));
		nnlen[*nbcnod] = 0;
		nnptr[*nbcnod] = *nnlist + 1;
		lstnbc[i__] = 0;
	    }
	} else if (lstnbc[i__] > 0) {
	    if (enter) {
		ihere[lstnbc[i__]] = 1;
		lstnbc[i__] = 0;
	    }
	}
/* L120: */
    }
    if (found) {
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (ihere[i__] == 1) {
		++(*nnlist);
		++nnlen[*nbcnod];
		nodes[*nnlist] = i__;
	    }
/* L130: */
	}
	goto L100;
    } else {
	return 0;
    }
    return 0;
} /* srtnbc_ */

#ifdef __cplusplus
	}
#endif

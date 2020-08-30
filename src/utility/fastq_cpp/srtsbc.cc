/* srtsbc.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int srtsbc_(integer *mxsflg, integer *npsbc, integer *npelem,
	 integer *nnxk, integer *nxk, integer *nsflg, integer *nslen, integer 
	*nsptr, integer *nvlen, integer *nvptr, integer *lstsbc, integer *
	nelems, integer *nsiden, integer *nsides, integer *nnsbc, integer *
	nslist, integer *nvlist, integer *nbcsid)
{
    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j, j1, j2, iend, iflag;
    static logical found;
    static integer ibegin, istart;

/* *********************************************************************** */
/*  SUBROUTINE SRTSBC = SORTS THE LIST OF SIDE BOUNDARY CARDS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     NSFLG  = THE ARRAY OF FLAG VALUES */
/*     NSLEN  = NUMBER OF ELEMENTS IN ILIST ASSOCIATED WITH EACH FLAG */
/*     NSPTR  = POINTER TO THE FIRST ELEMENT IN LIST FOR EACH FLAG */
/*     ILIST  = THE ELEMENT LIST */
/*     KKK    = THE NUMBER OF ELEMENTS IN THE MESH */
/*     MXSFLG = THE NUMBER OF ENTRIES IN THE BOUNDARY LIST */
/*     FOUND  = .TRUE. IF A NEW UNIQUE FLAG HAS BEEN FOUND */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nvptr;
    --nvlen;
    --nsptr;
    --nslen;
    --nsflg;
    --nsides;
    --nsiden;
    --nelems;
    --lstsbc;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;

    /* Function Body */
    iflag = -1;
    *nslist = 0;
    *nbcsid = 0;
    ibegin = 1;
L100:
    found = FALSE_;
    i__1 = *nnsbc;
    for (i__ = ibegin; i__ <= i__1; i__ += 3) {
	if (lstsbc[i__] < 0) {
	    if (found) {
		if (iflag == (i__2 = lstsbc[i__], abs(i__2))) {
		    ++(*nslist);
		    nelems[*nslist] = lstsbc[i__ + 1];
		    nsides[*nslist] = lstsbc[i__ + 2];
		    ++nslen[*nbcsid];
		    lstsbc[i__] = 0;
		}
	    } else {
		found = TRUE_;
		++(*nbcsid);
		iflag = -lstsbc[i__];
		nsflg[*nbcsid] = iflag;
		nslen[*nbcsid] = 1;
		++(*nslist);
		nsptr[*nbcsid] = *nslist;
		nelems[*nslist] = lstsbc[i__ + 1];
		nsides[*nslist] = lstsbc[i__ + 2];
		lstsbc[i__] = 0;
		ibegin = i__;
	    }
	}
/* L110: */
    }
    if (found) {
	goto L100;
    } else {
/*  PUT ALL THE NODES ATTACHED TO THE ELEMENT BCC INTO THE */
/*  NSIDEN LIST */
	*nvlist = 0;
	i__1 = *nbcsid;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    istart = nsptr[i__];
	    iend = nsptr[i__] + nslen[i__] - 1;
	    nvptr[i__] = *nvlist + 1;
	    i__2 = iend;
	    for (j = istart; j <= i__2; ++j) {
		j1 = nsides[j];
		j2 = j1 + 1;
		if (j2 == 5) {
		    j2 = 1;
		}
		++(*nvlist);
		nsiden[*nvlist] = nxk[j1 + nelems[j] * nxk_dim1];
		++(*nvlist);
		nsiden[*nvlist] = nxk[j2 + nelems[j] * nxk_dim1];
/* L120: */
	    }
	    nvlen[i__] = *nvlist - nvptr[i__] + 1;
/* L130: */
	}
	return 0;
    }
    return 0;
} /* srtsbc_ */

#ifdef __cplusplus
	}
#endif

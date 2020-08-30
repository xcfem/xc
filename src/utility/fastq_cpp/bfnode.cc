/* bfnode.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int bfnode_(integer *nlist, integer *nnxk, integer *npnode, 
	integer *npelem, integer *maxkxn, integer *nnuid, integer *node, 
	integer *newnod, integer *list, integer *kxn, integer *nxk, integer *
	nuid, integer *jloc, integer *line1, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 ERROR GETTING NODES ATTACHED TO NODE\
:\002,i5)";
    static char fmt_10010[] = "(\002 ERROR - THREE NODES HAVE BEEN FOUND FOR\
 SEQUENCE FROM\002,\002 NODE:\002,i5)";
    static char fmt_10020[] = "(\002 WARNING - CLOSED LOOP FOUND AT BEGINNIN\
G POINT OF\002,\002 WEIGHTING\002,/,\002 BEGINNING POINT CORRESPONDS TO NODE:\
\002,i5,/,\002 NODE:\002,i5,\002 ON LINE:\002,i5,\002 USED AS SECOND WEIGHTI\
NG NODE\002)";
    static char fmt_10030[] = "(\002 ERROR - FOR CLOSED LOOP FOUND AT BEGINN\
ING POINT OF\002,\002 WEIGHTING, \002,/,\002POSSIBLE SECOND NODES DO NOT LIE\
 ON THE FIRST LINE:\002,i5,/,\002ATTACHED TO THE SIDEBC - DIRECTION IS THUS \
UNDETERMINABLE.\002)";

    /* System generated locals */
    integer nxk_dim1, nxk_offset, kxn_dim1, kxn_offset, i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j;
    static logical all, found;
    static integer jlist[20], linet1, linet2;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static logical closed;
    static integer nodold, ifound;
    extern /* Subroutine */ int getnxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *);
    static logical notify;

    /* Fortran I/O blocks */
    static cilist io___7 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___11 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___14 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___15 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___17 = { 0, 6, 0, fmt_10030, 0 };


/* *********************************************************************** */
/*  SUBROUTINE BFNODE = FINDS ANY NODES IN A PORTION OF THE NODAL FLAG */
/*                      LIST WHICH IS ATTACHED TO THE GIVEN NODE. */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     ADDWT = ADDS THE WEIGHTING FACTORS TO ANY NODES WITH */
/*             FLAGS CONTAINING WEIGHTS */
/* *********************************************************************** */
    /* Parameter adjustments */
    --list;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;
    kxn_dim1 = *nnxk;
    kxn_offset = 1 + kxn_dim1;
    kxn -= kxn_offset;
    --nuid;

    /* Function Body */
    *err = TRUE_;
    found = FALSE_;
    closed = FALSE_;
    notify = TRUE_;
    *newnod = 0;
    *jloc = 0;
    all = FALSE_;
    getnxn_(npnode, npelem, maxkxn, nnxk, &kxn[kxn_offset], &nxk[nxk_offset], 
	    &nuid[1], node, jlist, &ifound, &all, err);
    if (*err) {
	s_wsfe(&io___7);
	do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(integer));
	e_wsfe();
	return 0;
    }
/*  SEE IF ANY OF THE FOUND NODES ARE IN THE FLAGGED NODE LIST */
/*  ONLY 1 SHOULD BE  (REPEATS OF THE SAME NODE ARE OK FOR SIDEBC) */
    *newnod = 0;
    nodold = 0;
    i__1 = ifound;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *nlist;
	for (j = 1; j <= i__2; ++j) {
	    if (list[j] == jlist[i__ - 1]) {
		if (found && list[j] != *newnod) {
		    if (closed && nodold != list[j]) {
			s_wsfe(&io___11);
			do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(
				integer));
			e_wsfe();
			return 0;
		    } else if (! closed) {
/*  ASSUME IN A CLOSED LOOP THAT THE FIRST LINE IN THE SIDEBC DEFINITION */
/*   (LINE1) INDICATES THE APPROPRIATE DIRECTION */
			linet1 = (nuid[list[j]] - 1000000000) / 100000;
			linet2 = (nuid[*newnod] - 1000000000) / 100000;
			if (linet1 == linet2 && linet1 == *line1) {
			    closed = TRUE_;
			    if (notify) {
				s_wsfe(&io___14);
				do_fio(&c__1, (char *)&(*node), (ftnlen)
					sizeof(integer));
				do_fio(&c__1, (char *)&(*newnod), (ftnlen)
					sizeof(integer));
				do_fio(&c__1, (char *)&(*line1), (ftnlen)
					sizeof(integer));
				e_wsfe();
			    }
			    mesage_("NOTE - NEITHER 2ND NODE IS ON FIRST LINE"
				    , (ftnlen)40);
			} else if (*line1 == linet1) {
			    nodold = *newnod;
			    *newnod = list[j];
			    *jloc = j;
			    closed = TRUE_;
			    if (notify) {
				s_wsfe(&io___15);
				do_fio(&c__1, (char *)&(*node), (ftnlen)
					sizeof(integer));
				do_fio(&c__1, (char *)&(*newnod), (ftnlen)
					sizeof(integer));
				do_fio(&c__1, (char *)&(*line1), (ftnlen)
					sizeof(integer));
				e_wsfe();
			    }
			    notify = FALSE_;
			} else if (*line1 == linet2) {
			    nodold = list[j];
			    closed = TRUE_;
			    if (notify) {
				s_wsfe(&io___16);
				do_fio(&c__1, (char *)&(*node), (ftnlen)
					sizeof(integer));
				do_fio(&c__1, (char *)&(*newnod), (ftnlen)
					sizeof(integer));
				do_fio(&c__1, (char *)&(*line1), (ftnlen)
					sizeof(integer));
				e_wsfe();
			    }
			    notify = FALSE_;
			} else {
			    s_wsfe(&io___17);
			    do_fio(&c__1, (char *)&(*node), (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    return 0;
			}
		    }
		} else {
		    found = TRUE_;
		    *newnod = list[j];
		    *jloc = j;
		}
	    }
/* L100: */
	}
/* L110: */
    }
    *err = FALSE_;
    return 0;
} /* bfnode_ */

#ifdef __cplusplus
	}
#endif

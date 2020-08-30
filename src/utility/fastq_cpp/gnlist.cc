/* gnlist.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int gnlist_(integer *mxlist, integer *nnuid, integer *msc, 
	integer *npnode, integer *npelem, integer *maxkxn, integer *nnxk, 
	integer *kxn, integer *nxk, integer *nuid, real *xn, real *yn, 
	integer *list, integer *nlist, char *number, integer *kcrd, integer *
	nnn, logical *err, logical *noroom, ftnlen number_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 NODE\002,i10,\002 IS ALREADY IN THE L\
IST\002)";
    static char fmt_10010[] = "(\002 NODE\002,i10,\002 IS NOT AN IDENTIFIER \
OF A NODE IN THIS MESH\002)";

    /* System generated locals */
    integer kxn_dim1, kxn_offset, nxk_dim1, nxk_offset, i__1, i__2;
    real r__1, r__2;
    icilist ici__1;

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen), s_rsfi(icilist *), do_fio(
	    integer *, char *, ftnlen), e_rsfi();
    double sqrt(doublereal);
    integer s_wsfe(cilist *), e_wsfe();

    /* Local variables */
    static real d__;
    static integer i__, j, k, ip1, ip2, new__, line, ival;
    extern integer indx_(integer *, integer *, integer *);
    static real dist, xval, yval;
    static integer nnow, inear, lastn, nlist1, mxlst1;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    extern integer ioccur_(integer *, integer *, integer *);
    extern /* Subroutine */ int getplp_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, logical *);
    static integer nlistn, numnew;

    /* Fortran I/O blocks */
    static cilist io___9 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___13 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___14 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___23 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBRUOTINE GNLIST = GETS INITIAL NODE LIST TO BEGIN CUTHILL - MCKEE */
/*                      PROCESS */
/* *********************************************************************** */
/*  NOTE: */
/*     AS MANY CARDS OF AS MANY TYPES AS DESIRED MAY BE USED IN */
/*     ANY ORDER.  IF A NODE IS REFERENCED MORE THAN ONCE A WARNING */
/*     WILL BE PRINTED AND ONLY THE FIRST REFERENCE WILL BE USED. */
/*      (IT MAY BE NECESSARY TO MULTIPLY REFERENCE A NODE IN THE */
/*     CASE OF MORE THAN ONE P - L - P CARD.) */
/* *********************************************************************** */
/*     EXAMPLE INPUT CARDS */
/*     COL.1    5   ETC. */
/*         X-Y              3.5       4.0 */
/*         NODE               7 */
/*         NODE               7  100100002  100100003  100100004    8 */
/*         P-L-P         1    1    2 */
/*         P-L-P         1   77    3   66    5 */
/* *********************************************************************** */
/*  INITIALIZE */
    /* Parameter adjustments */
    --list;
    --nuid;
    number -= 80;
    --yn;
    --xn;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;
    kxn_dim1 = *nnxk;
    kxn_offset = 1 + kxn_dim1;
    kxn -= kxn_offset;

    /* Function Body */
    *err = FALSE_;
    *nlist = 0;
/*  NEXT DATA CARD */
    i__1 = *kcrd;
    for (k = 1; k <= i__1; ++k) {
/*  X - Y */
	if (s_cmp(number + k * 80, "X-Y", (ftnlen)3, (ftnlen)3) == 0 || s_cmp(
		number + k * 80, "x-y", (ftnlen)3, (ftnlen)3) == 0) {
	    ici__1.icierr = 0;
	    ici__1.iciend = 0;
	    ici__1.icirnum = 1;
	    ici__1.icirlen = 10;
	    ici__1.iciunit = number + (k * 80 + 10);
	    ici__1.icifmt = " (E10.0)";
	    s_rsfi(&ici__1);
	    do_fio(&c__1, (char *)&xval, (ftnlen)sizeof(real));
	    e_rsfi();
	    ici__1.icierr = 0;
	    ici__1.iciend = 0;
	    ici__1.icirnum = 1;
	    ici__1.icirlen = 10;
	    ici__1.iciunit = number + (k * 80 + 20);
	    ici__1.icifmt = " (E10.0)";
	    s_rsfi(&ici__1);
	    do_fio(&c__1, (char *)&yval, (ftnlen)sizeof(real));
	    e_rsfi();
	    inear = 1;
/* Computing 2nd power */
	    r__1 = xn[1] - xval;
/* Computing 2nd power */
	    r__2 = yn[1] - yval;
	    dist = sqrt(r__1 * r__1 + r__2 * r__2);
	    i__2 = *nnn;
	    for (i__ = 2; i__ <= i__2; ++i__) {
/* Computing 2nd power */
		r__1 = xn[i__] - xval;
/* Computing 2nd power */
		r__2 = yn[i__] - yval;
		d__ = sqrt(r__1 * r__1 + r__2 * r__2);
		if (d__ < dist) {
		    dist = d__;
		    inear = i__;
		}
/* L100: */
	    }
	    nnow = max(*nlist,1);
	    if (ioccur_(&nnow, &list[1], &inear) == 1) {
		s_wsfe(&io___9);
		do_fio(&c__1, (char *)&nuid[inear], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		++(*nlist);
		list[*nlist] = inear;
	    }
/*  NODE ID */
	} else if (s_cmp(number + k * 80, "NOD", (ftnlen)3, (ftnlen)3) == 0 ||
		 s_cmp(number + k * 80, "nod", (ftnlen)3, (ftnlen)3) == 0) {
	    for (i__ = 11; i__ <= 71; i__ += 10) {
		j = i__ + 9;
		ici__1.icierr = 0;
		ici__1.iciend = 0;
		ici__1.icirnum = 1;
		ici__1.icirlen = j - (i__ - 1);
		ici__1.iciunit = number + (k * 80 + (i__ - 1));
		ici__1.icifmt = " (I10)";
		s_rsfi(&ici__1);
		do_fio(&c__1, (char *)&ival, (ftnlen)sizeof(integer));
		e_rsfi();
		if (ival > 0) {
		    new__ = indx_(nnn, &nuid[1], &ival);
		    if (new__ == 0) {
			s_wsfe(&io___13);
			do_fio(&c__1, (char *)&ival, (ftnlen)sizeof(integer));
			e_wsfe();
			mesage_("THIS NODE WILL BE SKIPPED", (ftnlen)25);
		    } else if (ioccur_(nlist, &list[1], &new__) == 0) {
			++(*nlist);
			list[*nlist] = new__;
		    } else {
			s_wsfe(&io___14);
			do_fio(&c__1, (char *)&ival, (ftnlen)sizeof(integer));
			e_wsfe();
		    }
		}
/* L110: */
	    }
/*  P-L-P */
	} else if (s_cmp(number + k * 80, "P-L", (ftnlen)3, (ftnlen)3) == 0 ||
		 s_cmp(number + k * 80, "p-l", (ftnlen)3, (ftnlen)3) == 0) {
	    numnew = 0;
	    for (j = 6; j <= 66; j += 10) {
		ici__1.icierr = 0;
		ici__1.iciend = 0;
		ici__1.icirnum = 1;
		ici__1.icirlen = 5;
		ici__1.iciunit = number + (k * 80 + (j - 1));
		ici__1.icifmt = " (I5)";
		s_rsfi(&ici__1);
		do_fio(&c__1, (char *)&ip1, (ftnlen)sizeof(integer));
		e_rsfi();
		i__2 = j + 4;
		ici__1.icierr = 0;
		ici__1.iciend = 0;
		ici__1.icirnum = 1;
		ici__1.icirlen = j + 9 - i__2;
		ici__1.iciunit = number + (k * 80 + i__2);
		ici__1.icifmt = " (I5)";
		s_rsfi(&ici__1);
		do_fio(&c__1, (char *)&line, (ftnlen)sizeof(integer));
		e_rsfi();
		i__2 = j + 9;
		ici__1.icierr = 0;
		ici__1.iciend = 0;
		ici__1.icirnum = 1;
		ici__1.icirlen = j + 14 - i__2;
		ici__1.iciunit = number + (k * 80 + i__2);
		ici__1.icifmt = " (I5)";
		s_rsfi(&ici__1);
		do_fio(&c__1, (char *)&ip2, (ftnlen)sizeof(integer));
		e_rsfi();
		if (ip1 > 0) {
		    mxlst1 = *mxlist - *nlist;
		    getplp_(npnode, npelem, maxkxn, nnxk, &mxlst1, &kxn[
			    kxn_offset], &nxk[nxk_offset], &nuid[1], &ip1, &
			    line, &ip2, &list[*nlist + 1], &numnew, nnn, &
			    lastn, noroom, err);
		    if (*noroom) {
			mesage_("DIMENSIONS MUST BE INCREASED", (ftnlen)28);
			return 0;
		    } else if (*err) {
			return 0;
		    }
		    nlist1 = *nlist + 1;
		    nlistn = *nlist + numnew;
		    if (numnew > 0) {
			i__2 = nlistn;
			for (i__ = nlist1; i__ <= i__2; ++i__) {
			    if (*nlist == 0 || ioccur_(nlist, &list[1], &list[
				    i__]) == 0) {
				++(*nlist);
				list[*nlist] = list[i__];
			    } else {
				s_wsfe(&io___23);
				do_fio(&c__1, (char *)&list[i__], (ftnlen)
					sizeof(integer));
				e_wsfe();
			    }
/* L120: */
			}
		    }
		} else {
		    goto L140;
		}
/* L130: */
	    }
L140:
	    ;
	}
/* L150: */
    }
    return 0;
} /* gnlist_ */

#ifdef __cplusplus
	}
#endif

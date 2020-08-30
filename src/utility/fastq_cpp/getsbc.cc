/* getsbc.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int getsbc_(integer *mxnd, integer *mxnper, integer *nper, 
	integer *nl, integer *ml, integer *maxsbc, integer *maxprm, integer *
	nprm, integer *nid, integer *listl, real *xn, real *yn, integer *nuid,
	 integer *lxk, integer *kxl, integer *nxl, integer *lstsbc, integer *
	nperim, integer *ksbc, integer *lcon, integer *isboun, integer *linkl,
	 integer *nspf, integer *ifsb, integer *listsb, integer *linksb, 
	integer *lll, logical *bar, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 SIDE BOUNDARY FLAG\002,i5,\002 IS NOT P\
ROPERLY LINKED\002)";
    static char fmt_10010[] = "(\002 ERROR FINDING CORRECT BOUNDARY SIDE ON \
ELEMENT\002,i5)";

    /* System generated locals */
    integer nid_dim1, nid_offset, i__1, i__2, i__3, i__4, i__5, i__6;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    static integer i__, j, k, l, i1, i2, j1, j2, ii, jj, jm1, jp1;
    static logical ccw;
    static real area;
    static integer nsum, iflag, nflag, jside, nelem, nodes[4];
    extern /* Subroutine */ int gnxka_(integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *, logical *);
    static logical exter;
    static integer ltest;
    static logical addlnk;
    extern /* Subroutine */ int ltsort_(integer *, 
	    integer *, integer *, integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___18 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___25 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___26 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE GETSBC = GETS THE SIDE BOUNDARY LIST */
/* *********************************************************************** */
    /* Parameter adjustments */
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --yn;
    --xn;
    --listl;
    linksb -= 3;
    listsb -= 3;
    --ifsb;
    --nspf;
    linkl -= 3;
    --isboun;
    lcon -= 4;
    --lstsbc;
    --nperim;
    nid_dim1 = *mxnper;
    nid_offset = 1 + nid_dim1;
    nid -= nid_offset;

    /* Function Body */
    *err = TRUE_;
    ccw = TRUE_;
    addlnk = FALSE_;
    nperim[1] = *nper;
    i__1 = *nprm;
    for (ii = 1; ii <= i__1; ++ii) {
	i__2 = nperim[ii];
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (*bar) {
		nid[i__ + ii * nid_dim1] = (i__3 = nuid[i__], abs(i__3));
	    } else {
		if (nid[i__ + ii * nid_dim1] < 0) {
		    nid[i__ + ii * nid_dim1] = -nid[i__ + ii * nid_dim1];
		}
	    }
/* L100: */
	}
/* L110: */
    }
/*  SORT THROUGH AND PICK OFF ELEMENTS WITH SIDE BOUNDARY CONDITIONS */
    i__1 = *lll;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (*bar) {
	    i1 = lxk[(i__ << 2) + 1];
	    i2 = lxk[(i__ << 2) + 2];
	} else {
	    i1 = nxl[(i__ << 1) + 1];
	    i2 = nxl[(i__ << 1) + 2];
	}
/*  SEE IF THE LINE IS CLEARLY INTERIOR */
	if (i1 > 0 && i2 > 0) {
	    if (nuid[i1] != 0 && nuid[i2] != 0) {
		ltest = 0;
		exter = FALSE_;
/*  CHECK AGAINST THE PERIMETER LIST TO SEE IF IT IS TRULY EXTERIOR */
		i__2 = *nprm;
		for (jj = 1; jj <= i__2; ++jj) {
		    i__3 = nperim[jj];
		    for (j = 1; j <= i__3; ++j) {
			if ((i__4 = nuid[i1], abs(i__4)) == nid[j + jj * 
				nid_dim1]) {
			    if (j == 1) {
				j1 = j + 1;
				j2 = nperim[jj];
			    } else if (j == nperim[jj]) {
				j1 = j - 1;
				j2 = 1;
			    } else {
				j1 = j - 1;
				j2 = j + 1;
			    }
			    if ((i__4 = nuid[i2], abs(i__4)) == nid[j1 + jj * 
				    nid_dim1] || (i__5 = nuid[i2], abs(i__5)) 
				    == nid[j2 + jj * nid_dim1]) {
				exter = TRUE_;
			    }
			    goto L140;
			}
/* L120: */
		    }
/* L130: */
		}
L140:
		if (exter) {
/*  FIND THE LINE NUMBER IT BELONGS TO */
		    if ((i__2 = nuid[i1], abs(i__2)) > 1000000000) {
			ltest = ((i__2 = nuid[i1], abs(i__2)) - 1000000000) / 
				100000;
		    } else if ((i__2 = nuid[i2], abs(i__2)) > 1000000000) {
			ltest = ((i__2 = nuid[i2], abs(i__2)) - 1000000000) / 
				100000;
		    } else {
			nsum = (i__2 = nuid[i1], abs(i__2)) + (i__3 = nuid[i2]
				, abs(i__3));
			i__2 = *nl;
			for (j = 1; j <= i__2; ++j) {
			    ltsort_(ml, &linkl[3], &listl[j], &k, &addlnk);
			    if (lcon[k * 3 + 1] + lcon[k * 3 + 2] == nsum) {
				if (lcon[k * 3 + 1] == (i__3 = nuid[i1], abs(
					i__3)) && lcon[k * 3 + 2] == (i__4 = 
					nuid[i2], abs(i__4)) || lcon[k * 3 + 
					1] == (i__5 = nuid[i2], abs(i__5)) && 
					lcon[k * 3 + 2] == (i__6 = nuid[i1], 
					abs(i__6))) {
				    ltest = listl[j];
				    goto L160;
				}
			    }
/* L150: */
			}
L160:
			;
		    }
/*  FIND THE ELEMENT BOUNDARY FLAG IF THERE IS ONE */
		    if (ltest <= 0) {
			mesage_(" ERROR IN SEARCHING NXL FOR ELEMENT BCC", (
				ftnlen)39);
			return 0;
		    } else {
			ltsort_(ml, &linkl[3], &ltest, &j, &addlnk);
			if (isboun[j] > 0) {
			    iflag = isboun[j];
/*  CHECK TO MAKE SURE LINE IS LINKED TO FLAG */
/*  AND GET THE NEXT LINK  (NFLAG) */
			    ltsort_(ml, &linksb[3], &iflag, &l, &addlnk);
			    i__2 = ifsb[l] + nspf[l] - 1;
			    for (jj = ifsb[l]; jj <= i__2; ++jj) {
				if (listsb[(jj << 1) + 1] < 0) {
				    mesage_("PROBLEMS WITH SIDES IN FLAG LIS\
T IN GETSBC", (ftnlen)42);
				} else {
				    if (listsb[(jj << 1) + 1] == ltest) {
					nflag = listsb[(jj << 1) + 2];
					goto L180;
				    }
				}
/* L170: */
			    }
			    s_wsfe(&io___18);
			    do_fio(&c__1, (char *)&iflag, (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    return 0;
L180:
			    if (*bar) {
				nelem = i__;
			    } else {
				nelem = kxl[(i__ << 1) + 1];
				if (nelem == 0) {
				    nelem = kxl[(i__ << 1) + 2];
				}
			    }
			    ++(*ksbc);
			    lstsbc[*ksbc] = -iflag;
			    ++(*ksbc);
			    if (*ksbc > *maxsbc) {
				s_stop("maxsbc error", (ftnlen)12);
			    }
			    lstsbc[*ksbc] = nelem;
/*  GET THE CORRECT ELEMENT SIDE */
			    if (*bar) {
				jside = 1;
			    } else {
				gnxka_(mxnd, &xn[1], &yn[1], &nelem, nodes, &
					area, &lxk[5], &nxl[3], &ccw);
				for (j = 1; j <= 4; ++j) {
				    if (i1 == nodes[j - 1]) {
					jp1 = j + 1;
					jm1 = j - 1;
					if (jp1 == 5) {
					    jp1 = 1;
					}
					if (jm1 == 0) {
					    jm1 = 4;
					}
					if (i2 == nodes[jp1 - 1]) {
					    jside = j;
					    goto L200;
					} else if (i2 == nodes[jm1 - 1]) {
					    jside = jm1;
					    goto L200;
					}
				    }
/* L190: */
				}
				s_wsfe(&io___25);
				do_fio(&c__1, (char *)&nelem, (ftnlen)sizeof(
					integer));
				e_wsfe();
				return 0;
L200:
				;
			    }
			    ++(*ksbc);
			    lstsbc[*ksbc] = jside;
/*  SEE IF ANY MORE FLAGS ARE ATTACHED TO THIS SIDE */
L210:
			    if (nflag > 0) {
/*  CHECK TO MAKE SURE LINE IS LINKED TO FLAG */
/*  AND GET THE NEXT LINK  (NFLAG) */
				iflag = nflag;
				ltsort_(ml, &linksb[3], &iflag, &l, &addlnk);
				i__2 = ifsb[l] + nspf[l];
				for (jj = ifsb[l]; jj <= i__2; ++jj) {
				    if (listsb[(jj << 1) + 1] == ltest) {
					nflag = listsb[(jj << 1) + 2];
					goto L230;
				    }
/* L220: */
				}
				s_wsfe(&io___26);
				do_fio(&c__1, (char *)&iflag, (ftnlen)sizeof(
					integer));
				e_wsfe();
				return 0;
L230:
				++(*ksbc);
				lstsbc[*ksbc] = -iflag;
				++(*ksbc);
				lstsbc[*ksbc] = nelem;
				++(*ksbc);
				lstsbc[*ksbc] = jside;
				goto L210;
			    }
			}
		    }
		}
	    }
	}
/* L240: */
    }
    *err = FALSE_;
    return 0;
} /* getsbc_ */

#ifdef __cplusplus
	}
#endif

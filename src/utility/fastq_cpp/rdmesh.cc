/* rdmesh.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int rdmesh_(integer *npnode, integer *npelem, integer *npnbc,
	 integer *npsbc, integer *npregn, integer *ms, integer *mr, integer *
	nnuid, integer *nnxk, integer *iunit, integer *nnn, integer *kkk, 
	integer *ipart, integer *lstnbc, integer *lstsbc, integer *nuid, real 
	*xn, real *yn, integer *nxk, integer *mat, integer *matmap, integer *
	nummat, integer *iside, integer *nlps, integer *ifline, integer *
	illist, integer *links, integer *linkr, integer *imat, integer *linkb,
	 integer *jmat, integer *nnnbc, integer *nnsbc, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 THE ALTERNATING MATERIAL NUMBERS FOR RE\
GION(S) WITH\002,/,\002 NEGATIVE MATERIAL NUMBER:\002,i5,\002 DO NOT CORRESP\
OND TO A\002,/,\002 VALID SIDE NUMBER:\002,i5,\002 WITH AT LEAST TWO LINES\
.\002)";

    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2, i__3, i__4;
    alist al__1;

    /* Builtin functions */
    integer f_rew(alist *), s_rsue(cilist *), do_uio(integer *, char *, 
	    ftnlen), e_rsue(), s_wsfe(cilist *), do_fio(integer *, char *, 
	    ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, k, l, k1, m1, n1, ir;
    static logical bar;
    static integer kreg, kmat, ipntr, jpntr, nnnbc1, nnsbc1;
    static logical addlnk;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static integer nnbcrg, kkkreg, nsbcrg, nnnreg, matpnt;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 0, 1, 0, 0 };
    static cilist io___13 = { 0, 0, 1, 0, 0 };
    static cilist io___15 = { 0, 0, 1, 0, 0 };
    static cilist io___22 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___24 = { 0, 0, 0, 0, 0 };
    static cilist io___26 = { 0, 0, 0, 0, 0 };


/* *********************************************************************** */
/*  SUBROUTINE RDMESH = THE CURRENT MESH STORED ON DISK */
/* *********************************************************************** */
    /* Parameter adjustments */
    --yn;
    --xn;
    --mat;
    --lstnbc;
    --lstsbc;
    matmap -= 4;
    ipart -= 4;
    --jmat;
    linkb -= 3;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --iside;
    --imat;
    linkr -= 3;
    --nuid;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;

    /* Function Body */
    *err = FALSE_;
/*  READ THE MESH TAPE */
    al__1.aerr = 0;
    al__1.aunit = *iunit;
    f_rew(&al__1);
    *nnn = 0;
    *kkk = 0;
    *nnnbc = 0;
    *nnsbc = 0;
    addlnk = FALSE_;
    i__1 = *npregn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ipart[i__ * 3 + 1] = 0;
	ipart[i__ * 3 + 2] = 0;
	ipart[i__ * 3 + 3] = 0;
/* L100: */
    }
    *nummat = 0;
    i__1 = *npregn;
    for (ir = 1; ir <= i__1; ++ir) {
	io___4.ciunit = *iunit;
	i__2 = s_rsue(&io___4);
	if (i__2 != 0) {
	    goto L230;
	}
	i__2 = do_uio(&c__1, (char *)&kkkreg, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L230;
	}
	i__2 = do_uio(&c__1, (char *)&nnnreg, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L230;
	}
	i__2 = do_uio(&c__1, (char *)&nnbcrg, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L230;
	}
	i__2 = do_uio(&c__1, (char *)&nsbcrg, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L230;
	}
	i__2 = do_uio(&c__1, (char *)&kreg, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L230;
	}
	i__2 = do_uio(&c__1, (char *)&bar, (ftnlen)sizeof(logical));
	if (i__2 != 0) {
	    goto L230;
	}
	i__2 = do_uio(&c__1, (char *)&m1, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L230;
	}
	i__2 = e_rsue();
	if (i__2 != 0) {
	    goto L230;
	}
/*  READ THE NODES */
	n1 = *nnn + 1;
	*nnn += nnnreg;
	if (nnnreg >= 1) {
	    io___13.ciunit = *iunit;
	    i__2 = s_rsue(&io___13);
	    if (i__2 != 0) {
		goto L230;
	    }
	    i__3 = *nnn;
	    for (i__ = n1; i__ <= i__3; ++i__) {
		i__2 = do_uio(&c__1, (char *)&nuid[i__], (ftnlen)sizeof(
			integer));
		if (i__2 != 0) {
		    goto L230;
		}
		i__2 = do_uio(&c__1, (char *)&xn[i__], (ftnlen)sizeof(real));
		if (i__2 != 0) {
		    goto L230;
		}
		i__2 = do_uio(&c__1, (char *)&yn[i__], (ftnlen)sizeof(real));
		if (i__2 != 0) {
		    goto L230;
		}
	    }
	    i__2 = e_rsue();
	    if (i__2 != 0) {
		goto L230;
	    }
	}
/*  READ THE ELEMENTS */
	k1 = *kkk + 1;
	ipart[ir * 3 + 1] = kreg;
	ipart[ir * 3 + 2] = k1;
	*kkk += kkkreg;
	ipart[ir * 3 + 3] = *kkk;
	io___15.ciunit = *iunit;
	i__2 = s_rsue(&io___15);
	if (i__2 != 0) {
	    goto L230;
	}
	i__3 = *kkk;
	for (k = k1; k <= i__3; ++k) {
	    for (i__ = 1; i__ <= 4; ++i__) {
		i__2 = do_uio(&c__1, (char *)&nxk[i__ + k * nxk_dim1], (
			ftnlen)sizeof(integer));
		if (i__2 != 0) {
		    goto L230;
		}
	    }
	}
	i__2 = e_rsue();
	if (i__2 != 0) {
	    goto L230;
	}
/*  ZERO THE MIDSIDE NODE LOCATIONS IN THE NXK ARRAY */
	i__2 = *nnxk;
	for (i__ = 5; i__ <= i__2; ++i__) {
	    i__3 = *kkk;
	    for (k = k1; k <= i__3; ++k) {
		nxk[i__ + k * nxk_dim1] = 0;
/* L110: */
	    }
/* L120: */
	}
/*  SET UP THE MATERIAL ARRAY AND MAXIMUM NUMBER OF MATERIALS */
	if (bar) {
	    ltsort_(ms, &linkb[3], &kreg, &ipntr, &addlnk);
	    kmat = (i__2 = jmat[ipntr], abs(i__2));
	} else {
	    ltsort_(mr, &linkr[3], &kreg, &ipntr, &addlnk);
	    kmat = imat[ipntr];
	}
/*  SEE IF ALTERNATING MATERIALS WITHIN A REGION ARE ENABLED */
	if (kmat < 0) {
	    i__2 = abs(kmat);
	    ltsort_(ms, &links[3], &i__2, &jpntr, &addlnk);
	    if (jpntr > 0 && nlps[jpntr] >= 2) {
/*  ADD MATERIAL NUMBER BY ROW OF ELEMENTS */
		matpnt = ifline[jpntr];
		i__2 = *kkk - m1 + 1;
		i__3 = m1;
		for (k = k1; i__3 < 0 ? k >= i__2 : k <= i__2; k += i__3) {
		    i__4 = k + m1 - 1;
		    for (l = k; l <= i__4; ++l) {
			mat[l] = illist[matpnt];
/* L130: */
		    }
/*  UPDATE THE POINTER TO THE NEXT MATERIAL */
		    ++matpnt;
		    if (matpnt > ifline[jpntr] + nlps[jpntr] - 1) {
			matpnt = ifline[jpntr];
		    }
/* L140: */
		}
	    } else {
		s_wsfe(&io___22);
		i__3 = abs(kmat);
		do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iside[jpntr], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
/*  PUT THE NEW MATERIALS INTO THE MATERIAL ARRAYS */
	    i__3 = ifline[jpntr] + nlps[jpntr] - 1;
	    for (matpnt = ifline[jpntr]; matpnt <= i__3; ++matpnt) {
		i__2 = *nummat;
		for (k = 1; k <= i__2; ++k) {
		    if (matmap[k * 3 + 1] == illist[matpnt]) {
			goto L160;
		    }
/* L150: */
		}
		++(*nummat);
		matmap[*nummat * 3 + 1] = illist[matpnt];
L160:
/* L170: */
		;
	    }
	} else {
/*  JUST INPUT THE ONE MATERIAL */
	    i__3 = *kkk;
	    for (k = k1; k <= i__3; ++k) {
		mat[k] = kmat;
/* L180: */
	    }
	    i__3 = *nummat;
	    for (k = 1; k <= i__3; ++k) {
		if (matmap[k * 3 + 1] == kmat) {
		    goto L200;
		}
/* L190: */
	    }
	    ++(*nummat);
	    matmap[*nummat * 3 + 1] = kmat;
	}
L200:
/*  READ THE NODAL BOUNDARY CONDITIONS */
	nnnbc1 = *nnnbc + 1;
	*nnnbc += nnbcrg;
	if (nnbcrg >= 1) {
	    io___24.ciunit = *iunit;
	    s_rsue(&io___24);
	    i__3 = *nnnbc;
	    for (i__ = nnnbc1; i__ <= i__3; ++i__) {
		do_uio(&c__1, (char *)&lstnbc[i__], (ftnlen)sizeof(integer));
	    }
	    e_rsue();
	}
/*  READ THE SIDE BOUNDARY CONDITIONS */
	nnsbc1 = *nnsbc + 1;
	*nnsbc += nsbcrg;
	if (nsbcrg >= 1) {
	    io___26.ciunit = *iunit;
	    s_rsue(&io___26);
	    i__3 = *nnsbc;
	    for (i__ = nnsbc1; i__ <= i__3; ++i__) {
		do_uio(&c__1, (char *)&lstsbc[i__], (ftnlen)sizeof(integer));
	    }
	    e_rsue();
	    i__3 = *nnsbc;
	    for (i__ = nnsbc1 + 1; i__ <= i__3; i__ += 3) {
		lstsbc[i__] = lstsbc[i__] + k1 - 1;
/* L210: */
	    }
	}
/* L220: */
    }
    return 0;
L230:
    mesage_("PREMATURE END OF FILE ON MESH READ", (ftnlen)34);
    mesage_("CHECK MESH PROCESSING OUTPUT TO DETERMINE", (ftnlen)41);
    mesage_("INCOMPLETE MESH DESCRIPTION", (ftnlen)27);
    *err = TRUE_;
    *nnn = 0;
    *kkk = 0;
    return 0;
} /* rdmesh_ */

#ifdef __cplusplus
	}
#endif

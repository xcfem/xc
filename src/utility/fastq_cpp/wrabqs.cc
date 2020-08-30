/* wrabqs.f -- translated by f2c (version 20160102).
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
static integer c__2 = 2;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int wrabqs_(integer *ms, integer *mr, integer *npnode, 
	integer *npelem, integer *mxnflg, integer *mxsflg, integer *npregn, 
	integer *npnbc, integer *npsbc, integer *iunit, integer *nnn, integer 
	*kkk, integer *nnxk, integer *nodes, integer *nelems, integer *nnflg, 
	integer *nnptr, integer *nnlen, integer *nsflg, integer *nsptr, 
	integer *nslen, integer *nvptr, integer *nvlen, integer *nsiden, 
	integer *mapdxg, real *xn, real *yn, integer *nxk, integer *mat, 
	integer *mapgxd, integer *matmap, integer *nbcnod, integer *nnlist, 
	integer *nbcsid, integer *nslist, integer *nvlist, integer *nummat, 
	integer *linkm, char *title, logical *err, logical *eight, logical *
	nine, ftnlen title_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002*HEADING\002,/,a72)";
    static char fmt_10010[] = "(\002**\002,/,\002**     MESH GENERATED USING\
 FASTQ        \002,/,\002**  NUMBER OF NODES:                     \002,i5,/\
,\002**  NUMBER OF ELEMENTS:                  \002,i5,/,\002**  NUMBER OF NO\
DAL BOUNDARY CONDITIONS: \002,i5,/,\002**  NUMBER OF SIDE BOUNDARY CONDITION\
S:  \002,i5,/,\002**\002)";
    static char fmt_10020[] = "(\002*NODE\002)";
    static char fmt_10030[] = "(i10,\002,\002,2(e14.7,\002,\002),e14.7)";
    static char fmt_10060[] = "(\002*ELEMENT, TYPE = C1D2, ELSET = MAT\002,a)"
	    ;
    static char fmt_10040[] = "(\002 FOR ELEMENT BLOCK ID:\002,i7)";
    static char fmt_10050[] = "(\002*ELEMENT, TYPE = \002,a,\002, ELSET = \
MAT\002,a)";
    static char fmt_10070[] = "(\002*ELEMENT, TYPE = C1D3, ELSET = MAT\002,a)"
	    ;
    static char fmt_10080[] = "(\002*ELEMENT, TYPE = CPE8, ELSET = MAT\002,a)"
	    ;
    static char fmt_10090[] = "(\002*ELEMENT, TYPE = INTER9, ELSET = MAT\002\
,a)";
    static char fmt_10100[] = "(\002*ELEMENT, TYPE = CPE4, ELSET = MAT\002,a)"
	    ;
    static char fmt_10110[] = "(8(i8,\002,\002),i8)";
    static char fmt_10120[] = "(4(i10,\002,\002),i10)";
    static char fmt_10130[] = "(\002*NSET, NSET = NB\002,a)";
    static char fmt_10150[] = "(\002*ELSET, ELSET = EB\002,a)";

    /* System generated locals */
    address a__1[2];
    integer nxk_dim1, nxk_offset, i__1, i__2, i__3, i__4, i__5[2];
    char ch__1[74];

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    static integer i__, j, k;
    static real z__;
    static integer j1, j2, ii, jj1, jj2, len, len2, kelem, inode, ihold[9], 
	    jhold;
    static logical found;
    static char dummy[72], dummy2[72];
    extern /* Subroutine */ int getdum_(integer *, 
	    char *, integer *, ftnlen);
    static logical deftyp;
    extern /* Subroutine */ int strlng_(char *, integer *, ftnlen), inqstr_(
	    char *, char *, ftnlen, ftnlen), inqtru_(char *, logical *, 
	    ftnlen);

    /* Fortran I/O blocks */
    static cilist io___1 = { 1, 0, 0, fmt_10000, 0 };
    static cilist io___2 = { 1, 0, 0, fmt_10010, 0 };
    static cilist io___3 = { 1, 0, 0, fmt_10020, 0 };
    static cilist io___6 = { 1, 0, 0, fmt_10030, 0 };
    static cilist io___10 = { 1, 0, 0, fmt_10060, 0 };
    static cilist io___11 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___14 = { 1, 0, 0, fmt_10050, 0 };
    static cilist io___16 = { 1, 0, 0, fmt_10070, 0 };
    static cilist io___17 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___18 = { 1, 0, 0, fmt_10050, 0 };
    static cilist io___19 = { 1, 0, 0, fmt_10080, 0 };
    static cilist io___20 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___21 = { 1, 0, 0, fmt_10050, 0 };
    static cilist io___22 = { 1, 0, 0, fmt_10090, 0 };
    static cilist io___23 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___24 = { 1, 0, 0, fmt_10050, 0 };
    static cilist io___25 = { 1, 0, 0, fmt_10100, 0 };
    static cilist io___26 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___27 = { 1, 0, 0, fmt_10050, 0 };
    static cilist io___30 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___31 = { 1, 0, 0, fmt_10120, 0 };
    static cilist io___35 = { 1, 0, 0, fmt_10130, 0 };
    static cilist io___36 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___41 = { 1, 0, 0, fmt_10150, 0 };
    static cilist io___43 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___45 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___46 = { 1, 0, 0, fmt_10150, 0 };
    static cilist io___47 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___48 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___49 = { 1, 0, 0, fmt_10150, 0 };
    static cilist io___50 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___51 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___52 = { 1, 0, 0, fmt_10150, 0 };
    static cilist io___53 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___54 = { 1, 0, 0, fmt_10110, 0 };


/* *********************************************************************** */
/*  SUBROUTINE WRABQS = WRITES ABAQUS DATABASE MESH OUTPUT FILE */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkm -= 3;
    --mapgxd;
    --yn;
    --xn;
    --mapdxg;
    --mat;
    --nnlen;
    --nnptr;
    --nnflg;
    --nvlen;
    --nvptr;
    --nslen;
    --nsptr;
    --nsflg;
    matmap -= 4;
    --nodes;
    --nsiden;
    --nelems;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;

    /* Function Body */
    *err = TRUE_;
/*  WRITE OUT HEADER TITLE AND INFORMATION */
    io___1.ciunit = *iunit;
    i__1 = s_wsfe(&io___1);
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = do_fio(&c__1, title, (ftnlen)72);
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L200;
    }
    io___2.ciunit = *iunit;
    i__1 = s_wsfe(&io___2);
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = do_fio(&c__1, (char *)&(*nnn), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = do_fio(&c__1, (char *)&(*kkk), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = do_fio(&c__1, (char *)&(*nbcnod), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = do_fio(&c__1, (char *)&(*nbcsid), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L200;
    }
/*  WRITE OUT NODE BLOCK */
    io___3.ciunit = *iunit;
    i__1 = s_wsfe(&io___3);
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L200;
    }
    z__ = (float)0.;
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___6.ciunit = *iunit;
	i__2 = s_wsfe(&io___6);
	if (i__2 != 0) {
	    goto L200;
	}
	i__2 = do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L200;
	}
	i__2 = do_fio(&c__1, (char *)&xn[i__], (ftnlen)sizeof(real));
	if (i__2 != 0) {
	    goto L200;
	}
	i__2 = do_fio(&c__1, (char *)&yn[i__], (ftnlen)sizeof(real));
	if (i__2 != 0) {
	    goto L200;
	}
	i__2 = do_fio(&c__1, (char *)&z__, (ftnlen)sizeof(real));
	if (i__2 != 0) {
	    goto L200;
	}
	i__2 = e_wsfe();
	if (i__2 != 0) {
	    goto L200;
	}
/* L100: */
    }
/*  QUERY THE USER FOR LOCAL CONTROL OF ELEMENT TYPE */
    inqtru_("USE DEFAULT ELEMENT TYPES FOR ELSETS", &deftyp, (ftnlen)36);
/*  WRITE OUT ELEMENT BLOCKS */
    i__1 = *nummat;
    for (i__ = 1; i__ <= i__1; ++i__) {
	getdum_(&matmap[i__ * 3 + 1], dummy, &len, (ftnlen)72);
	if (nxk[matmap[i__ * 3 + 2] * nxk_dim1 + 3] == 0) {
	    if (deftyp) {
		io___10.ciunit = *iunit;
		i__2 = s_wsfe(&io___10);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy, len);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    } else {
		s_wsfe(&io___11);
		do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)sizeof(
			integer));
		e_wsfe();
		inqstr_("NEW 2 NODE ELEMENT TYPE:  ", dummy2, (ftnlen)26, (
			ftnlen)72);
		strlng_(dummy2, &len2, (ftnlen)72);
		io___14.ciunit = *iunit;
		i__2 = s_wsfe(&io___14);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy2, len2);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy, len);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    }
	    inode = 2;
	} else if (nxk[matmap[i__ * 3 + 2] * nxk_dim1 + 4] == 0) {
	    if (deftyp) {
		io___16.ciunit = *iunit;
		i__2 = s_wsfe(&io___16);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy, len);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    } else {
		s_wsfe(&io___17);
		do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)sizeof(
			integer));
		e_wsfe();
		inqstr_("NEW 3 NODE ELEMENT TYPE:  ", dummy2, (ftnlen)26, (
			ftnlen)72);
		strlng_(dummy2, &len2, (ftnlen)72);
		io___18.ciunit = *iunit;
		i__2 = s_wsfe(&io___18);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy2, len2);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy, len);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    }
	    inode = 3;
	} else if (*eight) {
	    if (deftyp) {
		io___19.ciunit = *iunit;
		i__2 = s_wsfe(&io___19);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy, len);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    } else {
		s_wsfe(&io___20);
		do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)sizeof(
			integer));
		e_wsfe();
		inqstr_("NEW 8 NODE ELEMENT TYPE:  ", dummy2, (ftnlen)26, (
			ftnlen)72);
		strlng_(dummy2, &len2, (ftnlen)72);
		io___21.ciunit = *iunit;
		i__2 = s_wsfe(&io___21);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy2, len2);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy, len);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    }
	    inode = 8;
	} else if (*nine) {
	    if (deftyp) {
		io___22.ciunit = *iunit;
		i__2 = s_wsfe(&io___22);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy, len);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    } else {
		s_wsfe(&io___23);
		do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)sizeof(
			integer));
		e_wsfe();
		inqstr_("NEW 9 NODE ELEMENT TYPE:  ", dummy2, (ftnlen)26, (
			ftnlen)72);
		strlng_(dummy2, &len2, (ftnlen)72);
		io___24.ciunit = *iunit;
		i__2 = s_wsfe(&io___24);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy2, len2);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy, len);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    }
	    inode = 9;
	} else {
	    if (deftyp) {
		io___25.ciunit = *iunit;
		i__2 = s_wsfe(&io___25);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy, len);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    } else {
		s_wsfe(&io___26);
		do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)sizeof(
			integer));
		e_wsfe();
		inqstr_("NEW 4 NODE ELEMENT TYPE:  ", dummy2, (ftnlen)26, (
			ftnlen)72);
		strlng_(dummy2, &len2, (ftnlen)72);
		io___27.ciunit = *iunit;
		i__2 = s_wsfe(&io___27);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy2, len2);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = do_fio(&c__1, dummy, len);
		if (i__2 != 0) {
		    goto L200;
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    }
	    inode = 4;
	}
	if (inode == 8) {
	    i__2 = matmap[i__ * 3 + 3];
	    for (kelem = matmap[i__ * 3 + 2]; kelem <= i__2; ++kelem) {
		k = mapgxd[kelem];
		io___30.ciunit = *iunit;
		i__3 = s_wsfe(&io___30);
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 1], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 3], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 5], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 7], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 2], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 4], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 6], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 8], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = e_wsfe();
		if (i__3 != 0) {
		    goto L200;
		}
/* L110: */
	    }
	} else {
	    i__2 = matmap[i__ * 3 + 3];
	    for (kelem = matmap[i__ * 3 + 2]; kelem <= i__2; ++kelem) {
		k = mapgxd[kelem];
		io___31.ciunit = *iunit;
		i__3 = s_wsfe(&io___31);
		if (i__3 != 0) {
		    goto L200;
		}
		i__3 = do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L200;
		}
		i__4 = inode;
		for (j = 1; j <= i__4; ++j) {
		    i__3 = do_fio(&c__1, (char *)&nxk[j + kelem * nxk_dim1], (
			    ftnlen)sizeof(integer));
		    if (i__3 != 0) {
			goto L200;
		    }
		}
		i__3 = e_wsfe();
		if (i__3 != 0) {
		    goto L200;
		}
/* L120: */
	    }
	}
/* L130: */
    }
/*  WRITE OUT THE NODAL BOUNDARY CONDITIONS */
    if (*nbcnod > 0) {
	i__1 = *nbcnod;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j1 = nnptr[i__];
	    j2 = nnptr[i__] + nnlen[i__] - 1;
	    getdum_(&nnflg[i__], dummy, &len, (ftnlen)72);
	    io___35.ciunit = *iunit;
	    i__2 = s_wsfe(&io___35);
	    if (i__2 != 0) {
		goto L200;
	    }
	    i__2 = do_fio(&c__1, dummy, len);
	    if (i__2 != 0) {
		goto L200;
	    }
	    i__2 = e_wsfe();
	    if (i__2 != 0) {
		goto L200;
	    }
	    io___36.ciunit = *iunit;
	    i__2 = s_wsfe(&io___36);
	    if (i__2 != 0) {
		goto L200;
	    }
	    i__3 = j2;
	    for (j = j1; j <= i__3; ++j) {
		i__2 = do_fio(&c__1, (char *)&nodes[j], (ftnlen)sizeof(
			integer));
		if (i__2 != 0) {
		    goto L200;
		}
	    }
	    i__2 = e_wsfe();
	    if (i__2 != 0) {
		goto L200;
	    }
/* L140: */
	}
    }
/*  WRITE OUT THE SIDE BOUNDARY FLAGS */
    if (*nbcsid > 0) {
/*         CALL MESAGE ('ELEMENT NUMBERING IS WRITTEN WITH ELEMENT' // */
/*     &      BOUNDARY FLAGS') */
/*         CALL INQTRU ('WOULD YOU LIKE TO CHANGE THIS TO NODES', IANS) */
/*         IF (IANS) THEN */
/*            DO 190 I = 1, NBCSID */
/*               J1 = NVPTR (I) */
/*               J2 = NVPTR (I) + NVLEN (I)-1 */
/*               CALL GETDUM (NSFLG (I), DUMMY, LEN) */
/*               WRITE (IUNIT, 180, ERR = 200) DUMMY (1:LEN) */
/*               WRITE (IUNIT, 160, ERR = 200) (NSIDEN (J), J = J1, J2) */
/* 190        CONTINUE */
/*         ELSE */
	i__1 = *nbcsid;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j1 = nsptr[i__];
	    j2 = nsptr[i__] + nslen[i__] - 1;
	    getdum_(&nsflg[i__], dummy, &len, (ftnlen)72);
/*  WRITE OUT THE SIDE 1 ELEMENTS */
	    found = FALSE_;
	    jhold = 0;
	    i__2 = j2;
	    for (j = j1; j <= i__2; ++j) {
		jj1 = nsiden[(j << 1) - 1];
		jj2 = nsiden[j * 2];
		k = nelems[j];
		if (nxk[k * nxk_dim1 + 3] == 0) {
		    inode = 2;
		} else if (nxk[k * nxk_dim1 + 4] == 0) {
		    inode = 3;
		} else if (*eight || *nine) {
		    inode = 8;
		} else {
		    inode = 4;
		}
		if (inode == 4 && (jj1 == nxk[k * nxk_dim1 + 1] && jj2 == nxk[
			k * nxk_dim1 + 2] || jj2 == nxk[k * nxk_dim1 + 1] && 
			jj1 == nxk[k * nxk_dim1 + 2]) || inode == 8 && (jj1 ==
			 nxk[k * nxk_dim1 + 1] && jj2 == nxk[k * nxk_dim1 + 2]
			 || jj2 == nxk[k * nxk_dim1 + 1] && jj1 == nxk[k * 
			nxk_dim1 + 2])) {
/*     &            ( (JJ1 .EQ. NXK (2, K)) .AND. */
/*     &            (JJ2 .EQ. NXK (3, K)) ) .OR. */
/*     &            ( (JJ2 .EQ. NXK (2, K)) .AND. */
/*     &            (JJ1 .EQ. NXK (3, K)) ) ) ) ) THEN */
		    if (! found) {
			io___41.ciunit = *iunit;
			i__3 = s_wsfe(&io___41);
			if (i__3 != 0) {
			    goto L200;
			}
/* Writing concatenation */
			i__5[0] = len, a__1[0] = dummy;
			i__5[1] = 2, a__1[1] = "_1";
			s_cat(ch__1, a__1, i__5, &c__2, (ftnlen)74);
			i__3 = do_fio(&c__1, ch__1, len + 2);
			if (i__3 != 0) {
			    goto L200;
			}
			i__3 = e_wsfe();
			if (i__3 != 0) {
			    goto L200;
			}
			found = TRUE_;
		    }
		    ++jhold;
		    ihold[jhold - 1] = mapgxd[k];
		    if (jhold == 9) {
			io___43.ciunit = *iunit;
			i__3 = s_wsfe(&io___43);
			if (i__3 != 0) {
			    goto L200;
			}
			for (ii = 1; ii <= 9; ++ii) {
			    i__3 = do_fio(&c__1, (char *)&ihold[ii - 1], (
				    ftnlen)sizeof(integer));
			    if (i__3 != 0) {
				goto L200;
			    }
			}
			i__3 = e_wsfe();
			if (i__3 != 0) {
			    goto L200;
			}
			jhold = 0;
		    }
		}
/* L150: */
	    }
	    if (jhold > 0) {
		io___45.ciunit = *iunit;
		i__2 = s_wsfe(&io___45);
		if (i__2 != 0) {
		    goto L200;
		}
		i__3 = jhold;
		for (ii = 1; ii <= i__3; ++ii) {
		    i__2 = do_fio(&c__1, (char *)&ihold[ii - 1], (ftnlen)
			    sizeof(integer));
		    if (i__2 != 0) {
			goto L200;
		    }
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    }
/*  WRITE OUT THE SIDE 2 ELEMENTS */
	    found = FALSE_;
	    jhold = 0;
	    i__2 = j2;
	    for (j = j1; j <= i__2; ++j) {
		jj1 = nsiden[(j << 1) - 1];
		jj2 = nsiden[j * 2];
		k = nelems[j];
		if (nxk[k * nxk_dim1 + 3] == 0) {
		    inode = 2;
		} else if (nxk[k * nxk_dim1 + 4] == 0) {
		    inode = 3;
		} else if (*eight || *nine) {
		    inode = 8;
		} else {
		    inode = 4;
		}
		if (inode == 4 && (jj1 == nxk[k * nxk_dim1 + 2] && jj2 == nxk[
			k * nxk_dim1 + 3] || jj2 == nxk[k * nxk_dim1 + 2] && 
			jj1 == nxk[k * nxk_dim1 + 3]) || inode == 8 && (jj1 ==
			 nxk[k * nxk_dim1 + 3] && jj2 == nxk[k * nxk_dim1 + 4]
			 || jj2 == nxk[k * nxk_dim1 + 3] && jj1 == nxk[k * 
			nxk_dim1 + 4])) {
/*     &            ( (JJ1 .EQ. NXK (4, K)) .AND. */
/*     &            (JJ2 .EQ. NXK (5, K)) ) .OR. */
/*     &            ( (JJ2 .EQ. NXK (4, K)) .AND. */
/*     &            (JJ1 .EQ. NXK (5, K)) ) ) ) ) THEN */
		    if (! found) {
			io___46.ciunit = *iunit;
			i__3 = s_wsfe(&io___46);
			if (i__3 != 0) {
			    goto L200;
			}
/* Writing concatenation */
			i__5[0] = len, a__1[0] = dummy;
			i__5[1] = 2, a__1[1] = "_2";
			s_cat(ch__1, a__1, i__5, &c__2, (ftnlen)74);
			i__3 = do_fio(&c__1, ch__1, len + 2);
			if (i__3 != 0) {
			    goto L200;
			}
			i__3 = e_wsfe();
			if (i__3 != 0) {
			    goto L200;
			}
			found = TRUE_;
		    }
		    ++jhold;
		    ihold[jhold - 1] = mapgxd[k];
		    if (jhold == 9) {
			io___47.ciunit = *iunit;
			i__3 = s_wsfe(&io___47);
			if (i__3 != 0) {
			    goto L200;
			}
			for (ii = 1; ii <= 9; ++ii) {
			    i__3 = do_fio(&c__1, (char *)&ihold[ii - 1], (
				    ftnlen)sizeof(integer));
			    if (i__3 != 0) {
				goto L200;
			    }
			}
			i__3 = e_wsfe();
			if (i__3 != 0) {
			    goto L200;
			}
			jhold = 0;
		    }
		}
/* L160: */
	    }
	    if (jhold > 0) {
		io___48.ciunit = *iunit;
		i__2 = s_wsfe(&io___48);
		if (i__2 != 0) {
		    goto L200;
		}
		i__3 = jhold;
		for (ii = 1; ii <= i__3; ++ii) {
		    i__2 = do_fio(&c__1, (char *)&ihold[ii - 1], (ftnlen)
			    sizeof(integer));
		    if (i__2 != 0) {
			goto L200;
		    }
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    }
/*  WRITE OUT THE SIDE 3 ELEMENTS */
	    found = FALSE_;
	    jhold = 0;
	    i__2 = j2;
	    for (j = j1; j <= i__2; ++j) {
		jj1 = nsiden[(j << 1) - 1];
		jj2 = nsiden[j * 2];
		k = nelems[j];
		if (nxk[k * nxk_dim1 + 3] == 0) {
		    inode = 2;
		} else if (nxk[k * nxk_dim1 + 4] == 0) {
		    inode = 3;
		} else if (*eight || *nine) {
		    inode = 8;
		} else {
		    inode = 4;
		}
		if (inode == 4 && (jj1 == nxk[k * nxk_dim1 + 3] && jj2 == nxk[
			k * nxk_dim1 + 4] || jj2 == nxk[k * nxk_dim1 + 3] && 
			jj1 == nxk[k * nxk_dim1 + 4]) || inode == 8 && (jj1 ==
			 nxk[k * nxk_dim1 + 5] && jj2 == nxk[k * nxk_dim1 + 6]
			 || jj2 == nxk[k * nxk_dim1 + 5] && jj1 == nxk[k * 
			nxk_dim1 + 6])) {
/*     &            ( (JJ1 .EQ. NXK (6, K)) .AND. */
/*     &            (JJ2 .EQ. NXK (7, K)) ) .OR. */
/*     &            ( (JJ2 .EQ. NXK (6, K)) .AND. */
/*     &            (JJ1 .EQ. NXK (7, K)) ) ) ) ) THEN */
		    if (! found) {
			io___49.ciunit = *iunit;
			i__3 = s_wsfe(&io___49);
			if (i__3 != 0) {
			    goto L200;
			}
/* Writing concatenation */
			i__5[0] = len, a__1[0] = dummy;
			i__5[1] = 2, a__1[1] = "_3";
			s_cat(ch__1, a__1, i__5, &c__2, (ftnlen)74);
			i__3 = do_fio(&c__1, ch__1, len + 2);
			if (i__3 != 0) {
			    goto L200;
			}
			i__3 = e_wsfe();
			if (i__3 != 0) {
			    goto L200;
			}
			found = TRUE_;
		    }
		    ++jhold;
		    ihold[jhold - 1] = mapgxd[k];
		    if (jhold == 9) {
			io___50.ciunit = *iunit;
			i__3 = s_wsfe(&io___50);
			if (i__3 != 0) {
			    goto L200;
			}
			for (ii = 1; ii <= 9; ++ii) {
			    i__3 = do_fio(&c__1, (char *)&ihold[ii - 1], (
				    ftnlen)sizeof(integer));
			    if (i__3 != 0) {
				goto L200;
			    }
			}
			i__3 = e_wsfe();
			if (i__3 != 0) {
			    goto L200;
			}
			jhold = 0;
		    }
		}
/* L170: */
	    }
	    if (jhold > 0) {
		io___51.ciunit = *iunit;
		i__2 = s_wsfe(&io___51);
		if (i__2 != 0) {
		    goto L200;
		}
		i__3 = jhold;
		for (ii = 1; ii <= i__3; ++ii) {
		    i__2 = do_fio(&c__1, (char *)&ihold[ii - 1], (ftnlen)
			    sizeof(integer));
		    if (i__2 != 0) {
			goto L200;
		    }
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    }
/*  WRITE OUT THE SIDE 4 ELEMENTS */
	    found = FALSE_;
	    jhold = 0;
	    i__2 = j2;
	    for (j = j1; j <= i__2; ++j) {
		jj1 = nsiden[(j << 1) - 1];
		jj2 = nsiden[j * 2];
		k = nelems[j];
		if (nxk[k * nxk_dim1 + 3] == 0) {
		    inode = 2;
		} else if (nxk[k * nxk_dim1 + 4] == 0) {
		    inode = 3;
		} else if (*eight || *nine) {
		    inode = 8;
		} else {
		    inode = 4;
		}
		if (inode == 4 && (jj1 == nxk[k * nxk_dim1 + 4] && jj2 == nxk[
			k * nxk_dim1 + 1] || jj2 == nxk[k * nxk_dim1 + 4] && 
			jj1 == nxk[k * nxk_dim1 + 1]) || inode == 8 && (jj1 ==
			 nxk[k * nxk_dim1 + 7] && jj2 == nxk[k * nxk_dim1 + 8]
			 || jj2 == nxk[k * nxk_dim1 + 7] && jj1 == nxk[k * 
			nxk_dim1 + 8])) {
/*     &            ( (JJ1 .EQ. NXK (8, K)) .AND. */
/*     &            (JJ2 .EQ. NXK (1, K)) ) .OR. */
/*     &            ( (JJ2 .EQ. NXK (8, K)) .AND. */
/*     &            (JJ1 .EQ. NXK (1, K)) ) ) ) ) THEN */
		    if (! found) {
			io___52.ciunit = *iunit;
			i__3 = s_wsfe(&io___52);
			if (i__3 != 0) {
			    goto L200;
			}
/* Writing concatenation */
			i__5[0] = len, a__1[0] = dummy;
			i__5[1] = 2, a__1[1] = "_4";
			s_cat(ch__1, a__1, i__5, &c__2, (ftnlen)74);
			i__3 = do_fio(&c__1, ch__1, len + 2);
			if (i__3 != 0) {
			    goto L200;
			}
			i__3 = e_wsfe();
			if (i__3 != 0) {
			    goto L200;
			}
			found = TRUE_;
		    }
		    ++jhold;
		    ihold[jhold - 1] = mapgxd[k];
		    if (jhold == 9) {
			io___53.ciunit = *iunit;
			i__3 = s_wsfe(&io___53);
			if (i__3 != 0) {
			    goto L200;
			}
			for (ii = 1; ii <= 9; ++ii) {
			    i__3 = do_fio(&c__1, (char *)&ihold[ii - 1], (
				    ftnlen)sizeof(integer));
			    if (i__3 != 0) {
				goto L200;
			    }
			}
			i__3 = e_wsfe();
			if (i__3 != 0) {
			    goto L200;
			}
			jhold = 0;
		    }
		}
/* L180: */
	    }
	    if (jhold > 0) {
		io___54.ciunit = *iunit;
		i__2 = s_wsfe(&io___54);
		if (i__2 != 0) {
		    goto L200;
		}
		i__3 = jhold;
		for (ii = 1; ii <= i__3; ++ii) {
		    i__2 = do_fio(&c__1, (char *)&ihold[ii - 1], (ftnlen)
			    sizeof(integer));
		    if (i__2 != 0) {
			goto L200;
		    }
		}
		i__2 = e_wsfe();
		if (i__2 != 0) {
		    goto L200;
		}
	    }
/* L190: */
	}
    }
    mesage_("ABAQUS OUTPUT FILE SUCCESSFULLY WRITTEN", (ftnlen)39);
    *err = FALSE_;
    return 0;
/*  ERR DURING WRITE PROBLEMS */
L200:
    mesage_("ERR DURING WRITE TO ABAQUS OUTPUT FILE", (ftnlen)38);
    mesage_("         - NO FILE SAVED -            ", (ftnlen)38);
    return 0;
} /* wrabqs_ */

#ifdef __cplusplus
	}
#endif

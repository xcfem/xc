/* wrjerr.f -- translated by f2c (version 20160102).
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
static integer c__3 = 3;
static integer c__4 = 4;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int wrjerr_(integer *ms, integer *mr, integer *npnode, 
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
    static char fmt_10000[] = "(a72)";
    static char fmt_10010[] = "(\002*PROSTRFAC 0.0\002,/,\002*PSTRESS\002,/\
,\002*MATERIAL\002,/,\002*ELASTIC\002,/,\00230.E+6 .3\002,/,\002*MATERIAL\
\002,/,\002*ELASTIC\002,/,\00230.E+6 .3\002)";
    static char fmt_10020[] = "(\002*NODE\002)";
    static char fmt_10030[] = "(i10,\002,\002,2(e14.7,\002,\002),e14.7)";
    static char fmt_10040[] = "(\002*ELEMENT\002)";
    static char fmt_10050[] = "(8(i8,\002,\002),i8)";
    static char fmt_10060[] = "(4(i10,\002,\002),i10)";
    static char fmt_10070[] = "(\002*FIXED\002)";
    static char fmt_10080[] = "(/,\002 FOR NODE BOUNDARY FLAG\002,i5)";
    static char fmt_10100[] = "(i8,\002,\002,i8)";
    static char fmt_10120[] = "(\002*STEP\002,/,\0021.,1.\002,/,\002*POST\
\002,/,\002*MAXIT  2\002,/,\002PRINT  50\002)";
    static char fmt_10130[] = "(\002*PTOL  1.E-2\002)";
    static char fmt_10090[] = "(/,\002 FOR ELEMENT BOUNDARY FLAG\002,i5)";
    static char fmt_10110[] = "(\002EL,\002,i8,\002, P\002,i1,\002,\002,f10.\
6)";
    static char fmt_10140[] = "(\002*END\002)";

    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2, i__3, i__4;
    icilist ici__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(), 
	    s_rsfi(icilist *), e_rsfi();

    /* Local variables */
    static integer i__, j, k;
    static real z__;
    static integer j1, j2, ii, jj1, jj2, len, int__;
    static real pmag;
    static integer kelem, inode, ihold[9], jhold;
    static logical found;
    static char dummy[72];
    extern /* Subroutine */ int getdum_(integer *, 
	    char *, integer *, ftnlen), inqstr_(char *, char *, ftnlen, 
	    ftnlen);

    /* Fortran I/O blocks */
    static cilist io___1 = { 1, 0, 0, fmt_10000, 0 };
    static cilist io___2 = { 1, 0, 0, fmt_10010, 0 };
    static cilist io___3 = { 1, 0, 0, fmt_10020, 0 };
    static cilist io___6 = { 1, 0, 0, fmt_10030, 0 };
    static cilist io___9 = { 1, 0, 0, fmt_10040, 0 };
    static cilist io___13 = { 1, 0, 0, fmt_10050, 0 };
    static cilist io___14 = { 1, 0, 0, fmt_10060, 0 };
    static cilist io___16 = { 0, 0, 0, fmt_10070, 0 };
    static cilist io___19 = { 0, 6, 0, fmt_10080, 0 };
    static cilist io___21 = { 1, 0, 0, fmt_10100, 0 };
    static cilist io___22 = { 1, 0, 0, fmt_10120, 0 };
    static cilist io___23 = { 1, 0, 0, fmt_10130, 0 };
    static cilist io___24 = { 0, 6, 0, fmt_10090, 0 };
    static cilist io___32 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___33 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___34 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___35 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___36 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___37 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___38 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___39 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___40 = { 1, 0, 0, fmt_10140, 0 };


/* *********************************************************************** */
/*  SUBROUTINE WRJERR = WRITES JOE'S ERROR DATABASE MESH OUTPUT FILE */
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
    inqstr_("TITLE: ", title, (ftnlen)7, (ftnlen)72);
    io___1.ciunit = *iunit;
    i__1 = s_wsfe(&io___1);
    if (i__1 != 0) {
	goto L290;
    }
    i__1 = do_fio(&c__1, title, (ftnlen)72);
    if (i__1 != 0) {
	goto L290;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L290;
    }
    io___2.ciunit = *iunit;
    i__1 = s_wsfe(&io___2);
    if (i__1 != 0) {
	goto L290;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L290;
    }
/*  WRITE OUT NODE BLOCK */
    io___3.ciunit = *iunit;
    i__1 = s_wsfe(&io___3);
    if (i__1 != 0) {
	goto L290;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L290;
    }
    z__ = (float)0.;
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___6.ciunit = *iunit;
	i__2 = s_wsfe(&io___6);
	if (i__2 != 0) {
	    goto L290;
	}
	i__2 = do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L290;
	}
	i__2 = do_fio(&c__1, (char *)&xn[i__], (ftnlen)sizeof(real));
	if (i__2 != 0) {
	    goto L290;
	}
	i__2 = do_fio(&c__1, (char *)&yn[i__], (ftnlen)sizeof(real));
	if (i__2 != 0) {
	    goto L290;
	}
	i__2 = do_fio(&c__1, (char *)&z__, (ftnlen)sizeof(real));
	if (i__2 != 0) {
	    goto L290;
	}
	i__2 = e_wsfe();
	if (i__2 != 0) {
	    goto L290;
	}
/* L100: */
    }
/*  WRITE OUT ELEMENT BLOCKS */
    i__1 = *nummat;
    for (i__ = 1; i__ <= i__1; ++i__) {
	getdum_(&matmap[i__ * 3 + 1], dummy, &len, (ftnlen)72);
	io___9.ciunit = *iunit;
	i__2 = s_wsfe(&io___9);
	if (i__2 != 0) {
	    goto L290;
	}
	i__2 = e_wsfe();
	if (i__2 != 0) {
	    goto L290;
	}
	if (nxk[matmap[i__ * 3 + 2] * nxk_dim1 + 3] == 0) {
	    inode = 2;
	} else if (nxk[matmap[i__ * 3 + 2] * nxk_dim1 + 4] == 0) {
	    inode = 3;
	} else if (*eight) {
	    inode = 8;
	} else if (*nine) {
	    inode = 9;
	} else {
	    inode = 4;
	}
	if (inode == 8) {
	    i__2 = matmap[i__ * 3 + 3];
	    for (kelem = matmap[i__ * 3 + 2]; kelem <= i__2; ++kelem) {
		k = mapgxd[kelem];
		io___13.ciunit = *iunit;
		i__3 = s_wsfe(&io___13);
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 1], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 3], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 5], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 7], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 2], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 4], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 6], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[kelem * nxk_dim1 + 8], (
			ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = e_wsfe();
		if (i__3 != 0) {
		    goto L290;
		}
/* L110: */
	    }
	} else {
	    i__2 = matmap[i__ * 3 + 3];
	    for (kelem = matmap[i__ * 3 + 2]; kelem <= i__2; ++kelem) {
		k = mapgxd[kelem];
		io___14.ciunit = *iunit;
		i__3 = s_wsfe(&io___14);
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__4 = inode;
		for (j = 1; j <= i__4; ++j) {
		    i__3 = do_fio(&c__1, (char *)&nxk[j + kelem * nxk_dim1], (
			    ftnlen)sizeof(integer));
		    if (i__3 != 0) {
			goto L290;
		    }
		}
		i__3 = e_wsfe();
		if (i__3 != 0) {
		    goto L290;
		}
/* L120: */
	    }
	}
/* L130: */
    }
/*  WRITE OUT THE NODAL BOUNDARY CONDITIONS */
    if (*nbcnod > 0) {
	io___16.ciunit = *iunit;
	s_wsfe(&io___16);
	e_wsfe();
	i__1 = *nbcnod;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j1 = nnptr[i__];
	    j2 = nnptr[i__] + nnlen[i__] - 1;
	    getdum_(&nnflg[i__], dummy, &len, (ftnlen)72);
	    s_wsfe(&io___19);
	    do_fio(&c__1, (char *)&nnflg[i__], (ftnlen)sizeof(integer));
	    e_wsfe();
	    inqstr_("FIXED DEGREE OF FREEDOM: ", title, (ftnlen)25, (ftnlen)
		    72);
	    ici__1.icierr = 0;
	    ici__1.iciend = 0;
	    ici__1.icirnum = 1;
	    ici__1.icirlen = 72;
	    ici__1.iciunit = title;
	    ici__1.icifmt = "(I10)";
	    s_rsfi(&ici__1);
	    do_fio(&c__1, (char *)&int__, (ftnlen)sizeof(integer));
	    e_rsfi();
	    i__2 = j2;
	    for (j = j1; j <= i__2; ++j) {
		io___21.ciunit = *iunit;
		i__3 = s_wsfe(&io___21);
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&nodes[j], (ftnlen)sizeof(
			integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = do_fio(&c__1, (char *)&int__, (ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L290;
		}
		i__3 = e_wsfe();
		if (i__3 != 0) {
		    goto L290;
		}
/* L140: */
	    }
/* L150: */
	}
    }
/*  WRITE OUT THE SIDE BOUNDARY FLAGS */
    io___22.ciunit = *iunit;
    i__1 = s_wsfe(&io___22);
    if (i__1 != 0) {
	goto L290;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L290;
    }
    if (*nbcsid > 0) {
	io___23.ciunit = *iunit;
	i__1 = s_wsfe(&io___23);
	if (i__1 != 0) {
	    goto L290;
	}
	i__1 = e_wsfe();
	if (i__1 != 0) {
	    goto L290;
	}
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
	    s_wsfe(&io___24);
	    do_fio(&c__1, (char *)&nsflg[i__], (ftnlen)sizeof(integer));
	    e_wsfe();
	    inqstr_("PRESSURE MAGNITUDE: ", title, (ftnlen)20, (ftnlen)72);
	    ici__1.icierr = 0;
	    ici__1.iciend = 0;
	    ici__1.icirnum = 1;
	    ici__1.icirlen = 72;
	    ici__1.iciunit = title;
	    ici__1.icifmt = "(F10.0)";
	    s_rsfi(&ici__1);
	    do_fio(&c__1, (char *)&pmag, (ftnlen)sizeof(real));
	    e_rsfi();
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
			found = TRUE_;
		    }
		    ++jhold;
		    ihold[jhold - 1] = mapgxd[k];
		    if (jhold == 9) {
			i__3 = jhold;
			for (ii = 1; ii <= i__3; ++ii) {
			    io___32.ciunit = *iunit;
			    i__4 = s_wsfe(&io___32);
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&ihold[ii - 1], (
				    ftnlen)sizeof(integer));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&c__1, (ftnlen)
				    sizeof(integer));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&pmag, (ftnlen)
				    sizeof(real));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = e_wsfe();
			    if (i__4 != 0) {
				goto L290;
			    }
/* L160: */
			}
			jhold = 0;
		    }
		}
/* L170: */
	    }
	    if (jhold > 0) {
		i__2 = jhold;
		for (ii = 1; ii <= i__2; ++ii) {
		    io___33.ciunit = *iunit;
		    i__3 = s_wsfe(&io___33);
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&ihold[ii - 1], (ftnlen)
			    sizeof(integer));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(
			    integer));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&pmag, (ftnlen)sizeof(real));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = e_wsfe();
		    if (i__3 != 0) {
			goto L290;
		    }
/* L180: */
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
			found = TRUE_;
		    }
		    ++jhold;
		    ihold[jhold - 1] = mapgxd[k];
		    if (jhold == 9) {
			i__3 = jhold;
			for (ii = 2; ii <= i__3; ++ii) {
			    io___34.ciunit = *iunit;
			    i__4 = s_wsfe(&io___34);
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&ihold[ii - 1], (
				    ftnlen)sizeof(integer));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&c__2, (ftnlen)
				    sizeof(integer));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&pmag, (ftnlen)
				    sizeof(real));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = e_wsfe();
			    if (i__4 != 0) {
				goto L290;
			    }
/* L190: */
			}
			jhold = 0;
		    }
		}
/* L200: */
	    }
	    if (jhold > 0) {
		i__2 = jhold;
		for (ii = 1; ii <= i__2; ++ii) {
		    io___35.ciunit = *iunit;
		    i__3 = s_wsfe(&io___35);
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&ihold[ii - 1], (ftnlen)
			    sizeof(integer));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(
			    integer));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&pmag, (ftnlen)sizeof(real));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = e_wsfe();
		    if (i__3 != 0) {
			goto L290;
		    }
/* L210: */
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
			found = TRUE_;
		    }
		    ++jhold;
		    ihold[jhold - 1] = mapgxd[k];
		    if (jhold == 9) {
			i__3 = jhold;
			for (ii = 1; ii <= i__3; ++ii) {
			    io___36.ciunit = *iunit;
			    i__4 = s_wsfe(&io___36);
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&ihold[ii - 1], (
				    ftnlen)sizeof(integer));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&c__3, (ftnlen)
				    sizeof(integer));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&pmag, (ftnlen)
				    sizeof(real));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = e_wsfe();
			    if (i__4 != 0) {
				goto L290;
			    }
/* L220: */
			}
			jhold = 0;
		    }
		}
/* L230: */
	    }
	    if (jhold > 0) {
		i__2 = jhold;
		for (ii = 1; ii <= i__2; ++ii) {
		    io___37.ciunit = *iunit;
		    i__3 = s_wsfe(&io___37);
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&ihold[ii - 1], (ftnlen)
			    sizeof(integer));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(
			    integer));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&pmag, (ftnlen)sizeof(real));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = e_wsfe();
		    if (i__3 != 0) {
			goto L290;
		    }
/* L240: */
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
			found = TRUE_;
		    }
		    ++jhold;
		    ihold[jhold - 1] = mapgxd[k];
		    if (jhold == 9) {
			i__3 = jhold;
			for (ii = 1; ii <= i__3; ++ii) {
			    io___38.ciunit = *iunit;
			    i__4 = s_wsfe(&io___38);
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&ihold[ii - 1], (
				    ftnlen)sizeof(integer));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&c__4, (ftnlen)
				    sizeof(integer));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = do_fio(&c__1, (char *)&pmag, (ftnlen)
				    sizeof(real));
			    if (i__4 != 0) {
				goto L290;
			    }
			    i__4 = e_wsfe();
			    if (i__4 != 0) {
				goto L290;
			    }
/* L250: */
			}
			jhold = 0;
		    }
		}
/* L260: */
	    }
	    if (jhold > 0) {
		i__2 = jhold;
		for (ii = 1; ii <= i__2; ++ii) {
		    io___39.ciunit = *iunit;
		    i__3 = s_wsfe(&io___39);
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&ihold[ii - 1], (ftnlen)
			    sizeof(integer));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(
			    integer));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = do_fio(&c__1, (char *)&pmag, (ftnlen)sizeof(real));
		    if (i__3 != 0) {
			goto L290;
		    }
		    i__3 = e_wsfe();
		    if (i__3 != 0) {
			goto L290;
		    }
/* L270: */
		}
	    }
/* L280: */
	}
    }
    io___40.ciunit = *iunit;
    i__1 = s_wsfe(&io___40);
    if (i__1 != 0) {
	goto L290;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L290;
    }
    mesage_("JOE'S ERROR OUTPUT FILE SUCCESSFULLY WRITTEN", (ftnlen)44);
    *err = FALSE_;
    return 0;
/*  ERR DURING WRITE PROBLEMS */
L290:
    mesage_("ERR DURING WRITE TO ABAQUS OUTPUT FILE", (ftnlen)38);
    mesage_("         - NO FILE SAVED -            ", (ftnlen)38);
    return 0;
} /* wrjerr_ */

#ifdef __cplusplus
	}
#endif

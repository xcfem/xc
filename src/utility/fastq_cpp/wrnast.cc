/* wrnast.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int wrnast_(integer *ms, integer *mr, integer *npnode, 
	integer *npelem, integer *mxnflg, integer *mxsflg, integer *npregn, 
	integer *npnbc, integer *npsbc, integer *iunit, integer *nnn, integer 
	*kkk, integer *nnxk, integer *nodes, integer *nelems, integer *nnflg, 
	integer *nnptr, integer *nnlen, integer *nsflg, integer *nsptr, 
	integer *nslen, integer *nvptr, integer *nvlen, integer *nsiden, 
	integer *mapdxg, real *xn, real *yn, integer *nxk, integer *mat, 
	integer *mapgxd, integer *matmap, integer *nbcnod, integer *nnlist, 
	integer *nbcsid, integer *nslist, integer *nvlist, integer *nummat, 
	integer *linkm, char *title, logical *err, logical *eight, logical *
	nine, logical *long__, ftnlen title_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002$TITLE: \002,/,a72)";
    static char fmt_10010[] = "(\002$\002,/,\002$     MESH GENERATED USING F\
ASTQ        \002,/,\002$  NUMBER OF NODES:                     \002,i5,/,\
\002$  NUMBER OF ELEMENTS:                  \002,i5,/,\002$  NUMBER OF NODAL\
 BOUNDARY CONDITIONS: \002,i5,/,\002$\002,/,\002BEGIN BULK\002)";
    static char fmt_10020[] = "(\002$ NODE (GRID) DATA FOLLOWS:\002)";
    static char fmt_10030[] = "(\002GRID*   \002,i16,16x,2e16.9,\002*N\002,a\
6,/,\002*N\002,a6,e16.9,\002345\002)";
    static char fmt_10040[] = "(\002GRID    \002,i8,8x,3f8.4,\002345\002)";
    static char fmt_10050[] = "(\002$ 2 NODE BAR ELEMENTS FOR BLOCK ID \002,\
a,\002 FOLLOW:\002)";
    static char fmt_10060[] = "(\002 FOR BLOCK ID:\002,i7,\002  ENTER NEW\
\002)";
    static char fmt_10070[] = "(\002$ 8 NODE QUAD ELEMENTS FOR BLOCK ID \002\
,a,\002 FOLLOW:\002)";
    static char fmt_10080[] = "(\002$ 4 NODE QUAD ELEMENTS FOR BLOCK ID \002\
,a,\002 FOLLOW:\002)";
    static char fmt_10090[] = "(a8,4i8)";
    static char fmt_10100[] = "(a8,9i8,/,8x,i8)";
    static char fmt_10110[] = "(a8,6i8)";
    static char fmt_10120[] = "(\002$ NODAL CONSTRAINTS FOR BOUNDARY FLAG\
 \002,a,\002 FOLLOW:\002)";
    static char fmt_10130[] = "(\002 INPUT THE CONSTRAINTS FOR NODAL BOUNDAR\
Y FLAG: \002,i5)";
    static char fmt_10140[] = "(\002SPC     \002,2i8,a8)";
    static char fmt_10150[] = "(\002ENDDATA\002)";

    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2, i__3, i__4;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, j, k;
    static real z__;
    static integer j1, j2, len, len2, inode, ileft;
    static char dummy[72], dummy2[72];
    extern /* Subroutine */ int getdum_(integer *, 
	    char *, integer *, ftnlen);
    static integer iright;
    static logical deftyp;
    extern /* Subroutine */ int stripb_(char *, integer *, integer *, ftnlen),
	     strlng_(char *, integer *, ftnlen), inqstr_(char *, char *, 
	    ftnlen, ftnlen), inqtru_(char *, logical *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___1 = { 1, 0, 0, fmt_10000, 0 };
    static cilist io___2 = { 1, 0, 0, fmt_10010, 0 };
    static cilist io___3 = { 1, 0, 0, fmt_10020, 0 };
    static cilist io___8 = { 1, 0, 0, fmt_10030, 0 };
    static cilist io___9 = { 1, 0, 0, fmt_10040, 0 };
    static cilist io___11 = { 1, 0, 0, fmt_10050, 0 };
    static cilist io___15 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___16 = { 1, 0, 0, fmt_10050, 0 };
    static cilist io___17 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___18 = { 1, 0, 0, fmt_10070, 0 };
    static cilist io___19 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___20 = { 1, 0, 0, fmt_10080, 0 };
    static cilist io___21 = { 0, 6, 0, fmt_10060, 0 };
    static cilist io___25 = { 1, 0, 0, fmt_10090, 0 };
    static cilist io___27 = { 1, 0, 0, fmt_10090, 0 };
    static cilist io___28 = { 1, 0, 0, fmt_10100, 0 };
    static cilist io___29 = { 1, 0, 0, fmt_10110, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_10120, 0 };
    static cilist io___33 = { 0, 6, 0, fmt_10130, 0 };
    static cilist io___34 = { 1, 0, 0, fmt_10140, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_10150, 0 };


/* ************************************************************************ */
/*  SUBROUTINE WRNAST = WRITES NASTRAN DATABASE MESH OUTPUT FILE */
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
	goto L180;
    }
    i__1 = do_fio(&c__1, title, (ftnlen)72);
    if (i__1 != 0) {
	goto L180;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L180;
    }
    io___2.ciunit = *iunit;
    i__1 = s_wsfe(&io___2);
    if (i__1 != 0) {
	goto L180;
    }
    i__1 = do_fio(&c__1, (char *)&(*nnn), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L180;
    }
    i__1 = do_fio(&c__1, (char *)&(*kkk), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L180;
    }
    i__1 = do_fio(&c__1, (char *)&(*nbcnod), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L180;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L180;
    }
/*  WRITE OUT NODE BLOCK */
    io___3.ciunit = *iunit;
    i__1 = s_wsfe(&io___3);
    if (i__1 != 0) {
	goto L180;
    }
    i__1 = e_wsfe();
    if (i__1 != 0) {
	goto L180;
    }
    z__ = (float)0.;
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	getdum_(&i__, dummy, &len, (ftnlen)72);
	if (*long__) {
	    io___8.ciunit = *iunit;
	    i__2 = s_wsfe(&io___8);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, (char *)&xn[i__], (ftnlen)sizeof(real));
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, (char *)&yn[i__], (ftnlen)sizeof(real));
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, dummy, (ftnlen)6);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, dummy, (ftnlen)6);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, (char *)&z__, (ftnlen)sizeof(real));
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = e_wsfe();
	    if (i__2 != 0) {
		goto L180;
	    }
	} else {
	    io___9.ciunit = *iunit;
	    i__2 = s_wsfe(&io___9);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, (char *)&xn[i__], (ftnlen)sizeof(real));
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, (char *)&yn[i__], (ftnlen)sizeof(real));
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, (char *)&z__, (ftnlen)sizeof(real));
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = e_wsfe();
	    if (i__2 != 0) {
		goto L180;
	    }
	}
/* L100: */
    }
/*  QUERY THE USER FOR LOCAL CONTROL OF ELEMENT TYPE */
    inqtru_("USE DEFAULT ELEMENT TYPES FOR ELEMENT BLOCKS", &deftyp, (ftnlen)
	    44);
/*  WRITE OUT ELEMENT BLOCKS */
    i__1 = *nummat;
    for (i__ = 1; i__ <= i__1; ++i__) {
	getdum_(&matmap[i__ * 3 + 1], dummy, &len, (ftnlen)72);
	if (nxk[matmap[i__ * 3 + 2] * nxk_dim1 + 3] == 0) {
	    io___11.ciunit = *iunit;
	    i__2 = s_wsfe(&io___11);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, dummy, len);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = e_wsfe();
	    if (i__2 != 0) {
		goto L180;
	    }
	    inode = 2;
	    if (deftyp) {
		s_copy(dummy2, "CBAR", (ftnlen)72, (ftnlen)4);
		len2 = 4;
	    } else {
		s_wsfe(&io___15);
		do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)sizeof(
			integer));
		e_wsfe();
		inqstr_("2 NODE ELEMENT TYPE:  ", dummy2, (ftnlen)22, (ftnlen)
			72);
		strlng_(dummy2, &len2, (ftnlen)72);
	    }
	} else if (nxk[matmap[i__ * 3 + 2] * nxk_dim1 + 4] == 0) {
	    mesage_("THREE NODE BAR ELEMENTS NOT SUPPORTED", (ftnlen)37);
	    mesage_("THE CENTER NODE WILL BE IGNORED", (ftnlen)31);
	    io___16.ciunit = *iunit;
	    i__2 = s_wsfe(&io___16);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, dummy, len);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = e_wsfe();
	    if (i__2 != 0) {
		goto L180;
	    }
	    if (deftyp) {
		s_copy(dummy2, "CBAR", (ftnlen)72, (ftnlen)4);
		len2 = 4;
	    } else {
		s_wsfe(&io___17);
		do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)sizeof(
			integer));
		e_wsfe();
		inqstr_("2 NODE ELEMENT TYPE:  ", dummy2, (ftnlen)22, (ftnlen)
			72);
		strlng_(dummy2, &len2, (ftnlen)72);
	    }
	    inode = 3;
	} else if (*eight || *nine) {
	    io___18.ciunit = *iunit;
	    i__2 = s_wsfe(&io___18);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, dummy, len);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = e_wsfe();
	    if (i__2 != 0) {
		goto L180;
	    }
	    if (*nine) {
		mesage_("NINE NODE QUAD ELEMENTS NOT SUPPORTED", (ftnlen)37);
		mesage_("THE CENTER NODE WILL BE IGNORED", (ftnlen)31);
	    }
	    if (deftyp) {
		s_copy(dummy2, "CQUAD8", (ftnlen)72, (ftnlen)6);
		len2 = 6;
	    } else {
		s_wsfe(&io___19);
		do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)sizeof(
			integer));
		e_wsfe();
		inqstr_("8 NODE ELEMENT TYPE:  ", dummy2, (ftnlen)22, (ftnlen)
			72);
		strlng_(dummy2, &len2, (ftnlen)72);
	    }
	    inode = 8;
	} else {
	    io___20.ciunit = *iunit;
	    i__2 = s_wsfe(&io___20);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = do_fio(&c__1, dummy, len);
	    if (i__2 != 0) {
		goto L180;
	    }
	    i__2 = e_wsfe();
	    if (i__2 != 0) {
		goto L180;
	    }
	    if (deftyp) {
		s_copy(dummy2, "CQUAD4", (ftnlen)72, (ftnlen)6);
		len2 = 6;
	    } else {
		s_wsfe(&io___21);
		do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)sizeof(
			integer));
		e_wsfe();
		inqstr_("4 NODE ELEMENT TYPE:  ", dummy2, (ftnlen)22, (ftnlen)
			72);
		strlng_(dummy2, &len2, (ftnlen)72);
	    }
	    inode = 4;
	}
	stripb_(dummy2, &ileft, &iright, (ftnlen)72);
	iright = ileft + 7;
	if (nxk[matmap[i__ * 3 + 2] * nxk_dim1 + 3] == 0) {
	    i__2 = matmap[i__ * 3 + 3];
	    for (k = matmap[i__ * 3 + 2]; k <= i__2; ++k) {
		io___25.ciunit = *iunit;
		i__3 = s_wsfe(&io___25);
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, dummy2 + (ileft - 1), iright - (ileft - 
			1));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__4 = inode;
		for (j = 1; j <= i__4; ++j) {
		    i__3 = do_fio(&c__1, (char *)&nxk[j + k * nxk_dim1], (
			    ftnlen)sizeof(integer));
		    if (i__3 != 0) {
			goto L180;
		    }
		}
		i__3 = e_wsfe();
		if (i__3 != 0) {
		    goto L180;
		}
/* L110: */
	    }
	} else if (nxk[matmap[i__ * 3 + 2] * nxk_dim1 + 4] == 0) {
	    i__2 = matmap[i__ * 3 + 3];
	    for (k = matmap[i__ * 3 + 2]; k <= i__2; ++k) {
		io___27.ciunit = *iunit;
		i__3 = s_wsfe(&io___27);
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, dummy2 + (ileft - 1), iright - (ileft - 
			1));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__4 = inode;
		for (j = 1; j <= i__4; j += 2) {
		    i__3 = do_fio(&c__1, (char *)&nxk[j + k * nxk_dim1], (
			    ftnlen)sizeof(integer));
		    if (i__3 != 0) {
			goto L180;
		    }
		}
		i__3 = e_wsfe();
		if (i__3 != 0) {
		    goto L180;
		}
/* L120: */
	    }
	} else if (*eight || *nine) {
	    i__2 = matmap[i__ * 3 + 3];
	    for (k = matmap[i__ * 3 + 2]; k <= i__2; ++k) {
		io___28.ciunit = *iunit;
		i__3 = s_wsfe(&io___28);
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, dummy2 + (ileft - 1), iright - (ileft - 
			1));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[k * nxk_dim1 + 1], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[k * nxk_dim1 + 3], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[k * nxk_dim1 + 5], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[k * nxk_dim1 + 7], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[k * nxk_dim1 + 2], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[k * nxk_dim1 + 4], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[k * nxk_dim1 + 6], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&nxk[k * nxk_dim1 + 8], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = e_wsfe();
		if (i__3 != 0) {
		    goto L180;
		}
/* L130: */
	    }
	} else {
	    i__2 = matmap[i__ * 3 + 3];
	    for (k = matmap[i__ * 3 + 2]; k <= i__2; ++k) {
		io___29.ciunit = *iunit;
		i__3 = s_wsfe(&io___29);
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, dummy2 + (ileft - 1), iright - (ileft - 
			1));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)
			sizeof(integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__4 = inode;
		for (j = 1; j <= i__4; ++j) {
		    i__3 = do_fio(&c__1, (char *)&nxk[j + k * nxk_dim1], (
			    ftnlen)sizeof(integer));
		    if (i__3 != 0) {
			goto L180;
		    }
		}
		i__3 = e_wsfe();
		if (i__3 != 0) {
		    goto L180;
		}
/* L140: */
	    }
	}
/* L150: */
    }
/*  WRITE OUT THE NODAL BOUNDARY CONDITIONS */
    if (*nbcnod > 0) {
	i__1 = *nbcnod;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j1 = nnptr[i__];
	    j2 = nnptr[i__] + nnlen[i__] - 1;
	    getdum_(&nnflg[i__], dummy, &len, (ftnlen)72);
	    io___32.ciunit = *iunit;
	    s_wsfe(&io___32);
	    do_fio(&c__1, dummy, len);
	    e_wsfe();
	    s_wsfe(&io___33);
	    do_fio(&c__1, (char *)&nnflg[i__], (ftnlen)sizeof(integer));
	    e_wsfe();
	    inqstr_("DEGREES OF FREEDOM  RESTRAINED (NO BLANKS): ", dummy, (
		    ftnlen)44, (ftnlen)72);
	    i__2 = j2;
	    for (j = j1; j <= i__2; ++j) {
		io___34.ciunit = *iunit;
		i__3 = s_wsfe(&io___34);
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&nnflg[i__], (ftnlen)sizeof(
			integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, (char *)&nodes[j], (ftnlen)sizeof(
			integer));
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = do_fio(&c__1, dummy, (ftnlen)6);
		if (i__3 != 0) {
		    goto L180;
		}
		i__3 = e_wsfe();
		if (i__3 != 0) {
		    goto L180;
		}
/* L160: */
	    }
/* L170: */
	}
    }
/*  NOTIFY USER THAT SIDE BOUNDARY FLAGS ARE NOT SUPPORTED */
    if (*nbcsid > 0) {
	mesage_("NO SIDE BOUNDARY FLAGS SUPPORTED BY NASTRAN", (ftnlen)43);
    }
/*  END THE DATA */
    io___35.ciunit = *iunit;
    s_wsfe(&io___35);
    e_wsfe();
    mesage_("NASTRAN OUTPUT FILE SUCCESSFULLY WRITTEN", (ftnlen)40);
    *err = FALSE_;
    return 0;
/*  ERR DURING WRITE PROBLEMS */
L180:
    mesage_("ERR DURING WRITE TO ABAQUS OUTPUT FILE", (ftnlen)38);
    mesage_("         - NO FILE SAVED -            ", (ftnlen)38);
    return 0;
} /* wrnast_ */

#ifdef __cplusplus
	}
#endif

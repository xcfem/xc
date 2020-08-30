/* wrgens.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int wrgens_(integer *ms, integer *mr, integer *npnode, 
	integer *npelem, integer *mxnflg, integer *mxsflg, integer *npregn, 
	integer *npnbc, integer *npsbc, integer *iunit, integer *nnn, integer 
	*kkk, integer *nnxk, integer *nodes, integer *nelems, integer *nnflg, 
	integer *nnptr, integer *nnlen, integer *nsflg, integer *nsptr, 
	integer *nslen, integer *nvptr, integer *nvlen, integer *nsiden, 
	integer *mapdxg, real *xn, real *yn, integer *nxk, integer *mat, 
	integer *mapgxd, integer *matmap, real *wtnode, real *wtside, integer 
	*nbcnod, integer *nnlist, integer *nbcsid, integer *nslist, integer *
	nvlist, integer *nummat, integer *linkm, char *title, logical *err, 
	logical *eight, logical *nine, char *versn, ftnlen title_len, ftnlen 
	versn_len)
{
    /* Initialized data */

    static integer lcon[9] = { 1,3,5,7,2,4,6,8,9 };
    static integer lbar[3] = { 1,3,2 };

    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2, i__3, i__4;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsue(cilist *), do_uio(integer *, char *, ftnlen), e_wsue();

    /* Local variables */
    static integer i__, j, k, ii, ibc, iel, ijk, ipe, ipn, ibee, ibeg;
    static char date[8];
    static integer iend, iene;
    static char hold[80];
    static integer inod;
    static char time[8];
    static real attr;
    static char ename[8*1000];
    static integer inode, ihold;
    static char xname[8], yname[8];
    static integer nloop, ivers, nattr, nslst, nvlst;
    static char versn1[8], versn2[8];
    extern /* Subroutine */ int exdate_(char *, 
	    ftnlen), extime_(char *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___10 = { 1, 0, 0, 0, 0 };
    static cilist io___15 = { 1, 0, 0, 0, 0 };
    static cilist io___16 = { 1, 0, 0, 0, 0 };
    static cilist io___18 = { 1, 0, 0, 0, 0 };
    static cilist io___23 = { 1, 0, 0, 0, 0 };
    static cilist io___24 = { 1, 0, 0, 0, 0 };
    static cilist io___27 = { 1, 0, 0, 0, 0 };
    static cilist io___28 = { 1, 0, 0, 0, 0 };
    static cilist io___31 = { 1, 0, 0, 0, 0 };
    static cilist io___32 = { 1, 0, 0, 0, 0 };
    static cilist io___33 = { 1, 0, 0, 0, 0 };
    static cilist io___34 = { 1, 0, 0, 0, 0 };
    static cilist io___35 = { 1, 0, 0, 0, 0 };
    static cilist io___36 = { 1, 0, 0, 0, 0 };
    static cilist io___46 = { 1, 0, 0, 0, 0 };
    static cilist io___47 = { 1, 0, 0, 0, 0 };
    static cilist io___48 = { 1, 0, 0, 0, 0 };
    static cilist io___49 = { 1, 0, 0, 0, 0 };
    static cilist io___50 = { 1, 0, 0, 0, 0 };
    static cilist io___51 = { 1, 0, 0, 0, 0 };
    static cilist io___52 = { 1, 0, 0, 0, 0 };
    static cilist io___53 = { 1, 0, 0, 0, 0 };
    static cilist io___55 = { 1, 0, 0, 0, 0 };
    static cilist io___56 = { 1, 0, 0, 0, 0 };
    static cilist io___57 = { 1, 0, 0, 0, 0 };
    static cilist io___58 = { 0, 0, 0, 0, 0 };
    static cilist io___59 = { 0, 0, 0, 0, 0 };


/* ************************************************************************ */
/*  SUBROUTINE WRGENS = WRITES GENESIS DATABASE MESH OUTPUT */
/* *********************************************************************** */
/*  IGUESS IS THE NUMBER OF ELEMENT BLOCKS,  FOR USE WITH THE ENAME */
/*  VARIABLE.  IF THIS VARIABLE IS NOT AS LARGE AS NUMMAT,  IT WILL NOT */
/*  RESULT IN A FATAL ERROR,  BUT SIMPLY A WARNING,  AND NO ELEMENT */
/*  NAMES WILL BE WRITTEN. */
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
    --wtnode;
    --nodes;
    --wtside;
    --nsiden;
    --nelems;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;

    /* Function Body */
    exdate_(date, (ftnlen)8);
    extime_(time, (ftnlen)8);
    s_copy(versn1, "        ", (ftnlen)8, (ftnlen)8);
    s_copy(versn2, "        ", (ftnlen)8, (ftnlen)8);
    s_copy(versn1, versn, (ftnlen)8, (ftnlen)5);
    s_copy(versn2, versn + 5, (ftnlen)8, (ftnlen)5);
    *err = TRUE_;
    s_copy(hold, title, (ftnlen)80, (ftnlen)72);
    s_copy(xname, "X", (ftnlen)8, (ftnlen)1);
    s_copy(yname, "Y", (ftnlen)8, (ftnlen)1);
/*  CHECK TO MAKE SURE THAT THERE IS ENOUGH ROOM FOR ELEMENT NAMES */
    if (*nummat > 1000) {
	mesage_("WARNING:  THE NUMBER OF ELEMENT BLOCKS EXCEEDS", (ftnlen)46);
	mesage_("          THE CAPACITY TO NAME EACH BLOCK.", (ftnlen)42);
	mesage_("          NO ELEMENT NAMES WILL BE WRITTEN.", (ftnlen)43);
    }
/*  WRITE OUT HEADER INFORMATION */
    io___10.ciunit = *iunit;
    i__1 = s_wsue(&io___10);
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, hold, (ftnlen)80);
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    ijk = 2;
    ivers = 1;
/* ... Fix up side set nodes for 8 and 9 node elements. */
/* ... At this point, they are treated as two linear segments, */
/* ... They should be ends followed by middle */
/*    1-----3-----2 Now: 1 3 3 2 Correct: 1 2 3 */
    if (*eight || *nine) {
	nvlst = *nvlist / 4 * 3;
	if (*nslist > 1) {
	    nslst = *nslist / 2;
	} else {
	    nslst = *nslist;
	}
    } else {
	nvlst = *nvlist;
	nslst = *nslist;
    }
    io___15.ciunit = *iunit;
    i__1 = s_wsue(&io___15);
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&(*nnn), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&ijk, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&(*kkk), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&(*nummat), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&(*nbcnod), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&(*nnlist), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&(*nbcsid), (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&nslst, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&nvlst, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&ivers, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
/*  WRITE OUT NODE BLOCK */
    io___16.ciunit = *iunit;
    i__1 = s_wsue(&io___16);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = *nnn;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&xn[i__], (ftnlen)sizeof(real));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__3 = *nnn;
    for (i__ = 1; i__ <= i__3; ++i__) {
	i__1 = do_uio(&c__1, (char *)&yn[i__], (ftnlen)sizeof(real));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    io___18.ciunit = *iunit;
    i__1 = s_wsue(&io___18);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = *kkk;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&mapdxg[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
/*  WRITE OUT ELEMENT BLOCKS */
    i__1 = *nummat;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nxk[matmap[i__ * 3 + 2] * nxk_dim1 + 3] == 0) {
	    inode = 2;
	    nattr = 1;
	    attr = (float)1.;
	    if (i__ <= 1000) {
		s_copy(ename + (i__ - 1 << 3), "BEAM", (ftnlen)8, (ftnlen)4);
	    }
	} else if (nxk[matmap[i__ * 3 + 2] * nxk_dim1 + 4] == 0) {
	    inode = 3;
	    nattr = 1;
	    attr = (float)1.;
	    if (i__ <= 1000) {
		s_copy(ename + (i__ - 1 << 3), "BEAM", (ftnlen)8, (ftnlen)4);
	    }
	    mesage_("NOTE:  The connectivity numbering for 3-node", (ftnlen)
		    44);
	    mesage_("       beams/trusses has been fixed to", (ftnlen)38);
	    mesage_("       conform to EXODUS convention (1-3-2)", (ftnlen)43)
		    ;
	} else if (*eight) {
	    inode = 8;
	    nattr = 0;
	    if (i__ <= 1000) {
		s_copy(ename + (i__ - 1 << 3), "QUAD", (ftnlen)8, (ftnlen)4);
	    }
	} else if (*nine) {
	    inode = 9;
	    nattr = 0;
	    if (i__ <= 1000) {
		s_copy(ename + (i__ - 1 << 3), "QUAD", (ftnlen)8, (ftnlen)4);
	    }
	} else {
	    inode = 4;
	    nattr = 0;
	    if (i__ <= 1000) {
		s_copy(ename + (i__ - 1 << 3), "QUAD", (ftnlen)8, (ftnlen)4);
	    }
	}
/*  NLOOP IS NEEDED TO WRITE SOMETHING OUT THE CURRENT COUNTER IS ZERO. */
/*  THIS IS DONE TO SOLVE A CRAY OPERATING SYSTEM [CTSS] PROBLEM */
/*  WHERE NULL RECORD WRITES ARE NOT DONE APPROPRIATELY */
	io___23.ciunit = *iunit;
	i__2 = s_wsue(&io___23);
	if (i__2 != 0) {
	    goto L110;
	}
	i__2 = do_uio(&c__1, (char *)&matmap[i__ * 3 + 1], (ftnlen)sizeof(
		integer));
	if (i__2 != 0) {
	    goto L110;
	}
	i__3 = matmap[i__ * 3 + 3] - matmap[i__ * 3 + 2] + 1;
	i__2 = do_uio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L110;
	}
	i__2 = do_uio(&c__1, (char *)&inode, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L110;
	}
	i__2 = do_uio(&c__1, (char *)&nattr, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L110;
	}
	i__2 = e_wsue();
	if (i__2 != 0) {
	    goto L110;
	}
/* ... 8 or 9 node quads */
	if (inode == 8 || inode == 9) {
	    io___24.ciunit = *iunit;
	    i__2 = s_wsue(&io___24);
	    if (i__2 != 0) {
		goto L110;
	    }
	    i__3 = matmap[i__ * 3 + 3];
	    for (k = matmap[i__ * 3 + 2]; k <= i__3; ++k) {
		i__4 = *nnxk;
		for (ii = 1; ii <= i__4; ++ii) {
		    i__2 = do_uio(&c__1, (char *)&nxk[lcon[ii - 1] + k * 
			    nxk_dim1], (ftnlen)sizeof(integer));
		    if (i__2 != 0) {
			goto L110;
		    }
		}
	    }
	    i__2 = e_wsue();
	    if (i__2 != 0) {
		goto L110;
	    }
/* ... 3 node beam/truss */
	} else if (inode == 3) {
	    io___27.ciunit = *iunit;
	    i__2 = s_wsue(&io___27);
	    if (i__2 != 0) {
		goto L110;
	    }
	    i__4 = matmap[i__ * 3 + 3];
	    for (k = matmap[i__ * 3 + 2]; k <= i__4; ++k) {
		i__3 = inode;
		for (ii = 1; ii <= i__3; ++ii) {
		    i__2 = do_uio(&c__1, (char *)&nxk[lbar[ii - 1] + k * 
			    nxk_dim1], (ftnlen)sizeof(integer));
		    if (i__2 != 0) {
			goto L110;
		    }
		}
	    }
	    i__2 = e_wsue();
	    if (i__2 != 0) {
		goto L110;
	    }
/* ... 4 node quad or 2 node beam/truss */
	} else {
	    io___28.ciunit = *iunit;
	    i__2 = s_wsue(&io___28);
	    if (i__2 != 0) {
		goto L110;
	    }
	    i__3 = matmap[i__ * 3 + 3];
	    for (k = matmap[i__ * 3 + 2]; k <= i__3; ++k) {
		i__4 = inode;
		for (j = 1; j <= i__4; ++j) {
		    i__2 = do_uio(&c__1, (char *)&nxk[j + k * nxk_dim1], (
			    ftnlen)sizeof(integer));
		    if (i__2 != 0) {
			goto L110;
		    }
		}
	    }
	    i__2 = e_wsue();
	    if (i__2 != 0) {
		goto L110;
	    }
	}
/* Computing MAX */
	i__2 = 1, i__4 = nattr * *kkk;
	nloop = max(i__2,i__4);
	io___31.ciunit = *iunit;
	i__2 = s_wsue(&io___31);
	if (i__2 != 0) {
	    goto L110;
	}
	i__4 = nloop;
	for (j = 1; j <= i__4; ++j) {
	    i__2 = do_uio(&c__1, (char *)&attr, (ftnlen)sizeof(real));
	    if (i__2 != 0) {
		goto L110;
	    }
	}
	i__2 = e_wsue();
	if (i__2 != 0) {
	    goto L110;
	}
/* L100: */
    }
/*  WRITE OUT NODAL BOUNDARY FLAGS */
    nloop = max(1,*nbcnod);
    io___32.ciunit = *iunit;
    i__1 = s_wsue(&io___32);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nnflg[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    io___33.ciunit = *iunit;
    i__1 = s_wsue(&io___33);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nnlen[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    io___34.ciunit = *iunit;
    i__1 = s_wsue(&io___34);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nnptr[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    nloop = max(1,*nnlist);
    io___35.ciunit = *iunit;
    i__1 = s_wsue(&io___35);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nodes[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    io___36.ciunit = *iunit;
    i__1 = s_wsue(&io___36);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&wtnode[i__], (ftnlen)sizeof(real));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
/*  WRITE OUT SIDE BOUNDARY FLAGS */
/* ... Fix up side set nodes and elements for 8 and 9 node elements. */
/* ... At this point, they are treated as two linear segments, */
/* ... They should be ends followed by middle */
/*    1-----3-----2 Now: 1 3 3 2 Correct: 1 2 3 */
    if (*eight || *nine) {
	ipn = 1;
	ipe = 1;
	i__1 = *nbcsid;
	for (ibc = 1; ibc <= i__1; ++ibc) {
	    ibee = nsptr[ibc];
	    iene = nsptr[ibc] + nslen[ibc] - 1;
	    nsptr[ibc] = ipe;
/* Computing MAX */
	    i__2 = 1, i__4 = nslen[ibc] / 2;
	    nslen[ibc] = max(i__2,i__4);
	    i__2 = iene;
	    for (iel = ibee; iel <= i__2; iel += 2) {
		nelems[ipe] = nelems[iel];
		++ipe;
/* L115: */
	    }
	    ibeg = nvptr[ibc];
	    iend = nvptr[ibc] + nvlen[ibc] - 1;
	    nvptr[ibc] = ipn;
	    nvlen[ibc] = nvlen[ibc] / 4 * 3;
	    i__2 = iend;
	    for (inod = ibeg; inod <= i__2; inod += 4) {
		nsiden[ipn] = nsiden[inod];
		nsiden[ipn + 2] = nsiden[inod + 1];
		nsiden[ipn + 1] = nsiden[inod + 3];
		wtside[ipn] = wtside[inod];
		wtside[ipn + 2] = wtside[inod + 1];
		wtside[ipn + 1] = wtside[inod + 3];
		ipn += 3;
/* L120: */
	    }
/* L130: */
	}
	*nvlist = ipn - 1;
	*nslist = ipe - 1;
    }
    nloop = max(1,*nbcsid);
    io___46.ciunit = *iunit;
    i__1 = s_wsue(&io___46);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nsflg[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    io___47.ciunit = *iunit;
    i__1 = s_wsue(&io___47);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nslen[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    io___48.ciunit = *iunit;
    i__1 = s_wsue(&io___48);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nvlen[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    io___49.ciunit = *iunit;
    i__1 = s_wsue(&io___49);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nsptr[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    io___50.ciunit = *iunit;
    i__1 = s_wsue(&io___50);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nvptr[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    nloop = max(1,*nslist);
    io___51.ciunit = *iunit;
    i__1 = s_wsue(&io___51);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nelems[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    nloop = max(1,*nvlist);
    io___52.ciunit = *iunit;
    i__1 = s_wsue(&io___52);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&nsiden[i__], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    io___53.ciunit = *iunit;
    i__1 = s_wsue(&io___53);
    if (i__1 != 0) {
	goto L110;
    }
    i__2 = nloop;
    for (i__ = 1; i__ <= i__2; ++i__) {
	i__1 = do_uio(&c__1, (char *)&wtside[i__], (ftnlen)sizeof(real));
	if (i__1 != 0) {
	    goto L110;
	}
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
/*  WRITE OUT THE QA INFORMATION */
    ihold = 1;
    io___55.ciunit = *iunit;
    i__1 = s_wsue(&io___55);
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&ihold, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
    io___56.ciunit = *iunit;
    i__1 = s_wsue(&io___56);
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, versn1, (ftnlen)8);
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, versn2, (ftnlen)8);
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, date, (ftnlen)8);
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, time, (ftnlen)8);
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
/*  WRITE THE HEADER INFORMATION */
    ihold = 0;
    io___57.ciunit = *iunit;
    i__1 = s_wsue(&io___57);
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = do_uio(&c__1, (char *)&ihold, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L110;
    }
    i__1 = e_wsue();
    if (i__1 != 0) {
	goto L110;
    }
/*  WRITE THE COORDINATE NAMES AND ELEMENT NAMES */
    io___58.ciunit = *iunit;
    s_wsue(&io___58);
    do_uio(&c__1, xname, (ftnlen)8);
    do_uio(&c__1, yname, (ftnlen)8);
    e_wsue();
    if (*nummat <= 1000) {
	io___59.ciunit = *iunit;
	s_wsue(&io___59);
	i__1 = *nummat;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_uio(&c__1, ename + (i__ - 1 << 3), (ftnlen)8);
	}
	e_wsue();
    }
/*  SUCCESSFUL WRITE COMPLETED */
    mesage_(" ", (ftnlen)1);
    mesage_(" ", (ftnlen)1);
    mesage_("GENESIS OUTPUT FILE SUCCESSFULLY WRITTEN", (ftnlen)40);
    mesage_(" ", (ftnlen)1);
    *err = FALSE_;
    return 0;
/*  ERR DURING WRITE PROBLEMS */
L110:
    mesage_("ERR DURING WRITE TO OUTPUT FILE", (ftnlen)31);
    mesage_("      - NO FILE SAVED -", (ftnlen)23);
    return 0;
} /* wrgens_ */

#ifdef __cplusplus
	}
#endif

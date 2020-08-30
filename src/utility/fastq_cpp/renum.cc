/* renum.f -- translated by f2c (version 20160102).
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

static integer c__9 = 9;
static integer c__1 = 1;
static integer c__3 = 3;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int renum_(integer *npnode, integer *npelem, integer *mxnflg,
	 integer *mxsflg, integer *npnbc, integer *npsbc, integer *npwts, 
	integer *npregn, integer *mp, integer *ml, integer *ms, integer *mr, 
	integer *msc, integer *maxkxn, integer *nnuid, integer *nnxk, integer 
	*mxlps, integer *iunit, integer *nnn, integer *kkk, integer *kcrd, 
	integer *nl, integer *npbf, integer *nlbf, integer *nsbf, integer *
	ipart, integer *lstnbc, integer *lstsbc, integer *nnflg, integer *
	nnptr, integer *nnlen, integer *nsflg, integer *nsptr, integer *nslen,
	 integer *nvptr, integer *nvlen, integer *nsiden, integer *nuid, real 
	*xn, real *yn, integer *nxk, integer *mat, integer *kxn, integer *
	list, integer *la, integer *lb, integer *matmap, integer *listn, real 
	*wtnode, real *wtside, real *wthold, integer *ihere, integer *ilist, 
	real *xlist, integer *nummat, integer *nbcnod, integer *nnlist, 
	integer *nbcsid, integer *nslist, integer *nvlist, real *coor, 
	integer *iline, integer *ltype, integer *lcon, integer *iside, 
	integer *nlps, integer *ifline, integer *illist, integer *linkp, 
	integer *linkl, integer *links, integer *linkr, integer *imat, 
	integer *linkb, integer *jmat, integer *ipbf, integer *nppf, integer *
	ifpb, integer *listpb, integer *iwtpbf, integer *ilbf, integer *nlpf, 
	integer *iflb, integer *listlb, integer *iwtlbf, integer *isbf, 
	integer *nspf, integer *ifsb, integer *listsb, integer *iwtsbf, 
	integer *linkpb, integer *linklb, integer *linksb, char *number, 
	logical *three, logical *eight, logical *nine, logical *optim, 
	logical *isbars, ftnlen number_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 **   LARGEST NODE DIFFERENCE PER ELEMEN\
T:\002,i6,\002  **\002)";
    static char fmt_10010[] = "(\002 ** NODES:\002,i6,\002; ELEMENTS:\002,\
i6,\002; MATERIALS:\002,i3,\002 **\002)";

    /* System generated locals */
    integer nxk_dim1, nxk_offset, kxn_dim1, kxn_offset, i__1, i__2, i__3, 
	    i__4, i__5;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(), s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), 
	    e_wsfe();

    /* Local variables */
    static integer i__, j, k, n, n1, n2, n3, n4, n5, n6, n7, n8, n9, ii, jj;
    static logical all;
    static integer ihi, nhi, ilo;
    static logical err;
    static integer new__, nlo, iend, node, kmat, nall, numa;
    extern integer indx_(integer *, integer *, integer *);
    static integer numb, jend, nold, lwid, numk, numl;
    extern /* Subroutine */ int sort_(integer *, integer *, integer *);
    static integer itry, numb1;
    static real dist1, dist2;
    static integer nodea, nodeb, nnnbc, nnsbc;
    extern /* Subroutine */ int addwt_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    real *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *);
    static integer kelem, klist[20], nlist[20];
    static logical itsok;
    static integer kount, kount2;
    extern /* Subroutine */ int kxnadd_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     midnod_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, integer *
	    , integer *, integer *, integer *, logical *, logical *, logical *
	    );
    static logical noroom;
    extern /* Subroutine */ int rdmesh_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *), nodord_(integer *, real *, real *, integer *, integer 
	    *, integer *), gnlist_(integer *, integer *, integer *, integer *,
	     integer *, integer *, integer *, integer *, integer *, integer *,
	     real *, real *, integer *, integer *, char *, integer *, integer 
	    *, logical *, logical *, ftnlen), getnxn_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, logical *);
    static integer numkxn;
    extern /* Subroutine */ int getkxn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     srtnbc_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *), srtsbc_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *), nxkbdy_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, logical *, logical *);
    static integer istart;

    /* Fortran I/O blocks */
    static cilist io___45 = { 0, 6, 0, 0, 0 };
    static cilist io___52 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___53 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE RENUM = NUMBERS QMESH OUTPUT, AND RENUMBERS AS NEEDED FOR */
/*                     OPTIMIZATION */
/* *********************************************************************** */
/*    THE REFERENCE DOCUMENTS FOR THIS CODE ARE SLA-73-1088, JULY 1974, */
/*    AND SLA-74-0239, JULY 1974 */
/* *********************************************************************** */
/*  HEADER */
    /* Parameter adjustments */
    --lb;
    --la;
    --yn;
    --xn;
    --mat;
    --nnlen;
    --nnptr;
    --nnflg;
    --nvlen;
    --nvptr;
    --nslen;
    --nsptr;
    --nsflg;
    --wtnode;
    --lstnbc;
    --wtside;
    --nsiden;
    --lstsbc;
    --wthold;
    matmap -= 4;
    ipart -= 4;
    linkpb -= 3;
    iwtpbf -= 4;
    listpb -= 3;
    --ifpb;
    --nppf;
    --ipbf;
    linkp -= 3;
    coor -= 3;
    linksb -= 3;
    linklb -= 3;
    iwtsbf -= 4;
    listsb -= 3;
    --ifsb;
    --nspf;
    --isbf;
    iwtlbf -= 4;
    listlb -= 3;
    --iflb;
    --nlpf;
    --ilbf;
    linkl -= 3;
    lcon -= 4;
    --ltype;
    --iline;
    --jmat;
    linkb -= 3;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --iside;
    --imat;
    linkr -= 3;
    number -= 80;
    --ihere;
    --listn;
    --list;
    --nuid;
    kxn_dim1 = *nnxk;
    kxn_offset = 1 + kxn_dim1;
    kxn -= kxn_offset;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;
    --xlist;
    --ilist;

    /* Function Body */
    mesage_(" ", (ftnlen)1);
    mesage_("NUMBERING OF GENERATED OUTPUT BEGUN", (ftnlen)35);
    if (*optim) {
	mesage_("  -- OPTIMIZATION IS ENABLED --", (ftnlen)31);
    }
/*  READ THE MESH TAPE */
    rdmesh_(npnode, npelem, npnbc, npsbc, npregn, ms, mr, nnuid, nnxk, iunit, 
	    nnn, kkk, &ipart[4], &lstnbc[1], &lstsbc[1], &nuid[1], &xn[1], &
	    yn[1], &nxk[nxk_offset], &mat[1], &matmap[4], nummat, &iside[1], &
	    nlps[1], &ifline[1], &illist[1], &links[3], &linkr[3], &imat[1], &
	    linkb[3], &jmat[1], &nnnbc, &nnsbc, &err);
    if (err) {
	mesage_("** NUMBERING ABORT **", (ftnlen)21);
	return 0;
    }
/*  SORT NODE LIST INTO INCREASING NUID-S */
    i__1 = *nnn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	listn[i__] = nuid[i__];
/* L100: */
    }
    if (*optim) {
	nodord_(npnode, &xn[1], &yn[1], &listn[1], &nuid[1], nnn);
    } else {
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    list[i__] = i__;
/* L110: */
	}
	sort_(nnn, &nuid[1], &list[1]);
    }
/*  CONVERT REFERENCES TO NUID-S TO REFERENCES TO */
/*  SEQUENCE NUMBERS */
    for (i__ = 1; i__ <= 4; ++i__) {
	i__1 = *kkk;
	for (k = 1; k <= i__1; ++k) {
	    if (nxk[i__ + k * nxk_dim1] > 0) {
		new__ = indx_(nnn, &nuid[1], &nxk[i__ + k * nxk_dim1]);
		if (new__ == 0) {
		    mesage_("ERROR SORTING NUMBERING DATA -- NODE", (ftnlen)
			    36);
		    mesage_("*** NO MESH SAVED ***", (ftnlen)21);
		    *kkk = 0;
		    return 0;
		}
		if (*optim) {
		    nxk[i__ + k * nxk_dim1] = new__;
		} else {
		    nxk[i__ + k * nxk_dim1] = list[new__];
		}
	    }
/* L120: */
	}
/* L130: */
    }
    if (nnnbc > 0) {
	i__1 = nnnbc;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (lstnbc[i__] > 0) {
		new__ = indx_(nnn, &nuid[1], &lstnbc[i__]);
		if (new__ == 0) {
		    mesage_("ERROR SORTING NUMBERING DATA -- NBC", (ftnlen)35)
			    ;
		    mesage_("*** NO MESH SAVED ***", (ftnlen)21);
		    *kkk = 0;
		    return 0;
		}
		if (*optim) {
		    lstnbc[i__] = new__;
		} else {
		    lstnbc[i__] = list[new__];
		}
	    }
/* L140: */
	}
    }
/*  BUILD KXN ARRAY */
    numkxn = *nnn;
    i__1 = *nnxk;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *maxkxn;
	for (j = 1; j <= i__2; ++j) {
	    kxn[i__ + j * kxn_dim1] = 0;
/* L150: */
	}
/* L160: */
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	i__1 = *kkk;
	for (k = 1; k <= i__1; ++k) {
	    if (nxk[i__ + k * nxk_dim1] > 0) {
		kxnadd_(maxkxn, nnxk, &kxn[kxn_offset], &numkxn, &k, &nxk[i__ 
			+ k * nxk_dim1], &err);
		if (err) {
		    mesage_("** NUMBERING ABORT **", (ftnlen)21);
		    *kkk = 0;
		    return 0;
		}
	    }
/* L170: */
	}
/* L180: */
    }
    if (*optim) {
/*  GET STARTING LIST FOR CUTHILL-MCKEE PROCESS */
	if (*kcrd > 0) {
	    gnlist_(npnode, nnuid, msc, npnode, npelem, maxkxn, nnxk, &kxn[
		    kxn_offset], &nxk[nxk_offset], &nuid[1], &xn[1], &yn[1], &
		    list[1], &numl, number + 80, kcrd, nnn, &err, &noroom, (
		    ftnlen)80);
	    if (noroom) {
		mesage_("TOO MANY NODES IN STARTING LIST", (ftnlen)31);
		mesage_("*** NO MESH SAVED ***", (ftnlen)21);
		mesage_("** NUMBERING ABORT **", (ftnlen)21);
		*kkk = 0;
		return 0;
	    } else if (err) {
		mesage_("ERROR GENERATING STARTING LIST", (ftnlen)30);
		mesage_("*** NO MESH SAVED ***", (ftnlen)21);
		mesage_("** NUMBERING ABORT **", (ftnlen)21);
		*kkk = 0;
		return 0;
	    }
	} else {
	    mesage_("NO CARDS AVAILABLE FOR STARTING LIST", (ftnlen)36);
	    mesage_("FIRST NODE IN THE OUTPUT USED", (ftnlen)29);
	    numl = 1;
	    list[1] = 1;
	}
/*  INITIALIZE LISTS */
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    listn[i__] = i__;
/* L190: */
	}
/*  USE LISTN AS A CHECK ON WHETHER THE NODE HAS BEEN USED (NEGATED) */
	i__1 = numl;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    la[i__] = list[i__];
	    node = list[i__];
	    listn[node] = -listn[node];
/* L200: */
	}
	numa = numl;
/*  CREATE LIST OF NEW NODES CONNECTED TO LIST A */
L210:
	numb = 0;
	i__1 = numa;
	for (n = 1; n <= i__1; ++n) {
	    all = TRUE_;
	    getnxn_(npnode, npelem, maxkxn, nnxk, &kxn[kxn_offset], &nxk[
		    nxk_offset], &listn[1], &la[n], nlist, &numb1, &all, &err)
		    ;
	    if (err) {
		mesage_("** NUMBERING ABORT **", (ftnlen)21);
		*kkk = 0;
		return 0;
	    }
	    if (numb1 > 0) {
		if (numb + numb1 > *npnode) {
		    mesage_("LIST B HAS OVERFLOWED", (ftnlen)21);
		    mesage_("*** NO MESH SAVED  ***", (ftnlen)22);
		    mesage_("** NUMBERING ABORT **", (ftnlen)21);
		    *kkk = 0;
		    return 0;
		}
		i__2 = numb1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    node = nlist[i__ - 1];
		    ++numb;
		    lb[numb] = node;
		    listn[node] = -listn[node];
/* L220: */
		}
	    }
/* L230: */
	}
	if (numb > 0) {
/*  INCLUDE LIST B INTO FULL LIST */
/*  ALSO TRANSFER LIST B TO LIST A */
	    i__1 = numb;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		++numl;
		list[numl] = lb[i__];
		la[i__] = lb[i__];
/* L240: */
	    }
	    numa = numb;
/*  CHECK FOR CONVERGENCE */
	    if (numl < *nnn) {
		goto L210;
	    }
/*  PROCESS HAS CONVERGED */
/*  CHECK IF ALL NODES WERE COVERED */
	} else if (numl < *nnn) {
	    i__1 = *nnn;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (listn[i__] > 0) {
/*  START THE LIST AGAIN WITH THE MISSED NODE */
		    mesage_("A DISCONTINUITY (SLIDE LINE) IN THE BODY HAS BE\
EN FOUND", (ftnlen)55);
		    mesage_("SEPARATE PART NUMBERING WILL ALSO BE OPTIMIZED", 
			    (ftnlen)46);
		    numa = 1;
		    la[1] = i__;
		    goto L210;
		}
/* L250: */
	    }
/*  DEFINITE ERROR IN THE NUMBERING PROCESS */
	    mesage_("ALL NODES COULD NOT BE FOUND TO NUMBER", (ftnlen)38);
	    mesage_("       *** NO MESH SAVED ***", (ftnlen)28);
	    mesage_("       ** NUMBERING ABORT **", (ftnlen)28);
	    *kkk = 0;
	    return 0;
	}
/*  PREPARE TO PUT NODE LIST INTO NEWLY DETERMINED ORDER */
/*  LISTN BECOMES THE POINTER FROM THE OLD NUMBER TO THE NEW */
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = list[i__];
	    listn[j] = i__;
/* L260: */
	}
/*  CONVERT NODE NUMBERS TO NEW NODE ORDER BY REDOING THE NXK ARRAY */
	for (i__ = 1; i__ <= 4; ++i__) {
	    i__1 = *kkk;
	    for (k = 1; k <= i__1; ++k) {
		j = nxk[i__ + k * nxk_dim1];
		if (j > 0) {
		    nxk[i__ + k * nxk_dim1] = listn[j];
		}
/* L270: */
	    }
/* L280: */
	}
	if (nnnbc > 0) {
	    i__1 = nnnbc;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (lstnbc[i__] > 0) {
		    j = lstnbc[i__];
		    lstnbc[i__] = listn[j];
		}
/* L290: */
	    }
	}
/*  PUT NODE LIST INTO NEW ORDER */
	nodord_(npnode, &xn[1], &yn[1], &listn[1], &nuid[1], nnn);
/*  REBUILD KXN ARRAY */
	numkxn = *nnn;
	i__1 = *nnxk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *maxkxn;
	    for (j = 1; j <= i__2; ++j) {
		kxn[i__ + j * kxn_dim1] = 0;
/* L300: */
	    }
/* L310: */
	}
	for (i__ = 1; i__ <= 4; ++i__) {
	    i__1 = *kkk;
	    for (k = 1; k <= i__1; ++k) {
		if (nxk[i__ + k * nxk_dim1] > 0) {
		    kxnadd_(maxkxn, nnxk, &kxn[kxn_offset], &numkxn, &k, &nxk[
			    i__ + k * nxk_dim1], &err);
		    if (err) {
			mesage_("** NUMBERING ABORT **", (ftnlen)21);
			*kkk = 0;
			return 0;
		    }
		}
/* L320: */
	    }
/* L330: */
	}
/*  PUT ELEMENT NUMBERING INTO NEW ORDER USING LA AS TEMPORARY STORAGE */
	i__1 = *kkk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    la[i__] = 0;
	    lb[i__] = 0;
/* L340: */
	}
	kount = 0;
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    getkxn_(npnode, maxkxn, nnxk, &kxn[kxn_offset], &nuid[1], &i__, 
		    klist, &numk, &err);
	    i__2 = numk;
	    for (j = 1; j <= i__2; ++j) {
		if (lb[klist[j - 1]] == 0) {
		    ++kount;
		    la[kount] = klist[j - 1];
		    lb[klist[j - 1]] = 1;
		}
/* L350: */
	    }
/* L360: */
	}
/*  END OF OPTIMIZATION */
    } else {
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nuid[i__] = listn[i__];
/* L370: */
	}
    }
/*  STICK LSTNBC INTO LISTN AS A WORK ARRAY FOR SORTING NODAL BOUNDARY */
/*  CONDITIONS LISTS */
    if (nnnbc > 0) {
	i__1 = nnnbc;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    listn[i__] = lstnbc[i__];
/* L380: */
	}
/*  SORT THROUGH LSTNBC AND RECREATE IT IN PLACE */
/*  USING LISTN AS THE ARRAY TO TAKE LSTNBC OVER */
/*  AND IHERE AS A WORK ARRAY */
/*  (LSTNBC NOW BECOMES THE NODES ARRAY FOR THE */
/*  GENESIS DATA BASE) */
	srtnbc_(mxnflg, npnbc, nnn, &nnflg[1], &nnlen[1], &nnptr[1], &lstnbc[
		1], &listn[1], &ihere[1], &nnnbc, nbcnod, nnlist);
    } else {
	*nnlist = 0;
	*nbcnod = 0;
    }
/*  SORT THROUGH LSTSBC AND RECREATE IT IN PLACE */
/*  USING LISTN AS THE ARRAY TO TAKE LSTSBC OVER */
/*  AND KXN AS A WORK ARRAY */
/*  (LSTSBC NOW BECOMES THE NELEMS ARRAY FOR THE */
/*  GENESIS DATA BASE) */
    if (nnsbc > 0) {
	i__1 = nnsbc;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    listn[i__] = lstsbc[i__];
/* L390: */
	}
	srtsbc_(mxsflg, npsbc, npelem, nnxk, &nxk[nxk_offset], &nsflg[1], &
		nslen[1], &nsptr[1], &nvlen[1], &nvptr[1], &listn[1], &lstsbc[
		1], &nsiden[1], &ihere[1], &nnsbc, nslist, nvlist, nbcsid);
    } else {
	*nbcsid = 0;
	*nslist = 0;
	*nvlist = 0;
    }
/*  PUT WEIGHTS ON FLAGGED NODES AS NEEDED */
/*  USE THE IHERE ARRAY AS A WORK ARRAY */
    addwt_(nnuid, nnxk, maxkxn, npnode, npelem, mxlps, mp, ml, ms, npnbc, 
	    npsbc, mxnflg, mxsflg, npwts, &coor[3], &iline[1], &ltype[1], &
	    lcon[4], &iside[1], &nlps[1], &ifline[1], &illist[1], &linkp[3], &
	    linkl[3], &links[3], &ipbf[1], &nppf[1], &ifpb[1], &listpb[3], &
	    iwtpbf[4], &ilbf[1], &nlpf[1], &iflb[1], &listlb[3], &iwtlbf[4], &
	    isbf[1], &nspf[1], &ifsb[1], &listsb[3], &iwtsbf[4], &linkpb[3], &
	    linklb[3], &linksb[3], &xn[1], &yn[1], &nuid[1], &nxk[nxk_offset],
	     &kxn[kxn_offset], &lstnbc[1], &nnflg[1], &nnptr[1], &nnlen[1], &
	    nsflg[1], &nvptr[1], &nvlen[1], &nsiden[1], &wtnode[1], &wtside[1]
	    , &wthold[1], nbcnod, nnlist, nbcsid, nslist, nvlist, &ilist[1], &
	    xlist[1]);
/*  SORT NUMBERS ACCORDING TO MATERIAL TYPE */
/*  USE KXN AS A WORK ARRAY */
    for (j = 1; j <= 4; ++j) {
	i__1 = *kkk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    kxn[j + i__ * kxn_dim1] = nxk[j + i__ * nxk_dim1];
/* L400: */
	}
/* L410: */
    }
/*  SET UP THE MATERIAL MAPPING ARRAY */
/*      MATMAP(1, I) = THE MATERIAL ID FOR THE I'TH BLOCK */
/*      MATMAP(2, I) = THE FIRST ELEMENT IN THE I'TH BLOCK */
/*      MATMAP(3, I) = THE LAST ELEMENT IN THE I'TH BLOCK */
    kount = 1;
    i__1 = *nummat;
    for (i__ = 1; i__ <= i__1; ++i__) {
	kmat = matmap[i__ * 3 + 1];
	matmap[i__ * 3 + 2] = kount;
	i__2 = *kkk;
	for (j = 1; j <= i__2; ++j) {
	    if (mat[j] == kmat) {
		listn[j] = kount;
		list[kount] = j;
		for (k = 1; k <= 4; ++k) {
		    nxk[k + kount * nxk_dim1] = kxn[k + j * kxn_dim1];
/* L420: */
		}
		++kount;
	    }
/* L430: */
	}
	matmap[i__ * 3 + 3] = kount - 1;
/* L440: */
    }
    if (kount - 1 != *kkk) {
	mesage_("ALL ELEMENTS DID NOT HAVE AN ELEMENT ID", (ftnlen)39);
	mesage_("MESH NUMBERING ABORTED", (ftnlen)22);
	*kkk = 0;
	return 0;
    }
/*  REDO THE REGION POINTER ARRAY */
    i__1 = *npregn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ipart[i__ * 3 + 2] = listn[ipart[i__ * 3 + 2]];
	ipart[i__ * 3 + 3] = listn[ipart[i__ * 3 + 3]];
/* L450: */
    }
/*  REDO THE MATERIAL ARRAY */
    i__1 = *nummat;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = matmap[i__ * 3 + 3];
	for (j = matmap[i__ * 3 + 2]; j <= i__2; ++j) {
	    mat[j] = matmap[i__ * 3 + 1];
/* L460: */
	}
/* L470: */
    }
/*  REDO THE MAPPING ARRAY IF OPTIMIZING RENUMBERING HAS BEEN DONE */
    if (*optim) {
	i__1 = *kkk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    la[i__] = listn[la[i__]];
/* L480: */
	}
    }
/*  REDO THE ELEMENT SIDE BOUNDARY LISTING WITH THE CURRENT ELEMENT NO. */
    i__1 = *nslist;
    for (i__ = 1; i__ <= i__1; ++i__) {
	lstsbc[i__] = listn[lstsbc[i__]];
/* L490: */
    }
/*  STORE THE LISTN POINTER SYSTEM FOR NOW */
    i__1 = *kkk;
    for (i__ = 1; i__ <= i__1; ++i__) {
	list[i__] = listn[i__];
/* L500: */
    }
/*  ADD THE MID-SIDE NODES IF EIGHT OR NINE NODE QUADS ARE WANTED */
/*  OR IF THREE NODE BARS ARE WANTED */
    if (*eight || *nine || *three) {
/*  FLAG ALL ELEMENT SIDES ONLY ONCE (NO SHARED SIDE FLAGGED) */
	i__1 = *nnxk * *maxkxn;
	nxkbdy_(&i__1, nnxk, npelem, &nxk[nxk_offset], kkk, &kxn[kxn_offset], 
		three, eight, nine);
/*  CREATE THE MIDSIDE NODES */
	midnod_(npnode, nnuid, npelem, nnxk, mp, ml, kkk, nnn, &nall, nl, &
		nxk[nxk_offset], &nuid[1], &xn[1], &yn[1], &listn[1], &coor[3]
		, &iline[1], &ltype[1], &lcon[4], &linkp[3], &linkl[3], three,
		 eight, nine);
/*  MODIFY THE IDENTIFIERS OF THE OLD NODES */
	i__1 = *nnn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    listn[i__] = i__ * 100000;
/* L510: */
	}
	*nnn = nall;
/*  ORDER THE EXPANDED NODE LIST */
	nodord_(npnode, &xn[1], &yn[1], &listn[1], &nuid[1], nnn);
/*  EXPAND THE CONNECTIVITY ARRAY TO INCLUDE THE MIDSIDE NODES */
/*  WHILE REPOSITIONING THE CORNER NODES INTO PROPER SEQUENCE */
	i__1 = *kkk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (*three && nxk[i__ * nxk_dim1 + 3] == 0) {
		itsok = TRUE_;
	    } else if (nxk[i__ * nxk_dim1 + 3] != 0 && (*eight || *nine)) {
		itsok = TRUE_;
	    } else {
		itsok = FALSE_;
	    }
	    if (itsok) {
		for (j = 4; j >= 1; --j) {
		    jj = j + 1;
		    if (jj == 5) {
			jj = 1;
		    }
		    nodea = (i__2 = nxk[j + i__ * nxk_dim1], abs(i__2));
		    nodeb = (i__2 = nxk[jj + i__ * nxk_dim1], abs(i__2));
/*  CHECK FOR 3 NODE BAR ELEMENTS */
		    if (nodea > 0 && nodeb > 0) {
			nxk[(j << 1) - 1 + i__ * nxk_dim1] = nodea * 100000;
			nlo = min(nodea,nodeb);
			nhi = max(nodea,nodeb);
			nxk[(j << 1) + i__ * nxk_dim1] = nlo * 100000 + nhi;
		    } else if (nodea > 0) {
			nxk[(j << 1) - 1 + i__ * nxk_dim1] = nodea * 100000;
		    }
/* L520: */
		}
	    } else {
		for (j = 1; j <= 4; ++j) {
		    nxk[j + i__ * nxk_dim1] = (i__2 = nxk[j + i__ * nxk_dim1],
			     abs(i__2)) * 100000;
/* L530: */
		}
	    }
/* L540: */
	}
/*  GET THE LIST OF NODAL BOUNDARY FLAGS EXTENDED AND THE NEW IDENTIFIERS */
/*  IN PLACE (USE NUID AND WTHOLD AS WORK ARRAYS) */
	if (*nbcnod > 0) {
	    kount = 0;
	    i__1 = *nbcnod;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		jend = nnlen[i__] + nnptr[i__] - 1;
		i__2 = jend;
		for (j = nnptr[i__]; j <= i__2; ++j) {
		    ++kount;
		    nuid[kount] = lstnbc[j] * 100000;
		    wthold[kount] = wtnode[j];
		    i__3 = jend;
		    for (k = j + 1; k <= i__3; ++k) {
/* Computing MIN */
			i__4 = lstnbc[j], i__5 = lstnbc[k];
			ilo = min(i__4,i__5);
/* Computing MAX */
			i__4 = lstnbc[j], i__5 = lstnbc[k];
			ihi = max(i__4,i__5);
			itry = ilo * 100000 + ihi;
			if (indx_(nnn, &listn[1], &itry) > 0) {
			    ++kount;
			    ++nnlen[i__];
			    nuid[kount] = itry;
			    wthold[kount] = (wtnode[j] + wtnode[k]) * (float)
				    .5;
			}
/* L550: */
		    }
/* L560: */
		}
		if (i__ != 1) {
		    nnptr[i__] = nnptr[i__ - 1] + nnlen[i__ - 1];
		}
/* L570: */
	    }
	    *nnlist = kount;
	    i__1 = *nnlist;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		lstnbc[i__] = nuid[i__];
		wtnode[i__] = wthold[i__];
/* L580: */
	    }
	}
/*  GET THE LIST OF SIDE BOUNDARY FLAGS EXTENDED AND THE NEW IDENTIFIERS */
/*  IN PLACE (USE NUID, WTHOLD, AND KXN AS WORK ARRAYS) */
	if (*nbcsid > 0) {
	    kount = 0;
	    kount2 = 0;
	    i__1 = *nbcsid;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		istart = nsptr[i__];
		iend = nsptr[i__] + nslen[i__] - 1;
		j = nvptr[i__];
		jend = nvptr[i__] + nvlen[i__] - 1;
		i__2 = iend;
		for (ii = istart; ii <= i__2; ++ii) {
		    kelem = lstsbc[ii];
		    ++kount;
		    kxn[kount * kxn_dim1 + 1] = kelem;
		    k = j + 1;
		    ++kount2;
		    nuid[kount2] = nsiden[j] * 100000;
		    wthold[kount2] = wtside[j];
/*  DO THE ADJUSTMENTS IF THE ELEMENT IS ONE THAT HAS BEEN EXPANDED */
		    if (*three && nxk[kelem * nxk_dim1 + 4] == 0 || (*eight ||
			     *nine) && nxk[kelem * nxk_dim1 + 4] != 0) {
			++kount;
			kxn[kount * kxn_dim1 + 1] = lstsbc[ii];
			++nslen[i__];
/* Computing MIN */
			i__3 = nsiden[j], i__4 = nsiden[k];
			ilo = min(i__3,i__4);
/* Computing MAX */
			i__3 = nsiden[j], i__4 = nsiden[k];
			ihi = max(i__3,i__4);
			itry = ilo * 100000 + ihi;
			++kount2;
			nuid[kount2] = itry;
			wthold[kount2] = (wtside[j] + wtside[k]) * (float).5;
			++kount2;
			nuid[kount2] = itry;
			wthold[kount2] = (wtside[j] + wtside[k]) * (float).5;
			nvlen[i__] += 2;
		    }
		    ++kount2;
		    nuid[kount2] = nsiden[k] * 100000;
		    wthold[kount2] = wtside[k];
		    j += 2;
/* L590: */
		}
		if (i__ > 1) {
		    nsptr[i__] = nsptr[i__ - 1] + nslen[i__ - 1];
		    nvptr[i__] = nvptr[i__ - 1] + nvlen[i__ - 1];
		}
/* L600: */
	    }
/*  TRANSFER THE ELEMENT BOUNDARIES BACK FROM THE WORK ARRAYS */
	    *nslist = kount;
	    i__1 = *nslist;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		lstsbc[i__] = kxn[i__ * kxn_dim1 + 1];
/* L610: */
	    }
	    *nvlist = kount2;
	    i__1 = *nvlist;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		nsiden[i__] = nuid[i__];
		wtside[i__] = wthold[i__];
/* L620: */
	    }
	}
/*  ADD A CENTER NODE TO THE NODE LIST IF NEEDED */
	if (*nine) {
	    nold = *nnn;
	    i__1 = *kkk;
	    for (i__ = 1; i__ <= i__1; ++i__) {
/*  WATCH OUT FOR 3 NODE BAR ELEMENTS */
		if (! (*isbars) || nxk[i__ * nxk_dim1 + 4] > 0) {
		    n2 = indx_(&nold, &listn[1], &nxk[i__ * nxk_dim1 + 2]);
		    n4 = indx_(&nold, &listn[1], &nxk[i__ * nxk_dim1 + 4]);
		    n6 = indx_(&nold, &listn[1], &nxk[i__ * nxk_dim1 + 6]);
		    n8 = indx_(&nold, &listn[1], &nxk[i__ * nxk_dim1 + 8]);
		    if (n2 == 0 || n4 == 0 || n6 == 0 || n8 == 0) {
			mesage_("BAD LINK IN RENUM AT 8 NODE LIST", (ftnlen)
				32);
			mesage_("NO MESH SAVED", (ftnlen)13);
			*kkk = 0;
			return 0;
		    }
/* Computing 2nd power */
		    r__1 = xn[n2] - xn[n6];
/* Computing 2nd power */
		    r__2 = yn[n2] - yn[n6];
		    dist1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
		    r__1 = xn[n8] - xn[n4];
/* Computing 2nd power */
		    r__2 = yn[n8] - yn[n4];
		    dist2 = sqrt(r__1 * r__1 + r__2 * r__2);
		    ++(*nnn);
		    if (dist1 < dist2) {
			xn[*nnn] = (xn[n2] + xn[n6]) * (float).5;
			yn[*nnn] = (yn[n2] + yn[n6]) * (float).5;
		    } else {
			xn[*nnn] = (xn[n4] + xn[n8]) * (float).5;
			yn[*nnn] = (yn[n4] + yn[n8]) * (float).5;
		    }
/* Computing MIN */
		    i__2 = nxk[i__ * nxk_dim1 + 1], i__3 = nxk[i__ * nxk_dim1 
			    + 5];
/* Computing MAX */
		    i__4 = nxk[i__ * nxk_dim1 + 1], i__5 = nxk[i__ * nxk_dim1 
			    + 5];
		    listn[*nnn] = (integer) (min(i__2,i__3) + max(i__4,i__5) *
			     (float)1e-4);
		    nxk[i__ * nxk_dim1 + 9] = listn[*nnn];
		} else {
		    s_wsle(&io___45);
		    do_lio(&c__9, &c__1, "Element ", (ftnlen)8);
		    do_lio(&c__3, &c__1, (char *)&i__, (ftnlen)sizeof(integer)
			    );
		    do_lio(&c__9, &c__1, " is a 3-node bar?", (ftnlen)17);
		    e_wsle();
		}
/* L630: */
	    }
/*  NOW, ORDER THE EXPANDED NODE LIST AGAIN */
	    nodord_(npnode, &xn[1], &yn[1], &listn[1], &nuid[1], nnn);
	    iend = 9;
	} else if (*eight) {
	    iend = 8;
	} else {
	    iend = 4;
	}
/*  NOW REPLACE THE NODE REFERENCES WITH AN EXPANDED ORDER NUMBER */
/*  FIRST FIX THE CONNECTIVITY (NXK ARRAY) */
	i__1 = iend;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *kkk;
	    for (k = 1; k <= i__2; ++k) {
/*  AGAIN, WATCH OUT FOR 3 NODE BAR ELEMENTS */
		if (! (*isbars) || nxk[i__ + k * nxk_dim1] > 0) {
		    new__ = indx_(nnn, &listn[1], &nxk[i__ + k * nxk_dim1]);
		    if (new__ == 0) {
			mesage_("BAD LINK IN RENUM 8 NODE LIST", (ftnlen)29);
			mesage_("NO MESH SAVED", (ftnlen)13);
			*kkk = 0;
			return 0;
		    }
		    nxk[i__ + k * nxk_dim1] = new__;
		}
/* L640: */
	    }
/* L650: */
	}
/*  NOW FIX THE NODE BOUNDARY FLAGS */
	if (*nbcnod > 0) {
	    i__1 = *nnlist;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		new__ = indx_(nnn, &listn[1], &lstnbc[i__]);
		if (new__ == 0) {
		    mesage_("BAD LINK IN RENUM AT 8 NODE NBC", (ftnlen)31);
		    mesage_("NO MESH SAVED", (ftnlen)13);
		    *kkk = 0;
		    return 0;
		}
		lstnbc[i__] = new__;
/* L660: */
	    }
	}
/*  NOW FIX THE SIDE BOUNDARY FLAGS */
	if (*nbcsid > 0) {
	    i__1 = *nvlist;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		new__ = indx_(nnn, &listn[1], &nsiden[i__]);
		if (new__ == 0) {
		    mesage_("BAD LINK IN RENUM AT 8 NODE SBC", (ftnlen)31);
		    mesage_("NO MESH SAVED", (ftnlen)13);
		    *kkk = 0;
		    return 0;
		}
		nsiden[i__] = new__;
/* L670: */
	    }
	}
    }
/*  RENUMBERING COMPLETED */
    mesage_(" ", (ftnlen)1);
    mesage_("**************************************************", (ftnlen)50);
    mesage_("**           MESH PROCESSING COMPLETED          **", (ftnlen)50);
    if (*isbars && *nine) {
	mesage_("**            THREE NODE BARS OUTPUT            **", (ftnlen)
		50);
    }
    if (*nine) {
	mesage_("**            NINE NODE QUADS OUTPUT            **", (ftnlen)
		50);
    } else if (*eight) {
	mesage_("**           EIGHT NODE QUADS OUTPUT            **", (ftnlen)
		50);
    }
    if (*optim) {
	mesage_("**   WITH NODE AND ELEMENT NUMBERING OPTIMIZED  **", (ftnlen)
		50);
/*  FIND LARGEST NODE DIFFERENCE FOR AN ELEMENT */
	lwid = 0;
	i__1 = *kkk;
	for (k = 1; k <= i__1; ++k) {
	    n1 = nxk[k * nxk_dim1 + 1];
	    n2 = nxk[k * nxk_dim1 + 2];
	    n3 = nxk[k * nxk_dim1 + 3];
	    n4 = nxk[k * nxk_dim1 + 4];
	    if (n4 > 0 && (*eight || *nine)) {
		n5 = nxk[k * nxk_dim1 + 5];
		n6 = nxk[k * nxk_dim1 + 6];
		n7 = nxk[k * nxk_dim1 + 7];
		n8 = nxk[k * nxk_dim1 + 8];
		if (*nine) {
		    n9 = nxk[k * nxk_dim1 + 9];
/* Computing MIN */
		    i__2 = min(n1,n2), i__2 = min(i__2,n3), i__2 = min(i__2,
			    n4), i__2 = min(i__2,n5), i__2 = min(i__2,n6), 
			    i__2 = min(i__2,n7), i__2 = min(i__2,n8);
		    nlo = min(i__2,n9);
/* Computing MAX */
		    i__2 = max(n1,n2), i__2 = max(i__2,n3), i__2 = max(i__2,
			    n4), i__2 = max(i__2,n5), i__2 = max(i__2,n6), 
			    i__2 = max(i__2,n7), i__2 = max(i__2,n8);
		    nhi = max(i__2,n9);
		} else {
/* Computing MIN */
		    i__2 = min(n1,n2), i__2 = min(i__2,n3), i__2 = min(i__2,
			    n4), i__2 = min(i__2,n5), i__2 = min(i__2,n6), 
			    i__2 = min(i__2,n7);
		    nlo = min(i__2,n8);
/* Computing MAX */
		    i__2 = max(n1,n2), i__2 = max(i__2,n3), i__2 = max(i__2,
			    n4), i__2 = max(i__2,n5), i__2 = max(i__2,n6), 
			    i__2 = max(i__2,n7);
		    nhi = max(i__2,n8);
		}
	    } else {
		if (n3 <= 0) {
		    n3 = n1;
		}
		if (n4 <= 0) {
		    n4 = n2;
		}
/* Computing MIN */
		i__2 = min(n1,n2), i__2 = min(i__2,n3);
		nlo = min(i__2,n4);
/* Computing MAX */
		i__2 = max(n1,n2), i__2 = max(i__2,n3);
		nhi = max(i__2,n4);
	    }
/* Computing MAX */
	    i__2 = lwid, i__3 = nhi - nlo;
	    lwid = max(i__2,i__3);
/* L680: */
	}
	s_wsfe(&io___52);
	do_fio(&c__1, (char *)&lwid, (ftnlen)sizeof(integer));
	e_wsfe();
    }
    s_wsfe(&io___53);
    do_fio(&c__1, (char *)&(*nnn), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*kkk), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*nummat), (ftnlen)sizeof(integer));
    e_wsfe();
    mesage_("**************************************************", (ftnlen)50);
/*  RESTORE THE NUID ARRAY AS A POINTER ARRAY OF OLD TO NEW ELEMENTS */
/*  (MAPDXG ARRAY) */
    if (*optim) {
	i__1 = *kkk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nuid[i__] = la[i__];
	    list[nuid[i__]] = i__;
/* L690: */
	}
    } else {
	i__1 = *kkk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nuid[i__] = i__;
	    list[i__] = i__;
/* L700: */
	}
    }
    return 0;
} /* renum_ */

#ifdef __cplusplus
	}
#endif

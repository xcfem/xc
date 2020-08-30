/* addwt.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int addwt_(integer *nnuid, integer *nnxk, integer *maxkxn, 
	integer *npnode, integer *npelem, integer *mxlps, integer *mp, 
	integer *ml, integer *ms, integer *npnbc, integer *npsbc, integer *
	mxnflg, integer *mxsflg, integer *npwts, real *coor, integer *iline, 
	integer *ltype, integer *lcon, integer *iside, integer *nlps, integer 
	*ifline, integer *illist, integer *linkp, integer *linkl, integer *
	links, integer *ipbf, integer *nppf, integer *ifpb, integer *listpb, 
	integer *iwtpbf, integer *ilbf, integer *nlpf, integer *iflb, integer 
	*listlb, integer *iwtlbf, integer *isbf, integer *nspf, integer *ifsb,
	 integer *listsb, integer *iwtsbf, integer *linkpb, integer *linklb, 
	integer *linksb, real *xn, real *yn, integer *nuid, integer *nxk, 
	integer *kxn, integer *lstnbc, integer *nnflg, integer *nnptr, 
	integer *nnlen, integer *nsflg, integer *nvptr, integer *nvlen, 
	integer *nsiden, real *wtnode, real *wtside, real *wthold, integer *
	nbcnod, integer *nnlist, integer *nbcsid, integer *nslist, integer *
	nvlist, integer *ilist, real *xlist)
{
    /* Format strings */
    static char fmt_10000[] = "(/,\002 WEIGHTING BEGUN FOR NODAL FLAG\002,i5)"
	    ;
    static char fmt_10010[] = "(\002 NO WEIGHTING POSSIBLE FOR NODAL FLAG\
\002,i5)";
    static char fmt_10020[] = "(/,\002 WEIGHTING BEGUN FOR ELEMENT FLAG\002,\
i5)";
    static char fmt_10030[] = "(\002 NO WEIGHTING POSSIBLE FOR ELEMENT FLA\
G\002,i5)";

    /* System generated locals */
    integer nxk_dim1, nxk_offset, kxn_dim1, kxn_offset, i__1, i__2, i__3;
    real r__1, r__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, j, j1, j2, ji;
    static logical err;
    static integer nix, node, iloc, jloc, line1, jside;
    extern /* Subroutine */ int chkwt_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, logical *, logical *, logical *), getwt_(
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, real *, integer *, real *, 
	    logical *, logical *);
    static integer ipntr;
    static logical ispnt;
    static real acclen;
    static logical addlnk;
    extern /* Subroutine */ int bfnode_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *);
    static integer newnod, ipoint, jpoint;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___11 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___17 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___19 = { 0, 6, 0, fmt_10020, 0 };
    static cilist io___22 = { 0, 6, 0, fmt_10030, 0 };


/* *********************************************************************** */
/*  SUBROUTINE ADDWT = ADDS THE WEIGHTING FACTORS TO ANY NODES WITH */
/*                     FLAGS CONTAINING WEIGHTS */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     RENUM = NUMBERS QMESH OUTPUT,  AND RENUMBERS AS NEEDED FOR */
/*             OPTIMIZATION */
/* *********************************************************************** */
    /* Parameter adjustments */
    --nuid;
    kxn_dim1 = *nnxk;
    kxn_offset = 1 + kxn_dim1;
    kxn -= kxn_offset;
    --yn;
    --xn;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;
    --xlist;
    --ilist;
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
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --iside;
    --wtnode;
    --lstnbc;
    --wtside;
    --nsiden;
    --nnlen;
    --nnptr;
    --nnflg;
    --nvlen;
    --nvptr;
    --nsflg;
    --wthold;

    /* Function Body */
    addlnk = FALSE_;
    ipntr = 0;
/* FIRST FLAG ALL WEIGHT ARRAYS TO -1.0 TO KNOW WHICH REMAIN DEFAULTED */
    i__1 = *nnlist;
    for (i__ = 1; i__ <= i__1; ++i__) {
	wtnode[i__] = (float)-1.;
	wthold[i__] = (float)-1.;
/* L100: */
    }
    i__1 = *nvlist;
    for (i__ = 1; i__ <= i__1; ++i__) {
	wtside[i__] = (float)-1.;
/* L110: */
    }
/*  NOW CHECK ALL POINT FLAGS FOR WEIGHTS AND APPLY THE POINT */
/*  Y VALUE AS THE WEIGHT FOR THE NODE AT THE BEGINNING POINT */
    ispnt = TRUE_;
    i__1 = *nbcnod;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mp, &linkpb[3], &nnflg[i__], &ipntr, &addlnk);
	if (ipntr > 0) {
	    if (iwtpbf[ipntr * 3 + 1] > 0) {
		ipoint = iwtpbf[ipntr * 3 + 1];
		jpoint = iwtpbf[ipntr * 3 + 2];
		chkwt_(mp, ml, ms, nnlist, nbcnod, nnuid, mxlps, &linkp[3], &
			linkl[3], &links[3], &nuid[1], &nnflg[1], &nnlen[1], &
			nnptr[1], &lstnbc[1], &ipbf[ipntr], &ltype[1], &lcon[
			4], &nlps[1], &ifline[1], &illist[1], &coor[3], &
			jpoint, &ipoint, &iloc, &jloc, &nix, &ilist[1], &
			xlist[1], &addlnk, &ispnt, &err);
		if (err) {
		    wtnode[jloc] = (float)1.;
		} else {
		    ltsort_(mp, &linkp[3], &ipoint, &ipntr, &addlnk);
		    wtnode[jloc] = coor[(ipntr << 1) + 2];
		    wthold[jloc] = (float)1.;
		}
	    }
	}
/* L120: */
    }
    ispnt = FALSE_;
/*  NOW CHECK ALL LINE FLAGS FOR WEIGHTS AND APPLY THE APPROPRIATE */
/*  WEIGHT ALL ALONG CONTINUOUS NODES ON THE BOUNDARY. */
    i__1 = *nbcnod;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(ml, &linklb[3], &nnflg[i__], &ipntr, &addlnk);
	if (ipntr > 0) {
	    if (iwtlbf[ipntr * 3 + 1] != 0) {
		s_wsfe(&io___11);
		do_fio(&c__1, (char *)&nnflg[i__], (ftnlen)sizeof(integer));
		e_wsfe();
		jside = iwtlbf[ipntr * 3 + 1];
		jpoint = iwtlbf[ipntr * 3 + 2];
		chkwt_(mp, ml, ms, nnlist, nbcnod, nnuid, mxlps, &linkp[3], &
			linkl[3], &links[3], &nuid[1], &nnflg[1], &nnlen[1], &
			nnptr[1], &lstnbc[1], &ilbf[ipntr], &ltype[1], &lcon[
			4], &nlps[1], &ifline[1], &illist[1], &coor[3], &
			jpoint, &jside, &iloc, &jloc, &nix, &ilist[1], &xlist[
			1], &addlnk, &ispnt, &err);
		if (! err) {
/*  LOOP UNTIL ALL THE NODES HAVE BEEN FOUND, */
/*  FIRST PUTTING THE ACCUMULATED LENGTH IN THE WTNODE ARRAY */
		    newnod = 0;
		    acclen = (float)0.;
		    node = lstnbc[jloc];
		    if (iwtlbf[ipntr * 3 + 3] != 0) {
			line1 = iwtlbf[ipntr * 3 + 3];
		    } else {
			line1 = listlb[(iflb[ipntr] << 1) + 1];
		    }
L130:
		    wtnode[jloc] = acclen;
		    bfnode_(&nnlen[iloc], nnxk, npnode, npelem, maxkxn, nnuid,
			     &node, &newnod, &lstnbc[nnptr[iloc]], &kxn[
			    kxn_offset], &nxk[nxk_offset], &nuid[1], &jloc, &
			    line1, &err);
		    if (err) {
			s_wsfe(&io___17);
			do_fio(&c__1, (char *)&nnflg[ipntr], (ftnlen)sizeof(
				integer));
			e_wsfe();
			goto L150;
		    }
		    jloc = jloc + nnptr[iloc] - 1;
		    if (newnod > 0) {
/* Computing 2nd power */
			r__1 = xn[node] - xn[newnod];
/* Computing 2nd power */
			r__2 = yn[node] - yn[newnod];
			acclen += sqrt(r__1 * r__1 + r__2 * r__2);
			nuid[node] = -(i__2 = nuid[node], abs(i__2));
			node = newnod;
			newnod = 0;
			goto L130;
		    }
/*  NOW CHANGE THE ACCUMULATED LENGTH TO A PERCENTAGE LENGTH */
/*  AND GET THE WEIGHTING FUNCTION */
		    i__2 = nnptr[iloc] + nnlen[iloc] - 1;
		    for (j = nnptr[iloc]; j <= i__2; ++j) {
			if (wtnode[j] >= (float)0. && acclen != (float)0.) {
			    wthold[j] = (float)1.;
			    wtnode[j] /= acclen;
			    getwt_(mp, ml, mxlps, &nix, &ilist[1], &xlist[1], 
				    &iline[1], &lcon[4], &ltype[1], &coor[3], 
				    &linkp[3], &wtnode[j], &addlnk, &err);
			    if (err) {
				goto L150;
			    }
			}
/* L140: */
		    }
		}
	    }
	}
L150:
/* L160: */
	;
    }
/*  NOW RESET NUIDS AND PUT ALL DEFAULTS TO 1.0 */
    i__1 = *nnlist;
    for (i__ = 1; i__ <= i__1; ++i__) {
	nuid[lstnbc[i__]] = (i__2 = nuid[lstnbc[i__]], abs(i__2));
	if (wthold[i__] < (float)0.) {
	    wtnode[i__] = (float)1.;
	}
/* L170: */
    }
/*  NOW CHECK ALL SIDE FLAGS FOR WEIGHTS AND APPLY THE APPROPRIATE */
/*  WEIGHT ALL ALONG CONTINUOUS NODES ON THE BOUNDARY. */
    i__1 = *nbcsid;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mp, &linksb[3], &nsflg[i__], &ipntr, &addlnk);
	if (ipntr > 0) {
	    if (iwtsbf[ipntr * 3 + 1] != 0) {
		s_wsfe(&io___19);
		do_fio(&c__1, (char *)&nsflg[i__], (ftnlen)sizeof(integer));
		e_wsfe();
		jside = iwtsbf[ipntr * 3 + 1];
		jpoint = iwtsbf[ipntr * 3 + 2];
		chkwt_(mp, ml, ms, nvlist, nbcsid, nnuid, mxlps, &linkp[3], &
			linkl[3], &links[3], &nuid[1], &nsflg[1], &nvlen[1], &
			nvptr[1], &nsiden[1], &isbf[ipntr], &ltype[1], &lcon[
			4], &nlps[1], &ifline[1], &illist[1], &coor[3], &
			jpoint, &jside, &iloc, &jloc, &nix, &ilist[1], &xlist[
			1], &addlnk, &ispnt, &err);
		if (err) {
		    i__2 = nvptr[ipntr] + nvlen[ipntr] + 1;
		    for (j = nvptr[ipntr]; j <= i__2; ++j) {
			wtside[j] = (float)1.;
/* L180: */
		    }
		} else {
/*  LOOP UNTIL ALL THE NODES HAVE BEEN FOUND, */
/*  FIRST PUTTING THE ACCUMULATED LENGTH IN THE WTSIDE ARRAY */
		    newnod = 0;
		    j1 = nvptr[iloc];
		    j2 = nvptr[iloc] + nvlen[iloc] - 1;
		    acclen = (float)0.;
		    node = nsiden[jloc];
		    if (iwtsbf[ipntr * 3 + 3] != 0) {
			line1 = iwtsbf[ipntr * 3 + 3];
		    } else {
			line1 = listlb[(ifsb[ipntr] << 1) + 1];
		    }
L190:
/*  PUT THIS ACCLEN FOR ALL OCCURRENCES OF NODE IN THE LIST */
		    i__2 = j2;
		    for (j = j1; j <= i__2; ++j) {
			if (nsiden[j] == node) {
			    wtside[j] = acclen;
			}
/* L200: */
		    }
		    bfnode_(&nvlen[iloc], nnxk, npnode, npelem, maxkxn, nnuid,
			     &node, &newnod, &nsiden[nvptr[iloc]], &kxn[
			    kxn_offset], &nxk[nxk_offset], &nuid[1], &jloc, &
			    line1, &err);
		    if (err) {
			i__2 = nvptr[ipntr] + nvlen[ipntr] + 1;
			for (j = nvptr[ipntr]; j <= i__2; ++j) {
			    wtside[j] = (float)1.;
/* L210: */
			}
			s_wsfe(&io___22);
			do_fio(&c__1, (char *)&nsflg[ipntr], (ftnlen)sizeof(
				integer));
			e_wsfe();
			goto L250;
		    }
		    jloc = jloc + nnptr[iloc] - 1;
		    if (newnod > 0) {
/* Computing 2nd power */
			r__1 = xn[node] - xn[newnod];
/* Computing 2nd power */
			r__2 = yn[node] - yn[newnod];
			acclen += sqrt(r__1 * r__1 + r__2 * r__2);
			nuid[node] = -(i__2 = nuid[node], abs(i__2));
			node = newnod;
			newnod = 0;
			goto L190;
		    }
/*  NOW CHANGE THE ACCUMULATED LENGTH TO A PERCENTAGE LENGTH */
/*  AND GET THE WEIGHTING FUNCTION */
		    i__2 = nvptr[iloc] + nvlen[iloc] - 1;
		    for (j = nvptr[iloc]; j <= i__2; ++j) {
			if (wtside[j] >= (float)0. && acclen != (float)0.) {
			    wtside[j] /= acclen;
			    getwt_(mp, ml, mxlps, &nix, &ilist[1], &xlist[1], 
				    &iline[1], &lcon[4], &ltype[1], &coor[3], 
				    &linkp[3], &wtside[j], &addlnk, &err);
			    if (err) {
				goto L250;
			    }
			} else {
			    wtside[j] = (float)0.;
			}
/* L220: */
		    }
/*  NOW RESET NUIDS FROM THIS SIDE SET */
		    i__2 = *nvlist;
		    for (ji = 1; ji <= i__2; ++ji) {
			nuid[nsiden[ji]] = (i__3 = nuid[nsiden[ji]], abs(i__3)
				);
/* L230: */
		    }
		}
	    } else {
		i__2 = nvptr[i__] + nvlen[i__] + 1;
		for (j = nvptr[i__]; j <= i__2; ++j) {
		    wtside[j] = (float)1.;
/* L240: */
		}
	    }
	}
L250:
/* L260: */
	;
    }
/*  NOW RESET NUIDS */
    i__1 = *nvlist;
    for (i__ = 1; i__ <= i__1; ++i__) {
	nuid[nsiden[i__]] = (i__2 = nuid[nsiden[i__]], abs(i__2));
/* L270: */
    }
    return 0;
} /* addwt_ */

#ifdef __cplusplus
	}
#endif

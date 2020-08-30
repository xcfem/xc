/* chkhol.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int chkhol_(integer *ia, integer *l, integer *mp, integer *
	ml, integer *ms, integer *mr, integer *msc, integer *ipoint, real *
	coor, integer *ipboun, integer *iline, integer *ltype, integer *nint, 
	real *factor, integer *lcon, integer *ilboun, integer *isboun, 
	integer *iside, integer *nlps, integer *ifline, integer *illist, 
	integer *iregn, integer *nspr, integer *ifside, integer *islist, 
	integer *nppf, integer *ifpb, integer *listpb, integer *nlpf, integer 
	*iflb, integer *listlb, integer *nspf, integer *ifsb, integer *listsb,
	 integer *ifhole, integer *nhpr, integer *ihlist, integer *linkp, 
	integer *linkl, integer *links, integer *linkr, integer *linksc, 
	integer *linkpb, integer *linklb, integer *linksb, real *rsize, 
	integer *npregn, integer *npsbc, integer *npnode, integer *maxnp, 
	integer *maxnl, integer *mxnper, integer *mxrnbc, integer *mxrsbc, 
	real *x, real *y, integer *nid, integer *listl, integer *marked, 
	integer *mxnl, integer *maxnbc, integer *maxsbc, real *amesur, real *
	xnold, real *ynold, integer *nxkold, integer *mmpold, integer *linkeg,
	 integer *listeg, real *bmesur, integer *mlink, integer *nprold, 
	integer *npnold, integer *npeold, integer *nnxk, logical *remesh, 
	real *rexmin, real *rexmax, real *reymin, real *reymax, integer *
	idivis, real *sizmin, real *emax, real *emin, logical *noroom, 
	logical *errchk, logical *err)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 ** ERROR - DATA PROBLEMS FOR HOLE REGIO\
N:\002,i5,\002 **\002)";
    static char fmt_10010[] = "(\002 ** ERROR - PERIMETER GENERATION ERRORS \
FOR HOLE REGION:\002,i5,\002 **\002)";
    static char fmt_10020[] = "(\002 ** ERROR - HOLE REGION\002,i5,\002 DOES\
 NOT EXIST **\002)";

    /* System generated locals */
    integer nxkold_dim1, nxkold_offset, i__1, i__2, i__3;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, ll, nl;
    static logical ccw;
    static integer knbc, ksbc;
    static logical real__, even;
    static integer nper;
    extern /* Subroutine */ int perim_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, logical *, 
	    logical *, logical *, logical *, logical *, real *, real *, real *
	    , integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, real *, 
	    real *, real *, real *, integer *, real *, real *, real *);
    static logical count;
    static integer ipntr1;
    static logical addlnk;
    extern /* Subroutine */ int dataok_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    logical *, logical *);
    static integer idummy[1], iminus;
    extern /* Subroutine */ int mkused_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *), ltsort_(integer *, integer *, integer *, integer *, 
	    logical *);

    /* Fortran I/O blocks */
    static cilist io___8 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___17 = { 0, 6, 0, fmt_10020, 0 };


/* *********************************************************************** */
/*  CHKRGN - CHECK THAT A REGION MAY BE MESHED */
/* *********************************************************************** */
    /* Parameter adjustments */
    --ia;
    linkpb -= 3;
    linkp -= 3;
    listpb -= 3;
    --ifpb;
    --nppf;
    --ipboun;
    coor -= 3;
    --ipoint;
    linksb -= 3;
    linklb -= 3;
    linkl -= 3;
    listsb -= 3;
    --ifsb;
    --nspf;
    listlb -= 3;
    --iflb;
    --nlpf;
    --isboun;
    --ilboun;
    lcon -= 4;
    --factor;
    --nint;
    --ltype;
    --iline;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --iside;
    --rsize;
    linksc -= 3;
    linkr -= 3;
    --ihlist;
    --nhpr;
    --ifhole;
    --islist;
    --ifside;
    --nspr;
    --iregn;
    --nid;
    --y;
    --x;
    marked -= 4;
    --listl;
    linkeg -= 3;
    mmpold -= 4;
    --bmesur;
    --ynold;
    --xnold;
    --listeg;
    --amesur;
    nxkold_dim1 = *nnxk;
    nxkold_offset = 1 + nxkold_dim1;
    nxkold -= nxkold_offset;

    /* Function Body */
    addlnk = FALSE_;
    count = TRUE_;
    even = FALSE_;
    real__ = FALSE_;
/*  CHECK TO MAKE SURE CONNECTING DATA FOR THE REGION EXISTS */
/*  AND FILL IN ANY BLANK INTERVALS ACCORDING TO THE GIVEN SIZE */
/*  FOR THE REGION AND THE LINE'S LENGTH */
    if (nhpr[*l] > 0) {
	i__1 = ifhole[*l] + nhpr[*l] - 1;
	for (i__ = ifhole[*l]; i__ <= i__1; ++i__) {
	    ipntr1 = 0;
	    ltsort_(mr, &linkr[3], &ihlist[i__], &ipntr1, &addlnk);
	    if (ipntr1 > 0) {
		ll = ipntr1;
		dataok_(mp, ml, ms, mr, &ll, &iregn[ll], &coor[3], &iline[1], 
			&ltype[1], &nint[1], &lcon[4], &nlps[1], &ifline[1], &
			illist[1], &nspr[1], &ifside[1], &islist[1], &linkp[3]
			, &linkl[3], &links[3], &rsize[ll], errchk, err);
		if (*err) {
		    s_wsfe(&io___8);
		    do_fio(&c__1, (char *)&iregn[ll], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    addlnk = TRUE_;
		    iminus = -ll;
		    ltsort_(mr, &linkr[3], &iregn[ll], &iminus, &addlnk);
		    addlnk = FALSE_;
/*  CALCULATE THE PERIMETER OF THE REGION */
		} else {
		    knbc = 0;
		    ksbc = 0;
		    perim_(mp, ml, ms, &nspr[ll], maxnl, maxnp, &c__1, &c__1, 
			    &knbc, &ksbc, &iregn[ll], &ipoint[1], &coor[3], &
			    ipboun[1], &iline[1], &ltype[1], &nint[1], &
			    factor[1], &lcon[4], &ilboun[1], &isboun[1], &
			    iside[1], &nlps[1], &ifline[1], &illist[1], &
			    islist[ifside[ll]], &nppf[1], &ifpb[1], &listpb[3]
			    , &nlpf[1], &iflb[1], &listlb[3], &nspf[1], &ifsb[
			    1], &listsb[3], &linkp[3], &linkl[3], &links[3], &
			    linkpb[3], &linklb[3], &linksb[3], &x[1], &y[1], &
			    nid[1], &nper, &listl[1], &nl, idummy, &marked[4],
			     &even, &real__, err, &ccw, &count, noroom, &
			    amesur[1], &xnold[1], &ynold[1], &nxkold[
			    nxkold_offset], &mmpold[4], &linkeg[3], &listeg[1]
			    , &bmesur[1], mlink, nprold, npnold, npeold, nnxk,
			     remesh, rexmin, rexmax, reymin, reymax, idivis, 
			    sizmin, emax, emin);
		    if (nper <= 0 || *err) {
			s_wsfe(&io___16);
			do_fio(&c__1, (char *)&iregn[ll], (ftnlen)sizeof(
				integer));
			e_wsfe();
			addlnk = TRUE_;
			iminus = -ll;
			ltsort_(mr, &linkr[3], &iregn[ll], &iminus, &addlnk);
			addlnk = FALSE_;
		    } else {
/*  WHEN CHECKING THE MAXIMUMS - ADD ENOUGH FOR ONE MORE INTERVAL */
/*  ON THE LINE AS THIS LINE MAY BE INCREMENTED BY ONE IF THE */
/*  PERIMETER IS ODD */
/* Computing MAX */
			i__2 = *maxnbc, i__3 = knbc + 3 + *mxrnbc;
			*maxnbc = max(i__2,i__3);
/* Computing MAX */
			i__2 = *maxsbc, i__3 = ksbc + 3 + *mxrsbc;
			*maxsbc = max(i__2,i__3);
			*mxnl = max(*mxnl,nl);
/* Computing MAX */
			i__2 = *mxnper, i__3 = nper + 2;
			*mxnper = max(i__2,i__3);
/*  MARK THE LINES AND POINTS IN THE REGION AS BEING USED */
			mkused_(maxnl, mp, ml, &listl[1], &ipoint[1], &nint[1]
				, &linkp[3], &linkl[3], &lcon[4], &nl);
		    }
		}
	    } else {
		s_wsfe(&io___17);
		do_fio(&c__1, (char *)&iregn[ll], (ftnlen)sizeof(integer));
		e_wsfe();
		*err = TRUE_;
	    }
/* L100: */
	}
    }
    return 0;
} /* chkhol_ */

#ifdef __cplusplus
	}
#endif

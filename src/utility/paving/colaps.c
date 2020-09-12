/* colaps.f -- translated by f2c (version 20160102).
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
//#include "f2c.h"
#include "paving.h"

/* Common Block Declarations */

struct {
    real timea, timep, timec, timepc, timeaj, times;
} timing_;

#define timing_1 timing_

/* Table of constant values */

static integer c__1 = 1;
static integer c__2 = 2;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int colaps_(integer *mxnd, integer *mxcorn, integer *mln, 
	integer *mxloop, integer *nuid, real *xn, real *yn, real *zn, integer 
	*lxk, integer *kxl, integer *nxl, integer *lxn, real *angle, integer *
	lnodes, real *bnsize, integer *node, integer *kkkold, integer *lllold,
	 integer *nnnold, integer *iavail, integer *navail, logical *done, 
	real *xmin, real *xmax, real *ymin, real *ymax, real *zmin, real *
	zmax, char *dev1, integer *lll, integer *kkk, integer *nnn, integer *
	lcorn, integer *ncorn, integer *nloop, integer *nextn1, integer *
	kloop, logical *graph, logical *video, integer *kreg, logical *noroom,
	 logical *err, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset;
    real r__1, r__2;

    /* Builtin functions */
    double atan2(doublereal, doublereal), sqrt(doublereal);

    /* Local variables */
    static integer i__;
    static real u, w;
    static integer i1, i2, j1, j2, n0, n1, n2, n3;
    static real pi;
    static logical bok;
    static integer nnn2;
    extern /* Subroutine */ int sew2_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, logical *);
    static real fact;
    static integer idum;
    extern /* Subroutine */ int b4bad_(integer *, integer *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, logical *);
    static logical done1, done2;
    static integer node1, node2;
    static real time1, time2;
    static integer idum1, idum2;
    static real ahold;
    extern /* Subroutine */ int node12_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *), wedge_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, real *, 
	    integer *, real *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *, logical *, logical *, logical *, logical *), pinch_(
	    integer *, integer *, integer *, integer *, real *, real *, real *
	    , integer *, integer *, integer *, integer *, real *, integer *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, real *, real *, real *, real *, 
	    real *, real *, char *, integer *, integer *, integer *, integer *
	    , integer *, integer *, integer *, logical *, logical *, integer *
	    , logical *, logical *, ftnlen);
    static logical donep;
    static real disti, distj;
    extern /* Subroutine */ int d2node_(integer *, real *, real *, integer *, 
	    integer *), match2_(integer *, integer *, real *, real *, integer 
	    *, integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, integer *, 
	    integer *, real *, real *, integer *, logical *, logical *);
    static integer nloop1, nloop2, n1test, n0test, n2test, kount1, kount2, 
	    n3test;
    static logical cwedge;
    extern /* Subroutine */ int mesage_(char *, ftnlen);
    static logical cgraph, lmatch;
    extern /* Subroutine */ int getime_(real *);
    static logical pmatch;
    extern /* Subroutine */ int lupang_(integer *, integer *, real *, real *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, real *, real *, real *, 
	    real *, real *, real *, char *, integer *, logical *, ftnlen), 
	    filsmo_(integer *, integer *, real *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    real *, real *, char *, integer *, ftnlen), lcolor_(char *, 
	    ftnlen), bcross_(integer *, integer *, real *, real *, real *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *, integer *, 
	    real *, real *, real *, real *, real *, real *, char *, integer *,
	     logical *, ftnlen), marksm_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, logical *),
	     snapit_(integer *), sflush_();
    static logical lcross;
    extern /* Subroutine */ int intsct_(real *, real *, real *, real *, real *
	    , real *, real *, real *, real *, real *, logical *), rplotl_(
	    integer *, real *, real *, real *, integer *, real *, real *, 
	    real *, real *, real *, real *, integer *, char *, integer *, 
	    ftnlen);
    static integer kountl;

/* *********************************************************************** */
/*  SUBROUTINE COLAPS = COLLAPSES A LOOP INTO TWO POSSIBLE LOOPS */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --zn;
    --yn;
    --xn;
    --nuid;
    --lcorn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    --nextn1;
    --nloop;

    /* Function Body */
    pi = atan2((float)0., (float)-1.);
/*  FIND THE FIRST OVERLAPPING LINE STARTING AT THE CURRENT NODE */
    getime_(&time1);
    cgraph = FALSE_;
    cwedge = TRUE_;
    done1 = FALSE_;
    done2 = FALSE_;
    donep = FALSE_;
    pmatch = FALSE_;
    *err = FALSE_;
L100:
    n1 = *node;
    kount1 = 0;
    if (cgraph) {
	rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, ymax,
		 zmin, zmax, lll, dev1, kreg, (ftnlen)3);
    }
L110:
    n0 = lnodes[n1 * lnodes_dim1 + 2];
    n2 = lnodes[n1 * lnodes_dim1 + 3];
    n3 = lnodes[n2 * lnodes_dim1 + 3];
    ++kount1;
/*  CHECK FOR COMPLETION */
    if (n2 == *node) {
	goto L140;
    } else if (kount1 > nloop[1] + 1) {
	mesage_("** PROBLEMS WITH LOOP CLOSING IN COLAPS **", (ftnlen)42);
	*err = TRUE_;
	goto L140;
    }
/*  CHECK THIS LINE AGAINST ALL REMAINING LINES */
    kount2 = 2;
    n1test = lnodes[n2 * lnodes_dim1 + 3];
L120:
    n0test = lnodes[n1test * lnodes_dim1 + 2];
    n2test = lnodes[n1test * lnodes_dim1 + 3];
    n3test = lnodes[n2test * lnodes_dim1 + 3];
    if (cgraph) {
	lcolor_("YELOW", (ftnlen)5);
	d2node_(mxnd, &xn[1], &yn[1], &n1, &n2);
	d2node_(mxnd, &xn[1], &yn[1], &n1test, &n2test);
	lcolor_("WHITE", (ftnlen)5);
	sflush_();
    }
    intsct_(&xn[n1], &yn[n1], &xn[n2], &yn[n2], &xn[n1test], &yn[n1test], &xn[
	    n2test], &yn[n2test], &u, &w, &lcross);
    if (! lcross) {
	if (cgraph) {
	    d2node_(mxnd, &xn[1], &yn[1], &n1, &n2);
	    d2node_(mxnd, &xn[1], &yn[1], &n1test, &n2test);
	    sflush_();
	}
	n1test = n2test;
	++kount2;
	if (kount2 > nloop[1] / 2) {
	    n1 = n2;
	    goto L110;
	}
	goto L120;
    }
/*  AN INTERSECTION HAS OCCURRED. */
/*  GET THE BEST SEAM FROM THIS INTERSECTION */
    if (*graph || *video) {
	if (! donep) {
	    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, 
		    ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
	    donep = TRUE_;
	    if (*video) {
		snapit_(&c__1);
	    }
	}
	if (*graph) {
	    lcolor_("YELOW", (ftnlen)5);
	    d2node_(mxnd, &xn[1], &yn[1], &n1, &n2);
	    d2node_(mxnd, &xn[1], &yn[1], &n1test, &n2test);
	    lcolor_("WHITE", (ftnlen)5);
	    sflush_();
	}
    }
    match2_(mxnd, mln, &xn[1], &yn[1], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], &angle[1], &n0, &n1, &n2, &n3, &n0test, &n1test, &
	    n2test, &n3test, &i1, &i2, &j1, &j2, &kountl, &lmatch, &kount2, 
	    node, &u, &w, &nloop[1], &pmatch, err);
    if (*err) {
	goto L140;
    }
    if (*graph) {
	lcolor_("PINK ", (ftnlen)5);
	d2node_(mxnd, &xn[1], &yn[1], &i1, &i2);
	d2node_(mxnd, &xn[1], &yn[1], &j1, &j2);
	lcolor_("WHITE", (ftnlen)5);
	sflush_();
    }
    if (! lmatch) {
	n1test = n2test;
	++kount2;
	if (kount2 > nloop[1] / 2) {
	    n1 = n2;
	    goto L110;
	}
	goto L120;
    }
/*  NOW CHECK TO SEE IF THE ATTACHMENT WOULD CAUSE */
/*  LINES ON THE BOUNDARY TO CROSS */
    bcross_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &
	    lxn[5], &lnodes[lnodes_offset], &i1, &i2, &j1, &j2, &nloop[1], &
	    bok, lll, xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, err, (
	    ftnlen)3);
    if (*err) {
	goto L140;
    }
    if (! bok) {
	n1test = n2test;
	++kount2;
	if (kount2 > nloop[1] / 2) {
	    n1 = n2;
	    goto L110;
	}
	goto L120;
    }
/*  NOW CHECK TO SEE IF THE ATTACHMENT WOULD CAUSE */
/*  AN ILLFORMED 4 NODE ELEMENT */
    b4bad_(mxnd, mln, &xn[1], &yn[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
	    lnodes[lnodes_offset], &angle[1], &i1, &i2, &j1, &j2, &nloop[1], &
	    kountl, &bok, err);
    if (*err) {
	goto L140;
    }
    if (! bok) {
	n1test = n2test;
	++kount2;
	if (kount2 > nloop[1] / 2) {
	    n1 = n2;
	    goto L110;
	}
	goto L120;
    }
/*  SEE IF THE COLLAPSE IS BETWEEN TWO ELEMENT SIDES OF DISPROPORTIONATE */
/*  SIZES - IF SO A WEDGE MUST BE ADDED */
/* Computing 2nd power */
    r__1 = xn[i1] - xn[i2];
/* Computing 2nd power */
    r__2 = yn[i1] - yn[i2];
    disti = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
    r__1 = xn[j1] - xn[j2];
/* Computing 2nd power */
    r__2 = yn[j1] - yn[j2];
    distj = sqrt(r__1 * r__1 + r__2 * r__2);
    fact = (float)2.5;
    if (disti > fact * distj && (lxn[(j1 << 2) + 3] > 0 || lxn[(j1 << 2) + 2] 
	    < 0) && (lxn[(j2 << 2) + 3] > 0 || lxn[(j2 << 2) + 2] < 0)) {
	ahold = angle[i2];
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[i2 * lnodes_dim1 + 2], err);
	if (*err) {
	    goto L140;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[lnodes[i2 * lnodes_dim1 + 2] * 
		lnodes_dim1 + 2], err);
	if (*err) {
	    goto L140;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[i2 * lnodes_dim1 + 3], err);
	if (*err) {
	    goto L140;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[lnodes[i2 * lnodes_dim1 + 3] * 
		lnodes_dim1 + 3], err);
	if (*err) {
	    goto L140;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &i2, err);
	if (*err) {
	    goto L140;
	}
	wedge_(mxnd, mln, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &xn[1]
		, &yn[1], &lnodes[lnodes_offset], &bnsize[3], iavail, navail, 
		lll, kkk, nnn, lllold, nnnold, &i2, &idum, &nloop[1], &cwedge,
		 graph, video, noroom, err);
	if (*noroom || *err) {
	    goto L140;
	}
	if (*video) {
	    snapit_(&c__2);
	}
	++kountl;
	i1 = i2;
	i2 = lnodes[i1 * lnodes_dim1 + 3];
	angle[lnodes[i2 * lnodes_dim1 + 3]] = ahold;
	angle[i1] = pi;
	angle[i2] = pi;
	goto L100;
    } else if (distj > fact * disti && (lxn[(i1 << 2) + 3] > 0 || lxn[(i1 << 
	    2) + 2] < 0) && (lxn[(i2 << 2) + 3] > 0 || lxn[(i2 << 2) + 2] < 0)
	    ) {
	ahold = angle[j2];
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[j2 * lnodes_dim1 + 2], err);
	if (*err) {
	    goto L140;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[lnodes[j2 * lnodes_dim1 + 2] * 
		lnodes_dim1 + 2], err);
	if (*err) {
	    goto L140;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[j2 * lnodes_dim1 + 3], err);
	if (*err) {
	    goto L140;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &lnodes[lnodes[j2 * lnodes_dim1 + 3] * 
		lnodes_dim1 + 3], err);
	if (*err) {
	    goto L140;
	}
	marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
		lnodes_offset], &j2, err);
	if (*err) {
	    goto L140;
	}
	wedge_(mxnd, mln, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &xn[1]
		, &yn[1], &lnodes[lnodes_offset], &bnsize[3], iavail, navail, 
		lll, kkk, nnn, lllold, nnnold, &j2, &idum, &nloop[1], &cwedge,
		 graph, video, noroom, err);
	if (*noroom || *err) {
	    goto L140;
	}
	if (*video) {
	    snapit_(&c__2);
	}
	++kountl;
	j1 = j2;
	j2 = lnodes[j1 * lnodes_dim1 + 3];
	angle[lnodes[j2 * lnodes_dim1 + 3]] = ahold;
	angle[j1] = pi;
	angle[j2] = pi;
	goto L100;
    }
/*  NOW THAT THE APPROPRIATE COLLAPSE HAS BEEN FOUND, THE TWO LINES */
/*  MUST BE JOINED. */
    sew2_(mxnd, mln, &nuid[1], &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], iavail, navail, lll, kkk, nnn, &i1, &i2, &j1, &j2,
	     noroom, err);
    if (*noroom || *err) {
	goto L140;
    }
/*  NOW SMOOTH AND PLOT THE CURRENT MESH */
    nnn2 = 1;
    getime_(&time2);
    timing_1.timec = timing_1.timec + time2 - time1;
    filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &
	    lxn[5], lll, nnn, &nnn2, &lnodes[lnodes_offset], &bnsize[3], &
	    nloop[1], xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, (ftnlen)
	    3);
    getime_(&time1);
    if (*graph || *video) {
	rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, ymin, ymax,
		 zmin, zmax, lll, dev1, kreg, (ftnlen)3);
	if (*video) {
	    snapit_(&c__1);
	}
	if (*graph) {
	    lcolor_("YELOW", (ftnlen)5);
	    d2node_(mxnd, &xn[1], &yn[1], &i1, &i2);
	    lcolor_("WHITE", (ftnlen)5);
	    sflush_();
	}
    }
    nloop1 = kountl;
    nloop2 = nloop[1] - kountl - 2;
/*  NOW UPDATE THE DEFINITIONS OF NODE FOR BOTH LOOPS */
    if (j1 == *node) {
	*node = i2;
    } else if (j2 == *node) {
	*node = i1;
    }
    node12_(mxnd, mln, &lnodes[lnodes_offset], &i1, &i2, &nloop1, &nloop2, &
	    node1, &node2, node, err);
    if (*err) {
	goto L140;
    }
/*  NOW TRY TO PINCH BOTH LOOPS */
    lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &
	    lxn[5], &nloop1, &angle[1], &lnodes[lnodes_offset], &node2, lll, 
	    xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, err, (ftnlen)3);
    if (*err) {
	goto L140;
    }
    idum1 = 0;
    idum2 = 0;
    getime_(&time2);
    timing_1.timec = timing_1.timec + time2 - time1;
    pinch_(mxnd, mxcorn, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[
	    3], &nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], &bnsize[
	    3], &node2, &nloop1, kkkold, lllold, nnnold, iavail, navail, &
	    done1, xmin, xmax, ymin, ymax, zmin, zmax, dev1, lll, kkk, nnn, &
	    lcorn[1], ncorn, &idum1, &idum2, graph, video, kreg, noroom, err, 
	    (ftnlen)3);
    if (*noroom || *err) {
	goto L140;
    }
    getime_(&time1);
    lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[3], &
	    lxn[5], &nloop2, &angle[1], &lnodes[lnodes_offset], &node1, lll, 
	    xmin, xmax, ymin, ymax, zmin, zmax, dev1, kreg, err, (ftnlen)3);
    if (*err) {
	goto L140;
    }
    getime_(&time2);
    timing_1.timec = timing_1.timec + time2 - time1;
    pinch_(mxnd, mxcorn, mln, &nuid[1], &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[
	    3], &nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], &bnsize[
	    3], &node1, &nloop2, kkkold, lllold, nnnold, iavail, navail, &
	    done2, xmin, xmax, ymin, ymax, zmin, zmax, dev1, lll, kkk, nnn, &
	    lcorn[1], ncorn, &idum1, &idum2, graph, video, kreg, noroom, err, 
	    (ftnlen)3);
    if (*noroom || *err) {
	goto L140;
    }
    getime_(&time1);
/*  NOW HANDLE THE PLACEMENT OF THOSE LOOPS */
    if (done1 && done2) {
	*done = TRUE_;
	goto L140;
    } else if (done1) {
	nloop[1] = nloop2;
	*node = node1;
    } else if (done2) {
	nloop[1] = nloop1;
	*node = node2;
    } else {
/*  MOVE PREVIOUS LOOPS DOWN IN THE LIST */
	++(*kloop);
	for (i__ = *kloop; i__ >= 3; --i__) {
	    nloop[i__] = nloop[i__ - 1];
	    nextn1[i__] = nextn1[i__ - 1];
/* L130: */
	}
	nextn1[*kloop] = 0;
/*  INSERT THE TWO NEW LISTS AS THE TOP TWO - KEEPING NODE */
/*  THE SAME FOR ONE OF THE LOOPS */
	nloop[1] = nloop1;
	nloop[2] = nloop2;
	nextn1[2] = nextn1[1];
	nextn1[1] = node1;
	*node = node2;
    }
/*  NOW MAKE SURE THAT THE TOP LOOP DOES NOT NEED A COLAPS AGAIN */
    goto L100;
L140:
    getime_(&time2);
    timing_1.timec = timing_1.timec + time2 - time1;
    return 0;
} /* colaps_ */

#ifdef __cplusplus
	}
#endif

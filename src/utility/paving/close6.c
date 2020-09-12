/* close6.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int close6_(integer *mxnd, integer *mxcorn, integer *mln, 
	integer *nuid, real *xn, real *yn, integer *lxk, integer *kxl, 
	integer *nxl, integer *lxn, real *angle, real *bnsize, integer *
	lnodes, integer *node, integer *nloop, integer *kkkold, integer *
	lllold, integer *nnnold, integer *navail, integer *iavail, 
	 real *xmin, real *xmax, real *ymin, real *ymax, char *dev1, integer *
	lll, integer *kkk, integer *nnn, integer *lcorn, integer *ncorn, 
	logical *graph, logical *video, logical *sizeit, logical *noroom, 
	logical *err, real *xnold, real *ynold, integer *nxkold, integer *
	linkeg, integer *listeg, real *bmesur, integer *mlink, integer *
	npnold, integer *npeold, integer *nnxk, real *rexmin, 
	real *rexmax, real *reymin, real *reymax, integer *idivis, real *
	sizmin, real *emax, real *emin, ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, nxkold_dim1, nxkold_offset, i__1, 
	    i__2;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i1, i2, i3, i4, i5, i6;
    static real xnew, ynew, dist1, dist2, dist3, xnew1, xnew2, xnew3, ynew1, 
	    ynew2, ynew3;
    static integer inode;
    extern /* Subroutine */ int add2el_(integer *, 
	    integer *, real *, real *, integer *, integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, logical *, logical *, logical *),
	    mesage_(char *, ftnlen), marksm_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    logical *), cntcrn_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *);
    static integer ikount;

/* *********************************************************************** */
/*  SUBROUTINE CLOSE6 = FINISHES UP A LOOP WITH ONLY 6 LINES IN THE LOOP */
/* *********************************************************************** */
    /* Parameter adjustments */
    bnsize -= 3;
    --angle;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --yn;
    --xn;
    --nuid;
    --lcorn;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;
    linkeg -= 3;
    --bmesur;
    --ynold;
    --xnold;
    --listeg;
    nxkold_dim1 = *nnxk;
    nxkold_offset = 1 + nxkold_dim1;
    nxkold -= nxkold_offset;

    /* Function Body */
    *err = FALSE_;
/*  SET ALL THE LOOP NODES TO BE INTERIOR */
    inode = *node;
    ikount = 0;
L100:
    ++ikount;
    if (ikount > 6) {
	mesage_("** PROBLEMS IN CLOSE6 WITH TOO MANY IN LOOP **", (ftnlen)46);
	*err = TRUE_;
	goto L110;
    }
    lnodes[inode * lnodes_dim1 + 4] = -2;
    marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &lnodes[
	    lnodes_offset], &inode, err);
    if (*err) {
	goto L110;
    }
    inode = lnodes[inode * lnodes_dim1 + 3];
    if (inode != *node) {
	goto L100;
    }
/*  NOW GET THE CORRECT INTERPRETATION OF THE SHAPE */
    cntcrn_(mxnd, mxcorn, mln, &lnodes[lnodes_offset], &lcorn[1], ncorn, 
	    nloop, node, err);
    if (*err) {
	goto L110;
    }
/*  PROCESS A TRIANGLE SHAPE WITH VARIOUS INTERVAL COMBINATIONS */
    if (*ncorn == 3) {
	i1 = lnodes[lcorn[1] * lnodes_dim1 + 7];
	i2 = lnodes[lcorn[2] * lnodes_dim1 + 7];
	i3 = lnodes[lcorn[3] * lnodes_dim1 + 7];
/*  HANDLE A 4-1-1 TRIANGLE */
/* Computing MAX */
	i__1 = int_max(i1,i2);
	if (int_max(i__1,i3) == 4) {
	    if (i1 == 4) {
		xnew = ((xn[lcorn[1]] + xn[lcorn[2]]) * (float).5 + xn[lcorn[
			3]]) * (float).5;
		ynew = ((yn[lcorn[1]] + yn[lcorn[2]]) * (float).5 + yn[lcorn[
			3]]) * (float).5;
		add1cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &bnsize[3], &lnodes[
			lnodes_offset], nnn, kkk, lll, nnnold, lllold, nloop, 
			&xnew, &ynew, &lcorn[1], iavail, navail, graph, video,
			 sizeit, noroom, err, &xnold[1], &ynold[1], &nxkold[
			nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], 
			mlink, npnold, npeold, nnxk, rexmin, rexmax, 
			reymin, reymax, idivis, sizmin, emax, emin);
		if (*noroom || *err) {
		    goto L110;
		}
	    } else if (i2 == 4) {
		xnew = ((xn[lcorn[2]] + xn[lcorn[3]]) * (float).5 + xn[lcorn[
			1]]) * (float).5;
		ynew = ((yn[lcorn[2]] + yn[lcorn[3]]) * (float).5 + yn[lcorn[
			1]]) * (float).5;
		add1cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &bnsize[3], &lnodes[
			lnodes_offset], nnn, kkk, lll, nnnold, lllold, nloop, 
			&xnew, &ynew, &lcorn[2], iavail, navail, graph, video,
			 sizeit, noroom, err, &xnold[1], &ynold[1], &nxkold[
			nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], 
			mlink, npnold, npeold, nnxk, rexmin, rexmax, 
			reymin, reymax, idivis, sizmin, emax, emin);
		if (*noroom || *err) {
		    goto L110;
		}
	    } else {
		xnew = ((xn[lcorn[3]] + xn[lcorn[1]]) * (float).5 + xn[lcorn[
			2]]) * (float).5;
		ynew = ((yn[lcorn[3]] + yn[lcorn[1]]) * (float).5 + yn[lcorn[
			2]]) * (float).5;
		add1cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &bnsize[3], &lnodes[
			lnodes_offset], nnn, kkk, lll, nnnold, lllold, nloop, 
			&xnew, &ynew, &lcorn[3], iavail, navail, graph, video,
			 sizeit, noroom, err, &xnold[1], &ynold[1], &nxkold[
			nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], 
			mlink, npnold, npeold, nnxk, rexmin, rexmax, 
			reymin, reymax, idivis, sizmin, emax, emin);
		if (*noroom || *err) {
		    goto L110;
		}
	    }
/*  HANDLE A 3-2-1 TRIANGLE */
	} else /* if(complicated condition) */ {
/* Computing MAX */
	    i__1 = int_max(i1,i2);
	    if (int_max(i__1,i3) == 3) {
		if (i1 == 1) {
		    add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &
			    kxl[3], &nxl[3], &lxn[5], &angle[1], &lnodes[
			    lnodes_offset], nnn, kkk, lll, nnnold, lllold, 
			    nloop, &lnodes[lcorn[1] * lnodes_dim1 + 2], &
			    lnodes[lcorn[2] * lnodes_dim1 + 3], iavail, 
			    navail, graph, video, noroom, err);
		    if (*noroom || *err) {
			goto L110;
		    }
		} else if (i2 == 1) {
		    add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &
			    kxl[3], &nxl[3], &lxn[5], &angle[1], &lnodes[
			    lnodes_offset], nnn, kkk, lll, nnnold, lllold, 
			    nloop, &lnodes[lcorn[2] * lnodes_dim1 + 2], &
			    lnodes[lcorn[3] * lnodes_dim1 + 3], iavail, 
			    navail, graph, video, noroom, err);
		    if (*noroom || *err) {
			goto L110;
		    }
		} else {
		    add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &
			    kxl[3], &nxl[3], &lxn[5], &angle[1], &lnodes[
			    lnodes_offset], nnn, kkk, lll, nnnold, lllold, 
			    nloop, &lnodes[lcorn[3] * lnodes_dim1 + 2], &
			    lnodes[lcorn[1] * lnodes_dim1 + 3], iavail, 
			    navail, graph, video, noroom, err);
		    if (*noroom || *err) {
			goto L110;
		    }
		}
/*  HANDLE A 2-2-2 TRIANGLE */
	    } else {
		xnew1 = ((xn[lcorn[1]] + xn[lcorn[2]]) * (float).5 + xn[lcorn[
			3]]) * (float).5;
		xnew2 = ((xn[lcorn[2]] + xn[lcorn[3]]) * (float).5 + xn[lcorn[
			1]]) * (float).5;
		xnew3 = ((xn[lcorn[3]] + xn[lcorn[1]]) * (float).5 + xn[lcorn[
			2]]) * (float).5;
		ynew1 = ((yn[lcorn[1]] + yn[lcorn[2]]) * (float).5 + yn[lcorn[
			3]]) * (float).5;
		ynew2 = ((yn[lcorn[2]] + yn[lcorn[3]]) * (float).5 + yn[lcorn[
			1]]) * (float).5;
		ynew3 = ((yn[lcorn[3]] + yn[lcorn[1]]) * (float).5 + yn[lcorn[
			2]]) * (float).5;
		xnew = (xnew1 + xnew2 + xnew3) / (float)3.;
		ynew = (ynew1 + ynew2 + ynew3) / (float)3.;
		add1cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &bnsize[3], &lnodes[
			lnodes_offset], nnn, kkk, lll, nnnold, lllold, nloop, 
			&xnew, &ynew, &lcorn[1], iavail, navail, graph, video,
			 sizeit, noroom, err, &xnold[1], &ynold[1], &nxkold[
			nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], 
			mlink, npnold, npeold, nnxk, rexmin, rexmax, 
			reymin, reymax, idivis, sizmin, emax, emin);
		if (*noroom || *err) {
		    goto L110;
		}
	    }
	}
/*  PROCESS A RECTANGLE SHAPE WITH VARIOUS INTERVAL COMBINATIONS */
    } else if (*ncorn == 4) {
	i1 = lnodes[lcorn[1] * lnodes_dim1 + 7];
	i2 = lnodes[lcorn[2] * lnodes_dim1 + 7];
	i3 = lnodes[lcorn[3] * lnodes_dim1 + 7];
	i4 = lnodes[lcorn[4] * lnodes_dim1 + 7];
/*  HANDLE A 3-1-1-1 RECTANGLE */
/* Computing MAX */
	i__1 = int_max(i1,i2), i__1 = int_max(i__1,i3);
	if (int_max(i__1,i4) == 3) {
	    if (i1 == 3) {
		add2cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &bnsize[3], &lnodes[
			lnodes_offset], nnn, kkk, lll, nnnold, lllold, nloop, 
			&lcorn[1], iavail, navail, graph, video, sizeit, 
			noroom, err, &xnold[1], &ynold[1], &nxkold[
			nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], 
			mlink, npnold, npeold, nnxk, rexmin, rexmax, 
			reymin, reymax, idivis, sizmin, emax, emin);
		if (*noroom || *err) {
		    goto L110;
		}
	    } else if (i2 == 3) {
		add2cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &bnsize[3], &lnodes[
			lnodes_offset], nnn, kkk, lll, nnnold, lllold, nloop, 
			&lcorn[2], iavail, navail, graph, video, sizeit, 
			noroom, err, &xnold[1], &ynold[1], &nxkold[
			nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], 
			mlink, npnold, npeold, nnxk, rexmin, rexmax, 
			reymin, reymax, idivis, sizmin, emax, emin);
		if (*noroom || *err) {
		    goto L110;
		}
	    } else if (i3 == 3) {
		add2cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &bnsize[3], &lnodes[
			lnodes_offset], nnn, kkk, lll, nnnold, lllold, nloop, 
			&lcorn[3], iavail, navail, graph, video, sizeit, 
			noroom, err, &xnold[1], &ynold[1], &nxkold[
			nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], 
			mlink, npnold, npeold, nnxk, rexmin, rexmax, 
			reymin, reymax, idivis, sizmin, emax, emin);
		if (*noroom || *err) {
		    goto L110;
		}
	    } else {
		add2cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &bnsize[3], &lnodes[
			lnodes_offset], nnn, kkk, lll, nnnold, lllold, nloop, 
			&lcorn[4], iavail, navail, graph, video, sizeit, 
			noroom, err, &xnold[1], &ynold[1], &nxkold[
			nxkold_offset], &linkeg[3], &listeg[1], &bmesur[1], 
			mlink, npnold, npeold, nnxk, rexmin, rexmax, 
			reymin, reymax, idivis, sizmin, emax, emin);
		if (*noroom || *err) {
		    goto L110;
		}
	    }
/*  HANDLE A 2-2-1-1 RECTANGLE */
	} else /* if(complicated condition) */ {
/* Computing MAX */
	    i__1 = i1 + i2, i__2 = i2 + i3, i__1 = int_max(i__1,i__2), i__2 = i3 
		    + i4, i__1 = int_max(i__1,i__2), i__2 = i4 + i1;
	    if (int_max(i__1,i__2) == 4) {
		if (i1 + i2 == 4) {
		    xnew = (xn[lnodes[lcorn[1] * lnodes_dim1 + 3]] + xn[
			    lnodes[lcorn[2] * lnodes_dim1 + 3]] + xn[lcorn[3]]
			     * (float).5 + xn[lcorn[4]] + xn[lcorn[1]] * (
			    float).5) * (float).25;
		    ynew = (yn[lnodes[lcorn[1] * lnodes_dim1 + 3]] + yn[
			    lnodes[lcorn[2] * lnodes_dim1 + 3]] + yn[lcorn[3]]
			     * (float).5 + yn[lcorn[4]] + yn[lcorn[1]] * (
			    float).5) * (float).25;
		    add1cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &
			    kxl[3], &nxl[3], &lxn[5], &angle[1], &bnsize[3], &
			    lnodes[lnodes_offset], nnn, kkk, lll, nnnold, 
			    lllold, nloop, &xnew, &ynew, &lcorn[1], iavail, 
			    navail, graph, video, sizeit, noroom, err, &xnold[
			    1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3],
			     &listeg[1], &bmesur[1], mlink, npnold, npeold, 
			    nnxk, rexmin, rexmax, reymin, reymax, 
			    idivis, sizmin, emax, emin);
		    if (*noroom || *err) {
			goto L110;
		    }
		} else if (i2 + i3 == 4) {
		    xnew = (xn[lnodes[lcorn[2] * lnodes_dim1 + 3]] + xn[
			    lnodes[lcorn[3] * lnodes_dim1 + 3]] + xn[lcorn[4]]
			     * (float).5 + xn[lcorn[1]] + xn[lcorn[2]] * (
			    float).5) * (float).25;
		    ynew = (yn[lnodes[lcorn[2] * lnodes_dim1 + 3]] + yn[
			    lnodes[lcorn[3] * lnodes_dim1 + 3]] + yn[lcorn[4]]
			     * (float).5 + yn[lcorn[1]] + yn[lcorn[2]] * (
			    float).5) * (float).25;
		    add1cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &
			    kxl[3], &nxl[3], &lxn[5], &angle[1], &bnsize[3], &
			    lnodes[lnodes_offset], nnn, kkk, lll, nnnold, 
			    lllold, nloop, &xnew, &ynew, &lcorn[2], iavail, 
			    navail, graph, video, sizeit, noroom, err, &xnold[
			    1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3],
			     &listeg[1], &bmesur[1], mlink, npnold, npeold, 
			    nnxk, rexmin, rexmax, reymin, reymax, 
			    idivis, sizmin, emax, emin);
		    if (*noroom || *err) {
			goto L110;
		    }
		} else if (i3 + i4 == 4) {
		    xnew = (xn[lnodes[lcorn[3] * lnodes_dim1 + 3]] + xn[
			    lnodes[lcorn[4] * lnodes_dim1 + 3]] + xn[lcorn[1]]
			     * (float).5 + xn[lcorn[2]] + xn[lcorn[3]] * (
			    float).5) * (float).25;
		    ynew = (yn[lnodes[lcorn[3] * lnodes_dim1 + 3]] + yn[
			    lnodes[lcorn[4] * lnodes_dim1 + 3]] + yn[lcorn[1]]
			     * (float).5 + yn[lcorn[2]] + yn[lcorn[3]] * (
			    float).5) * (float).25;
		    add1cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &
			    kxl[3], &nxl[3], &lxn[5], &angle[1], &bnsize[3], &
			    lnodes[lnodes_offset], nnn, kkk, lll, nnnold, 
			    lllold, nloop, &xnew, &ynew, &lcorn[3], iavail, 
			    navail, graph, video, sizeit, noroom, err, &xnold[
			    1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3],
			     &listeg[1], &bmesur[1], mlink, npnold, npeold, 
			    nnxk, rexmin, rexmax, reymin, reymax, 
			    idivis, sizmin, emax, emin);
		    if (*noroom || *err) {
			goto L110;
		    }
		} else {
		    xnew = (xn[lnodes[lcorn[4] * lnodes_dim1 + 3]] + xn[
			    lnodes[lcorn[1] * lnodes_dim1 + 3]] + xn[lcorn[2]]
			     * (float).5 + xn[lcorn[3]] + xn[lcorn[4]] * (
			    float).5) * (float).25;
		    ynew = (yn[lnodes[lcorn[4] * lnodes_dim1 + 3]] + yn[
			    lnodes[lcorn[1] * lnodes_dim1 + 3]] + yn[lcorn[2]]
			     * (float).5 + yn[lcorn[3]] + yn[lcorn[4]] * (
			    float).5) * (float).25;
		    add1cn_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &
			    kxl[3], &nxl[3], &lxn[5], &angle[1], &bnsize[3], &
			    lnodes[lnodes_offset], nnn, kkk, lll, nnnold, 
			    lllold, nloop, &xnew, &ynew, &lcorn[4], iavail, 
			    navail, graph, video, sizeit, noroom, err, &xnold[
			    1], &ynold[1], &nxkold[nxkold_offset], &linkeg[3],
			     &listeg[1], &bmesur[1], mlink, npnold, npeold, 
			    nnxk, rexmin, rexmax, reymin, reymax, 
			    idivis, sizmin, emax, emin);
		    if (*noroom || *err) {
			goto L110;
		    }
		}
/*  HANDLE A 2-1-2-1 RECTANGLE */
	    } else {
		if (i1 == 2) {
		    add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &
			    kxl[3], &nxl[3], &lxn[5], &angle[1], &lnodes[
			    lnodes_offset], nnn, kkk, lll, nnnold, lllold, 
			    nloop, &lnodes[lcorn[1] * lnodes_dim1 + 3], &
			    lnodes[lcorn[3] * lnodes_dim1 + 3], iavail, 
			    navail, graph, video, noroom, err);
		    if (*noroom || *err) {
			goto L110;
		    }
		} else {
		    add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &
			    kxl[3], &nxl[3], &lxn[5], &angle[1], &lnodes[
			    lnodes_offset], nnn, kkk, lll, nnnold, lllold, 
			    nloop, &lnodes[lcorn[2] * lnodes_dim1 + 3], &
			    lnodes[lcorn[4] * lnodes_dim1 + 3], iavail, 
			    navail, graph, video, noroom, err);
		    if (*noroom || *err) {
			goto L110;
		    }
		}
	    }
	}
/*  PROCESS A SEMICIRCLE SHAPE WITH VARIOUS INTERVAL COMBINATIONS */
    } else if (*ncorn == 2) {
	i1 = lnodes[lcorn[1] * lnodes_dim1 + 7];
	i2 = lnodes[lcorn[2] * lnodes_dim1 + 7];
/*  HANDLE A 5-1 SEMICIRCLE */
	if (int_max(i1,i2) == 5) {
	    if (i1 == 1) {
		add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], 
			nnn, kkk, lll, nnnold, lllold, nloop, &lnodes[lcorn[1]
			 * lnodes_dim1 + 2], &lnodes[lcorn[2] * lnodes_dim1 + 
			3], iavail, navail, graph, video, noroom, err);
		if (*noroom || *err) {
		    goto L110;
		}
	    } else {
		add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], 
			nnn, kkk, lll, nnnold, lllold, nloop, &lnodes[lcorn[2]
			 * lnodes_dim1 + 2], &lnodes[lcorn[1] * lnodes_dim1 + 
			3], iavail, navail, graph, video, noroom, err);
		if (*noroom || *err) {
		    goto L110;
		}
	    }
/*  HANDLE A 4-2 SEMICIRCLE */
	} else if (int_max(i1,i2) == 4) {
	    if (i1 == 2) {
		add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], 
			nnn, kkk, lll, nnnold, lllold, nloop, &lnodes[lcorn[1]
			 * lnodes_dim1 + 3], &lnodes[lnodes[lcorn[2] * 
			lnodes_dim1 + 3] * lnodes_dim1 + 3], iavail, navail, 
			graph, video, noroom, err);
		if (*noroom || *err) {
		    goto L110;
		}
	    } else {
		add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3],
			 &nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], 
			nnn, kkk, lll, nnnold, lllold, nloop, &lnodes[lcorn[2]
			 * lnodes_dim1 + 3], &lnodes[lnodes[lcorn[1] * 
			lnodes_dim1 + 3] * lnodes_dim1 + 3], iavail, navail, 
			graph, video, noroom, err);
		if (*noroom || *err) {
		    goto L110;
		}
	    }
/*  HANDLE A 3-3 SEMICIRCLE */
	} else {
	    add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &
		    nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], nnn, 
		    kkk, lll, nnnold, lllold, nloop, &lnodes[lcorn[1] * 
		    lnodes_dim1 + 3], &lnodes[lcorn[2] * lnodes_dim1 + 3], 
		    iavail, navail, graph, video, noroom, err);
	    if (*noroom || *err) {
		goto L110;
	    }
/*            CALL ADD2CN (MXND, MLN, XN, YN, NUID, LXK, KXL, NXL, LXN, */
/*     &         ANGLE, BNSIZE, LNODES, NNN, KKK, LLL, NNNOLD, LLLOLD, */
/*     &         NLOOP, LCORN(1), IAVAIL, NAVAIL, GRAPH, VIDEO, SIZEIT, */
/*     &            NOROOM, ERR, XNOLD, YNOLD, NXKOLD, LINKEG, LISTEG, */
/*     &            BMESUR, MLINK, NPNOLD, NPEOLD, NNXK, REXMIN, */
/*     &            REXMAX, REYMIN, REYMAX, IDIVIS, SIZMIN, EMAX, EMIN) */
/*            IF ((NOROOM) .OR. (ERR)) GOTO 110 */
	}
/*  PROCESS A TEAR-DROP SHAPE (1 CORNER) */
    } else if (*ncorn == 1) {
	add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &nxl[3]
		, &lxn[5], &angle[1], &lnodes[lnodes_offset], nnn, kkk, lll, 
		nnnold, lllold, nloop, &lnodes[lcorn[1] * lnodes_dim1 + 3], &
		lnodes[lnodes[lcorn[1] * lnodes_dim1 + 2] * lnodes_dim1 + 2], 
		iavail, navail, graph, video, noroom, err);
	if (*noroom || *err) {
	    goto L110;
	}
/*  PROCESS A TRUE CIRCLE (OR ANYTHING ELSE FOR THAT MATTER) */
    } else {
	i1 = *node;
	i2 = lnodes[*node * lnodes_dim1 + 3];
	i3 = lnodes[lnodes[*node * lnodes_dim1 + 3] * lnodes_dim1 + 3];
	i4 = lnodes[lnodes[lnodes[*node * lnodes_dim1 + 3] * lnodes_dim1 + 3] 
		* lnodes_dim1 + 3];
	i5 = lnodes[lnodes[*node * lnodes_dim1 + 2] * lnodes_dim1 + 2];
	i6 = lnodes[*node * lnodes_dim1 + 2];
/* Computing 2nd power */
	r__1 = xn[i1] - xn[i4];
/* Computing 2nd power */
	r__2 = yn[i1] - yn[i4];
	dist1 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xn[i2] - xn[i5];
/* Computing 2nd power */
	r__2 = yn[i2] - yn[i5];
	dist2 = sqrt(r__1 * r__1 + r__2 * r__2);
/* Computing 2nd power */
	r__1 = xn[i3] - xn[i6];
/* Computing 2nd power */
	r__2 = yn[i3] - yn[i6];
	dist3 = sqrt(r__1 * r__1 + r__2 * r__2);
	if (dist1 <= dist2 && dist1 <= dist3) {
	    add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &
		    nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], nnn, 
		    kkk, lll, nnnold, lllold, nloop, &i1, &i4, iavail, navail,
		     graph, video, noroom, err);
	    if (*noroom || *err) {
		goto L110;
	    }
	} else if (dist2 <= dist3 && dist2 <= dist1) {
	    add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &
		    nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], nnn, 
		    kkk, lll, nnnold, lllold, nloop, &i2, &i5, iavail, navail,
		     graph, video, noroom, err);
	    if (*noroom || *err) {
		goto L110;
	    }
	} else {
	    add2el_(mxnd, mln, &xn[1], &yn[1], &nuid[1], &lxk[5], &kxl[3], &
		    nxl[3], &lxn[5], &angle[1], &lnodes[lnodes_offset], nnn, 
		    kkk, lll, nnnold, lllold, nloop, &i3, &i6, iavail, navail,
		     graph, video, noroom, err);
	    if (*noroom || *err) {
		goto L110;
	    }
	}
    }
L110:
    return 0;
} /* close6_ */

#ifdef __cplusplus
	}
#endif

/* getrow.f -- translated by f2c (version 20160102).
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
#include "paving.h"

/* Table of constant values */

static integer c__0 = 0;
static integer c__3 = 3;
static integer c__1 = 1;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int getrow_(integer *mxnd, integer *mxcorn, integer *mxpick, 
	integer *mln, integer *nuid, integer *lxk, integer *kxl, integer *nxl,
	 integer *lxn, integer *lnodes, integer *ncorn, integer *lcorn, real *
	bnsize, real *angle, real *xn, real *yn, real *zn, integer *icomb, 
	integer *itype, integer *nloop, integer *nbegin, integer *nend, 
	integer *iavail, integer *navail, integer *lll, integer *kkk, integer 
	*nnn, logical *graph, logical *video, real *xmin, real *xmax, real *
	ymin, real *ymax, real *zmin, real *zmax, char *dev1, integer *kreg, 
	logical *sizeit, integer *nextpr, logical *noroom, logical *err, 
	ftnlen dev1_len)
{
    /* System generated locals */
    integer lnodes_dim1, lnodes_offset, icomb_dim1, icomb_offset, i__1, i__2;

    /* Local variables */
    static integer i__;
    static real x[1], y[1];
    static integer ii, jj, in, is2c, nnn2, iend;
    static real qual;
    extern /* Subroutine */ int tuck_(integer *, integer *, integer *, real *,
	     real *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, logical *, logical *, logical *);
    static integer isqr;
    extern /* Subroutine */ int keep3_(integer *, integer *, integer *, 
	    integer *);
    static logical redo2, redo3;
    static real best2, best3, best4;
    extern /* Subroutine */ int qual3_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, integer *, real *, logical *, logical *, logical *);
    static integer npin2;
    extern /* Subroutine */ int qual4_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, integer *, real *, logical *, logical *);
    static integer nbeg24, nbeg34, ichng, nend24, nend34, inode, npick;
    extern /* Subroutine */ int ch3to4_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, real *, logical *, integer *);
    extern logical cornp_(real *);
    static logical onlyc;
    static integer istat, itest[5], ltest[5], ibest2, ibest3, ibest4;
    static logical found2, found3, found4, posbl2, posbl3, posbl4;
    extern /* Subroutine */ int qual2n_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, real *, 
	    integer *, integer *, integer *, real *, logical *, logical *, 
	    logical *, logical *, integer *, integer *, integer *, integer *, 
	    logical *);
    static integer ipinch[4], jpinch[4], npinch;
    extern /* Subroutine */ int getcrn_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, real *, real *, integer *
	    , integer *, integer *, logical *, logical *, logical *, logical *
	    );
    static logical sidpin;
    extern /* Subroutine */ int lupang_(integer *, integer *, real *, real *, 
	    real *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, integer *, integer *, real *, real *, real *, 
	    real *, real *, real *, char *, integer *, logical *, ftnlen), 
	    filsmo_(integer *, integer *, real *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    real *, real *, char *, integer *, ftnlen), lcolor_(char *, 
	    ftnlen), marksm_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, logical *), snapit_(
	    integer *);
    static logical pposbl, rowchn;
    extern /* Subroutine */ int ndstat_(integer *, integer *, real *, integer 
	    *), setcir_(integer *, integer *, integer *, integer *, integer *,
	     logical *), sflush_(), symbol_(integer *, real *, real *, char *,
	     ftnlen), setlop_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *), comsrt_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *);
    static integer istart;
    extern /* Subroutine */ int rplotl_(integer *, real *, real *, real *, 
	    integer *, real *, real *, real *, real *, real *, real *, 
	    integer *, char *, integer *, ftnlen);

/* *********************************************************************** */
/*  SUBROUTINE GETROW = GETS THE CURRENT ROW TO START ON */
/* *********************************************************************** */
    /* Parameter adjustments */
    --zn;
    --yn;
    --xn;
    --angle;
    bnsize -= 3;
    lxn -= 5;
    nxl -= 3;
    kxl -= 3;
    lxk -= 5;
    --nuid;
    --lcorn;
    --itype;
    icomb_dim1 = *mxcorn;
    icomb_offset = 1 + icomb_dim1;
    icomb -= icomb_offset;
    lnodes_dim1 = *mln;
    lnodes_offset = 1 + lnodes_dim1;
    lnodes -= lnodes_offset;

    /* Function Body */
    *err = FALSE_;
    onlyc = FALSE_;
    npin2 = 0;
/*  RESET EVERYTHING TO BE FREE */
L100:
    if (*graph) {
	lcolor_("YELOW", (ftnlen)5);
    }
    setlop_(mxnd, mln, nloop, &lnodes[lnodes_offset], nbegin, &c__0, err);
    if (*err) {
	goto L270;
    }
/*  GET THE CURRENT CORNERS */
    getcrn_(mxnd, mxcorn, mln, &lnodes[lnodes_offset], ncorn, &lcorn[1], &
	    angle[1], &xn[1], &yn[1], &lxn[5], nloop, nbegin, &onlyc, &pposbl,
	     graph, err);
    if (*err) {
	goto L270;
    }
/*  GET ALL THE COMBINATIONS - NPICK IS THE NUMBER OF COMBINATIONS */
    if (pposbl) {
	comsrt_(mxnd, mxcorn, mxpick, mln, &lnodes[lnodes_offset], &lcorn[1], 
		ncorn, &icomb[icomb_offset], &itype[1], &npick);
    } else {
	npick = 0;
    }
/*  NOW CHECK FOR THE STANDARD PRIMITIVE TYPES */
    found4 = FALSE_;
    found3 = FALSE_;
    found2 = FALSE_;
    redo2 = TRUE_;
    redo3 = TRUE_;
    sidpin = FALSE_;
/*  SKIP THE PRIMITIVES IF SIZEIT IS IN EFFECT */
    if (*sizeit || *nextpr != 0) {
	goto L130;
    }
/*  SET UP THE MINIMUM ACCEPTABLE QUALITIES */
    best2 = (float)3.;
    best3 = (float)3.;
    best4 = (float)4.;
    i__1 = npick;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*  NOW GET THE BEST RECTANGLE COMBINATION */
	if (itype[i__] < 5) {
	    if (itype[i__] == 4) {
		qual4_(mxnd, mxcorn, mln, ncorn, &lcorn[1], &lnodes[
			lnodes_offset], &icomb[i__ * icomb_dim1 + 1], &angle[
			1], &lxn[5], itest, ltest, &qual, &posbl4, err);
		if (*err) {
		    goto L270;
		}
/*  GET THE RECTANGLE INTERPRETATION */
		if (posbl4) {
		    if (qual < best4) {
			is2c = 0;
			ibest4 = i__;
			best4 = qual;
			found4 = TRUE_;
		    }
		}
/*  NOW GET THE BEST TRIANGLE COMBINATION */
	    } else if (itype[i__] == 3) {
		qual3_(mxnd, mxcorn, mln, ncorn, &lcorn[1], &lnodes[
			lnodes_offset], &icomb[i__ * icomb_dim1 + 1], &angle[
			1], &lxn[5], itest, ltest, &qual, &posbl3, &posbl4, 
			err);
		if (*err) {
		    goto L270;
		}
/*  GET THE PURE TRIANGLE INTERPRETATION */
		if (posbl3) {
		    if (qual < best3) {
			ibest3 = i__;
			best3 = qual;
			found3 = TRUE_;
			redo3 = TRUE_;
		    }
/*  TRY A CHANGE TO A RECTANGLE - OR RATHER SET THE RIGHT ROW FOR */
/*  THE PROPER CONCLUSION OF A TRIANGLE */
		} else if (posbl4) {
/*  MAKE SURE THAT THE RESULTING CORNER IS NOT ON A BAD SIDE */
		    ch3to4_(mxnd, mxcorn, mln, ncorn, &lcorn[1], &lnodes[
			    lnodes_offset], &icomb[i__ * icomb_dim1 + 1], &
			    angle[1], itest, ltest, &qual, &posbl4, &ichng);
		    if (lxn[(ichng << 2) + 2] > 0 || lxn[(ichng << 2) + 2] < 
			    0 && cornp_(&angle[ichng])) {
/*  SEE IF WE SHOULD KEEP IT BASED ON QUALITY */
			if (qual < best3) {
			    ibest3 = i__;
			    best3 = qual;
			    found3 = TRUE_;
			    redo3 = FALSE_;
			    nbeg34 = *nbegin;
			    keep3_(itest, ltest, &nbeg34, &nend34);
			}
		    }
/*                  IF ((POSBL4) .AND. */
/*     &               (QUAL .LT. BEST4)) THEN */
/*                     IS2C = II */
/*                     IBEST4 = I */
/*                     BEST4 = QUAL */
/*                     FOUND4 = .TRUE. */
/*                  ENDIF */
		}
/*  NOW GET THE BEST SEMICIRCLE COMBINATION */
	    } else if (itype[i__] == 2) {
		qual2n_(mxnd, mxcorn, mln, ncorn, &lcorn[1], &lnodes[
			lnodes_offset], &icomb[i__ * icomb_dim1 + 1], &bnsize[
			3], &angle[1], &lxn[5], itest, ltest, &qual, &posbl2, 
			&posbl3, &rowchn, &sidpin, &istart, &iend, jpinch, &
			npin2, err);
		if (*err) {
		    goto L270;
		}
/*               CALL QUAL2 (MXND, MXCORN, MLN, NCORN, LCORN, LNODES, */
/*     &            ICOMB(1, I), BNSIZE, ANGLE, LXN, ITEST, LTEST, QUAL, */
/*     &            POSBL2, POSBL3, ROWCHN, ISTART, IEND) */
/*  GET THE SEMICIRCLE INTERPRETATION */
		if (posbl2) {
		    if (qual < best2) {
			is2c = 0;
			ibest2 = i__;
			best2 = qual;
			found2 = TRUE_;
			if (sidpin) {
			    npinch = npin2;
			    i__2 = npinch;
			    for (in = 1; in <= i__2; ++in) {
				ipinch[in - 1] = jpinch[in - 1];
/* L110: */
			    }
			} else {
			    sidpin = FALSE_;
			    if (rowchn) {
				nbeg24 = istart;
				nend24 = iend;
				redo2 = FALSE_;
			    } else {
				redo2 = TRUE_;
			    }
			}
		    }
/*  TRY A CHANGE TO A RECTANGLE - OR RATHER SET THE RIGHT ROW FOR */
/*  THE PROPER CONCLUSION OF A TRIANGLE */
		} else if (posbl3) {
/*  MAKE SURE THAT THE RESULTING CORNER IS NOT ON A BAD SIDE */
		    if (lxn[(iend << 2) + 2] > 0 || lxn[(iend << 2) + 2] < 0 
			    && cornp_(&angle[iend])) {
/*  SEE IF WE SHOULD KEEP IT BASED ON QUALITY */
			if (qual < best3) {
			    ibest3 = i__;
			    best3 = qual;
			    found3 = TRUE_;
			    redo3 = FALSE_;
			    nbeg34 = istart;
			    nend34 = iend;
			}
		    }
		}
	    }
	}
/* L120: */
    }
L130:
/*  FOR NOW, THE RECTANGLE WILL ALWAYS WIN, ETC. */
/*  TAKE THE RECTANGLE */
    if (found4) {
	setlop_(mxnd, mln, nloop, &lnodes[lnodes_offset], nbegin, &c__3, err);
	if (*err) {
	    goto L270;
	}
	i__1 = *ncorn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (icomb[i__ + ibest4 * icomb_dim1] == 1) {
		lnodes[lcorn[i__] * lnodes_dim1 + 1] = 1;
		if (*graph) {
		    isqr = lcorn[i__];
		    x[0] = xn[isqr];
		    y[0] = yn[isqr];
		    symbol_(&c__1, x, y, "SQUARE", (ftnlen)6);
		    sflush_();
		}
	    }
/* L140: */
	}
	if (is2c > 0) {
	    *nbegin = is2c;
	    lnodes[*nbegin * lnodes_dim1 + 1] = 1;
	    if (*graph) {
		isqr = *nbegin;
		x[0] = xn[isqr];
		y[0] = yn[isqr];
		symbol_(&c__1, x, y, "SQUARE", (ftnlen)6);
		sflush_();
	    }
	} else {
	    i__1 = *ncorn;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (icomb[i__ + ibest4 * icomb_dim1] == 1) {
		    *nbegin = lcorn[i__];
		    ii = i__;
		    goto L160;
		}
/* L150: */
	    }
	    *nbegin = lcorn[1];
L160:
	    ;
	}
/*  TAKE THE TRIANGLE */
    } else if (found3) {
	setlop_(mxnd, mln, nloop, &lnodes[lnodes_offset], nbegin, &c__3, err);
	if (*err) {
	    goto L270;
	}
	i__1 = *ncorn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (icomb[i__ + ibest3 * icomb_dim1] == 1) {
		lnodes[lcorn[i__] * lnodes_dim1 + 1] = 1;
		if (*graph) {
		    isqr = lcorn[i__];
		    x[0] = xn[isqr];
		    y[0] = yn[isqr];
		    symbol_(&c__1, x, y, "SQUARE", (ftnlen)6);
		    sflush_();
		}
	    }
/* L170: */
	}
	if (redo3) {
	    i__1 = *ncorn;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (icomb[i__ + ibest3 * icomb_dim1] == 1) {
		    *nbegin = lcorn[i__];
		    ii = i__;
		    goto L190;
		}
/* L180: */
	    }
	    *nbegin = lcorn[1];
	    ii = ibest3;
	} else {
	    *nbegin = nbeg34;
	    *nend = nend34;
	    lnodes[*nbegin * lnodes_dim1 + 1] = 1;
	    lnodes[*nend * lnodes_dim1 + 1] = 1;
	    goto L260;
	}
L190:
/*  OTHERWISE TAKE THE SEMICIRCLE */
	;
    } else if (found2) {
/*  IF THE BEST SEMICIRCLE MUST BE TUCKED, THEN DO SO AND THEN */
/*  REDO THE WHOLE SORTING - A RECTANGLE SHOULD RESULT */
	if (sidpin) {
	    i__1 = npinch;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		*nbegin = lnodes[ipinch[i__ - 1] * lnodes_dim1 + 2];
/*  MAR   K THE SMOOTHING */
		marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
			lnodes[lnodes_offset], &lnodes[ipinch[i__ - 1] * 
			lnodes_dim1 + 2], err);
		if (*err) {
		    goto L270;
		}
		marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
			lnodes[lnodes_offset], &lnodes[lnodes[ipinch[i__ - 1] 
			* lnodes_dim1 + 2] * lnodes_dim1 + 2], err);
		if (*err) {
		    goto L270;
		}
		marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
			lnodes[lnodes_offset], &lnodes[ipinch[i__ - 1] * 
			lnodes_dim1 + 3], err);
		if (*err) {
		    goto L270;
		}
		marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
			lnodes[lnodes_offset], &lnodes[lnodes[ipinch[i__ - 1] 
			* lnodes_dim1 + 3] * lnodes_dim1 + 3], err);
		if (*err) {
		    goto L270;
		}
		marksm_(mxnd, mln, &lxk[5], &kxl[3], &nxl[3], &lxn[5], &
			lnodes[lnodes_offset], &lnodes[lnodes[lnodes[ipinch[
			i__ - 1] * lnodes_dim1 + 3] * lnodes_dim1 + 3] * 
			lnodes_dim1 + 3], err);
		if (*err) {
		    goto L270;
		}
		tuck_(mxnd, mln, &nuid[1], &xn[1], &yn[1], &lxk[5], &kxl[3], &
			nxl[3], &lxn[5], &lnodes[lnodes_offset], iavail, 
			navail, lll, kkk, nnn, &ipinch[i__ - 1], nloop, graph,
			 noroom, err);
		if (*err) {
		    goto L270;
		}
		if (*video) {
		    rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax,
			     ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)
			    3);
		    snapit_(&c__1);
		}
/* L200: */
	    }
	    nnn2 = 1;
	    filsmo_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[
		    3], &lxn[5], lll, nnn, &nnn2, &lnodes[lnodes_offset], &
		    bnsize[3], nloop, xmin, xmax, ymin, ymax, zmin, zmax, 
		    dev1, kreg, (ftnlen)3);
	    lupang_(mxnd, mln, &xn[1], &yn[1], &zn[1], &lxk[5], &kxl[3], &nxl[
		    3], &lxn[5], nloop, &angle[1], &lnodes[lnodes_offset], 
		    nbegin, lll, xmin, xmax, ymin, ymax, zmin, zmax, dev1, 
		    kreg, err, (ftnlen)3);
	    if (*err) {
		goto L270;
	    }
	    if (*graph || *video) {
		rplotl_(mxnd, &xn[1], &yn[1], &zn[1], &nxl[3], xmin, xmax, 
			ymin, ymax, zmin, zmax, lll, dev1, kreg, (ftnlen)3);
		if (*video) {
		    snapit_(&c__1);
		}
	    }
	    goto L100;
	} else {
	    setlop_(mxnd, mln, nloop, &lnodes[lnodes_offset], nbegin, &c__3, 
		    err);
	    if (*err) {
		goto L270;
	    }
	    i__1 = *ncorn;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (icomb[i__ + ibest2 * icomb_dim1] == 1) {
		    lnodes[lcorn[i__] * lnodes_dim1 + 1] = 1;
		    if (*graph) {
			isqr = lcorn[i__];
			x[0] = xn[isqr];
			y[0] = yn[isqr];
			symbol_(&c__1, x, y, "SQUARE", (ftnlen)6);
			sflush_();
		    }
		}
/* L210: */
	    }
	    if (redo2) {
		i__1 = *ncorn;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    if (icomb[i__ + ibest2 * icomb_dim1] == 1) {
			*nbegin = lcorn[i__];
			ii = i__;
			goto L230;
		    }
/* L220: */
		}
		*nbegin = lcorn[1];
	    } else {
		*nbegin = nbeg24;
		*nend = nend24;
		lnodes[*nbegin * lnodes_dim1 + 1] = 1;
		lnodes[*nend * lnodes_dim1 + 1] = 1;
		goto L260;
	    }
L230:
	    ;
	}
/*  CHECK FOR A ONE SIDED SEMICIRCLE */
    } else if (*ncorn == 2 && (lnodes[lcorn[1] * lnodes_dim1 + 7] == 1 || 
	    lnodes[lcorn[2] * lnodes_dim1 + 7] == 1)) {
	if (lnodes[lcorn[1] * lnodes_dim1 + 7] == 1) {
	    *nbegin = lcorn[1];
	    *nend = lcorn[2];
	} else {
	    *nbegin = lcorn[2];
	    *nend = lcorn[1];
	}
	goto L260;
/*  OTHERWISE, THE DEFAULT IS TO JUST START AT THE NEXT CORNER */
    } else if (*ncorn > 0) {
	setlop_(mxnd, mln, nloop, &lnodes[lnodes_offset], nbegin, &c__3, err);
	if (*err) {
	    goto L270;
	}
	ii = 0;
	*nend = 0;
	inode = *nbegin;
	i__1 = *nloop + 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ndstat_(&inode, &lxn[(inode << 2) + 1], &angle[inode], &istat);
	    lnodes[inode * lnodes_dim1 + 1] = istat;
/*  SAVE THE FIRST NATURAL CORNER AS THE START */
	    if (ii == 0 && istat == 1) {
		*nbegin = inode;
		ii = 1;
/*  A ROW END HAS BEEN FOUND */
	    } else if (istat == 1) {
		*nend = inode;
		goto L260;
	    }
	    inode = lnodes[inode * lnodes_dim1 + 3];
/* L240: */
	}
/*  THE ROW IS A CLOSED LOOP BACK TO THE SAME CORNER */
	if (ii != 0 && *nend == 0) {
	    *nend = *nbegin;
/*  THE ROW DOESN'T CONTAIN ANY TRUE CORNERS - TREAT IT AS A CIRCLE */
	} else {
	    setcir_(mxnd, mln, nloop, &lnodes[lnodes_offset], nbegin, err);
	    *nend = *nbegin;
	}
	goto L260;
/*  NO CORNERS - JUST SET EVERYTHING TO BE A SIDE */
    } else {
	setlop_(mxnd, mln, nloop, &lnodes[lnodes_offset], nbegin, &c__3, err);
	if (*err) {
	    goto L270;
	}
	*nend = *nbegin;
	goto L260;
    }
/*  FIND THE NEXT NATURAL CORNER */
    *nend = *nbegin;
    jj = 0;
    i__1 = *ncorn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	inode = lcorn[i__];
	if (lnodes[inode * lnodes_dim1 + 1] == 1 && inode != *nbegin) {
	    if (jj == 0 && i__ < ii || jj < ii && i__ > ii) {
		jj = i__;
		*nend = inode;
	    }
	}
/* L250: */
    }
L260:
    if (*graph) {
/*  5 IS PINK; 4 IS BLUE; 3 IS YELLOW; 0 IS BLACK ; 7 IS WHITE; 1 IS RED */
	lcolor_("PINK ", (ftnlen)5);
	x[0] = xn[*nbegin];
	y[0] = yn[*nbegin];
	symbol_(&c__1, x, y, "SQUARE", (ftnlen)6);
	x[0] = xn[*nend];
	y[0] = yn[*nend];
	symbol_(&c__1, x, y, "SQUARE", (ftnlen)6);
	sflush_();
	lcolor_("WHITE", (ftnlen)5);
    }
L270:
    return 0;
} /* getrow_ */

#ifdef __cplusplus
	}
#endif

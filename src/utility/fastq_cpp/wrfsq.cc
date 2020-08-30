/* wrfsq.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int wrfsq_(integer *iunit, integer *mp, integer *ml, integer 
	*ms, integer *mr, integer *msnap, integer *msc, integer *mcom, 
	integer *icom, integer *jcom, char *cin, real *rin, integer *iin, 
	integer *kin, integer *n, integer *ipoint, real *coor, integer *
	ipboun, integer *iline, integer *ltype, integer *nint, real *factor, 
	integer *lcon, integer *ilboun, integer *isboun, integer *iside, 
	integer *nlps, integer *ifline, integer *illist, integer *ibarst, 
	integer *jmat, integer *jcent, integer *nlpb, integer *jfline, 
	integer *jllist, integer *iregn, integer *imat, integer *nspr, 
	integer *ifside, integer *islist, integer *irpb, integer *ipbf, 
	integer *nppf, integer *ifpb, integer *listpb, integer *ilbf, integer 
	*nlpf, integer *iflb, integer *listlb, integer *isbf, integer *nspf, 
	integer *ifsb, integer *listsb, integer *linkp, integer *linkl, 
	integer *links, integer *linkb, integer *linkr, integer *linksc, 
	integer *linkpb, integer *linklb, integer *linksb, integer *iwtpbf, 
	integer *iwtlbf, integer *iwtsbf, real *rsize, integer *ifhole, 
	integer *nhpr, integer *ihlist, integer *irgflg, integer *ischm, char 
	*scheme, char *number, char *defsch, real *defsiz, char *title, 
	logical *optim, logical *three, logical *eight, logical *nine, 
	logical *snap, real *snapdx, integer *nsnap, logical *regwrt, logical 
	*barwrt, ftnlen cin_len, ftnlen scheme_len, ftnlen number_len, ftnlen 
	defsch_len, ftnlen title_len)
{
    /* Initialized data */

    static char type__[5*7+1] = "  STR CORN CIRC CIRM PARA CIRR ELIP";

    /* Format strings */
    static char fmt_10010[] = "(\002 TITLE \002,/,\002 \002,a)";
    static char fmt_10020[] = "(\002 POINT \002,i5,2(5x,1pe14.7))";
    static char fmt_10030[] = "(\002 LINE  \002,i5,1x,a5,4(1x,i5),2x,f6.4)";
    static char fmt_10040[] = "(a7,11(1x,i5),\002 *\002)";
    static char fmt_10050[] = "(7x,11(1x,i5),\002 *\002)";
    static char fmt_10060[] = "(a7,11(1x,i5))";
    static char fmt_10070[] = "(7x,11(1x,i5))";
    static char fmt_10080[] = "(a7,1x,1pe14.7,2x,i5)";
    static char fmt_10110[] = "(\002 SCHEME\002,i5,1x,a)";
    static char fmt_10000[] = "(\002      \002)";
    static char fmt_10100[] = "(\002 WEIGHT\002,1x,a5,3(1x,i5))";
    static char fmt_10090[] = "(\002 WEIGHT\002,1x,a5,4(1x,i5))";
    static char fmt_10120[] = "(\002 RENUM \002,a5,1x,a)";
    static char fmt_10130[] = "(\002 RENUM \002)";
    static char fmt_10140[] = "(\002 THREE \002)";
    static char fmt_10150[] = "(\002 EIGHT \002)";
    static char fmt_10160[] = "(\002 NINE  \002)";
    static char fmt_10180[] = "(\002 SNAP ON\002)";
    static char fmt_10190[] = "(\002 SNAP OFF\002)";
    static char fmt_10200[] = "(a7,5(1x,1pe13.6),\002 *\002)";
    static char fmt_10210[] = "(7x,5(1x,1pe13.6),\002 *\002)";
    static char fmt_10220[] = "(a7,5(1x,1pe13.6))";
    static char fmt_10230[] = "(7x,5(1x,1pe13.6))";
    static char fmt_10170[] = "(\002 EXIT  \002)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1, r__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, j, k, l, i1, i2, n1, n2, id[13], ii, jj, kk, ll, iz, 
	    len;
    static logical flag__;
    static real xadd, yadd;
    static logical ians;
    static integer kmax;
    static logical star;
    extern /* Subroutine */ int flagd_(integer *, integer *, integer *, 
	    integer *, logical *), check_(integer *, integer *, integer *), 
	    geti12_(integer *, integer *, integer *, char *, integer *, 
	    integer *, integer *, integer *, integer *, ftnlen);
    static integer izero;
    static char dummy[72];
    static integer kount;
    static logical addlnk;
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen);
    static integer ifound, iostat;
    extern /* Subroutine */ int strlng_(char *, integer *, ftnlen);
    static logical gowrit;
    extern /* Subroutine */ int intrup_(char *, logical *, integer *, integer 
	    *, integer *, char *, integer *, real *, integer *, ftnlen, 
	    ftnlen), ltsort_(integer *, integer *, integer *, integer *, 
	    logical *);

    /* Fortran I/O blocks */
    static cilist io___22 = { 0, 0, 0, fmt_10010, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_10020, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_10030, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_10040, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_10050, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_10060, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_10070, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_10040, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_10050, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_10060, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_10070, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_10040, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_10050, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_10060, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_10070, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_10080, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_10080, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_10040, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_10050, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_10060, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_10070, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_10040, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_10050, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_10060, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_10070, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_10110, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_10110, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_10040, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_10050, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_10060, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_10070, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_10000, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_10040, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_10050, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_10060, 0 };
    static cilist io___65 = { 0, 0, 0, fmt_10070, 0 };
    static cilist io___66 = { 0, 0, 0, fmt_10000, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_10100, 0 };
    static cilist io___68 = { 0, 0, 0, fmt_10040, 0 };
    static cilist io___69 = { 0, 0, 0, fmt_10050, 0 };
    static cilist io___70 = { 0, 0, 0, fmt_10060, 0 };
    static cilist io___71 = { 0, 0, 0, fmt_10070, 0 };
    static cilist io___72 = { 0, 0, 0, fmt_10000, 0 };
    static cilist io___73 = { 0, 0, 0, fmt_10090, 0 };
    static cilist io___74 = { 0, 0, 0, fmt_10100, 0 };
    static cilist io___75 = { 0, 0, 0, fmt_10040, 0 };
    static cilist io___76 = { 0, 0, 0, fmt_10050, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_10060, 0 };
    static cilist io___78 = { 0, 0, 0, fmt_10070, 0 };
    static cilist io___79 = { 0, 0, 0, fmt_10000, 0 };
    static cilist io___80 = { 0, 0, 0, fmt_10090, 0 };
    static cilist io___81 = { 0, 0, 0, fmt_10100, 0 };
    static cilist io___82 = { 0, 0, 0, fmt_10120, 0 };
    static cilist io___83 = { 0, 0, 0, fmt_10130, 0 };
    static cilist io___84 = { 0, 0, 0, fmt_10140, 0 };
    static cilist io___85 = { 0, 0, 0, fmt_10150, 0 };
    static cilist io___86 = { 0, 0, 0, fmt_10160, 0 };
    static cilist io___87 = { 0, 0, 0, fmt_10180, 0 };
    static cilist io___88 = { 0, 0, 0, fmt_10190, 0 };
    static cilist io___89 = { 0, 0, 0, fmt_10200, 0 };
    static cilist io___90 = { 0, 0, 0, fmt_10210, 0 };
    static cilist io___91 = { 0, 0, 0, fmt_10220, 0 };
    static cilist io___92 = { 0, 0, 0, fmt_10230, 0 };
    static cilist io___93 = { 0, 0, 0, fmt_10200, 0 };
    static cilist io___94 = { 0, 0, 0, fmt_10210, 0 };
    static cilist io___95 = { 0, 0, 0, fmt_10220, 0 };
    static cilist io___96 = { 0, 0, 0, fmt_10230, 0 };
    static cilist io___97 = { 0, 0, 0, fmt_10170, 0 };


/* *********************************************************************** */
/*  SUBROUTINE WRFSQ  =  WRITES FASTQ CARD FILE */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     FASTQ  =  A PROGRAM TO QUICKLY PREPARE FASTQ INPUT */
/* *********************************************************************** */
    /* Parameter adjustments */
    iwtsbf -= 4;
    iwtlbf -= 4;
    iwtpbf -= 4;
    linkpb -= 3;
    linkp -= 3;
    listpb -= 3;
    --ifpb;
    --nppf;
    --ipbf;
    --ipboun;
    coor -= 3;
    --ipoint;
    linksb -= 3;
    linklb -= 3;
    linkl -= 3;
    listsb -= 3;
    --ifsb;
    --nspf;
    --isbf;
    listlb -= 3;
    --iflb;
    --nlpf;
    --ilbf;
    --isboun;
    --ilboun;
    lcon -= 4;
    --factor;
    --nint;
    --ltype;
    --iline;
    linkb -= 3;
    links -= 3;
    --jllist;
    --jfline;
    --nlpb;
    --jcent;
    --jmat;
    --ibarst;
    --illist;
    --ifline;
    --nlps;
    --iside;
    --irgflg;
    --ihlist;
    --nhpr;
    --ifhole;
    --rsize;
    linksc -= 3;
    linkr -= 3;
    --irpb;
    --islist;
    --ifside;
    --nspr;
    --imat;
    --iregn;
    snapdx -= 3;
    number -= 80;
    scheme -= 72;
    --ischm;
    --kin;
    --iin;
    --rin;
    cin -= 72;
    --n;
    --nsnap;

    /* Function Body */
    addlnk = FALSE_;
    gowrit = FALSE_;
    xadd = (float)0.;
    yadd = (float)0.;
/*  WRITE OUT ONLY THE REGIONS OF INTEREST IF THE REGWRT FLAG HAS BEEN SET */
    if (*regwrt) {
/*  SEE IF A SHIFT OF THE REGION IS NEEDED */
	intrup_("SHIFT REGION", &ians, mcom, icom, jcom, cin + 72, &iin[1], &
		rin[1], &kin[1], (ftnlen)12, (ftnlen)72);
	if (ians) {
	    iz = 0;
	    frefld_(&iz, &iz, "X SHIFT: ", mcom, &iostat, &ifound, &kin[1], 
		    cin + 72, &iin[1], &rin[1], (ftnlen)9, (ftnlen)72);
	    if (ifound > 0) {
		xadd = rin[1];
	    } else {
		xadd = (float)0.;
	    }
	    frefld_(&iz, &iz, "Y SHIFT: ", mcom, &iostat, &ifound, &kin[1], 
		    cin + 72, &iin[1], &rin[1], (ftnlen)9, (ftnlen)72);
	    if (ifound > 0) {
		yadd = rin[1];
	    } else {
		yadd = (float)0.;
	    }
	}
	flag__ = FALSE_;
	flagd_(mp, &n[18], &linkp[3], &ipoint[1], &flag__);
	flagd_(ml, &n[19], &linkl[3], &iline[1], &flag__);
	flagd_(ms, &n[20], &links[3], &iside[1], &flag__);
	flagd_(ms, &n[21], &linkb[3], &ibarst[1], &flag__);
	flagd_(mr, &n[22], &linkr[3], &iregn[1], &flag__);
	mesage_("WRITE REGIONS FROM <I1> TO <I2>", (ftnlen)31);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L100:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    if (i1 > 0) {
		check_(&i1, &i2, &n[22]);
/*  FLAG ALL DATA ASSOCIATED WITH THE REGIONS */
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(mr, &linkr[3], &i__, &ii, &addlnk);
		    if (ii > 0) {
			gowrit = TRUE_;
			iregn[ii] = -(i__2 = iregn[ii], abs(i__2));
			i__2 = ifside[ii] + nspr[ii] - 1;
			for (j = ifside[ii]; j <= i__2; ++j) {
/*  FLAG SIDE DATA */
			    ltsort_(ms, &links[3], &islist[j], &jj, &addlnk);
			    if (islist[j] > 0 && jj > 0) {
				iside[jj] = -(i__3 = iside[jj], abs(i__3));
				i__3 = ifline[jj] + nlps[jj] - 1;
				for (k = ifline[jj]; k <= i__3; ++k) {
				    ltsort_(ml, &linkl[3], &illist[k], &kk, &
					    addlnk);
				    if (kk > 0) {
					iline[kk] = -(i__4 = iline[kk], abs(
						i__4));
					for (l = 1; l <= 3; ++l) {
					    if (lcon[l + kk * 3] > 0) {
			  ltsort_(mp, &linkp[3], &lcon[l + kk * 3], &ll, &
				  addlnk);
			  if (ll > 0) {
			      ipoint[ll] = -(i__4 = ipoint[ll], abs(i__4));
			  }
					    }
/* L110: */
					}
				    }
/* L120: */
				}
/*  FLAG LINE DATA */
			    } else {
				jj = (i__3 = islist[j], abs(i__3));
				ltsort_(ml, &linkl[3], &jj, &kk, &addlnk);
				if (kk > 0) {
				    iline[kk] = -(i__3 = iline[kk], abs(i__3))
					    ;
				    for (l = 1; l <= 3; ++l) {
					if (lcon[l + kk * 3] > 0) {
					    ltsort_(mp, &linkp[3], &lcon[l + 
						    kk * 3], &ll, &addlnk);
					    if (ll > 0) {
			  ipoint[ll] = -(i__3 = ipoint[ll], abs(i__3));
					    }
					}
/* L130: */
				    }
				}
			    }
/* L140: */
			}
		    }
/* L150: */
		}
		goto L100;
	    }
	}
/*  WRITE OUT THE BARSET DATA */
    } else if (*barwrt) {
	flag__ = FALSE_;
	flagd_(mp, &n[18], &linkp[3], &ipoint[1], &flag__);
	flagd_(ml, &n[19], &linkl[3], &iline[1], &flag__);
	flagd_(ms, &n[20], &links[3], &iside[1], &flag__);
	flagd_(ms, &n[21], &linkb[3], &ibarst[1], &flag__);
	flagd_(mr, &n[22], &linkr[3], &iregn[1], &flag__);
	gowrit = FALSE_;
	mesage_("WRITE BARSETS FROM <I1> TO <I2>", (ftnlen)31);
	mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L160:
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    if (i1 > 0) {
		check_(&i1, &i2, &n[21]);
/*  FLAG ALL LINES ASSOCIATED WITH THE BARSETS */
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(ms, &linkb[3], &i__, &ii, &addlnk);
		    if (ii > 0) {
			gowrit = TRUE_;
			ibarst[ii] = -(i__2 = ibarst[ii], abs(i__2));
			i__2 = jfline[ii] + nlpb[ii] - 1;
			for (j = jfline[ii]; j <= i__2; ++j) {
			    jj = (i__3 = jllist[j], abs(i__3));
			    ltsort_(ml, &linkl[3], &jj, &kk, &addlnk);
			    if (kk > 0) {
				iline[kk] = -(i__3 = iline[kk], abs(i__3));
				for (l = 1; l <= 3; ++l) {
				    if (lcon[l + kk * 3] > 0) {
					ltsort_(mp, &linkp[3], &lcon[l + kk * 
						3], &ll, &addlnk);
					if (ll > 0) {
					    ipoint[ll] = -(i__3 = ipoint[ll], 
						    abs(i__3));
					}
				    }
/* L170: */
				}
			    }
/* L180: */
			}
		    }
/* L190: */
		}
		goto L160;
	    }
	}
/*  OTHERWISE FLAG ALL THE DATA TO BE WRITTEN */
    } else {
	flag__ = TRUE_;
	flagd_(mp, &n[18], &linkp[3], &ipoint[1], &flag__);
	flagd_(ml, &n[19], &linkl[3], &iline[1], &flag__);
	flagd_(ms, &n[20], &links[3], &iside[1], &flag__);
	flagd_(ms, &n[21], &linkb[3], &ibarst[1], &flag__);
	flagd_(mr, &n[22], &linkr[3], &iregn[1], &flag__);
	gowrit = TRUE_;
    }
    if (! gowrit) {
	mesage_("** NO DATA HAS BEEN WRITTEN **", (ftnlen)30);
	goto L510;
    }
/*  WRITE OUT THE TITLE */
    strlng_(title, &len, (ftnlen)72);
    io___22.ciunit = *iunit;
    s_wsfe(&io___22);
    do_fio(&c__1, title, len);
    e_wsfe();
/*  WRITE OUT THE POINTS IN ORDER */
    i__1 = n[18];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mp, &linkp[3], &i__, &j, &addlnk);
	if (j > 0 && ipoint[j] < 0) {
	    io___23.ciunit = *iunit;
	    s_wsfe(&io___23);
	    i__3 = (i__2 = ipoint[j], abs(i__2));
	    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
	    r__1 = coor[(j << 1) + 1] + xadd;
	    do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	    r__2 = coor[(j << 1) + 2] + yadd;
	    do_fio(&c__1, (char *)&r__2, (ftnlen)sizeof(real));
	    e_wsfe();
	}
/* L200: */
    }
/*  WRITE OUT THE LINES IN ORDER */
    i__1 = n[19];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(ml, &linkl[3], &i__, &j, &addlnk);
	if (j > 0 && iline[j] < 0) {
	    io___24.ciunit = *iunit;
	    s_wsfe(&io___24);
	    i__3 = (i__2 = iline[j], abs(i__2));
	    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
	    do_fio(&c__1, type__ + (ltype[j] - 1) * 5, (ftnlen)5);
	    do_fio(&c__1, (char *)&lcon[j * 3 + 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&lcon[j * 3 + 2], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&lcon[j * 3 + 3], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nint[j], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&factor[j], (ftnlen)sizeof(real));
	    e_wsfe();
	}
/* L210: */
    }
/*  WRITE OUT THE SIDES IN ORDER */
    i__1 = n[20];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(ms, &links[3], &i__, &j, &addlnk);
	if (j > 0 && iside[j] < 0) {
	    n2 = ifline[j] - 1;
L220:
	    n1 = n2 + 1;
	    if (n1 == ifline[j]) {
		n2 = n1 + 9;
	    } else {
		n2 = n1 + 10;
	    }
/* Computing MIN */
	    i__2 = n2, i__3 = ifline[j] + nlps[j] - 1;
	    n2 = min(i__2,i__3);
	    if (n2 < ifline[j] + nlps[j] - 1) {
		if (n1 == ifline[j]) {
		    io___27.ciunit = *iunit;
		    s_wsfe(&io___27);
		    do_fio(&c__1, " SIDE  ", (ftnlen)7);
		    i__3 = (i__2 = iside[j], abs(i__2));
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    i__4 = n2;
		    for (k = n1; k <= i__4; ++k) {
			do_fio(&c__1, (char *)&illist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___28.ciunit = *iunit;
		    s_wsfe(&io___28);
		    i__2 = n2;
		    for (k = n1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&illist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
		goto L220;
	    } else {
		if (n1 == ifline[j]) {
		    io___29.ciunit = *iunit;
		    s_wsfe(&io___29);
		    do_fio(&c__1, "SIDE  ", (ftnlen)6);
		    i__3 = (i__2 = iside[j], abs(i__2));
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    i__4 = n2;
		    for (k = n1; k <= i__4; ++k) {
			do_fio(&c__1, (char *)&illist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___30.ciunit = *iunit;
		    s_wsfe(&io___30);
		    i__2 = n2;
		    for (k = n1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&illist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
	    }
	}
/* L230: */
    }
/*  WRITE OUT THE BAR SETS IN ORDER */
    i__1 = n[21];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(ms, &linkb[3], &i__, &j, &addlnk);
	if (j > 0 && ibarst[j] < 0) {
	    n2 = jfline[j] - 1;
L240:
	    n1 = n2 + 1;
	    if (n1 == jfline[j]) {
		n2 = n1 + 7;
	    } else {
		n2 = n1 + 10;
	    }
/* Computing MIN */
	    i__2 = n2, i__3 = jfline[j] + nlpb[j] - 1;
	    n2 = min(i__2,i__3);
	    if (n2 < jfline[j] + nlpb[j] - 1) {
		if (n1 == jfline[j]) {
		    io___31.ciunit = *iunit;
		    s_wsfe(&io___31);
		    do_fio(&c__1, " BARSET", (ftnlen)7);
		    i__3 = (i__2 = ibarst[j], abs(i__2));
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jmat[j], (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jcent[j], (ftnlen)sizeof(integer));
		    i__4 = n2;
		    for (k = n1; k <= i__4; ++k) {
			do_fio(&c__1, (char *)&jllist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___32.ciunit = *iunit;
		    s_wsfe(&io___32);
		    i__2 = n2;
		    for (k = n1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&jllist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
		goto L240;
	    } else {
		if (n1 == jfline[j]) {
		    io___33.ciunit = *iunit;
		    s_wsfe(&io___33);
		    do_fio(&c__1, "BARSET", (ftnlen)6);
		    i__3 = (i__2 = ibarst[j], abs(i__2));
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jmat[j], (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jcent[j], (ftnlen)sizeof(integer));
		    i__4 = n2;
		    for (k = n1; k <= i__4; ++k) {
			do_fio(&c__1, (char *)&jllist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___34.ciunit = *iunit;
		    s_wsfe(&io___34);
		    i__2 = n2;
		    for (k = n1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&jllist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
	    }
	}
/* L250: */
    }
/*  WRITE OUT THE REGIONS IN ORDER */
    i__1 = n[22];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mr, &linkr[3], &i__, &j, &addlnk);
	if (j > 0 && irgflg[j] <= -1 && iregn[j] <= 0) {
	    n2 = ifside[j] - 1;
L260:
	    n1 = n2 + 1;
	    if (n1 == ifside[j]) {
		n2 = n1 + 8;
	    } else {
		n2 = n1 + 10;
	    }
/* Computing MIN */
	    i__2 = n2, i__3 = ifside[j] + nspr[j] - 1;
	    n2 = min(i__2,i__3);
	    if (n2 < ifside[j] + nspr[j] - 1) {
		if (n1 == ifside[j]) {
		    io___35.ciunit = *iunit;
		    s_wsfe(&io___35);
		    do_fio(&c__1, " REGION", (ftnlen)7);
		    i__3 = (i__2 = iregn[j], abs(i__2));
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&imat[j], (ftnlen)sizeof(integer));
		    i__4 = n2;
		    for (k = n1; k <= i__4; ++k) {
			do_fio(&c__1, (char *)&islist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___36.ciunit = *iunit;
		    s_wsfe(&io___36);
		    i__2 = n2;
		    for (k = n1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&islist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
		goto L260;
	    } else {
		if (n1 == ifside[j]) {
		    io___37.ciunit = *iunit;
		    s_wsfe(&io___37);
		    do_fio(&c__1, "REGION", (ftnlen)6);
		    i__3 = (i__2 = iregn[j], abs(i__2));
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&imat[j], (ftnlen)sizeof(integer));
		    i__4 = n2;
		    for (k = n1; k <= i__4; ++k) {
			do_fio(&c__1, (char *)&islist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___38.ciunit = *iunit;
		    s_wsfe(&io___38);
		    i__2 = n2;
		    for (k = n1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&islist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
	    }
/*  WRITE OUT THE REGION INTERVAL SIZE DATA */
	    if (rsize[j] > (float)0.) {
		io___39.ciunit = *iunit;
		s_wsfe(&io___39);
		do_fio(&c__1, "SIZE  ", (ftnlen)6);
		do_fio(&c__1, (char *)&rsize[j], (ftnlen)sizeof(real));
		i__3 = (i__2 = iregn[j], abs(i__2));
		do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
/* L270: */
    }
    if (*defsiz > (float)0.) {
	io___40.ciunit = *iunit;
	s_wsfe(&io___40);
	do_fio(&c__1, "SIZE  ", (ftnlen)6);
	do_fio(&c__1, (char *)&(*defsiz), (ftnlen)sizeof(real));
	e_wsfe();
    }
/*  WRITE OUT THE HOLES IN ORDER */
    i__1 = n[22];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mr, &linkr[3], &i__, &j, &addlnk);
	if (nhpr[j] > 0 && iregn[j] < 0) {
	    n2 = ifhole[j] - 1;
L280:
	    n1 = n2 + 1;
	    if (n1 == ifhole[j]) {
		n2 = n1 + 9;
	    } else {
		n2 = n1 + 10;
	    }
/* Computing MIN */
	    i__2 = n2, i__3 = ifhole[j] + nhpr[j] - 1;
	    n2 = min(i__2,i__3);
	    if (n2 < ifhole[j] + nhpr[j] - 1) {
		if (n1 == ifhole[j]) {
		    io___41.ciunit = *iunit;
		    s_wsfe(&io___41);
		    do_fio(&c__1, " HOLE  ", (ftnlen)7);
		    i__3 = (i__2 = iregn[j], abs(i__2));
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    i__4 = n2;
		    for (k = n1; k <= i__4; ++k) {
			do_fio(&c__1, (char *)&ihlist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___42.ciunit = *iunit;
		    s_wsfe(&io___42);
		    i__2 = n2;
		    for (k = n1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&ihlist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
		goto L280;
	    } else {
		if (n1 == ifhole[j]) {
		    io___43.ciunit = *iunit;
		    s_wsfe(&io___43);
		    do_fio(&c__1, " HOLE  ", (ftnlen)7);
		    i__3 = (i__2 = iregn[j], abs(i__2));
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    i__4 = n2;
		    for (k = n1; k <= i__4; ++k) {
			do_fio(&c__1, (char *)&ihlist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___44.ciunit = *iunit;
		    s_wsfe(&io___44);
		    i__2 = n2;
		    for (k = n1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&ihlist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
	    }
	}
/* L290: */
    }
/*  WRITE OUT THE GROUPS IN ORDER */
    i__1 = n[22];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mr, &linkr[3], &i__, &j, &addlnk);
	if (j > 0 && irgflg[j] >= 1) {
	    n2 = ifside[j] - 1;
L300:
	    n1 = n2 + 1;
	    if (n1 == ifside[j]) {
		n2 = n1 + 9;
	    } else {
		n2 = n1 + 10;
	    }
/* Computing MIN */
	    i__2 = n2, i__3 = ifside[j] + nspr[j] - 1;
	    n2 = min(i__2,i__3);
	    if (n2 < ifside[j] + nspr[j] - 1) {
		if (n1 == ifside[j]) {
		    io___45.ciunit = *iunit;
		    s_wsfe(&io___45);
		    do_fio(&c__1, " GROUP ", (ftnlen)7);
		    i__3 = (i__2 = iregn[j], abs(i__2));
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    i__4 = n2;
		    for (k = n1; k <= i__4; ++k) {
			do_fio(&c__1, (char *)&islist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___46.ciunit = *iunit;
		    s_wsfe(&io___46);
		    i__2 = n2;
		    for (k = n1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&islist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
		goto L300;
	    } else {
		if (n1 == ifside[j]) {
		    io___47.ciunit = *iunit;
		    s_wsfe(&io___47);
		    do_fio(&c__1, "GROUP ", (ftnlen)6);
		    i__3 = (i__2 = iregn[j], abs(i__2));
		    do_fio(&c__1, (char *)&i__3, (ftnlen)sizeof(integer));
		    i__4 = n2;
		    for (k = n1; k <= i__4; ++k) {
			do_fio(&c__1, (char *)&islist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___48.ciunit = *iunit;
		    s_wsfe(&io___48);
		    i__2 = n2;
		    for (k = n1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&islist[k], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
	    }
	}
/* L310: */
    }
/*  WRITE OUT THE SCHEMES IN ORDER */
    i__1 = n[24];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mr, &linksc[3], &i__, &j, &addlnk);
	if (j > 0) {
	    ltsort_(mr, &linkr[3], &ischm[j], &jj, &addlnk);
	    if (jj > 0 && iregn[jj] < 0) {
		strlng_(scheme + j * 72, &len, (ftnlen)72);
		s_copy(dummy, scheme + j * 72, (ftnlen)72, len);
		io___50.ciunit = *iunit;
		s_wsfe(&io___50);
		do_fio(&c__1, (char *)&ischm[j], (ftnlen)sizeof(integer));
		do_fio(&c__1, dummy, len);
		e_wsfe();
	    }
	}
/* L320: */
    }
    strlng_(defsch, &len, (ftnlen)72);
    izero = 0;
    io___52.ciunit = *iunit;
    s_wsfe(&io___52);
    do_fio(&c__1, (char *)&izero, (ftnlen)sizeof(integer));
    do_fio(&c__1, defsch, len);
    e_wsfe();
/*  WRITE OUT THE BODY LIST */
    n2 = 0;
    kmax = 11;
L330:
    n1 = n2 + 1;
    if (n[9] > 0) {
	kount = 0;
	i__1 = n[9];
	for (i__ = n1; i__ <= i__1; ++i__) {
	    if (irpb[i__] > 0) {
		ltsort_(mr, &linkr[3], &irpb[i__], &j, &addlnk);
	    } else if (irpb[i__] < 0) {
		i__3 = (i__2 = irpb[i__], abs(i__2));
		ltsort_(ms, &linkb[3], &i__3, &j, &addlnk);
	    } else {
		j = 0;
	    }
	    if (irpb[i__] > 0 && j > 0 && iregn[j] < 0 || irpb[i__] < 0 && j 
		    > 0 && ibarst[j] < 0) {
		++kount;
		id[kount - 1] = irpb[i__];
	    }
	    n2 = i__;
	    if (kount == kmax) {
		goto L350;
	    }
/* L340: */
	}
L350:
	if (n2 < n[9]) {
	    if (n1 == 1) {
		io___56.ciunit = *iunit;
		s_wsfe(&io___56);
		do_fio(&c__1, " BODY  ", (ftnlen)7);
		i__1 = kount;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    do_fio(&c__1, (char *)&id[i__ - 1], (ftnlen)sizeof(
			    integer));
		}
		e_wsfe();
	    } else {
		io___57.ciunit = *iunit;
		s_wsfe(&io___57);
		i__1 = kount;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    do_fio(&c__1, (char *)&id[i__ - 1], (ftnlen)sizeof(
			    integer));
		}
		e_wsfe();
	    }
	    goto L330;
	} else if (kount > 0) {
	    if (n1 == 1) {
		io___58.ciunit = *iunit;
		s_wsfe(&io___58);
		do_fio(&c__1, "BODY  ", (ftnlen)6);
		i__1 = kount;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    do_fio(&c__1, (char *)&id[i__ - 1], (ftnlen)sizeof(
			    integer));
		}
		e_wsfe();
	    } else {
		io___59.ciunit = *iunit;
		s_wsfe(&io___59);
		i__1 = kount;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    do_fio(&c__1, (char *)&id[i__ - 1], (ftnlen)sizeof(
			    integer));
		}
		e_wsfe();
	    }
	} else {
	    io___60.ciunit = *iunit;
	    s_wsfe(&io___60);
	    e_wsfe();
	}
    }
/*  WRITE OUT THE POINT BOUNDARY FLAGS IN ORDER */
    i__1 = n[25];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(mp, &linkpb[3], &i__, &j, &addlnk);
	if (j > 0) {
	    star = FALSE_;
	    n2 = ifpb[j] - 1;
L360:
	    n1 = n2 + 1;
	    if (n1 == ifpb[j]) {
		kmax = 10;
	    } else {
		kmax = 11;
	    }
	    kount = 0;
	    i__2 = ifpb[j] + nppf[j] - 1;
	    for (k = n1; k <= i__2; ++k) {
		ltsort_(mp, &linkp[3], &listpb[(k << 1) + 1], &jj, &addlnk);
		if (jj > 0 && ipoint[jj] < 0) {
		    ++kount;
		    id[kount - 1] = listpb[(k << 1) + 1];
		}
		n2 = k;
		if (kount == kmax) {
		    goto L380;
		}
/* L370: */
	    }
L380:
	    if (n2 < ifpb[j] + nppf[j] - 1) {
		star = TRUE_;
		if (n1 == ifpb[j]) {
		    io___62.ciunit = *iunit;
		    s_wsfe(&io___62);
		    do_fio(&c__1, " POINBC", (ftnlen)7);
		    do_fio(&c__1, (char *)&ipbf[j], (ftnlen)sizeof(integer));
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___63.ciunit = *iunit;
		    s_wsfe(&io___63);
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
		goto L360;
	    } else if (kount > 0) {
		if (n1 == ifpb[j]) {
		    io___64.ciunit = *iunit;
		    s_wsfe(&io___64);
		    do_fio(&c__1, " POINBC", (ftnlen)7);
		    do_fio(&c__1, (char *)&ipbf[j], (ftnlen)sizeof(integer));
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___65.ciunit = *iunit;
		    s_wsfe(&io___65);
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
	    } else if (star) {
		io___66.ciunit = *iunit;
		s_wsfe(&io___66);
		e_wsfe();
	    }
	    if (iwtpbf[j * 3 + 1] > 0) {
		io___67.ciunit = *iunit;
		s_wsfe(&io___67);
		do_fio(&c__1, "POINT", (ftnlen)5);
		do_fio(&c__1, (char *)&ipbf[j], (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iwtpbf[j * 3 + 1], (ftnlen)sizeof(
			integer));
		do_fio(&c__1, (char *)&iwtpbf[j * 3 + 2], (ftnlen)sizeof(
			integer));
		e_wsfe();
	    }
	}
/* L390: */
    }
/*  WRITE OUT THE LINE BOUNDARY FLAGS IN ORDER */
    i__1 = n[26];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(ml, &linklb[3], &i__, &j, &addlnk);
	if (j > 0) {
	    star = FALSE_;
	    n2 = iflb[j] - 1;
L400:
	    n1 = n2 + 1;
	    if (n1 == iflb[j]) {
		kmax = 10;
	    } else {
		kmax = 11;
	    }
	    kount = 0;
	    i__2 = iflb[j] + nlpf[j] - 1;
	    for (k = n1; k <= i__2; ++k) {
		ltsort_(ml, &linkl[3], &listlb[(k << 1) + 1], &jj, &addlnk);
		if (jj > 0 && iline[jj] < 0) {
		    ++kount;
		    id[kount - 1] = listlb[(k << 1) + 1];
		}
		n2 = k;
		if (kount == kmax) {
		    goto L420;
		}
/* L410: */
	    }
L420:
	    if (n2 < iflb[j] + nlpf[j] - 1) {
		star = TRUE_;
		if (n1 == iflb[j]) {
		    io___68.ciunit = *iunit;
		    s_wsfe(&io___68);
		    do_fio(&c__1, " NODEBC", (ftnlen)7);
		    do_fio(&c__1, (char *)&ilbf[j], (ftnlen)sizeof(integer));
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___69.ciunit = *iunit;
		    s_wsfe(&io___69);
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
		goto L400;
	    } else if (kount > 0) {
		if (n1 == iflb[j]) {
		    io___70.ciunit = *iunit;
		    s_wsfe(&io___70);
		    do_fio(&c__1, " NODEBC", (ftnlen)7);
		    do_fio(&c__1, (char *)&ilbf[j], (ftnlen)sizeof(integer));
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___71.ciunit = *iunit;
		    s_wsfe(&io___71);
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
	    } else if (star) {
		io___72.ciunit = *iunit;
		s_wsfe(&io___72);
		e_wsfe();
	    }
	    if (iwtlbf[j * 3 + 1] != 0) {
		if (iwtlbf[j * 3 + 3] > 0) {
		    io___73.ciunit = *iunit;
		    s_wsfe(&io___73);
		    do_fio(&c__1, "  LINE", (ftnlen)6);
		    do_fio(&c__1, (char *)&ilbf[j], (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&iwtlbf[j * 3 + 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&iwtlbf[j * 3 + 2], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&iwtlbf[j * 3 + 3], (ftnlen)sizeof(
			    integer));
		    e_wsfe();
		} else {
		    io___74.ciunit = *iunit;
		    s_wsfe(&io___74);
		    do_fio(&c__1, "  LINE", (ftnlen)6);
		    do_fio(&c__1, (char *)&ilbf[j], (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&iwtlbf[j * 3 + 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&iwtlbf[j * 3 + 2], (ftnlen)sizeof(
			    integer));
		    e_wsfe();
		}
	    }
	}
/* L430: */
    }
/*  WRITE OUT THE SIDE BOUNDARY FLAGS IN ORDER */
    i__1 = n[27];
    for (i__ = 1; i__ <= i__1; ++i__) {
	ltsort_(ml, &linksb[3], &i__, &j, &addlnk);
	if (j > 0) {
	    star = FALSE_;
	    n2 = ifsb[j] - 1;
L440:
	    n1 = n2 + 1;
	    if (n1 == ifsb[j]) {
		kmax = 10;
	    } else {
		kmax = 11;
	    }
	    kount = 0;
	    i__2 = ifsb[j] + nspf[j] - 1;
	    for (k = n1; k <= i__2; ++k) {
		ltsort_(ml, &linkl[3], &listsb[(k << 1) + 1], &jj, &addlnk);
		if (jj > 0 && iline[jj] < 0) {
		    ++kount;
		    id[kount - 1] = listsb[(k << 1) + 1];
		}
		n2 = k;
		if (kount == kmax) {
		    goto L460;
		}
/* L450: */
	    }
L460:
	    if (n2 < ifsb[j] + nspf[j] - 1) {
		star = TRUE_;
		if (n1 == ifsb[j]) {
		    io___75.ciunit = *iunit;
		    s_wsfe(&io___75);
		    do_fio(&c__1, " ELEMBC", (ftnlen)7);
		    do_fio(&c__1, (char *)&isbf[j], (ftnlen)sizeof(integer));
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___76.ciunit = *iunit;
		    s_wsfe(&io___76);
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
		goto L440;
	    } else if (kount > 0) {
		if (n1 == ifsb[j]) {
		    io___77.ciunit = *iunit;
		    s_wsfe(&io___77);
		    do_fio(&c__1, " ELEMBC", (ftnlen)7);
		    do_fio(&c__1, (char *)&isbf[j], (ftnlen)sizeof(integer));
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		} else {
		    io___78.ciunit = *iunit;
		    s_wsfe(&io___78);
		    i__2 = kount;
		    for (k = 1; k <= i__2; ++k) {
			do_fio(&c__1, (char *)&id[k - 1], (ftnlen)sizeof(
				integer));
		    }
		    e_wsfe();
		}
	    } else if (star) {
		io___79.ciunit = *iunit;
		s_wsfe(&io___79);
		e_wsfe();
	    }
	    if (iwtsbf[j * 3 + 1] != 0) {
		if (iwtsbf[j * 3 + 3] > 0) {
		    io___80.ciunit = *iunit;
		    s_wsfe(&io___80);
		    do_fio(&c__1, " SIDE", (ftnlen)5);
		    do_fio(&c__1, (char *)&isbf[j], (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&iwtsbf[j * 3 + 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&iwtsbf[j * 3 + 2], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&iwtsbf[j * 3 + 3], (ftnlen)sizeof(
			    integer));
		    e_wsfe();
		} else {
		    io___81.ciunit = *iunit;
		    s_wsfe(&io___81);
		    do_fio(&c__1, " SIDE", (ftnlen)5);
		    do_fio(&c__1, (char *)&isbf[j], (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&iwtsbf[j * 3 + 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&iwtsbf[j * 3 + 2], (ftnlen)sizeof(
			    integer));
		    e_wsfe();
		}
	    }
	}
/* L470: */
    }
/*  WRITE OUT THE RENUMBERING CARDS */
    if (*optim) {
	if (n[28] > 0) {
	    i__1 = n[28];
	    for (i__ = 1; i__ <= i__1; ++i__) {
		s_copy(dummy, number + (i__ * 80 + 5), (ftnlen)72, (ftnlen)72)
			;
		strlng_(dummy, &len, (ftnlen)72);
		io___82.ciunit = *iunit;
		s_wsfe(&io___82);
		do_fio(&c__1, number + i__ * 80, (ftnlen)5);
		do_fio(&c__1, number + (i__ * 80 + 5), len + 1);
		e_wsfe();
/* L480: */
	    }
	} else {
	    io___83.ciunit = *iunit;
	    s_wsfe(&io___83);
	    e_wsfe();
	}
    }
/*  WRITE OUT THREE NODE, EIGHT NODE, OR NINE NODE FLAG */
    if (*three) {
	io___84.ciunit = *iunit;
	s_wsfe(&io___84);
	e_wsfe();
    }
    if (*eight) {
	io___85.ciunit = *iunit;
	s_wsfe(&io___85);
	e_wsfe();
    } else if (*nine) {
	io___86.ciunit = *iunit;
	s_wsfe(&io___86);
	e_wsfe();
    }
/*  WRITE DIGITIZER SNAP-TO-GRID FLAG */
    if (nsnap[1] > 0 || nsnap[2] > 0) {
	if (*snap) {
	    io___87.ciunit = *iunit;
	    s_wsfe(&io___87);
	    e_wsfe();
	} else {
	    io___88.ciunit = *iunit;
	    s_wsfe(&io___88);
	    e_wsfe();
	}
/*  WRITE X-GRID LINES */
	if (nsnap[1] > 0) {
	    n2 = 0;
L490:
	    n1 = n2 + 1;
	    n2 = n1 + 4;
	    n2 = min(n2,nsnap[1]);
	    if (n2 < nsnap[1]) {
		if (n1 == 1) {
		    io___89.ciunit = *iunit;
		    s_wsfe(&io___89);
		    do_fio(&c__1, "XGRID ", (ftnlen)6);
		    i__1 = n2;
		    for (k = n1; k <= i__1; ++k) {
			do_fio(&c__1, (char *)&snapdx[(k << 1) + 1], (ftnlen)
				sizeof(real));
		    }
		    e_wsfe();
		} else {
		    io___90.ciunit = *iunit;
		    s_wsfe(&io___90);
		    i__1 = n2;
		    for (k = n1; k <= i__1; ++k) {
			do_fio(&c__1, (char *)&snapdx[(k << 1) + 1], (ftnlen)
				sizeof(real));
		    }
		    e_wsfe();
		}
		goto L490;
	    } else {
		if (n1 == 1) {
		    io___91.ciunit = *iunit;
		    s_wsfe(&io___91);
		    do_fio(&c__1, " XGRID ", (ftnlen)7);
		    i__1 = n2;
		    for (k = n1; k <= i__1; ++k) {
			do_fio(&c__1, (char *)&snapdx[(k << 1) + 1], (ftnlen)
				sizeof(real));
		    }
		    e_wsfe();
		} else {
		    io___92.ciunit = *iunit;
		    s_wsfe(&io___92);
		    i__1 = n2;
		    for (k = n1; k <= i__1; ++k) {
			do_fio(&c__1, (char *)&snapdx[(k << 1) + 1], (ftnlen)
				sizeof(real));
		    }
		    e_wsfe();
		}
	    }
/*  WRITE Y-GRID LINES */
	    if (nsnap[2] > 0) {
		n2 = 0;
L500:
		n1 = n2 + 1;
		n2 = n1 + 4;
		n2 = min(n2,nsnap[2]);
		if (n2 < nsnap[2]) {
		    if (n1 == 1) {
			io___93.ciunit = *iunit;
			s_wsfe(&io___93);
			do_fio(&c__1, " YGRID ", (ftnlen)7);
			i__1 = n2;
			for (k = n1; k <= i__1; ++k) {
			    do_fio(&c__1, (char *)&snapdx[(k << 1) + 2], (
				    ftnlen)sizeof(real));
			}
			e_wsfe();
		    } else {
			io___94.ciunit = *iunit;
			s_wsfe(&io___94);
			i__1 = n2;
			for (k = n1; k <= i__1; ++k) {
			    do_fio(&c__1, (char *)&snapdx[(k << 1) + 2], (
				    ftnlen)sizeof(real));
			}
			e_wsfe();
		    }
		    goto L500;
		} else {
		    if (n1 == 1) {
			io___95.ciunit = *iunit;
			s_wsfe(&io___95);
			do_fio(&c__1, "YGRID ", (ftnlen)6);
			i__1 = n2;
			for (k = n1; k <= i__1; ++k) {
			    do_fio(&c__1, (char *)&snapdx[(k << 1) + 2], (
				    ftnlen)sizeof(real));
			}
			e_wsfe();
		    } else {
			io___96.ciunit = *iunit;
			s_wsfe(&io___96);
			i__1 = n2;
			for (k = n1; k <= i__1; ++k) {
			    do_fio(&c__1, (char *)&snapdx[(k << 1) + 2], (
				    ftnlen)sizeof(real));
			}
			e_wsfe();
		    }
		}
	    }
	}
    }
/*  WRITE EXIT */
    io___97.ciunit = *iunit;
    s_wsfe(&io___97);
    e_wsfe();
    mesage_("FASTQ DATA FILE SUCCESSFULLY WRITTEN", (ftnlen)36);
L510:
    flag__ = FALSE_;
    flagd_(mp, &n[18], &linkp[3], &ipoint[1], &flag__);
    flagd_(ml, &n[19], &linkl[3], &iline[1], &flag__);
    flagd_(ms, &n[20], &links[3], &iside[1], &flag__);
    flagd_(ms, &n[21], &linkb[3], &ibarst[1], &flag__);
    flagd_(mr, &n[22], &linkr[3], &iregn[1], &flag__);
    return 0;
} /* wrfsq_ */

#ifdef __cplusplus
	}
#endif

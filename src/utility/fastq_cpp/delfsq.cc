/* delfsq.f -- translated by f2c (version 20160102).
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
static integer c__6 = 6;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int delfsq_(integer *mp, integer *ml, integer *ms, integer *
	mr, integer *msc, integer *mcom, integer *icom, integer *jcom, char *
	cin, real *rin, integer *iin, integer *kin, integer *n, integer *
	ipboun, integer *ilboun, integer *isboun, integer *nlps, integer *
	ifline, integer *illist, integer *nspr, integer *ifside, integer *
	islist, integer *irpb, integer *ipbf, integer *nppf, integer *ifpb, 
	integer *listpb, integer *ilbf, integer *nlpf, integer *iflb, integer 
	*listlb, integer *isbf, integer *nspf, integer *ifsb, integer *listsb,
	 integer *linkp, integer *linkl, integer *links, integer *linkb, 
	integer *linkr, integer *linksc, integer *linkpb, integer *linklb, 
	integer *linksb, integer *iwtpbf, integer *iwtlbf, integer *iwtsbf, 
	integer *ifhole, integer *nhpr, integer *ihlist, integer *irgflg, 
	char *number, char *defsch, logical *optim, logical *vaxvms, logical *
	wrote, real *time1, logical *batch, char *versn, ftnlen cin_len, 
	ftnlen number_len, ftnlen defsch_len, ftnlen versn_len)
{
    /* Format strings */
    static char fmt_10040[] = "(1x,i5,2x,a72)";
    static char fmt_10050[] = "(8x,a8)";
    static char fmt_10000[] = "(\002 NO HOLES DEFINED IN REGION:\002,i6)";
    static char fmt_10010[] = "(\002 UNDEFINED REGION:\002,i6)";
    static char fmt_10020[] = "(\002 DEFLT: \002,a72)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;

    /* Builtin functions */
    integer s_cmp(char *, char *, ftnlen, ftnlen), s_wsfe(cilist *), do_fio(
	    integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__, j, k, l, i1, i2, k1, k2, iz, len, iptr;
    extern /* Subroutine */ int check_(integer *, integer *, integer *), 
	    geti12_(integer *, integer *, integer *, char *, integer *, 
	    integer *, integer *, integer *, integer *, ftnlen), fexit_(
	    logical *, integer *, integer *, integer *, char *, integer *, 
	    real *, integer *, real *, logical *, char *, ftnlen, ftnlen);
    static integer itest;
    extern /* Subroutine */ int spawn_(logical *);
    static logical addlnk;
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen), linkbc_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, logical *, logical *);
    static logical sideok;
    static integer ifound, iostat;
    static logical lgroup, noroom;
    extern /* Subroutine */ int strlng_(char *, integer *, ftnlen), ltsort_(
	    integer *, integer *, integer *, integer *, logical *), help_fq__(
	    integer *);

    /* Fortran I/O blocks */
    static cilist io___9 = { 0, 6, 0, fmt_10040, 0 };
    static cilist io___10 = { 0, 6, 0, fmt_10050, 0 };
    static cilist io___18 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___19 = { 0, 6, 0, fmt_10010, 0 };
    static cilist io___20 = { 0, 6, 0, fmt_10020, 0 };


/* *********************************************************************** */
/*  SUBROUTINE DELFSQ = DELETES POINTS, LINES, REGIONS, SCHEMES, AND */
/*                      BOUNDARY DEFINITIONS */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     FASTQ = A PROGRAM TO QUICKLY PREPARE QMESH INPUT */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     IANS   = LOGICAL RESPONSE FROM YES-NO QUESTION */
/*     ANS    = CHARACTER RESPONSE FOR MENU CHOICE */
/* *********************************************************************** */
    /* Parameter adjustments */
    iwtpbf -= 4;
    linkpb -= 3;
    linkp -= 3;
    listpb -= 3;
    --ifpb;
    --nppf;
    --ipbf;
    --ipboun;
    iwtsbf -= 4;
    iwtlbf -= 4;
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
    linkb -= 3;
    links -= 3;
    --illist;
    --ifline;
    --nlps;
    --irgflg;
    --ihlist;
    --nhpr;
    --ifhole;
    linksc -= 3;
    linkr -= 3;
    --irpb;
    --islist;
    --ifside;
    --nspr;
    number -= 80;
    --kin;
    --iin;
    --rin;
    cin -= 72;
    --n;

    /* Function Body */
    addlnk = TRUE_;
    iz = 0;
L100:
    if (*icom > *jcom) {
	mesage_(" ", (ftnlen)1);
	frefld_(&iz, &iz, "ENTER DELETE OPTION: ", mcom, &iostat, jcom, &kin[
		1], cin + 72, &iin[1], &rin[1], (ftnlen)21, (ftnlen)72);
	*icom = 1;
    }
    if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned char *)&cin[*
	    icom * 72] == 'p') {
	++(*icom);
	if (n[1] > 0) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("DELETE POINTS <I1> THROUGH <I2>:", (ftnlen)32);
	    mesage_("HIT A RETURN TO END", (ftnlen)19);
L110:
	    if (*icom > *jcom) {
		frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72,
			 &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		*icom = 1;
	    }
	    geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		    ifound, (ftnlen)72);
	    if (ifound > 0) {
		check_(&i1, &i2, &n[18]);
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(mp, &linkp[3], &i__, &iz, &addlnk);
/* L120: */
		}
		goto L110;
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*-----------------------------------*", (ftnlen)37);
	    mesage_("* NO POINTS IN THE CURRENT DATABASE *", (ftnlen)37);
	    mesage_("*-----------------------------------*", (ftnlen)37);
	}
    } else if (*(unsigned char *)&cin[*icom * 72] == 'L' || *(unsigned char *)
	    &cin[*icom * 72] == 'l') {
	++(*icom);
	if (n[2] > 0) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("DELETE LINES <I1> THROUGH <I2>:", (ftnlen)31);
	    mesage_("HIT A RETURN TO END", (ftnlen)19);
L130:
	    if (*icom > *jcom) {
		frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72,
			 &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		*icom = 1;
	    }
	    geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		    ifound, (ftnlen)72);
	    if (ifound > 0) {
		check_(&i1, &i2, &n[19]);
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(ml, &linkl[3], &i__, &iz, &addlnk);
/* L140: */
		}
		goto L130;
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*----------------------------------*", (ftnlen)36);
	    mesage_("* NO LINES IN THE CURRENT DATABASE *", (ftnlen)36);
	    mesage_("*----------------------------------*", (ftnlen)36);
	}
/*  DELETE BAR SET DEFINITIONS */
    } else if (s_cmp(cin + *icom * 72, "BA", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ba", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (n[5] > 0) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("DELETE BAR SETS <I1> THROUGH <I2>:", (ftnlen)34);
	    mesage_("HIT A RETURN TO END", (ftnlen)19);
L150:
	    if (*icom > *jcom) {
		frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72,
			 &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		*icom = 1;
	    }
	    geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		    ifound, (ftnlen)72);
	    if (ifound > 0) {
		check_(&i1, &i2, &n[21]);
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(ms, &linkb[3], &i__, &iz, &addlnk);
/* L160: */
		}
		goto L150;
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*-------------------------------------*", (ftnlen)39);
	    mesage_("* NO BAR SETS IN THE CURRENT DATABASE *", (ftnlen)39);
	    mesage_("*-------------------------------------*", (ftnlen)39);
	}
/*  DELETE THE RENUMBERING CARDS */
    } else if (s_cmp(cin + *icom * 72, "REN", (ftnlen)3, (ftnlen)3) == 0 || 
	    s_cmp(cin + *icom * 72, "ren", (ftnlen)3, (ftnlen)3) == 0) {
	++(*icom);
	if (n[28] > 0) {
	    mesage_(" ", (ftnlen)1);
	    i1 = 1;
	    i2 = n[28];
	    itest = 20;
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		s_wsfe(&io___9);
		do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
		do_fio(&c__1, number + i__ * 80, (ftnlen)72);
		e_wsfe();
		if (s_cmp(number + (i__ * 80 + 72), "        ", (ftnlen)8, (
			ftnlen)8) != 0) {
		    s_wsfe(&io___10);
		    do_fio(&c__1, number + (i__ * 80 + 72), (ftnlen)8);
		    e_wsfe();
		}
/* L170: */
	    }
	    mesage_(" ", (ftnlen)1);
	    mesage_("DELETE RENUMBER CARDS <I1> THROUGH <I2>:", (ftnlen)40);
	    mesage_("HIT A RETURN TO END", (ftnlen)19);
L180:
	    if (*icom > *jcom) {
		frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72,
			 &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		*icom = 1;
	    }
	    geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		    ifound, (ftnlen)72);
	    if (ifound > 0) {
		check_(&i1, &i2, &n[28]);
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    i__2 = n[28];
		    for (j = i__; j <= i__2; ++j) {
			s_copy(number + j * 80, number + (j + 1) * 80, (
				ftnlen)80, (ftnlen)80);
/* L190: */
		    }
		    --n[28];
/* L200: */
		}
		if (n[28] == 0) {
		    *optim = FALSE_;
		}
		goto L180;
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*-------------------------------------------*", (ftnlen)
		    45);
	    if (! (*optim)) {
		mesage_("*        NO RENUMBER CONTROL CARDS          *", (
			ftnlen)45);
	    }
	    mesage_("*        OPTIMIZATION HAS BEEN DISABLED     *", (ftnlen)
		    45);
	    mesage_("*-------------------------------------------*", (ftnlen)
		    45);
	    mesage_(" ", (ftnlen)1);
	    *optim = FALSE_;
	}
/*  DELETE THE REGIONS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'R' || *(unsigned char *)
	    &cin[*icom * 72] == 'r') {
	++(*icom);
	lgroup = TRUE_;
	i__1 = n[7];
	for (j = 1; j <= i__1; ++j) {
	    if (irgflg[j] <= -1) {
		lgroup = FALSE_;
		goto L220;
	    }
/* L210: */
	}
L220:
	if (! lgroup) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("DELETE REGIONS <I1> THROUGH <I2>:", (ftnlen)33);
	    mesage_("HIT A RETURN TO END", (ftnlen)19);
L230:
	    if (*icom > *jcom) {
		frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72,
			 &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		*icom = 1;
	    }
	    geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		    ifound, (ftnlen)72);
	    if (ifound > 0) {
		check_(&i1, &i2, &n[22]);
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    addlnk = FALSE_;
		    ltsort_(mr, &linkr[3], &i__, &iptr, &addlnk);
		    if (iptr > 0 && irgflg[iptr] <= 0) {
/*  DELETE REGION FROM BODY LIST */
			i__2 = n[9];
			for (j = 1; j <= i__2; ++j) {
			    if (irpb[j] == i__) {
				i__3 = n[9];
				for (k = j + 1; k <= i__3; ++k) {
				    irpb[k - 1] = irpb[k];
/* L240: */
				}
				--n[9];
			    }
/* L250: */
			}
/*  DELETE REGION FROM GROUPS */
			i__2 = n[7];
			for (j = 1; j <= i__2; ++j) {
			    if (irgflg[j] >= 1) {
				k1 = ifside[iptr];
				k2 = k1 + nspr[iptr] - 1;
				i__3 = k2;
				for (k = k1; k <= i__3; ++k) {
				    if (islist[k] == i__) {
					i__4 = k2;
					for (l = k + 1; l <= i__4; ++l) {
					    islist[k - 1] = islist[k];
/* L260: */
					}
					--nspr[iptr];
				    }
/* L270: */
				}
			    }
/* L280: */
			}
/*  DELETE REGION FROM HOLES */
			i__2 = n[7];
			for (j = 1; j <= i__2; ++j) {
			    if (nhpr[j] >= 1) {
				k1 = ifhole[iptr];
				k2 = k1 + nhpr[iptr] - 1;
				i__3 = k2;
				for (k = k1; k <= i__3; ++k) {
				    if (ihlist[k] == i__) {
					i__4 = k2;
					for (l = k + 1; l <= i__4; ++l) {
					    ihlist[k - 1] = ihlist[k];
/* L290: */
					}
					--nhpr[iptr];
				    }
/* L300: */
				}
			    }
/* L310: */
			}
/*  DELETE LINK TO REGION */
			addlnk = TRUE_;
			ltsort_(mr, &linkr[3], &i__, &iz, &addlnk);
		    }
/* L320: */
		}
		addlnk = TRUE_;
		goto L230;
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*------------------------------------*", (ftnlen)38);
	    mesage_("* NO REGIONS IN THE CURRENT DATABASE *", (ftnlen)38);
	    mesage_("*------------------------------------*", (ftnlen)38);
	}
/*  DELETE THE GROUPS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'G' || *(unsigned char *)
	    &cin[*icom * 72] == 'g') {
	++(*icom);
	lgroup = FALSE_;
	i__1 = n[7];
	for (j = 1; j <= i__1; ++j) {
	    if (irgflg[j] >= 1) {
		lgroup = TRUE_;
		goto L340;
	    }
/* L330: */
	}
L340:
	if (lgroup) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("DELETE GROUPS<I1> THROUGH <I2>:", (ftnlen)31);
	    mesage_("HIT A RETURN TO END", (ftnlen)19);
L350:
	    if (*icom > *jcom) {
		frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72,
			 &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		*icom = 1;
	    }
	    geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		    ifound, (ftnlen)72);
	    if (ifound > 0) {
		check_(&i1, &i2, &n[22]);
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    addlnk = FALSE_;
		    ltsort_(mr, &linkr[3], &i__, &iptr, &addlnk);
		    if (iptr > 0 && irgflg[iptr] >= 1) {
/*  DELETE GROUP FROM BODY LIST */
			i__2 = n[9];
			for (j = 1; j <= i__2; ++j) {
			    if (irpb[j] == i__) {
				i__3 = n[9];
				for (k = j + 1; k <= i__3; ++k) {
				    irpb[k - 1] = irpb[k];
/* L360: */
				}
				--n[9];
			    }
/* L370: */
			}
/*  DELETE LINK TO GROUP */
			addlnk = TRUE_;
			ltsort_(mr, &linkr[3], &i__, &iz, &addlnk);
		    }
/* L380: */
		}
		addlnk = TRUE_;
		goto L350;
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*-----------------------------------*", (ftnlen)37);
	    mesage_("* NO GROUPS IN THE CURRENT DATABASE *", (ftnlen)37);
	    mesage_("*-----------------------------------*", (ftnlen)37);
	}
/*  DELETE THE HOLES */
    } else if (s_cmp(cin + *icom * 72, "HO", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ho", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (n[29] > 0) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("DELETE HOLES FOR REGIONS <I1> THROUGH <I2>:", (ftnlen)43)
		    ;
	    mesage_("HIT A RETURN TO END", (ftnlen)19);
L390:
	    if (*icom > *jcom) {
		frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72,
			 &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		*icom = 1;
	    }
	    geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		    ifound, (ftnlen)72);
	    if (ifound > 0) {
		check_(&i1, &i2, &n[22]);
		addlnk = FALSE_;
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(mr, &linkr[3], &i__, &l, &addlnk);
		    if (l > 0) {
			if (nhpr[l] > 0) {
			    nhpr[l] = 0;
			} else {
			    s_wsfe(&io___18);
			    do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			}
		    } else {
			s_wsfe(&io___19);
			do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
			e_wsfe();
		    }
/* L400: */
		}
		addlnk = TRUE_;
		goto L390;
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*----------------------------------*", (ftnlen)36);
	    mesage_("* NO HOLES IN THE CURRENT DATABASE *", (ftnlen)36);
	    mesage_("*----------------------------------*", (ftnlen)36);
	}
/*  DELETE SCHEMES */
    } else if (s_cmp(cin + *icom * 72, "SC", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "sc", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	if (n[10] > 0) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("DELETE SCHEMES <I1> THROUGH <I2>:", (ftnlen)33);
	    mesage_("HIT A RETURN TO END", (ftnlen)19);
L410:
	    if (*icom > *jcom) {
		frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72,
			 &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		*icom = 1;
	    }
	    geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		    ifound, (ftnlen)72);
	    if (ifound > 0) {
		check_(&i1, &i2, &n[24]);
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(mr, &linksc[3], &i__, &iz, &addlnk);
/* L420: */
		}
		goto L410;
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*---------------------------------------------*", (
		    ftnlen)47);
	    mesage_("* ONLY DEFAULT SCHEME IN THE CURRENT DATABASE *", (
		    ftnlen)47);
	    mesage_("*---------------------------------------------*", (
		    ftnlen)47);
	    s_wsfe(&io___20);
	    do_fio(&c__1, defsch, (ftnlen)72);
	    e_wsfe();
	    mesage_(" ", (ftnlen)1);
	}
/*  SPAWN A PROCESS */
    } else if (s_cmp(cin + *icom * 72, "SP", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "sp", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	spawn_(vaxvms);
/*  DELETE SIDE DEFINITIONS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'S' || *(unsigned char *)
	    &cin[*icom * 72] == 's') {
	++(*icom);
	if (n[3] > 0) {
	    mesage_(" ", (ftnlen)1);
	    mesage_("DELETE SIDES <I1> THROUGH <I2>:", (ftnlen)31);
	    mesage_("HIT A RETURN TO END", (ftnlen)19);
L430:
	    if (*icom > *jcom) {
		frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72,
			 &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		*icom = 1;
	    }
	    geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		    ifound, (ftnlen)72);
	    if (ifound > 0) {
		check_(&i1, &i2, &n[20]);
		i__1 = i2;
		for (i__ = i1; i__ <= i__1; ++i__) {
		    ltsort_(ms, &links[3], &i__, &iz, &addlnk);
/* L440: */
		}
		goto L430;
	    }
	} else {
	    mesage_(" ", (ftnlen)1);
	    mesage_("*----------------------------------*", (ftnlen)36);
	    mesage_("* NO SIDES IN THE CURRENT DATABASE *", (ftnlen)36);
	    mesage_("*----------------------------------*", (ftnlen)36);
	}
/*  DELETE BOUNDARY CONDITIONS */
    } else if (*(unsigned char *)&cin[*icom * 72] == 'B' || *(unsigned char *)
	    &cin[*icom * 72] == 'b') {
	++(*icom);
	if (*icom > *jcom) {
	    mesage_("THE FOLLOWING BOUNDARY FLAG TYPES ARE AVAILABLE", (
		    ftnlen)47);
	    mesage_("        P*OINT FLAGS    - FOR NODES AT POINTS", (ftnlen)
		    45);
	    mesage_("        N*ODE FLAGS     - FOR NODES ON A BOUNDARY", (
		    ftnlen)49);
	    mesage_("        E*LEMENT FLAGS  - FOR ELEMENT SIDES ON A BOUNDA\
RY", (ftnlen)57);
	    frefld_(&iz, &iz, "TYPE OF BOUNDARY FLAG TO BE DELETED FROM: ", 
		    mcom, &iostat, jcom, &kin[1], cin + 72, &iin[1], &rin[1], 
		    (ftnlen)42, (ftnlen)72);
	    *icom = 1;
	}
	if (*(unsigned char *)&cin[*icom * 72] == 'P' || *(unsigned char *)&
		cin[*icom * 72] == 'p') {
	    ++(*icom);
	    if (n[11] > 0) {
		mesage_(" ", (ftnlen)1);
		mesage_("DELETE POINT BOUNDARY FLAGS <I1> THROUGH <I2>:", (
			ftnlen)46);
		mesage_("HIT A RETURN TO END", (ftnlen)19);
L450:
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[25]);
		    i__1 = i2;
		    for (i__ = i1; i__ <= i__1; ++i__) {
			ltsort_(mp, &linkpb[3], &i__, &iz, &addlnk);
/* L460: */
		    }
		    sideok = FALSE_;
		    linkbc_(mp, ms, &c__1, &n[11], &n[1], &n[25], &n[11], &n[
			    12], &n[20], &ipbf[1], &ifpb[1], &nppf[1], &
			    listpb[3], &nlps[1], &ifline[1], &illist[1], &
			    ipboun[1], &linkpb[3], &iwtpbf[4], &linkp[3], &
			    links[3], &sideok, &noroom);
		    goto L450;
		}
	    } else {
		mesage_(" ", (ftnlen)1);
		mesage_("*-----------------------------------------*", (
			ftnlen)43);
		mesage_("* NO POINT BOUNDARY FLAGS IN THE DATABASE *", (
			ftnlen)43);
		mesage_("*-----------------------------------------*", (
			ftnlen)43);
		mesage_(" ", (ftnlen)1);
	    }
	} else if (*(unsigned char *)&cin[*icom * 72] == 'N' || *(unsigned 
		char *)&cin[*icom * 72] == 'n') {
	    ++(*icom);
	    if (n[13] > 0) {
		mesage_(" ", (ftnlen)1);
		mesage_("DELETE NODE BOUNDARY FLAGS <I1> THROUGH <I2>:", (
			ftnlen)45);
		mesage_("HIT A RETURN TO END", (ftnlen)19);
L470:
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[26]);
		    i__1 = i2;
		    for (i__ = i1; i__ <= i__1; ++i__) {
			ltsort_(ml, &linklb[3], &i__, &iz, &addlnk);
/* L480: */
		    }
/*  RELINK UP THE LINES TO THEIR ASSOCIATED FLAGS */
		    sideok = TRUE_;
		    linkbc_(ml, ms, &c__1, &n[13], &n[2], &n[26], &n[13], &n[
			    14], &n[20], &ilbf[1], &iflb[1], &nlpf[1], &
			    listlb[3], &nlps[1], &ifline[1], &illist[1], &
			    ilboun[1], &linklb[3], &iwtlbf[4], &linkl[3], &
			    links[3], &sideok, &noroom);
		    goto L470;
		}
	    } else {
		mesage_(" ", (ftnlen)1);
		mesage_("*----------------------------------------*", (ftnlen)
			42);
		mesage_("* NO NODE BOUNDARY FLAGS IN THE DATABASE *", (ftnlen)
			42);
		mesage_("*----------------------------------------*", (ftnlen)
			42);
		mesage_(" ", (ftnlen)1);
	    }
	} else if (*(unsigned char *)&cin[*icom * 72] == 'E' || *(unsigned 
		char *)&cin[*icom * 72] == 'e') {
	    ++(*icom);
	    if (n[15] > 0) {
		mesage_(" ", (ftnlen)1);
		mesage_("DELETE ELEMENT BOUNDARY FLAGS <I1> THROUGH <I2>:", (
			ftnlen)48);
		mesage_("HIT A RETURN TO END", (ftnlen)19);
L490:
		if (*icom > *jcom) {
		    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin 
			    + 72, &iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
		    *icom = 1;
		}
		geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &
			i2, &ifound, (ftnlen)72);
		if (ifound > 0) {
		    check_(&i1, &i2, &n[27]);
		    i__1 = i2;
		    for (i__ = i1; i__ <= i__1; ++i__) {
			ltsort_(ml, &linksb[3], &i__, &iz, &addlnk);
/* L500: */
		    }
/*  RELINK UP THE LINES TO THEIR ASSOCIATED FLAGS */
		    sideok = TRUE_;
		    linkbc_(ml, ms, &c__1, &n[15], &n[2], &n[27], &n[15], &n[
			    16], &n[20], &isbf[1], &ifsb[1], &nspf[1], &
			    listsb[3], &nlps[1], &ifline[1], &illist[1], &
			    isboun[1], &linksb[3], &iwtsbf[4], &linkl[3], &
			    links[3], &sideok, &noroom);
		    goto L490;
		}
	    } else {
		mesage_(" ", (ftnlen)1);
		mesage_("*-------------------------------------------*", (
			ftnlen)45);
		mesage_("* NO ELEMENT BOUNDARY FLAGS IN THE DATABASE *", (
			ftnlen)45);
		mesage_("*-------------------------------------------*", (
			ftnlen)45);
		mesage_(" ", (ftnlen)1);
	    }
	}
/*  EXIT OPTION - EXITS FASTQ */
    } else if (s_cmp(cin + *icom * 72, "EX", (ftnlen)2, (ftnlen)2) == 0 || 
	    s_cmp(cin + *icom * 72, "ex", (ftnlen)2, (ftnlen)2) == 0) {
	++(*icom);
	strlng_(cin + *icom * 72, &len, (ftnlen)72);
	if (len > 1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'X' || len > 
		1 && *(unsigned char *)&cin[*icom * 72 + 1] != 'x') {
	    help_fq__(&c__6);
	} else {
	    fexit_(wrote, mcom, icom, jcom, cin + 72, &iin[1], &rin[1], &kin[
		    1], time1, batch, versn, (ftnlen)72, (ftnlen)9);
	}
	goto L100;
    } else if (*(unsigned char *)&cin[*icom * 72] == ' ') {
	++(*icom);
	return 0;
/*  PRINT HELP MESAGE */
    } else {
	++(*icom);
	help_fq__(&c__6);
    }
    goto L100;
} /* delfsq_ */

#ifdef __cplusplus
	}
#endif

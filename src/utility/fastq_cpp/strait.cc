/* strait.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int strait_(integer *mp, integer *ml, integer *mcom, integer 
	*icom, integer *jcom, char *cin, real *rin, integer *iin, integer *
	kin, integer *idump, integer *n, real *coor, integer *lcon, integer *
	linkp, integer *linkl, ftnlen cin_len)
{
    /* Format strings */
    static char fmt_10000[] = "(\002 LINE\002,i5,\002 HAS THE FOLLOWING END \
POINTS:\002,/,\002 POINT:\002,i5,\002   X COORDINATE\002,g14.7,\002   Y COOR\
DINATE \002,g14.7,/,\002 POINT:\002,i5,\002   X COORDINATE\002,g14.7,\002   \
Y COORDINATE \002,g14.7)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, j, i1, i2, iz, ip1, ip2;
    extern /* Subroutine */ int check_(integer *, integer *, integer *), 
	    geti12_(integer *, integer *, integer *, char *, integer *, 
	    integer *, integer *, integer *, integer *, ftnlen);
    static logical addlnk;
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen);
    static integer ifound, iostat;
    extern /* Subroutine */ int ltsort_(integer *, integer *, integer *, 
	    integer *, logical *);

    /* Fortran I/O blocks */
    static cilist io___11 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___12 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE STRAIT = STRAIGHTENS LINES IN THE X OR Y DIRECTION */
/* *********************************************************************** */
/*  SUBROUTINE CALLED BY: */
/*     FASTQ = A PROGRAM TO QUICKLY PREPARE QMESH INPUT */
/* *********************************************************************** */
/*  SUBROUTINES CALLED: */
/*     CHECK  = CHECKS 2 VALUES FOR BEING OUT OF PRESCRIBED BOUNDS */
/* *********************************************************************** */
/*  VARIABLES USED: */
/*     IANS   = LOGICAL RESPONSE FROM YES-NO QUESTION */
/*     ANS    = CHARACTER RESPONSE FOR MENU CHOICE */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkp -= 3;
    coor -= 3;
    linkl -= 3;
    lcon -= 4;
    --kin;
    --iin;
    --rin;
    cin -= 72;
    --n;

    /* Function Body */
    iz = 0;
    addlnk = FALSE_;
L100:
    if (n[2] > 0) {
	mesage_(" ", (ftnlen)1);
	mesage_("STRAIGHTEN LINES <I1> THROUGH <I2> PARALLEL TO THE <X OR Y>\
 AXIS", (ftnlen)64);
	if (*icom > *jcom) {
	    frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &
		    iin[1], &rin[1], (ftnlen)1, (ftnlen)72);
	    *icom = 1;
	}
	geti12_(mcom, icom, jcom, cin + 72, &iin[1], &kin[1], &i1, &i2, &
		ifound, (ftnlen)72);
	if (ifound > 0) {
	    check_(&i1, &i2, &n[19]);
	} else {
	    return 0;
	}
/*  STRAIGHTEN THE LINE IN THE Y DIRECTION */
	if (*(unsigned char *)&cin[*icom * 72] == 'Y' || *(unsigned char *)&
		cin[*icom * 72] == 'y') {
	    ++(*icom);
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		ltsort_(ml, &linkl[3], &i__, &j, &addlnk);
		if (j > 0) {
		    ltsort_(mp, &linkp[3], &lcon[j * 3 + 1], &ip1, &addlnk);
		    ltsort_(mp, &linkp[3], &lcon[j * 3 + 2], &ip2, &addlnk);
		    if (ip1 > 0 && ip2 > 0 && ip1 <= n[18] && ip2 <= n[18]) {
			s_wsfe(&io___11);
			do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&lcon[j * 3 + 1], (ftnlen)
				sizeof(integer));
			do_fio(&c__1, (char *)&coor[(ip1 << 1) + 1], (ftnlen)
				sizeof(real));
			do_fio(&c__1, (char *)&coor[(ip1 << 1) + 2], (ftnlen)
				sizeof(real));
			do_fio(&c__1, (char *)&lcon[j * 3 + 2], (ftnlen)
				sizeof(integer));
			do_fio(&c__1, (char *)&coor[(ip2 << 1) + 1], (ftnlen)
				sizeof(real));
			do_fio(&c__1, (char *)&coor[(ip2 << 1) + 2], (ftnlen)
				sizeof(real));
			e_wsfe();
L110:
			if (*icom > *jcom) {
			    frefld_(&iz, &iz, "NEW CONSTANT X VALUE: ", mcom, 
				    &iostat, jcom, &kin[1], cin + 72, &iin[1],
				     &rin[1], (ftnlen)22, (ftnlen)72);
			    *icom = 1;
			}
			if (kin[*icom] > 0) {
			    coor[(ip1 << 1) + 1] = rin[*icom];
			    coor[(ip2 << 1) + 1] = rin[*icom];
			    ++(*icom);
			} else {
			    mesage_("BAD INPUT - TRY AGAIN", (ftnlen)21);
			    ++(*icom);
			    goto L110;
			}
		    }
		}
/* L120: */
	    }
/*  STRAIGHTEN THE LINE IN THE X DIRECTION */
	} else if (*(unsigned char *)&cin[*icom * 72] == 'X' || *(unsigned 
		char *)&cin[*icom * 72] == 'x') {
	    ++(*icom);
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		ltsort_(ml, &linkl[3], &i__, &j, &addlnk);
		if (j > 0) {
		    ltsort_(mp, &linkp[3], &lcon[j * 3 + 1], &ip1, &addlnk);
		    ltsort_(mp, &linkp[3], &lcon[j * 3 + 2], &ip2, &addlnk);
		    if (ip1 > 0 && ip2 > 0 && ip1 <= n[18] && ip2 <= n[18]) {
			s_wsfe(&io___12);
			do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&lcon[j * 3 + 1], (ftnlen)
				sizeof(integer));
			do_fio(&c__1, (char *)&coor[(ip1 << 1) + 1], (ftnlen)
				sizeof(real));
			do_fio(&c__1, (char *)&coor[(ip1 << 1) + 2], (ftnlen)
				sizeof(real));
			do_fio(&c__1, (char *)&lcon[j * 3 + 2], (ftnlen)
				sizeof(integer));
			do_fio(&c__1, (char *)&coor[(ip2 << 1) + 1], (ftnlen)
				sizeof(real));
			do_fio(&c__1, (char *)&coor[(ip2 << 1) + 2], (ftnlen)
				sizeof(real));
			e_wsfe();
L130:
			if (*icom > *jcom) {
			    frefld_(&iz, &iz, "NEW CONSTANT Y VALUE: ", mcom, 
				    &iostat, jcom, &kin[1], cin + 72, &iin[1],
				     &rin[1], (ftnlen)22, (ftnlen)72);
			    *icom = 1;
			}
			if (kin[*icom] > 0) {
			    coor[(ip1 << 1) + 2] = rin[*icom];
			    coor[(ip2 << 1) + 2] = rin[*icom];
			    ++(*icom);
			} else {
			    mesage_("BAD INPUT - TRY AGAIN", (ftnlen)21);
			    ++(*icom);
			    goto L130;
			}
		    }
		}
/* L140: */
	    }
	} else {
	    mesage_("DIRECTION MUST BE \"X\" OR \"Y\"", (ftnlen)28);
	    ++(*icom);
	}
    } else {
	mesage_(" ", (ftnlen)1);
	mesage_("*------------------------------*", (ftnlen)32);
	mesage_("NO LINES IN THE CURRENT DATABASE", (ftnlen)32);
	mesage_("*------------------------------*", (ftnlen)32);
	return 0;
    }
    goto L100;
} /* strait_ */

#ifdef __cplusplus
	}
#endif

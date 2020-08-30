/* dpread.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int dpread_(real *x, real *y, char *button, ftnlen 
	button_len)
{
    /* Format strings */
    static char fmt_10000[] = "(a1,i5,1x,i5)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(), 
	    s_rsfe(cilist *), e_rsfe(), s_cmp(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer ix, iy;
    static char dummy[5];

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 6, 0, " (A)", 0 };
    static cilist io___3 = { 0, 5, 1, fmt_10000, 0 };
    static cilist io___6 = { 0, 6, 0, " (A)", 0 };


/* *********************************************************************** */
/*  SUBROUTINE DPREAD = READS INPUT FROM A DIGIPAD DIGITIZING TABLET */
/* *********************************************************************** */
/*  SWITCH THE TERMINAL TO PASSTHROUGH MODE <ESC>[5i */
    *(unsigned char *)dummy = '+';
    *(unsigned char *)&dummy[1] = '\33';
    s_copy(dummy + 2, "[5i", (ftnlen)3, (ftnlen)3);
    s_wsfe(&io___2);
    do_fio(&c__1, dummy, (ftnlen)5);
    e_wsfe();
/*  INPUT THE BUTTON AND X, Y PAIR FROM THE PAD */
    *(unsigned char *)button = ' ';
    i__1 = s_rsfe(&io___3);
    if (i__1 != 0) {
	goto L100;
    }
    i__1 = do_fio(&c__1, button, (ftnlen)1);
    if (i__1 != 0) {
	goto L100;
    }
    i__1 = do_fio(&c__1, (char *)&ix, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L100;
    }
    i__1 = do_fio(&c__1, (char *)&iy, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L100;
    }
    i__1 = e_rsfe();
    if (i__1 != 0) {
	goto L100;
    }
/*  CONVERT THE BUTTON */
    if (*(unsigned char *)button == ':') {
	*(unsigned char *)button = 'A';
    } else if (*(unsigned char *)button == ';') {
	*(unsigned char *)button = 'B';
    } else if (*(unsigned char *)button == '<') {
	*(unsigned char *)button = 'C';
    } else if (s_cmp(button, " = ", (ftnlen)1, (ftnlen)3) == 0) {
	*(unsigned char *)button = 'D';
    } else if (*(unsigned char *)button == '>') {
	*(unsigned char *)button = 'E';
    } else if (*(unsigned char *)button == '?') {
	*(unsigned char *)button = 'F';
    } else if (*(unsigned char *)button == ' ') {
	*(unsigned char *)button = 'E';
    }
/* CONVERT  (X,  Y) LOCATION */
    *x = (real) ix;
    *y = (real) iy;
L100:
/*  SWITCH THE TERMINAL OUT OF PASSTHROUGH MODE <ESC>[4i */
    s_wsfe(&io___6);
    do_fio(&c__1, " \033[4i", (ftnlen)5);
    e_wsfe();
    return 0;
} /* dpread_ */

#ifdef __cplusplus
	}
#endif

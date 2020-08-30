/* putcrs.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int putcrs_(real *x, real *y, logical *oldcur)
{
    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(), s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), 
	    e_wsfi(), s_wsfe(cilist *), e_wsfe();

    /* Local variables */
    static real x1, y1;
    static integer ix, iy, idum[2];
    extern /* Subroutine */ int mp2pt_(integer *, real *, real *, real *, 
	    real *, integer *);
    static char dummy[16];

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 6, 0, 0, 0 };
    static icilist io___8 = { 0, dummy+2, 0, "(I6)", 6, 1 };
    static icilist io___9 = { 0, dummy+9, 0, "(I6)", 6, 1 };
    static cilist io___10 = { 0, 6, 0, 0, 0 };
    static cilist io___11 = { 0, 6, 0, 0, 0 };
    static cilist io___12 = { 0, 6, 0, 0, 0 };
    static cilist io___13 = { 0, 6, 0, "(A)", 0 };


/* *********************************************************************** */
/*     SUBROUTINE PUTCRS = PLACES THE CROSSHAIRS AT THE CURRENT LOCATION */
/* *********************************************************************** */
/*  SELECT DECIMAL MODE */
    s_copy(dummy, "\033", (ftnlen)16, (ftnlen)1);
    s_copy(dummy + 1, "OR1", (ftnlen)3, (ftnlen)3);
    s_wsle(&io___2);
    do_lio(&c__9, &c__1, dummy, (ftnlen)16);
    e_wsle();
/*  PLACE THE CROSSHAIRS AT THE RIGHT LOCATION */
    mp2pt_(&c__1, x, y, &x1, &y1, idum);
    ix = (integer) (x1 * (float)4151.);
    iy = (integer) (y1 * (float)4151.);
    *(unsigned char *)dummy = '\33';
    *(unsigned char *)&dummy[1] = 'P';
    s_wsfi(&io___8);
    do_fio(&c__1, (char *)&ix, (ftnlen)sizeof(integer));
    e_wsfi();
    *(unsigned char *)&dummy[8] = ',';
    s_wsfi(&io___9);
    do_fio(&c__1, (char *)&iy, (ftnlen)sizeof(integer));
    e_wsfi();
    *(unsigned char *)&dummy[15] = ',';
    s_wsle(&io___10);
    do_lio(&c__9, &c__1, dummy, (ftnlen)16);
    e_wsle();
/*  DESELECT DECIMAL MODE */
    s_copy(dummy, "\033", (ftnlen)16, (ftnlen)1);
    s_copy(dummy + 1, "OR0", (ftnlen)3, (ftnlen)3);
    s_wsle(&io___11);
    do_lio(&c__9, &c__1, dummy, (ftnlen)16);
    e_wsle();
    if (! (*oldcur)) {
/*  ACTIVATE THE CROSSHAIRS */
	s_copy(dummy, "\033", (ftnlen)16, (ftnlen)1);
	s_copy(dummy + 1, "G1", (ftnlen)2, (ftnlen)2);
	s_wsle(&io___12);
	do_lio(&c__9, &c__1, dummy, (ftnlen)16);
	e_wsle();
	*oldcur = TRUE_;
    }
    s_wsfe(&io___13);
    do_fio(&c__1, " \033[2J", (ftnlen)5);
    e_wsfe();
    return 0;
} /* putcrs_ */

#ifdef __cplusplus
	}
#endif

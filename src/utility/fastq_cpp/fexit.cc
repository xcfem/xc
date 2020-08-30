/* fexit.f -- translated by f2c (version 20160102).
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
static integer c__10003 = 10003;
static integer c__0 = 0;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int fexit_(logical *wrote, integer *mcom, integer *icom, 
	integer *jcom, char *cin, integer *iin, real *rin, integer *kin, real 
	*time1, logical *batch, char *versn, ftnlen cin_len, ftnlen versn_len)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(), s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), 
	    e_wsfe();
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    static char date[8];
    static logical ians;
    static char time[8];
    static real time2;
    extern /* Subroutine */ int exdate_(char *, 
	    ftnlen), vdescp_(integer *, integer *, integer *), pltend_(), 
	    extime_(char *, ftnlen), excpus_(real *), intrup_(char *, logical 
	    *, integer *, integer *, integer *, char *, integer *, real *, 
	    integer *, ftnlen, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 6, 0, 0, 0 };
    static cilist io___6 = { 0, 6, 0, 0, 0 };
    static cilist io___7 = { 0, 6, 0, 0, 0 };
    static cilist io___8 = { 0, 6, 0, " (A, I5)", 0 };
    static cilist io___9 = { 0, 6, 0, " (A, I5)", 0 };


/* *********************************************************************** */
/*  SUBROUTINE FEXIT = GRACEFUL FASTQ EXIT */
/* *********************************************************************** */
    /* Parameter adjustments */
    --kin;
    --rin;
    --iin;
    cin -= 72;

    /* Function Body */
    if (! (*wrote)) {
	mesage_(" ", (ftnlen)1);
	mesage_("***********************************************", (ftnlen)47)
		;
	mesage_("*  WARNING: NO OUTPUT FILE HAS BEEN WRITTEN   *", (ftnlen)47)
		;
	mesage_("***********************************************", (ftnlen)47)
		;
	intrup_("EXIT ANYWAY", &ians, mcom, icom, jcom, cin + 72, &iin[1], &
		rin[1], &kin[1], (ftnlen)11, (ftnlen)72);
	if (! ians) {
	    return 0;
	}
    }
    mesage_(" ", (ftnlen)1);
    excpus_(&time2);
    if (*batch) {
	mesage_("FASTQ COMPLETED SUCCESSFULLY", (ftnlen)28);
	exdate_(date, (ftnlen)8);
	extime_(time, (ftnlen)8);
	s_wsle(&io___5);
	do_lio(&c__9, &c__1, "             DATE: ", (ftnlen)19);
	do_lio(&c__9, &c__1, date, (ftnlen)8);
	e_wsle();
	s_wsle(&io___6);
	do_lio(&c__9, &c__1, "             TIME: ", (ftnlen)19);
	do_lio(&c__9, &c__1, time, (ftnlen)8);
	e_wsle();
	s_wsle(&io___7);
	do_lio(&c__9, &c__1, "          VERSION: ", (ftnlen)19);
	do_lio(&c__9, &c__1, versn, (ftnlen)10);
	e_wsle();
	s_wsfe(&io___8);
	do_fio(&c__1, "  CPU SECONDS USED: ", (ftnlen)20);
	i__1 = (integer) (time2 - *time1);
	do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	e_wsfe();
	mesage_(" ", (ftnlen)1);
    } else {
	s_wsfe(&io___9);
	do_fio(&c__1, " CPU SECONDS USED: ", (ftnlen)19);
	i__1 = (integer) (time2 - *time1);
	do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	e_wsfe();
	mesage_("*--------------------------*", (ftnlen)28);
	mesage_(" ", (ftnlen)1);
	vdescp_(&c__10003, &c__0, &c__0);
	pltend_();
    }
    s_stop("", (ftnlen)0);
    return 0;
} /* fexit_ */

#ifdef __cplusplus
	}
#endif

/* disclm.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int disclm_(integer *ncols)
{
    /* Initialized data */

    static char blank[29+1] = "                             ";

    /* Format strings */
    static char fmt_10000[] = "(\002 \002,a,\002****************************\
**********\002,\002**************************************\002//,a,\002      \
            ISSUED BY SANDIA NAT\002,\002IONAL LABORATORIES, \002/,a,\002   \
                      A PRIME CONTR\002,\002ACTOR TO THE\002/,a,\002        \
             UNITED STATES DEP\002,\002ARTMENT OF ENERGY\002//,a,\002*******\
*******************************\002,\002************************************\
**\002//,a,\002THIS CODE WAS PREPARED IN THE COURSE O\002,\002F WORK SPONSOR\
ED BY THE UNITED STATES\002/,a,\002GOVERNMENT.  NEITHER THE UNITED STATES\
\002,\002,  NOR THE UNITED STATES DEPARTMENT OF\002/,a,\002ENERGY,  NOR THE \
UNITED STATES NUCLEAR \002,\002REGULATORY COMMISSION,  NOR ANY OF\002/,a,\
\002THEIR EMPLOYEES,  NOR ANY OF THEIR CONT\002,\002RACTORS,  SUBCONTRACTORS\
,  OR THEIR\002)";
    static char fmt_10010[] = "(a,\002EMPLOYEES,  MAKES ANY WARRANTY,  EXPRE\
SS\002,\002 OR IMPLIED,  OR ASSUMES ANY LEGAL\002/a,\002LIABILITY OR RESPONS\
IBILITY FOR THE AC\002,\002CURACY,  COMPLETENESS OR USEFULNESS OF\002/,a,\
\002ANY INFORMATION,  APPARATUS,  PRODUCT OR\002,\002 PROCESS DISCLOSED,  OR\
 REPRESENTS THAT\002/,a,\002ITS USE WOULD NOT INFRINGE PRIVATELY O\002,\002W\
NED RIGHTS.\002//,a,\002**************************************\002,\002*****\
*********************************\002/)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer i__, nshift;

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, fmt_10000, 0 };
    static cilist io___5 = { 0, 6, 0, fmt_10010, 0 };


/* *********************************************************************** */
/*  SUBROUTINE DISCLM = PRINTS THE SANDIA DISCLAIMER */
/* *********************************************************************** */
/* Computing MAX */
    i__1 = (*ncols - 76) / 2;
    nshift = max(i__1,1) + 1;
    nshift = min(nshift,29);
    s_wsfe(&io___3);
    for (i__ = 1; i__ <= 9; ++i__) {
	do_fio(&c__1, blank, nshift);
    }
    e_wsfe();
    s_wsfe(&io___5);
    for (i__ = 1; i__ <= 5; ++i__) {
	do_fio(&c__1, blank, nshift);
    }
    e_wsfe();
    return 0;
} /* disclm_ */

#ifdef __cplusplus
	}
#endif

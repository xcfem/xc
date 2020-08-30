/* intrup.f -- translated by f2c (version 20160102).
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

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int intrup_(char *prompt, logical *ians, integer *mcom, 
	integer *icom, integer *jcom, char *cin, integer *iin, real *rin, 
	integer *kin, ftnlen prompt_len, ftnlen cin_len)
{
    /* Initialized data */

    static char ans[1*4+1] = "YyNn";

    /* Format strings */
    static char fmt_10000[] = "(\002 RESPONSE MUST BE EITHER YES OR NO  -  T\
RY AGAIN\002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), e_wsfe();

    /* Local variables */
    static integer iz, len;
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen);
    static integer newlen, iostat;
    extern /* Subroutine */ int strlng_(char *, integer *, ftnlen);
    static char newpmt[72];
    extern /* Subroutine */ int inqtru_(char *, logical *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___7 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*  SUBROUTINE INTRUP = INPUTS A YES OR NO PLUS MORE IF NEEDED */
/* *********************************************************************** */
    /* Parameter adjustments */
    --kin;
    --rin;
    --iin;
    cin -= 72;

    /* Function Body */
    iz = 0;
    strlng_(prompt, &len, prompt_len);
/*  SEE IF A YES / NO ANSWER IS SITTING AS THE FIRST COMMAND IN THE LIST */
    if (*icom <= *jcom && (*(unsigned char *)&cin[*icom * 72] == *(unsigned 
	    char *)&ans[0] || *(unsigned char *)&cin[*icom * 72] == *(
	    unsigned char *)&ans[1])) {
	*ians = TRUE_;
	++(*icom);
    } else if (*icom <= *jcom && (*(unsigned char *)&cin[*icom * 72] == *(
	    unsigned char *)&ans[2] || *(unsigned char *)&cin[*icom * 72] == *
	    (unsigned char *)&ans[3])) {
	*ians = FALSE_;
	++(*icom);
/*  INPUT NEW COMMAND LISTS ONLY IF THE CURRENT ONES ARE USED UP */
/*  MAKE SURE THE FIRST ONE OF THESE COMMANDS IS EITHER YES OR NO */
    } else if (*icom > *jcom) {
	if (len <= 71) {
	    s_copy(newpmt, prompt, (ftnlen)72, len);
	    i__1 = len;
	    s_copy(newpmt + i__1, "?", len + 1 - i__1, (ftnlen)1);
	} else {
	    s_copy(newpmt, prompt, (ftnlen)72, prompt_len);
	}
	strlng_(newpmt, &newlen, (ftnlen)72);
/* Computing MIN */
	i__1 = 72, i__2 = newlen + 1;
	newlen = min(i__1,i__2);
L100:
	mesage_(" ", (ftnlen)1);
	frefld_(&iz, &iz, newpmt, mcom, &iostat, jcom, &kin[1], cin + 72, &
		iin[1], &rin[1], newlen, (ftnlen)72);
	*icom = 1;
	if (*(unsigned char *)&cin[*icom * 72] == *(unsigned char *)&ans[0] ||
		 *(unsigned char *)&cin[*icom * 72] == *(unsigned char *)&ans[
		1]) {
	    *ians = TRUE_;
	    ++(*icom);
	} else if (*(unsigned char *)&cin[*icom * 72] == *(unsigned char *)&
		ans[2] || *(unsigned char *)&cin[*icom * 72] == *(unsigned 
		char *)&ans[3]) {
	    *ians = FALSE_;
	    ++(*icom);
	} else {
	    s_wsfe(&io___7);
	    e_wsfe();
	    goto L100;
	}
/*  OTHERWISE,  JUST GET A YES / NO RESPONSE AND RETURN */
    } else {
	inqtru_(prompt, ians, prompt_len);
    }
    return 0;
} /* intrup_ */

#ifdef __cplusplus
	}
#endif

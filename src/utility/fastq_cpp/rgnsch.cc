/* rgnsch.f -- translated by f2c (version 20160102).
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
static integer c__13 = 13;

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int rgnsch_(integer *mcom, integer *icom, integer *jcom, 
	char *cin, integer *iin, real *rin, integer *kin, logical *step, 
	integer *iregn, integer *ipntr, integer *n24, integer *msc, char *
	scheme, char *defsch, char *schstr, integer *lensch, integer *nper, 
	logical *pentag, logical *triang, logical *trnsit, logical *halfc, 
	logical *fill, integer *icode, logical *remesh, ftnlen cin_len, 
	ftnlen scheme_len, ftnlen defsch_len, ftnlen schstr_len)
{
    /* Initialized data */

    static integer iexit = 1;
    static integer iover = 2;
    static integer iquit = 3;

    /* Format strings */
    static char fmt_10000[] = "(\0020INITIAL MESH DEFINED USING THIS SCHEME\
:\002,/,5x,a)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static integer j;
    static logical ians;
    extern /* Subroutine */ int strlng_(char *, 
	    integer *, ftnlen), inqstr_(char *, char *, ftnlen, ftnlen), 
	    intrup_(char *, logical *, integer *, integer *, integer *, char *
	    , integer *, real *, integer *, ftnlen, ftnlen), strcut_(char *, 
	    ftnlen), help_fq__(integer *);

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, fmt_10000, 0 };


/* *********************************************************************** */
/*     RGNSCH - GET A REGION'S SCHEME */
/* *********************************************************************** */
    /* Parameter adjustments */
    --kin;
    --rin;
    --iin;
    cin -= 72;
    scheme -= 72;

    /* Function Body */
    *icode = 0;
/*  CHECK FOR REMESHING */
    if (*remesh) {
	s_copy(schstr, "X", (ftnlen)72, (ftnlen)1);
    } else {
/*  GET THE INITIAL SCHEME */
	if (abs(*iregn) <= *n24 && *ipntr > 0) {
	    s_copy(schstr, scheme + *ipntr * 72, (ftnlen)72, (ftnlen)72);
	} else {
	    s_copy(schstr, defsch, (ftnlen)72, (ftnlen)72);
	}
    }
    strcut_(schstr, (ftnlen)72);
    strlng_(schstr, lensch, (ftnlen)72);
/*  STEP PROCESSING */
    if (*step) {
	s_wsfe(&io___4);
	do_fio(&c__1, schstr, (*lensch));
	e_wsfe();
	intrup_("USE CURRENT SCHEME TO BEGIN PROCESSING", &ians, mcom, icom, 
		jcom, cin + 72, &iin[1], &rin[1], &kin[1], (ftnlen)38, (
		ftnlen)72);
/*  CHANGE THE SCHEME */
	if (! ians) {
L100:
	    if (*icom <= *jcom) {
		s_copy(schstr, cin + *icom * 72, (ftnlen)72, (ftnlen)72);
		++(*icom);
		ians = TRUE_;
	    } else {
		inqstr_("ENTER PROCESSING SCHEME: ", schstr, (ftnlen)25, (
			ftnlen)72);
	    }
	    strcut_(schstr, (ftnlen)72);
	    strlng_(schstr, lensch, (ftnlen)72);
/*  HELP FOR SCHEME */
	    if (*(unsigned char *)schstr == 'H' || *(unsigned char *)schstr ==
		     'h') {
		mesage_(" ", (ftnlen)1);
		help_fq__(&c__13);
		mesage_(" ", (ftnlen)1);
		goto L100;
	    }
	}
/*  BLANK SCHEME */
	if (*lensch <= 0 || *(unsigned char *)schstr == ' ') {
	    mesage_("NO INITIAL SCHEME INPUT", (ftnlen)23);
	    mesage_("FORCED RECTANGLE PROCESSING USED", (ftnlen)32);
	    s_copy(schstr, " ", (ftnlen)72, (ftnlen)1);
	    *lensch = 1;
	    goto L120;
	}
    }
/*  DETERMINE MESHING SCHEME */
    *pentag = FALSE_;
    *triang = FALSE_;
    *trnsit = FALSE_;
    *fill = FALSE_;
    i__1 = *lensch;
    for (j = 1; j <= i__1; ++j) {
/*  SEE IF A PENTAGON REGION HAS BEEN FLAGGED */
	if (*(unsigned char *)&schstr[j - 1] == 'U' || *(unsigned char *)&
		schstr[j - 1] == 'u') {
	    if (*nper >= 10) {
		*pentag = TRUE_;
		mesage_("PENTAGON PRIMITIVE REGION PROCESSING USED", (ftnlen)
			41);
	    } else {
		mesage_("PENTAGON REGION GENERATION NOT", (ftnlen)30);
		mesage_("POSSIBLE WITH NO. IN PERIMETER < 10", (ftnlen)35);
		mesage_("REGULAR PROCESSING WILL BE ATTEMPTED", (ftnlen)36);
	    }
	    goto L120;
/*  SEE IF A TRANSITION REGION HAS BEEN FLAGGED */
	} else if (*(unsigned char *)&schstr[j - 1] == 'B' || *(unsigned char 
		*)&schstr[j - 1] == 'b') {
	    if (*nper >= 8) {
		*trnsit = TRUE_;
		*halfc = FALSE_;
		mesage_("TRANSITION PRIMITIVE REGION PROCESSING USED", (
			ftnlen)43);
	    } else {
		mesage_("TRANSITION REGION GENERATION NOT", (ftnlen)32);
		mesage_("POSSIBLE WITH NO. IN PERIMETER < 8", (ftnlen)34);
		mesage_("REGULAR PROCESSING WILL BE ATTEMPTED", (ftnlen)36);
	    }
	    goto L120;
/*  SEE IF A SEMI-CIRCLE REGION HAS BEEN FLAGGED */
	} else if (*(unsigned char *)&schstr[j - 1] == 'C' || *(unsigned char 
		*)&schstr[j - 1] == 'c') {
	    if (*nper >= 8) {
		*trnsit = TRUE_;
		*halfc = TRUE_;
		mesage_("SEMICIRCLE PRIMITIVE REGION PROCESSING USED", (
			ftnlen)43);
	    } else {
		mesage_("TRANSITION/SEMICIRCLE REGION GENERATION NOT", (
			ftnlen)43);
		mesage_("POSSIBLE WITH NO. IN PERIMETER < 8", (ftnlen)34);
		mesage_("REGULAR PROCESSING WILL BE ATTEMPTED", (ftnlen)36);
	    }
	    goto L120;
/*  SEE IF A TRIANGULAR REGION HAS BEEN FLAGGED */
	} else if (*(unsigned char *)&schstr[j - 1] == 'T' || *(unsigned char 
		*)&schstr[j - 1] == 't') {
	    if (*nper >= 6) {
		*triang = TRUE_;
		mesage_("TRIANGLE PRIMITIVE REGION PROCESSING USED", (ftnlen)
			41);
	    } else {
		mesage_("TRIANGULAR REGION GENERATION NOT", (ftnlen)32);
		mesage_("POSSIBLE WITH NO. IN PERIMETER < 6", (ftnlen)34);
		mesage_("REGULAR PROCESSING WILL BE ATTEMPTED", (ftnlen)36);
	    }
	    goto L120;
/*  SEE IF A FILL REGION HAS BEEN FLAGGED */
	} else if (*(unsigned char *)&schstr[j - 1] == 'X' || *(unsigned char 
		*)&schstr[j - 1] == 'x') {
	    *fill = TRUE_;
	    mesage_("PAVING TECHNIQUE INITIALLY USED", (ftnlen)31);
	    goto L120;
/*  SEE IF A REGULAR RECTANGULAR REGION HAS BEEN FLAGGED */
	} else if (*(unsigned char *)&schstr[j - 1] == 'M' || *(unsigned char 
		*)&schstr[j - 1] == 'm') {
	    goto L120;
/*  OTHER POSSIBILITIES */
	} else if (*(unsigned char *)&schstr[j - 1] == 'E' || *(unsigned char 
		*)&schstr[j - 1] == 'e') {
	    *icode = iexit;
	    goto L120;
	} else if (*(unsigned char *)&schstr[j - 1] == 'O' || *(unsigned char 
		*)&schstr[j - 1] == 'o') {
	    *icode = iover;
	    goto L120;
	} else if (*(unsigned char *)&schstr[j - 1] == 'Q' || *(unsigned char 
		*)&schstr[j - 1] == 'q') {
	    *icode = iquit;
	    goto L120;
	}
/* L110: */
    }
L120:
    return 0;
} /* rgnsch_ */

#ifdef __cplusplus
	}
#endif

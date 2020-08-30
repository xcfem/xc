/* adjmsh.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int adjmsh_(integer *ms, integer *mr, integer *npnode, 
	integer *npelem, integer *mxnflg, integer *mxsflg, integer *npregn, 
	integer *npnbc, integer *npsbc, integer *mcom, integer *icom, integer 
	*jcom, char *cin, real *rin, integer *iin, integer *kin, integer *nnn,
	 integer *kkk, integer *nnxk, integer *nodes, integer *nelems, 
	integer *nnflg, integer *nnptr, integer *nnlen, integer *nsflg, 
	integer *nsptr, integer *nslen, integer *nvptr, integer *nvlen, 
	integer *nsiden, integer *mapdxg, real *xn, real *yn, integer *nxk, 
	integer *mat, integer *mapgxd, integer *matmap, real *wtnode, real *
	wtside, integer *nbcnod, integer *nnlist, integer *nbcsid, integer *
	nslist, integer *nvlist, integer *nummat, integer *linkm, char *title,
	 logical *err, real *eight, integer *nine, char *versn, ftnlen 
	cin_len, ftnlen title_len, ftnlen versn_len)
{
    /* System generated locals */
    integer nxk_dim1, nxk_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j, i1, i2, j1, j2, ii, iz, iend;
    static logical found;
    static integer kount, ibegin;
    extern /* Subroutine */ int frefld_(integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    char *, integer *, real *, ftnlen, ftnlen);
    static integer iostat;

/* *********************************************************************** */
/*  SUBROUTINE ADJMSH = ADJUSTS A GENESIS DATABASE OUTPUT */
/* *********************************************************************** */
    /* Parameter adjustments */
    linkm -= 3;
    --mapgxd;
    --yn;
    --xn;
    --mapdxg;
    --mat;
    --nnlen;
    --nnptr;
    --nnflg;
    --nvlen;
    --nvptr;
    --nslen;
    --nsptr;
    --nsflg;
    matmap -= 4;
    --wtnode;
    --nodes;
    --wtside;
    --nsiden;
    --nelems;
    --kin;
    --iin;
    --rin;
    cin -= 72;
    nxk_dim1 = *nnxk;
    nxk_offset = 1 + nxk_dim1;
    nxk -= nxk_offset;

    /* Function Body */
    mesage_(" ", (ftnlen)1);
    mesage_("*********************************************************", (
	    ftnlen)57);
    mesage_("** MESH ADJUST OPTION IS CURRENTLY LIMITED TO DELETING **", (
	    ftnlen)57);
    mesage_("**      ELEMENTS SIDE BOUNDARY FLAGS BY MATERIAL       **", (
	    ftnlen)57);
    mesage_("*********************************************************", (
	    ftnlen)57);
    mesage_(" ", (ftnlen)1);
/*  ADJUST SIDE BOUNDARY FLAGS BY MATERIALS */
    mesage_("ENTER DATA IN THE FOLLOWING FORMAT:", (ftnlen)35);
    mesage_("[ MATERIAL NUMBER, FLAG ID ]", (ftnlen)28);
    mesage_("HIT RETURN TO END INPUT", (ftnlen)23);
L100:
    if (*icom > *jcom) {
	frefld_(&iz, &iz, ">", mcom, &iostat, jcom, &kin[1], cin + 72, &iin[1]
		, &rin[1], (ftnlen)1, (ftnlen)72);
	*icom = 1;
    }
    if (*icom > *jcom || *(unsigned char *)&cin[*icom * 72] == ' ') {
	++(*icom);
	goto L190;
    } else {
	i1 = iin[*icom];
	++(*icom);
	if (*icom <= *jcom && kin[*icom] > 0) {
	    i2 = iin[*icom];
	    ++(*icom);
	} else {
	    ++(*icom);
	    mesage_("** NOT ENOUGH INFORMATION IS SUPPLIED **", (ftnlen)40);
	    goto L100;
	}
    }
/*  NOW THAT THE MATERIAL (I1) AND THE FLAG ID (I2) ARE ENTERED */
/*  FIRST CHECK TO MAKE SURE THAT THAT MATERIAL IS PRESENT */
    i__1 = *nummat;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (matmap[i__ * 3 + 1] == i1) {
	    j1 = matmap[i__ * 3 + 2];
	    j2 = matmap[i__ * 3 + 3];
	    goto L120;
	}
/* L110: */
    }
    mesage_("** THAT MATERIAL IS NOT PRESENT IN THE MESH **", (ftnlen)46);
    goto L100;
L120:
/*  NOW FIND THE ELEMENT SIDE FLAG */
    i__1 = *nbcsid;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nsflg[i__] == i2) {
	    ii = i__;
	    goto L140;
	}
/* L130: */
    }
    mesage_("** THAT ELEMENT BOUNDARY FLAG IS NOT IN THE MESH **", (ftnlen)51)
	    ;
    goto L100;
L140:
/*  NOW SEARCH THE LOOP FOR ELEMENTS ATTACHED TO THAT BOUNDARY FLAG */
/*  OF THE SPECIFIED MATERIAL */
    ibegin = nsptr[ii];
    iend = nsptr[ii] + nslen[i__] - 1;
    found = FALSE_;
    kount = 0;
    i__1 = iend;
    for (i__ = ibegin; i__ <= i__1; ++i__) {
	if (nelems[i__ - kount] >= j1 && nelems[i__ - kount] <= j2) {
/*  AN ELEMENT SIDE FLAG HAS BEEN FOUND - NOW DELETE IT */
	    found = TRUE_;
	    i__2 = *nslist - 1;
	    for (j = i__ - kount; j <= i__2; ++j) {
		nelems[j] = nelems[j + 1];
/* L150: */
	    }
	    --(*nslist);
	    i__2 = *nvlist - 2;
	    for (j = (i__ - kount << 1) - 1; j <= i__2; ++j) {
		nsiden[j] = nsiden[j + 2];
		wtside[j] = wtside[j + 2];
/* L160: */
	    }
	    *nvlist += -2;
	    --nslen[ii];
	    nvlen[ii] += -2;
	    i__2 = *nbcsid;
	    for (j = ii + 1; j <= i__2; ++j) {
		--nsptr[j];
		nvptr[j] += -2;
/* L170: */
	    }
	    ++kount;
	}
/* L180: */
    }
    if (! found) {
	mesage_("** NO MATCHES OF ELEMENTS WITH THAT BOUNDARY FLAG AND MATER\
IAL **", (ftnlen)65);
    }
    goto L100;
L190:
    return 0;
} /* adjmsh_ */

#ifdef __cplusplus
	}
#endif

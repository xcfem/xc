/* pltrsd.f -- translated by f2c (version 20160102).
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
#include "paving.h"

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltrsd_()
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    extern /* Subroutine */ int vdstbc_(integer *), vdstfc_(integer *), 
	    vdstin_(xc_float *);

    i__1 = (integer) status_1.defout[0];
    vdstfc_(&i__1);
    i__1 = (integer) status_1.defout[1];
    vdstbc_(&i__1);
    vdstin_(&status_1.defout[2]);
    device_1.devp[0] = status_1.defout[0];
    device_1.devp[1] = status_1.defout[1];
    device_1.devp[2] = status_1.defout[2];
    return 0;
} /* pltrsd_ */

#ifdef __cplusplus
	}
#endif

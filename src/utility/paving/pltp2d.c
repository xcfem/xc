/* pltp2d.f -- translated by f2c (version 20160102).
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
/* Subroutine */ int pltp2d_(xc_float *x, xc_float *y, xc_float *xn, xc_float *yn)
{
    /* Initialized data */

    static xc_float dtr = (xc_float).01745329;

    /* Builtin functions */
    

    /* Local variables */
    static xc_float sx, xt, yt, sy, theta;

    xt = *x - device_1.devp[3] / (xc_float)2.;
    yt = *y - device_1.devp[4] / (xc_float)2.;
    theta = mappar_1.mapp[4] * dtr;
    xt *= mappar_1.mapp[0];
    yt *= mappar_1.mapp[1];
    *xn = xt * cos(theta) - yt * sin(theta) + device_1.devp[3] / (xc_float)2. + 
	    mappar_1.mapp[2];
    *yn = yt * cos(theta) + xt * sin(theta) + device_1.devp[4] / (xc_float)2. + 
	    mappar_1.mapp[3];
    sx = (mappar_1.mapp[7] - mappar_1.mapp[5]) / device_1.devp[3];
    sy = (mappar_1.mapp[8] - mappar_1.mapp[6]) / device_1.devp[4];
    *xn = *xn * sx + mappar_1.mapp[5];
    *yn = *yn * sy + mappar_1.mapp[6];
    return 0;
} /* pltp2d_ */

#ifdef __cplusplus
	}
#endif

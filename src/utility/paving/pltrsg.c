/* pltrsg.f -- translated by f2c (version 20160102).
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
#include "f2c.h"

/* Common Block Declarations */

struct {
    real devcap[23], defout[7];
} status_;

#define status_1 status_

struct {
    real devp[5];
} device_;

#define device_1 device_

struct {
    real colp[3], palett[48]	/* was [3][16] */;
} color_;

#define color_1 color_

struct {
    real textp[40];
} text_;

#define text_1 text_

struct {
    real vectp[5], xcur, ycur;
} vectrc_;

#define vectrc_1 vectrc_

struct {
    integer idex[400]	/* was [200][2] */, nvect[400]	/* was [200][2] */;
    real xsize[400]	/* was [200][2] */, ysize[400]	/* was [200][2] */, 
	    x0[4600]	/* was [2300][2] */, y0[4600]	/* was [2300][2] */, 
	    x1[4600]	/* was [2300][2] */, y1[4600]	/* was [2300][2] */;
} font_;

#define font_1 font_

struct {
    real graphp[100];
} graph_;

#define graph_1 graph_

struct {
    real mapp[11];
} mappar_;

#define mappar_1 mappar_

struct {
    integer memory[1000];
} storag_;

#define storag_1 storag_

/* Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/* of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/* NTESS, the U.S. Government retains certain rights in this software. */

/* See packages/seacas/LICENSE for details */
/* ======================================================================= */
/* Subroutine */ int pltrsg_()
{
    graph_1.graphp[6] = (float)1.;
    graph_1.graphp[7] = (float)0.;
    graph_1.graphp[8] = (float)0.;
    graph_1.graphp[9] = (float)1.;
    graph_1.graphp[10] = (float)0.;
    graph_1.graphp[11] = (float)0.;
    graph_1.graphp[12] = (float)0.;
    graph_1.graphp[13] = (float)0.;
    graph_1.graphp[14] = (float)1.;
    graph_1.graphp[15] = (float)0.;
    graph_1.graphp[16] = (float)1.;
    graph_1.graphp[17] = (float).75;
    graph_1.graphp[18] = (float)0.;
    graph_1.graphp[19] = (float).75;
    graph_1.graphp[0] = (float).15;
    graph_1.graphp[1] = (float).075;
    graph_1.graphp[2] = (float).75;
    graph_1.graphp[3] = (float).6;
    graph_1.graphp[4] = (float)1.;
    graph_1.graphp[5] = (float)0.;
    graph_1.graphp[22] = (float)1.;
    graph_1.graphp[20] = (float)1.;
    graph_1.graphp[21] = (float)1.;
    graph_1.graphp[46] = (float)5.;
    graph_1.graphp[31] = (float)1.;
    graph_1.graphp[34] = (float)0.;
    graph_1.graphp[37] = status_1.defout[0];
    graph_1.graphp[35] = status_1.defout[0];
    graph_1.graphp[36] = status_1.defout[0];
    graph_1.graphp[38] = status_1.defout[0];
    graph_1.graphp[39] = (float)3.;
    graph_1.graphp[40] = (float)1.;
    graph_1.graphp[41] = (float)1.;
    graph_1.graphp[43] = (float)5.;
    graph_1.graphp[44] = (float)5.;
    graph_1.graphp[87] = (float)5.;
    graph_1.graphp[88] = (float)5.;
    graph_1.graphp[45] = (float)5.;
    graph_1.graphp[47] = (float)0.;
    graph_1.graphp[48] = (float)0.;
    graph_1.graphp[61] = (float)160.;
    graph_1.graphp[62] = (float)160.;
    graph_1.graphp[63] = (float)160.;
    graph_1.graphp[64] = (float)160.;
    graph_1.graphp[65] = (float)160.;
    graph_1.graphp[66] = (float)160.;
    graph_1.graphp[67] = (float)160.;
    graph_1.graphp[68] = (float)160.;
    graph_1.graphp[69] = (float)160.;
    graph_1.graphp[70] = (float)2.;
    graph_1.graphp[71] = status_1.defout[0];
    graph_1.graphp[72] = (float)0.;
    graph_1.graphp[73] = status_1.defout[0];
    graph_1.graphp[74] = status_1.defout[0];
    graph_1.graphp[75] = status_1.defout[0];
    graph_1.graphp[76] = status_1.defout[0];
    graph_1.graphp[90] = (float)-999999.;
    graph_1.graphp[89] = (float)-999999.;
    graph_1.graphp[91] = (float)0.;
    return 0;
} /* pltrsg_ */

#ifdef __cplusplus
	}
#endif

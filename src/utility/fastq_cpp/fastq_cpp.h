// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//FastQ_CPP.h

#ifndef FASTQ_CPP_H
#define FASTQ_CPP_H

#include "f2c.h"

struct timing_{
    real timea, timep, timec, timepc, timeaj, times;
};

//timing_ timing_1;

int add1cn_(integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, logical *, logical *, logical *, logical *, logical *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, integer *, real *, real *, real *);

int add2cn_(integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, logical *, logical *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, integer *, real *, real *, real *);

int add2el_(integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, logical *);

int add2nd_(integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, real *, real *, real *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, integer *, real *, real *, real *);

int add3nd_(integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, integer *, real *, real *, real *);

int addkxl_(integer *, integer *, integer *, integer *);

int addlxn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int addnod_(integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, integer *, logical *, logical *, logical *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, integer *, real *, real *, real *);

//int addrow_(integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, char *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, integer *, logical *, logical *, logical *, logical *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, ftnlen);

int addrow_(integer *mxnd, integer *mxcorn, integer *mxloop, integer *mln, integer *maxprm, integer *nuid, real *xn, real *yn, real *zn, integer *lxk, integer *kxl, integer *nxl, integer *lxn, real *angle, real *bnsize, integer *lnodes, integer *nbegin, integer *nend, integer *nloop, integer *nextn1, integer *linkpr, integer *kperim, integer *kkkold, integer *lllold, integer *nnnold, integer *iavail, integer *navail, real *xmin, real *xmax, real *ymin, real *ymax, real *zmin, real *zmax, char *dev1, integer *lll, integer *kkk, integer *nnn, integer *nnn2, integer *nadj1, integer *nadj2, integer *lcorn, integer *kloop, logical *graph, logical *video, integer *kreg, logical *done, logical *sizeit, logical *noroom, logical *err, real *xnold, real *ynold, integer *nxkold, integer *linkeg, integer *listeg, real *bmesur, integer *mlink, integer *npnold, integer *npeold, integer *nnxk, real *remesh, real *rexmin, real *rexmax, real *reymin, real *reymax, integer *idivis, real *sizmin, real *emax, real *emin, ftnlen dev1_len);

int addsnp_(integer *, real *, integer *, integer *, real *, logical *);

int addtuk_(integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, logical *, logical *, char *, logical *, logical *, ftnlen);

int addwdg_(integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, logical *, logical *, char *, integer *, logical *, logical *, ftnlen);

int addwt_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, real *);

int adjrow_(integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, char *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, integer *, real *, logical *, logical *, logical *, ftnlen);

int adjtri_(integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, integer *, integer *, char *, logical *, logical *, logical *, logical *, logical *, logical *, integer *, ftnlen);

int apalib_(integer *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, real *, real *, real *);

int apalsm_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, logical *);

int arcpar_(integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, real *, logical *, integer *, integer *, real *, real *);

int arcy_(real *, real *, real *, real *, real *, real *, real *, real *, logical *);

int arelax_(integer *, real *, real *, integer *, integer *, integer *, integer *, real *);

int b4bad_(integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int bcross_(integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, integer *, real *, real *, real *, real *, real *, real *, char *, integer *, logical *, ftnlen);

int bfnode_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int bmschm_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, logical *, logical *, logical *);

int bpinch_(integer *, integer *, integer *, real *, real *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, real *, logical *, logical *);

int bubble_(real *, integer *, integer *, integer *);

int casmo_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *);

int cclock_(real *, real *, integer *, logical *, logical *, logical *);

int ccross_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int ch3to4_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, real *, logical *, integer *);

int chcond_(integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int check_(integer *, integer *, integer *);

int chkhol_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, logical *, logical *, logical * );

int chkkxl_(integer *, integer *, integer *, integer *, logical *);

//int chkrgn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, char *, char *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, logical *, logical *, logical *, ftnlen, ftnlen);

int chkrgn_(integer *ia, integer *l, integer *mp, integer *
	ml, integer *ms, integer *mr, integer *msc, integer *n24, integer *
	ipoint, real *coor, integer *ipboun, integer *iline, integer *ltype, 
	integer *nint, real *factor, integer *lcon, integer *ilboun, integer *
	isboun, integer *iside, integer *nlps, integer *ifline, integer *
	illist, integer *iregn, integer *nspr, integer *ifside, integer *
	islist, integer *nppf, integer *ifpb, integer *listpb, integer *nlpf, 
	integer *iflb, integer *listlb, integer *nspf, integer *ifsb, integer 
	*listsb, integer *ifhole, integer *nhpr, integer *ihlist, integer *
	linkp, integer *linkl, integer *links, integer *linkr, integer *
	linksc, integer *linkpb, integer *linklb, integer *linksb, real *
	rsize, char *scheme, char *defsch, integer *npregn, integer *npsbc, 
	integer *npnode, integer *maxnp, integer *maxnl, integer *max3, real *
	x, real *y, integer *nid, integer *listl, integer *nnps, real *angle, 
	integer *marked, integer *mxnd, integer *mxnper, integer *mxnl, 
	integer *maxnbc, integer *maxsbc, real *amesur, real *xnold, real *
	ynold, integer *nxkold, integer *mmpold, integer *linkeg, integer *
	listeg, real *bmesur, integer *mlink, integer *nprold, integer *
	npnold, integer *npeold, integer *nnxk, logical *remesh, real *rexmin,
	 real *rexmax, real *reymin, real *reymax, integer *idivis, real *
	sizmin, real *emax, real *emin, logical *noroom, logical *errchk, 
	    logical *err, ftnlen scheme_len, ftnlen defsch_len);

int chkwt_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, real *, logical *, logical *, logical *);

int ciapal_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *);

int close2_(integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, logical *, logical *, char *, integer *, logical *, logical *, ftnlen);

int close4_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

//int close6_(integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, char *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, logical *, logical *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, ftnlen);

int close6_(integer *mxnd, integer *mxcorn, integer *mln, 
	integer *nuid, real *xn, real *yn, integer *lxk, integer *kxl, 
	integer *nxl, integer *lxn, real *angle, real *bnsize, integer *
	lnodes, integer *node, integer *nloop, integer *kkkold, integer *
	lllold, integer *nnnold, integer *navail, integer *iavail, real *done,
	 real *xmin, real *xmax, real *ymin, real *ymax, char *dev1, integer *
	lll, integer *kkk, integer *nnn, integer *lcorn, integer *ncorn, 
	logical *graph, logical *video, logical *sizeit, logical *noroom, 
	logical *err, real *xnold, real *ynold, integer *nxkold, integer *
	linkeg, integer *listeg, real *bmesur, integer *mlink, integer *
	npnold, integer *npeold, integer *nnxk, real *remesh, real *rexmin, 
	real *rexmax, real *reymin, real *reymax, integer *idivis, real *
	    sizmin, real *emax, real *emin, ftnlen dev1_len);

int cntcrn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int colaps_(integer *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, real *, real *, char *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, integer *, logical *, logical *, ftnlen);

int comsrt_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *);

int condno_(integer *, integer *, real *, real *, real *, real *, real *, real *, logical *);

int connod_(integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, logical *);

//int crcsiz_(integer *, real *, real *, integer *, integer *, real *, real *, real *, real *, real *, real *, real *, real *, real *, logical *, logical *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, logical *, real *);

int crcsiz_(integer *maxnp, real *x, real *y, integer *nint, 
	integer *n, real *xend, real *yend, real *xcen, real *ycen, real *
	theta1, real *theta2, real *tang, real *aa, real *bb, logical *err, 
	logical *test, real *xnold, real *ynold, integer *nxkold, integer *
	linkeg, integer *listeg, real *bmesur, integer *mlink, integer *
	npnold, integer *npeold, integer *nnxk, logical *remesh, real *rexmin,
	 real *rexmax, real *reymin, real *reymax, integer *idivis, real *
	    sizmin, real *emax, real *emin, logical *graph);

int csmang_(integer *, real *, real *, real *, integer *, real *, integer *);

int d2node_(integer *, real *, real *, integer *, integer *);

int dataok_(integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, logical *, logical *);

int delem_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, logical *);

int delfsq_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, char *, logical *, logical *, logical *, real *, logical *, char *, ftnlen, ftnlen, ftnlen, ftnlen);

int delhol_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int dellxn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int digit_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, logical *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, logical *, char *, logical *, real *, integer *, logical *, logical *, ftnlen, ftnlen, ftnlen);

int dline_(integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, logical *, logical *, real *, real *, real *, real *);

int dlpara_(real *, real *, real *, real *, real *, real *, logical *);

int dpread_(real *, real *, char *, ftnlen);

int dstort_(real *, real *, real *, real *, real *, real *, real *, real *, real *);

int elpspr_(integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, integer *, integer *, real *, real *, logical *);

int endtan_(integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, logical *);

int eqlang_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real * );

int eqlang_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *);

int etheta_(real *, real *, real *, real *, real *, real *, logical *);

int excpus_(real *);

int exdate_(char *, ftnlen);

int exname_(integer *, char *, integer *, ftnlen);

int exparm_(char *, char *, integer *, integer *, integer *, integer *, ftnlen, ftnlen);

int extime_(char *, ftnlen);

int extnd1_(integer *, real *, real *, real *, integer *, integer *, integer *, real *, real *, real *);

int extnd3_(integer *, real *, real *, real *, integer *, integer *, integer *, real *, real *, real *);

int extnd5_(integer *, real *, real *, real *, integer *, integer *, integer *, real *, real *, real *);

int exupcs_(char *, ftnlen);

int fexit_(logical *, integer *, integer *, integer *, char *, integer *, real *, integer *, real *, logical *, char *, ftnlen, ftnlen);

int filsmo_(integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, real *, real *, real *, real *, real *, real *, char *, integer *, ftnlen);

int fixlxn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int fixsub_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int flagd_(integer *, integer *, integer *, integer *, logical *);

int flagk_(integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int flmnmx_(integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, real *, real *, real *, logical *);

int fndlin_fq__(integer *, integer *, integer *, integer *, integer *, logical *);

int fndlnk_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int fq_rotate__(integer *, real *, real *, integer *, integer *);

int frefld_(integer *, integer *, char *, integer *, integer *, integer *, integer *, char *, integer *, real *, ftnlen, ftnlen);

int fxnuid_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int gdata_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, char *, char *, real *, char *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, integer *, logical *, char *, logical *, char *, logical *, real *, logical *, logical *, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen);

int getang_(integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, logical *);

int getcrn_(integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, logical *, logical *, logical *, logical * );

int getdum_(integer *, char *, integer *, ftnlen);

int getext_(integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *);

int getfrm_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *);

int geti12_(integer *, integer *, integer *, char *, integer *, integer *, integer *, integer *, integer *, ftnlen);

int geti1r_(integer *, integer *, integer *, char *, integer *, real *, integer *, integer *, real *, integer *, ftnlen);

int getime_(real *);

int getinp_(integer *, integer *, char *, char *, integer *, ftnlen, ftnlen);

int getkxn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int getlxn_(integer *, integer *, integer *, integer *, integer *, logical *);

int getm1_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, integer *, char *, integer *, logical *, logical *, logical *, logical *, ftnlen);

int getm3_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, logical *, logical *);

int getm5_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, logical *, logical *);

int getnxn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int getplp_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int getrow_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, real *, real *, real *, real *, real *, real *, char *, integer *, logical *, integer *, logical *, logical *, ftnlen);

int getsbc_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

//int getsiz_(real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, real *, real *, real *);

int getsiz_(real *xnold, real *ynold, integer *nxkold, 
	integer *linkeg, integer *listeg, real *bmesur, integer *mlink, 
	integer *npnold, integer *npeold, integer *nnxk, real *remesh, real *
	rexmin, real *rexmax, real *reymin, real *reymax, integer *idivis, 
	    real *sizmin, real *emax, real *emin, real *x, real *y, real *size);

int gettrn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, real *, real *, logical *, logical *, logical *);

int getwt_(integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, real *, integer *, real *, logical *, logical *);

int gkxn_(integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int gnlist_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, char *, integer *, integer *, logical *, logical *, ftnlen);

int gnxka_(integer *, real *, real *, integer *, integer *, real *, integer *, integer *, logical *);

int gtxdum_(real *, char *, integer *, ftnlen);

int help_fq__(integer *);

int inbody_(integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int inboun_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int inbrst_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int indexi_fq__(integer *, integer *, integer *, integer *);

int infact_(integer *, integer *, integer *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *);

int ingrid_(integer *, real *, integer *, integer *, real *, integer *, logical *);

int ingrpn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int inhole_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int inintr_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int initdg_(integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, real *, real *, real *, logical *, logical *, ftnlen);

int inline_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, logical *, logical *);

int innerh_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int inpoin_(integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, logical *, logical *);

int inqstr_(char *, char *, ftnlen, ftnlen);

int inqtru_(char *, logical *, ftnlen);

int inregn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int inrenm_(integer *, integer *, char *, real *, integer *, integer *, char *, logical *, ftnlen, ftnlen);

int inschm_(integer *, integer *, integer *, integer *, integer *, char *, integer *, char *, integer *, char *, logical *, logical *, ftnlen, ftnlen, ftnlen);

int inside_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int intrup_(char *, logical *, integer *, integer *, integer *, char *, integer *, real *, integer *, ftnlen, ftnlen);

int intsct_(real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, logical *);

int invert_fq__(integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, char *, integer *, integer *, real *, real *, ftnlen);

int invmap_(real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, logical *);

int irever_(integer *, integer *);

int isolap_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, real *, real *);

int keep3_(integer *, integer *, integer *, integer *);

int keyin_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, char *, char *, char *, real *, char *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, real *, char *, logical *, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen);

int kxnadd_(integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int l2smo_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *);

int labove_(integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, integer *, integer *, integer *, integer *);

int lcolor_(char *, ftnlen);

int linepr_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *);

int linkbc_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int linken_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int linlen_(integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, logical *);

int listbf_(integer *, integer *, char *, integer *, integer *, integer *, integer *, integer *, integer *, ftnlen);

int list_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, char *, char *, char *, real *, char *, logical *, logical *, logical *, logical *, logical *, logical *, real *, char *, logical *, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen);

int llist_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int longel_(integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, real *, real *, real *, real *, integer *, integer *, logical *, logical *, logical *, logical *, logical *, integer *);

int lowfnd_(integer *, integer *, integer *, integer *, integer *, integer *);

int lswap_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int ltadd_(integer *, integer *, integer *, integer *, integer *);

int ltnew_(integer *, integer *);

int ltsort_(integer *, integer *, integer *, integer *, logical *);

int lupang_(integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, char *, integer *, logical *, ftnlen);

int mak2el_(integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, real *, real *, integer *, integer *, real *, integer *);

int marksm_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int match2_(integer *, integer *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, integer *, integer *, real *, real *, integer *, logical *, logical *);

int mddebg_(integer *);

int mddel_(char *, ftnlen);

int mderor_(integer *);

int mdfill_(integer *);

int mdinit_(real *);

int mdlong_(char *, integer *, integer *, ftnlen);

int mdrsrv_(char *, integer *, integer *, ftnlen);

int mdstat_(integer *, integer *);

int mesage_(const char *, ftnlen);

int mesh_(real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, char *, char *, char *, real *, char *, logical *, integer *, logical *, char *, logical *, logical *, logical *, logical *, logical *, char *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, real *, logical *, logical *, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen) ;

int midnod_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical * );

int minmax_fq__(integer *, integer *, real *, real *, real *, real *, real *, real *);

int mkused_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *);

int mmschm_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, char *, integer *, logical *, ftnlen);

int mnmxk_(integer *, integer *, integer *, integer *, real *, real *, real *, integer *, real *, real *, real *, real *);

int mnorm_(integer *, real *, real *, integer *, integer *, real *);

int mp2pt_(integer *, real *, real *, real *, real *, integer *);

int mpd2vc_(integer *, real *, real *, real *, real *);

int mport2_(real *, real *, real *, real *);

int mpview_(real *, real *, real *, real *);

int ncklce_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, logical *, logical *);

int ndstat_(integer *, integer *, real *, integer *);

int node12_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int nodord_(integer *, real *, real *, integer *, integer *, integer *);

int normnd_(integer *, real *, real *);

int nps_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int nsplit_(integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *);

int nxkbdy_(integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *);

int nxkord_(integer *, integer *);

int paving_(integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, real *, logical *, char *, integer *, logical *, logical *, logical *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, ftnlen);

int pcross_(integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, real *, real *, char *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, integer *, logical *, logical *, ftnlen);

int pdata_(integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, char *, char *, real *, real *, integer *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, char *, real *, real *, real *, real *, real *, real *, real *, real *, char *, char *, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen);

int perim_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, logical *, logical *, logical *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *);

int periml_(integer *, integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, char *, integer *, logical *, ftnlen);

int pickm1_(integer *, real *, real *, real *, integer *, integer *, logical *);

int pickm3_(integer *, real *, real *, real *, integer *, integer *, integer *);

int pickm5_(integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *);

int picktr_(integer *, real *, real *, integer *, real *, logical *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *);

int pinch_(integer *, integer *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, real *, real *, char *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, integer *, logical *, logical *, ftnlen);

int pline_(integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, logical *, logical *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, logical *, real *);

int plotl_(integer *, real *, real *, integer *, integer *, real *, real *, real *, real *, integer *, char *, ftnlen);

int pltbel_();

int pltbgn_();

int pltcrs_(real *, real *, char *, ftnlen);

int pltd2g_(real *, real *, real *, real *);

int pltdrw_(real *, real *);

int pltend_();

int pltflu_();

int pltfrm_(integer *);

int pltg2d_(real *, real *, real *, real *);

int pltgph_(real *, real *, integer *, char *, char *, char *, char *, ftnlen, ftnlen, ftnlen, ftnlen);

int pltini_(real *, real *, real *, real *, real *, integer *, integer *);

int pltint_();

int pltmov_(real *, real *);

int pltstd_(integer *, real *);

int pltstg_(integer *, real *);

int pltstv_(integer *, real *);

int pltxhe_(real *, real *);

int pltxhl_(char *, real *, ftnlen);

int pltxth_(real *, real *, char *, ftnlen);

int pmesh_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, logical *, real *, integer *, real *, real *, real *, real *, real *, real *, real *, real *, char *, char *, logical *, logical *, char *, logical *, ftnlen, ftnlen, ftnlen);

int pmschm_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, integer *, integer *, logical *, logical *, logical *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, ftnlen);

int pschem_(integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, char *, logical *, integer *, integer *, integer *, integer *, char *, logical *, logical *, logical *, integer *, integer *, integer *, char *, integer *, real *, integer *, integer *, integer *, real *, real *, real *, real *, integer *, logical *, logical *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, ftnlen, ftnlen, ftnlen, ftnlen);

int putlxn_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int qaaval_(integer *, integer *, real *, real *, real *, real *, real *, logical *);

int qual2n_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, real *, logical *, logical *, logical *, logical *, integer *, integer *, integer *, integer *, logical *);

int qual3_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, real *, logical *, logical *, logical *);

int qual4_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, real *, logical *, logical *);

int rdfsq_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, char *, char *, char *, real *, char *, logical *, logical *, logical *, logical *, logical *, logical *, real *, integer *, real *, logical *, logical *, ftnlen, ftnlen, ftnlen, ftnlen);

int rdmesh_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int regext_(integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *);

int repsmo_(integer *, real *, real *, integer *, integer *, integer *, integer *, real *, real *, integer *);

int resta_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, logical *, logical *);

int restry_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, logical *, logical *, logical *);

int revers_(real *, integer *);

int rgnsch_(integer *, integer *, integer *, char *, integer *, real *, integer *, logical *, integer *, integer *, integer *, integer *, char *, char *, char *, integer *, integer *, logical *, logical *, logical *, logical *, logical *, integer *, logical *, ftnlen, ftnlen, ftnlen, ftnlen);

int ringbl_();

int rmesh_(integer *, integer *, real *, real *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int rowsmo_(integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, real *, real *, integer *, integer *, real *, integer *, logical *, real *, real *, real *, real *, real *, real *, char *, integer *, ftnlen);

int rplotl_(integer *, real *, real *, real *, integer *, real *, real *, real *, real *, real *, real *, integer *, char *, integer *, ftnlen);

int savreg_(integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, integer *);

int setaxs_(real *, real *);

int setcir_(integer *, integer *, integer *, integer *, integer *, logical *);

int setlop_(integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int setn02_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *);

int sew2_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *);

int sflush_();

int skinit_(integer *, integer *, integer *, integer *);

int skpop_(integer *, integer *, integer *, integer *);

//int skprnt_(integer *, integer *, integer *, integer *);

int skprnt_(integer *lunit, integer *len, real *rstack, integer *ndim);

int skpush_(integer *, integer *, integer *, integer *);

int smogs_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *);

int snapit_(integer *);

int snappt_(integer *, real *, integer *, real *, real *);

int snapxy_(integer *, integer *, integer *, integer *, real *, integer *, real *, integer *);

int sortia_(integer *, integer *, integer *, integer *);

int sort_(integer *, integer *, integer *);

int sortst_(integer *, real *, integer *, integer *, integer *, integer *);

int spaced_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *);

int spawn_(logical * );

int spawn_(logical *);

//int spring_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, logical *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, logical *);

int spring_(integer *mp, integer *ml, integer *ms, integer *
	mxnper, integer *mxnd, integer *maxnbc, integer *maxsbc, integer *l, 
	integer *ipoint, real *coor, integer *ipboun, integer *linkp, integer 
	*iline, integer *ltype, integer *nint, real *factor, integer *lcon, 
	integer *ilboun, integer *isboun, integer *linkl, integer *nlpb, 
	integer *jfline, integer *jllist, integer *linkpb, integer *nppf, 
	integer *ifpb, integer *listpb, integer *linklb, integer *nlpf, 
	integer *iflb, integer *listlb, integer *linksb, integer *nspf, 
	integer *ifsb, integer *listsb, integer *lstnbc, real *x, real *y, 
	integer *nid, real *xn, real *yn, integer *nuid, integer *lxk, 
	integer *nnn, integer *kkk, integer *lll, integer *knbc, integer *
	ksbc, logical *err, logical *addlnk, logical *count, logical *noroom, 
	real *amesur, real *xnold, real *ynold, integer *nxkold, integer *
	mmpold, integer *linkeg, integer *listeg, real *bmesur, integer *
	mlink, integer *nprold, integer *npnold, integer *npeold, integer *
	nnxk, real *remesh, real *rexmin, real *rexmax, real *reymin, real *
	reymax, integer *idivis, real *sizmin, real *emax, real *emin, 
	    logical *graph);
int squash_(integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, logical *, logical *, logical *);

int srtnbc_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *);

int srtsbc_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *);

int strait_(integer *, integer *, integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, ftnlen);

int strcut_(char *, ftnlen);

int stripb_(char *, integer *, integer *, ftnlen);

int strlng_(char *, integer *, ftnlen);

int strsiz_(integer *, real *, real *, integer *, integer *, real *, real *, real *, real *, real *, logical *, logical *, real *, real *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *, logical *, real *);

int subpen_(integer *, integer *, real *, real *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *);

int subtri_(integer *, integer *, real *, real *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *);

//int subtrn_(integer *, integer *, integer *, real *, real *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, real *, real *, real *, real *, real *, real *, logical *, logical *);

int subtrn_(integer *nper, integer *newper, integer *ip, 
	real *x, real *y, integer *nid, real *xsub, real *ysub, integer *
	nidsub, integer *i1, integer *i2, integer *i3, integer *i4, integer *
	i5, integer *i6, integer *i7, integer *i8, real *xcen1, real *ycen1, 
	real *xcen2, real *ycen2, real *xmid1, real *ymid1, real *xmid2, real 
	    *ymid2, real *ccw, real *err);
int symbol_(integer *, real *, real *, char *, ftnlen);

int tabint_(real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, logical *);

int tablet_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, logical *, logical *, real *, real *, real *, real *, real *, real *, real *, real *, real *, logical *, char *, logical *, real *, integer *, logical *, logical *, logical *, logical *, logical *, char *, real *, ftnlen, ftnlen, ftnlen, ftnlen);

int tmschm_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, logical *, logical *);

int tridel_(integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, real *, integer *, char *, integer *, real *, real *, real *, real *, real *, real *, logical *, logical *, logical *, logical *, ftnlen);

int trifix_(integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, real *, integer *, char *, integer *, real *, real *, real *, real *, real *, real *, logical *, logical *, logical *, logical *, ftnlen);

int tuck_(integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *);

int umschm_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, real *, real *, real *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, logical *, logical *);

int undelm_(integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, logical *);

int unisnp_(integer *, real *, integer *, integer *, real *, real *, real *);

int vdescp_(integer *, integer *, integer *);

int vdiqes_(integer *, integer *);

int vinter_(integer *, real *, real *, integer *, integer *, integer *, real *, real *, real *, real *, logical *);

int wedge_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, logical *, logical *, logical *, logical *, logical *);

int wrfsq_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, char *, char *, char *, real *, char *, logical *, logical *, logical *, logical *, logical *, real *, integer *, logical *, logical *, ftnlen, ftnlen, ftnlen, ftnlen, ftnlen);

int zhole_(integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, real *, real *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, integer *, real *, logical *, logical *, real *, real *, real *, integer *, integer *, integer *, integer *, real *, integer *, integer *, integer *, integer *, integer *, logical *, real *, real *, real *, real *, integer *, real *, real *, real *);

int zoomlt_(integer *, integer *, integer *, char *, real *, integer *, integer *, integer *, logical *, logical *, char *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, real *, ftnlen, ftnlen);

#endif

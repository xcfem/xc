/*
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
*/
/*
 * File:  nmat.h
 * =============
 * altered to improve data access
 *
 * Originally written by:  David R. Mackay
 *
 * Modified by:
 *  Jun Peng (junpeng@stanford.edu)
 *  Prof. Kincho H. Law
 *  Stanford University
 * --------------------
 */


#ifndef nmat_h
#define nmat_h

#include "FeStructs.h"

int pfsfct(int neqns, double *diag, double **penv, int nblks, 
	   int *xblk, OFFDBLK **begblk, OFFDBLK *first, int *rowblks);

int pfefct(int neqns, double **penv, double *diag);

void pfsslv(int neqns, double *diag, double **penv, int nblks, 
	    int *xblk, double *rhs, OFFDBLK **begblk);

void pflslv (int neqns, double **penv, double *diag, double *rhs);

void pfuslv(int neqns, double **penv, double *diag, double *rhs);


#endif

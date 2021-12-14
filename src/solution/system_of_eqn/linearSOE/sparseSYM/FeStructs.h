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
 * File:  FeStructs.h
 * ==================
 * Data structures for finite element program.
 * This is changed from the original program by only including
 * the define of OFFDBLK.
 * ----------------------
 * Written by:
 * Jun Peng (junpeng@stanford.edu)
 * Prof. Kincho H. Law
 * Stanford University
 */

#ifndef FeStructs_h
#define FeStructs_h

struct offdblk
{
	int             row    ;
	int             beg    ;
	struct offdblk  *bnext ;
	struct offdblk  *next  ;
	double          *nz    ;
};

typedef  struct offdblk  OFFDBLK;

#endif

/************************************************************/

/*
 * data structure
 *  struct - pointer to pointer to nonzeros in L
 *  struct - pointer to data structure containing nonzeros in L
 *  struct .row - row number of i th row segment
 *  struct .beg - beginning column of i th row segment
 *  struct .bnext - pointer to next row segment in this column block
 *  struct .nz   - pointer to coefficients of L in rowsegment i
 *  struct .next - pointer to next row segment with the same row number 
 *		   this row segment if it exists, other wise the row segment
 *		   with the next highest row number
 *
 *  begblk       - pointer to the first structure in each block 
 *  xblk         - an integer array containing the beginning column/row
 *		   of each block
 *  penv         - pointers to pointers of the nonzeros in the 
 *		   profile next to the diagonal.
 *  penv[i]      - pointer to the nonzeros of L on the profile
 *		   structure next to the diagonal
 *  diag[i]      - the i the diagonal of L
 */

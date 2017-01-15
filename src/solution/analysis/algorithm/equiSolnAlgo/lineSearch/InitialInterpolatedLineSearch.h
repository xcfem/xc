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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.3 $
// $Date: 2003/02/14 23:00:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/InitialInterpolatedLineSearch.h,v $

// Written: fmk 
// Created: 11/01

// Description: This file contains the class definition for LinearInterpolatedSearch.
// This performs the search by using a form of linear interpolation to find the best solution.
// Solution procedure follows the one in Crissfields book.
// (M.A. Crissfield, Nonlinear Finite Element Analysis of Solid and Structures, Wiley. 97).
// NOTE: it is not quite linear interpolation/false-position/regula-falsi as eta(0) = 0.0
// does not change. uses eta(i) = eta(i-1)*s0
//                                -----------
//                                s0 - s(i-1)  to compute eta(i)
//

#ifndef InitialInterpolatedLineSearch_h
#define InitialInterpolatedLineSearch_h

#include "solution/analysis/algorithm/equiSolnAlgo/lineSearch/LineSearch.h"
namespace XC {
class Vector;

//! @ingroup EQSolAlgo
//
//! @brief This performs the search by using a form of linear
//! interpolation to find the best solution.
class InitialInterpolatedLineSearch: public LineSearch
  {
    friend class FEM_ObjectBroker;
    friend class NewtonLineSearch;
    InitialInterpolatedLineSearch(void);
    LineSearch *getCopy(void) const;
  public:
    int search(double s0, 
	       double s1, 
	       LinearSOE &theSOE, 
	       IncrementalIntegrator &theIntegrator);

  };
inline LineSearch *InitialInterpolatedLineSearch::getCopy(void) const
  { return new InitialInterpolatedLineSearch(*this); }
} // end of XC namespace

#endif



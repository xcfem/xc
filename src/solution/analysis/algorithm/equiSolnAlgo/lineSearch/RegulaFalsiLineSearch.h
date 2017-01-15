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
// $Date: 2003/02/14 23:00:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/RegulaFalsiLineSearch.h,v $

// Written: fmk 
// Created: 11/01

// Description: This file contains the class definition for RegulaFalsiLineSearch.
// This performs the search for U(i+1) = U(i) + eta * deltaU(i) by using the 
// secant method to find the best solution.
//
//                eta(j+1) = eta(u) -  s(u) * (eta(l) -eta(u))
//                                     ------------------------
//                                           s(l) - s(u)
//
// where     s(j) = U(i+1,j) ^ R(U(i+1, j))
//
//  and      U(i+1,j) = U(i) + eta(j)*deltaU(i)
//
// note that as we want eta(u) and eta(l) to bracket the solution, 
// i.e. s(u)*s(l)<0, if at all possible - the lower and upper bounds 
// change depending on sign.
//
//      if s(eta(j+1))*s(l) < 0 { eta(u) = eta(j+1) and s(u) = s(eta(j+1))
//      if s(eta(j+1))*s(u) < 0 { eta(l) = eta(j+1) and s(l) = s(eta(j+1))
//      if s(eta(j+1))*s(u) == 0  SOLN FOUND.

#ifndef RegulaFalsiLineSearch_h
#define RegulaFalsiLineSearch_h

#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/LineSearch.h>
namespace XC {
class Vector;

//! @ingroup EQSolAlgo
//
//! @brief performs the search for U(i+1) = U(i) + eta * deltaU(i) by
//! regula falsi method to find the best solution.
class RegulaFalsiLineSearch: public LineSearch
  {
    friend class NewtonLineSearch;
    friend class FEM_ObjectBroker;
    RegulaFalsiLineSearch(void);
    LineSearch *getCopy(void) const;
  public:

    int search(double s0, 
	       double s1, 
	       LinearSOE &theSOE, 
	       IncrementalIntegrator &theIntegrator);

  };
inline LineSearch *RegulaFalsiLineSearch::getCopy(void) const
  { return new RegulaFalsiLineSearch(*this); }
} // end of XC namespace

#endif



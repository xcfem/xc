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
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/Broyden.h,v $
                                                                        
#ifndef Broyden_h
#define Broyden_h

// File: ~/OOP/analysis/algorithm/Broyden.h 
// 
// Written: Ed C++ Love
// Created: 04/01

// Description: This file contains the class definition for
// Broyden.  Broyden is a class which performs the Broyden
// solution algorithm in solving the equations.
// No member functions are declared as virtual as 
// it is not expected that this class will be subclassed.
// 
// What: "@(#)Broyden.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/BFBRoydenBase.h>

namespace XC {

//! @ingroup EQSolAlgo
//
//! @brief Broyden method solution algorithm.
//!
//! Broyden algorithm object for general unsymmetric systems which performs
//! successive rank-one updates of the tangent at the first iteration of the
//! current time step. 
class Broyden: public BFBRoydenBase
  {
  private:
    void BroydenUpdate(IncrementalIntegrator *theIntegrator, LinearSOE *theSOE,
			Vector &du, int count);

    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    Broyden(AnalysisAggregation *,int tangent = CURRENT_TANGENT, int n= 10);    
    Broyden(AnalysisAggregation *,ConvergenceTest &theTest,int tangent = CURRENT_TANGENT, int n= 10);
    virtual SolutionAlgorithm *getCopy(void) const;
  public:

    int solveCurrentStep(void);

    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;    
  };
} // end of XC namespace

#endif



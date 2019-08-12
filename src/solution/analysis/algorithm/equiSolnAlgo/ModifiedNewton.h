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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/ModifiedNewton.h,v $
                                                                        
                                                                        
#ifndef ModifiedNewton_h
#define ModifiedNewton_h

// File: ~/OOP/analysis/algorithm/ModifiedNewton.h 
// 
// Written: fmk 
// Created: 11/96 
// Revision: A 
//
// Description: This file contains the class definition for 
// ModifiedNewton. ModifiedNewton is a class which performs a modified 
// Newton-Raphson  solution algorithm in solving the equations.
// No member functions are declared as virtual as 
// it is not expected that this class will be subclassed.
// 
// What: "@(#)ModifiedNewton.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/NewtonBased.h>

namespace XC {
class ConvergenceTest;

//! @ingroup EQSolAlgo
//
//! @brief Uses the tangent stiffness matrix computed in the
//! first iteration until convergence is achieved.
//!
//! The ModifiedNewton class is an algorithmic class which obtains a
//! solution to a non-linear system using the modified Newton-Raphson iteration
//! scheme. The Newton-Rapson iteration scheme is based on a Taylor expansion
//! of the non-linear system of equations \f$R(U) = 0\f$ about an approximate
//! solution \f$U{(i)}\f$:
//! \begin{equation} 
//! R(U) = 
//! R(U^{(i)}) +
//! \left[ {\frac{\partial R}{\partial U} \vert}_{U^{(i)}}\right]
//! \left( U - U^{(i)} \right) 
//! \end{equation}
//!
//! \noindent which can be expressed as:
//! \begin{equation}
//! K^{(i)}  \Delta U{(i)} = R(U^{(i)})
//! \end{equation}
//! which is solved for \f$\Delta U^{(i)}\f$ to give approximation for
//! \f$U^{(i+1)} = U^{(i)} + \Delta U^{(i)}\f$. To start the
//! iteration \f$U^{(1)} = U_{trial}\f$, i.e. the current trial
//! response quantities are chosen as initial response quantities.
//!
//! in the modified version the tangent is formed only once, i.e
//! \begin{equation}
//! K^{(1)}  \Delta U^{(i)} = R(U^{(i)})
//! \end{equation}
//!
//! To stop the iteration, a test must be performed to see if convergence
//! has been achieved at each iteration. Each NewtonRaphson object is
//! associated with a ConvergenceTest object. It is this object which
//! determines if convergence has been achieved.
class ModifiedNewton: public NewtonBased
  {
    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    ModifiedNewton(AnalysisAggregation *,int tangent = CURRENT_TANGENT);
    virtual SolutionAlgorithm *getCopy(void) const;
  public:
    int solveCurrentStep(void);    
    void Print(std::ostream &s, int flag =0);    
  };

} // end of XC namespace

#endif



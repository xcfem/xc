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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/Linear.h,v $
                                                                        
                                                                        
// File: ~/OOP/analysis/algorithm/Linear.h 
// 
// Written: fmk 
// Created: Sun Sept 15 15:06:47: 1996 
// Revision: A 
//

// Description: This file contains the class definition for 
// Linear. Linear is a class which performs a linear solution algorithm
// to solve the equations. No member functions are declared as virtual as 
// it is not expected that this class will be subclassed.
// 
// What: "@(#)Linear.h, revA"

#ifndef Linear_h
#define Linear_h

#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>

namespace XC {

//! @ingroup EQSolAlgo
//
//! @brief Linear solution algorithm.
//!
//! The Linear class is an algorithmic class which uses the
//! linear solution algorithm to solve the equations. This is based on
//! a Taylor expansion of the linear system \f$R(U) = zero\f$ about an
//! approximate solution \f$U_{a}\f$.
//! \begin{equation} 
//! R(U) = 
//! R(U_{a}) +
//! \left[ {\frac{\partial R}{\partial U} \vert}_{U_{a}}\right]
//! \left( U - U_{a} \right) 
//! \end{equation}
//! \noindent which can be expressed as:
//! \begin{equation}
//! K_{a} \Delta U = R(U_{a})
//! \end{equation}
//! which is solved for \f$\Delta U\f$ to give the approximation 
//! \f$U = U_{a} + \Delta U\f$.
//! To start the iteration \f$U_a = U_{trial}\f$, i.e. the current trial
//! response quantities are chosen as approximate solution quantities.
class Linear: public EquiSolnAlgo
  {
    int resuelve();
  protected:
    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    Linear(AnalysisAggregation *);
    virtual SolutionAlgorithm *getCopy(void) const;
  public:

    int solveCurrentStep(void);
    int setConvergenceTest(ConvergenceTest *theNewTest);
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0) const;    
  };
} // end of XC namespace

#endif



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
// Newton-Raphson  solution algorihm in solving the equations.
// No member functions are declared as virtual as 
// it is not expected that this class will be subclassed.
// 
// What: "@(#)ModifiedNewton.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/NewtonBased.h>

namespace XC {
class ConvergenceTest;

//! @ingroup EQSolAlgo
//
//! @brief Emplea la matriz de rigidez tangente en la
//! primera iteración para iterar hasta alcanzar la convengencia.
class ModifiedNewton: public NewtonBased
  {
    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    ModifiedNewton(SoluMethod *,int tangent = CURRENT_TANGENT);
    virtual SolutionAlgorithm *getCopy(void) const;
  public:
    int solveCurrentStep(void);    
    void Print(std::ostream &s, int flag =0);    
  };

inline SolutionAlgorithm *ModifiedNewton::getCopy(void) const
  { return new ModifiedNewton(*this); }
} // end of XC namespace

#endif



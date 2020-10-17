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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/29 22:42:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/EquiSolnConvAlgo.h,v $
                                                                        
                                                                        
#ifndef EquiSolnConvAlgo_h
#define EquiSolnConvAlgo_h

// File: ~/OOP/analysis/algorithm/EquiSolnConvAlgo.h 
// 
// Written: fmk 
// Created: 11/96 
// Revision: A 
//

// Description: This file contains the class definition for 
// EquiSolnConvAlgo. EquiSolnConvAlgo is a class which performs a Newton-Raphson 
// solution algorithm in solving the equations.
// No member functions are declared as virtual as 
// it is not expected that this class will be subclassed.
// 
// What: "@(#)EquiSolnConvAlgo.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>

namespace XC {
//! @ingroup EQSolAlgo
//
//! @brief EquiSolnConvAlgo is a class which performs a Newton-Raphson 
//! solution algorithm in solving the equations.
//! No member functions are declared as virtual as 
//! it is not expected that this class will be subclassed.
class EquiSolnConvAlgo: public EquiSolnAlgo
  {
  protected:
    ConvergenceTest *theTest;

    EquiSolnConvAlgo(SolutionStrategy *,int classTag);    
    EquiSolnConvAlgo(SolutionStrategy *,int classTag,ConvergenceTest *);
  public:

    int setConvergenceTest(ConvergenceTest *theNewTest);
    ConvergenceTest *getConvergenceTestPtr(void);
    const ConvergenceTest *getConvergenceTestPtr(void) const;
  };
} // end of XC namespace

#endif



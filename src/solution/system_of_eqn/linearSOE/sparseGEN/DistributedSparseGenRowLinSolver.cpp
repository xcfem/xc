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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/12/06 22:20:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSolver.cpp,v $
                                                                        
// Written: fmk 
// Created: 04/05
//
// Description: This file contains the implementation for DistributedSparseGenRowSolver
//
// What: "@(#) DistributedSparseGenRowLinSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSolver.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSOE.h>

XC::DistributedSparseGenRowLinSolver::DistributedSparseGenRowLinSolver(int theClassTag)    
:LinearSOESolver(theClassTag),
 theSOE(nullptr)
  {}    

//! @brief Sets the system of equations to solve.
bool XC::DistributedSparseGenRowLinSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    DistributedSparseGenRowLinSOE *tmp= dynamic_cast<DistributedSparseGenRowLinSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << getClassName() << "::setLinearSOE: el system of equations no es del tipo adecuado para este solver." << std::endl;
    return retval;
  }

//! @brief Sets the system of equations to solve.
bool XC::DistributedSparseGenRowLinSolver::setLinearSOE(DistributedSparseGenRowLinSOE &theDistributedSparseGenRowSOE)
  { return setLinearSOE(&theDistributedSparseGenRowSOE); }









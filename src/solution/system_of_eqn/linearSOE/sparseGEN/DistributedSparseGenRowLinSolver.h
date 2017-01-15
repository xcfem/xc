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
                                                                        
#ifndef DistributedSparseGenRowLinSolver_h
#define DistributedSparseGenRowLinSolver_h

// $Revision: 1.1 $
// $Date: 2005/12/06 22:20:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSolver.h,v $

// Written: fmk 
// Created: 04/05
// Revision: A
//
// Description: This file contains the class definition for DistributedSparseGenRowLinSolver.
// DistributedSparseGenRowLinSolver is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.  Instances of DistributedSparseGenRowLinSolver 
// are used to solve a system of equations of type DistributedSparseGenRowLinSOE.
//
// What: "@(#) DistributedSparseGenRowLinSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
namespace XC {
class DistributedSparseGenRowLinSOE;

//! @ingroup LinearSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de sistemas de ecuaciones lineales con matriz dispersa
//! distribuida no simétrica.
class DistributedSparseGenRowLinSolver : public LinearSOESolver
  {
  protected:
    DistributedSparseGenRowLinSOE *theSOE;

    DistributedSparseGenRowLinSolver(int classTag);    
    bool setLinearSOE(LinearSOE *theSOE);
  public:

    virtual bool setLinearSOE(DistributedSparseGenRowLinSOE &theSOE);
  };
} // end of XC namespace

#endif


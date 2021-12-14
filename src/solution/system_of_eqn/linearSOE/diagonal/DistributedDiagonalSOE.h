// -*-c++-*-
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
// $Date: 2005/05/25 23:33:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE.h,v $
                                                                        
// Written: fmk 
// Created: 05/05
//
// Description: This file contains the class definition for DistributedDiagonalSOE
// DistributedDiagonalSOE is a subclass of LinearSOE. It stores a diagonal system
// of equation, i.e. just the diagonal

// What: "@(#) DistributedDiagonalSOE.h, revA"

#ifndef DistributedDiagonalSOE_h
#define DistributedDiagonalSOE_h

#include <solution/system_of_eqn/linearSOE/FactoredSOEBase.h>
#include "solution/system_of_eqn/linearSOE/DistributedLinSOE.h"
#include <utility/matrix/ID.h>
#include "utility/matrix/Vector.h"

namespace XC {
class DistributedDiagonalSolver;

//! @ingroup SOE
//
//! @brief Diagonal matrix distributed systems of equations.
class DistributedDiagonalSOE: public FactoredSOEBase, public DistributedLinSOE
  {
  private:
    Vector A;

    int numProcesses;

    ID myDOFs;
    ID myDOFsShared;
    int numShared;
    Vector dataShared;
    Vector vectShared;

    friend class SolutionStrategy;
    DistributedDiagonalSOE(SolutionStrategy *);
    SystemOfEqn *getCopy(void) const;
  public:
    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    
    void zeroA(void);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    friend class DistributedDiagonalSolver;
  };
inline SystemOfEqn *DistributedDiagonalSOE::getCopy(void) const
  { return new DistributedDiagonalSOE(*this); }
} // end of XC namespace


#endif




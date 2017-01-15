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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/ActorPetscSOE.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/petsc/ActorPetscSOE.h
//
// Written: fmk & om
// Created: 7/98
// Revision: A
//
// Description: This file contains the class definition for ActorPetscSOE
// ActorPetscSOE is a subclass of LinearSOE. It uses the LAPACK storage
// scheme to store the components of the A matrix, which is a full matrix.


// What: "@(#) ActorPetscSOE.h, revA"

#ifndef ActorPetscSOE_h
#define ActorPetscSOE_h

#include <solution/system_of_eqn/linearSOE/LinearSOE.h>

// extern "C" {
#include "petsc.h"
// }

namespace XC {
class PetscSolver;
class PetscSOE;

//! @ingroup SOE
//
//! @brief ??.
class ActorPetscSOE
  {
  private:
    MPI_Comm theComm;
    PetscSOE *theSOE;  // the local portion of the SOE
    PetscSolver *theSolver; // created locally via data from process 0
    int myRank;				 
    int recvData[3];				 
    void *recvBuffer;
    int numProcessors;

    friend class SoluMethod;
    ActorPetscSOE(SoluMethod *,PetscSolver &theSolver, int blockSize);     
  public:
    ~ActorPetscSOE(void);
    int run(void);
  };
} // end of XC namespace


#endif




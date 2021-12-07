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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/ShadowPetscSOE.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/petsc/ShadowPetscSOE.h
//
// Written: fmk & om
// Created: 7/98
// Revision: A
//
// Description: This file contains the class definition for ShadowPetscSOE
// ShadowPetscSOE is a subclass of LinearSOE. 

// What: "@(#) ShadowPetscSOE.h, revA"

#ifndef ShadowPetscSOE_h
#define ShadowPetscSOE_h

#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/petsc/PetscSOE.h>

// extern "C" {
#include <petsc.h>
// }

namespace XC {
class PetscSolver;

//! @ingroup SOE
//
//! @brief ??.
class ShadowPetscSOE: public LinearSOE
  {
  private:
    MPI_Comm theComm; // a comm for communicating to the ActorPetscSOE's
                      // without using PETSC_COMM_WORLD
    PetscSOE theSOE;  // the local portion of the SOE
    PetscSolver *theSolver; // created by the user
    int myRank;
    int numProcessors;
    mutable int sendData[3];
    void *sendBuffer;
    int blockSize;

    friend class SolutionStrategy;
    ShadowPetscSOE(SolutionStrategy *,PetscSolver &theSolver, int blockSize);
    SystemOfEqn *getCopy(void) const;
  public:
    ~ShadowPetscSOE(void);

    int solve(void);

    int getNumEqn(void) const;
    int setSize(Graph &theGraph);

    int addA(const Matrix &, const ID &, double fact = 1.0);
    int addB(const Vector &, const ID &, const double &fact= 1.0);
    int setB(const Vector &, const double &fact= 1.0);

    void zeroA(void);
    void zeroB(void);

    const Vector &getX(void) const;
    const Vector &getB(void) const;
    double normRHS(void) const;

    void setX(int loc, double value);
    void setX(const Vector &x);

    int setSolver(PetscSolver &);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };
inline SystemOfEqn *ShadowPetscSOE::getCopy(void) const
  { return new ShadowPetscSOE(*this); }
} // end of XC namespace


#endif







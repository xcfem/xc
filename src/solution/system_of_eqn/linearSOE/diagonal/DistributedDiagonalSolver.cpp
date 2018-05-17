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
                                                                        
// $Revision: 1.2 $
// $Date: 2005/06/20 21:35:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.cpp,v $

// Written: fmk 
// Created: 05/05
//
// What: "@(#) DistributedDiagonalSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE.h>
#include "utility/matrix/Vector.h"
#include <utility/actor/channel/Channel.h>

XC::DistributedDiagonalSolver::DistributedDiagonalSolver(int classTag)
:LinearSOESolver(classTag), minDiagTol(0.0), theSOE(nullptr) {}    

XC::DistributedDiagonalSolver::DistributedDiagonalSolver(double tol)
:LinearSOESolver(SOLVER_TAGS_DistributedDiagonalSolver), minDiagTol(tol), theSOE(nullptr) {}    

//! @brief Sets the system of equations to solve.
bool XC::DistributedDiagonalSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    DistributedDiagonalSOE *tmp= dynamic_cast<DistributedDiagonalSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << getClassName() << "::setLinearSOE: not a suitable system of equations" << std::endl;
    return retval;
  }

bool XC::DistributedDiagonalSolver::setLinearSOE(DistributedDiagonalSOE &theProfileSPDSOE)
  { return setLinearSOE(&theProfileSPDSOE); }

int XC::DistributedDiagonalSolver::setSize(void)
  { return 0; }


int XC::DistributedDiagonalSolver::solve(void)
  {
    int size = theSOE->size;
    int processID = theSOE->processID;

    ChannelQueue theChannels= theSOE->theChannels;
    int numChannels= theChannels.size();

    int numShared = theSOE->numShared;
    ID &myDOFs = theSOE->myDOFs;
    ID &myDOFsShared = theSOE->myDOFsShared;
  
    double *X = theSOE->getPtrX();
    double *B = theSOE->getPtrB();
    double *A = theSOE->A.getDataPtr();
    double *dataShared = theSOE->dataShared.getDataPtr();
    Vector *vectShared = &theSOE->vectShared;

    //
    // first copy A & B contributions to sharedData
    //

    for(int i=0; i<numShared; i++)
      dataShared[i] = 0.0;

    // assuming numShared < size .. could do an if statement

    for(int i=0; i<numShared; i++)
      {
        int dof = myDOFsShared(i);
        int loc = myDOFs.getLocation(dof);
        if(loc >= 0)
          {
            dataShared[i] = A[loc];
            dataShared[i+numShared] = B[loc];
          }
      }

  //
  // use P0 to gather & send back out
  //

  if (numShared != 0) {
    if (processID != 0) {
      Channel *theChannel = theChannels[0];
      theChannel->sendVector(0, 0, *vectShared);
      theChannel->recvVector(0, 0, *vectShared);
    } 
    else {

      static XC::Vector otherShared(1);
      otherShared.resize(2*numShared);
      for (int i=0; i<numChannels; i++) {
	Channel *theChannel = theChannels[i];
	theChannel->recvVector(0, 0, otherShared);
	*vectShared += otherShared;
      }
      for (int i=0; i<numChannels; i++) {
	Channel *theChannel = theChannels[i];
	theChannel->sendVector(0, 0, *vectShared);
      }
    }
  }
  
  
  //
  // set the corresponding A & B entries
  //
  
  
  for (int i=0; i<numShared; i++) {
    int dof = myDOFsShared(i);
    int loc = myDOFs.getLocation(dof);
    if (loc >= 0) {
      A[loc] = dataShared[i];
      B[loc] = dataShared[i+numShared];
    }
  }  

  //
  // now solve
  //
  
  for (int i=0; i<size; i++) {
    X[i] = B[i]/A[i];
  }

  return 0;
}

int XC::DistributedDiagonalSolver::sendSelf(CommParameters &cp)
  { return cp.sendDouble(minDiagTol,getDbTagData(),CommMetaData(0)); }


int XC::DistributedDiagonalSolver::recvSelf(const CommParameters &cp)
  { return cp.receiveDouble(minDiagTol,getDbTagData(),CommMetaData(0)); }



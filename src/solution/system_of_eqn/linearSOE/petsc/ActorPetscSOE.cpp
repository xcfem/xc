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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:02:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/ActorPetscSOE.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/petsc/ActorPetscSOE.C
//
// Written: fmk & om
// Created: 7/98
// Revision: A
//
// Description: This file contains the implementation for XC::BandGenLinSOE


#include "solution/system_of_eqn/linearSOE/petsc/ActorPetscSOE.h"
#include "solution/system_of_eqn/linearSOE/petsc/PetscSolver.h"
#include "solution/system_of_eqn/linearSOE/petsc/PetscSOE.h"
#include <utility/matrix/Matrix.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>

XC::ActorPetscSOE::ActorPetscSOE(SolutionStrategy *owr, PetscSolver &theSOESolver, int blockSize)
  : theSOE(nullptr), theSolver(&theSOESolver), myRank(0), recvBuffer(0)
  {
    MPI_Comm_rank(PETSC_COMM_WORLD, &myRank);
    MPI_Comm_size(PETSC_COMM_WORLD, &numProcessors);
    //MPI_Comm_dup(PETSC_COMM_WORLD, &theComm);
    if(myRank == 0)
      { std::cerr << " ActorPetscSOE::ActorPetscSOE - must be rank 0\n"; }
    recvBuffer= reinterpret_cast<void *>(&recvData[0]);
    MPI_Barrier(PETSC_COMM_WORLD);
    theSOE=new PetscSOE(owr,blockSize);
    theSOE->setSolver(theSolver);
  }


//! @brief Destructor.
XC::ActorPetscSOE::~ActorPetscSOE(void)
  {
    if(theSOE)
      delete theSOE;
  }


int XC::ActorPetscSOE::run(void)
  {
    int flag = 1;
    int tag, low, high, ierr, n, numRows;
    double *theData;
    MPI_Status status;
    void *buffer = 0;
  
    std::vector<int> onnz;
    std::vector<int> dnnz;
    while(flag != 0)
      {
        MPI_Bcast(recvBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);  
        flag = recvData[0];
        switch(flag)
          {

    case 0:
      MPI_Barrier(PETSC_COMM_WORLD);
      break;

    case 1:
      if(!theSOE)
	return -1;
      theSOE->factored = recvData[1];
      theSOE->solve();
      break;

    case 2:
      if(!theSOE)
	return -1;
      tag = 100;
      MPI_Recv(recvBuffer, 3, MPI_INT, 0, tag, PETSC_COMM_WORLD, &status);

      numRows = recvData[1];
      n = recvData[2];
      onnz.resize(numRows);
      dnnz.resize(numRows);
      buffer = reinterpret_cast<void *>(dnnz.data());
      tag = 101;
      MPI_Recv(buffer, numRows, MPI_INT, 0, tag, PETSC_COMM_WORLD, &status);

      buffer= reinterpret_cast<void *>(onnz.data());
      tag = 102;
      MPI_Recv(buffer, numRows, MPI_INT, 0, tag, PETSC_COMM_WORLD, &status);

      MPI_Bcast(recvBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);  
      //theSOE->setSizeParallel(numRows, n, dnz, dnnz, onz, onnz);	
      std::cerr << "ActorPetscSOE::" << __FUNCTION__
		<< "setSizeParallel commented out because it's not defined."
		<< std::endl;
      break;

    case 3:
      if(!theSOE)
	return -1;
      theSOE->zeroA();
      break;

    case 4:
      if(!theSOE)
	return -1;
      theSOE->zeroB();
      break;

    case 5:
      if(!theSOE)
	return -1;
      tag = 99;
      ierr = VecGetOwnershipRange(theSOE->x, &low, &high);
      // CHKERRA(ierr); # Doesn't compile LCPT 20211207
      recvData[0] = low; recvData[1] = high;
      MPI_Send(recvBuffer, 2, MPI_INT, 0, tag, PETSC_COMM_WORLD);
      ierr = VecGetArray(theSOE->x, &theData);
      // CHKERRA(ierr); # Doesn't compile LCPT 20211207      
      MPI_Send(theData, high-low, MPI_DOUBLE, 0, tag, PETSC_COMM_WORLD); 
      ierr = VecRestoreArray(theSOE->x, &theData);
      // CHKERRA(ierr); # Doesn't compile LCPT 20211207         
      break;

    case 6:
      if(!theSOE)
	return -1;
      // some work here

    default:
      std::cerr << "ActorPetscSOE::" << __FUNCTION__
	        << "; invalid action " << flag << " received\n";
    }
  }
  return 0;
}





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
                                                                        
// $Revision: 1.7 $
// $Date: 2008-04-01 00:35:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/mumps/MumpsParallelSolver.cpp,v $

// Written: fmk 
// Created: 02/06
                                                                        
// Description: This file contains the implementation of MumpsParallelSolver

#include "solution/system_of_eqn/linearSOE/mumps/MumpsParallelSOE.h"
#include "solution/system_of_eqn/linearSOE/mumps/MumpsParallelSolver.h"

#define ICNTL(I) icntl[(I)-1] /* macro s.t. indices match documentation */

#include <mpi/mpi.h>

XC::MumpsParallelSolver::MumpsParallelSolver(int ICNTL7, int ICNTL14)
  :LinearSOESolver(SOLVER_TAGS_MumpsParallelSolver),
   theMumpsSOE(nullptr), rank(0), np(0)
  {
    memset(&id, 0, sizeof(id));
    icntl7 = ICNTL7;
    icntl14 = ICNTL14;
    init = false;
    needsSetSize = false;
  }

XC::MumpsParallelSolver::MumpsParallelSolver(int mpi_comm, int ICNTL7, int ICNTL14)
  :LinearSOESolver(SOLVER_TAGS_MumpsParallelSolver),
   theMumpsSOE(nullptr), rank(0), np(0)
  {
    memset(&id, 0, sizeof(id));

    icntl14 = ICNTL14;
    icntl7 = ICNTL7;
    init = false;
    needsSetSize = false;
  }
XC::LinearSOESolver *XC::MumpsParallelSolver::getCopy(void) const
  { return new MumpsParallelSolver(*this); }

XC::MumpsParallelSolver::~MumpsParallelSolver()
  {
    id.job=-2; 
    if (init == true)
      dmumps_c(&id); /* Terminate instance */
  }

int XC::MumpsParallelSolver::initializeMumps(void)
  {
    if (needsSetSize == false)	{
      return 0;
    }
    else {

      if (init == true) {
	id.job=-2; 
	dmumps_c(&id); /* Terminate instance */
	init = false;
      } 

      id.job = -1;

      id.par = 1; // host involved in calcs
      id.sym = theMumpsSOE->matType;

      id.comm_fortran = 0; // OPENMPI

      id.ICNTL(5) = 0; id.ICNTL(18) = 3;

      dmumps_c(&id);

      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_size(MPI_COMM_WORLD, &np);

      init = true;

      // parallel solver; distributed i/p matrix A
      id.ICNTL(5) = 0; id.ICNTL(18) = 3;

      // No outputs 
      //  id.ICNTL(1)=-1; id.ICNTL(2)=-1; id.ICNTL(3)=-1; id.ICNTL(4)=0; 
      id.ICNTL(1) = -1; id.ICNTL(2) = -1; id.ICNTL(3) = -1; id.ICNTL(4) = 3;
      //id.ICNTL(1) = 1; id.ICNTL(2) = 1; id.ICNTL(3) = 1; id.ICNTL(4) = 3;

      id.ICNTL(14) = icntl14;
      id.ICNTL(7) = icntl7;

      int nnz = theMumpsSOE->nnz;
      int *colA = theMumpsSOE->colA.getDataPtr();
      int *rowA = theMumpsSOE->rowA.getDataPtr();

      // increment row and col A values by 1 for mumps fortran indexing
      for (int i = 0; i < nnz; i++) {
	rowA[i]++;
	colA[i]++;
      }

      // analyze the matrix
      id.n = theMumpsSOE->size;
      id.nz_loc = theMumpsSOE->nnz;
      id.irn_loc = theMumpsSOE->rowA.getDataPtr();
      id.jcn_loc = theMumpsSOE->colA.getDataPtr();
      id.a_loc = theMumpsSOE->A.getDataPtr();

      // Call the MUMPS package to analyze the system
      id.job = 1;
      dmumps_c(&id);

      // decrement row and col A values by 1 to return to C++ indexing
      for (int i = 0; i < nnz; i++) {
	rowA[i]--;
	colA[i]--;
      }

      int info = id.infog[0];
      if (info != 0) {
	std::cerr << "WARNING MumpsParallelSolver::setSize(void)- ";
	std::cerr << " Error " << info << " returned in substitution dmumps()\n";
	return info;
      }

      needsSetSize = false;

      return info;
    }
  }

int XC::MumpsParallelSolver::solveAfterInitialization(void)
{
  int n = theMumpsSOE->size;
  int nnz = theMumpsSOE->nnz;
  int *rowA = theMumpsSOE->rowA.getDataPtr();
  int *colA = theMumpsSOE->colA.getDataPtr();
  double *A = theMumpsSOE->A.getDataPtr();
  double *X = theMumpsSOE->X.getDataPtr();
  double *B = theMumpsSOE->B.getDataPtr();

  // parallel solver; distributed i/p matrix A
  id.ICNTL(5)=0; id.ICNTL(18)=3; 

  // No outputs 
  id.ICNTL(1)=-1; id.ICNTL(2)=-1; id.ICNTL(3)=-1; id.ICNTL(4)=0;
  //id.ICNTL(1) = 1; id.ICNTL(2) = 1; id.ICNTL(3) = 1; id.ICNTL(4) = 3;


  id.ICNTL(14)=icntl14; 
  id.ICNTL(7)=icntl7; 
  
  // increment row and col A values by 1 for mumps fortran indexing
  for (int i=0; i<nnz; i++) {
    rowA[i]++;
    colA[i]++;
  }
  
  if (rank == 0) {
    id.n   = n; 
    for (int i=0; i<n; i++) {
      X[i] = B[i];
    }
    id.rhs = X;
  } 

  // factor the matrix
  id.nz_loc  = nnz; 
  id.irn_loc = rowA;
  id.jcn_loc = colA;
  id.a_loc   = A; 

  if (theMumpsSOE->factored == false) {

    // Call the MUMPS package to factor & solve the system
    id.job = 5;
    dmumps_c(&id);
    theMumpsSOE->factored = true;

  } else {

    // Call the MUMPS package to solve the system
    id.job = 3;
    dmumps_c(&id);
  }	

  int info = id.infog[0];
  int info2   = id.infog[1];
  if (info != 0) {	
    std::cerr << "WARNING MumpsParallelSolver::solve(void)- ";
    std::cerr << " Error " << info << " returned in substitution dmumps()\n";
	switch(info) {
	  case -2:
		std::cerr << "nz " << info2 << " out of range\n";
	    break;
	  case -5:
		std::cerr << " out of memory allocation error\n";
		break;
      case -6:  
		std::cerr << " cause: Matrix is Singular in Structure: check your model\n";
		break;
	  case -7:
		std::cerr << " out of memory allocation error\n";
		break;
	  case -8:
		std::cerr << "Work array too small; use -ICNTL14 option, the default is -ICNTL 20 make 20 larger\n";
		break;
	  case -9:
		std::cerr << "Work array too small; use -ICNTL14 option, the default is -ICNTL 20 make 20 larger\n";
		break;
	  case -10:  
		std::cerr << " cause: Matrix is Singular Numerically\n";
		break;
	  case -13:
		  std::cerr << " out of memory wanted " << info2 << " (if < 0 mult absolute by 1 million)\n";
		  break;
	  default:
		  ;
	}
    return info;
  }

  // decrement row and col A values by 1 to return to C++ indexing
  for (int i=0; i<nnz; i++) {
    rowA[i]--;
    colA[i]--;
  }

  return 0;

}

int XC::MumpsParallelSolver::solve(void)
  {
    int initializationResult = initializeMumps();

    if(initializationResult == 0)
      return solveAfterInitialization();
    else
      return initializationResult;
  }



int XC::MumpsParallelSolver::setSize()
  {
    needsSetSize = true;
     return 0;
  }

bool XC::MumpsParallelSolver::setLinearSOE(LinearSOE *theSOE)
  {
    MumpsParallelSOE *tmp= dynamic_cast<MumpsParallelSOE *>(theSOE);
    if(tmp)
      theMumpsSOE= tmp;
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; not a suitable system of equations object."
	        << std::endl;
    return 0;
  }


//! @brief Send object members through the communicator argument.
int XC::MumpsParallelSolver::sendData(Communicator &comm)
  {
    int res=comm.sendInts(icntl7, icntl14, getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::MumpsParallelSolver::recvData(const Communicator &comm)
  {
    int res= comm.receiveInts(icntl7, icntl14, getDbTagData(),CommMetaData(0));
    return res;
  }


int XC::MumpsParallelSolver::sendSelf(Communicator &comm)
  {
    inicComm(3); 
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
              << "; communicator failed to send data.\n";
    return res;
  }

int XC::MumpsParallelSolver::recvSelf(const Communicator &comm)  
  {
    // first we get the data about the state of the mesh for this cTag
    inicComm(3);
    int res= comm.receiveIdData(getDbTagData(),getDbTag());
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
              << "; communicator failed to recv the initial ID.\n";
    else
      res+= recvData(comm);
    return res;
  }










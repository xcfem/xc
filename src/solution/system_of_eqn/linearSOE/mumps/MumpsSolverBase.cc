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

#include "solution/system_of_eqn/linearSOE/mumps/MumpsSolverBase.h"

#define ICNTL(I) icntl[(I)-1] /* macro s.t. indices match documentation */

#include <mpi/mpi.h>

XC::MumpsSolverBase::MumpsSolverBase(int classTag, int ICNTL7, int ICNTL14)
  : LinearSOESolver(classTag)
  {
    memset(&id, 0, sizeof(id));
    icntl7 = ICNTL7;
    icntl14 = ICNTL14;
    init = false;
    needsSetSize = false;
  }

XC::MumpsSolverBase::~MumpsSolverBase(void)
  {
    id.job=-2; 
    if(init == true)
        dmumps_c(&id); /* Terminate instance */
  }

//! @brief Returns true if MPI is initialized.
bool XC::MumpsSolverBase::MPIInitialized(void)
  {
    bool retval= false;
    int mpi_initialized;

    int mpi_retval= MPI_Initialized(&mpi_initialized);
    if(mpi_retval!=MPI_SUCCESS)
      std::cerr << __FUNCTION__ << "; error: " << retval << std::endl;
    if(mpi_initialized)
      retval= true;
    return retval;
  }

//! @brief Initialize message passing interface.
int XC::MumpsSolverBase::initializeMPI(void)
  {
    int retval= 0;
    if(!MPIInitialized())
      {
	int argc= 0; char **argv= nullptr;
	retval= MPI_Init(&argc, &argv);
	if(retval!=MPI_SUCCESS)
	  std::cerr << __FUNCTION__ << "; error: " << retval << std::endl;
      }
    return retval;
  }

//! @brief Finalize message passing interface.
int XC::MumpsSolverBase::finalizeMPI(void)
  {
    int retval= 0;
    if(MPIInitialized())
      {
        retval= MPI_Finalize();
	if(retval!=MPI_SUCCESS)
	  std::cerr << __FUNCTION__ << "; error: " << retval << std::endl;
      }
    return retval;
  }

int XC::MumpsSolverBase::solve(void)
  {
    int retval= initializeMPI();
    if(retval==MPI_SUCCESS)
      retval= initializeMumps();
    if(retval == 0)
      retval= solveAfterInitialization();
    return retval;
  }


int XC::MumpsSolverBase::setSize()
  {
    needsSetSize = true;
     return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::MumpsSolverBase::sendData(Communicator &comm)
  {
    int res=comm.sendInts(icntl7, icntl14, getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::MumpsSolverBase::recvData(const Communicator &comm)
  {
    int res= comm.receiveInts(icntl7, icntl14, getDbTagData(),CommMetaData(0));
    return res;
  }

int XC::MumpsSolverBase::sendSelf(Communicator &comm)
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

int XC::MumpsSolverBase::recvSelf(const Communicator &comm)  
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






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

#include "solution/system_of_eqn/linearSOE/mumps/MumpsSolverBase.h"

#define ICNTL(I) icntl[(I)-1] /* macro s.t. indices match documentation */

#include <mpi/mpi.h>
#include <sstream>      // std::ostringstream

XC::MumpsSolverBase::MumpsSolverBase(int classTag, int ICNTL7, int ICNTL14)
  : LinearSOESolver(classTag)
  {
    memset(&id, 0, sizeof(id));
    icntl7 = ICNTL7;
    icntl14 = ICNTL14;
    mumps_init= false;
    needsSetSize = false;
  }

XC::MumpsSolverBase::~MumpsSolverBase(void)
  { terminateMumps(); }

//! @brief Terminate MUMPS.
int XC::MumpsSolverBase::terminateMumps(void)
  {
    if(mumps_init == true)
      {
        id.job=-2; 
        dmumps_c(&id); /* Terminate instance */
	mumps_init= false;
      }
    return 0;
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

//! @brief Return a message explaining the mumps error (if any).
std::string XC::MumpsSolverBase::getMUMPSErrorMessage(void)
  {
    std::string retval;
    const int info= id.infog[0];
    std::ostringstream oss;
    if(info != 0)
      {	
        const int info2= id.infog[1];
	switch(info)
	  {
          case -2:
	    oss << "nz " << info2 << " out of range";
	    break;
	  case -5:
	    oss << " out of memory allocation error";
	    break;
	  case -6:  
	    oss << " cause: Matrix is Singular in Structure: check your model";
	    break;
	  case -7:
	    oss << " out of memory allocation error";
	    break;
	  case -8:
	    oss << "Work array too small; use -ICNTL14 option, the default is -ICNTL 20 make 20 larger";
	    break;
	  case -9:
	    oss << "Work array too small; use -ICNTL14 option, the default is -ICNTL 20 make 20 larger";
	    break;
	  case -10:  
	    oss << " cause: Matrix is Singular Numerically";
	    break;
          case -13:
	    oss << " out of memory wanted " << info2 << " (if < 0 mult absolute by 1 million)";
	    break;
	  default:
	    oss << " error not catched in switch."  ;
	  }
	retval= oss.str();
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
                << "; communicator failed to send data."
		<< std::endl;
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






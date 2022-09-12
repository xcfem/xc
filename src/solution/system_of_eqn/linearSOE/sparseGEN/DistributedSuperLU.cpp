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
// $Date: 2005/12/06 22:21:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/DistributedSuperLU.cpp,v $
                                                                        
                                                                        
// Written: fmk 
//
// Description: This file contains the implementation of DistributedSuperLU
//
// What: "@(#) DistributedSuperLU.h, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSuperLU.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/matrix/ID.h>

//! @brief Constructor.
XC::DistributedSuperLU::DistributedSuperLU(int npR, int npC)
  :SparseGenColLinSolver(SOLVER_TAGS_DistributedSuperLU),
   gridInit(false), npRow(npR), npCol(npC) {}

//! @brief Default constructor.
XC::DistributedSuperLU::DistributedSuperLU(void)
  :SparseGenColLinSolver(SOLVER_TAGS_DistributedSuperLU), 
   gridInit(false), npRow(0), npCol(0){}


XC::DistributedSuperLU::~DistributedSuperLU(void)
  {
    //Destroy_LU(theSOE->getNumEqn(), &grid, &LUstruct); 
    ScalePermstructFree(&ScalePermstruct);
    LUstructFree(&LUstruct); 

    //superlu_gridexit(&grid);

    //  MPI_Comm_free(&comm_SuperLU);
  }

int XC::DistributedSuperLU::solve(void)
  {
  if (theSOE == 0) {
    std::cerr << "WARNING XC::DistributedSuperLU::solve(void)- ";
    std::cerr << " No XC::LinearSOE object has been set\n";
    return -1;
  }

  // check for quick return
  if (theSOE->getNumEqn() == 0)
    return 0;

  // if subprocess recv A & B from p0
  if (processID != 0)
    {
      Channel *theChannel = theChannels[0];
      theChannel->recvVector(0, 0, theSOE->getB());
      //Vector vectA(&theSOE->getA(), theSOE->getNNZ());    
      theChannel->recvVector(0, 0, theSOE->getA());
    } 

  //
  // if XC::main process, send B & A to all, solve and send back X, B & result


  else
    {
      //Vector vectA(theSOE->getA(), theSOE->nnz);    

      // send B & A to p1 through n-1
      const int numChannels= getNumChannels();
      for (int j=0; j<numChannels; j++)
	{
	  Channel *theChannel = theChannels[j];
	  theChannel->sendVector(0, 0, theSOE->getB());
	  theChannel->sendVector(0, 0, theSOE->getA());
	}
    }

  int info;

  int iam = grid.iam;

  if (iam < (npRow * npCol))
    {
    int n = theSOE->getNumEqn();
    int nnz = theSOE->getNNZ();
    int ldb = n;
    int nrhs = 1;
    static double berr[1];

    // first copy B into X
    double *Xptr = theSOE->getPtrX();
    double *Bptr = theSOE->getPtrB();

    for (int i=0; i<n; i++)
      *(Xptr++) = *(Bptr++);
    Xptr= theSOE->getPtrX();

    //
    // set the Fact options:
    //   leave DOFACT if never been factored
    //   otherwise use SamePattern if factored at least once
    //

    if((options.Fact == FACTORED) && (theSOE->getFactored() == false))
      {
        options.Fact = SamePattern;
        for (int i=0; i<nnz; i++) rowA(i) = theSOE->rowA[i];
      }

    //
    // solve & mark as factored
    //

    pdgssvx_ABglobal(&options, &A, &ScalePermstruct, Xptr, ldb, nrhs, &grid,
		     &LUstruct, berr, &stat, &info);

    if(theSOE->getFactored() == false)
      {
        options.Fact = FACTORED;      
        theSOE->setFactored(true);
      }
  }

  /*
  // synch processes again
  if (processID != 0) {
    static XC::ID idData(1); 
    idData(0) = info;
    Channel *theChannel = theChannels[0];
    theChannel->sendID(0, 0, idData);
  } 
  else {
    for (int j=0; j<numChannels; j++) {
      static XC::ID idData(1);
      Channel *theChannel = theChannels[j];
      theChannel->recvID(0, 0, idData);
    }
  }
  */

  return 0;
}

int XC::DistributedSuperLU::setSize()
  {
    int n = theSOE->getNumEqn();

  //
  // init the super lu process grid
  //
  if (gridInit == false) {

    MPI_Comm  comm_world;  //
    MPI_Group group_world; // group_worker;

    comm_world = MPI_COMM_WORLD;
    MPI_Comm_group(comm_world, &group_world);
    //    MPI_Group_excl(group_world, 1, 0, &group_worker);  /* process 0 not member */

    MPI_Comm_create(comm_world, group_world, &comm_SuperLU);

    superlu_gridinit(comm_SuperLU, npRow, npCol, &grid);

  // free old structures if resize already called
  } else {
    Destroy_LU(theSOE->getNumEqn(), &grid, &LUstruct); 
    ScalePermstructFree(&ScalePermstruct);
    LUstructFree(&LUstruct); 
  }
  
  //
  // Initialize the statistics variables.
  //
  PStatInit(&stat);
  
  //
  // Create compressed column matrix for A. 
  //
			      
  if(n > 0)
    {
      // create the SuperMatrix A	
      const int nnz = theSOE->getNNZ();
      rowA= theSOE->rowA;

      dCreate_CompCol_Matrix_dist(&A, n, n, nnz, theSOE->getA().getDataPtr(),rowA.getDataPtr(), theSOE->colStartA.getDataPtr(),SLU_NC, SLU_D, SLU_GE);

      //
      // Initialize ScalePermstruct and LUstruct.
      //
      ScalePermstructInit(n, n, &ScalePermstruct);
      LUstructInit(n, &LUstruct);
    }  
			      
    //
    //  set default options
    //
    set_default_options_dist(&options);    

    //
    // Initialize the statistics variables. 
    //
    PStatInit(&stat);
    return 0;
  }

//! @brief Returns a vector to store the dbTags
//! de los miembros of the clase.
XC::DbTagData &XC::DistributedSuperLU::getDbTagData(void) const
  {
    static DbTagData retval(3);
    return retval;
  }

int XC::DistributedSuperLU::sendSelf(Communicator &comm)
  {
    int sendID =0;

    // if P0 check if already sent. If already sent use old processID; if not allocate a new_ process 
    // id for remote part of object, enlarge channel * to hold a channel * for this remote object.

    // if not P0, send current processID

    if(processID == 0)
      {
	// check if already using this object
	bool found = false;
	Channel *theChannel= comm.getChannel();
	int numChannels= getNumChannels();
	for(int i=0; i<numChannels; i++)
	  if(theChannels[i] == theChannel)
	    {
	      sendID = i+1;
	      found = true;
	    }

	// if new_ object, enlarge Channel pointers to hold new_ channel * & allocate new_ ID
	if(found == false)
	  {
	    int nextNumChannels= numChannels + 1;
	    theChannels.resize(nextNumChannels,theChannel);

	    numChannels = nextNumChannels;

	    // allocate new_ processID for remote object
	    sendID = numChannels;
	  }
       }
     else 
       sendID = processID;

     static ID idData(3);
     idData(0) = sendID;
     idData(1) = npRow;
     idData(2) = npCol;
     int res= comm.sendInts(sendID, npRow, npCol,getDbTagData(),CommMetaData(0));
     if(res < 0)
       {
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; WARNING failed to send data\n";
	  return -1;
	}	      
    return 0;
  }

int XC::DistributedSuperLU::recvSelf(const Communicator &comm)
  {
    static ID idData(3);

    int res = comm.receiveInts(processID, npRow, npCol,getDbTagData(),CommMetaData(0));
    if(res < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed to receive data." << std::endl;
        return -1;
      }	      

    Channel *theChannel= const_cast<Channel *>(comm.getChannel());
    setChannel(theChannel);
    return 0;
  }












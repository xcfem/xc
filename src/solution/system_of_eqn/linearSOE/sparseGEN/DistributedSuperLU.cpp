//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
// Description: This file contains the implementation of XC::DistributedSuperLU
//
// What: "@(#) DistributedSuperLU.h, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSuperLU.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/matrix/ID.h>

XC::DistributedSuperLU::DistributedSuperLU(int npR, int npC)
  :SparseGenColLinSolver(SOLVER_TAGS_DistributedSuperLU), 
   gridInit(false), npRow(npR), npCol(npC),
   processID(0), numChannels(0), theChannels(0){}


XC::DistributedSuperLU::DistributedSuperLU()
  :SparseGenColLinSolver(SOLVER_TAGS_DistributedSuperLU), 
   gridInit(false), npRow(0), npCol(0),
   processID(0), numChannels(0), theChannels(0){}


XC::DistributedSuperLU::~DistributedSuperLU(void)
  {
    //Destroy_LU(theSOE->size, &grid, &LUstruct); 
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

  // check for XC::quick return
  if (theSOE->size == 0)
    return 0;

  // if subprocess recv A & B from p0
  if (processID != 0) {
    Channel *theChannel = theChannels[0];
    theChannel->recvVector(0, 0, (*theSOE->vectB));
    Vector vectA(theSOE->A, theSOE->nnz);    
    theChannel->recvVector(0, 0, vectA);
  } 

  //
  // if XC::main process, send B & A to all, solve and send back X, B & result


  else {

    Vector vectA(theSOE->A, theSOE->nnz);    

    // send B & A to p1 through n-1
    for (int j=0; j<numChannels; j++) {
      Channel *theChannel = theChannels[j];
      theChannel->sendVector(0, 0, *(theSOE->vectB));
      theChannel->sendVector(0, 0, vectA);
    }
  }

  int info;

  int iam = grid.iam;

  if (iam < (npRow * npCol)) {
    int n = theSOE->size;
    int nnz = theSOE->nnz;
    int ldb = n;
    int nrhs = 1;
    static double berr[1];

    // first copy B into X
    double *Xptr = theSOE->X;
    double *Bptr = theSOE->B;

    for (int i=0; i<n; i++)
      *(Xptr++) = *(Bptr++);
    Xptr = theSOE->X;

    //
    // set the Fact options:
    //   leave DOFACT if never been factored
    //   otherwise use SamePattern if factored at least once
    //

    if((options.Fact == FACTORED) && (theSOE->factored == false))
      {
        options.Fact = SamePattern;
        for (int i=0; i<nnz; i++) rowA(i) = theSOE->rowA[i];
      }

    //
    // solve & mark as factored
    //

    pdgssvx_ABglobal(&options, &A, &ScalePermstruct, Xptr, ldb, nrhs, &grid,
		     &LUstruct, berr, &stat, &info);

    if (theSOE->factored == false) {
      options.Fact = FACTORED;      
      theSOE->factored = true;
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
    int n = theSOE->size;

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
    Destroy_LU(theSOE->size, &grid, &LUstruct); 
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
      int nnz = theSOE->nnz;
      rowA.resize(nnz);
      for(int i=0; i<nnz; i++)
        rowA(i)= theSOE->rowA[i];

      dCreate_CompCol_Matrix_dist(&A, n, n, nnz, theSOE->A,rowA.getDataPtr(), theSOE->colStartA,SLU_NC, SLU_D, SLU_GE);

      //
      // Initialize ScalePermstruct and LUstruct.
      //
      ScalePermstructInit(n, n, &ScalePermstruct);
      LUstructInit(n, n, &LUstruct);
    }  
			      
    //
    //  set default options
    //
    set_default_options_Distributed(&options);    

    //
    // Initialize the statistics variables. 
    //
    PStatInit(&stat);
    return 0;
  }


int XC::DistributedSuperLU::setProcessID(int dTag) 
  {
    processID = dTag;
    return 0;
  }

int XC::DistributedSuperLU::setChannels(int nChannels, Channel **theC)
  {
    numChannels = nChannels;
    theChannels= std::vector<Channel *>(numChannels, nullptr);

    for(int i=0; i<numChannels; i++)
      theChannels[i] = theC[i];
    return 0;
  }

int
XC::DistributedSuperLU::sendSelf(CommParameters &cp)
{
  int sendID =0;

  // if P0 check if already sent. If already sent use old processID; if not allocate a new_ process 
  // id for remote part of object, enlarge channel * to hold a channel * for this remote object.

  // if not P0, send current processID

  if(processID == 0)
    {
      // check if already using this object
      bool found = false;
      for(int i=0; i<numChannels; i++)
        if(theChannels[i] == &theChannel)
          {
	    sendID = i+1;
	    found = true;
          }

      // if new_ object, enlarge Channel pointers to hold new_ channel * & allocate new_ ID
      if(found == false)
        {
          int nextNumChannels = numChannels + 1;
          theChannels.resize(nextNumChannels,nullptr);

          theChannels[numChannels] = &theChannel;    
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
    int res = cp.sendIdData(0);
   if (res < 0) {
    std::cerr <<"WARNING XC::DistributedSuperLU::sendSelf() - failed to send data\n";
    return -1;
  }	      
  return 0;
}

int XC::DistributedSuperLU::recvSelf(const CommParameters &cp)
  {
    static ID idData(3);

    int res = cp.receiveIdData(0);
    if(res < 0)
      {
        std::cerr <<"WARNING XC::DistributedSuperLU::recvSelf() - failed to receive data\n";
        return -1;
      }	      

    processID = idData(0);
    npRow = idData(1);
    npCol = idData(2);

    numChannels = 1;
    theChannels= std::vector<Channel *>(1,nullptr);
    theChannels[0]= &theChannel;
    return 0;
  }












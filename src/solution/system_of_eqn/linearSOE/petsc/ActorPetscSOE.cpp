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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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

XC::ActorPetscSOE::ActorPetscSOE(PetscSolver &theSOESolver, int blockSize)
  : theSOE(nullptr), theSolver(&theSOESolver), myRank(0), recvBuffer(0)
  {
    MPI_Comm_rank(PETSC_COMM_WORLD, &myRank);
    MPI_Comm_size(PETSC_COMM_WORLD, &numProcessors);
    //MPI_Comm_dup(PETSC_COMM_WORLD, &theComm);
    if(myRank == 0)
      { std::cerr << " ActorPetscSOE::ActorPetscSOE - must be rank 0\n"; }
    recvBuffer= reinterpret_cast<void *>(&recvData[0]);
    MPI_Barrier(PETSC_COMM_WORLD);
    theSOE=new PetscSOE(theSolver, blockSize);
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
    int dnz = 0;
    int onz = 0;
    MPI_Status status;
    void *buffer = 0;
  
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
      int onnz[numRows];
      int dnnz[numRows];
      buffer = reinterpret_cast<void *>(dnnz);
      tag = 101;
      MPI_Recv(buffer, numRows, MPI_INT, 0, tag, PETSC_COMM_WORLD, &status);

      buffer= reinterpret_cast<void *>(onnz);
      tag = 102;
      MPI_Recv(buffer, numRows, MPI_INT, 0, tag, PETSC_COMM_WORLD, &status);

      MPI_Bcast(recvBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);  
      theSOE->setSizeParallel(numRows, n, dnz, dnnz, onz, onnz);	
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
      ierr = VecGetOwnershipRange(theSOE->x, &low, &high); CHKERRA(ierr);
      recvData[0] = low; recvData[1] = high;
      MPI_Send(recvBuffer, 2, MPI_INT, 0, tag, PETSC_COMM_WORLD);
      ierr = VecGetArray(theSOE->x, &theData); CHKERRA(ierr);       
      MPI_Send(theData, high-low, MPI_DOUBLE, 0, tag, PETSC_COMM_WORLD); 
      ierr = VecRestoreArray(theSOE->x, &theData); CHKERRA(ierr);             
      break;

    case 6:
      if(!theSOE)
	return -1;
      // some work here

    default:
      std::cerr << "XC::ActorPetscSOE::invalid action " << flag << " received\n";
    }
  }
  return 0;
}





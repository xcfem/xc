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
                                                                        
// $Revision: 1.5 $
// $Date: 2006/01/13 00:00:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/PetscSOE.cpp,v $
                                                                        
// Written: fmk & om
// Created: 7/98
// Revision: A
//
// Description: This file contains the implementation for PetscSOE


#include "solution/system_of_eqn/linearSOE/petsc/PetscSOE.h"
#include "solution/system_of_eqn/linearSOE/petsc/PetscSolver.h"
#include "petsc/petscvec.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

XC::PetscSOE::PetscSOE(AnalysisAggregation *owr,int bs)
  :FactoredSOEBase(owr,LinSOE_TAGS_PetscSOE),
   numProcesses(0), A(0), x(0), b(0), blockSize(bs) {}

void XC::PetscSOE::InvokePetscDestructors(void)
  {
    // invoke the petsc destructors
#if (PETSC_VERSION_MAJOR == 3) && (PETSC_VERSION_MINOR >= 2)
    if(A) MatDestroy(&A);
    if(b) VecDestroy(&b);
    if(x) VecDestroy(&x);
#else
    if(A) MatDestroy(A);
    if(b) VecDestroy(b);
    if(x) VecDestroy(x);
#endif


  }

//! @brief Destructor.
XC::PetscSOE::~PetscSOE(void)
  { InvokePetscDestructors(); }



int XC::PetscSOE::setSize(Graph &theGraph)
  {
    PetscInitialize(0, PETSC_NULL, (char *)0, PETSC_NULL);
    MPI_Comm_size(PETSC_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(PETSC_COMM_WORLD, &processID);
  
    int result = 0;
    PetscErrorCode ierr= 0;

    // 
    // first determine system size
    //

    if(numProcesses == 1) {
      
      // if single process, the system size is size of graph
      size = theGraph.getNumVertex();
      factored= false;

    } else {

      // first determine local max
      size = 0;
      factored= false;
      VertexIter &theVertices = theGraph.getVertices();
      Vertex *theVertex;
      while ((theVertex = theVertices()) != 0) {
	int vertexTag = theVertex->getTag();
	if (vertexTag > size) 
	  size = vertexTag;
      }

      static XC::ID data(1);

      // all local max's sent to P0 which determines the max
      // and informs all others

      if (processID != 0) {
	Channel *theChannel = theChannels[0];
	
	data(0) = size;
	theChannel->sendID(0, 0, data);
	theChannel->recvID(0, 0, data);
	
	size = data(0);
      } else {

    const int numChannels= theChannels.size();
	for(int j=0; j<numChannels; j++) {
	  Channel *theChannel = theChannels[j];
	  theChannel->recvID(0, 0, data);
	  if (data(0) > size)
	    size = data(0);
	}
	data(0) = size;
	for (int j=0; j<numChannels; j++) {
	  Channel *theChannel = theChannels[j];
	  theChannel->sendID(0, 0, data);
	}
      }
      size = size+1; // vertices numbered 0 through n-1
    }

    InvokePetscDestructors();
    
    //
    // now we create the opensees vector objects
    //

        inic(size);
    
    
    // 
    // now create petsc matrix and vector objects
    //

    if(numProcesses == 1)
      {
    
        // we determine the number of non-zeros & number of nonzero's
        // in each row of A
        ID rowA(size);  // will contain no of non-zero entries
        // in each row
      
        int NNZ = 0;
        for(int a=0; a<size; a++)
          {
            Vertex *theVertex = theGraph.getVertexPtr(a);
	if (theVertex == 0) {
	  std::cerr << "WARNING:XC::PetscSOE::setSize :";
	  std::cerr << " vertex " << a << " not in graph! - size set to 0\n";
	  size = 0;
	  return -1;
	}
	
	const std::set<int> &theAdjacency = theVertex->getAdjacency();
	int idSize = theAdjacency.size();
	
	NNZ += idSize +1;
	rowA(a)= idSize +1;  // +1 for the diagonal entry
      }

      // 
      // Call Petsc VecCreate & MatCreate; NOTE: using previously allocated storage for vectors
      //      

      //      ierr = PetscOptionsGetInt(PETSC_NULL, "-n", &size, &flg); CHKERRQ(ierr);
      
      if (blockSize == 1) {
	ierr = MatCreateSeqAIJ(PETSC_COMM_SELF, size, size, 0, rowA.getDataPtr(), &A); CHKERRQ(ierr);
      } else {
	ierr = MatCreateSeqBAIJ(PETSC_COMM_SELF, blockSize, size,size, 0, rowA.getDataPtr(), &A); CHKERRQ(ierr);
      }
      
      ierr= VecCreateSeqWithArray(PETSC_COMM_WORLD, size, size, getPtrX(), &x); CHKERRQ(ierr); //LCPT
      ierr= VecCreateSeqWithArray(PETSC_COMM_WORLD, size, size, getPtrB(), &b); CHKERRQ(ierr); //LCPT

      // invoke setSize() on the XC::Solver
      LinearSOESolver *tSolver = this->getSolver();
      int solverOK = tSolver->setSize();
      if (solverOK < 0) {
	std::cerr << "WARNING:XC::PetscSOE::setSize :";
	std::cerr << " solver failed setSize()\n";
	return solverOK;
      }    
      

    } else {

      // 
      // Call Petsc VecCreate & MatCreate; NOTE: using previously allocated storage for vectors
      // 
      //

      ierr= PetscOptionsGetInt(PETSC_NULL, "-n", &size, &flg); CHKERRQ(ierr);
      ierr= MatCreate(PETSC_COMM_WORLD, &A); CHKERRQ(ierr);
      //ierr= MatCreate(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE,size, size, ); CHKERRQ(ierr);
      ierr= MatSetFromOptions(A);CHKERRQ(ierr);
      ierr= MatGetOwnershipRange(A, &startRow, &endRow);CHKERRQ(ierr);

      ierr= VecCreateMPIWithArray(PETSC_COMM_WORLD, endRow-startRow, size, size, &getX(startRow),  &x); CHKERRQ(ierr);  //LCPT
      ierr= VecCreateMPIWithArray(PETSC_COMM_WORLD, endRow-startRow, size, size, &getB(startRow),  &b); CHKERRQ(ierr);  //LCPT

      // invoke setSize() on the XC::Solver
      LinearSOESolver *tSolver = this->getSolver();
      int solverOK = tSolver->setSize();
      if (solverOK < 0) {
	std::cerr << "WARNING:XC::PetscSOE::setSize :";
	std::cerr << " solver failed setSize()\n";
	return solverOK;
      }    
    }

    return result;    
}


int XC::PetscSOE::addA(const XC::Matrix &m, const XC::ID &id, double fact)
  {
    factored= false;

    // check for a quick return 
    if(fact == 0.0)
      return 0;

    
    // check that m and id are of similar size
    int idSize = id.Size();    
    if(idSize != m.noRows() && idSize != m.noCols())
      {
	std::cerr << "XC::PetscSOE::addA() - Matrix and XC::ID not of similar sizes\n";
	return -1;
      }
    
    int n = id.Size();
    int row;
    int col;
    double value;
    for (int i=0; i<n; i++) {
      row = id(i);
      if (row >= 0) {
	for (int j=0; j<n; j++) {
	  col = id(j);
	  if (col >= 0) {
	    value = m(i,j)*fact;
	    int ierr = MatSetValues(A,1,&row,1,&col,&value,ADD_VALUES); 
	    if (ierr) std::cerr << processID << " " << row << " " << col << std::endl; 
	    CHKERRQ(ierr); 
	  }
	}
      }
    }

    return 0;
}

    
void XC::PetscSOE::zeroA(void)
  {
    factored= false;
    MatZeroEntries(A);
  }

//! @brief Asigna el solver que se empleará para resolver el sistema.	
bool XC::PetscSOE::setSolver(PetscSolver *newSolver)
  {    
    if(size != 0)
      {
	bool solverOK= (newSolver->setSize()>=0);
	if(!solverOK)
          {
	    std::cerr << getClassName() << "::"" __FUNCTION__"
	              << "; the new solver can't change size, "
	              << " keeping previous size.\n";
	    return solverOK;
	  }
      }	
    return FactoredSOEBase::setSolver(newSolver);
  }


int XC::PetscSOE::sendSelf(CommParameters &cp)
  { return send(cp); }


int XC::PetscSOE::recvSelf(const CommParameters &cp)
  { return receive(cp); }

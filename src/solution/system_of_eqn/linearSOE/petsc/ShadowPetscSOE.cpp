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
// $Date: 2003/02/14 23:02:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/ShadowPetscSOE.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/petsc/ShadowPetscSOE.C
//
// Written: fmk & om
// Created: 7/98
// Revision: A
//
// Description: This file contains the implementation for XC::BandGenLinSOE

#include <cstring>
#include "solution/system_of_eqn/linearSOE/petsc/ShadowPetscSOE.h"
#include "solution/system_of_eqn/linearSOE/petsc/PetscSolver.h"
#include <utility/matrix/Matrix.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>

XC::ShadowPetscSOE::ShadowPetscSOE(PetscSolver &theSOESolver, int bs)
  :LinearSOE(owr,LinSOE_TAGS_ShadowPetscSOE, &theSOESolver),
 theSOE(theSOESolver, bs), theSolver(&theSOESolver), myRank(0),
 sendBuffer(0), blockSize(bs)
{

  MPI_Comm_rank(PETSC_COMM_WORLD, &myRank);
  MPI_Comm_size(PETSC_COMM_WORLD, &numProcessors);
  // MPI_Comm_dup(PETSC_COMM_WORLD, &theComm);
  if (myRank != 0) {
    std::cerr << " XC::ShadowPetscSOE::ShadowPetscSOE - must be rank 0\n";
  }
  sendBuffer = (void *)(&sendData[0]);
  MPI_Barrier(PETSC_COMM_WORLD);

/************
  // send data on the XC::PetscSolver
  int petscMethod;
  int petscPre;

  KSPType method = theSOESolver.method;
  PCType preconditioner = theSOESolver.preconditioner;

  if (strcmp(method,KSPCG) == 0)
    petscMethod = 1;
  else if (strcmp(method,KSPGMRES) == 0)
    petscMethod = 2;
  else if (strcmp(method,KSPBCGS) == 0)
    petscMethod = 3;
  else if (strcmp(method,KSPCGS) == 0)
    petscMethod = 4;
  else if (strcmp(method,KSPTFQMR) == 0)
    petscMethod = 5;
  else if (strcmp(method,KSPTCQMR) == 0)
    petscMethod = 6;
  else if (strcmp(method,KSPCR) == 0)
    petscMethod = 7;
  else if (strcmp(method, KSPLSQR) == 0)
    petscMethod = 8;
  else if (strcmp(method, KSPRICHARDSON) == 0)
    petscMethod = 9;
  else if (strcmp(method,KSPCHEBYCHEV) == 0)
    petscMethod = 10;
  else if (strcmp(method, KSPPREONLY) == 0)
    petscMethod = 11;
  else {
    std::cerr << "XC::ShadowPetscSOE::ShadowPetscSOE - unknown KSP method\n";
    petscMethod = 12;
  }

  if (strcmp(preconditioner,PCNONE) == 0)
    petscPre = 1;
  else if (strcmp(preconditioner,PCJACOBI) == 0)
    petscPre = 2;
  else if (strcmp(preconditioner,PCSOR) == 0)
    petscPre = 3;
  else if (strcmp(preconditioner,PCEISENSTAT) == 0)
    petscPre = 4;
  else if (strcmp(preconditioner,PCBJACOBI) == 0)
    petscPre = 5;
  else if (strcmp(preconditioner,PCASM) == 0)
    petscPre = 6;
  else if (strcmp(preconditioner,PCILU) == 0)
    petscPre = 7;
  else if (strcmp(preconditioner,PCICC) == 0)
    petscPre = 8;  
  else if (strcmp(preconditioner,PCBGS) == 0)
    petscPre = 9;
  else if (strcmp(preconditioner,PCMG) == 0)
    petscPre = 10;
  else if (strcmp(preconditioner,PCSHELL) == 0)
    petscPre = 11;
  else if (strcmp(preconditioner,PCLU) == 0)
    petscPre = 12;
  else {
    std::cerr << "XC::ActorPetscSOE::ActorPetscSOE - unknown PC method\n";
    petscPre = 13;
  }

  // now send the data to the remote actor objects
  sendData[0] = petscMethod;
  sendData[1] = petscPre;
  MPI_Bcast(sendBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);    
**********/

}


int XC::ShadowPetscSOE::getNumEqn(void) const
{ return theSOE.size; }
    
XC::ShadowPetscSOE::~ShadowPetscSOE(void)
  {
    sendData[0] = 0;
    MPI_Bcast(sendBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);
    MPI_Barrier(PETSC_COMM_WORLD);
  }


int XC::ShadowPetscSOE::solve(void)
  {
    sendData[0] = 1;
    sendData[1] = theSOE.isFactored;
    MPI_Bcast(sendBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);
    return theSOE.solve();
  }

int XC::ShadowPetscSOE::setSize(Graph &theGraph)
  {
    int n= checkSize(theGraph);
    int size = n;
    const int N = n;

    // fist itearte through the vertices of the graph to get nnz,
    // the number of non-zeros.
    Vertex *theVertex;
    int NNZ = 0;
    VertexIter &theVertices = theGraph.getVertices();
    while((theVertex = theVertices()) != 0)
      {
        const std::set<int> &theAdjacency = theVertex->getAdjacency();
        NNZ += theAdjacency.size() +1; // the +1 is for the diag entry
      }

    // we determine the number of non-zeros & number of nonzero's
    // in each row of A
    // create two integer arrays. One containing the column indices for each
    // entry in A stored in order by row and column number. 
    // The other a pointer into this array for each row.
    ID rowStartA(size);  // start locations of rows in colA
    ID colA(NNZ); // column locations, stored row-wise

    // fill in rowStartA and colA
    if(size != 0)
      {
        rowStartA(0) = 0;
        int startLoc = 0;
        int lastLoc = 0;
        for(int a=0; a<size; a++)
          {
            theVertex = theGraph.getVertexPtr(a);
            if(theVertex == 0)
              {
	        std::cerr << "WARNING:XC::SparseGenLinSOE::setSize :";
	        std::cerr << " vertex " << a << " not in graph! - size set to 0\n";
	        size = 0;
	        return -1;
              }

            colA(lastLoc++) = theVertex->getTag(); // place diag in first
            const std::set<int> &theAdjacency = theVertex->getAdjacency();
            int idSize = theAdjacency.size();
	
            // now we have to place the entries in the ID into order in colA
            for(int i=0; i<idSize; i++)
              {
                int row = theAdjacency(i);
	        bool foundPlace = false;
	        // find a place in colA for current col
	        for(int j=startLoc; j<lastLoc; j++)
                  if(colA(j) > row)
                    { 
                      // move the entries already there one further on
	              // and place col in current location
                      for(int k=lastLoc; k>j; k--)
                        colA(k) = colA(k-1);
                      colA(j) = row;
                      foundPlace = true;
	              j = lastLoc;
	            }
                if(foundPlace == false) // put in at the end
                  colA(lastLoc) = row;
                lastLoc++;
              }
            rowStartA(a+1)= lastLoc;;	    
            startLoc = lastLoc;
          }
      }
    // now for each of the SOE's we determine how to invoke setSizeParallel()
    sendData[0] = 2;
    MPI_Bcast(sendBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);

    int dnz = 0;
    int onz = 0;
    int numRowsTyp = N/blockSize/numProcessors;
    numRowsTyp *= blockSize;
    int numRowsLast = numRowsTyp + (N - numProcessors*numRowsTyp); // lastProc extra rows
    ID dnnz(numRowsLast);
    ID onnz(numRowsLast);

    // first determine start and end rows of diag block
    int numRows = numRowsLast;
    int endRow = N-1;
    int startRow = endRow-numRows +1; 
  
    int result = 0;
    // we have to go last to first
    for(int i=numProcessors-1; i>=0; i--)
      {
        // for each processor determine onnz and dnnz info in colA
        for(int j=0; j<numRows; j++)
          {
            dnnz(j)= 0; 
            onnz(j)= 0;
            int rowStart = rowStartA(startRow+j);
            int nextRowStart = rowStartA(startRow+j+1);
            for(int k=rowStart; k<nextRowStart; k++)
              {
                int col = colA(k);
                if(col < startRow || col > endRow)
                  onnz(j)+= 1;
	        else
	          dnnz(j)+= 1;
              }
          }

        // now send the data
        if(i != 0)
          {  // remote object
            sendData[0] = 2;
            sendData[1] = numRows;
            sendData[2] = n;
      
            int tag = 100;
            MPI_Send(sendBuffer, 3, MPI_INT, i, tag, PETSC_COMM_WORLD);

            tag = 101;
            void *buffer = (void *)dnnz.getDataPtr();
            MPI_Send(buffer, numRows, MPI_INT, i, tag, PETSC_COMM_WORLD);

            tag = 102;
            buffer = (void *)onnz.getDataPtr();
            MPI_Send(buffer, numRows, MPI_INT, i, tag, PETSC_COMM_WORLD);
          }      

        // increment startRow, endRow and numRows if necessary
        endRow = startRow-1;
        numRows = numRowsTyp;
        startRow = endRow-numRows +1; 
      }

    // we broadcast again before we start setSizeParallel()
    // this is because Petsc all processes need to call setup at same
    // time .. if don't we hang
    MPI_Bcast(sendBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);  
    result = theSOE.setSizeParallel(numRows, n, dnz, dnnz.getDataPtr(), onz, onnz.getDataPtr());

    return result;
  }



int XC::ShadowPetscSOE::addA(const XC::Matrix &m, const XC::ID &id, double fact)
  { return theSOE.addA(m, id, fact); }

    
int XC::ShadowPetscSOE::addB(const XC::Vector &v, const XC::ID &id,const double &fact)
  { return theSOE.addB(v, id, fact); }


int XC::ShadowPetscSOE::setB(const XC::Vector &v,const double &fact)
  { return theSOE.setB(v, fact); }


void  XC::ShadowPetscSOE::zeroA(void)
  {
    sendData[0] = 3;
    MPI_Bcast(sendBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);
    theSOE.zeroA();
  }
	
void 
XC::ShadowPetscSOE::zeroB(void)
{
  sendData[0] = 4;
  MPI_Bcast(sendBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);
  theSOE.zeroB();
}


const XC::Vector &XC::ShadowPetscSOE::getX(void)
  {
    sendData[0] = 5;
    MPI_Bcast(sendBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);


  double *theData =0;
  double *X = theSOE.X;

  // put the local data into the X array
  int high, low;
  int ierr = VecGetOwnershipRange(theSOE.x, &low, &high);CHKERRA(ierr);       

  ierr = VecGetArray(theSOE.x, &theData); CHKERRA(ierr);       
  std::cerr << "XC::ShadowPetscSOE::getX - low:high " << low << " " << high << std::endl;
  for (int i=low; i<=high; i++)
    X[i] = theData[i-low];
  ierr = VecRestoreArray(theSOE.x, &theData); CHKERRA(ierr);             

  // now from each actor object get it's local copy
  // and place in the array
  int tag = 99;
  for (i=1; i<numProcessors; i++) {
    MPI_Status status;
    MPI_Recv(sendBuffer, 3, MPI_INT, i, tag, PETSC_COMM_WORLD, &status);
    low = sendData[0];
    high = sendData[1];
  std::cerr << "XC::ShadowPetscSOE::getX - low:high " << low << " " << high << std::endl;
    double *dataBuf = &X[low];
    void *buffer = (void *)dataBuf;
    MPI_Recv(buffer, high-low, MPI_DOUBLE, i, tag, PETSC_COMM_WORLD, &status);
  }

  return theSOE.getX();
}


const XC::Vector &XC::ShadowPetscSOE::getB(void)
  {
    sendData[0] = 6;
    MPI_Bcast(sendBuffer, 3, MPI_INT, 0, PETSC_COMM_WORLD);

    // STOP ****** some more work here
    return theSOE.getB();
  }


double XC::ShadowPetscSOE::normRHS(void)
  {
    theSOE.getB();
    double norm =0.0;
    int size = theSOE.size;
    const double *Bptr = theSOE.getPtrB();
    for(int i=0; i<size; i++)
      {
        double Yi = *Bptr++;
        norm += Yi*Yi;
      }
    return sqrt(norm);
  }    


void XC::ShadowPetscSOE::setX(int loc, double value)
  { theSOE.setX(loc, value); }

bool XC::ShadowPetscSOE::setSolver(PetscSolver &newSolver)
  {
    std::cerr << "XC::ShadowPetscSOE::setSolver - not yet working\n";
    return false;
  }


int XC::ShadowPetscSOE::sendSelf(CommParameters &cp)
  {
    std::cerr << "WARNING XC::ShadowPetscSOE::sendSelf - does not send itself YET\n";
    return 0;
  }


int XC::ShadowPetscSOE::recvSelf(const CommParameters &cp)
  {
    std::cerr << "WARNING XC::ShadowPetscSOE::sendSelf - does not receive itself YET\n";
    return 0;
  }






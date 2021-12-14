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
                                                                        
// $Revision: 1.3 $
// $Date: 2004/10/05 00:17:31 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/eigenSOE/SymBandEigenSOE.cpp,v $

// Written: MHS
// Created: Oct 2001
//
// Description: This file contains the class definition for
// SymBandEigenSOE, which stores a symmetric banded matrix, A, for
// standard eigenvalue computations.

#include <solution/system_of_eqn/eigenSOE/SymBandEigenSOE.h>
#include <solution/system_of_eqn/eigenSOE/SymBandEigenSolver.h>
#include <utility/matrix/Matrix.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <cmath>
#include <cstdio>

//! @brief Constructor.
XC::SymBandEigenSOE::SymBandEigenSOE(SolutionStrategy *owr)
  :EigenSOE(owr,EigenSOE_TAGS_SymBandEigenSOE), numSuperD(0) {}

//! @brief Sets the solver that will be used to solve the eigenvalue problem.
bool XC::SymBandEigenSOE::setSolver(EigenSolver *newSolver)
  {
    bool retval= false;
    SymBandEigenSolver *tmp= dynamic_cast<SymBandEigenSolver *>(newSolver);
    if(tmp)
      {
        tmp->setEigenSOE(*this);
        retval= EigenSOE::setSolver(tmp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; incompatible solver." << std::endl;
    return retval;
  }

//! @brief Sets the size of the system from the number of vertices in the graph.
int XC::SymBandEigenSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size= checkSize(theGraph);
  
    // determine the number of superdiagonals and subdiagonals
    numSuperD = theGraph.getVertexDiffExtrema();

    int tmp;
    if(__builtin_smul_overflow(size, numSuperD, &tmp))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ERROR: integer overflow."
		  << " Number of diagonals is huge: "
	          << numSuperD << std::endl;
      }
    const int newSize = size*(numSuperD+1);
    if(newSize > A.Size())
      A.resize(newSize);
    A.Zero();  
    factored = false;
  
    // invoke setSize() on the Solver
    EigenSolver *theSolvr = getSolver();
    result = theSolvr->setSize();
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< ";- solver failed in " << __FUNCTION__
	        << std::endl;
    return result;
  }

//! @brief Assemblies into A the matrix being passed as parameter
//! multimplied by the fact parameter.
int XC::SymBandEigenSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a quick return 
    if(fact == 0.0) return 0;
    if(A.Size()==0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ERROR: A matrix has zero size - has setSize() been called?\n";
        return -1;
      }      

    // check that m and id are of similar size
    int idSize = id.Size();    
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; matrix and ID not of similar sizes,\n";
        return -1;
      }
    if(fact == 1.0)
      { // do not need to multiply 
        for(int i = 0; i < idSize; i++)
          {
            int col = id(i);
            if(col < size && col >= 0)
              {
	        double *coliiPtr= A.getDataPtr() +(col+1)*(numSuperD+1) - 1;
	        const int minColRow= col - (numSuperD+1) + 1;
	        for(int j= 0;j<idSize;j++)
                  {
	            int row = id(j);
	            if(row<size && row>=0 && row<=col && row>=minColRow)
                      { // only add upper
	                double *APtr= coliiPtr + (row-col);
	                *APtr+= m(j,i);
	              }
	          }// for j
              } 
          }// for i
      }
    else
      {
        for(int i= 0;i<idSize;i++)
          {
            int col = id(i);
            if(col < size && col >= 0)
              {
	        double *coliiPtr= A.getDataPtr()+(col+1)*(numSuperD+1) - 1;
	        const int minColRow= col - (numSuperD+1) +1;
	        for(int j = 0; j < idSize; j++)
                  {
	            int row = id(j);
	            if(row < size && row >= 0 && row <= col && row >= minColRow)
                      { // only add upper
	                double *APtr = coliiPtr + (row-col);
	                *APtr+= m(j,i)*fact;
	              }
	          }  // for j
              } 
          }  // for i
      }
    return 0;
  }

//! @brief Zeroes the matrix A.
void XC::SymBandEigenSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }

//! @brief Assemblies into A the matrix being passed as parameter
//! multimplied by the fact parameter.
int XC::SymBandEigenSOE::addM(const Matrix &m, const ID &id, double fact)
  {
    // check for a quick return 
    if(fact == 0.0)
      return 0;

    // check memory for M has been allocated
    if(M.Size() != size)
      {
        M.resize(size);
        M.Zero();
      }
    resize_mass_matrix_if_needed(size);

    // check that m and id are of similar size
    int idSize= id.Size();    
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; matrix and ID not of similar sizes!!\n";
        return -1;
      }

    for(int i = 0; i <idSize; i++)
      {
        int loc = id(i);
        if(loc >= 0)
	  {
            M[loc]+= fact * m(i,i);
            massMatrix(loc,loc)+= m(i,i);
          }
      }

    bool issueWarning = false;
    for(int ii=0; ii<idSize; ii++) 
      for(int jj=0; jj<idSize; jj++)
        if(ii!=jj)
	  if(m(ii,jj) != 0.0)
	    issueWarning = true;
    if(issueWarning == true)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: m passed was not diagonal,"
	          << " only diagonal entries added\n";
      }
    return 0;
  }

//! @brief Zeroes the matrix M.
void XC::SymBandEigenSOE::zeroM(void)
  {
    EigenSOE::zeroM();
    M.Zero();
  }

//! @brief Makes M the identity matrix (to find stiffness matrix eigenvalues).
void XC::SymBandEigenSOE::identityM(void)
  {
    EigenSOE::identityM();
    //M.Identity();
    const int sz= M.Size();
    for(int i=0;i<sz;i++)
      M(i)= 1.0;
  }

int XC::SymBandEigenSOE::sendSelf(Communicator &comm)
  { return 0; }
    
int XC::SymBandEigenSOE::recvSelf(const Communicator &comm)
  { return 0; }

//! @brief Save the SOE matrices and vectors to file.
//!
//! Normally it's used to store temporarily those
//! objects on disk while executing getRCond to avoid
//! interferences with solve (different types of
//! factorization...).
void XC::SymBandEigenSOE::save(void) const
  {
    tmpFileName= std::tmpnam(nullptr);
    std::ofstream out(tmpFileName,std::ios::out|std::ios::binary);
    if(!out)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; cannot open file: "
		<< tmpFileName << std::endl;
    size_t sz= A.Size();
    out.write((char *)(&sz), sizeof(sz));
    out.write((char *)(A.getDataPtr()),sz);
    sz= M.Size();
    out.write((char *)(&sz), sizeof(sz));
    out.write((char *)(M.getDataPtr()),sz);
    out.close();
  }

//! @brief Restore the SOE matrices and vectors from file.
//!
//! Normally it's used to restore those
//! objects from disk after executing getRCond to avoid
//! interferences with solve (different types of
//! factorization...).
void XC::SymBandEigenSOE::restore(void)
  {
    std::ifstream in(tmpFileName,std::ios::in|std::ios::binary);
    if(!in)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; cannot open file: " << tmpFileName
		<< std::endl;
    size_t sz= A.Size();
    in.read((char *)(&sz), sizeof(sz));
    in.read((char *)(A.getDataPtr()),sz);
    sz= M.Size();
    in.read((char *)(&sz), sizeof(sz));
    in.read((char *)(M.getDataPtr()),sz);
    in.close();
    remove(tmpFileName.c_str()); //Not needed anymore;
    tmpFileName= "";
  }

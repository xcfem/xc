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

// $Revision: 1.4 $
// $Date: 2005/12/06 21:56:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.cpp,v $


// Written: fmk
//
// Description: This file contains the implementation for XC::BandGenLinSOE


#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
XC::BandGenLinSOE::BandGenLinSOE(SolutionStrategy *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_BandGenLinSOE), numSuperD(0), numSubD(0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
//! @param classTag: class identifier.
XC::BandGenLinSOE::BandGenLinSOE(SolutionStrategy *owr,int classTag)
  :FactoredSOEBase(owr,classTag), numSuperD(0), numSubD(0) {}


//! @brief Constructor.
//!
//! Sets the size of the system to \f$N\f$, the number of superdiagonals 
//! and the number of subdiagonals. Allocates memory for the arrays; if
//! not enough memory is available a warning message is printed and the
//! system size is set to \f$0\f$.
//! 
//! @param N: size of the system.
//! @param numSuperDiag: number of superdiagonals.
//! @param numSubDiag: number of subdiagonals.
XC::BandGenLinSOE::BandGenLinSOE(SolutionStrategy *owr,int N, int numSuperDiag, int numSubDiag,BandGenLinSolver *theSolvr)
  :FactoredSOEBase(owr,LinSOE_TAGS_BandGenLinSOE,N),
   numSuperD(numSuperDiag), numSubD(numSubDiag)
  {
    const size_t Asize= N * (2*numSubD + numSuperD +1);
    A= Vector(Asize);
    A.Zero();

    inic(size);

    if(theSolvr)
      {
        setSolver(theSolvr);

        const int solverOK= theSolvr->setSize();
        if(solverOK < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING solver failed setSize() in constructor\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; solver not set." << std::endl;
  }

//! @brief Sets the solver.
//!
//! Checks the compatibility of the solver and invokes
//! {\em setSOE(*this)} on \p newSolver.
bool XC::BandGenLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    BandGenLinSolver *tmp= dynamic_cast<BandGenLinSolver *>(newSolver);
    if(tmp)
      retval= FactoredSOEBase::setSolver(tmp);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; incompatible solver."
		<< std::endl;
    return retval;
  }

//! @brief Compute the size of the system from the graph.
//! 
//! The size of the system is determined by looking at the adjacency ID of
//! each Vertex in the Graph object \p G. This is done by first setting
//! \p kl and \p ku equal to \f$0\f$ and then checking for each Vertex
//! in \p G, whether any of the vertex tags in the Vertices adjacency
//! ID results in \p kl or \p ku being increased. Knowing \p kl,
//! \p ku and the size of the system (the number of Vertices in \p G),
//! a check to see if the previously allocated 1d arrays for \f$A\f$, \f$x\f$
//! and \f$b\f$ are large enough. If the memory portions allocated for the 1d
//! arrays are not big enough, the old space is returned to the heap and
//! new space is allocated from the heap. Prints a warning message if
//! not enough memory is available on the heap for the 1d arrays and
//! returns a \f$-1\f$. If memory is available, the components of the arrays
//! are zeroed and \f$A\f$ is marked as being unfactored. If the system size
//! has increased, new Vector objects for \f$x\f$ and \f$b\f$ using the {\em (do//! uble*,int)} Vector constructor are created. Finally, the result of
//! invoking setSize() on the associated Solver object is returned.
int XC::BandGenLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size= checkSize(theGraph);

    /*
     * determine the number of superdiagonals and subdiagonals
     */
    theGraph.getBand(numSubD,numSuperD);

    int newSize = size * (2*numSubD + numSuperD +1);
    if(newSize > A.Size())
      { // we have to get another space for A
        A.resize(newSize);
      }
    A.Zero();

    factored = false;

    if(size > B.Size())
      { // we have to get space for the vectors

        // create the new
        inic(size);
      }

    // invoke setSize() on the Solver
    LinearSOESolver *theSolvr = this->getSolver();
    const int solverOK = theSolvr->setSize();
    if(solverOK < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING solver failed setSize().\n";
        return solverOK;
      }
    return result;
  }

//! @brief Assembles fact times the matrix m into the matrix A.
//! 
//! First tests that \p loc and \p M are of compatible sizes; if not
//! a warning message is printed and a \f$-1\f$ is returned. The LinearSOE
//! object then assembles \p fact times the Matrix {\em 
//! M} into the matrix \f$A\f$. The Matrix is assembled into \f$A\f$ at the
//! locations given by the ID object \p loc, i.e. \f$a_{loc(i),loc(j)} +=
//! fact * M(i,j)\f$. If the location specified is outside the range,
//! i.e. \f$(-1,-1)\f$ the corresponding entry in \p M is not added to
//! \f$A\f$. If \p fact is equal to \f$0.0\f$ or \f$1.0\f$, more efficient steps
//! are performed. Returns \f$0\f$.
//!
//! @param m: matrix to assemble.
//! @param id: row identifiers.
//! @param fact: factor.
int XC::BandGenLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
  //  std::cerr << "BAND- addA() " << m << id;

    // check for a quick return
    if(fact == 0.0)  return 0;


    // check that m and id are of similar size
    const int idSize = id.Size();
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Matrix and ID not of similar sizes.\n";
        return -1;
      }

    const int ldA = 2*numSubD + numSuperD + 1;


    if(fact == 1.0)
      { // do not need to multiply
        for(int i=0; i<idSize; i++)
          {
            const int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = A.getDataPtr() + col*ldA + numSubD + numSuperD;
                for(int j=0; j<idSize; j++)
                  {
                    int row = id(j);
                    if(row <size && row >= 0)
                      {
                        int diff = col - row;
                        if(diff > 0)
                          {
                            if(diff <= numSuperD)
                              {
                                double *APtr = coliiPtr - diff;
                                *APtr += m(j,i);
                              }
                          }
                        else
                          {
                            diff *= -1;
                            if(diff <= numSubD)
                              {
                                double *APtr = coliiPtr + diff;
                                *APtr += m(j,i);
                              }
                          }
                      }
                  }  // for j
              }
          }  // for i
      }
    else
      {
        for(int i=0; i<idSize; i++)
          {
            int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = A.getDataPtr() + col*ldA + numSubD + numSuperD;
                for(int j=0; j<idSize; j++)
                  {
                    int row = id(j);
                    if(row <size && row >= 0)
                      {
                        int diff = col - row;
                        if(diff > 0)
                          {
                            if(diff <= numSuperD)
                              {
                                double *APtr = coliiPtr - diff;
                                *APtr += m(j,i) *fact;
                              }
                          }
                        else
                          {
                            diff *= -1;
                            if(diff <= numSubD)
                              {
                                double *APtr = coliiPtr + diff;
                                *APtr += m(j,i) *fact;
                              }
                          }
                      }
                  }  // for j
              }
          }  // for i
      }
    //    std::cerr << A;
    // std::cerr << std::endl;
    return 0;
  }

//! @brief Zeros the entries in the 1d array for \f$A\f$ and marks the
//! system as not having been factored.
void XC::BandGenLinSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }

int XC::BandGenLinSOE::sendSelf(Communicator &comm)
  { return 0; }


int XC::BandGenLinSOE::recvSelf(const Communicator &comm)
  { return 0; }



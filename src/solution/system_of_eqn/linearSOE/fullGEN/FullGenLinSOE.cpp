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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:02:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.C
//
// Written: fmk 
// Created: Febuary 1997
// Revision: A
//
// Description: This file contains the implementation for XC::FullGenLinSOE


#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSolver.h>
#include <utility/matrix/Matrix.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
XC::FullGenLinSOE::FullGenLinSOE(AnalysisAggregation *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_FullGenLinSOE) {}


//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
XC::FullGenLinSOE::FullGenLinSOE(AnalysisAggregation *owr,int N)
  :FactoredSOEBase(owr,LinSOE_TAGS_FullGenLinSOE,N)
  {
    A= Vector(size*size);
        
    // zero the matrix
    A.Zero();
    inic(size);        
  }

//! @brief Virtual constructor.
XC::SystemOfEqn *XC::FullGenLinSOE::getCopy(void) const
  { return new FullGenLinSOE(*this); }


//! @brief Checks if the solver has a suitable type and then
//! it class its parent setSolver method.
//!
//! @param newSolver: solver to use.
bool XC::FullGenLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    FullGenLinSolver *tmp= dynamic_cast<FullGenLinSolver *>(newSolver);
    if(tmp)
      retval= FactoredSOEBase::setSolver(tmp);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; solver is not of a suitable type."
		<< std::endl;
    return retval;
  }

//! @brief Computes the size of the system from the graph data.
//!
//! The size of the system is determined by invoking getNumVertex()
//! on \p theGraph. If the old space allocated for the 1d arrays is not
//! big enough, it the old space is returned to the heap and new space is
//! allocated from the heap. Prints a warning message, sets size to \f$0\f$
//! and returns a \f$-1\f$, if not enough memory is available on the heap for
//! the 1d arrays. If memory is available, the components of the arrays are
//! zeroed and \f$A\f$ is marked as being unfactored. If the system size has
//! increased, new Vector objects for \f$x\f$ and \f$b\f$ using the {\em (double
//! *,int)} Vector constructor are created. Finally, the result of
//! invoking setSize() on the associated Solver object is returned.
int XC::FullGenLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size= checkSize(theGraph);

    const int size2= size*size;
    A.resize(size2);
    A.Zero();
        
    factored = false;
    
    if(size > B.Size())
      inic(size);

    // invoke setSize() on the XC::Solver    
    LinearSOESolver *theSolvr = this->getSolver();
    int solverOK = theSolvr->setSize();
    if(solverOK < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: solver failed setSize().\n";
        return solverOK;
      }    
    return result;
  }

//! @brief Assembles the product of the matrix and the factor on the
//! system matrix.
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
int XC::FullGenLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a XC::quick return 
    if(fact == 0.0)  return 0;

    int idSize = id.Size();
    
    // check that m and id are of similar size
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Matrix and ID not of similar sizes\n";
        return -1;
      }
    
    double *addrA= A.getDataPtr();
    if(fact == 1.0)
      { // do not need to multiply 
        for(int i=0; i<idSize; i++)
          {
            const int col = id(i);
            if(col < size && col >= 0)
              {
                double *startColiPtr= addrA + col*size;
                for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
                    if(row <size && row >= 0)
                      {
                        double *APtr= startColiPtr + row;
                        *APtr+= m(j,i);
                      }
                  }  // for j
              }
          }  // for i
      }
    else
      {
        for(int i=0; i<idSize; i++)
          {
            const int col= id(i);
            if(col < size && col >= 0)
              {
                double *startColiPtr= addrA + col*size;
                for(int j=0; j<idSize; j++)
                  {
                    const int row= id(j);
                    if(row <size && row >= 0)
                      {
                         double *APtr= startColiPtr + row;
                         *APtr += m(j,i) * fact;
                      }
                  }  // for j
              } 
          }  // for i
      }    
    return 0;
  }

//! @brief Zeros the entries in the 1d array for \f$A\f$ and marks the system
//! as not having been factored.
void XC::FullGenLinSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }

//! @brief Sends objects through the communicator.
int XC::FullGenLinSOE::sendSelf(CommParameters &cp)
  {
    return 0;
  }

//! @brief Receives objects through the communicator.
int XC::FullGenLinSOE::recvSelf(const CommParameters &cp)
  {
    return 0;
  }







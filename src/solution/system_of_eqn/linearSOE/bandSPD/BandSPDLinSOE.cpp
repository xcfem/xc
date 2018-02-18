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
// $Date: 2005/12/06 22:03:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.cpp,v $


// file: ~/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.C
//
// Written: fmk
// Created: Febuary 1997
// Revision: A
//
// Description: This file contains the implementation for XC::BandSPDLinSOE


#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.h>
#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>

void XC::BandSPDLinSOE::inicA(const size_t &hsz)
  {
    A.resize(hsz);
    A.Zero();
  }

//! @brief Constructor.
//!
//! The system size is set to \f$0\f$ and the matrix \f$A\f$ is marked as
//! not having been factored. Invokes {\em setLinearSOE(*this)} on the
//! \p solver. No memory is allocated for the 3 1d arrays.
//! 
//! @param owr: analysis aggregation that owns this object.
XC::BandSPDLinSOE::BandSPDLinSOE(AnalysisAggregation *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_BandSPDLinSOE), half_band(0){}


//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
//! @param classTag: identifier of the class.
XC::BandSPDLinSOE::BandSPDLinSOE(AnalysisAggregation *owr,int classTag)
  :FactoredSOEBase(owr,classTag), half_band(0) {}


//! @brief Constructor.
//!
//! Invokes {\em setLinearSOE(*this)} and setSize() on the \p theSolver,
//! printing a warning message if setSize() returns a negative number. Also
//! creates Vector objects for \f$x\f$ and \f$b\f$ using the
//! {\em (double *,int)} Vector constructor.
//! 
//! @param owr: analysis aggregation that owns this object.
//! @param classTag: identifier of the class.
//! @param N: size of the system.
//! @param numSuper: number of super diagonals.
//! @param the_Solver: pointer to the solver.
XC::BandSPDLinSOE::BandSPDLinSOE(AnalysisAggregation *owr,int N, int numSuper,BandSPDLinSolver *the_Solver)
  :FactoredSOEBase(owr,LinSOE_TAGS_BandSPDLinSOE,N), half_band(0)
  {
    half_band = numSuper+1;

    inicA(half_band*size);

    inic(size);

    if(the_Solver)
      setSolver(the_Solver);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; solver not set." << std::endl;
  }

//! @brief Virtual constructor.
XC::SystemOfEqn *XC::BandSPDLinSOE::getCopy(void) const
  { return new BandSPDLinSOE(*this); }

//! @brief Set pointer to solver.
//!
//! Invokes {\em setLinearSOE(*this)} on \p newSolver.
//! If the system size is not equal to \f$0\f$, it also invokes setSize()
//! on \p newSolver, printing a warning and returning the returned value if this
//! method returns a number less than \f$0\f$. Finally it returns the result
//! of invoking the LinearSOE classes setSolver() method.
bool XC::BandSPDLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    BandSPDLinSolver *tmp= dynamic_cast<BandSPDLinSolver *>(newSolver);
    if(tmp)
      retval= FactoredSOEBase::setSolver(tmp);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; incompatible solver." << std::endl;
    return retval;
  }

//! @brief Sets the size of the system from the number of vertices in the graph.
//!
//! The size of the system is determined by looking at the adjacency ID of
//! each Vertex in the Graph object \p G. This is done by first setting
//! \p ku equal to \f$0\f$ and then checking for each Vertex
//! in \p G, whether any of the vertex tags in the Vertices adjacency
//! ID results in \p ku being increased. Knowing \p ku and the size
//! of the system (the number of Vertices in \p G, a check to see if
//! the previously allocated 1d arrays for \f$A\f$, \f$x\f$ and \f$b\f$ are
//! large enough. If the memory portions allocated for the 1d arrays are not big
//! enough, the old space is returned to the heap and new space is
//! allocated from the heap. Prints a warning message if not enough
//! memory is available on the heap for the 1d arrays and returns a
//! \f$-1\f$. If memory is available, the components of the arrays are zeroed
//! and \f$A\f$ is marked as being unfactored. If the system size has
//! increased, new Vector objects for \f$x\f$ and \f$b\f$ using the {\em (double
//! *,int)} Vector constructor are created. Finally, the result of
//! invoking setSize() on the associated Solver object is
//! returned. 
int XC::BandSPDLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size= checkSize(theGraph);
    half_band= theGraph.getVertexDiffMaxima();


    half_band+= 1; // include the diagonal


    if(half_band*size > A.Size())// we have to get another space for A
      inicA(half_band*size); 

    factored = false;

    if(size > B.Size())
      inic(size);


    // invoke setSize() on the Solver
    LinearSOESolver *the_Solver= this->getSolver();
    int solverOK = the_Solver->setSize();
    if(solverOK < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: solver failed in setSize()\n";
        return solverOK;
      }

    return result;
  }

//! First tests that \p loc and \p M are of compatible sizes; if not
//! a warning message is printed and a \f$-1\f$ is returned. The LinearSOE
//! object then assembles \p fact times the Matrix {\em 
//! M} into the matrix \f$A\f$. The Matrix is assembled into \f$A\f$ at the
//! locations given by the ID object \p loc, i.e. \f$a_{loc(i),loc(j)} +=
//! fact * M(i,j)\f$. If the location specified is outside the range,
//! i.e. \f$(-1,-1)\f$ the corresponding entry in \p M is not added to
//! \f$A\f$. If \p fact is equal to \f$0.0\f$ or \f$1.0\f$, more efficient steps
//! are performed. Returns \f$0\f$.
int XC::BandSPDLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a quick return
    if(fact == 0.0)  return 0;

    // check that m and id are of similar size
    int idSize = id.Size();
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Matrix and ID not of similar sizes.\n";
        return -1;
      }

    if(fact == 1.0)
      { // do not need to multiply
        for(int i=0; i<idSize; i++)
          {
            int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = A.getDataPtr() +(col+1)*half_band -1;
                int minColRow = col - half_band + 1;
                for(int j=0; j<idSize; j++)
                  {
                    int row = id(j);
                    if(row <size && row >= 0 &&
                        row <= col && row >= minColRow)
                      { // only add upper
                         double *APtr = coliiPtr + (row-col);
                         *APtr += m(j,i);
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
                double *coliiPtr = A.getDataPtr() +(col+1)*half_band -1;
                int minColRow = col - half_band +1;
                for(int j=0; j<idSize; j++)
                  {
                    int row = id(j);
                    if(row <size && row >= 0 &&
                        row <= col && row >= minColRow)
                      { // only add upper
                         double *APtr = coliiPtr + (row-col);
                         *APtr += m(j,i)*fact;
                      }
                  }  // for j
              }
          }  // for i
      }
    return 0;
  }


//! @brief Zeros the entries in the 1d array for \f$A\f$ and marks the system
//! as not having been factored.
void XC::BandSPDLinSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }

int XC::BandSPDLinSOE::sendSelf(CommParameters &cp)
  { return 0; }


int XC::BandSPDLinSOE::recvSelf(const CommParameters &cp)
  { return 0; }

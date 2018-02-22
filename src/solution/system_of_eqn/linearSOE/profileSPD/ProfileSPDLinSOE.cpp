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
// $Date: 2005/12/06 22:06:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation for XC::ProfileSPDLinSOE


#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSolver.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
XC::ProfileSPDLinSOE::ProfileSPDLinSOE(AnalysisAggregation *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_ProfileSPDLinSOE),
   profileSize(0), isAcondensed(false), numInt(0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
//! @param classTag: identifier of the class.
XC::ProfileSPDLinSOE::ProfileSPDLinSOE(AnalysisAggregation *owr,int classTag)
  :FactoredSOEBase(owr,classTag),
   profileSize(0), isAcondensed(false), numInt(0) {}


//! @brief Constructor.
//!
//! The size of \f$A\f$ is given
//! by \f$newIloc(N-1)\f$, if this is not a valid address in \p newIloc a
//! segmentation fault or erronious results will result. The contents of
//! \f$iLoc\f$ are set equal to those of \p newIloc. Invokes {\em
//! setLinearSOE(*this)} and setSize() on \p solver,
//! printing a warning message if setSize() returns a negative
//! number. Also creates Vector objects for \f$x\f$ and \f$b\f$.
//! 
//! @param owr: analysis aggregation that owns this object.
//! @param N: size of the system.
//! @param the_Solver: pointer to the solver to use.
XC::ProfileSPDLinSOE::ProfileSPDLinSOE(AnalysisAggregation *owr,int N, int *iLoc,ProfileSPDLinSolver *the_Solver)
  :FactoredSOEBase(owr,LinSOE_TAGS_ProfileSPDLinSOE,N),
   profileSize(0), isAcondensed(false), numInt(0)
  {
    size = N;
    profileSize= iLoc[N-1];
    
    A= Vector(iLoc[N-1]);
        
    // zero the matrix
    A.Zero();
    
    inic(size);
    iDiagLoc= ID(size);
    
     // zero the vectors
     for(int i=0; i<size; i++)
       { iDiagLoc(i) = iLoc[i]; }

    if(the_Solver)
      setSolver(the_Solver);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; solver not set." << std::endl;
  }

//! @brief Set the solver to use.
//!
//! Invokes {\em setLinearSOE(*this)} on \p newSolver.
//! If the system size is not equal to \f$0\f$, it also invokes setSize()
//! on \p newSolver, printing a warning and returning the returned value if this
//! method returns a number less than \f$0\f$. Finally it returns the result
//! of invoking the LinearSOE classes setSolver() method.
bool XC::ProfileSPDLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    ProfileSPDLinSolver *tmp= dynamic_cast<ProfileSPDLinSolver *>(newSolver);
    if(tmp)
      retval= FactoredSOEBase::setSolver(tmp);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; not a suitable solver." << std::endl;
    return retval;
  }

//! @brief Sets the system size.
//!
//! The size of the system is determined by looking at the adjacency ID of
//! each Vertex in the Graph object \p G. This is done by first
//! determining the column height for each Vertex \f$i\f$ in \p G, done by
//! setting \f$iLoc(i)\f$ equal to \f$0\f$ and then checking for each Vertex
//! in \p G, whether any of the vertex tags in the Vertices adjacency
//! ID results in \f$iLoc(i)\f$ being increased. Knowing the col height of
//! each column, the values of \p iLoc can be determined. Knowing {\em
//! iLoc} and the size of the system (the number of Vertices in \p G, 
//! a check to see if the previously allocated 1d arrays for \f$A\f$, \f$x\f$
//! and \f$b\f$ are large enough. If the memory portions allocated for the 1d
//! arrays are not big enough, the old space is returned to the heap and
//! new space is allocated from the heap. Printins a warning message if
//! not enough memory is available on the heap for the 1d arrays and
//! returns a \f$-1\f$. If memory is available, the components of the arrays
//! are zeroed and \f$A\f$ is marked as being unfactored. If the system size
//! has increased, new Vector objects for \f$x\f$ and \f$b\f$ using the {\em
//! (double *,int)} Vector constructor are created. Finally, the result of 
//! invoking setSize() on the associated Solver object is
//! returned. 
int XC::ProfileSPDLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size= checkSize(theGraph);

    // check we have enough space in iDiagLoc and iLastCol
    // if not delete old and create new
    if(size > B.Size())
       { iDiagLoc.resize(size); }

    // zero out iDiagLoc 
    iDiagLoc.Zero();

    // now we go through the vertices to find the height of each col and
    // width of each row from the connectivity information.
    
    Vertex *vertexPtr;
    VertexIter &theVertices = theGraph.getVertices();

    while ((vertexPtr = theVertices()) != 0)
      {
        int vertexNum = vertexPtr->getTag();
        const std::set<int> &theAdjacency = vertexPtr->getAdjacency();
        int iiDiagLoc = iDiagLoc(vertexNum);
        int *iiDiagLocPtr = &(iDiagLoc(vertexNum));

        for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
          {
            const int otherNum= *i;
            const int diff= vertexNum-otherNum;
            if(diff > 0)
              {
                if(iiDiagLoc < diff)
                  {
                    iiDiagLoc = diff;
                    *iiDiagLocPtr = diff;
                  }
              } 
          }
      }


    // now go through iDiagLoc, adding 1 for the diagonal element
    // and then adding previous entry to give current location.
    if(!iDiagLoc.Nulo())
      iDiagLoc(0) = 1; // NOTE FORTRAN ARRAY LOCATION

    for(int j=1; j<size; j++)
      iDiagLoc(j)= iDiagLoc(j) + 1 + iDiagLoc(j-1);

    if(!iDiagLoc.Nulo())       
      profileSize = iDiagLoc[size-1];

    // check if we need more space to hold A
    // if so then go get it
    if(profileSize > A.Size())
      { A.resize(profileSize); }

    A.Zero();

    factored = false;
    isAcondensed = false;    

    if(size > B.Size())
      inic(size);

    // invoke setSize() on the XC::Solver
    LinearSOESolver *the_Solver = this->getSolver();
    int solverOK = the_Solver->setSize();
    if(solverOK < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING solver failed setSize()\n";
        return solverOK;
      }    
    return result;
  }

//! @brief Assembles the product of m by fact into A.
//! 
//! First tests that \p loc and \p M are of compatable sizes; if not
//! a warning message is printed and a \f$-1\f$ is returned. The LinearSOE
//! object then assembles \p fact times the Matrix {\em 
//! M} into the matrix \f$A\f$. The Matrix is assembled into \f$A\f$ at the
//! locations given by the ID object \p loc, i.e. \f$a_{loc(i),loc(j)} +=
//! fact * M(i,j)\f$. If the location specified is outside the range,
//! i.e. \f$(-1,-1)\f$ the corrseponding entry in \p M is not added to
//! \f$A\f$. If \p fact is equal to \f$0.0\f$ or \f$1.0\f$, more efficient steps
//! are performed. Returns \f$0\f$.
int XC::ProfileSPDLinSOE::addA(const Matrix &m, const ID &id, double fact)
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

    double *addrA= A.getDataPtr();
    if(fact == 1.0)
      { // do not need to multiply 
        for(int i=0; i<idSize; i++)
          {
            int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = &addrA[iDiagLoc(col)-1]; // -1 as fortran indexing 
                int minColRow;
                if(col == 0)
                    minColRow = 0;
                else
		  minColRow = col - (iDiagLoc(col) - iDiagLoc(col-1)) +1;
                for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
                    if(row <size && row >= 0 && row <= col && row >= minColRow)
                      { 
                        // we only add upper and inside profile
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
            const int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = &addrA[iDiagLoc(col)-1]; // -1 as fortran indexing                 
                int minColRow;
                if(col == 0)
                    minColRow = 0;
                else
		  minColRow = col - (iDiagLoc(col) - iDiagLoc(col-1)) +1;
                for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
                    if(row <size && row >= 0 && row <= col && row >= minColRow)
                      { 
                        // we only add upper and inside profile
                        double *APtr = coliiPtr + (row-col);
                         *APtr+= m(j,i) * fact;
                       }
                  }  // for j
              } 
          }  // for i
      }
    return 0;
  }

    
//! @brief Zeros the entries in the 1d array for \f$A\f$ and marks the system
//! as not having been factored.
void XC::ProfileSPDLinSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }


int XC::ProfileSPDLinSOE::sendSelf(CommParameters &cp)
  { return 0; }


int XC::ProfileSPDLinSOE::recvSelf(const CommParameters &cp)
  { return 0; }

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
// File: ~/system_of_eqn/eigenSOE/BandArpackSOE.C
//
// Written: Jun Peng
// Created: Febuary 1999
// Revision: A
//
// Description: This file contains the class definition for XC::BandArpackSOE
// BandArpackSOE is a subclass of XC::ArpackSOE. It uses the LAPACK storage
// scheme to store the components of the K, M matrix, which is a full matrix.
// It uses the ARPACK to do eigenvalue analysis.

#include <solution/system_of_eqn/eigenSOE/BandArpackSOE.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackSolver.h>
#include <utility/matrix/Matrix.h>
#include <solution/graph/graph/Graph.h>
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>


//! @brief Constructor.
XC::BandArpackSOE::BandArpackSOE(SoluMethod *owr, double theShift)
  :ArpackSOE(owr,EigenSOE_TAGS_BandArpackSOE,theShift),
   numSuperD(0), numSubD(0) {}

//! @brief Sets the solver that will be used to compute the solution.
bool XC::BandArpackSOE::setSolver(EigenSolver *newSolver)
  {
    bool retval= false;
    BandArpackSolver *tmp= dynamic_cast<BandArpackSolver *>(newSolver);
    if(tmp)
      {
        tmp->setEigenSOE(*this);
        retval= ArpackSOE::setSolver(tmp);
      }
    else
      std::cerr << "BandArpackSOE::setSolver; incompatible solver." << std::endl;
    return retval;
  }

//! @brief Sets the size of the system from the number of vertices in the graph.
int XC::BandArpackSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size= checkSize(theGraph);

    // determine the number of superdiagonals and subdiagonals
    theGraph.getBand(numSubD,numSuperD);

    const int newSize = size * (2*numSubD + numSuperD +1);
    if(newSize > A.Size())
      A.resize(newSize);
    A.Zero();
    factored = false;

    // invoke setSize() on the XC::Solver
    EigenSolver *theSolvr = this->getSolver();
    int solverOK = theSolvr->setSize();
    if(solverOK < 0)
      {
        std::cerr << "WARNING: BandArpackSOE::setSize :";
        std::cerr << " solver failed setSize()\n";
        return solverOK;
      }
    return result;
  }

//! @brief Assemblies in A the matrix being passed as parameter
//! multiplied by the fact parameter.
int XC::BandArpackSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a quick return
    if(fact == 0.0)  return 0;

    // check that m and id are of same size
    int idSize = id.Size();
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << "BandArpackSOE::addA(); Matrix and ID not of similar sizes\n";
        return -1;
      }

    const int ldA = 2*numSubD + numSuperD + 1;

    if(fact == 1.0)
      { // do not need to multiply
        for(int i=0; i<idSize; i++)
          {
            const int col= id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr= A.getDataPtr() + col*ldA + numSubD + numSuperD;
                for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
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
                            diff*= -1;
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
        for(int i=0;i<idSize;i++)
          {
            const int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = A.getDataPtr() + col*ldA + numSubD + numSuperD;
                for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
                    if(row <size && row >= 0)
                      {
                        int diff = col - row;
                        if(diff > 0)
                          {
                            if(diff <= numSuperD)
                              {
                                double *APtr = coliiPtr - diff;
                                *APtr+= m(j,i)*fact;
                              }
                          }
                        else
                          {
                            diff*= -1;
                            if(diff <= numSubD)
                              {
                                double *APtr = coliiPtr + diff;
                                *APtr+= m(j,i)*fact;
                              }
                          }
                      }
                  }  // for j
              }
          }  // for i
      }
    return 0;
  }

//! @brief Anula la matriz M.
void XC::BandArpackSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }

//! @brief Assemblies in M the matrix being passed as parameter
//! multiplied by the fact parameter.
int XC::BandArpackSOE::addM(const Matrix &m, const ID &id, double fact)
  {
    bool retval= 0;
    //Added by LCPT.
    if(fact!=0.0)
      {
        const int idSize = id.Size();
        // check that m and id are of same size
        if(idSize != m.noRows() && idSize != m.noCols())
          {
            std::cerr << "BandArpackSOE::addM(); Matrix and ID not of similar sizes\n";
            retval= -1;
          }
        else
          {
            resize_mass_matrix_if_needed(size);      
            int col= 0, row= 0;
            if(fact==1.0)
              {
                for(int i=0; i<idSize; i++)
                  for(int j=0; j<idSize; j++)
                    {
                      col= id(i);
                      row = id(j);
                      massMatrix(row,col)+= m(i,j);
                    }
              }
            else
              {
                for(int i=0; i<idSize; i++)
                  for(int j=0; j<idSize; j++)
                    {
                      col= id(i);
                      row = id(j);
                      massMatrix(row,col)+= m(i,j)*fact;
                    }
              }
          }
      }
    //Added by LCPT ends.
    retval= this->addA(m, id, -shift);
    return retval;
  }

//! @brief Anula la matriz M.
void XC::BandArpackSOE::zeroM(void)
  {
    EigenSOE::zeroM();
  }


int XC::BandArpackSOE::sendSelf(CommParameters &cp)
  { return 0; }

int XC::BandArpackSOE::recvSelf(const CommParameters &cp)
  { return 0; }


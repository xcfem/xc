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
//SpectraSOE.cpp

#include <solution/system_of_eqn/eigenSOE/SpectraSOE.h>
#include <solution/system_of_eqn/eigenSOE/SpectraSolver.h>
#include "solution/graph/graph/Graph.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::SpectraSOE::SpectraSOE(SolutionStrategy *owr)
  :EigenSOE(owr,EigenSOE_TAGS_SpectraSOE), A(), M() {}

//! @brief Sets the solver to use.
bool XC::SpectraSOE::setSolver(EigenSolver *newSolver)
  {
    bool retval= false;
    SpectraSolver *tmp= dynamic_cast<SpectraSolver *>(newSolver);
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

//! @brief Sets the system size.
int XC::SpectraSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    const int newSize= checkSize(theGraph);
    if(size!=newSize)
      {
        size= newSize;
        A.resize(size,size);
        M.resize(size,size);
      }
    return result;
  }

//! @brief Assemblies into A the matrix being passed as parameter
//! multimplied by the fact parameter.
int XC::SpectraSOE::addToMatrix(std::deque<T> &tripletList,const Matrix &m, const ID &id,const double &fact)
  {
    // check for a quick return 
    if(fact == 0.0) return 0;

    // check that m and id are of similar size
    const int idSize = id.Size();
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Matrix and ID not of similar sizes.\n";
        return -1;
      }
    if(fact == 1.0)
      {
        for(int i= 0;i<idSize;i++)
          {
            const int row= id(i);
            if(row <= size && row >= 0)
              {
                for(int j= 0;j<idSize;j++)
                  {
	            const int col= id(j);
		    const double &value= m(i,j);
	            if(col <= size && col >= 0 && (value!=0.0))
                      tripletList.push_back(T(row,col,value));
	          }// for j
              }
          }// for i
      }
    else if(fact == -1.0)
      { // we save the product.
        for(int i= 0;i<idSize;i++)
          {
            const int row= id(i);
            if(row <= size && row >= 0)
              {
                for(int j= 0;j<idSize;j++)
                  {
	            const int col= id(j);
	            if(col <= size && col >= 0 && (m(i,j)!= 0.0))
                      tripletList.push_back(T(row,col,-m(i,j)));
	          }// for j
              }
          }// for i
      }
    else
      {
        for(int i= 0;i<idSize;i++)
          {
            const int row = id(i);
            if(row <= size && row >= 0)
              {
                for(int j= 0;j<idSize;j++)
                  {
                    const int col = id(j);
	            if(col <= size && col >= 0 && (m(i,j)!= 0.0))
                      tripletList.push_back(T(row,col,m(i,j)*fact));
	          }  // for j
              }
          }  // for i
      }
    return 0;
  }

//! @brief Assemblies into A the matrix being passed as parameter
//! multimplied by the fact parameter.
int XC::SpectraSOE::addA(const Matrix &a, const ID &id, double fact)
  { return addToMatrix(tripletListA,a,id,fact); }

//! @brief Zeroes the matrix A.
void XC::SpectraSOE::zeroA(void)
  { A.setZero(); }

//! @brief Assemblies into M the matrix being passed as parameter
//! multimplied by the fact parameter.
int XC::SpectraSOE::addM(const Matrix &m, const ID &id, double fact)
  { 
    int retval= 0;
    if(fact!=0.0)
      {
        const int idSize = id.Size();
        // check that m and id are of same size
        if(idSize != m.noRows() && idSize != m.noCols())
          {
            std::cerr <<  getClassName() << "::" << __FUNCTION__
		      << "; matrix and ID not of similar sizes\n";
            retval= -1;
          }
        else
          {
            int col= 0, row= 0;
            if(fact==1.0)
              {
                for(int i= 0;i<idSize;i++)
                  {
                    row= id(i);
                    if(row < size && row >= 0)
                      {
                        for(int j= 0;j<idSize;j++)
                          {
                            col= id(j);
                            if(col < size && col >= 0 && (m(i,j)!= 0.0))
                              tripletListM.push_back(T(row,col, m(i,j)));
                           }// for j
                      }
                  }// for i
              }
            else
              {
                for(int i= 0;i<idSize;i++)
                  {
                    row= id(i);
                    if(row < size && row >= 0)
                      {
                        for(int j= 0;j<idSize;j++)
                          {
                            col= id(j);
                            if(col < size && col >= 0 && (m(i,j)!= 0.0))
                              tripletListM.push_back(T(row,col,m(i,j)*fact));
                           }// for j
                      }
                  }// for i
               }
           }
      }
    return retval;
  }

//! @brief Dump the values stored in M into the massMatrix member of EigenSOE
//! so the values of participation factors etc. could be computed.
void XC::SpectraSOE::store_mass_matrix(void)
  {
    resize_mass_matrix_if_needed(this->size);
    for(int k=0; k<M.outerSize(); ++k)
      for(Eigen::SparseMatrix<double>::InnerIterator it(M,k); it; ++it)
        {
	  const double &v= it.value();
	  const int &i= it.row(); // row index
	  const int &j= it.col();   // col index (here it is equal to k)
	  //it.index(); // inner index, here it is equal to it.row()
	  massMatrix(i,j)= v; //Assing to the corresponding position in massMatrx.
      }
    //Release memory occupied by M.
    M.setZero();
    M.data().squeeze(); // release as much as memory as possible
  }

//! @brief Assemble the matrices.
void XC::SpectraSOE::assembleMatrices(void)
  {
    M.setZero();
    M.setFromTriplets(tripletListM.begin(), tripletListM.end()); // mass matrix is ready to go!
    A.setZero();
    A.setFromTriplets(tripletListA.begin(), tripletListA.end()); // stiffness matrix is ready to go!
  }
 
//! @brief Zeroes the matrix M.
void XC::SpectraSOE::zeroM(void)
  { 
    EigenSOE::zeroM();
    M.setZero();
  }

//! @brief Makes M the identity matrix (to find stiffness matrix eigenvalues).
void XC::SpectraSOE::identityM(void)
  {
    M.setIdentity();
  }


int XC::SpectraSOE::sendSelf(Communicator &comm)
  { return 0; }
    
int XC::SpectraSOE::recvSelf(const Communicator &comm)
  { return 0; }


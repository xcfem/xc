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
//BandArpackppSOE.cpp

#include <solution/system_of_eqn/eigenSOE/BandArpackppSOE.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackppSolver.h>
#include "solution/graph/graph/Graph.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::BandArpackppSOE::BandArpackppSOE(SolutionStrategy *owr,const double &shift)
  :ArpackSOE(owr,EigenSOE_TAGS_BandArpackppSOE,shift), A(), M() {}

//! @brief Sets the solver to use.
bool XC::BandArpackppSOE::setSolver(EigenSolver *newSolver)
  {
    bool retval= false;
    BandArpackppSolver *tmp= dynamic_cast<BandArpackppSolver *>(newSolver);
    if(tmp)
      {
        tmp->setEigenSOE(*this);
        retval= ArpackSOE::setSolver(tmp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; incompatible solver." << std::endl;
    return retval;
  }

//! @brief Sets the system size.
int XC::BandArpackppSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    const int newSize= checkSize(theGraph);
    if(size!=newSize)
      {
        size= newSize;
        A= msp_double(size,size);
        M= msp_double(size,size);
        resize_mass_matrix_if_needed(size);      
      }
    return result;
  }

//! @brief Assemblies into A the matrix being passed as parameter
//! multimplied by the fact parameter.
int XC::BandArpackppSOE::addToMatrix(msp_double &MT,const Matrix &m, const ID &id,const double &fact)
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
            const int row= id(i)+1;
            if(row <= size && row > 0)
              {
                for(int j= 0;j<idSize;j++)
                  {
	            const int col= id(j)+1;
		    const double value= m(i,j);
	            if(col <= size && col > 0 && (value!=0.0))
                      MT(row,col)+= value;
	          }// for j
              }
          }// for i
      }
    else if(fact == -1.0)
      { // we save the product.
        for(int i= 0;i<idSize;i++)
          {
            const int row= id(i)+1;
            if(row <= size && row > 0)
              {
                for(int j= 0;j<idSize;j++)
                  {
	            const int col= id(j)+1;
	            if(col <= size && col > 0 && (m(i,j)!= 0.0))
                      MT(row,col)-= m(i,j);
	          }// for j
              }
          }// for i
      }
    else
      {
        for(int i= 0;i<idSize;i++)
          {
            const int row = id(i)+1;
            if(row <= size && row > 0)
              {
                for(int j= 0;j<idSize;j++)
                  {
                    const int col = id(j)+1;
	            if(col <= size && col > 0 && (m(i,j)!= 0.0))
                      MT(row,col)+= m(i,j)*fact;
	          }  // for j
              }
          }  // for i
      }
    MT.Compacta();
    return 0;
  }

//! @brief Assemblies into A the matrix being passed as parameter
//! multimplied by the fact parameter.
int XC::BandArpackppSOE::addA(const Matrix &a, const ID &id, double fact)
  { return addToMatrix(A,a,id,fact); }

//! @brief Zeroes the matrix A.
void XC::BandArpackppSOE::zeroA(void)
  { A.Anula(); }

//! @brief Assemblies into M the matrix being passed as parameter
//! multimplied by the fact parameter.
int XC::BandArpackppSOE::addM(const Matrix &m, const ID &id, double fact)
  { 
    int retval= 0;
    //Added by LCPT.
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
            resize_mass_matrix_if_needed(size);      
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
                              massMatrix(row,col)+= m(i,j);
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
                              massMatrix(row,col)+= m(i,j)*fact;
                           }// for j
                      }
                  }// for i
               }
           }
      }
    //Added by LCPT ends.
    retval= addToMatrix(M,m,id,fact);
    return retval;
  }

//! @brief Zeroes the matrix M.
void XC::BandArpackppSOE::zeroM(void)
  { 
    EigenSOE::zeroM();
    M.Anula();
  }

//! @brief Makes M the identity matrix (to find stiffness matrix eigenvalues).
void XC::BandArpackppSOE::identityM(void)
  {
    EigenSOE::identityM();
    M.Identity();
  }


int XC::BandArpackppSOE::sendSelf(Communicator &comm)
  { return 0; }
    
int XC::BandArpackppSOE::recvSelf(const Communicator &comm)
  { return 0; }


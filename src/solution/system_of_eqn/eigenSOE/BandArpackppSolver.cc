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
//BandArpackppSolver.cpp

#include <solution/system_of_eqn/eigenSOE/BandArpackppSolver.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackppSOE.h>
#include "arpack++/arbgnsym.h"

//! @brief Constructor.
XC::BandArpackppSolver::BandArpackppSolver(void)
:EigenSolver(EigenSOLVER_TAGS_BandArpackppSolver),
 theSOE(nullptr), eigenvalues(1), eigenvectors(1,Vector()) {}

//! @brief Constructor.
XC::BandArpackppSolver::BandArpackppSolver(const int &nModes)
 :EigenSolver(EigenSOLVER_TAGS_BandArpackppSolver,nModes),
 theSOE(nullptr), eigenvalues(nModes), eigenvectors(nModes,Vector()) {}

ARbdNonSymMatrix<double, double> creaARbdNonSymMatrix(const msp_double &m, XC::Vector &v)
  {
    const size_t ndiagL= m.ndiagL();
    const size_t ndiagU= m.ndiagU();
    const size_t n_columns= m.getNumberOfColumns();
    assert(m.getNumberOfRows()==n_columns);
    const size_t sizeCol= (ndiagL+ndiagU+1);
    const size_t sizeV= sizeCol*n_columns;
    v.resize(sizeV);
    v.Zero();
    
    m.FillVectorBanda(v.getDataPtr());
    return ARbdNonSymMatrix<double, double>(n_columns,ndiagL,ndiagU,v.getDataPtr());
  }

void XC::BandArpackppSolver::setup_autos(const size_t &nmodes,const size_t &n)
  {
    if(eigenvalues.size()!=nmodes)
      eigenvalues.resize(nmodes);
    if(eigenvectors.size()!=nmodes)
      eigenvectors.resize(nmodes);
    for(size_t i=0;i<nmodes;i++)
      eigenvectors[i].resize(n);
  }

ARluNonSymGenEig<double> getEigenProblem(const int &nmodes,ARbdNonSymMatrix<double, double> &ak,ARbdNonSymMatrix<double, double> &am,const double &shift)
  {
    if(shift<0.0)
      return ARluNonSymGenEig<double>(nmodes, ak, am); //Greatest eigenvalues.
    else
      return ARluNonSymGenEig<double>(nmodes, ak, am,shift); //Eigenvalues nearest to shift.
  }

//! @brief Compute eigenvalues.
int XC::BandArpackppSolver::solve(void)
  {
    int retval= 0;
    if(!theSOE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; eigen SOE was not assigned yet.\n";
        retval= -1;
      }
    else
      {
        const int n= theSOE->size; // Number of equations
	if(which!="LM")
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; computation of: " << which
		    << " eigenvalues not implemented yet."
		    << std::endl;
    
        // Check for quick return
        if((numModes <= 1) || (numModes>=(n-1)))
          {
            numModes= 0;
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; the number of modes must be"
                      << " between: " << 2 << " and " << n-2 << ".\n";
            retval= -2;
          }	
        else
          {
            Vector vk;
            ARbdNonSymMatrix<double, double> ak=  creaARbdNonSymMatrix(theSOE->A,vk);
            Vector vm;
            ARbdNonSymMatrix<double, double> am=  creaARbdNonSymMatrix(theSOE->M,vm);
            ARluNonSymGenEig<double> dprob= getEigenProblem(numModes, ak, am,theSOE->shift);

            if(verbosity>5)
              dprob.Trace();

            // Finding eigenvalues and eigenvectors.
            dprob.FindEigenvectors();

            //dprob.NoTrace();
            // Store the solution.
            const size_t nconv= dprob.ConvergedEigenvalues();
            if(nconv>0)
              {
                setup_autos(nconv,n);
                for(size_t i=0; i<nconv; i++)
                  eigenvalues[i]= dprob.EigenvalueReal(i);

                for(size_t i=0; i<nconv; i++)
                  for(int j=0; j<n;j++)
                    eigenvectors[i](j)= dprob.RawEigenvector(i)[j];
               }
            else
              retval= -3;
          }
      }
    return retval;
  }

//! @brief Sets the eigenproblem to solve.
bool XC::BandArpackppSolver::setEigenSOE(EigenSOE *soe)
  {
    bool retval= false;
    BandArpackppSOE *tmp= dynamic_cast<BandArpackppSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "the system of equations has not"
	        << "a suitable type for this solver."
		<< std::endl;
    return retval;
  }

//! @brief Sets the eigenproblem to solve.
bool XC::BandArpackppSolver::setEigenSOE(BandArpackppSOE &theBandSOE)
  { return setEigenSOE(&theBandSOE); }

const XC::Vector &XC::BandArpackppSolver::getEigenvector(int mode) const
  {
    static Vector retval(1);
    if(mode < 1 || mode > numModes)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; mode " << mode << " is out of range (1 - "
	          << numModes << ")\n";
      }
    else
      retval= eigenvectors[mode-1];
    return retval;
  }

const double &XC::BandArpackppSolver::getEigenvalue(int mode) const
  {
    static double retval= 0.0;
    if(mode < 1 || mode > numModes)
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; -- mode " 
                  << mode << " is out of range (1 - " << numModes << ")\n";
    else
      retval= eigenvalues[mode-1];
    return retval;
  }

int XC::BandArpackppSolver::setSize(void)
  {
    setup_autos(numModes,theSOE->size);
    return 0;
  }

//! @brief Return the dimension of eigenvectors.
const int &XC::BandArpackppSolver::getSize(void) const
  { return theSOE->size; }

int XC::BandArpackppSolver::sendSelf(Communicator &comm)
  { return 0; }

int XC::BandArpackppSolver::recvSelf(const Communicator &comm)
  { return 0; }

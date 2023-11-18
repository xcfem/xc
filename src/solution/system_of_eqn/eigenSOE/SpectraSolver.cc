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
//SpectraSolver.cpp

#include <solution/system_of_eqn/eigenSOE/SpectraSolver.h>
#include <solution/system_of_eqn/eigenSOE/SpectraSOE.h>
#include <Spectra/SymGEigsShiftSolver.h>
#include <Spectra/MatOp/SymShiftInvert.h>
#include <Spectra/MatOp/SparseSymMatProd.h>

//! @brief Constructor.
XC::SpectraSolver::SpectraSolver(void)
:EigenSolver(EigenSOLVER_TAGS_SpectraSolver),
 theSOE(nullptr), eigenvalues(1), eigenvectors(1,Vector()) {}

//! @brief Constructor.
XC::SpectraSolver::SpectraSolver(const int &nModes)
 :EigenSolver(EigenSOLVER_TAGS_SpectraSolver,nModes),
 theSOE(nullptr), eigenvalues(nModes), eigenvectors(nModes,Vector()) {}

void XC::SpectraSolver::setup_autos(const size_t &nmodes,const size_t &n)
  {
    if(eigenvalues.size()!=nmodes)
      eigenvalues.resize(nmodes);
    if(eigenvectors.size()!=nmodes)
      eigenvectors.resize(nmodes);
    for(size_t i=0;i<nmodes;i++)
      eigenvectors[i].resize(n);
  }


//! @brief Compute eigenvalues.
int XC::SpectraSolver::solve(void)
  {
    int retval= 0;
    if(!theSOE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; eigen SOE was not assigned yet."
	          << std::endl;
        retval= -1;
      }
    else
      {
	if(!findSmallest)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; computation of largest eigenvalues not implemented yet."
		    << std::endl;
    
        const int n= theSOE->size; // Number of equations
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
   	    theSOE->assembleMatrices();
	    const Eigen::SparseMatrix<double> &A= theSOE->getA();
	    const Eigen::SparseMatrix<double> &M= theSOE->getM();

	    // Construct matrix operation objects using the wrapper classes
	    // both matrices are sparse.
	    using OpType= Spectra::SymShiftInvert<double, Eigen::Sparse, Eigen::Sparse>;
	    using BOpType= Spectra::SparseSymMatProd<double>;
	    OpType op(A, M);
	    BOpType Bop(M);
	    
	    // Construct generalized eigen solver object, seeking three
	    // generalized eigenvalues that are closest to zero. This is
	    // equivalent to specifying a shift sigma = 0.0 combined with
	    // the SortRule::LargestMagn selection rule
	    const int nRows=  A.rows();
	    const int ncv= std::min(2*numModes, nRows);
	    Spectra::SymGEigsShiftSolver<OpType, BOpType, Spectra::GEigsMode::ShiftInvert>
		geigs(op, Bop, numModes, ncv, 0.0);
	    
	    // Initialize and compute
	    geigs.init();
	    const int nconv= geigs.compute(Spectra::SortRule::LargestMagn);
	    
            // Store the solution.
            if(nconv>0 and (geigs.info() == Spectra::CompInfo::Successful))
              {
                setup_autos(nconv,n);
		const Eigen::VectorXd evalues= geigs.eigenvalues();
		Eigen::MatrixXd evecs= geigs.eigenvectors();
                for(int i=0; i<nconv; i++)
		  {
		    const int k= nconv-i-1; // reverse order.
                    this->eigenvalues[k]= evalues(i);
                    for(int j=0; j<n;j++)
                      this->eigenvectors[k](j)= evecs(j,i);
		  }
		theSOE->store_mass_matrix();
               }
            else
              retval= -3;
          }
      }
    return retval;
  }

//! @brief Sets the eigenproblem to solve.
bool XC::SpectraSolver::setEigenSOE(EigenSOE *soe)
  {
    bool retval= false;
    SpectraSOE *tmp= dynamic_cast<SpectraSOE *>(soe);
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
bool XC::SpectraSolver::setEigenSOE(SpectraSOE &theSOE)
  { return setEigenSOE(&theSOE); }

const XC::Vector &XC::SpectraSolver::getEigenvector(int mode) const
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

const double &XC::SpectraSolver::getEigenvalue(int mode) const
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

int XC::SpectraSolver::setSize(void)
  {
    setup_autos(numModes,theSOE->size);
    return 0;
  }

//! @brief Return the dimension of eigenvectors.
const int &XC::SpectraSolver::getSize(void) const
  { return theSOE->size; }

int XC::SpectraSolver::sendSelf(Communicator &comm)
  { return 0; }

int XC::SpectraSolver::recvSelf(const Communicator &comm)
  { return 0; }

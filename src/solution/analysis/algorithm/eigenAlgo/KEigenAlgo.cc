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
//KEigenAlgo.cpp

#include <solution/analysis/algorithm/eigenAlgo/KEigenAlgo.h>
#include <solution/analysis/integrator/eigen/KEigenIntegrator.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>

//! @brief Returns, if possible, a pointer to a suitable integrator.
XC::KEigenIntegrator *XC::KEigenAlgo::getKEigenIntegrator(void)
  {
    KEigenIntegrator *retval= nullptr;
    Integrator *theIntegrator= getIntegratorPtr();
    if(theIntegrator)
      {
        retval= dynamic_cast<KEigenIntegrator *>(theIntegrator);
        if(!retval)
	  std::cerr << getClassName() << "::" << __FUNCTION__
                    << " is not of the right type." << std::endl;
      }
    return retval;
  }

//! @brief Constructor.
XC::KEigenAlgo::KEigenAlgo(SolutionStrategy *owr)
  :EigenAlgorithm(owr,EigenALGORITHM_TAGS_KEigen), ns(0), nl(0), condNumberThreshold(1e5) {}

//! @brief Compute the smallest or largest eigenvalues.
int XC::KEigenAlgo::form_matrices(void)
  {
    AnalysisModel *theModel= getAnalysisModelPtr();
    KEigenIntegrator *theIntegrator= getKEigenIntegrator();
    EigenSOE *theSOE = getEigenSOEPtr();
    if((theModel == nullptr) || (theIntegrator == nullptr) || (theSOE == nullptr))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; domain, model or integrator not assigned.\n";
        return -1;
      }
    if(theIntegrator->formK() < 0) //Builds tangent stiffness matrix.
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the Integrator failed in formK()\n";
        return -2;
      }
    if(theIntegrator->formM()<0) //Builds form the M= I (identity) matrix.
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - the Integrator failed in formKtplusDt().\n";
        return -3;
      }
    return 0;
  }
//! @brief Compute the smallest or largest eigenvalues.
int XC::KEigenAlgo::compute_eigenvalues(int numEigen, const std::string &which)
  {
    EigenSOE *theSOE = getEigenSOEPtr();
    theSOE->save();
    EigenSolver *solver= theSOE->getSolver();
    solver->setWhichEigenvalues(which); //Which eigenvalues to compute.
    if(theSOE->solve(numEigen) < 0) //Computes numEigen eigenvalues.
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Warning - the EigenSOE failed in solve().\n";
        return -4;
      }
    if(theSOE->getNumModes()<numEigen)
      {
	std::clog << getClassName() << "::" << __FUNCTION__
		  << "; Warning - number of modes reduced from: "
	          << numEigen << " to " << theSOE->getNumModes()
	          << " due to the size of the system matrix."
	          << std::endl;
      }
    return 0;
  }

//! @brief Put the computed eigen values into the eigenvalues
//! member and the computed eigen vectors into the eigenvectors
//! member.
int XC::KEigenAlgo::dump_modes(void)
  {
    EigenSOE *theSOE = getEigenSOEPtr();
    const int numModes= theSOE->getNumModes();
    for(int i= 1;i<=numModes;i++)
      {
	const double ev= theSOE->getEigenvalue(i);
	const double denom= 1.0-ev;
	if(denom!=0.0)
	  eigenvalues.push_back(1.0/denom);
	else
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; theSOE.eigenvalue(" << i << ")= "
		      << ev << std::endl;
	    eigenvalues.push_back(1e99);
	  }
	eigenvectors.push_back(theSOE->getEigenvector(i));
      }
    return 0;
  }

//! @brief Compute the ns smallest eigenvalues.
int XC::KEigenAlgo::compute_smallest_eigenvalues(void)
  {
    EigenSOE *theSOE = getEigenSOEPtr();
    theSOE->save();
    if(ns>0)
      {
	if(compute_eigenvalues(ns,"LM") == 0) // YES LM.
	  dump_modes();
      }
    theSOE->restore();
    return 0;
  }

//! @brief Compute the nl largest eigenvalues.
int XC::KEigenAlgo::compute_largest_eigenvalues(void)
  {
    EigenSOE *theSOE = getEigenSOEPtr();
    theSOE->save();
    if(nl>0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; computation of largest eigenvalues"
                  << " not implemented yet." << std::endl;
    	// if(compute_eigenvalues(nl,"SM") == 0) // YES SM.
    	//   dump_modes();
      }
    theSOE->restore();
    return 0;
  }

//! @brief Solves the current step.
//!
//! In this particular case numModes is ignored.
int XC::KEigenAlgo::solveCurrentStep(int numModes)
  {
    const int fm= form_matrices();
    if(fm<0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error in matrix assembly.\n";
        return fm;
      }
    EigenSOE *theSOE = getEigenSOEPtr(); 
    rcond= theSOE->getRCond();
    if(rcond<1.0/condNumberThreshold)
      {
	// std::cerr << getClassName() << "::" << __FUNCTION__
	// 	  << "; ill conditioned system RCOND= "
	// 	  << rcond << std::endl;
	if(ns==0)
	  ns= numModes;
        compute_largest_eigenvalues();
        compute_smallest_eigenvalues();
	eigen_to_model();
      }
    else
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; condition number: " << (1.0/rcond)
	        << " is good enough. No eigenvalues computed."
	        << std::endl;
    return 0;
  }

//! @brief Dump the eigenvalues into the model (see Finite Element
//! Procedures. Klaus Jurgen Bathe page 632).
void XC::KEigenAlgo::eigen_to_model(void)
  {
    const size_t numModes= eigenvalues.size();
    if(numModes)
      {
	AnalysisModel *theModel= getAnalysisModelPtr();
	theModel->setNumEigenvectors(numModes);
	Vector theEigenvalues(numModes);
	for(size_t i= 0;i<numModes;i++)
	  {
	    theEigenvalues[i]= eigenvalues[i];
	    theModel->setEigenvector(i+1, eigenvectors[i]);
	  }
	theModel->setEigenvalues(theEigenvalues);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no eigenvalues to transfer."
	        << std::endl;
   }

//! @brief Print the object
void XC::KEigenAlgo::Print(std::ostream &s, int flag) const
  { s << "\t ill-conditioning algorithm \n"; }

//! @brief Send.
int XC::KEigenAlgo::sendSelf(Communicator &comm)
  { return 0; }

//! @brief Receive.
int XC::KEigenAlgo::recvSelf(const Communicator &comm)
  { return 0; }


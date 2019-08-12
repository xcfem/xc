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
XC::KEigenAlgo::KEigenAlgo(AnalysisAggregation *owr)
  :EigenAlgorithm(owr,EigenALGORITHM_TAGS_KEigen) {}

//! @brief Solves the current step.
int XC::KEigenAlgo::solveCurrentStep(int numModes)
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

    if(theIntegrator->formM()<0) //Builds form the M= I (identity) matrix.
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - the Integrator failed in formKtplusDt().\n";
        return -3;
      }

    if(theSOE->solve(numModes) < 0) //Computes eigenmodes.
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Warning - the EigenSOE failed in solve().\n";
        return -4;
      }

    eigen_to_model(numModes); //Send eigenvectors (modes) and eigenvalues to the model.
    return 0;
  }

//! @brief Dump the eigenvalues into the model (see Finite Element Procedures. Klaus Jurgen Bathe page 632).
void XC::KEigenAlgo::eigen_to_model(int numModes)
  {
    AnalysisModel *theModel= getAnalysisModelPtr();
    theModel->setNumEigenvectors(numModes);
    Vector theEigenvalues(numModes);
    EigenSOE *theSOE = getEigenSOEPtr();
    for(int i= 1;i<=numModes;i++)
      {
        const double ev= theSOE->getEigenvalue(i);
        const double denom= 1.0-ev;
        if(denom!=0.0)
          theEigenvalues[i-1]= 1.0/denom;
        else
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; theSOE.eigenvalue(" << i << ")= "
                      << ev << std::endl;
	    theEigenvalues[i-1]= 1e99;
          }
        theModel->setEigenvector(i, theSOE->getEigenvector(i));
      }
    theModel->setEigenvalues(theEigenvalues);
  }

//! @brief Print the object
void XC::KEigenAlgo::Print(std::ostream &s, int flag)
  { s << "\t ill-conditioning algorithm \n"; }

//! @brief Send.
int XC::KEigenAlgo::sendSelf(CommParameters &cp)
  { return 0; }

//! @brief Receive.
int XC::KEigenAlgo::recvSelf(const CommParameters &cp)
  { return 0; }


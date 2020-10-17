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
// $Date: 2005/08/31 17:39:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/EigenAnalysis.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/analysis/eigenAnalysis/EigenAnalysis.C
//
// Written: Jun Peng
// Created: Wed Jan 27, 1999
// Revision: A
//
// Description: This file contains the class definition of XC::EigenAnalysis.
// EigenAnalysis is a subclass of XC::Analysis, it is used to perform the 
// eigen value analysis on the FE_Model.
//
// This class is inheritanted from the base class of XC::Analysis
// which was created by fmk (Frank).


#include <solution/analysis/analysis/EigenAnalysis.h>
#include <solution/analysis/algorithm/eigenAlgo/EigenAlgorithm.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <solution/analysis/numberer/DOF_Numberer.h>
#include <solution/analysis/handler/ConstraintHandler.h>
#include <solution/analysis/integrator/EigenIntegrator.h>
#include <domain/domain/Domain.h>
#include "solution/analysis/ModelWrapper.h"
#include "solution/SolutionStrategy.h"



#include "utility/matrix/Matrix.h"

//! @brief Constructor.
XC::EigenAnalysis::EigenAnalysis(SolutionStrategy *analysis_aggregation)
  :Analysis(analysis_aggregation), domainStamp(0) {}

//! @brief Virtual constructor.
XC::Analysis *XC::EigenAnalysis::getCopy(void) const
  { return new EigenAnalysis(*this); }

//! @brief Clears all object members (constraint handler, analysis model,...).
void XC::EigenAnalysis::clearAll(void)
  {
    // invoke the destructor on all the objects in the aggregation
    Analysis::clearAll();
    delete getEigenSolutionAlgorithmPtr();
  }

//! @brief Performs the analysis.
//!
//! @param numModes: number of modes to compute.
int XC::EigenAnalysis::analyze(int numModes)
  {
    int result= 0;
    assert(solution_strategy);
    CommandEntity *old= solution_strategy->Owner();
    solution_strategy->set_owner(this);

    // check for change in Domain since last step. As a change can
    // occur in a commit() in a domaindecomp with load balancing
    // this must now be inside the loop
    int stamp = getDomainPtr()->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp = stamp;
	result = domainChanged();
	if(result < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; domainChanged failed\n";
	    return -1;
	  }
      }
    result = getEigenIntegratorPtr()->newStep();
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; integrator failed\n";
	return -2;
      }
    result = getEigenSolutionAlgorithmPtr()->solveCurrentStep(numModes);
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; algorithm failed\n";
	return -3;
      }
    solution_strategy->set_owner(old);
    return 0;
  }

//! @brief Make the changes derived of a change in the domain.
int XC::EigenAnalysis::domainChanged(void)
  {
    getAnalysisModelPtr()->clearAll();    
    getConstraintHandlerPtr()->clearAll();      
    int result= getConstraintHandlerPtr()->handle();
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__ 
		  << "; ConstraintHandler::handle() failed." << std::endl;
        return -1;
      }

    //Set equation numbers.
    result= getDOF_NumbererPtr()->numberDOF();
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in equation numbering." << std::endl;
        return -2;
      }
    result= getConstraintHandlerPtr()->doneNumberingDOF();
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in doneNumberingDOF()." << std::endl;
        return -3;
      }

    Graph &theGraph = getAnalysisModelPtr()->getDOFGraph();
    result= getEigenSOEPtr()->setSize(theGraph);
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; error when setting system size." << std::endl;
        return -4;
      }

    result= getEigenIntegratorPtr()->domainChanged();
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; error in Integrator::domainChanged()." << std::endl;
        return -5;
      }
    result= getEigenSolutionAlgorithmPtr()->domainChanged();
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; error in Algorithm::domainChanged()." << std::endl;
        return -5;
      }
    return 0;
  }

//! @brief Sets the algorithm to use in the analysis.
int XC::EigenAnalysis::setAlgorithm(EigenAlgorithm &theAlgo)
  {
    Analysis::setAlgorithm(theAlgo);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; does nothing yet\n";
    return 0;
  }

//! @brief Sets the integrator to use in the analysis.
int XC::EigenAnalysis::setIntegrator(EigenIntegrator &theIntegrator)
  {
    Analysis::setIntegrator(theIntegrator);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; does nothing yet\n";    
    return 0;
  }

//! @brief Sets the system of eigenvalues to use in the analysis.
int XC::EigenAnalysis::setEigenSOE(EigenSOE &theSOE)
  {
    Analysis::setEigenSOE(theSOE);
    return 0;
  }

//! @brief Returns the autovector that corresponds to the mode
//! being passed as parameter.
const XC::Vector &XC::EigenAnalysis::getEigenvector(int mode) const
  {
    static Vector retval(1);
    retval.Zero();
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getEigenvector(mode);
    return retval;
  }

//! @brief Returns the normalized autovector that correspond to the mode being
//! passed as parameter. The vector is normalized to make the maximum of the
//! absolute values of its components to be 1 (infinity_norm(v)= 1).
XC::Vector XC::EigenAnalysis::getNormalizedEigenvector(int mode) const
  {
    static Vector retval(1);
    retval.Zero();
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getNormalizedEigenvector(mode);
    return retval;
  }

//! @brief Returns a matrix with the computed eigenvectors as
//! columns of the matrix.
XC::Matrix XC::EigenAnalysis::getEigenvectors(void) const
  {
    Matrix retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getEigenvectors();
    return retval;
  }

//! @brief Returns a matrix with the normalized eigenvectors
//! (infinity norm) as columns of the matrix.
XC::Matrix XC::EigenAnalysis::getNormalizedEigenvectors(void) const
  {
    Matrix retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getNormalizedEigenvectors();
    return retval;
  }

//! @brief Returns the eigenvalue that corresponds to the mode
//! being passed as parameter.
const double &XC::EigenAnalysis::getEigenvalue(int mode) const
  {
    static double retval= 0.0;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getEigenvalue(mode);
    return retval;
  }

//! @brief Return the angular frequency for the i-th mode.
double XC::EigenAnalysis::getAngularFrequency(int i) const
  { return sqrt(getEigenvalue(i)); }

//! @brief Returns the period for the i-th mode.
double XC::EigenAnalysis::getPeriod(int i) const
  { return 2.0*M_PI/getAngularFrequency(i); }

//! @brief Return the frequency for the i-th mode.
double XC::EigenAnalysis::getFrequency(int i) const
  { return 1./getPeriod(i); }

//! @brief Returns a vector with the computed eigenvalues for each mode.
XC::Vector XC::EigenAnalysis::getEigenvalues(void) const
  {
    Vector retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getEigenvalues();
    return retval;
  }

//! @brief Returns a vector with the computed angular frequencies for each mode.
XC::Vector XC::EigenAnalysis::getAngularFrequencies(void) const
  {
    Vector retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getAngularFrequencies();
    return retval;
  }

//! @brief Returns a vector with the computed vectors for each mode.
XC::Vector XC::EigenAnalysis::getPeriods(void) const
  {
    Vector retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getPeriods();
    return retval;
  }

//! @brief Returns a vector with the computed frequencies.
XC::Vector XC::EigenAnalysis::getFrequencies(void) const
  {
    Vector retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getFrequencies();
    return retval;
  }

//! @brief Returns the number of computed eigenvalues.
int XC::EigenAnalysis::getNumModes(void) const
  {
    int retval= 0;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getNumModes();
    return retval;
  }

//! @brief Returns the modal participation factor corresponding to i mode.
double XC::EigenAnalysis::getModalParticipationFactor(int mode) const
  {
    double retval= 0.0;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getModalParticipationFactor(mode);
    else
      std::cerr << "EigenAnalysis::getModalParticipationFactor( " << mode
                << "); the pointer al system of equations es nulo." << std::endl;
    return retval;
  }

//! @brief Returns the modal participation factors.
XC::Vector XC::EigenAnalysis::getModalParticipationFactors(void) const
  {
    Vector retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getModalParticipationFactors();
    else
      std::cerr << "EigenAnalysis::getModalParticipationFactors; "
                << "the pointer al system of equations es nulo." << std::endl;
    return retval;
  }
//! @brief Returns the distribution factor correspondint to the mode
//! being passed as parameter.
XC::Vector XC::EigenAnalysis::getDistributionFactor(int mode) const
  {
    Vector retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getDistributionFactor(mode);
    return retval;
  }

//! @brief Returns a matrix with the distribution factors
//! by columns.
XC::Matrix XC::EigenAnalysis::getDistributionFactors(void) const
  {
    Matrix retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getDistributionFactors();
    return retval;
  }

//! @brief Return the masa modal efectiva 
//! correspondiente al modo i.
double XC::EigenAnalysis::getEffectiveModalMass(int mode) const
  {
    double retval= 0.0;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getEffectiveModalMass(mode);
    return retval;
  }

//! @brief Returns the masas modales efectivas.
XC::Vector XC::EigenAnalysis::getEffectiveModalMasses(void) const
  {
    Vector retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getEffectiveModalMasses();
    return retval;
  }

//! @brief Return the masa total del modelo.
double XC::EigenAnalysis::getTotalMass(void) const
  {
    double retval= 0.0;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getTotalMass();
    return retval;
  }

//! @brief Returns the equivalent static load for the
//! mode being passed as parameter.
XC::Vector XC::EigenAnalysis::getEquivalentStaticLoad(int mode,const double &accel_mode) const
  {
    Vector retval;
    EigenSOE *ptr_soe= getEigenSOEPtr();
    if(ptr_soe)
      retval= ptr_soe->getEquivalentStaticLoad(mode,accel_mode);
    return retval;
  }


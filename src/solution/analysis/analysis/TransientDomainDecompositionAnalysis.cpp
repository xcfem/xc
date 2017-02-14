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
// $Date: 2005/12/12 19:22:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/TransientDomainDecompositionAnalysis.cpp,v $
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation of XC::TransientDomainDecompositionAnalysis.
//
// What: "@(#) TransientDomainDecompositionAnalysis.C, revA"

#include <solution/analysis/analysis/TransientDomainDecompositionAnalysis.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
#include <solution/analysis/numberer/DOF_Numberer.h>
#include <solution/analysis/handler/ConstraintHandler.h>
#include <solution/analysis/integrator/TransientIntegrator.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include "domain/domain/subdomain/Subdomain.h"
#include "solution/SoluMethod.h"

// AddingSensitivity:BEGIN //////////////////////////////////
#ifdef _RELIABILITY
#include <reliability/FEsensitivity/SensitivityAlgorithm.h>
#endif
// AddingSensitivity:END ////////////////////////////////////

#include <utility/matrix/Matrix.h>
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::TransientDomainDecompositionAnalysis::TransientDomainDecompositionAnalysis(Subdomain &the_Domain,SoluMethod *s)
  :DomainDecompositionAnalysis(ANALYSIS_TAGS_TransientDomainDecompositionAnalysis, the_Domain,s)
  {}

//! brief Destructor.
XC::TransientDomainDecompositionAnalysis::~TransientDomainDecompositionAnalysis()
  {
    // we don't invoke the destructors in case user switching
    // from a static to a direct integration analysis 
    // clearAll() must be invoked if user wishes to invoke destructor
  }    

//! Borra todos los miembros del objeto (Manejador coacciones, analysis model,...).
void XC::TransientDomainDecompositionAnalysis::clearAll(void)
  {
    // invoke the destructor on all the objects in the aggregation
    Analysis::clearAll();
  }


bool XC::TransientDomainDecompositionAnalysis::doesIndependentAnalysis(void)
  { return true; }

//! @brief Performs the analysis.
int XC::TransientDomainDecompositionAnalysis::analyze(double dT)
  {
    assert(metodo_solu);
    EntCmd *old= metodo_solu->Owner();
    metodo_solu->set_owner(this);
    int result = 0;
    Domain *the_Domain = this->getDomainPtr();

    // check for change in XC::Domain since last step. As a change can
    // occur in a commit() in a domaindecomp with load balancing
    // this must now be inside the loop
    int stamp = the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
        domainStamp = stamp;
        result= this->domainChanged();
        if(result < 0)
          {
            std::cerr << "XC::TransientDomainDecompositionAnalysis::analyze() - domainChanged failed";
            return -1;
          }	
      }

    // result =newStepDomain(theAnalysisModel);
    if(result < 0)
      {
        std::cerr << "XC::TransientDomainDecompositionAnalysis::analyze() - the XC::AnalysisModel failed";
        std::cerr << " with domain at load factor ";
        std::cerr << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
        the_Domain->revertToLastCommit();
        return -2;
      }

    result = getTransientIntegratorPtr()->newStep(dT);
    if(result < 0)
      {
        std::cerr << "XC::TransientDomainDecompositionAnalysis::analyze() - the XC::Integrator failed";
        std::cerr << " with domain at load factor ";
        std::cerr << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
        the_Domain->revertToLastCommit();
        return -2;
      }

    result = getEquiSolutionAlgorithmPtr()->solveCurrentStep();
    if(result < 0)
      {
        std::cerr << "XC::TransientDomainDecompositionAnalysis::analyze() - the Algorithm failed";
        std::cerr << " with domain at load factor ";
        std::cerr << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
        the_Domain->revertToLastCommit();	    
        getTransientIntegratorPtr()->revertToLastStep();
        return -3;
      }   

    result = getTransientIntegratorPtr()->commit();
    if(result < 0)
      {
        std::cerr << "TransientDomainDecompositionAnalysis::analyze() - ";
        std::cerr << "the Integrator failed to commit";
        std::cerr << " with domain at load factor ";
        std::cerr << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
        the_Domain->revertToLastCommit();	    
        getTransientIntegratorPtr()->revertToLastStep();
        return -4;
      }    	
    metodo_solu->set_owner(old);
    return 0;
  }


int XC::TransientDomainDecompositionAnalysis::initialize(void)
  {
    Domain *the_Domain = this->getDomainPtr();
    
    // check if domain has undergone change
    int stamp = the_Domain->hasDomainChanged();
    if (stamp != domainStamp) {
      domainStamp = stamp;	
      if (this->domainChanged() < 0) {
	std::cerr << "XC::DirectIntegrationAnalysis::initialize() - domainChanged() failed\n";
	return -1;
      }	
    }
    if (getTransientIntegratorPtr()->initialize() < 0) {
	std::cerr << "XC::DirectIntegrationAnalysis::initialize() - integrator initialize() failed\n";
	return -2;
    } else
      getTransientIntegratorPtr()->commit();
    
    return 0;
  }

//! @brief Hace los cambios que sean necesarios como consecuencia de un cambio en el domain.
int XC::TransientDomainDecompositionAnalysis::domainChanged(void)
  {

  int result = 0;
  
  getAnalysisModelPtr()->clearAll();    
  getConstraintHandlerPtr()->clearAll();

  // now we invoke handle() on the constraint handler which
  // causes the creation of XC::FE_Element and XC::DOF_Group objects
  // and their addition to the XC::AnalysisModel.
  
  result = getConstraintHandlerPtr()->handle();
  if (result < 0) {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::handle() - ";
    std::cerr << "XC::ConstraintHandler::handle() failed";
    return -1;
  }	
  
  // we now invoke number() on the numberer which causes
  // equation numbers to be assigned to all the DOFs in the
  // AnalysisModel.

  result = getDOF_NumbererPtr()->numberDOF();
  if (result < 0) {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::handle() - ";
    std::cerr << "XC::DOF_Numberer::numberDOF() failed";
    return -2;
  }	    
  
  // we invoke setSize() on the XC::LinearSOE which
  // causes that object to determine its size
  Graph &theGraph = getAnalysisModelPtr()->getDOFGraph();
  result = getLinearSOEPtr()->setSize(theGraph);
  if (result < 0) {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::handle() - ";
    std::cerr << "XC::LinearSOE::setSize() failed";
    return -3;
  }	    

  // finally we invoke domainChanged on the XC::Integrator and Algorithm
  // objects .. informing them that the model has changed
  
  result = getTransientIntegratorPtr()->domainChanged();
  if (result < 0) {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::setAlgorithm() - ";
    std::cerr << "XC::Integrator::domainChanged() failed";
    return -4;
  }	    

  result = getEquiSolutionAlgorithmPtr()->domainChanged();
  if (result < 0) {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::setAlgorithm() - ";
    std::cerr << "XC::Algorithm::domainChanged() failed";
    return -5;
  }	        

  // if get here successfull
  return 0;
}    

//! @brief Returns the number of external equations.
int XC::TransientDomainDecompositionAnalysis::getNumExternalEqn(void)
  {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::getNumExternalEqn() - should never be called\n";
    return 0;
  }

//! @brief Returns the number of equations internas.
int XC::TransientDomainDecompositionAnalysis::getNumInternalEqn(void)
  {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::getNumInternalEqn() - should never be called\n";
    return 0;
  }


int XC::TransientDomainDecompositionAnalysis::newStep(double dT) 
  {
    this->analyze(dT);
    return 0;
  }

int XC::TransientDomainDecompositionAnalysis::computeInternalResponse(void)
  {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::computeInternalResponse() - should never be called\n";
    return 0;
  }

//! @brief Forma la tangent stiffness matrix.
int XC::TransientDomainDecompositionAnalysis::formTangent(void)
  {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::formTangent() - should never be called\n";
    return 0;
  }

//! @brief Forma el vector residuo.
int XC::TransientDomainDecompositionAnalysis::formResidual(void)
  {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::formResidual() - should never be called\n";
    return 0;
  }

//! @brief ??
int XC::TransientDomainDecompositionAnalysis::formTangVectProduct(Vector &force)
  {
    std::cerr << "XC::TransientDomainDecompositionAnalysis::formTangVectProduct() - should never be called\n";
    return 0;
  }

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::TransientDomainDecompositionAnalysis::getTangent(void)
  {
    static XC::Matrix errMatrix;
    std::cerr << "XC::TransientDomainDecompositionAnalysis::getTangent() - should never be called\n";
    return errMatrix;
  }

//! @brief Returns the vector residuo.
const XC::Vector &XC::TransientDomainDecompositionAnalysis::getResidual(void)
  {
    static XC::Vector errVector;
    std::cerr << "XC::TransientDomainDecompositionAnalysis::getResidual() - should never be called\n";
    return errVector;
  }

//! @brief ??
const XC::Vector &XC::TransientDomainDecompositionAnalysis::getTangVectProduct(void)
  {
    static XC::Vector errVector;
    std::cerr << "XC::TransientDomainDecompositionAnalysis::getTangVectProduct() - should never be called\n";
    return errVector;
  }
  
//! @brief Sends object through the channel being passed as parameter.  
int XC::TransientDomainDecompositionAnalysis::sendSelf(CommParameters &cp)
  {

    // receive the data identifyng the objects in the aggregation
    this->getDbTag();
    static ID data(8);

    if(getEquiSolutionAlgorithmPtr() == 0)
      {
        std::cerr << "XC::TransientDomainDecompositionAnalysis::sendSelf() - no objects exist!\n";
        return -1;
      }

    LinearSOESolver *theSolver = getLinearSOEPtr()->getSolver();
  
    data(0)= getConstraintHandlerPtr()->getClassTag();
    data(1)= getDOF_NumbererPtr()->getClassTag();
    data(2)= getAnalysisModelPtr()->getClassTag();
    data(3)= getEquiSolutionAlgorithmPtr()->getClassTag();
    data(4)= getLinearSOEPtr()->getClassTag();
    data(5)= theSolver->getClassTag();
    data(6)= getTransientIntegratorPtr()->getClassTag();

    if(metodo_solu->getConvergenceTestPtr() != 0)
      data(7) = metodo_solu->getConvergenceTestPtr()->getClassTag();
    else
      data(7) = -1;

    cp.sendID(data,getDbTagData(),CommMetaData(0));  

    // invoke sendSelf() on all the objects
    if(getConstraintHandlerPtr()->sendSelf(cp) != 0)
      {
        std::cerr << "XC::TransientDomainDecompositionAnalysis::sendSelf() - failed to send handler\n";
        return -1;
      }

    if(getDOF_NumbererPtr()->sendSelf(cp) != 0)
      {
        std::cerr << "XC::TransientDomainDecompositionAnalysis::sendSelf() - failed to send numberer\n";
        return -1;
      }

    if(getAnalysisModelPtr()->sendSelf(cp) != 0)
      {
        std::cerr << "XC::TransientDomainDecompositionAnalysis::sendSelf() - failed to send model\n";
        return -1;
      }

    if(getEquiSolutionAlgorithmPtr()->sendSelf(cp) != 0)
      {
        std::cerr << "XC::TransientDomainDecompositionAnalysis::sendSelf() - failed to send algorithm\n";
        return -1;
      }

    if(getLinearSOEPtr()->sendSelf(cp) != 0)
      {
       std::cerr << "XC::TransientDomainDecompositionAnalysis::sendSelf() - failed to send SOE\n";
       return -1;
      }
    else 
      ;

    //    theSOE->setAnalysisModel(*theAnalysisModel);

    if(theSolver->sendSelf(cp) != 0)
      {
        std::cerr << "XC::TransientDomainDecompositionAnalysis::sendSelf() - failed to send XC::Solver\n";
        return -1;
      }

    if(getTransientIntegratorPtr()->sendSelf(cp) != 0)
      {
        std::cerr << "XC::TransientDomainDecompositionAnalysis::sendSelf() - failed to send integrator\n";
        return -1;
      }

    if(metodo_solu->getConvergenceTestPtr() != 0)
      if(metodo_solu->getConvergenceTestPtr()->sendSelf(cp) != 0)
        {
          std::cerr << "XC::TransientDomainDecompositionAnalysis::sendSelf() - failed to send integrator\n";
          return -1;
        }
    return 0;
  }

int XC::TransientDomainDecompositionAnalysis::recvSelf(const CommParameters &cp)
  {

    //Domain *the_Domain = this->getSubdomainPtr();

    // receive the data identifyng the objects in the aggregation
    static ID data(8);
    this->getDbTag();
    cp.receiveID(data,getDbTagData(),CommMetaData(0));

    // for all objects in the aggregation:
    //  1. make sure objects exist & are of correct type; create new_ objects if not
    //  2. invoke recvSelf on the object
    if(getConstraintHandlerPtr() == 0 || getConstraintHandlerPtr()->getClassTag() != data(0))
      {
        if(getConstraintHandlerPtr() != 0)
          delete getConstraintHandlerPtr();
    
        brokeConstraintHandler(cp,data);
        if(getConstraintHandlerPtr() == 0)
          {
            std::cerr << "XC::TransientDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the XC::ConstraintHandler\n";
            return -1;
          }
      }
    getConstraintHandlerPtr()->recvSelf(cp);
  
    if(getDOF_NumbererPtr() == 0 || getDOF_NumbererPtr()->getClassTag() != data(1))
      {
        if(getDOF_NumbererPtr()) delete getDOF_NumbererPtr();

        brokeNumberer(cp,data);
        if(getDOF_NumbererPtr() == 0)
          {
            std::cerr << "XC::TransientDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the XC::ConstraintHandler\n";
            return -1;
          }
      }
    getDOF_NumbererPtr()->recvSelf(cp);

    if(getAnalysisModelPtr() == 0 || getAnalysisModelPtr()->getClassTag() != data(2))
      {
        if(getAnalysisModelPtr() != 0)
          delete getAnalysisModelPtr();
    
        brokeAnalysisModel(cp,data);
        if(getAnalysisModelPtr() == 0)
          {
            std::cerr << "XC::TransientDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the XC::Analysis Model\n";
            return -1;
          }
      }
    getAnalysisModelPtr()->recvSelf(cp);

    if(getEquiSolutionAlgorithmPtr() == 0 || getEquiSolutionAlgorithmPtr()->getClassTag() != data(3))
      {
        if(getEquiSolutionAlgorithmPtr() != 0)
          delete getEquiSolutionAlgorithmPtr();
    
        brokeEquiSolnAlgo(cp,data);
        if(getEquiSolutionAlgorithmPtr() == 0)
          {
            std::cerr << "XC::TransientDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the Solution Algorithm\n";
            return -1;
          }
      }
    getEquiSolutionAlgorithmPtr()->recvSelf(cp);

    if(getLinearSOEPtr() == 0 || getLinearSOEPtr()->getClassTag() != data(4))
      {
        if(getLinearSOEPtr() != 0) delete getLinearSOEPtr();
    
        brokeLinearSOE(cp,data);
        if(getLinearSOEPtr() == 0)
          {
            std::cerr << "XC::TransientDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the XC::LinearSOE\n";
            return -1;
          }
      }

    getLinearSOEPtr()->recvSelf(cp);
    LinearSOESolver *theSolver = getLinearSOEPtr()->getSolver();
    theSolver->recvSelf(cp);  
    //  theSOE->setAnalysisModel(*theAnalysisModel);

    if(getTransientIntegratorPtr() == 0 || getTransientIntegratorPtr()->getClassTag() != data(6))
      {
        if(getTransientIntegratorPtr())
          delete getTransientIntegratorPtr();
    
        brokeTransientIntegrator(cp,data);
        if(getTransientIntegratorPtr() == 0)
          {
            std::cerr << "TransientDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the XC::Integrator\n";
            return -1;
          }
      }

    getTransientIntegratorPtr()->recvSelf(cp);


    if(getConvergenceTestPtr() == 0 || getConvergenceTestPtr()->getClassTag() != data(7))
      {
	std::cerr << "TransientDomainDecompositionAnalysis; falta implementar la lectura del convergence test." << std::endl;
//         if(metodo_solu->getConvergenceTestPtr() != 0)
//           delete metodo_solu->getConvergenceTestPtr();
    
//         if(data(7) != -1)
//           {
//   	    ConvergenceTest *tmp= cp.getNewConvergenceTest(this,data(7));
//             if(!tmp)
//               {
// 	        std::cerr << "XC::TransientDomainDecompositionAnalysis::recvSelf";
// 	        std::cerr << " - failed to get the XC::ConvergenceTest\n";
// 	        return -1;
//               }
//             else
//               metodo_solu->setConvergenceTest(*tmp);
//           }
      }
    if(getConvergenceTestPtr() != 0)
      getConvergenceTestPtr()->recvSelf(cp);

    // set up the links needed by the elements in the aggregation
    set_all_links();
    return 0;
  }

//! @brief Sets the solution algorithm que se empleará en the analysis.
int XC::TransientDomainDecompositionAnalysis::setAlgorithm(EquiSolnAlgo &theNewAlgorithm)
  {
    DomainDecompositionAnalysis::setAlgorithm(theNewAlgorithm);

    // invoke domainChanged() either indirectly or directly
    domainStamp = 0;
    return 0;
  }

//! @brief Sets the integrator que se empleará en the analysis.
int XC::TransientDomainDecompositionAnalysis::setIntegrator(IncrementalIntegrator &theNewIntegrator) 
  {
    DomainDecompositionAnalysis::setIntegrator(theNewIntegrator);
    // cause domainChanged to be invoked on next analyze
    domainStamp = 0;
    return 0;
  }

//! @brief Sets the linear system of equations que se empleará en the analysis.
int XC::TransientDomainDecompositionAnalysis::setLinearSOE(LinearSOE &theNewSOE)
  {
    DomainDecompositionAnalysis::setLinearSOE(theNewSOE);
    // cause domainChanged to be invoked on next analyze
    domainStamp = 0;
    return 0;
  }



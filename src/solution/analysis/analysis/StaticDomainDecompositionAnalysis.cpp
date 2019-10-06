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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/StaticDomainDecompositionAnalysis.cpp,v $
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation of XC::StaticDomainDecompositionAnalysis.
//
// What: "@(#) StaticDomainDecompositionAnalysis.C, revA"

#include <solution/analysis/analysis/StaticDomainDecompositionAnalysis.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
#include <solution/analysis/numberer/DOF_Numberer.h>
#include <solution/analysis/handler/ConstraintHandler.h>
#include <solution/analysis/integrator/StaticIntegrator.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include "domain/domain/subdomain/Subdomain.h"
#include "solution/AnalysisAggregation.h"

// AddingSensitivity:BEGIN //////////////////////////////////
#ifdef _RELIABILITY
#include <reliability/FEsensitivity/SensitivityAlgorithm.h>
#endif
// AddingSensitivity:END ////////////////////////////////////

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::StaticDomainDecompositionAnalysis::StaticDomainDecompositionAnalysis(Subdomain &the_Domain,AnalysisAggregation *s)
  :DomainDecompositionAnalysis(ANALYSIS_TAGS_StaticDomainDecompositionAnalysis, the_Domain) {}    

//! @brief Destructor.
XC::StaticDomainDecompositionAnalysis::~StaticDomainDecompositionAnalysis(void)
  {
    // we don't invoke the destructors in case user switching
    // from a static to a direct integration analysis 
    // clearAll() must be invoked if user wishes to invoke destructor
  }    

//! @brief Clears object members (Constraint handler, analysis model,...).
void XC::StaticDomainDecompositionAnalysis::clearAll(void)
  {
    // invoke the destructor on all the objects in the aggregation
    Analysis::clearAll();
  }


bool XC::StaticDomainDecompositionAnalysis::doesIndependentAnalysis(void)
  { return true; }


//! @brief Performs the analysis.
//!
//! @param dT: time increment.
int XC::StaticDomainDecompositionAnalysis::analyze(double dT)
  {
    assert(solution_method);
    CommandEntity *old= solution_method->Owner();
    solution_method->set_owner(this);

    int result = 0;
    Domain *the_Domain = this->getDomainPtr();

    //   std::cerr << " StaticDomainDecompositionAnalysis::analyze() - 1\n";

    // check for change in Domain since last step. As a change can
    // occur in a commit() in a domaindecomp with load balancing
    // this must now be inside the loop

    int stamp = the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
        domainStamp = stamp;
        result = this->domainChanged();
        if(result < 0)
          {
            std::cerr << "StaticDomainDecompositionAnalysis::analyze() - domainChanged failed";
            return -1;
          }
      }

    // result =newStepDomain(theAnalysisModel);
    if(result < 0)
      {
        std::cerr << "StaticDomainDecompositionAnalysis::analyze() - the XC::AnalysisModel failed";
        std::cerr << " with domain at load factor ";
        std::cerr << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
        the_Domain->revertToLastCommit();
    
        return -2;
      }

    //  std::cerr << " XC::StaticDomainDecompositionAnalysis::analyze() - 2\n";

    result = getStaticIntegratorPtr()->newStep();
    if(result < 0)
      {
        std::cerr << "XC::StaticDomainDecompositionAnalysis::analyze() - the Integrator failed";
        std::cerr << " with domain at load factor ";
        std::cerr << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
        the_Domain->revertToLastCommit();
    
        return -2;
      }

    result = getEquiSolutionAlgorithmPtr()->solveCurrentStep();
    if(result < 0)
      {
        std::cerr << "XC::StaticDomainDecompositionAnalysis::analyze() - the Algorithm failed";
        std::cerr << " with domain at load factor ";
        std::cerr << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
        the_Domain->revertToLastCommit();	    
        getStaticIntegratorPtr()->revertToLastStep();
    
        return -3;
      }    

    //   std::cerr << " XC::StaticDomainDecompositionAnalysis::analyze() - done ALGO\n";

    result = getStaticIntegratorPtr()->commit();
    if(result < 0)
      {
        std::cerr << "XC::StaticDomainDecompositionAnalysis::analyze() - ";
        std::cerr << "the Integrator failed to commit";
        std::cerr << " with domain at load factor ";
        std::cerr << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
        the_Domain->revertToLastCommit();	    
        getStaticIntegratorPtr()->revertToLastStep();
    
        return -4;
      }    	
    //   std::cerr << " XC::StaticDomainDecompositionAnalysis::analyze() - done COMMIT\n";
    solution_method->set_owner(old);
    return 0;
  }


int XC::StaticDomainDecompositionAnalysis::initialize(void)
  {
    Domain *the_Domain = this->getDomainPtr();
    
    // check if domain has undergone change
    int stamp = the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
        domainStamp = stamp;	
        if(this->domainChanged() < 0)
          {
	    std::cerr << "XC::DirectIntegrationAnalysis::initialize() - domainChanged() failed\n";
	    return -1;
          }	
      }
    if(getStaticIntegratorPtr()->initialize() < 0)
      {
	std::cerr << "XC::DirectIntegrationAnalysis::initialize() - integrator initialize() failed\n";
	return -2;
      }
    else
      getStaticIntegratorPtr()->commit();
    return 0;
  }

//! @brief Make the necessary changes due to a change in the
//! domain.
int XC::StaticDomainDecompositionAnalysis::domainChanged(void)
  {
  Domain *the_Domain = this->getDomainPtr();
  int stamp = the_Domain->hasDomainChanged();
  domainStamp = stamp;

  int result = 0;
  
  getAnalysisModelPtr()->clearAll();    
  getConstraintHandlerPtr()->clearAll();

  // now we invoke handle() on the constraint handler which
  // causes the creation of XC::FE_Element and XC::DOF_Group objects
  // and their addition to the XC::AnalysisModel.
  
  result = getConstraintHandlerPtr()->handle();
  if (result < 0) {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::handle() - ";
    std::cerr << "XC::ConstraintHandler::handle() failed";
    return -1;
  }	
  
  // we now invoke number() on the numberer which causes
  // equation numbers to be assigned to all the DOFs in the
  // AnalysisModel.

  result = getDOF_NumbererPtr()->numberDOF();
  if(result < 0)
    {
      std::cerr << "XC::StaticDomainDecompositionAnalysis::handle() - ";
      std::cerr << "XC::DOF_Numberer::numberDOF() failed";
      return -2;
    }

  // we invoke setSize() on the XC::LinearSOE which
  // causes that object to determine its size
  Graph &theGraph = getAnalysisModelPtr()->getDOFGraph();
  result = getLinearSOEPtr()->setSize(theGraph);
  if (result < 0) {
    std::cerr << "StaticDomainDecompositionAnalysis::handle() - ";
    std::cerr << "LinearSOE::setSize() failed";
    return -3;
  }	    

  // finally we invoke domainChanged on the XC::Integrator and Algorithm
  // objects .. informing them that the model has changed
  
  result = getStaticIntegratorPtr()->domainChanged();
  if (result < 0) {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::setAlgorithm() - ";
    std::cerr << "XC::Integrator::domainChanged() failed";
    return -4;
  }	    

  result = getEquiSolutionAlgorithmPtr()->domainChanged();
  if (result < 0) {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::setAlgorithm() - ";
    std::cerr << "XC::Algorithm::domainChanged() failed";
    return -5;
  }	        

  // if get here successful
  return 0;
}    

//! @brief Returns the number of external equations.
int XC::StaticDomainDecompositionAnalysis::getNumExternalEqn(void)
  {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::getNumExternalEqn() - should never be called\n";
    return 0;
  }

//! @brief Returns the number of equations internas.
int XC::StaticDomainDecompositionAnalysis::getNumInternalEqn(void)
  {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::getNumInternalEqn() - should never be called\n";
    return 0;
  }

//! @brief ??
int XC::StaticDomainDecompositionAnalysis::newStep(double dT) 
  {
    this->analyze(dT);
    return 0;
  }

//! @brief ??
int XC::StaticDomainDecompositionAnalysis::computeInternalResponse(void)
  {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::computeInternalResponse() - should never be called\n";
    return 0;
  }

//! @brief Forma la tangent stiffness matrix.
int XC::StaticDomainDecompositionAnalysis::formTangent(void)
  {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::formTangent() - should never be called\n";
    return 0;
  }

//! @brief Forma el vector residuo.
int XC::StaticDomainDecompositionAnalysis::formResidual(void)
  {
   std::cerr << "XC::StaticDomainDecompositionAnalysis::formResidual() - should never be called\n";
    return 0;
  }

int XC::StaticDomainDecompositionAnalysis::formTangVectProduct(Vector &force)
  {
  std::cerr << "XC::StaticDomainDecompositionAnalysis::formTangVectProduct() - should never be called\n";
  return 0;
  }

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::StaticDomainDecompositionAnalysis::getTangent(void)
  {
    static XC::Matrix errMatrix;
    std::cerr << "XC::StaticDomainDecompositionAnalysis::getTangent() - should never be called\n";
    return errMatrix;
  }

//! @brief Returns the vector residuo.
const XC::Vector &XC::StaticDomainDecompositionAnalysis::getResidual(void)
  {
    static XC::Vector errVector;
    std::cerr << "XC::StaticDomainDecompositionAnalysis::getResidual() - should never be called\n";
    return errVector;
  }

const XC::Vector &XC::StaticDomainDecompositionAnalysis::getTangVectProduct(void)
  {
    static XC::Vector errVector;
    std::cerr << "XC::StaticDomainDecompositionAnalysis::getTangVectProduct() - should never be called\n";
    return errVector;
  }
    
int XC::StaticDomainDecompositionAnalysis::sendSelf(CommParameters &cp)
  {
    // receive the data identifyng the objects in the aggregation
    getDbTag();
    static ID data(8);

  if(getEquiSolutionAlgorithmPtr())
    {
      std::cerr << "XC::StaticDomainDecompositionAnalysis::sendSelf() - no objects exist!\n";
      return -1;
    }

  LinearSOESolver *theSolver = getLinearSOEPtr()->getSolver();
  
    data(0) = getConstraintHandlerPtr()->getClassTag();
  data(1) = getDOF_NumbererPtr()->getClassTag();
  data(2) = getAnalysisModelPtr()->getClassTag();
  data(3) = getEquiSolutionAlgorithmPtr()->getClassTag();
  data(4) = getLinearSOEPtr()->getClassTag();
  data(5) = theSolver->getClassTag();
  data(6) = getStaticIntegratorPtr()->getClassTag();

  if(solution_method->getConvergenceTestPtr())
    data(7) = solution_method->getConvergenceTestPtr()->getClassTag();
  else
    data(7) = -1;

    cp.sendID(data,getDbTagData(),CommMetaData(0));  

  // invoke sendSelf() on all the objects
  if (getConstraintHandlerPtr()->sendSelf(cp) != 0) {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::sendSelf() - failed to send handler\n";
    return -1;
  }

  if(getDOF_NumbererPtr()->sendSelf(cp) != 0) {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::sendSelf() - failed to send numberer\n";
    return -1;
  }

  if (getAnalysisModelPtr()->sendSelf(cp) != 0) {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::sendSelf() - failed to send model\n";
    return -1;
  }

  if (getEquiSolutionAlgorithmPtr()->sendSelf(cp) != 0) {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::sendSelf() - failed to send algorithm\n";
    return -1;
  }

  if(getLinearSOEPtr()->sendSelf(cp) != 0)
    {
      std::cerr << "XC::StaticDomainDecompositionAnalysis::sendSelf() - failed to send SOE\n";
      return -1;
    } else
    ;
  //    theSOE->setAnalysisModel(*theAnalysisModel);

  if (theSolver->sendSelf(cp) != 0) {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::sendSelf() - failed to send XC::Solver\n";
    return -1;
  }

  if (getStaticIntegratorPtr()->sendSelf(cp) != 0) {
    std::cerr << "XC::StaticDomainDecompositionAnalysis::sendSelf() - failed to send integrator\n";
    return -1;
  }

  if(solution_method->getConvergenceTestPtr())
    if(solution_method->getConvergenceTestPtr()->sendSelf(cp) != 0) {
      std::cerr << "XC::StaticDomainDecompositionAnalysis::sendSelf() - failed to send integrator\n";
      return -1;
  }

  return 0;
}

int XC::StaticDomainDecompositionAnalysis::recvSelf(const CommParameters &cp)
  {
    //Domain *the_Domain = this->getSubdomainPtr();

    // receive the data identifyng the objects in the aggregation
    static ID data(8);
    getDbTag();
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
            std::cerr << "StaticDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the XC::ConstraintHandler\n";
            return -1;
          }
      }
    getConstraintHandlerPtr()->recvSelf(cp);
  
    if(getDOF_NumbererPtr() == 0 || getDOF_NumbererPtr()->getClassTag() != data(1))
      {
        if(getDOF_NumbererPtr())
          delete getDOF_NumbererPtr();
    
        brokeNumberer(cp,data);
        if(getDOF_NumbererPtr() == 0)
          {
            std::cerr << "StaticDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the XC::ConstraintHandler\n";
            return -1;
          }
      }
    getDOF_NumbererPtr()->recvSelf(cp);

    if(getAnalysisModelPtr() == 0 || getAnalysisModelPtr()->getClassTag() != data(2))
      {
        if(getAnalysisModelPtr())
          delete getAnalysisModelPtr();
    
        brokeAnalysisModel(cp,data);
        if(getAnalysisModelPtr() == 0)
          {
            std::cerr << "XC::StaticDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the XC::Analysis Model\n";
            return -1;
          }
      }
    getAnalysisModelPtr()->recvSelf(cp);

    if(getEquiSolutionAlgorithmPtr() == 0 || getEquiSolutionAlgorithmPtr()->getClassTag() != data(3))
      {
        if(getEquiSolutionAlgorithmPtr())
          delete getEquiSolutionAlgorithmPtr();
    
        brokeEquiSolnAlgo(cp,data);
        if(getEquiSolutionAlgorithmPtr() == 0)
          {
            std::cerr << "XC::StaticDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the Solution Algorithm\n";
            return -1;
          }
      }
    getEquiSolutionAlgorithmPtr()->recvSelf(cp);

    if(getLinearSOEPtr() == 0 || getLinearSOEPtr()->getClassTag() != data(4))
      {
        if(getLinearSOEPtr() != 0)
          delete getLinearSOEPtr();
        brokeLinearSOE(cp,data);
        if(getLinearSOEPtr() == 0)
          {
            std::cerr << "XC::StaticDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the XC::LinearSOE\n";
            return -1;
          }
      } 

    getLinearSOEPtr()->recvSelf(cp);
    LinearSOESolver *theSolver = getLinearSOEPtr()->getSolver();
    theSolver->recvSelf(cp);  
    //  theSOE->setAnalysisModel(*theAnalysisModel);


    if(getStaticIntegratorPtr() == 0 || getStaticIntegratorPtr()->getClassTag() != data(6))
      {
        if(getStaticIntegratorPtr() != 0)
          delete getStaticIntegratorPtr();
    
        brokeStaticIntegrator(cp,data);
        if(getStaticIntegratorPtr() == 0)
          {
            std::cerr << "StaticDomainDecompositionAnalysis::recvSelf";
            std::cerr << " - failed to get the XC::Integrator\n";
            return -1;
          }
      }
    getStaticIntegratorPtr()->recvSelf(cp);


    if(solution_method->getConvergenceTestPtr() == 0 || solution_method->getConvergenceTestPtr()->getClassTag() != data(7))
      {
	std::cerr << "StaticDomainDecompositionAnalysis::recvSelf; falta implementar la lectura del convergence test." << std::endl;
//         if(solution_method->getConvergenceTestPtr() != 0)
//           delete solution_method->getConvergenceTestPtr();
    
//         if(data(7) != -1)
//           {
//   	    ConvergenceTest *tmp= cp.getNewConvergenceTest(this,data(7));
//             if(!tmp)
//               {
// 	        std::cerr << "XC::StaticDomainDecompositionAnalysis::recvSelf";
// 	        std::cerr << " - failed to get the XC::ConvergenceTest\n";
// 	        return -1;
//               }
//             else
//               solution_method->setConvergenceTest(*tmp);
//           }
       }
     if(solution_method->getConvergenceTestPtr() != 0)
        solution_method->getConvergenceTestPtr()->recvSelf(cp);

     // set up the links needed by the elements in the aggregation
     set_all_links();
     return 0;
  }

//! @brief Sets the solution algorithm to use in the analysis.
int XC::StaticDomainDecompositionAnalysis::setAlgorithm(EquiSolnAlgo &theNewAlgorithm)
  {
    DomainDecompositionAnalysis::setAlgorithm(theNewAlgorithm);
    domainStamp = 0;
    return 0;
  }

//! @brief Sets the integrator to use in the analysis.
int XC::StaticDomainDecompositionAnalysis::setIntegrator(IncrementalIntegrator &theNewIntegrator) 
  {
    DomainDecompositionAnalysis::setIntegrator(theNewIntegrator);
    // cause domainChanged to be invoked on next analyze
    domainStamp = 0;
    return 0;
  }

//! @brief Sets the linear system of equations to use in the analysis.
int XC::StaticDomainDecompositionAnalysis::setLinearSOE(LinearSOE &theNewSOE)
  {
    DomainDecompositionAnalysis::setLinearSOE(theNewSOE);
    // cause domainChanged to be invoked on next analyze
    domainStamp = 0;
    return 0;
  }

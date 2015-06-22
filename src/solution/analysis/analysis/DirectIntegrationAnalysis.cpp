//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.8 $
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/DirectIntegrationAnalysis.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of the
// DirectIntegrationAnalysis class.
//
// What: "@(#) DirectIntegrationAnalysis.C, revA"


#include <solution/analysis/analysis/DirectIntegrationAnalysis.h>
#include "solution/analysis/ModelWrapper.h"
#include "solution/SoluMethod.h"
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/numberer/DOF_Numberer.h>
#include <solution/analysis/handler/ConstraintHandler.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <solution/analysis/integrator/TransientIntegrator.h>
#include <domain/domain/Domain.h>

#include "xc_utils/src/base/CmdStatus.h"
// AddingSensitivity:BEGIN //////////////////////////////////
#ifdef _RELIABILITY
#include <reliability/FEsensitivity/SensitivityAlgorithm.h>
#endif
// AddingSensitivity:END ////////////////////////////////////

//! @brief Constructor.
XC::DirectIntegrationAnalysis::DirectIntegrationAnalysis(SoluMethod *metodo_solu)
  :TransientAnalysis(metodo_solu), domainStamp(0)
  {
// AddingSensitivity:BEGIN ////////////////////////////////////
#ifdef _RELIABILITY
	theSensitivityAlgorithm= nullptr;
#endif
// AddingSensitivity:END //////////////////////////////////////
  }

//! @brief Lee un objeto XC::EigenAnalysis desde archivo
bool XC::DirectIntegrationAnalysis::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(DirectIntegrationAnalysis) Procesando comando: " << cmd << std::endl;
    return TransientAnalysis::procesa_comando(status);
  }    

//! @brief Destructor.
XC::DirectIntegrationAnalysis::~DirectIntegrationAnalysis(void)
  {
    // we don't invoke the destructors in case user switching
    // from a static to a direct integration analysis 
    // clearAll() must be invoked if user wishes to invoke destructor
  }

//! @brief Borra todos los miembros del objeto (Manejador coacciones, modelo de análisis,...).
void XC::DirectIntegrationAnalysis::clearAll(void)
  {
// AddingSensitivity:BEGIN ////////////////////////////////////
#ifdef _RELIABILITY
    delete theSensitivityAlgorithm;
#endif
// AddingSensitivity:END //////////////////////////////////////
  }    

int XC::DirectIntegrationAnalysis::initialize(void)
  {
    assert(metodo_solu);
    Domain *the_Domain = metodo_solu->getDomainPtr();

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
    if(metodo_solu->getTransientIntegratorPtr()->initialize() < 0)
      {
	std::cerr << "XC::DirectIntegrationAnalysis::initialize() - integrator initialize() failed\n";
	return -2;
      }
    else
      metodo_solu->getTransientIntegratorPtr()->commit();
    return 0;
  }

//! @brief Ejecuta el análisis.
int XC::DirectIntegrationAnalysis::analyze(int numSteps, double dT)
  {
    int result= 0;
    assert(metodo_solu);
    EntCmd *old= metodo_solu->Owner();
    metodo_solu->set_owner(this);
    Domain *the_Domain = metodo_solu->getDomainPtr();

    for(int i=0; i<numSteps; i++)
      {
        if(newStepDomain(metodo_solu->getModelWrapperPtr()->getAnalysisModelPtr(),dT) < 0)
          {
	    std::cerr << "DirectIntegrationAnalysis::analyze() - the XC::AnalysisModel failed";
	    std::cerr << " at time " << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
	    the_Domain->revertToLastCommit();
	    return -2;
          }

        // check if domain has undergone change
        int stamp = the_Domain->hasDomainChanged();
        if(stamp != domainStamp)
          {
	    domainStamp = stamp;	
	    if(this->domainChanged() < 0)
              {
	        std::cerr << "XC::DirectIntegrationAnalysis::analyze() - domainChanged() failed\n";
	        return -1;
              }	
          }

        if(metodo_solu->getTransientIntegratorPtr()->newStep(dT) < 0)
          {
	    std::cerr << "XC::DirectIntegrationAnalysis::analyze() - the XC::Integrator failed";
	    std::cerr << " at time " << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
	    the_Domain->revertToLastCommit();
	    return -2;
          }

        result = metodo_solu->getEquiSolutionAlgorithmPtr()->solveCurrentStep();
        if(result < 0)
          {
	    std::cerr << "XC::DirectIntegrationAnalysis::analyze() - the Algorithm failed";
	    std::cerr << " at time " << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
	    the_Domain->revertToLastCommit();	    
	    metodo_solu->getTransientIntegratorPtr()->revertToLastStep();
	    return -3;
          }    

// AddingSensitivity:BEGIN ////////////////////////////////////
#ifdef _RELIABILITY
        if(theSensitivityAlgorithm != 0)
          {
	    result = theSensitivityAlgorithm->computeSensitivities();
	    if(result < 0)
              {
                std::cerr << "XC::StaticAnalysis::analyze() - the XC::SensitivityAlgorithm failed";
	        std::cerr << " at iteration: " << i << " with domain at load factor ";
	        std::cerr << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
	        the_Domain->revertToLastCommit();	    
	        metodo_solu->getTransientIntegratorPtr()->revertToLastStep();
	        return -5;
	      }
          }
#endif
// AddingSensitivity:END //////////////////////////////////////
      
        result= metodo_solu->getTransientIntegratorPtr()->commit();
        if(result < 0)
          {
	    std::cerr << "XC::DirectIntegrationAnalysis::analyze() - ";
	    std::cerr << "the XC::Integrator failed to commit";
	    std::cerr << " at time " << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
	    the_Domain->revertToLastCommit();	    
	    metodo_solu->getTransientIntegratorPtr()->revertToLastStep();
	    return -4;
          }
        // std::cerr << "DirectIntegrationAnalysis - time: " << the_Domain->getTimeTracker().getCurrentTime() << std::endl;
      }    
    metodo_solu->set_owner(old);
    return result;
  }

//! @brief Ejecuta los cambios que implica un cambio en el dominio del problema.
int XC::DirectIntegrationAnalysis::domainChanged(void)
  {
    assert(metodo_solu);
    Domain *the_Domain = metodo_solu->getDomainPtr();
    int stamp = the_Domain->hasDomainChanged();
    domainStamp = stamp;
   
    metodo_solu->getModelWrapperPtr()->getAnalysisModelPtr()->clearAll();    
    metodo_solu->getModelWrapperPtr()->getConstraintHandlerPtr()->clearAll();
    
    // now we invoke handle() on the constraint handler which
    // causes the creation of XC::FE_Element and XC::DOF_Group objects
    // and their addition to the XC::AnalysisModel.

    metodo_solu->getModelWrapperPtr()->getConstraintHandlerPtr()->handle();
    // we now invoke number() on the numberer which causes
    // equation numbers to be assigned to all the DOFs in the
    // AnalysisModel.

    // std::cerr << theAnalysisModel->getDOFGroupGraph();

    metodo_solu->getModelWrapperPtr()->getDOF_NumbererPtr()->numberDOF();

    metodo_solu->getModelWrapperPtr()->getConstraintHandlerPtr()->doneNumberingDOF();

    // we invoke setGraph() on the XC::LinearSOE which
    // causes that object to determine its size

    metodo_solu->getLinearSOEPtr()->setSize(metodo_solu->getModelWrapperPtr()->getAnalysisModelPtr()->getDOFGraph());

    // we invoke domainChange() on the integrator and algorithm
    metodo_solu->getTransientIntegratorPtr()->domainChanged();
    metodo_solu->getEquiSolutionAlgorithmPtr()->domainChanged();


    return 0;
  }    

// AddingSensitivity:BEGIN //////////////////////////////
#ifdef _RELIABILITY
int XC::DirectIntegrationAnalysis::setSensitivityAlgorithm(SensitivityAlgorithm *passedSensitivityAlgorithm)
  {
    int result = 0;

    // invoke the destructor on the old one
    if(theSensitivityAlgorithm)
      { delete theSensitivityAlgorithm; }

    theSensitivityAlgorithm = passedSensitivityAlgorithm;
    return 0;
  }
#endif
// AddingSensitivity:END ///////////////////////////////

//! @brief Establece el renumerador a emplear en el análisis.
int XC::DirectIntegrationAnalysis::setNumberer(DOF_Numberer &theNewNumberer) 
  {
    int result= TransientAnalysis::setNumberer(theNewNumberer);

    // invoke domainChanged() either indirectly or directly
    Domain *the_Domain = metodo_solu->getDomainPtr();
    int stamp = the_Domain->hasDomainChanged();
    domainStamp = stamp;
    result = this->domainChanged();    
    if(result < 0)
      {
        std::cerr << "XC::StaticAnalysis::setNumberer() - setNumberer() failed";
        return -1;
      }	
    return 0;
  }


//! @brief Establece el algoritmo de solución a emplear en el análisis.
int XC::DirectIntegrationAnalysis::setAlgorithm(EquiSolnAlgo &theNewAlgorithm) 
  {
    // invoke the destructor on the old one
    TransientAnalysis::setAlgorithm(theNewAlgorithm);

    // invoke domainChanged() either indirectly or directly
    assert(metodo_solu);
    Domain *the_Domain = metodo_solu->getDomainPtr();
    // check if domain has undergone change
    int stamp = the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp = stamp;	    
	if(this->domainChanged() < 0)
          {
	    std::cerr << "XC::DirectIntegrationAnalysis::setAlgorithm() - ";
	    std::cerr << "domainChanged failed";
	    return -1;
	  }	
      }
    else
      {
	if(metodo_solu->getEquiSolutionAlgorithmPtr()->domainChanged() < 0)
          {
	    std::cerr << "XC::DirectIntegrationAnalysis::setAlgorithm() - ";
	    std::cerr << "XC::algorithm::domainChanged() failed";
	    return -2;
	  }
      }
    return 0;
  }

//! @brief Establece el integrador a emplear en el análisis.
int XC::DirectIntegrationAnalysis::setIntegrator(TransientIntegrator &theNewIntegrator)
  {
    // set the links needed by the other objects in the aggregation
    TransientAnalysis::setIntegrator(theNewIntegrator);

    // invoke domainChanged() either indirectly or directly
    int stamp = metodo_solu->getDomainPtr()->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp = stamp;	    
	if(this->domainChanged() < 0)
          {
	    std::cerr << "XC::DirectIntegrationAnalysis::setAlgorithm() - ";
	    std::cerr << "domainChanged failed";
	    return -1;
          }	
      }
    else
      {
        if(metodo_solu->getTransientIntegratorPtr()->domainChanged() < 0)
          {
	    std::cerr << "XC::DirectIntegrationAnalysis::setAlgorithm() - ";
	    std::cerr << "XC::Integrator::domainChanged failed";
	    return -1;
          }	
      }
    return 0;
  }

//! @brief Establece el sistema de ecuaciones lineal a emplear en el análisis.
int XC::DirectIntegrationAnalysis::setLinearSOE(LinearSOE &theNewSOE)
  {
    // invoke the destructor on the old one
    TransientAnalysis::setLinearSOE(theNewSOE);

    // set the links needed by the other objects in the aggregation

    // set the size either indirectly or directly
    assert(metodo_solu);
    Domain *the_Domain= metodo_solu->getDomainPtr();
    int stamp = the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
        domainStamp = stamp;	    
        if(this->domainChanged() < 0)
          {
	    std::cerr << "XC::DirectIntegrationAnalysis::setAlgorithm() - ";
	    std::cerr << "domainChanged failed";
	    return -1;
          }	
      }
    else
      {
        Graph &theGraph = metodo_solu->getModelWrapperPtr()->getAnalysisModelPtr()->getDOFGraph();
        if(metodo_solu->getLinearSOEPtr()->setSize(theGraph) < 0)
          {
	    std::cerr << "XC::DirectIntegrationAnalysis::setAlgorithm() - ";
	    std::cerr << "XC::LinearSOE::setSize() failed";
	    return -2;	
          }
      }
    return 0;
  }

//! @brief Establece el test de convergencia a emplear en el análisis.
int XC::DirectIntegrationAnalysis::setConvergenceTest(ConvergenceTest &theNewTest)
  {
    if(metodo_solu)
      metodo_solu->setConvergenceTest(theNewTest);
    return 0;
  }

//! @brief Comprueba si el dominio ha cambiado.
int XC::DirectIntegrationAnalysis::checkDomainChange(void)
  {
    assert(metodo_solu);
    Domain *the_Domain = metodo_solu->getDomainPtr();

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
    return 0;
  }





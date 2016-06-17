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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/12/19 22:43:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/EigenIntegrator.cpp,v $
                                                                        
// Written: Jun Peng
// Created: Wed Jan 27, 1999
// Revision: A
//
// Description: This file contains the class definition of XC::EigenIntegrator.
// EigenIntegrator is an algorithmic class for setting up the finite element 
// equations for a eigen problem analysis.
//
// This class is inheritanted from the base class of XC::Integrator which was
// created by fmk (Frank).


#include "solution/analysis/integrator/EigenIntegrator.h"
#include "solution/SoluMethod.h"
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/DOF_GrpIter.h>

//! @brief Constructor.
XC::EigenIntegrator::EigenIntegrator(SoluMethod *owr)
  :Integrator(owr,EigenINTEGRATOR_TAGS_Eigen) {}

//! @brief Solicita al elemento being passed as parameter
//! que forme su matriz de rigidez o de masas dependiendo
//! del valor de flagK.
int XC::EigenIntegrator::formEleTangent(FE_Element *theEle)
  {
    if(flagK == 0)
      return formEleTangK(theEle);
    else
      return formEleTangM(theEle);
  }

int XC::EigenIntegrator::formNodTangent(DOF_Group *theDof)
  { return this->formNodTangM(theDof); }

int XC::EigenIntegrator::formEleResidual(FE_Element *theEle)
  { return 0; }

int XC::EigenIntegrator::formNodUnbalance(DOF_Group *theDof)
  { return 0; }

int XC::EigenIntegrator::newStep(void)
  { return 0; }

int XC::EigenIntegrator::getLastResponse(Vector &result, const XC::ID &id)
  { return 0; }

//! @brief Ensambla la matriz de rigidez.
int XC::EigenIntegrator::formK()
  {
    AnalysisModel *mdl= getAnalysisModelPtr();
    EigenSOE *theSOE= getEigenSOEPtr();
    if(!mdl || !theSOE)
      {
	std::cerr << "WARNING EigenIntegrator::formK -";
	std::cerr << " no AnalysisModel or EigenSOE has been set\n";
	return -1;
      }
    
    // the loops to form and add the tangents are broken into two for 
    // efficiency when performing parallel computations

    // loop through the FE_Elements getting them to form the tangent
    // FE_EleIter &theEles1= mdl->getFEs();
    FE_Element *elePtr;

    flagK= 0; //matriz de rigidez (ver formEleTangent)
    theSOE->zeroA();

    //while((elePtr = theEles1()) != 0) 
    //  elePtr->formTangent(this);
   
   // loop through the FE_Elements getting them to add the tangent    
    int result = 0;
    FE_EleIter &theEles2= mdl->getFEs();    
    while((elePtr = theEles2()) != 0)
      {
        if(theSOE->addA(elePtr->getTangent(this), elePtr->getID()) < 0)
          {
	    std::cerr << "WARNING XC::EigenIntegrator::formK -";
	    std::cerr << " failed in addA for XC::ID " << elePtr->getID();	    
	    result = -2;
	  }
      }
    return result;    
  }

//! @brief Ensambla la matriz de masas.
int XC::EigenIntegrator::formM(void)
  {
    AnalysisModel *mdl= getAnalysisModelPtr();
    EigenSOE *theSOE= getEigenSOEPtr();
    if(!mdl || !theSOE)
      {
	std::cerr << "WARNING EigenIntegrator::formM -";
	std::cerr << " no AnalysisModel or EigenSOE has been set\n";
	return -1;
      }
    
    // the loops to form and add the tangents are broken into two for 
    // efficiency when performing parallel computations


    flagK = 1; //matriz de masas (ver formEleTangent)
    theSOE->zeroM();

    // while((elePtr = theEles1()) != 0) 
    //     elePtr->formTangent(this);
   
    int result = 0;
    // loop through the FE_Elements getting them to form the tangent
    // FE_EleIter &theEles1 = mdl->getFEs();
    FE_Element *elePtr= nullptr;
    FE_EleIter &theEles2 = mdl->getFEs();    
    while((elePtr = theEles2()) != 0)
      {     
	if(theSOE->addM(elePtr->getTangent(this), elePtr->getID()) < 0)
          {
	    std::cerr << "WARNING EigenIntegrator::formM -";
	    std::cerr << " failed in addM for ID " << elePtr->getID();	    
	    result = -2;
	  }
      }

    DOF_Group *dofGroupPtr= nullptr;
    DOF_GrpIter &theDofs= mdl->getDOFGroups();    
    while((dofGroupPtr = theDofs()) != 0)
      {
	//   	dofGroupPtr->formTangent(this);
	if(theSOE->addM(dofGroupPtr->getTangent(this),dofGroupPtr->getID()) < 0)
          {
	    std::cerr << "WARNING EigenIntegrator::formM -";
	    std::cerr << " failed in addM for ID " << dofGroupPtr->getID();	    
	    result = -3;
	  }
      }
    return result;    
  }

//! @brief Solicita al elemento being passed as parameter
//! que forme su matriz de rigidez.
int XC::EigenIntegrator::formEleTangK(FE_Element *theEle)
  {
    theEle->zeroTangent();
    theEle->addKtToTang(1.0);
    return 0;
  }

//! @brief Solicita al elemento being passed as parameter
//! que forme su matriz de masas.
int XC::EigenIntegrator::formEleTangM(FE_Element *theEle)
  {
    theEle->zeroTangent();
    theEle->addMtoTang(1.0);
    return 0;
  }

int XC::EigenIntegrator::formNodTangM(DOF_Group *theDof)
  {
    theDof->zeroTangent();
    theDof->addMtoTang(1.0);
    return 0;
  }

int XC::EigenIntegrator::update(const XC::Vector &deltaU)
  { return 0; }

//! @brief Devuelve un puntero al sistema de ecuaciones de autovalores.
XC::EigenSOE *XC::EigenIntegrator::getEigenSOEPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getEigenSOEPtr();
  }

//! @brief Devuelve un puntero al sistema de ecuaciones de autovalores.
const XC::EigenSOE *XC::EigenIntegrator::getEigenSOEPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getEigenSOEPtr();
  }

int XC::EigenIntegrator::sendSelf(CommParameters &cp)
  { return 0; }

int XC::EigenIntegrator::recvSelf(const CommParameters &cp)
  { return 0; }


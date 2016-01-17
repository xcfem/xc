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
// $Date: 2005/11/28 21:34:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/ConstraintHandler.cpp,v $
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of XC::ConstraintHandler.
//
// What: "@(#) ConstraintHandler.h, revA"

#include "solution/analysis/handler/ConstraintHandler.h"
#include "solution/analysis/ModelWrapper.h"
#include <domain/domain/Domain.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/integrator/Integrator.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>

XC::ConstraintHandler::ConstraintHandler(ModelWrapper *owr,int clasTag)
  :MovableObject(clasTag), EntCmd(owr) {}

int XC::ConstraintHandler::doneNumberingDOF(void)
  {
    // iterate through the FE_Element getting them to set their IDs
    AnalysisModel *am= getAnalysisModelPtr();
    assert(am);
    FE_EleIter &theEle= am->getFEs();
    FE_Element *elePtr;
    while ((elePtr = theEle()) != 0)
      elePtr->setID();
    return 0;
  }

int XC::ConstraintHandler::update(void)
  { return 0; }

int XC::ConstraintHandler::applyLoad(void)
  { return 0; }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
XC::ModelWrapper *XC::ConstraintHandler::getModelWrapper(void)
  { return dynamic_cast<ModelWrapper *>(Owner()); }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
const XC::ModelWrapper *XC::ConstraintHandler::getModelWrapper(void) const
  { return dynamic_cast<const ModelWrapper *>(Owner()); }

//! @brief Devuelve un puntero al dominio.
XC::Domain *XC::ConstraintHandler::getDomainPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getDomainPtr();
  }

//! @brief Devuelve un puntero al dominio.
const XC::Domain *XC::ConstraintHandler::getDomainPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getDomainPtr();
  }

//! @brief Devuelve un puntero al modelo de análisis.
const XC::AnalysisModel *XC::ConstraintHandler::getAnalysisModelPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Devuelve un puntero al modelo de análisis.
XC::AnalysisModel *XC::ConstraintHandler::getAnalysisModelPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Devuelve un puntero al integrador.
const XC::Integrator *XC::ConstraintHandler::getIntegratorPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getIntegratorPtr();
  }

//! @brief Devuelve un puntero al integrador.
XC::Integrator *XC::ConstraintHandler::getIntegratorPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getIntegratorPtr();
  }

//! @brief For the nodes reset the DOF_Group pointers to nullptr
void XC::ConstraintHandler::clearAll(void)
  {
    Domain *theDomain = this->getDomainPtr();
    if(theDomain)
      theDomain->clearDOF_GroupPtr();
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ConstraintHandler::sendData(CommParameters &cp)
  {
    return 0;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ConstraintHandler::recvData(const CommParameters &cp)
  {
    return 0;
  }


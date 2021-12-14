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

//! @brief Constructor.
//! @param owr: pointer to the model wrapper that owns the handler.
//! @param classTag: identifier of the class.
XC::ConstraintHandler::ConstraintHandler(ModelWrapper *owr,int classTag)
  :MovableObject(classTag), CommandEntity(owr) {}

//! @brief Numbering of degrees of freedom.
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

//! @brief Update the state of the constraints.
int XC::ConstraintHandler::update(void)
  { return 0; }

//! @brief ??
int XC::ConstraintHandler::applyLoad(void)
  { return 0; }

//! @brief Return a pointer to the model wrapper.
XC::ModelWrapper *XC::ConstraintHandler::getModelWrapper(void)
  { return dynamic_cast<ModelWrapper *>(Owner()); }

//! @brief Return a const pointer to the model wrapper.
const XC::ModelWrapper *XC::ConstraintHandler::getModelWrapper(void) const
  { return dynamic_cast<const ModelWrapper *>(Owner()); }

//! @brief Return a pointer to the Domain object associated with the ConstraintHandler.
XC::Domain *XC::ConstraintHandler::getDomainPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getDomainPtr();
  }

//! @brief Return a const pointer to the domain object associated with the ConstraintHandler.
const XC::Domain *XC::ConstraintHandler::getDomainPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getDomainPtr();
  }

//! @brief Return a const pointer to the analysis model object associated with the ConstraintHandler.
const XC::AnalysisModel *XC::ConstraintHandler::getAnalysisModelPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Returns a pointer to the AnalysisModel object associated with the ConstraintHandler.
XC::AnalysisModel *XC::ConstraintHandler::getAnalysisModelPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Return a pointer to the Integrator object associated with the ConstraintHandler.
const XC::Integrator *XC::ConstraintHandler::getIntegratorPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getIntegratorPtr();
  }

//! @brief Returns a pointer to the Integrator object associated with the ConstraintHandler.
XC::Integrator *XC::ConstraintHandler::getIntegratorPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getIntegratorPtr();
  }

//! @brief Reset the DOF_Group pointers to nullptr for all the nodes.
//!
//! Invoked to inform the ConstraintHandler object that the FE\_Elements
//! and DOF\_Groups it constructed are no longer part of the AnalysisModel. The
//! ConstraintHandler can delete these objects if necessary; or the
//! ConstraintHandler can store them and use them in subsequent calls to
//! handle(). 
void XC::ConstraintHandler::clearAll(void)
  {
    Domain *theDomain = this->getDomainPtr();
    if(theDomain)
      theDomain->clearDOF_GroupPtr();
  }

//! @brief Send object members through the communicator argument.
int XC::ConstraintHandler::sendData(Communicator &comm)
  { return 0; }

//! @brief Receive object members through the communicator argument.
int XC::ConstraintHandler::recvData(const Communicator &comm)
  { return 0; }


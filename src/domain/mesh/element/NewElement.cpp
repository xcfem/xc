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

// $Revision: 1.5 $
// $Date: 2003/03/11 20:40:07 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/NewElement.cpp,v $

// Written: fmk
// Created: 08/01
//
// Description: This file contains the implementation for the XC::NewElement class.
//
// What: "@(#) NewElement.cpp, revA"

#include "domain/mesh/element/NewElement.h"
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"



static int NUM_DOF  =4;

// constructors:
XC::NewElement::NewElement(int tag)
 :Element(tag,ELE_TAG_NewElement),
  theMatrix(NUM_DOF, NUM_DOF), theVector(NUM_DOF)
{}

XC::NewElement::NewElement()
 :Element(0,ELE_TAG_NewElement),
  theMatrix(NUM_DOF, NUM_DOF), theVector(NUM_DOF)
{}

XC::NodePtrsWithIDs &XC::NewElement::getNodePtrs(void)
  {
    std::cerr << "NewElement::getNodePtrs() - not implemented\n";
    static NodePtrsWithIDs retval(this,1);
    return retval;
  }

const XC::NodePtrsWithIDs &XC::NewElement::getNodePtrs(void) const
  {
    std::cerr << "NewElement::getNodePtrs() - not implemented\n";
    static NodePtrsWithIDs retval(const_cast<NewElement *>(this),1);
    return retval;
  }

int XC::NewElement::getNumDOF(void) const
  { return NUM_DOF; }


void XC::NewElement::setDomain(Domain *theDomain)
{
    // call the base class method
    this->XC::DomainComponent::setDomain(theDomain);

}


int XC::NewElement::commitState()
{
  int retVal = 0;

  // call the base class method
  retVal = this->Element::commitState();
  if(retVal < 0) {
    std::cerr << "XC::NewElement::commitState() - failed in base class\n";
    return retVal;
  }

  return retVal;
}

int XC::NewElement::revertToLastCommit()
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    return 0;
  }

int XC::NewElement::revertToStart()
  { return Element::revertToStart(); }

int XC::NewElement::update(void)
{
  return 0;
}


const XC::Matrix &XC::NewElement::getTangentStiff(void) const
  {
    if(isDead())
      theMatrix*=dead_srf;
    return theMatrix;
  }

const XC::Matrix &XC::NewElement::getInitialStiff(void) const
  {
    if(isDead())
      theMatrix*=dead_srf;
    return theMatrix;
  }

void XC::NewElement::zeroLoad(void)
  { return; }

int XC::NewElement::addLoad(const Vector &addP)
  {
    if(isDead())
      std::cerr << getClassName() 
                << "; load over inactive element: "
                << getTag() << std::endl;
    return 0;
  }

int XC::NewElement::addInertiaLoadToUnbalance(const Vector &accel)
  { return 0; }

const XC::Vector &XC::NewElement::getResistingForce(void) const
  { return theVector; }


const XC::Vector &XC::NewElement::getResistingForceIncInertia(void) const
  { return theVector; }


int XC::NewElement::sendSelf(Communicator &comm)
  { return -1; }

int XC::NewElement::recvSelf(const Communicator &comm)
  { return -1; }


void XC::NewElement::Print(std::ostream &s, int flag) const
  { return; }


XC::Response *XC::NewElement::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  { return 0; }


int XC::NewElement::getResponse(int responseID, Information &eleInfo)
  { return -1; }


int XC::NewElement::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }


int XC::NewElement::updateParameter(int parameterID, Information &info)
  { return -1; }

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
#include <domain/mesh/element/Information.h>
#include <utility/recorder/response/ElementResponse.h>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "NodePtrsWithIDs.h"



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
    std::cerr << "NewElement::getNodePtrs() - no implementada\n";
    static NodePtrsWithIDs retval(this,1);
    return retval;
  }

const XC::NodePtrsWithIDs &XC::NewElement::getNodePtrs(void) const
  {
    std::cerr << "NewElement::getNodePtrs() - no implementada\n";
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
  retVal = this->XC::Element::commitState();
  if(retVal < 0) {
    std::cerr << "XC::NewElement::commitState() - failed in base class\n";
    return retVal;
  }

  return retVal;
}

int XC::NewElement::revertToLastCommit()
{
  return 0;
}

int XC::NewElement::revertToStart()
{
  return 0;
}

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
      std::cerr << nombre_clase() 
                << "; se intentó cargar el elemento "
                << getTag() << " que está desactivado." 
                << std::endl;
    return 0;
  }

int XC::NewElement::addInertiaLoadToUnbalance(const Vector &accel)
  { return 0; }

const XC::Vector &XC::NewElement::getResistingForce(void) const
  { return theVector; }


const XC::Vector &XC::NewElement::getResistingForceIncInertia(void) const
  { return theVector; }


int XC::NewElement::sendSelf(CommParameters &cp)
  { return -1; }

int XC::NewElement::recvSelf(const CommParameters &cp)
  { return -1; }


void XC::NewElement::Print(std::ostream &s, int flag)
  { return; }


XC::Response *XC::NewElement::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  { return 0; }


int XC::NewElement::getResponse(int responseID, Information &eleInfo)
  { return -1; }


int XC::NewElement::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }


int XC::NewElement::updateParameter(int parameterID, Information &info)
  { return -1; }

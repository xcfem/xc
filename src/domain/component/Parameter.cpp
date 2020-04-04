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
** (C) Copyright 2001, The Regents of the University of California    **
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
                                                                        
// $Revision: 1.9 $
// $Date: 2008/08/26 15:43:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/component/Parameter.cpp,v $

#include "DomainComponent.h"
#include "classTags.h"
#include "Parameter.h"

XC::Parameter::Parameter(int passedTag,DomainComponent *parentObject,const std::vector<std::string> &argv)
  :TaggedObject(passedTag), MovableObject(PARAMETER_TAG_Parameter),
   theComponents(), numComponents(0), maxNumComponents(0),
   theObjects(),numObjects(0), maxNumObjects(0), parameterID(), gradIndex(-1)
  {
    int ok = -1;

    maxNumObjects = initialSize;
    maxNumComponents = initialSize;
    theComponents= std::vector<DomainComponent *>(maxNumComponents,static_cast<DomainComponent *>(nullptr));
    theObjects= std::vector<MovableObject *>(maxNumObjects,static_cast<MovableObject *>(nullptr));
    parameterID= std::vector<int>(maxNumObjects,0);

    if(parentObject!=0)
      {
        ok = parentObject->setParameter(argv, *this);
        theComponents[0] = parentObject;
        numComponents = 1;
      }

    if(numObjects == 0 || ok < 0)
      {
        std::cerr << "Parameter::Parameter "<< this->getTag()
                  <<" -- no objects were able to identify parameter" << std::endl;
        for(size_t i=0;i<argv.size();i++)
          std::cerr << argv[i] << ' ';
        std::cerr << std::endl;
      }
  }

XC::Parameter::Parameter(const Parameter &param)
  : TaggedObject(param), MovableObject(PARAMETER_TAG_Parameter),
    theComponents(param.theComponents), numComponents(param.numComponents), maxNumComponents(param.maxNumComponents),
    theObjects(param.theObjects),numObjects(param.numObjects), maxNumObjects(param.maxNumObjects), parameterID(param.parameterID), gradIndex(param.gradIndex)
  {
    theInfo = param.theInfo;
  }

XC::Parameter::Parameter(int tag, int classTag)
  :TaggedObject(tag), MovableObject(classTag),
   theComponents(), numComponents(0), maxNumComponents(0),
   theObjects(),numObjects(0), maxNumObjects(0), parameterID(0), gradIndex(-1)
  {
    maxNumObjects = 1;
    maxNumComponents = 1;

    theComponents= std::vector<DomainComponent *>(maxNumComponents,static_cast<DomainComponent *>(nullptr));
    theObjects = std::vector<MovableObject *>(maxNumObjects,static_cast<MovableObject *>(nullptr));
    parameterID = std::vector<int>(maxNumObjects);
  }


XC::Parameter::Parameter(void)
  :TaggedObject(0), MovableObject(PARAMETER_TAG_Parameter), 
   theComponents(), numComponents(0), maxNumComponents(0),
   theObjects(),numObjects(0), maxNumObjects(0), parameterID(0), gradIndex(-1)
  {}

int XC::Parameter::addComponent(DomainComponent *parentObject, const std::vector<std::string> &argv)
  {
    if(numComponents == maxNumComponents)
      {
        maxNumComponents+= expandSize;
        theComponents.resize(maxNumComponents, nullptr);
      }

    theComponents[numComponents]= parentObject;
    numComponents++;

    //return (parentObject != 0) ?
    //  parentObject->setParameter(argv, *this) : -1;

    int ok = -1;

    int oldNumObjects = numObjects;
    if(parentObject != 0)
      ok = parentObject->setParameter(argv, *this);

    if(numObjects == oldNumObjects || ok < 0)
      {
        std::cerr << "Parameter::addComponent "<< this->getTag() <<" -- no objects were able to identify parameter" << std::endl;
        for(size_t i=0;i<argv.size();i++)
          std::cerr << argv[i] << ' ';
        std::cerr << std::endl;
        return -1;
      }
    return 0;
  }

int XC::Parameter::update(int newValue)
  {
    theInfo.theInt = newValue;
    int ok = 0;

    for(int i = 0; i < numObjects; i++)
      ok += theObjects[i]->updateParameter(parameterID[i], theInfo);
    return ok;
  }

int XC::Parameter::update(double newValue)
  {
    theInfo.theDouble = newValue;
    int ok = 0;
    for(int i = 0; i < numObjects; i++)
      ok += theObjects[i]->updateParameter(parameterID[i], theInfo);
    return ok;
  }

int XC::Parameter::activate(bool active)
  {
    int ok= 0;
    for(int i = 0; i < numObjects; i++)
      { ok += theObjects[i]->activateParameter(active ? parameterID[i] : 0); }
    return ok;
  }

void XC::Parameter::Print(std::ostream &s, int flag) const 
  {
    s << "Parameter, tag = " << this->getTag() << std::endl;
    //s << "\tparameterID = " << parameterID << std::endl;
  }

int XC::Parameter::addObject(int paramID, MovableObject *object)
  {
    if(numObjects == maxNumObjects)
      {
        maxNumObjects+= expandSize;
        theComponents.resize(maxNumObjects, nullptr);
        parameterID.resize(maxNumObjects,0);
      }
    parameterID[numObjects]= paramID;
    theObjects[numObjects]= object;
    numObjects++;
    return 0;
  }

void XC::Parameter::setDomain(Domain *theDomain)
  { return; }

int XC::Parameter::sendSelf(Communicator &comm)
  { return 0; }

int XC::Parameter::recvSelf(const Communicator &comm)
  { return 0; }

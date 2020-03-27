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
//Positioner.cpp

#include "Positioner.h"
#include <classTags.h>
#include "domain/component/DomainComponent.h"


XC::Positioner::Positioner(int passedTag, int classTag,DomainComponent *object,const std::vector<std::string> &argv)
  :ReliabilityDomainComponent(passedTag, classTag), theObject(object), parameterID(-1)
  {
    if(theObject) 
      parameterID = theObject->setParameter(argv, theParam);
    if(parameterID < 0)
      std::cerr << "Positioner::" << __FUNCTION__ << this->getTag() 
                <<" -- unable to set parameter" << std::endl;
  }


int XC::Positioner::update(double newValue)
  {
    theInfo.theDouble = newValue;
    if(parameterID >= 0)
      return theObject->updateParameter(parameterID, theInfo);
    else
      return -1;
  }

int XC::Positioner::activate(bool active)
  {
    if(active)
      { theObject->activateParameter(parameterID); }
    else
      { theObject->activateParameter(0); }
    return 0;
  }

//! @brief Print stuff.
void XC::Positioner::Print(std::ostream &s, int flag) const
  {}



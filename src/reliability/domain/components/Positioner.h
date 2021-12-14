// -*-c++-*-
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
//Positioner.h

#ifndef Positioner_h
#define Positioner_h

#include <reliability/domain/components/ReliabilityDomainComponent.h>
#include "domain/mesh/element/utils/Information.h"
#include "domain/component/Parameter.h"

namespace XC {

class DomainComponent;
//! @ingroup ReliabilityAnalysis
// 
//! @brief A positioner maps a random variable to a
//! domain component.
class Positioner: public ReliabilityDomainComponent
  {
  protected:
    Information theInfo;
    Parameter theParam; //LCPT 22/04/2209
    DomainComponent *theObject;
    int parameterID;
  public:
    Positioner(int tag, int classTag, DomainComponent *theObject, const std::vector<std::string> &argv);

    void Print(std::ostream &s, int flag =0) const;
    int update(double newValue); 
    int activate(bool active);

  };
} // end of XC namespace

#endif


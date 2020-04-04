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
//ContinuaReprComponent.cc

#include "ContinuaReprComponent.h"

#include "utility/matrix/ID.h"

XC::ContinuaReprComponent::ContinuaReprComponent(int classTag)
  : DomainComponent(0,classTag), dead(false){}

XC::ContinuaReprComponent::ContinuaReprComponent(int tag, int classTag)
  : DomainComponent(tag,classTag), dead(false){}

//! @brief Send members through the channel being passed as parameter.
int XC::ContinuaReprComponent::sendData(Communicator &comm)
  {
    setDbTagDataPos(0,getTag());
    int res= comm.sendBool(dead,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::ContinuaReprComponent::recvData(const Communicator &comm)
  {
    setTag(getDbTagDataPos(0));
    int res= comm.receiveBool(dead,getDbTagData(),CommMetaData(1));
    return res;
  }


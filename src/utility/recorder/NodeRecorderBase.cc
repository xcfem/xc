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
//NodeRecorderBase.cpp

#include <utility/recorder/NodeRecorderBase.h>
#include <utility/matrix/ID.h>
#include "utility/actor/actor/ArrayCommMetaData.h"

XC::NodeRecorderBase::NodeRecorderBase(int classTag)
  :MeshCompRecorder(classTag), theDofs(nullptr), theNodalTags(nullptr), theNodes(),
   dataFlag(0),numValidNodes(0)
  {}

XC::NodeRecorderBase::NodeRecorderBase(int classTag,const ID &dofs, const ID &nodes, 
                                       Domain &theDom, DataOutputHandler &theOutputHandler,
                                       double dT, bool timeFlag)
  :MeshCompRecorder(classTag,theDom,theOutputHandler,dT,timeFlag),
   theDofs(nullptr), theNodalTags(nullptr), theNodes(), 
   dataFlag(0), numValidNodes(0) {}

XC::NodeRecorderBase::~NodeRecorderBase(void)
  {
    if(theDofs) delete theDofs;
    if(theNodalTags) delete theNodalTags;
  }

//! @brief Send the object through the communicator
//! being passed as parameter.
int XC::NodeRecorderBase::sendData(CommParameters &cp)
  {
    int res= MeshCompRecorder::sendData(cp);
    res+= cp.sendIDPtr(theDofs,getDbTagData(),ArrayCommMetaData(6,7,8));
    res+= cp.sendIDPtr(theNodalTags,getDbTagData(),ArrayCommMetaData(9,10,11));
    res+= cp.sendInts(dataFlag,numValidNodes,getDbTagData(),CommMetaData(12));
    return res;
  }

//! @brief Receive the object through the communicator
//! being passed as parameter.
int XC::NodeRecorderBase::receiveData(const CommParameters &cp)
  {
    int res= MeshCompRecorder::receiveData(cp);
    theDofs= cp.receiveIDPtr(theDofs,getDbTagData(),ArrayCommMetaData(6,7,8));
    theNodalTags= cp.receiveIDPtr(theNodalTags,getDbTagData(),ArrayCommMetaData(9,10,11));
    res+= cp.receiveInts(dataFlag,numValidNodes,getDbTagData(),CommMetaData(12));
    return res;
  }

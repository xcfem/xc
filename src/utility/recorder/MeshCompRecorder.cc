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
//MeshCompRecorder.cpp

#include <utility/recorder/MeshCompRecorder.h>
#include <utility/handler/DataOutputHandler.h>

XC::MeshCompRecorder::MeshCompRecorder(int classTag)
  :HandlerRecorder(classTag), deltaT(0.0), nextTimeStampToRecord(0.0)
  {}

XC::MeshCompRecorder::MeshCompRecorder(int classTag,Domain &theDom, 
                                       DataOutputHandler &theOutputHandler,
                                       double dT, bool timeFlag)
  :HandlerRecorder(classTag,theDom,theOutputHandler,timeFlag),
   deltaT(dT), nextTimeStampToRecord(0.0) {}

//! @brief Send the object mediante el comunicador
//! being passed as parameter.
int XC::MeshCompRecorder::sendData(CommParameters &cp)
  {
    int res= HandlerRecorder::sendData(cp);
    res+= cp.sendDoubles(deltaT,nextTimeStampToRecord,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives object through the communicator being passed as parameter.
int XC::MeshCompRecorder::receiveData(const CommParameters &cp)
  {
    int res= HandlerRecorder::receiveData(cp);
    res+= cp.receiveDoubles(deltaT,nextTimeStampToRecord,getDbTagData(),CommMetaData(5));
    return res;
  }

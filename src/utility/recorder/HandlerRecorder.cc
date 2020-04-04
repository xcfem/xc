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
//HandlerRecorder.cpp

#include <utility/recorder/HandlerRecorder.h>
#include <utility/handler/DataOutputHandler.h>

XC::HandlerRecorder::HandlerRecorder(int classTag)
  :DomainRecorderBase(classTag,nullptr), theHandler(nullptr), initializationDone(false), echoTimeFlag(false)
  {}

XC::HandlerRecorder::HandlerRecorder(int classTag,Domain &theDom,DataOutputHandler &theOutputHandler,bool tf)
  :DomainRecorderBase(classTag,&theDom), theHandler(&theOutputHandler), initializationDone(false), echoTimeFlag(tf) {}

//! @brief Sets de data output handler
void XC::HandlerRecorder::SetOutputHandler(DataOutputHandler *tH)
  { theHandler= tH; }


//! @brief Sends object through the communicator being passed as parameter.
int XC::HandlerRecorder::sendData(Communicator &comm)
  {
    comm.sendBrokedPtr(theHandler,getDbTagData(),BrokedPtrCommMetaData(0,1,2));
    comm.sendBool(echoTimeFlag,getDbTagData(),CommMetaData(3));
    comm.sendBool(initializationDone,getDbTagData(),CommMetaData(4));
    return 0;
  }

//! @brief Receives object through the communicator being passed as parameter.
int XC::HandlerRecorder::receiveData(const Communicator &comm)
  {
    if(theHandler)
      {
        delete theHandler;
        theHandler= nullptr;
      }
    theHandler= comm.getBrokedDataOutputHandler(theHandler,getDbTagData(),BrokedPtrCommMetaData(0,1,2));
    comm.receiveBool(echoTimeFlag,getDbTagData(),CommMetaData(3));
    comm.receiveBool(initializationDone,getDbTagData(),CommMetaData(4));
    return 0;
  }

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
//ElementRecorderBase.cpp
                                                                        
#include <utility/recorder/ElementRecorderBase.h>
#include <utility/recorder/response/Response.h>
#include "utility/utils/text/text_string.h"
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/handler/DataOutputHandler.h>
#include <utility/actor/message/Message.h>

XC::ElementRecorderBase::ElementRecorderBase(int classTag)
  : MeshCompRecorder(classTag), eleID(0), theResponses(), responseArgs()
  {}

XC::ElementRecorderBase::ElementRecorderBase(int classTag,const XC::ID &ele, const std::vector<std::string> &argv, 
                                 bool echoTime, 
                                 Domain &theDom, DataOutputHandler &theOutputHandler,
                                 double dT)
  :MeshCompRecorder(classTag,theDom,theOutputHandler,dT,echoTime),
   eleID(ele), theResponses(), responseArgs(argv) {}

//! @brief set the responses to get in a record
void XC::ElementRecorderBase::setup_responses(const std::string &dataToStore)
  {
    std::deque<std::string> campos= separa_cadena(dataToStore," ");
    const int argc= campos.size();
    responseArgs.resize(argc);
    for(int i=0; i<argc; i++)
      responseArgs[i]= campos[i];
  }

//@brief Destructor.
XC::ElementRecorderBase::~ElementRecorderBase(void)
  {
    const int numEle= eleID.Size();
    for(int i= 0;i<numEle;i++)
      {
        if(theResponses[i])
          {
            delete theResponses[i];
            theResponses[i]= nullptr;
          }
      }
  }

//! @brief Send the object to another process.
int XC::ElementRecorderBase::sendData(Communicator &comm)
  {
    int res= MeshCompRecorder::sendData(comm);
    res+= comm.sendID(eleID,getDbTagData(),CommMetaData(6));
    res+= comm.sendStrings(responseArgs,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receive the object from other process.
int XC::ElementRecorderBase::receiveData(const Communicator &comm)
  {
    int res= MeshCompRecorder::receiveData(comm);
    res+= comm.receiveID(eleID,getDbTagData(),CommMetaData(6));
    res+= comm.receiveStrings(responseArgs,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Send the object to other process.
int XC::ElementRecorderBase::sendSelf(Communicator &comm)
  {
    int res= 0;
    if(comm.isDatastore() == 1)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; does not send data to a datastore\n";
    else
      {
        setDbTag(comm);
        const int dataTag= getDbTag();
        inicComm(8);
        res= sendData(comm);
        if(comm.sendIdData(getDbTagData(),dataTag)< 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
                      << "; failed to send idData\n";
            return -1;
          }
      }
    return res;
  }

//! @brief Receive the object from other process.
int XC::ElementRecorderBase::recvSelf(const Communicator &comm)
  {
    int res= 0;
    if(comm.isDatastore() == 1)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; does not recv data to a datastore\n";
    else
      {
        inicComm(8);
        const int dataTag= getDbTag();
        res= comm.receiveIdData(getDbTagData(),dataTag);
        if(res < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to recv idData\n";
            return res;
          }
        res= receiveData(comm);
      }
    return res;
  }

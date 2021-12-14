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
                                                                        
#include "ShadowActorBase.h"
#include "utility/actor/channel/Channel.h"
#include "utility/matrix/ID.h"
#include "utility/actor/machineBroker/MachineBroker.h"

//! @brief Constructor. Sets the channel and the broker and make
//! #theChannel call setUpConnection().
XC::ShadowActorBase::ShadowActorBase(Channel &theChan, FEM_ObjectBroker &myBroker)
  : ObjectWithObjBroker(myBroker), commitTag(0), theChannel(&theChan)
  { theChannel->setUpConnection(); }

//! @brief Constructor. Sets the channel and the broker and gets
//! the channel from the machine broker.
XC::ShadowActorBase::ShadowActorBase(int actorType, FEM_ObjectBroker &myBroker, MachineBroker &theMachineBroker, int compDemand)
  : ObjectWithObjBroker(myBroker), commitTag(0), theChannel(nullptr)
  {
    // start the remote actor process running
    theChannel = theMachineBroker.startActor(actorType, compDemand);
    if(theChannel < 0)
      {
        std::cerr << "ShadowActorBase::ShadowActorBase; "
	          << " could not start remote actor\n";
        std::cerr << " using program " << actorType << std::endl;
        exit(-1);
      }
    else
      {
        // now call setUpShadow on the channel
        theChannel->setUpConnection();
      }
  }

//! @brief Set the value for #commitTag.
void XC::ShadowActorBase::setCommitTag(int tag)
  { commitTag = tag; }

//! @brief Returns a pointer to the object channel #theChannel.
XC::Channel *XC::ShadowActorBase::getChannelPtr(void) const
  { return theChannel; }

//! @brief barrier check.
int XC::ShadowActorBase::barrierCheck(int myResult = 0)
  {
    int result;
    static ID data(1);
    data(0) = myResult; 
    theChannel->sendID(0, commitTag, data); 
    theChannel->recvID(0, commitTag, data);
    result = data(0);
    return result;
  }



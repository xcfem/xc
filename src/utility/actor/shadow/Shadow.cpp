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
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/23 18:25:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/shadow/Shadow.cpp,v $
                                                                        

// Written: fmk
// Revision: A
//
// Purpose: This file contains the implementation of XC::Shadow.
//
// What: "@(#) Shadow.C, revA"

#include <utility/actor/shadow/Shadow.h>
#include <stdlib.h>

#include <utility/actor/channel/Channel.h>
#include <utility/actor/machineBroker/MachineBroker.h>
#include <utility/actor/message/Message.h>
#include <utility/actor/actor/MovableObject.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

//! @brief Constructor. Sets the channel and the broker.
//!
//! This constructor is used when the actor process is already up and
//! running. The constructor sets its channel to be \p theChan, its 
//! associated object broker to be \p myBroker.
XC::Shadow::Shadow(Channel &theChan, FEM_ObjectBroker &myBroker)
  : ShadowActorBase(theChan,myBroker), theRemoteActorsAddress(nullptr)
  {}


//! @brief Constructor. Sets the channel, the broker, and the
//! address to send data.
//!
//! This constructor is used when the actor process is already up and
//! running. The constructor sets its channel to be \p theChan, its 
//! associated object broker to be \p myBroker, and the address to
//! which it will send data to be \p thaAddress.
XC::Shadow::Shadow(Channel &theChan, FEM_ObjectBroker &myBroker, ChannelAddress &theAddress)
  : ShadowActorBase(theChan,myBroker), theRemoteActorsAddress(&theAddress)
  {}

//! @brief Constructor. Sets the channel and the broker and gets
//! the channel from the machine broker.
XC::Shadow::Shadow(int actorType, FEM_ObjectBroker &myBroker, MachineBroker &theMachineBroker, int compDemand)
  :ShadowActorBase(actorType,myBroker,theMachineBroker, compDemand), theRemoteActorsAddress(nullptr)
  {
    theRemoteActorsAddress = theChannel->getLastSendersAddress();
  }

//! @brief Send object.
//!
//! Will send the MovableObject \p theObject to
//! the actor object through the shadows channel. It returns the
//! result of invoking sendObj(0,0,theObject, theBroker, theActorsAddress)
//! on the shadow's associated channel #theChannel. 
int XC::Shadow::sendObject(MovableObject &theObject)
  { return theChannel->sendObj(getCommitTag(), theObject, theRemoteActorsAddress); }

//! @brief Receive object.
//!
//! Will cause the object to read the MovableObject \p theObject
//! from the channel. It will return the result of invoking
//! recvObj(0,0,theObject, theBroker, theActorsAddress) on the
//! shadows associated channel #theChannel.
int XC::Shadow::recvObject(MovableObject &theObject)
{
    return theChannel->recvObj(getCommitTag(), theObject,*theBroker, theRemoteActorsAddress);
}


//! @brief Send message.
//!
//! Will send the \p theMessage to
//! the actor message through the shadows channel. It returns the
//! result of invoking sendObj(0,0,theMessage, theBroker, theActorsAddress)
//! on the shadow's associated channel #theChannel. 
int XC::Shadow::recvMessage(Message &theMessage)
{
    return theChannel->recvMsg(0, getCommitTag(), theMessage, theRemoteActorsAddress);
}

//! @brief Receive message.
//!
//! Will cause to read the \p theMessage
//! from the channel. It will return the result of invoking
//! recvObj(0,0,theMessage, theBroker, theActorsAddress) on the
//! shadows associated channel #theChannel.
int XC::Shadow::sendMessage(const XC::Message &theMessage)
  {
    return theChannel->sendMsg(0, getCommitTag(), theMessage, theRemoteActorsAddress);
  }

//! @brief Send matrix.
//!
//! Will send the \p theMatrix to
//! the actor matrix through the shadows channel. It returns the
//! result of invoking sendObj(0,0,theMatrix, theBroker, theActorsAddress)
//! on the shadow's associated channel #theChannel. 
int XC::Shadow::sendMatrix(const XC::Matrix &theMatrix)
  {
    return theChannel->sendMatrix(0, getCommitTag(), theMatrix, theRemoteActorsAddress);
  }

//! @brief Receive matrix.
//!
//! Will cause to read the \p theMatrix
//! from the channel. It will return the result of invoking
//! recvObj(0,0,theMatrix, theBroker, theActorsAddress) on the
//! shadows associated channel #theChannel.
int XC::Shadow::recvMatrix(Matrix &theMatrix)
  {
    return theChannel->recvMatrix(0, getCommitTag(), theMatrix, theRemoteActorsAddress);
  }

//! @brief Send vector.
//!
//! Will send the \p theVector to
//! the actor vector through the shadows channel. It returns the
//! result of invoking sendObj(0,0,theVector, theBroker, theActorsAddress)
//! on the shadow's associated channel #theChannel. 
int XC::Shadow::sendVector(const XC::Vector &theVector)
  {
    return theChannel->sendVector(0, getCommitTag(), theVector, theRemoteActorsAddress);
  }

//! @brief Receive vector.
//!
//! Will cause to read the \p theVector
//! from the channel. It will return the result of invoking
//! recvObj(0,0,theVector, theBroker, theActorsAddress) on the
//! shadows associated channel #theChannel.
int XC::Shadow::recvVector(Vector &theVector)
  {
    return theChannel->recvVector(0, getCommitTag(), theVector, theRemoteActorsAddress);
  }

//! @brief Send integer vector.
//!
//! Will send the \p theID to
//! the actor integer vector through the shadows channel. It returns the
//! result of invoking sendObj(0,0,theID, theBroker, theActorsAddress)
//! on the shadow's associated channel #theChannel. 
int XC::Shadow::sendID(const XC::ID &theID)
  {
    return theChannel->sendID(0, getCommitTag(), theID, theRemoteActorsAddress);
  }

//! @brief Receive integer vector.
//!
//! Will cause to read the \p theID
//! from the channel. It will return the result of invoking
//! recvObj(0,0,theID, theBroker, theActorsAddress) on the
//! shadows associated channel #theChannel.
int XC::Shadow::recvID(ID &theID)
  {
    return theChannel->recvID(0, getCommitTag(), theID, theRemoteActorsAddress);
  }

//! @brief Return a pointer to the channel in the actors address space. 
XC::ChannelAddress *XC::Shadow::getActorAddressPtr(void) const
  { return theRemoteActorsAddress; }



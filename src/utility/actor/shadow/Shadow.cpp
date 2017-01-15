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

XC::Shadow::Shadow(Channel &theChan, FEM_ObjectBroker &myBroker)
  :theRemoteActorsAddress(nullptr), commitTag(0), theChannel(&theChan), theBroker(&myBroker)
  { theChannel->setUpConnection(); }


XC::Shadow::Shadow(Channel &theChan, FEM_ObjectBroker &myBroker, ChannelAddress &theAddress)
  :theRemoteActorsAddress(&theAddress), commitTag(0), theChannel(&theChan), theBroker(&myBroker)
  { theChannel->setUpConnection(); }

XC::Shadow::Shadow(int actorType, FEM_ObjectBroker &myBroker, MachineBroker &theMachineBroker, int compDemand)
  :theRemoteActorsAddress(nullptr), commitTag(0), theBroker(&myBroker)
  {
    // start the remote actor process running
    theChannel = theMachineBroker.startActor(actorType, compDemand);
    if(theChannel < 0)
      {
        std::cerr << "Shadow::Shadow - could not start remote actor\n";
        std::cerr << " using program " << actorType << std::endl;
        exit(-1);
      }

    // now call setUpShadow on the channel
    theChannel->setUpConnection();
    theRemoteActorsAddress = theChannel->getLastSendersAddress();
  }

int XC::Shadow::sendObject(MovableObject &theObject)
  { return theChannel->sendObj(commitTag, theObject, theRemoteActorsAddress); }

int XC::Shadow::recvObject(MovableObject &theObject)
{
    return theChannel->recvObj(commitTag, theObject,*theBroker, theRemoteActorsAddress);
}


int
XC::Shadow::recvMessage(Message &theMessage)
{
    return theChannel->recvMsg(0, commitTag, theMessage, theRemoteActorsAddress);
}

int
XC::Shadow::sendMessage(const XC::Message &theMessage)
{
    return theChannel->sendMsg(0, commitTag, theMessage, theRemoteActorsAddress);
}

int
XC::Shadow::sendMatrix(const XC::Matrix &theMatrix)
{
    return theChannel->sendMatrix(0, commitTag, theMatrix, theRemoteActorsAddress);
}

int
XC::Shadow::recvMatrix(Matrix &theMatrix)
{
    return theChannel->recvMatrix(0, commitTag, theMatrix, theRemoteActorsAddress);
}

int
XC::Shadow::sendVector(const XC::Vector &theVector)
{
    return theChannel->sendVector(0, commitTag, theVector, theRemoteActorsAddress);
}

int
XC::Shadow::recvVector(Vector &theVector)
{
    return theChannel->recvVector(0, commitTag, theVector, theRemoteActorsAddress);
}

int
XC::Shadow::sendID(const XC::ID &theID)
{
    return theChannel->sendID(0, commitTag, theID, theRemoteActorsAddress);
}

int
XC::Shadow::recvID(ID &theID)
{
    return theChannel->recvID(0, commitTag, theID, theRemoteActorsAddress);
}


void XC::Shadow::setCommitTag(int tag)
  { commitTag = tag; }


XC::Channel *XC::Shadow::getChannelPtr(void) const
  { return theChannel; }

XC::FEM_ObjectBroker *XC::Shadow::getObjectBrokerPtr(void) const
  { return theBroker; }

XC::ChannelAddress *XC::Shadow::getActorAddressPtr(void) const
  { return theRemoteActorsAddress; }



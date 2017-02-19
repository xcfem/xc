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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/23 18:24:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/actor/Actor.cpp,v $
                                                                        
                                                                        
// Written: fmk
// Revision: A
//
// Purpose: This file contains the implementation of XC::Actor.
//
// What: "@(#) Actor.C, revA"

#include "utility/actor/actor/Actor.h"
#include "../channel/Channel.h"
#include "../address/ChannelAddress.h"
#include "../message/Message.h"
#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"

//! @brief Libera la memoria reservada por este objeto.
void XC::Actor::libera(void)
  {
    const size_t sz= actorMethods.size();
    for(size_t i=0; i<sz;i++)
      if(actorMethods[i]!=nullptr)
        {
          delete actorMethods[i];
          actorMethods[i]= nullptr;
        }
    actorMethods.clear();
  }

//! @brief Reserva memoria para pointers to actores.
void XC::Actor::alloc(const std::vector<ActorMethod *> &otro)
  {
    libera();
    const size_t sz= otro.size();
    actorMethods= std::vector<ActorMethod *>(sz,nullptr);
    for(size_t i=0; i<sz;i++)
      if(otro[i]!=nullptr)
        actorMethods[i]=new ActorMethod(*otro[i]);
  }

int XC::Actor::alloc_method(const ActorMethod &am)
  {
    int retval= 0;
    //add the new method
    ActorMethod *newMethod=new ActorMethod(am);
    if(!newMethod)
      retval= -3;
    
    if(actorMethods[numMethods])
      delete actorMethods[numMethods];
    actorMethods[numMethods]= newMethod;
    numMethods++;
    return retval;
  }


//! @brief Constructor to init the list.
XC::Actor::Actor(Channel &theChan,FEM_ObjectBroker &myBroker,int numActorMethods)
  : numMethods(0), maxNumMethods(numActorMethods), actorMethods(numActorMethods,static_cast<ActorMethod *>(nullptr)), 
   theRemoteShadowsAddress(0), commitTag(0), theBroker(&myBroker), theChannel(&theChan)
  {
    // call setUpActor on the channel and get shadows address
    theChannel->setUpConnection();
    theRemoteShadowsAddress = theChan.getLastSendersAddress();

    if(actorMethods.empty())
      maxNumMethods = 0;
  
  }

//! @brief Copy constructor.
XC::Actor::Actor(const Actor &otro)
  : numMethods(otro.numMethods), maxNumMethods(otro.maxNumMethods), actorMethods(), 
   theRemoteShadowsAddress(otro.theRemoteShadowsAddress), commitTag(otro.commitTag),
   theBroker(otro.theBroker), theChannel(otro.theChannel)
  {
    alloc(otro.actorMethods);
  }

//! @brief Assignment operator.
XC::Actor &XC::Actor::operator=(const Actor &otro)
  {
    numMethods= otro.numMethods;
    maxNumMethods= otro.maxNumMethods;
    alloc(otro.actorMethods);
    theRemoteShadowsAddress= otro.theRemoteShadowsAddress;
    commitTag= otro.commitTag;
    theBroker= otro.theBroker;
    theChannel= otro.theChannel;
    return *this;
  }

//! @brief Destructor.
XC::Actor::~Actor(void)
  { libera(); }



//! @brief Method to add a function to the list of avaiable actor methods.
//! The function will be identified as tag, it is a function with
//! no args that returns an int.
int XC::Actor::addMethod(int tag, int (*fp)())
  {
    int retval= 0;
    // check we are not over our limit
    if(numMethods >= maxNumMethods)
      retval= -2;
    else
      {
        // check no other with the same tag exists
        ActorMethod *methodPtr;
        for(int i=0; i<numMethods; i++)
          {
	    methodPtr = actorMethods[i];
	    if(methodPtr->tag == tag)
              {
	        retval= -1;
                break;
              }
          }
        retval= alloc_method(ActorMethod(tag,fp));
      }
    return retval;
  }





//! @brief Method to return the integer tag of the next method the actor
//! has been asked to invoke.
int XC::Actor::getMethod(void)
  {
    int method = -1;
    Message msg(&method,1);
    this->recvMessage(msg);
    return method;
  }



//! @brief Method to process the function whose id is tag.
int XC::Actor::processMethod(int tag)
  {
    ActorMethod *current =0;

    for(int i=0; i<numMethods; i++)
      if(actorMethods[i]->tag == tag)
        {current = actorMethods[tag];}
    if(current == 0)
      return -1;
    return (*current).theMethod();
  }


//! @brief Sends object theObject through the channel being passed as parameter.
int XC::Actor::sendObject(MovableObject &theObject, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->sendObj(commitTag, theObject,theRemoteShadowsAddress);
    else
      return theChannel->sendObj(commitTag, theObject,theAddress);	
  }

//! @brief Receives object theObject through the channel being passed as parameter.
int XC::Actor::recvObject(MovableObject &theObject, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->recvObj(commitTag, theObject,*theBroker, theRemoteShadowsAddress); 
    else
      return theChannel->recvObj(commitTag, theObject,*theBroker,theAddress);	
  }


//! @brief Recibe el mensaje through the channel being passed as parameter.
int XC::Actor::recvMessage(Message &theMessage, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->recvMsg(0, commitTag, theMessage,theRemoteShadowsAddress);
    else
      return theChannel->recvMsg(0, commitTag, theMessage,theAddress);	
  }

//! @brief Envía el mensaje through the channel being passed as parameter.
int XC::Actor::sendMessage(const Message &theMessage, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->sendMsg(0, commitTag, theMessage,theRemoteShadowsAddress);
    else
      return theChannel->sendMsg(0, commitTag, theMessage,theAddress);	
  }



//! @brief Envía la matriz through the channel being passed as parameter.
int XC::Actor::sendMatrix(const Matrix &theMatrix, ChannelAddress *theAddress )
  {
    if(theAddress)    
      return theChannel->sendMatrix(0, commitTag, theMatrix,theRemoteShadowsAddress);
    else
      return theChannel->sendMatrix(0, commitTag, theMatrix,theAddress);	
  }

//! @brief Recibe la matriz through the channel being passed as parameter.
int XC::Actor::recvMatrix(Matrix &theMatrix, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->recvMatrix(0, commitTag, theMatrix,theRemoteShadowsAddress);
    else
      return theChannel->recvMatrix(0, commitTag, theMatrix,theAddress);	
  }

//! @brief Envía el vector through the channel being passed as parameter.
int XC::Actor::sendVector(const XC::Vector &theVector, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->sendVector(0, commitTag, theVector,theRemoteShadowsAddress);
    else
      return theChannel->sendVector(0, commitTag, theVector,theAddress);	
  }

//! @brief Recibe el vector through the channel being passed as parameter.
int XC::Actor::recvVector(Vector &theVector, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->recvVector(0, commitTag, theVector,theRemoteShadowsAddress);
    else
      return theChannel->recvVector(0, commitTag, theVector,theAddress);	
  }

//! @brief Envía el vector de enteros through the channel being passed as parameter.
int XC::Actor::sendID(const XC::ID &theID, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->sendID(0, commitTag, theID,theRemoteShadowsAddress);
    else
      return theChannel->sendID(0, commitTag, theID,theAddress);	
  }

//! @brief Recibe el vector de enteros through the channel being passed as parameter.
int XC::Actor::recvID(ID &theID, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->recvID(0, commitTag, theID,theRemoteShadowsAddress);
    else
      return theChannel->recvID(0, commitTag, theID,theAddress);	
  }


void XC::Actor::setCommitTag(int tag)
  { commitTag = tag; }


//! @brief Returns a pointer to the canal.
XC::Channel *XC::Actor::getChannelPtr(void) const
  { return theChannel; }

//! @brief Returns a pointer to the broker.
XC::FEM_ObjectBroker *XC::Actor::getObjectBrokerPtr(void) const
  { return theBroker; }



XC::ChannelAddress *XC::Actor::getShadowsAddressPtr(void) const
  { return theRemoteShadowsAddress; }


//! @brief barrier check:
int XC::Actor::barrierCheck(int myResult = 0)
  {
    int result;
    static ID data(1);
    data(0) = myResult; 
    theChannel->sendID(0, commitTag, data); 
    theChannel->recvID(0, commitTag, data);
    result = data(0);
    return result;
  }

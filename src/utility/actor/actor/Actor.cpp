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

//! @brief Frees memory.
void XC::Actor::free_memory(void)
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

//! @brief Allocates memory for ActorMethod pointers.
void XC::Actor::alloc(const std::vector<ActorMethod *> &other)
  {
    free_memory();
    const size_t sz= other.size();
    actorMethods= std::vector<ActorMethod *>(sz,nullptr);
    for(size_t i=0; i<sz;i++)
      if(other[i]!=nullptr)
        actorMethods[i]=new ActorMethod(*other[i]);
  }

//! @brief Allocates memory for a copy of the argument.
int XC::Actor::alloc_method(const ActorMethod &am)
  {
    int retval= 0;
    //add the new method
    ActorMethod *newMethod= am.getCopy();
    if(!newMethod)
      retval= -3;

    const ActorMethod *old= actorMethods[numMethods];
    if(old)
      delete old;
    actorMethods[numMethods]= newMethod;
    numMethods++;
    return retval;
  }


//! @brief Constructor to init the list.
//!
//!This is called by the remote process upon initialization to construct
//! the local actor object. It is used to create an Actor object in that
//! remote address space which will communicate with objects in other
//! processes through a channel object, \p theChan and which uses
//! \p myBroker to receive movable objects sent from other
//! processes. The subclass will be able to add at most \p numActorMethods actor
//! methods using addMethod() call.
//! The base classes constructor invokes setUpActor() on #theChannel object.
//! It then sets the Address of the remote shadow
//! object which created the actor process by invoking 
//! getLastSendersAddress() on #theChannel.
XC::Actor::Actor(Channel &theChan,FEM_ObjectBroker &myBroker,int numActorMethods)
  : ShadowActorBase(theChan, myBroker), numMethods(0), maxNumMethods(numActorMethods),
    actorMethods(numActorMethods,static_cast<ActorMethod *>(nullptr)), 
    theRemoteShadowsAddress(0)
  {
    // call setUpActor on the channel and get shadows address
    theRemoteShadowsAddress = theChan.getLastSendersAddress();

    if(actorMethods.empty())
      maxNumMethods = 0;
  
  }

//! @brief Copy constructor.
XC::Actor::Actor(const Actor &other)
  : ShadowActorBase(other), numMethods(other.numMethods),
    maxNumMethods(other.maxNumMethods), actorMethods(), 
   theRemoteShadowsAddress(other.theRemoteShadowsAddress)
  {
    alloc(other.actorMethods);
  }

//! @brief Assignment operator.
XC::Actor &XC::Actor::operator=(const Actor &other)
  {
    ShadowActorBase::operator=(other);
    numMethods= other.numMethods;
    maxNumMethods= other.maxNumMethods;
    alloc(other.actorMethods);
    theRemoteShadowsAddress= other.theRemoteShadowsAddress;
    return *this;
  }

//! @brief Destructor.
XC::Actor::~Actor(void)
  { free_memory(); }



//! @brief Method to add a function to the list of avaiable actor methods.
//!
//! Aadd as a function to the actor object the function \p fp,
//! this function is identified by the \p tag value. This
//! function will be invoked by the actor on invocation of  
//! processMethod()} with \p tag as the argument. The object checks to
//! see that the \p tag has not been used previously. If it has not and
//! the number of functions so far added is less than #numActorMethods
//! the function is added and $0$ is returned, otherwise a $-1$ (if \p tag
//! was already used) or $-2$ (if #numActorMethods already
//! added) or a $-3$ (if running out of space) is returned to indicate the
//! function was not added. 
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
//!
//! A method which returns the next integer value sitting in the actors
//! channel. This int value corresponds to the tag of the next
//! method that the shadow object wants the actor to perform. If an error
//! occurs $-1$ will be returned. 
int XC::Actor::getMethod(void)
  {
    int method = -1;
    Message msg(&method,1);
    this->recvMessage(msg);
    return method;
  }



//! @brief Method to process the function whose id is tag.
//!
//! This causes the actor object to invoke the function that was added to
//! the actor with the {\em tag} identifier. If no method with {\em tag}
//! exists a $-1$ is returned.
int XC::Actor::processMethod(int tag)
  {
    ActorMethod *current= nullptr;

    for(int i=0; i<numMethods; i++)
      if(actorMethods[i]->tag == tag)
        {current = actorMethods[tag];}
    if(current == 0)
      return -1;
    return (*current).theMethod();
  }


//! @brief Sends object \p theObject.
//!
//! A method which will send #theObject
//! through the actors channel either to the address given by \p theAddress
//! or to the address of the shadow object that created the
//! actor if no address is specified.
//! Returns the result of invoking sendObj(0, theObject,theBroker,theAddress)
//! on the actors channel object if an address is specified, otherwise the
//! result of invoking sendObj(theMessage,theBroker,this->getShadowAdressPtr())
//! on the actors channel object is returned.
int XC::Actor::sendObject(MovableObject &theObject, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->sendObj(getCommitTag(), theObject,theRemoteShadowsAddress);
    else
      return theChannel->sendObj(getCommitTag(), theObject,theAddress);	
  }

//! @brief Receives object \p theObject.
//!
//! A method which will receive \p theObject
//! from the actors channel either from the address given by \p theAddress
//! or from the address of the shadow object that created the
//! actor if no address is specified.
//! Returns the result of invoking recvObj(0, theObject,theBroker,theAddress)}
//! on the actors channel object if an address is specified, otherwise the
//! result of invoking recvObj(theMessage,theBroker,this->getShadowAdressPtr())
//! on the actors channel object is returned.
int XC::Actor::recvObject(MovableObject &theObject, ChannelAddress *theAddress)
  {
    if(theAddress)
      return theChannel->recvObj(getCommitTag(), theObject,*theBroker, theRemoteShadowsAddress); 
    else
      return theChannel->recvObj(getCommitTag(), theObject,*theBroker,theAddress);	
  }

//! @brief Sends the message.
//!
//! A method which will send the data in the \p theMessage 
//! through the actors channel either to the address given by \p theAddress
//! or to the address of the shadow object that created the
//! actor if no address is specified.
//! Returns the result of invoking sendMsg(0,0,theMessage,theAddress) on the
//! actors channel object if an address is specified, otherwise the result
//! of invoking sendMsg(theMessage,this->getShadowAdressPtr())} on
//! the actors channel object is returned.
int XC::Actor::sendMessage(const Message &theMessage, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->sendMsg(0, getCommitTag(), theMessage,theRemoteShadowsAddress);
    else
      return theChannel->sendMsg(0, getCommitTag(), theMessage,theAddress);	
  }


//! @brief Receives the message.
//!
//! A method which will receive the data in the \p theMessage from
//! the actors channel either from the address given by \p theAddress or from
//! the address of the shadow object that created the actor if no address
//! is specified.
//! Returns the result of invoking recvMsg(0,0,theMessage,theAddress)} on
//! the actors channel object if an address is specified, otherwise the result
//! of invoking recvMsg(0,0,theMessage,this->getShadowAdressPtr())} on the
//! actors channel object is returned.
int XC::Actor::recvMessage(Message &theMessage, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->recvMsg(0, getCommitTag(), theMessage,theRemoteShadowsAddress);
    else
      return theChannel->recvMsg(0, getCommitTag(), theMessage,theAddress);	
  }




//! @brief Sends the matrix.
//!
//! A method which will send \p theMatrix through the actors channel
//! either to the address given by \p theAddress or to the address of
//! the shadow object that created the actor if no address is specified.
//! Returns the result of invoking sendMatrix(0,0,theMatrix,theAddress)} on
//! the actors channel object if an address is specified, otherwise the result
//! of invoking sendMatrix(0,0,theMatrix,this->getShadowAdressPtr())
//! on the actors channel object is returned. 
int XC::Actor::sendMatrix(const Matrix &theMatrix, ChannelAddress *theAddress )
  {
    if(theAddress)    
      return theChannel->sendMatrix(0, getCommitTag(), theMatrix,theRemoteShadowsAddress);
    else
      return theChannel->sendMatrix(0, getCommitTag(), theMatrix,theAddress);	
  }

//! @brief Receives the matrix.
//!
//! A method which will receive \p theMatrix from the actors channel either
//! from the address given by \p theAddress or from the address of the shadow
//! object that created the actor if no address is specified.
//! Returns the result of invoking recvMatrix(0,0,theMatrix,theAddress) on the
//! actors channel object if an address is specified, otherwise the result of
//! invoking recvMatrix(0,0,theMatrix,this->getShadowAdressPtr()) on the actors
//! channel object is returned.
int XC::Actor::recvMatrix(Matrix &theMatrix, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->recvMatrix(0, getCommitTag(), theMatrix,theRemoteShadowsAddress);
    else
      return theChannel->recvMatrix(0, getCommitTag(), theMatrix,theAddress);	
  }

//! @brief Sends the vector.
//!
//! A method which will send \p theVector through the actors channel either
//! to the address given by \p theAddress or to the address of the shadow
//! object that created the actor if no address is specified.
//! Returns the result of invoking sendVector(0,0,theVector,theAddress) on the
//! actors channel object if an address is specified, otherwise the result of
//! invoking sendVector(0,0,theVector,this->getShadowAdressPtr())} on the
//! actors channel object is returned.
int XC::Actor::sendVector(const XC::Vector &theVector, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->sendVector(0, getCommitTag(), theVector,theRemoteShadowsAddress);
    else
      return theChannel->sendVector(0, getCommitTag(), theVector,theAddress);	
  }

//! @brief Receive the vector.
//!
//! A method which will receive \p theVector from the actors channel either
//! from the address given by \p theAddress or from the address of the shadow
//! object that created the actor if no address is specified.
//! Returns the result of invoking recvVector(0,0,theVector,theAddress) on the
//! actors channel object if an address is specified, otherwise the result of
//! invoking recvVector(0,0,theVector,this->getShadowAdressPtr()) on the actors
//! channel object is returned.
int XC::Actor::recvVector(Vector &theVector, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->recvVector(0, getCommitTag(), theVector,theRemoteShadowsAddress);
    else
      return theChannel->recvVector(0, getCommitTag(), theVector,theAddress);	
  }

//! @brief Send the vector of integers\p theID.
//!
//! A method which will send \p theID through the actors channel either
//! to the address given by \p theAddress or to the address of the shadow
//! object that created the actor if no address is specified.
//! Returns the result of invoking sendID(0,0,theID,theAddress) on the
//! actors channel object if an address is specified, otherwise the result of
//! invoking sendID(0,0,theID,this->getShadowAdressPtr())} on the
//! actors channel object is returned.
int XC::Actor::sendID(const ID &theID, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->sendID(0, getCommitTag(), theID,theRemoteShadowsAddress);
    else
      return theChannel->sendID(0, getCommitTag(), theID,theAddress);	
  }

//! @brief Receive the integer vector\p theID.
//!
//! A method which will receive \p theID from the actors channel either
//! from the address given by \p theAddress or from the address of the shadow
//! object that created the actor if no address is specified.
//! Returns the result of invoking recvID(0,0,theID,theAddress) on the
//! actors channel object if an address is specified, otherwise the result of
//! invoking recvID(0,0,theID,this->getShadowAdressPtr()) on the actors
//! channel object is returned.
int XC::Actor::recvID(ID &theID, ChannelAddress *theAddress )
  {
    if(theAddress)
      return theChannel->recvID(0, getCommitTag(), theID,theRemoteShadowsAddress);
    else
      return theChannel->recvID(0, getCommitTag(), theID,theAddress);	
  }

//! @brief Return a pointer to the channel address for the shadow
//! object that created the actor.
XC::ChannelAddress *XC::Actor::getShadowsAddressPtr(void) const
  { return theRemoteShadowsAddress; }



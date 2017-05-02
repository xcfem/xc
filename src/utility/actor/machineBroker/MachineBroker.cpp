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
                                                                        
// $Revision: 1.2 $
// $Date: 2005/11/23 18:33:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/MachineBroker.cpp,v $
                                                                        
// Written: fmk
// Revision: A

#include "MachineBroker.h"
#include "../channel/Channel.h"

#include "../actor/Actor.h"
#include "../objectBroker/FEM_ObjectBroker.h"


//! @brief Constructor. Set the object broker.
XC::MachineBroker::MachineBroker(FEM_ObjectBroker *theBroker)
  : ObjectWithObjBroker(theBroker), actorChannels(0), numActorChannels(0), numActiveChannels(0), activeChannels() {}

//! @brief Destructor.
XC::MachineBroker::~MachineBroker(void)
  {}


int XC::MachineBroker::shutdown(void)
  {  
    // send the termination notice to all machineBrokers running actorProcesses
    for(int i=0; i<numActorChannels; i++)
      {
        ID idData(1);
        idData(0) = 0;
        Channel *theChannel= actorChannels[i];
        if(theChannel->sendID(0, 0, idData) < 0)
          { std::cerr << "MachineBroker::shutdown(void) - failed to send XC::ID\n"; }

        if(theChannel->recvID(0, 0, idData) < 0)
          { std::cerr << "MachineBroker::shutdown(void) - failed to recv XC::ID\n"; }

        this->freeProcess(theChannel);
      }

    if(!actorChannels.empty())
      {
        activeChannels.resize(0);
        numActorChannels = 0;
        numActiveChannels = 0;
      }
    return 0;
  }


int XC::MachineBroker::runActors(void)
  {
    Channel *theChannel = this->getMyChannel();
    ID idData(1);
    int done = 0;

    // loop until recv kill signal
    while(done == 0)
      {
        if(theChannel->recvID(0, 0, idData) < 0)
          { std::cerr << "MachineBroker::runActors(void) - failed to recv XC::ID\n"; }

        const int actorType = idData(0);
    
        // switch on data type
        if(idData(0) == 0)
          {
            done = 1;
            if(theChannel->sendID(0, 0, idData) < 0)
              { std::cerr << "MachineBroker::run(void) - failed to send XC::ID\n"; }
            return 0;
          }
        else
          {
            // create an actor of approriate type
            Actor *theActor= getObjectBrokerPtr()->getNewActor(actorType, theChannel);
            if(!theActor)
              {
                std::cerr << "MachineBroker::run(void) - invalid actor type\n";
                idData(0) = 1;
              }
            else
              idData(0) = 0;

            // send ID back indicating wheter actor was created 
            if(theChannel->sendID(0, 0, idData) < 0)
              { std::cerr << "MachineBroker::run(void) - failed to send XC::ID\n"; }

            // run the actor object
            if(theActor->run() != 0)
              { std::cerr << "MachineBroker::run(void) - actor failed while running\n"; }  
            // destroying theActor
            delete theActor;
          }
        done = 0;
      }
    return 0;
  }
//! @brief Invoked to start the program.
//! 
//! @brief Invoked to start the program, #actorProgram, on the parallel
//! machine. The remote actor process uses information supplied by 
//! #theChannel to allow the remote process to connect to the local
//! process. The integer \p compDemand provides an indication of the
//! computational demands of the remote process in a heterogeneous
//! environment. 
XC::Channel *XC::MachineBroker::startActor(int actorType, int compDemand)
  {
    if(compDemand != 0) 
      std::cerr << "MachineBroker::startActor() - does not take computational demand variable into account\n";

    Channel *theChannel= nullptr;

    // check if have an available machine broker running runActors() and waiting to start an actor running
    if(numActiveChannels < numActorChannels)
      {
        for(int i=0; i<numActorChannels; i++)
          {
            if(activeChannels(i) == 0)
              {
                theChannel = actorChannels[i];
                i=numActorChannels;
                numActiveChannels++;
                activeChannels(i) = 1;
              }
          }
      }
    //No hay conexión disponible, establece una nueva.
    if(!theChannel)
      {
        theChannel= this->getRemoteProcess();

        if(!theChannel)
          {
            std::cerr << "MachineBroker::startActor() - no available channel available\n";
            return 0;
          }
        actorChannels.resize(numActorChannels+1,nullptr);
        activeChannels.resize(numActorChannels+1);

        numActorChannels++;
        numActiveChannels++;    
      }

     // now that we have a channel to a machine broker waiting to run some actor processes, start the actor
     ID idData(1);
     idData(0) = actorType;
     if(theChannel->sendID(0, 0, idData) != 0)
       {
         std::cerr << "MachineBroker::startActor() - failed to send actorType\n";
         this->freeProcess(theChannel);
         return 0;    
       }

     if(theChannel->recvID(0, 0, idData) != 0)
       {
         std::cerr << "MachineBroker::startActor() - remote process failure\n";
         return 0;
       }
     if(idData(0) != 0)
       {
         std::cerr << "MachineBroker::startActor() - could not start actorType: " << actorType << std::endl;
         this->freeProcess(theChannel);
         return 0;    
       }
     return theChannel;
  }

int XC::MachineBroker::finishedWithActor(Channel *theChannel)
  {
    // send the termination notice to all machineBrokers running actorProcesses
    for(int i=0; i<numActorChannels; i++)
      {
        if(theChannel == actorChannels[i])
          {
            numActiveChannels--;
            activeChannels(i)= 0;
            return 0;
          }
      }
    // if get here .. startActor() not called or subclass override
    return -1;
  }

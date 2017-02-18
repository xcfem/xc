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
// $Source: /usr/local/cvs/OpenSees/SRC/actor/actor/Actor.h,v $
                                                                        
                                                                        
#ifndef Actor_h
#define Actor_h

// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for Actor.
// Actor is meant to be an abstract base class and thus no objects of it's type
// can be instantiated. 
//
// What: "@(#) Actor.h, revA"

#include <vector>

namespace XC {

class ObjectBroker;
class Message;
class Channel;
class ChannelAddress;
class MovableObject;
class Matrix;
class Vector;
class ID;
class FEM_ObjectBroker;

class ActorMethod
  {
  public:
    int tag;
    int (*theMethod)();
    inline ActorMethod(int tg, int (*f)())
      : tag(tg), theMethod(f) {}
  };

//! @ingroup IPComm
//
//! @brief 
class Actor
  {
  private:	
    int numMethods, maxNumMethods;
    std::vector<ActorMethod *> actorMethods;
    ChannelAddress *theRemoteShadowsAddress;

    int commitTag;

    void libera(void);
    void alloc(const std::vector<ActorMethod *> &otro);
    int alloc_method(const ActorMethod &);

  protected:
    FEM_ObjectBroker *theBroker; 
    Channel *theChannel;
  public:
    Actor(Channel &theChannel, FEM_ObjectBroker &theBroker,int numActorMethods =0);
    Actor(const Actor &);
    Actor &operator=(const Actor &);
    virtual ~Actor(void);
    
    virtual int  run(void) = 0;

    virtual int  addMethod(int tag, int (*fp)());
    virtual int  getMethod();
    virtual int  processMethod(int tag);

    virtual int sendObject(MovableObject &theObject, ChannelAddress *theAddress =0);  
    virtual int recvObject(MovableObject &theObject, ChannelAddress *theAddress =0);

    virtual int sendMessage(const Message &theMessage, ChannelAddress *theAddress =0);   
    virtual int recvMessage(Message &theMessage, ChannelAddress *theAddress =0);  
    
    virtual int sendMatrix(const Matrix &theMatrix, ChannelAddress *theAddress =0);   
    virtual int recvMatrix(Matrix &theMatrix, ChannelAddress *theAddress =0);  
    
    virtual int sendVector(const Vector &theVector, ChannelAddress *theAddress =0);   
    virtual int recvVector(Vector &theVector, ChannelAddress *theAddress =0);  
    
    virtual int sendID(const ID &theID, ChannelAddress *theAddress =0);   
    virtual int recvID(ID &theID, ChannelAddress *theAddress =0);  

    Channel *getChannelPtr(void) const;
    FEM_ObjectBroker *getObjectBrokerPtr(void) const;    
    ChannelAddress *getShadowsAddressPtr(void) const;            

    virtual int barrierCheck(int result);
    void setCommitTag(int commitTag);
  };

} // end of XC namespace

#endif


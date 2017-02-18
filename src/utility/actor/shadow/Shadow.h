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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/11/23 18:25:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/shadow/Shadow.h,v $
                                                                        
                                                                        
// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for Shadow.
// Shadow is meant to be an abstract base class and thus no objects of it's type
// can be instantiated. 
//
// What: "@(#) Shadow.h, revA"

#ifndef Shadow_h
#define Shadow_h



namespace XC {
class MachineBroker;
class Message;
class Channel;
class ChannelAddress;
class MovableObject;
class Matrix;
class Vector;
class ID;

class FEM_ObjectBroker;

//! @ingroup IPComm
//
class Shadow
  {
  private:
    ChannelAddress *theRemoteActorsAddress;    
    int commitTag;
  protected:
    Channel *theChannel;    
    FEM_ObjectBroker *theBroker;
  public:
    Shadow(Channel &theChannel, FEM_ObjectBroker &);

    Shadow(Channel &theChannel, FEM_ObjectBroker &, ChannelAddress &theAddress);
    
    Shadow(int actorType, FEM_ObjectBroker &, MachineBroker &, int compDemand);

    virtual int sendObject(MovableObject &theObject);  
    virtual int recvObject(MovableObject &theObject);      
    virtual int sendMessage(const Message &theMessage);  
    virtual int recvMessage(Message &theMessage);  
    virtual int sendMatrix(const Matrix &theMatrix);  
    virtual int recvMatrix(Matrix &theMatrix);      
    virtual int sendVector(const Vector &theVector);  
    virtual int recvVector(Vector &theVector);      
    virtual int sendID(const ID &theID);  
    virtual int recvID(ID &theID);      
    void setCommitTag(int commitTag);

    Channel 		  *getChannelPtr(void) const;
    FEM_ObjectBroker 	  *getObjectBrokerPtr(void) const;        
    ChannelAddress        *getActorAddressPtr(void) const;
  };
} // end of XC namespace

#endif

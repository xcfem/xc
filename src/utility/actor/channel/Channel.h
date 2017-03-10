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
// $Date: 2005/11/23 18:27:24 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/Channel.h,v $
                                                                        
                                                                        
#ifndef Channel_h
#define Channel_h

// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for Channel.
// Channel is an abstract base class which defines the channel interface.
// A channel is a point of communication in a program, a mailbox to/from
// which data enters/leaves a program.
//
// What: "@(#) Channel.h, revA"


#include "xc_utils/src/nucleo/EntCmd.h"
#include <set>

namespace XC {
class ChannelAddress;
class Message;
class MovableObject;
class Matrix;
class Vector;
class ID;
class FEM_ObjectBroker;

//! @ingroup IPComm
//
//! @brief Channel is an abstract base class which defines the channel
//! interface.
//! A channel is a point of communication in a program, a mailbox to/from
//! which data enters/leaves a program.
class Channel: public EntCmd
  {
  private:
    static int numChannel;
    int tag; //! channel identifier;
    std::set<int> usedDbTags;//!< dbTags already used.
  protected:
    int sendMovable(int commitTag, MovableObject &);
    int receiveMovable(int commitTag, MovableObject &, FEM_ObjectBroker &);
  public:
    Channel(void);
    inline virtual ~Channel(void) {}

    // methods to set up the channel in an actor space
    virtual char *addToProgram(void) =0;
    virtual int setUpConnection(void) =0;
    virtual int setNextAddress(const ChannelAddress &theAddress) =0;
    virtual ChannelAddress *getLastSendersAddress(void) =0;

    virtual bool isDatastore(void) const;
    virtual int getDbTag(void) const;
    bool checkDbTag(const int &dbTag);
    const ID &getUsedTags(void) const;
    void clearDbTags(void);
    int getTag(void) const;
    
    // methods to send/receive messages and objects on channels.
    virtual int sendObj(int commitTag, MovableObject &, ChannelAddress *theAddress= nullptr) =0;
    virtual int recvObj(int commitTag, MovableObject &, FEM_ObjectBroker &, ChannelAddress *theAddress= nullptr) =0;
    template <class inputIterator>
    int sendObjs(int commitTag,const inputIterator &first,const inputIterator &last,ChannelAddress *theAddress= nullptr);
    template <class inputIterator>
    int recvObjs(int commitTag,const inputIterator &first,const inputIterator &last, FEM_ObjectBroker &, ChannelAddress *theAddress= nullptr);

    virtual int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress= nullptr) =0;  
    virtual int recvMsg(int dbTag, int commitTag, Message &,ChannelAddress *theAddress= nullptr) =0;  
    
    virtual int sendMatrix(int dbTag, int commitTag, const Matrix &,ChannelAddress *theAddress= nullptr) =0;  
    virtual int recvMatrix(int dbTag, int commitTag, Matrix &, ChannelAddress *theAddress= nullptr) =0;  
 
    virtual int sendVector(int dbTag, int commitTag, const Vector &, ChannelAddress *theAddress= nullptr) =0;
    virtual int recvVector(int dbTag, int commitTag, Vector &, ChannelAddress *theAddress= nullptr) =0;

    virtual int sendID(int dbTag, int commitTag,const ID &, ChannelAddress *theAddress= nullptr) =0;  
    virtual int recvID(int dbTag, int commitTag,ID &, ChannelAddress *theAddress= nullptr) =0;      
  };

//! @brief Envía una serie de objetos.
template <class inputIterator>
int Channel::sendObjs(int commitTag,const inputIterator &first,const inputIterator &last,ChannelAddress *theAddress)
  {
    int retval= 0;
    for(inputIterator i= first;i!=last;i++)
      {
        retval= sendObj(commitTag,*i,theAddress);
        if(retval!=0)
          break;
      }
    return retval;
  }

//! @brief Recibe una serie de objetos.
template <class inputIterator>
int Channel::recvObjs(int commitTag,const inputIterator &first,const inputIterator &last, FEM_ObjectBroker &ob, ChannelAddress *theAddress)
  {
    int retval= 0;
    for(inputIterator i= first;i!=last;i++)
      {
        retval= recvObj(commitTag,*i,ob,theAddress);
        if(retval!=0)
          break;
      }
    return retval;
  }

} // end of XC namespace

#endif

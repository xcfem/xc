// -*-c++-*-
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


#include "utility/kernel/CommandEntity.h"
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
//!
//! is a point of communication in a program, a
//! mailbox to/from which data enters/leaves a program. Channels are
//! objects through which the objects in the current processes address
//! space can interact with objects in another processes address space. A
//! channel in one process space is associated with a channel in the
//! address space of another process space. The interaction is in the form
//! of data sent between the two processes along the connection line.
class Channel: public CommandEntity
  {
  private:
    static int numChannel; //!< Number of channels.
    int tag; //!< channel identifier;
    std::set<int> usedDbTags;//!< dbTags already used.
  protected:
    int sendMovable(int commitTag, MovableObject &);
    int receiveMovable(int commitTag, MovableObject &, FEM_ObjectBroker &);
  public:
    Channel(CommandEntity *owr= nullptr);
    inline virtual ~Channel(void) {}

    // methods to set up the channel in an actor space
    //! @brief When creating remote actors the channels created in the actor
    //! space need to know how to contact the shadows channels. This
    //! information is provided in the string returned from this method. It
    //! is used by the machine broker when starting the remote process. It
    //! places this information as the last arguments to the program.
    virtual char *addToProgram(void) =0;
    virtual int setUpConnection(void) =0;
    //! @brief A method invoked to set specify the next address that the next
    //! messages to be sent if {\em sendMessage()} or received if {\em
    //! recvMessage()} is invoked with a null pointer.
    virtual int setNextAddress(const ChannelAddress &theAddress) =0;
    virtual ChannelAddress *getLastSendersAddress(void) =0;

    virtual bool isDatastore(void) const;
    virtual int getDbTag(void) const;
    bool checkDbTag(const int &dbTag);
    const ID &getUsedTags(void) const;
    void clearDbTags(void);
    int getTag(void) const;
    
    // methods to send/receive messages and objects on channels.
    //! @brief To send the object \p theObj and the commit tag \p commitTag
    //! to a remote Channel whose address is given by \p theAddress. If
    //! \p theAddress is 0, the Channel sends to the Channel with the
    //! address last set in a send..(), recv..(), or {\p
    //! setNextAddress() operation. To return 0 if successful, a negative
    //! number if not.
    virtual int sendObj(int commitTag, MovableObject &theObj, ChannelAddress *theAddress= nullptr) =0;
    //! @biref To receive the object \p theObj with the commit tag \p commitTag
    //! from a remote Channel whose address is given by \p theAddress. If
    //! \p theAddress is 0, the Channel receives from the Channel with the
    //! address last set in a send..(), recv..()}, or setNextAddress()
    //! operation. To return 0 if successful, a negative number if not.
    virtual int recvObj(int commitTag, MovableObject &theObj, FEM_ObjectBroker &theBroker, ChannelAddress *theAddress= nullptr) =0;
    template <class inputIterator>
    int sendObjs(int commitTag,const inputIterator &first,const inputIterator &last,ChannelAddress *theAddress= nullptr);
    template <class inputIterator>
    int recvObjs(int commitTag,const inputIterator &first,const inputIterator &last, FEM_ObjectBroker &, ChannelAddress *theAddress= nullptr);

    //! @brief A method invoked to send the data in the Message object \p
    //! theMsg to another Channel object. The object will obtain the
    //! data and size of the data to be sent by invoking getData() and
    //! getSize() on theMsg. The channel object is then
    //! responsible for sending that data to the remote channel address given
    //! by \p theAddress. If \p theAddress is 0, the Channel sends to
    //! the Channel with the address last set in a send..()}, recv..(),
    //! or setNextAddress() operation. To return 0 if successful, a
    //! negative number if not.
    virtual int sendMsg(int dbTag, int commitTag, const Message &theMsg, ChannelAddress *theAddress= nullptr) =0;
    
    //! @brief Invoked to send the data in the Message object \p
    //! theMsg to another Channel object. The object will obtain the
    //! the size of the data that is being received by invoking getSize()
    //! on theMsg. The channel object is then responsible for
    //! receiving that amount of data from the channel whose address is given
    //! by \p theAddress. If \p theAddress is 0, the Channel receives from
    //! the Channel with the address last set in a send..(), recv..(),
    //! or setNextAddress() operation. To return 0 if successful, a
    //! negative number if not.
    virtual int recvMsg(int dbTag, int commitTag, Message &theMsg, ChannelAddress *theAddress= nullptr) =0;  

    //! @brief Invoked to receive the data in the Matrix object \p
    //! theMatrix to another Channel object. The object will obtain the
    //! data and size of the data to be sent by invoking getData() and
    //! getSize() on theMatrix. The channel object is then
    //! responsible for sending that data to the remote channel address given
    //! by \p theAddress. If \p theAddress is 0, the Channel sends to
    //! the Channel with the address last set in a send..()}, recv..(),
    //! or setNextAddress() operation. To return 0 if successful, a
    //! negative number if not.
    virtual int sendMatrix(int dbTag, int commitTag, const Matrix &theMatrix,ChannelAddress *theAddress= nullptr) =0;  

    //! @brief Invoked to receive the data in the Matrix object \p
    //! theMatrix to another Channel object. The object will obtain the
    //! data and size of the data to be sent by invoking getData() and
    //! getSize() on theMatrix. The channel object is then
    //! responsible for sending that data to the remote channel address given
    //! by \p theAddress. If \p theAddress is 0, the Channel sends to
    //! the Channel with the address last set in a send..()}, recv..(),
    //! or setNextAddress() operation. To return 0 if successful, a
    //! negative number if not.
    virtual int recvMatrix(int dbTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress= nullptr) =0;  
 
    //! @brief Invoked to receive the data in the Vector object \p
    //! theVector to another Channel object. The object will obtain the
    //! data and size of the data to be sent by invoking getData() and
    //! getSize() on theVector. The channel object is then
    //! responsible for sending that data to the remote channel address given
    //! by \p theAddress. If \p theAddress is 0, the Channel sends to
    //! the Channel with the address last set in a send..()}, recv..(),
    //! or setNextAddress() operation. To return 0 if successful, a
    //! negative number if not.
    virtual int sendVector(int dbTag, int commitTag, const Vector &theVector, ChannelAddress *theAddress= nullptr) =0;

    //! @brief Invoked to receive the data in the Vector object \p
    //! theVector to another Channel object. The object will obtain the
    //! data and size of the data to be sent by invoking getData() and
    //! getSize() on theVector. The channel object is then
    //! responsible for sending that data to the remote channel address given
    //! by \p theAddress. If \p theAddress is 0, the Channel sends to
    //! the Channel with the address last set in a send..()}, recv..(),
    //! or setNextAddress() operation. To return 0 if successful, a
    //! negative number if not.
    virtual int recvVector(int dbTag, int commitTag, Vector &theVector, ChannelAddress *theAddress= nullptr) =0;

    //! @brief Invoked to receive the data in the ID object \p
    //! theID to another Channel object. The object will obtain the
    //! data and size of the data to be sent by invoking getData() and
    //! getSize() on theID. The channel object is then
    //! responsible for sending that data to the remote channel address given
    //! by \p theAddress. If \p theAddress is 0, the Channel sends to
    //! the Channel with the address last set in a send..()}, recv..(),
    //! or setNextAddress() operation. To return 0 if successful, a
    //! negative number if not.
    virtual int sendID(int dbTag, int commitTag,const ID &theID, ChannelAddress *theAddress= nullptr) =0;  

    //! @brief Invoked to receive the data in the ID object \p
    //! theID to another Channel object. The object will obtain the
    //! data and size of the data to be sent by invoking getData() and
    //! getSize() on theID. The channel object is then
    //! responsible for sending that data to the remote channel address given
    //! by \p theAddress. If \p theAddress is 0, the Channel sends to
    //! the Channel with the address last set in a send..()}, recv..(),
    //! or setNextAddress() operation. To return 0 if successful, a
    //! negative number if not.
    virtual int recvID(int dbTag, int commitTag,ID &theID, ChannelAddress *theAddress= nullptr) =0;      
  };

//! @brief Send the objects on interval [first,last).
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

//! @brief Receive an object sequence.
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

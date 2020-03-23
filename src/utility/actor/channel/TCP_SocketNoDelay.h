// -*-c++-*-
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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/TCP_SocketNoDelay.h,v $
                                                                        
                                                                        
// File: ~/actor/TCP_SocketNoDelay.h
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for TCP_SocketNoDelay.

#ifndef TCP_SocketNoDelay_h
#define TCP_SocketNoDelay_h

#include "../channel/Socket.h"
#include "../channel/Channel.h"
#include "../address/SocketAddress.h"

namespace XC {
//! @ingroup IPComm
//
//! @brief TCP_SocketNoDelay is a sub-class of channel. It is implemented with Berkeley
//! stream sockets using the TCP protocol. Messages delivery is garaunteed. 
//! Communication is full-duplex between a pair of connected sockets.
class TCP_SocketNoDelay: public Channel
  {
  private:
    int sockfd;
    struct sockaddr_in  my_Addr, other_Addr;
    int addrLength;
    unsigned int myPort;
    char add[40];
  public:
    TCP_SocketNoDelay();        
    TCP_SocketNoDelay(unsigned int);    
    TCP_SocketNoDelay(unsigned int other_Port, char *other_InetAddr); 
    ~TCP_SocketNoDelay();

    virtual int setUpShadow(void);
    virtual int setUpActor(void);

    int setNextAddress(const ChannelAddress &otherChannelAddress);
    virtual ChannelAddress *getLastSendersAddress(void){ return 0;};

    int sendObj(MovableObject &theObject, FEM_ObjectBroker &, ChannelAddress *theAddress =0);
    int recvObj(MovableObject &theObject, FEM_ObjectBroker &, ChannelAddress *theAddress =0);
		
    int recvMsg(Message &, ChannelAddress *theAddress =0);       
    int sendMsg(const Message &, ChannelAddress *theAddress =0);    

    int sendMatrix(const Matrix &theMatrix, ChannelAddress *theAddress =0);
    int recvMatrix(Matrix &theMatrix, ChannelAddress *theAddress =0);
    
    int sendVector(const Vector &theVector, ChannelAddress *theAddress =0);
    int recvVector(Vector &theVector, ChannelAddress *theAddress =0);
    
    int sendID(const ID &theID, ChannelAddress *theAddress =0);
    int recvID(ID &theID, ChannelAddress *theAddress =0);    
    
    unsigned int getPortNumber(void) const;
    char *addToProgram(void);
  };
} // end of XC namespace


#endif 






 


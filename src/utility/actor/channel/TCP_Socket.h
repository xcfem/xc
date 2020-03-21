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
// $Date: 2005/11/23 23:43:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/TCP_Socket.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// What: "@(#) TCP_Socket.h, revA"

#ifndef TCP_Socket_h
#define TCP_Socket_h

#include "utility/actor/channel/Socket.h"
#include "utility/actor/channel/Channel.h"
#include "../address/SocketAddress.h"

namespace XC {
//! @ingroup IPComm
//
//! @brief TCP_Socket is a sub-class of channel. It is implemented with Berkeley
//! stream sockets using the TCP protocol. Messages delivery is garaunteed. 
//! Communication is full-duplex between a pair of connected sockets.
class TCP_Socket: public Channel
  {
  private:
    socket_type sockfd;

    union {
      struct sockaddr    addr;
      struct sockaddr_in addr_in;
    } my_Addr;
    union {
      struct sockaddr    addr;
      struct sockaddr_in addr_in;
    } other_Addr;

    socklen_type addrLength;

    unsigned int myPort;
    int connectType;

    char add[40];
  protected:
    unsigned int getPortNumber(void) const;
  public:
    TCP_Socket();        
    TCP_Socket(unsigned int);    
    TCP_Socket(unsigned int other_Port, char *other_InetAddr); 
    ~TCP_Socket();

    char *addToProgram(void);
    
    virtual int setUpConnection(void);

    int setNextAddress(const ChannelAddress &otherChannelAddress);
    virtual ChannelAddress *getLastSendersAddress(void){ return 0;};

    int sendObj(int commitTag, MovableObject &, ChannelAddress *theAddress =0);
    int recvObj(int commitTag, MovableObject &, FEM_ObjectBroker &, ChannelAddress *theAddress =0);
		
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress =0);    
    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress =0);        

    int sendMatrix(int dbTag, int commitTag, const Matrix &, ChannelAddress *theAddress =0);
    int recvMatrix(int dbTag, int commitTag, Matrix &, ChannelAddress *theAddress =0);
    
    int sendVector(int dbTag, int commitTag, const Vector &, ChannelAddress *theAddress =0);
    int recvVector(int dbTag, int commitTag, Vector &, ChannelAddress *theAddress =0);
    
    int sendID(int dbTag, int commitTag, const ID &, ChannelAddress *theAddress =0);
    int recvID(int dbTag, int commitTag, ID &, ChannelAddress *theAddress =0);    
  };
} // end of XC namespace


#endif 






 


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

#ifndef TCP_UDP_Socket_base_h
#define TCP_UDP_Socket_base_h

#include "utility/actor/channel/Socket.h"
#include "utility/actor/channel/Channel.h"

namespace XC {
//! @ingroup IPComm
//
//! @brief TCP_UDP_Socket_base is a sub-class of channel. Base class for
//! TCP and UDP socket classes.
class TCP_UDP_Socket_base: public Channel
  {
  protected:
    socket_type sockfd;
    int connectType;

    union
      {
        struct sockaddr    addr;
        struct sockaddr_in addr_in;
      } my_Addr;
    union
      {
        struct sockaddr    addr;
        struct sockaddr_in addr_in;
      } other_Addr;
    
    socklen_type addrLength;
    unsigned int myPort;

    char add[40];
    bool checkEndianness;
    bool endiannessProblem;

    unsigned int getPortNumber(void) const;
    std::string getHostName(void) const;
    virtual void checkForEndiannessProblem(void);
    static int getHostAddress(const std::string &, char *);
    static void int2a(unsigned int , char *, int *);
    void setup(unsigned int , int );
  public:
    TCP_UDP_Socket_base(int connectType= 0, bool checkEndianness = false);    
    ~TCP_UDP_Socket_base();
  };
} // end of XC namespace


#endif 






 


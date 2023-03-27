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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/23 23:43:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/TCP_UDP_Socket_base.cpp,v $
                                                                        
                                                                        
// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of the methods needed
// to define the TCP_UDP_Socket_base class interface.
//
// What: "@(#) TCP_UDP_Socket_base.C, revA"

#include "utility/actor/channel/TCP_UDP_Socket_base.h"
#include "utility/actor/channel/Socket.h"
#include <cstring>
#include "../message/Message.h"
#include "../address/ChannelAddress.h"
#include "../actor/MovableObject.h"
#include "boost/asio/ip/host_name.hpp"

static int GetHostAddr(char *host, char *IntAddr);
static void inttoa(unsigned int no, char *string, int *cnt);

// TCP_UDP_Socket_base(unsigned int other_Port, char *other_InetAddr): 
// 	constructor to open a socket with my inet_addr and with a port number 
//	given by the OS. 

void XC::TCP_UDP_Socket_base::setup(unsigned int port, int socketType)
  {
    // set up my_Addr.addr_in
    my_Addr.addr_in.sin_family = AF_INET;
    my_Addr.addr_in.sin_port = htons(port);
    my_Addr.addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    addrLength = sizeof(my_Addr.addr);
    
    // open a socket
    if((sockfd = socket(AF_INET, socketType, 0)) < 0)
      {
 	std::cerr << "XC::TCP_UDP_Socket_base::TCP_UDP_Socket_base - could not open socket\n";
      }

    // bind local address to it
    if(bind(sockfd, (struct sockaddr *) &my_Addr.addr_in, sizeof(my_Addr.addr_in)) < 0)
      {
	std::cerr << "XC::TCP_UDP_Socket_base::TCP_UDP_Socket_base - could not bind local address\n";
      }
    
    // get my_address info
    getsockname(sockfd, &my_Addr.addr, &addrLength);
    myPort = ntohs(my_Addr.addr_in.sin_port);
  }    

//! @brief Constructor to open a socket with my inet_addr and with a
//! port number port.
XC::TCP_UDP_Socket_base::TCP_UDP_Socket_base(int connectTyp, bool checkEnd)
  : sockfd(0), connectType(connectTyp), myPort(0),
    checkEndianness(checkEnd), endiannessProblem(false)
  {
    // initialize sockets
    startup_sockets();    
  }





// ~TCP_UDP_Socket_base():
//	destructor

XC::TCP_UDP_Socket_base::~TCP_UDP_Socket_base(void)
  {
    close(sockfd);
    // cleanup sockets
    cleanup_sockets();
  }

std::string XC::TCP_UDP_Socket_base::getHostName(void) const
  {
    return boost::asio::ip::host_name();
  }


unsigned int XC::TCP_UDP_Socket_base::getPortNumber(void) const
  { return myPort; }

void XC::TCP_UDP_Socket_base::checkForEndiannessProblem(void)
  {
    std::cerr << "TCP_UDP_Socket_base::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
  }

//! @brief Is a static method to get the internet address of a host
//! Takes machine name host & Returns 0 if o.k,  -1 if gethostbyname 
//! error, -2 otherwise. The internet address is returned in IntAddr
int XC::TCP_UDP_Socket_base::getHostAddress(const std::string &host, char *IntAddr)
  {
    char *tmp= const_cast<char *>(host.c_str());
    return GetHostAddr(tmp, IntAddr);
  }

//! @brief Static method to convert int to ascii
void XC::TCP_UDP_Socket_base::int2a(unsigned int no, char *string, int *cnt)
  { inttoa(no, string, cnt); }


//! @brief GetHostAddr is a function to get the internet address of a host
//! Takes machine name host & Returns 0 if o.k,  -1 if gethostbyname 
//! error, -2 otherwise. The internet address is returned in IntAddr
static int GetHostAddr(char *host, char *IntAddr)
  {
    struct hostent *hostptr;

    if ( (hostptr = gethostbyname(host)) == nullptr) 
	return (-1);

    switch(hostptr->h_addrtype) {
      case AF_INET:
	strcpy(IntAddr,inet_ntoa(*(struct in_addr *)*hostptr->h_addr_list));
	return (0);
	
      default:
	return (-2);
    }
  }

    
//! @brief Function to convert int to ascii
static void inttoa(unsigned int no, char *string, int *cnt)
  {
    if (no /10) {
        inttoa(no/10, string, cnt);
        *cnt = *cnt+1;
    }
    string[*cnt] = no % 10 + '0';
  }


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
// $Date: 2003/10/15 00:31:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/UDP_Socket.cpp,v $
                                                                        
                                                                        
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the implementation of the methods needed
// to define the UDP_Socket class interface, given below:
//

#include "utility/actor/channel/UDP_Socket.h"
#include <strings.h>
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"
#include "../message/Message.h"
#include "../address/SocketAddress.h"
#include "../actor/MovableObject.h"
#include "boost/asio/ip/host_name.hpp"

static int GetHostAddr(const std::string &, char *IntAddr);
static void inttoa(unsigned int no, char *string, int *cnt);

// UDP_Socket(): 
//	constructor to open a socket with my inet_addr and with a port number 
//	assigned by the OS from the available free port numbers.

XC::UDP_Socket::UDP_Socket(void) 
  :sockfd(0), connectType(0), shadow_inetAddr(0), shadow_port(0) 
{
    // set up my_Addr 
    my_Addr.addr_in.sin_family = AF_INET;
    my_Addr.addr_in.sin_addr.s_addr = htonl(INADDR_ANY); 
    my_Addr.addr_in.sin_port = htons(0);
    addrLength = sizeof(my_Addr.addr_in);
    
    // open a socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	std::cerr << "XC::UDP_Socket::UDP_Socket - could not open socket\n";
    }
    
    // bind local address to it
    if (bind(sockfd, (struct sockaddr *) &my_Addr.addr_in,sizeof(my_Addr.addr_in)) < 0) {
	std::cerr << "XC::UDP_Socket::UDP_Socket - could not bind local address\n";
    }    
    
    getsockname(sockfd, &my_Addr.addr, &addrLength);
    myPort = ntohs(my_Addr.addr_in.sin_port);        
}    




// UDP_Socket(unsigned int port): 
//	constructor to open a socket with my inet_addr and with a port number port.

XC::UDP_Socket::UDP_Socket(unsigned int port) 
  :sockfd(0), connectType(0), shadow_inetAddr(0), shadow_port(0)
{
    // set up my_Addr.addr_in 
    my_Addr.addr_in.sin_family = AF_INET;
    my_Addr.addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    my_Addr.addr_in.sin_port = htons(port);
    addrLength = sizeof(my_Addr.addr_in);
    
    // open a socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	std::cerr << "XC::UDP_Socket::UDP_Socket - could not open socket\n";
    }
    
    // bind local address to it
    if (bind(sockfd, (struct sockaddr *) &my_Addr.addr_in,sizeof(my_Addr.addr_in)) < 0) {
	std::cerr << "XC::UDP_Socket::UDP_Socket - could not bind local address\n";
    }    
    getsockname(sockfd, &my_Addr.addr, &addrLength);    
    myPort = ntohs(my_Addr.addr_in.sin_port);        
}    




// UDP_Socket(unsigned int other_Port, char *other_InetAddr): 
// 	constructor to open a socket with my inet_addr and with a port number 
//	given by the OS. It then sends a messege comprising of a single character
// 	to an address given by other_InetAddr and other_Port. This is to allow
// 	a shadow object to find address of the actor it initiatites.

XC::UDP_Socket::UDP_Socket(unsigned int other_Port, char *other_InetAddr) 
:sockfd(0), connectType(1), shadow_inetAddr(other_InetAddr), shadow_port(other_Port)
{
    // set up my_Addr.addr_in 
    my_Addr.addr_in.sin_family = AF_INET;
    my_Addr.addr_in.sin_addr.s_addr = htonl(INADDR_ANY); 
    my_Addr.addr_in.sin_port = htons(0);
    addrLength = sizeof(my_Addr.addr_in);
    
    // open a socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	std::cerr << "XC::UDP_Socket::UDP_Socket - could not open socket\n";
    }
    
    // bind local address to it
    if (bind(sockfd, (struct sockaddr *) &my_Addr.addr_in,sizeof(my_Addr.addr_in)) < 0) {
	std::cerr << "XC::UDP_Socket::UDP_Socket - could not bind local address\n";
    }    
    getsockname(sockfd, &my_Addr.addr, &addrLength);    
    myPort = ntohs(my_Addr.addr_in.sin_port);        
}    

    


// ! @brief Destructor.
XC::UDP_Socket::~UDP_Socket(void)
  { close(sockfd); }


int XC::UDP_Socket::setUpConnection(void)
  {
  if (connectType == 1) {
    if (shadow_inetAddr == 0) {
	std::cerr << "XC::FATAL::XC::UDP_Socket::setUpActor() -";
	std::cerr << "incorrect choice of UDP_Socket constructor for actor.\n";
	exit(-1);
    }

    // send a message to address.
    SocketAddress other(shadow_inetAddr, shadow_port);
    char data = 'a';
    sendto(sockfd, &data, 1, 0, &other.address.addr, other.addrLength);    
    bcopy((char *) &other.address.addr, (char *) &last_Addr, addrLength);
    
    // receive a message from other
    recvfrom(sockfd, &data, 1, 0, &last_Addr.addr, &addrLength);    

  } else {

    // wait for remote process to send message;
    char data = 'b';
    recvfrom(sockfd, &data, 1, 0, &last_Addr.addr, &addrLength);    
    
    // then send a message back
    sendto(sockfd, &data, 1, 0, &last_Addr.addr, addrLength);        
  }

  return 0;    
}    


int
XC::UDP_Socket::setNextAddress(const XC::ChannelAddress &theAddress)
{	
    SocketAddress *theSocketAddress = 0;
    if (theAddress.getType() == SOCKET_TYPE) {
	theSocketAddress = (SocketAddress *)(&theAddress);    
	// set up the address of the Socket to which data will be sent
	
	bcopy((char *) &theSocketAddress->address.addr, (char *) &last_Addr, 
	      theSocketAddress->addrLength);
	addrLength = theSocketAddress->addrLength;	

	return  0;	    
    }
    else {
	std::cerr << "XC::TCP_Socket::setNextAddress() - a UDP_Socket ";
	std::cerr << "can only communicate with a UDP_Socket";
	std::cerr << " address given is not of type XC::SocketAddress\n"; 
	return -1;	    
    }		    	
}

//! @brief Envía el objeto a través de éste canal.
int XC::UDP_Socket::sendObj(int commitTag, MovableObject &theObject, ChannelAddress *theAddress) 
  {
    // set the address of last_Addr
    SocketAddress *theSocketAddress = 0;
    if(theAddress != 0)
      {
	if(theAddress->getType() == SOCKET_TYPE)
          {
	    theSocketAddress = (SocketAddress *)theAddress;
	
	    bcopy((char *) &theSocketAddress->address.addr, (char *) &last_Addr.addr, theSocketAddress->addrLength);
	    addrLength = theSocketAddress->addrLength;
	  }
	else
          {
	    std::cerr << "XC::UDP_Socket::sendObj() - a UDP_Socket ";
	    std::cerr << "can only communicate with a UDP_Socket";
	    std::cerr << " address given is not of type XC::SocketAddress\n"; 
	    return -1;	    
	  }		    	    
      }

    // now get the object to send itself
    return sendMovable(commitTag,theObject);
  }


//! @brief Recibe el objeto a través de éste canal.
int XC::UDP_Socket::recvObj(int commitTag, MovableObject &theObject, FEM_ObjectBroker &theBroker, ChannelAddress *theAddress) 
  {

    int res = receiveMovable(commitTag,theObject, theBroker);
    if (res < 0) 
	return res;
    
    // check the address that message came from was correct
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	

	    if (bcmp((char *) &theSocketAddress->address.addr, (char *) &last_Addr.addr, 
		     theSocketAddress->addrLength) != 0) {
	    
		std::cerr << "XC::UDP_Socket::recvObj() - a UDP_Socket ";
		std::cerr << "can only look at first incoming message\n"; 
		std::cerr << "The last message did not come from write scource\n";
		return -1;
	    }	
	}
	else {
	    std::cerr << "XC::UDP_Socket::recvObj() - a UDP_Socket ";
	    std::cerr << "can only communicate with a UDP_Socket";
	    std::cerr << " address given is not of type XC::SocketAddress\n"; 
	    return -1;	    
	}		    	    	    
    }
    return 0;
}





int XC::UDP_Socket::recvMsg(int dbTag, int commitTag, Message &msg, ChannelAddress *theAddress )
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en UDP_Socket::recvMsg" << std::endl;
    // if o.k. get a ponter to the data in the message and 
    // place the incoming data there
    int size;
    char *gMsg;
    gMsg = msg.data;
    size = msg.length;

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    recvfrom(sockfd, gMsg, size, 0, &last_Addr.addr, &addrLength);
	    size = 0;
	}
	else {
	  recvfrom(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &last_Addr.addr, &addrLength);
		   
	    
	  gMsg += MAX_UDP_DATAGRAM;
	  size -= MAX_UDP_DATAGRAM;
	}
    }

    // check the address that message came from was correct
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	    
	    if (bcmp((char *) &theSocketAddress->address.addr, (char *) &last_Addr.addr, 
		     theSocketAddress->addrLength) != 0) {
		
		std::cerr << "XC::UDP_Socket::recvMsg() - a UDP_Socket ";
		std::cerr << "can only look at first incoming message\n"; 
		std::cerr << "The last message did not come from write scource\n";
		return -1;
	    }
	}	
	else {
	    std::cerr << "XC::UDP_Socket::sendObj() - a UDP_Socket ";
	    std::cerr << "can only communicate with a UDP_Socket";
	    std::cerr << " address given is not of type XC::SocketAddress\n"; 
	    return -1;	    
	}		    	    	    
    }
    return 0;
}


int XC::UDP_Socket::sendMsg(int dbTag, int commitTag, const Message &msg, ChannelAddress *theAddress )
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en UDP_Socket::sendMsg" << std::endl;
    // set up the address of the Socket to which data will be sent
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	
	    bcopy((char *) &theSocketAddress->address.addr, (char *) &last_Addr.addr, 
		  theSocketAddress->addrLength);
	    addrLength = theSocketAddress->addrLength;	
	}
	else {
	    std::cerr << "XC::UDP_Socket::sendMsg() - a UDP_Socket ";
	    std::cerr << "can only communicate with a UDP_Socket";
	    std::cerr << " address given is not of type XC::SocketAddress\n"; 
	    return -1;	    
	}		    	    	    	    
    }
    
    // send the data
    int size; 
    char *gMsg;
    gMsg = msg.data;
    size = msg.length;

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    sendto(sockfd, gMsg, size, 0, &last_Addr.addr, addrLength);
	    size = 0;
	}
	else {	
	  sendto(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &last_Addr.addr, addrLength);
	  gMsg += MAX_UDP_DATAGRAM;
	  size -= MAX_UDP_DATAGRAM;
	}
    }
    return 0;
}






int XC::UDP_Socket::recvMatrix(int dbTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress )
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en UDP_Socket::recvMatrix" << std::endl;
    // if o.k. get a ponter to the data in the message and 
    // place the incoming data there
    double *data= theMatrix.getDataPtr();
    char *gMsg= reinterpret_cast<char *>(data);
    int size=  theMatrix.getNumBytes();

    while(size > 0)
      {
	if(size <= MAX_UDP_DATAGRAM)
          {
	    recvfrom(sockfd, gMsg, size, 0, &last_Addr.addr, &addrLength);
	    size = 0;
	  }
	else
          {
	    recvfrom(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &last_Addr.addr, &addrLength);
	    gMsg += MAX_UDP_DATAGRAM;
	    size -= MAX_UDP_DATAGRAM;
	  }
      }

    // check the address that message came from was correct
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	    
	    if (bcmp((char *) &theSocketAddress->address.addr, (char *) &last_Addr.addr, 
		     theSocketAddress->addrLength) != 0) {
		
		std::cerr << "XC::UDP_Socket::recvMsg() - a UDP_Socket ";
		std::cerr << "can only look at first incoming message\n"; 
		std::cerr << "The last message did not come from write scource\n";
		return -1;
	    }
	}	
	else {
	    std::cerr << "XC::UDP_Socket::recvMatrix() - a UDP_Socket ";
	    std::cerr << "can only communicate with a UDP_Socket";
	    std::cerr << " address given is not of type XC::SocketAddress\n"; 
	    return -1;	    
	}		    	    	    
    }
    
    
    return 0;
}


int XC::UDP_Socket::sendMatrix(int dbTag, int commitTag, const XC::Matrix &theMatrix, ChannelAddress *theAddress )
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en UDP_Socket::sendMatrix" << std::endl;
    // set up the address of the Socket to which data will be sent
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	
	    bcopy((char *) &theSocketAddress->address.addr, (char *) &last_Addr.addr, 
		  theSocketAddress->addrLength);
	    addrLength = theSocketAddress->addrLength;	
	}
	else {
	    std::cerr << "XC::UDP_Socket::sendMatrix() - a UDP_Socket ";
	    std::cerr << "can only communicate with a UDP_Socket";
	    std::cerr << " address given is not of type XC::SocketAddress\n"; 
	    return -1;	    
	}		    	    	    	    
    }
    
    // send the data
    const double *data = theMatrix.getDataPtr();
    const char *gMsg = reinterpret_cast<const char *>(data);
    int size =  theMatrix.getNumBytes();

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    sendto(sockfd, gMsg, size, 0, &last_Addr.addr, addrLength);
	    size = 0;
	}
	else {
	    sendto(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &last_Addr.addr, addrLength);
			
	    gMsg += MAX_UDP_DATAGRAM;
	    size -= MAX_UDP_DATAGRAM;
	}
    }
    return 0;
}






int XC::UDP_Socket::recvVector(int dbTag, int commitTag, Vector &theVector, ChannelAddress *theAddress )
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en UDP_Socket::recvVector" << std::endl;

    // if o.k. get a ponter to the data in the message and 
    // place the incoming data there
    int size;
    double *data = theVector.theData;
    char *gMsg = (char *)data;;
    size =  theVector.sz * sizeof(double);

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    recvfrom(sockfd, gMsg, size, 0, &last_Addr.addr, &addrLength);
	    size = 0;
	}
	else {
	    recvfrom(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &last_Addr.addr, &addrLength);
	    
	    gMsg += MAX_UDP_DATAGRAM;
	    size -= MAX_UDP_DATAGRAM;
	}
    }

    // check the address that message came from was correct
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	    
	    if (bcmp((char *) &theSocketAddress->address.addr, (char *) &last_Addr.addr, 
		     theSocketAddress->addrLength) != 0) {
		
		std::cerr << "XC::UDP_Socket::recvMsg() - a UDP_Socket ";
		std::cerr << "can only look at first incoming message\n"; 
		std::cerr << "The last message did not come from write scource\n";
		return -1;
	    }
	}	
	else {
	    std::cerr << "XC::UDP_Socket::recvVector() - a UDP_Socket ";
	    std::cerr << "can only communicate with a UDP_Socket";
	    std::cerr << " address given is not of type XC::SocketAddress\n"; 
	    return -1;	    
	}		    	    	    
    }
    
    return 0;
}


int XC::UDP_Socket::sendVector(int dbTag, int commitTag, const XC::Vector &theVector, ChannelAddress *theAddress )
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en UDP_Socket::sendVector" << std::endl;
    // set up the address of the Socket to which data will be sent
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	
	    bcopy((char *) &theSocketAddress->address.addr, (char *) &last_Addr.addr, 
		  theSocketAddress->addrLength);
	    addrLength = theSocketAddress->addrLength;	
	}
	else {
	    std::cerr << "XC::UDP_Socket::sendVector() - a UDP_Socket ";
	    std::cerr << "can only communicate with a UDP_Socket";
	    std::cerr << " address given is not of type XC::SocketAddress\n"; 
	    return -1;	    
	}		    	    	    	    
    }
    
    // send the data
    int size; 
    double *data = theVector.theData;
    char *gMsg = (char *)data;;
    size =  theVector.sz * sizeof(double);

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    sendto(sockfd, gMsg, size, 0, &last_Addr.addr, addrLength);
	    size = 0;
	}
	else {
	    sendto(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &last_Addr.addr, addrLength);
	    gMsg += MAX_UDP_DATAGRAM;
	    size -= MAX_UDP_DATAGRAM;
	}
    }
    return 0;
}






int XC::UDP_Socket::recvID(int dbTag, int commitTag, ID &theID, ChannelAddress *theAddress )
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en UDP_Socket::recvID" << std::endl;

    // if o.k. get a ponter to the data in the message and 
    // place the incoming data there
    int *data = theID.getDataPtr();
    char *gMsg = (char *)data;;
    int size=  theID.Size() * sizeof(int);

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    recvfrom(sockfd,gMsg,size,0,&last_Addr.addr,&addrLength);
	    size = 0;
	}
	else {
	    recvfrom(sockfd,gMsg,MAX_UDP_DATAGRAM,0, &last_Addr.addr,&addrLength);
			  
	    
	    gMsg += MAX_UDP_DATAGRAM;
	    size -= MAX_UDP_DATAGRAM;
	}
    }

    // check the address that message came from was correct
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	    
	    if (bcmp((char *) &theSocketAddress->address.addr, (char *) &last_Addr.addr, 
		     theSocketAddress->addrLength) != 0) {
		
		std::cerr << "XC::UDP_Socket::recvMsg() - a UDP_Socket ";
		std::cerr << "can only look at first incoming message\n"; 
		std::cerr << "The last message did not come from write scource\n";
		return -1;
	    }
	}	
	else {
	    std::cerr << "XC::UDP_Socket::recvID() - a UDP_Socket ";
	    std::cerr << "can only communicate with a UDP_Socket";
	    std::cerr << " address given is not of type XC::SocketAddress\n"; 
	    return -1;	    
	}		    	    	    
    }
    return 0;
}


int XC::UDP_Socket::sendID(int dbTag, int commitTag, const XC::ID &theID, ChannelAddress *theAddress )
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en UDP_Socket::sendID" << std::endl;
    // set up the address of the Socket to which data will be sent
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	
	    bcopy((char *) &theSocketAddress->address.addr, (char *) &last_Addr.addr, 
		  theSocketAddress->addrLength);
	    addrLength = theSocketAddress->addrLength;	
	}
	else {
	    std::cerr << "XC::UDP_Socket::sendID() - a UDP_Socket ";
	    std::cerr << "can only communicate with a UDP_Socket";
	    std::cerr << " address given is not of type XC::SocketAddress\n"; 
	    return -1;	    
	}		    	    	    	    
    }
    
    // send the data 
    const int *data = theID.getDataPtr();
    char *gMsg = (char *)data;;
    int size =  theID.Size() * sizeof(int);

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    sendto(sockfd,gMsg,size,0,&last_Addr.addr,addrLength);
	    size = 0;
	}
	else {
	    sendto(sockfd,gMsg,MAX_UDP_DATAGRAM,0,
			&last_Addr.addr, addrLength);
	    gMsg += MAX_UDP_DATAGRAM;
	    size -= MAX_UDP_DATAGRAM;
	}
    }
    return 0;
}


unsigned int 
XC::UDP_Socket::getPortNumber(void) const
{
    return myPort;
}



char *XC::UDP_Socket::addToProgram(void)
  {
    const char *tcp = " 2 ";
    
    //char  me[20];
    char  my_InetAddr[MAX_INET_ADDR];
    char  myPortNum[8];
    unsigned int thePort = this->getPortNumber();

    int start = 0;
    inttoa(thePort,myPortNum,&start);
    const std::string me= boost::asio::ip::host_name(); //gethostname(me,MAX_INET_ADDR);
    GetHostAddr(me,my_InetAddr);

    strcpy(add,tcp);
    strcat(add," ");          
    strcat(add,my_InetAddr);
    strcat(add," ");
    strcat(add,myPortNum);
    strcat(add," ");    
    
    return add;
  }



//! @brief GetHostAddr is a function to get the internet address of a host
//! Takes machine name host & Returns 0 if o.k,  -1 if gethostbyname 
//! error, -2 otherwise. The internet address is returned in IntAddr

static int GetHostAddr(const std::string &host, char *IntAddr)
  {
    register struct hostent *hostptr;

    if ( (hostptr = gethostbyname(host.c_str())) == nullptr) 
	return (-1);

    switch(hostptr->h_addrtype)
      {
      case AF_INET:
	strcpy(IntAddr,inet_ntoa(*(struct in_addr *)*hostptr->h_addr_list));
	return (0);

      default:
	return (-2);
      }
}


    
/*
 *  i n t t o a
 *
 *  Function to convert int to ascii
 *  
 */

static void inttoa(unsigned int no, char *string, int *cnt) {
    if (no /10) {
        inttoa(no/10, string, cnt);
        *cnt = *cnt+1;
    }
    string[*cnt] = no % 10 + '0';
}


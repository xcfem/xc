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

// UDP_Socket(): 
//	constructor to open a socket with my inet_addr and with a port number 
//	assigned by the OS from the available free port numbers.

XC::UDP_Socket::UDP_Socket(bool checkendianness) 
  : TCP_UDP_Socket_base(0, checkendianness), shadow_inetAddr(nullptr), shadow_port(0) 
  {
    bzero((char *) &my_Addr, sizeof(my_Addr));
    this->setup(0, SOCK_DGRAM);
  }    




// UDP_Socket(unsigned int port): 
//	constructor to open a socket with my inet_addr and with a port number port.

XC::UDP_Socket::UDP_Socket(unsigned int port, bool checkendianness) 
  : TCP_UDP_Socket_base(0, checkendianness), shadow_inetAddr(nullptr), shadow_port(0)
  {
    bzero((char *) &my_Addr, sizeof(my_Addr));
    this->setup(port, SOCK_DGRAM);
  }    




//! @brief UDP_Socket(unsigned int other_Port, char *other_InetAddr): 
//! constructor to open a socket with my inet_addr and with a port number 
//! given by the OS. It then sends a message comprising of a single character
//! to an address given by other_InetAddr and other_Port. This is to allow
//! a shadow object to find address of the actor it initiates.
XC::UDP_Socket::UDP_Socket(unsigned int other_Port, char *other_InetAddr, bool checkendianness) 
  : TCP_UDP_Socket_base(1, checkendianness), shadow_inetAddr(other_InetAddr), shadow_port(other_Port)
  {
    // set up remote address
    bzero((char *) &other_Addr, sizeof(other_Addr));
    other_Addr.addr_in.sin_family = AF_INET;
    other_Addr.addr_in.sin_port = htons(other_Port);
    
#ifdef _WIN32
    other_Addr.addr_in.sin_addr.S_un.S_addr = inet_addr(other_InetAddr);
#else
    other_Addr.addr_in.sin_addr.s_addr = inet_addr(other_InetAddr);
#endif
    
    // set up my_Addr.addr_in
    bzero((char *) &my_Addr, sizeof(my_Addr));
    this->setup(0, SOCK_DGRAM);
  }

void XC::UDP_Socket::checkForEndiannessProblem(void)
  {
    if(checkEndianness)
      {
	int i = 1;
	int j;

	int *data = &i;
	char *gMsg = (char *)data;
	sendto(sockfd, gMsg, 1, 0, &other_Addr.addr, addrLength);

	data = &j;
	gMsg = (char *)data;
	recvfrom(sockfd, gMsg, 1, 0, &other_Addr.addr, &addrLength);

	if (i != j)
	  {
	    int k = 0x41424344;
	    char *c = (char *)&k;

	    if (*c == 0x41)
	      {
		endiannessProblem = true;
	      }
	  }
      }
  }

int XC::UDP_Socket::setUpConnection(void)
  {
    int ierr, trial;
    char data;
    
    if(connectType == 1)
      {  
        // send a 1-byte message to address (try 3-times)
        data = 'a';
        addrLength = sizeof(other_Addr.addr);
        trial = 0;
        do
	  {
            ierr = sendto(sockfd, &data, 1, 0, &other_Addr.addr, addrLength);
            trial++;
          }
	while (ierr != 1 && trial < 3);
        if(ierr != 1)
	  {
            std::cerr << "UDP_Socket::setUpConnection() - client could not send intial message\n";
            return -1;
          }
        
        // receive a 1-byte message from other (try 3-times)
        trial = 0;
        do
	  {
            ierr = recvfrom(sockfd, &data, 1, 0, &other_Addr.addr, &addrLength);
            trial++;
          }
	while (ierr != 1 && data != 'b' && trial < 3);
        if(ierr != 1)
	  {
            std::cerr << "UDP_Socket::setUpConnection() - client could not receive intial message\n";
            return -1;
          }
        
        // check for endianness problem if requested
        checkForEndiannessProblem();
        
      }
    else
      {
        
        // wait for remote process to send a 1-byte message (try 3-times)
        addrLength = sizeof(other_Addr.addr);
        trial = 0;
        do {
            ierr = recvfrom(sockfd, &data, 1, 0, &other_Addr.addr, &addrLength);
            trial++;
        } while (ierr != 1 && data != 'a' && trial < 3);
        if (ierr != 1) {
            std::cerr << "UDP_Socket::setUpConnection() - server could not receive intial message\n";
            return -1;
        }
        
        // then send a 1-byte message back (try 3-times)
        data = 'b';
        trial = 0;
        do {
            ierr = sendto(sockfd, &data, 1, 0, &other_Addr.addr, addrLength);
            trial++;
        } while (ierr != 1 && trial < 3);
        if (ierr != 1) {
            std::cerr << "UDP_Socket::setUpConnection() - server could not send intial message\n";
            return -1;
        }
        
        // check for endianness problem if requested
        checkForEndiannessProblem();
    }
    
    return 0;
  }    


int XC::UDP_Socket::setNextAddress(const XC::ChannelAddress &theAddress)
  {	
    SocketAddress *theSocketAddress = nullptr;
    if (theAddress.getType() == SOCKET_TYPE) {
	theSocketAddress = (SocketAddress *)(&theAddress);    
	// set up the address of the Socket to which data will be sent
	
	bcopy((char *) &theSocketAddress->address.addr, (char *) &other_Addr, 
	      theSocketAddress->addrLength);
	addrLength = theSocketAddress->addrLength;	

	return  0;	    
    }
    else {
	std::cerr << "XC::UDP_Socket::setNextAddress() - a UDP_Socket ";
	std::cerr << "can only communicate with a UDP_Socket";
	std::cerr << " address given is not of type XC::SocketAddress\n"; 
	return -1;	    
    }		    	
}

//! @brief Send the object a través de éste canal.
int XC::UDP_Socket::sendObj(int commitTag, MovableObject &theObject, ChannelAddress *theAddress) 
  {
    // set the address of other_Addr
    SocketAddress *theSocketAddress = 0;
    if(theAddress != 0)
      {
	if(theAddress->getType() == SOCKET_TYPE)
          {
	    theSocketAddress = (SocketAddress *)theAddress;
	
	    bcopy((char *) &theSocketAddress->address.addr, (char *) &other_Addr.addr, theSocketAddress->addrLength);
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


//! @brief Receive the object a través de éste canal.
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

	    if (bcmp((char *) &theSocketAddress->address.addr, (char *) &other_Addr.addr, 
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
    // if o.k. get a pointer to the data in the message and 
    // place the incoming data there
    int size;
    char *gMsg;
    gMsg = msg.data;
    size = msg.length;

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    recvfrom(sockfd, gMsg, size, 0, &other_Addr.addr, &addrLength);
	    size = 0;
	}
	else {
	  recvfrom(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &other_Addr.addr, &addrLength);
		   
	    
	  gMsg += MAX_UDP_DATAGRAM;
	  size -= MAX_UDP_DATAGRAM;
	}
    }

    // check the address that message came from was correct
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	    
	    if (bcmp((char *) &theSocketAddress->address.addr, (char *) &other_Addr.addr, 
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
	
	    bcopy((char *) &theSocketAddress->address.addr, (char *) &other_Addr.addr, 
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
	    sendto(sockfd, gMsg, size, 0, &other_Addr.addr, addrLength);
	    size = 0;
	}
	else {	
	  sendto(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &other_Addr.addr, addrLength);
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
    // if o.k. get a pointer to the data in the message and 
    // place the incoming data there
    double *matrixData= theMatrix.getDataPtr();
    char *gMsg= reinterpret_cast<char *>(matrixData);
    int size=  theMatrix.getNumBytes();

    while(size > 0)
      {
	if(size <= MAX_UDP_DATAGRAM)
          {
	    recvfrom(sockfd, gMsg, size, 0, &other_Addr.addr, &addrLength);
	    size = 0;
	  }
	else
          {
	    recvfrom(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &other_Addr.addr, &addrLength);
	    gMsg += MAX_UDP_DATAGRAM;
	    size -= MAX_UDP_DATAGRAM;
	  }
      }

    // check the address that message came from was correct
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	    
	    if (bcmp((char *) &theSocketAddress->address.addr, (char *) &other_Addr.addr, 
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
	
	    bcopy((char *) &theSocketAddress->address.addr, (char *) &other_Addr.addr, 
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
    const double *matrixData = theMatrix.getDataPtr();
    const char *gMsg = reinterpret_cast<const char *>(matrixData);
    int size =  theMatrix.getNumBytes();

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    sendto(sockfd, gMsg, size, 0, &other_Addr.addr, addrLength);
	    size = 0;
	}
	else {
	    sendto(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &other_Addr.addr, addrLength);
			
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

    // if o.k. get a pointer to the data in the message and 
    // place the incoming data there
    int size;
    double *vectorData = theVector.theData;
    char *gMsg = (char *)vectorData;;
    size =  theVector.sz * sizeof(double);

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    recvfrom(sockfd, gMsg, size, 0, &other_Addr.addr, &addrLength);
	    size = 0;
	}
	else {
	    recvfrom(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &other_Addr.addr, &addrLength);
	    
	    gMsg += MAX_UDP_DATAGRAM;
	    size -= MAX_UDP_DATAGRAM;
	}
    }

    // check the address that message came from was correct
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	    
	    if (bcmp((char *) &theSocketAddress->address.addr, (char *) &other_Addr.addr, 
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
	
	    bcopy((char *) &theSocketAddress->address.addr, (char *) &other_Addr.addr, 
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
    double *vectorData = theVector.theData;
    char *gMsg = (char *)vectorData;;
    size =  theVector.sz * sizeof(double);

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    sendto(sockfd, gMsg, size, 0, &other_Addr.addr, addrLength);
	    size = 0;
	}
	else {
	    sendto(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &other_Addr.addr, addrLength);
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

    // if o.k. get a pointer to the data in the message and 
    // place the incoming data there
    int *idData = theID.getDataPtr();
    char *gMsg = (char *)idData;;
    int size=  theID.Size() * sizeof(int);

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    recvfrom(sockfd,gMsg,size,0,&other_Addr.addr,&addrLength);
	    size = 0;
	}
	else {
	    recvfrom(sockfd,gMsg,MAX_UDP_DATAGRAM,0, &other_Addr.addr,&addrLength);
			  
	    
	    gMsg += MAX_UDP_DATAGRAM;
	    size -= MAX_UDP_DATAGRAM;
	}
    }

    // check the address that message came from was correct
    if (theAddress != 0) {
	SocketAddress *theSocketAddress = 0;

	if (theAddress->getType() == SOCKET_TYPE) {
	    theSocketAddress = (SocketAddress *)theAddress;	
	    
	    if (bcmp((char *) &theSocketAddress->address.addr, (char *) &other_Addr.addr, 
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
	
	    bcopy((char *) &theSocketAddress->address.addr, (char *) &other_Addr.addr, 
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
    const int *idData = theID.getDataPtr();
    char *gMsg = (char *)idData;;
    int size =  theID.Size() * sizeof(int);

    while (size > 0) {
	if (size <= MAX_UDP_DATAGRAM) {
	    sendto(sockfd,gMsg,size,0,&other_Addr.addr,addrLength);
	    size = 0;
	}
	else {
	    sendto(sockfd,gMsg,MAX_UDP_DATAGRAM,0,
			&other_Addr.addr, addrLength);
	    gMsg += MAX_UDP_DATAGRAM;
	    size -= MAX_UDP_DATAGRAM;
	}
    }
    return 0;
}



std::string XC::UDP_Socket::addToProgram(void)
  {
    const char *tcp = " 2 ";
    
    //char  me[20];
    char  my_InetAddr[MAX_INET_ADDR];
    char  myPortNum[8];
    unsigned int thePort = this->getPortNumber();

    int start = 0;
    int2a(thePort,myPortNum,&start);
    const std::string me= getHostName(); //gethostname(me,MAX_INET_ADDR);

    getHostAddress(me,my_InetAddr);

    std::string retval(tcp);
    retval+= " "+std::string(my_InetAddr)+" "+std::string(myPortNum)+" ";
    return retval;
  }




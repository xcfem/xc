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
// $Date: 2005/11/23 18:27:24 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/MPI_Channel.cpp,v $
                                                                        
                                                                        
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the implementation of the methods needed
// to define the MPI_Channel class interface.

#include <utility/actor/channel/MPI_Channel.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/message/Message.h>
#include <utility/actor/address/MPI_ChannelAddress.h>
#include <utility/actor/actor/MovableObject.h>
#include <cstdlib>

// MPI_Channel(unsigned int other_Port, char *other_InetAddr): 
// 	constructor to open a socket with my inet_addr and with a port number 
//	given by the OS. 

XC::MPI_Channel::MPI_Channel(int other)
 :otherTag(other), otherComm(MPI_COMM_WORLD)
  {}    

int XC::MPI_Channel::setUpConnection(void)
  { return 0; }


XC::ChannelAddress *XC::MPI_Channel::getLastSendersAddress(void) 
  {
    std::cerr << "MPI_Channel::getLastSendersAddress(void) - ";
    std::cerr << " this should not be called - need MPI-2.0 to use\n";
  
    return 0;
  }    


int XC::MPI_Channel::setNextAddress(const ChannelAddress &theAddress)
  {	
    const MPI_ChannelAddress *theMPI_ChannelAddress= nullptr;
    if(theAddress.getType() == MPI_TYPE)
      {
        theMPI_ChannelAddress= dynamic_cast<const MPI_ChannelAddress *>(&theAddress);    
        otherTag = theMPI_ChannelAddress->otherTag;
        otherComm= theMPI_ChannelAddress->otherComm;
      }
    else
      {
	std::cerr << "MPI_Channel::setNextAddress() - an MPI_Channel ";
	std::cerr << "can only communicate with an MPI_Channel";
	std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	return -1;	    
      }
    return 0;
  }



int XC::MPI_Channel::sendObj(int commitTag,MovableObject &theObject, ChannelAddress *theAddress) 
  {
    // first check address is the only address a MPI_Channel can send to
    MPI_ChannelAddress *theMPI_ChannelAddress = 0;
    if(theAddress != 0)
      {
        if(theAddress->getType() == MPI_TYPE)
          {
	    theMPI_ChannelAddress= dynamic_cast<MPI_ChannelAddress *>(theAddress);
            otherTag = theMPI_ChannelAddress->otherTag;
	    otherComm= theMPI_ChannelAddress->otherComm;
          }
	else
          {
	    std::cerr << "MPI_Channel::sendObj() - a MPI_Channel ";
	    std::cerr << "can only communicate with a MPI_Channel";
	    std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	    return -1;	    
	  }
      }    
    return sendMovable(commitTag,theObject);
  }

int XC::MPI_Channel::recvObj(int commitTag,MovableObject &theObject, FEM_ObjectBroker &theBroker, ChannelAddress *theAddress)
  {
    // first check address is the only address a MPI_Channel can send to
    MPI_ChannelAddress *theMPI_ChannelAddress = 0;
    if(theAddress != 0)
      {
        if(theAddress->getType() == MPI_TYPE)
          {
            theMPI_ChannelAddress = dynamic_cast<MPI_ChannelAddress *>(theAddress);
	    otherTag = theMPI_ChannelAddress->otherTag;
	    otherComm= theMPI_ChannelAddress->otherComm;
          }
        else
          {
            std::cerr << "MPI_Channel::recvObj() - a MPI_Channel ";
            std::cerr << "can only communicate with a MPI_Channel";
            std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	    return -1;	    
          }
      }
    return receiveMovable(commitTag,theObject,theBroker);
  }


// void Recv(Message &):
// 	Method to receive a message, also sets other_Addr to that of sender

int XC::MPI_Channel::recvMsg(int dbTag, int commitTag, Message &msg, ChannelAddress *theAddress)
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MPI_Channel::recvMsg." << std::endl;
    // first check address is the only address a MPI_Channel can send to
    const MPI_ChannelAddress *theMPI_ChannelAddress= nullptr;
    if(theAddress != 0)
      {
        if(theAddress->getType() == MPI_TYPE)
          {
	    theMPI_ChannelAddress= dynamic_cast<MPI_ChannelAddress *>(theAddress);
	    otherTag = theMPI_ChannelAddress->otherTag;
	    otherComm= theMPI_ChannelAddress->otherComm;
          }
        else
          {
	    std::cerr << "MPI_Channel::recvMesg() - a MPI_Channel ";
	    std::cerr << "can only communicate with a MPI_Channel";
	    std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	    return -1;
          }		    
      }

    // if o.k. get a pointer to the data in the message and 
    // place the incoming data there
    char *gMsg;
    gMsg = msg.data;
    int nleft = msg.length;

    MPI_Status status;
    MPI_Recv((void *)gMsg, nleft, MPI_CHAR, otherTag, 0, otherComm, &status);
    int count =0;
    MPI_Get_count(&status, MPI_CHAR, &count);
    if(count != nleft)
      {
        std::cerr << "MPI_Channel::recvMesg() -";
        std::cerr << " incorrect size of XC::Message received ";
        return -1;
      }
    else
      return 0;
  }


// void Send(Message &):
// 	Method to send a message to an address given by other_Addr.

int XC::MPI_Channel::sendMsg(int dbTag, int commitTag, const XC::Message &msg, ChannelAddress *theAddress)
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MPI_Channel::sendMsg." << std::endl;
    // first check address is the only address a MPI_Channel can send to
    MPI_ChannelAddress *theMPI_ChannelAddress = 0;
    if(theAddress != 0)
      {
        if(theAddress->getType() == MPI_TYPE)
          {
	    theMPI_ChannelAddress = dynamic_cast<MPI_ChannelAddress *>(theAddress);
	    otherTag = theMPI_ChannelAddress->otherTag;
	    otherComm= theMPI_ChannelAddress->otherComm;
          }
        else
          {
            std::cerr << "MPI_Channel::sendMsg() - a MPI_Channel ";
	    std::cerr << "can only communicate with a MPI_Channel";
	    std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	    return -1;	    
          }
      }
    // if o.k. get a pointer to the data in the message and 
    // place the incoming data there
    char *gMsg= msg.data;
    int nleft = msg.length;

    MPI_Send((void *)gMsg, nleft, MPI_CHAR, otherTag, 0, otherComm);
    return 0;
  }

int XC::MPI_Channel::recvMatrix(int dbTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress)	    
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MPI_Channel::recvMatrix." << std::endl;
    // first check address is the only address a MPI_Channel can send to
    MPI_ChannelAddress *theMPI_ChannelAddress = 0;
    if(theAddress != 0)
      {
        if(theAddress->getType() == MPI_TYPE)
          {
	    theMPI_ChannelAddress = dynamic_cast<MPI_ChannelAddress *>(theAddress);
	    otherTag = theMPI_ChannelAddress->otherTag;
	    otherComm= theMPI_ChannelAddress->otherComm;
          }
        else
          {
	    std::cerr << "MPI_Channel::recvMatrix() - a MPI_Channel ";
	    std::cerr << "can only communicate with a MPI_Channel";
	    std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	    return -1;	    
          }
      }

    // if o.k. get a pointer to the data in the XC::Matrix and 
    // place the incoming data there
    const double *matrixData= theMatrix.getDataPtr();
    const char *gMsg= reinterpret_cast<const char *>(matrixData);
    int nleft =  theMatrix.getDataSize();

    MPI_Status status;
    MPI_Recv((void *)gMsg, nleft, MPI_DOUBLE, otherTag, 0, otherComm, &status);
    int count = 0;
    MPI_Get_count(&status, MPI_DOUBLE, &count);
    if(count != nleft)
      {
        std::cerr << "MPI_Channel::recvMatrix() -";
        std::cerr << " incorrect number of entries for Matrix received: " << count << "\n";
        return -1;
      }
    else
      return 0;
  }



//! @brief Method to send a XC::Matrix to an address given by other_Addr.
int XC::MPI_Channel::sendMatrix(int dbTag, int commitTag, const Matrix &theMatrix, ChannelAddress *theAddress)
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MPI_Channel::sendMatrix." << std::endl;
    // first check address is the only address a MPI_Channel can send to
    MPI_ChannelAddress *theMPI_ChannelAddress = 0;
    if(theAddress != 0)
      {
        if(theAddress->getType() == MPI_TYPE)
          {
	    theMPI_ChannelAddress = dynamic_cast<MPI_ChannelAddress *>(theAddress);
	    otherTag = theMPI_ChannelAddress->otherTag;
	    otherComm= theMPI_ChannelAddress->otherComm;
          }
        else
          {
	    std::cerr << "MPI_Channel::sendMatrix() - a MPI_Channel ";
	    std::cerr << "can only communicate with a MPI_Channel";
	    std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	    return -1;	    
          }
      }

    // if o.k. get a pointer to the data in the Matrix and 
    // place the incoming data there
    const double *matrixData= theMatrix.getDataPtr();
    const char *gMsg= reinterpret_cast<const char *>(matrixData);
    int nleft =  theMatrix.getDataSize();

    MPI_Send((void *)gMsg, nleft, MPI_DOUBLE, otherTag, 0, otherComm);
    return 0;
  }


int XC::MPI_Channel::recvVector(int dbTag, int commitTag, Vector &theVector, ChannelAddress *theAddress)
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MPI_Channel::recvVector." << std::endl;
    // first check address is the only address a MPI_Channel can send to
    MPI_ChannelAddress *theMPI_ChannelAddress = 0;
    if(theAddress != 0)
      {
        if(theAddress->getType() == MPI_TYPE)
          {
	    theMPI_ChannelAddress = dynamic_cast<MPI_ChannelAddress *>(theAddress);
	    otherTag = theMPI_ChannelAddress->otherTag;
	    otherComm= theMPI_ChannelAddress->otherComm;
          }
        else
          {
	    std::cerr << "MPI_Channel::recvVector() - a MPI_Channel ";
	    std::cerr << "can only communicate with a MPI_Channel";
	    std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	    return -1;	    
          }
      }

    // if o.k. get a pointer to the data in the Vector and 
    // place the incoming data there
    const double *vectorData = theVector.getDataPtr();
    const char *gMsg = reinterpret_cast<const char *>(vectorData);
    int nleft =  theVector.Size();

    MPI_Status status;
    MPI_Recv((void *)gMsg, nleft, MPI_DOUBLE, otherTag, 0, otherComm, &status);
    int count =0;
    MPI_Get_count(&status, MPI_DOUBLE, &count);
    if(count != nleft)
      {
        std::cerr << "MPI_Channel::recvVector() -";
        std::cerr << " incorrect number of entries for XC::Vector received: " << count << 
	  " expected: " << theVector.Size() << "\n";
        return -1;
      }
    else
      return 0;
  }


//! @brief Method to send a Vector to an address given by other_Addr.
int XC::MPI_Channel::sendVector(int dbTag, int commitTag, const Vector &theVector, ChannelAddress *theAddress)
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MPI_Channel::sendVector." << std::endl;
    // first check address is the only address a MPI_Channel can send to
    MPI_ChannelAddress *theMPI_ChannelAddress = 0;
    if(theAddress != 0)
      {
        if(theAddress->getType() == MPI_TYPE)
          {
	    theMPI_ChannelAddress = dynamic_cast<MPI_ChannelAddress *>(theAddress);
	    otherTag = theMPI_ChannelAddress->otherTag;
	    otherComm= theMPI_ChannelAddress->otherComm;
          }
        else
          {
	    std::cerr << "MPI_Channel::sendVector() - a MPI_Channel ";
	    std::cerr << "can only communicate with a MPI_Channel";
	    std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	    return -1;	    
          }
      }
    // if o.k. get a pointer to the data in the Vector and 
    // place the incoming data there
    const double *vectorData = theVector.getDataPtr();
    const char *gMsg = reinterpret_cast<const char *>(vectorData);
    int nleft =  theVector.Size();

    MPI_Send((void *)gMsg, nleft, MPI_DOUBLE, otherTag, 0, otherComm);
    return 0;
  }

int XC::MPI_Channel::recvID(int dbTag, int commitTag, ID &theID, ChannelAddress *theAddress)
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MPI_Channel::recvID." << std::endl;
    // first check address is the only address a MPI_Channel can send to
    MPI_ChannelAddress *theMPI_ChannelAddress = 0;
    if(theAddress != 0)
      {
        if(theAddress->getType() == MPI_TYPE)
          {
	    theMPI_ChannelAddress= dynamic_cast<MPI_ChannelAddress *>(theAddress);
	    otherTag = theMPI_ChannelAddress->otherTag;
	    otherComm= theMPI_ChannelAddress->otherComm;
          }
        else
          {
	    std::cerr << "MPI_Channel::recvID() - a MPI_Channel ";
	    std::cerr << "can only communicate with a MPI_Channel";
	    std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	    return -1;	    
          }
      }

    // if o.k. get a pointer to the data in the XC::ID and 
    // place the incoming data there
    const int *idData = theID.getDataPtr();
    const char *gMsg = reinterpret_cast<const char *>(idData);
    int nleft =  theID.Size();

    MPI_Status status;
    MPI_Recv((void *)gMsg, nleft, MPI_INT, otherTag, 0, otherComm, &status);
    int count =0;
    MPI_Get_count(&status, MPI_INT, &count);

    //    int rank;
    //MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //std::cerr << "MPI_Channel::recvID " << rank << " " << otherTag << " " << theID;

    if(count != nleft)
      {
        std::cerr << "MPI_Channel::recvID() -";
        std::cerr << " incorrect number of entries for XC::ID received: " << count << "\n";
        return -1;
      }
    else
      return 0;
  }


// void Send(ID &):
// 	Method to send a XC::ID to an address given by other_Addr.

int XC::MPI_Channel::sendID(int dbTag, int commitTag, const ID &theID, ChannelAddress *theAddress)
  {	
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MPI_Channel::sendID." << std::endl;
    // first check address is the only address a MPI_Channel can send to
    MPI_ChannelAddress *theMPI_ChannelAddress = 0;
    if(theAddress != 0)
      {
        if(theAddress->getType() == MPI_TYPE)
          {
	    theMPI_ChannelAddress = dynamic_cast<MPI_ChannelAddress *>(theAddress);
	    otherTag = theMPI_ChannelAddress->otherTag;
	    otherComm= theMPI_ChannelAddress->otherComm;
          }
        else
          {
            std::cerr << "MPI_Channel::sendID() - a MPI_Channel ";
            std::cerr << "can only communicate with a MPI_Channel";
            std::cerr << " address given is not of type MPI_ChannelAddress\n"; 
	    return -1;	    
          }		    
      }

    // if o.k. get a pointer to the data in the ID and 
    // place the incoming data there
    const int *idData = theID.getDataPtr();
    const char *gMsg = reinterpret_cast<const char *>(idData);
    int nleft =  theID.Size();

    MPI_Send((void *)gMsg, nleft, MPI_INT, otherTag, 0, otherComm);

    // int rank;
    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //    std::cerr << "MPI_Channel::sendID " << rank << " " << otherTag << " " << theID;

    return 0;
  }


/*
int 
XC::MPI_Channel::getPortNumber(void) const
{
    return otherTag;
}
*/

char *XC::MPI_Channel::addToProgram(void)
  {
    std::cerr << "MPI_Channel::addToProgram(void) - ";
    std::cerr << " this should not be called - need MPI-2.0\n";
    char *newStuff =(char *)malloc(10*sizeof(char));
    for(int i=0; i<10; i++) 
	newStuff[i] = ' ';
    return newStuff;
  }



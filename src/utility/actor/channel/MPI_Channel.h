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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/10/15 00:26:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/MPI_Channel.h,v $
                                                                        
                                                                        
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for MPI_Channel.

#ifndef MPI_Channel_h
#define MPI_Channel_h

#include <utility/actor/channel/Channel.h>
#include <mpi/mpi.h>

namespace XC {
//! @ingroup IPComm
//
//! @brief MPI_Channel is a sub-class of channel. It is implemented with Berkeley
//! stream sockets using the TCP protocol. Messages delivery is garaunteed. 
//! Communication is full-duplex between a pair of connected sockets.
class MPI_Channel: public Channel
  {
  private:
    int otherTag;
    MPI_Comm otherComm;    
  public:
    MPI_Channel(int otherProcess);        

    char *addToProgram(void);
    
    virtual int setUpConnection(void);

    int setNextAddress(const ChannelAddress &otherChannelAddress);
    virtual ChannelAddress *getLastSendersAddress(void);

    int sendObj(int commitTag,MovableObject &theObject, ChannelAddress *theAddress =0);
    
    int recvObj(int commitTag,MovableObject &theObject, FEM_ObjectBroker &theBroker,ChannelAddress *theAddress =0);

    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress =0);       
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress =0);    

    int sendMatrix(int dbTag, int commitTag, const Matrix &, ChannelAddress *theAddress =0);
    int recvMatrix(int dbTag, int commitTag, Matrix &, ChannelAddress *theAddress =0);
    
    int sendVector(int dbTag, int commitTag, const Vector &, ChannelAddress *theAddress =0);
    int recvVector(int dbTag, int commitTag, Vector &, ChannelAddress *theAddress =0);
    
    int sendID(int dbTag, int commitTag, const ID &, ChannelAddress *theAddress =0);
    int recvID(int dbTag, int commitTag, ID &, ChannelAddress *theAddress =0);    
  };
} // end of XC namespace


#endif 






 


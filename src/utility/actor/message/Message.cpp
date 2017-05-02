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
                                                                        
// $Revision: 1.6 $
// $Date: 2004/11/25 00:54:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/message/Message.cpp,v $
                                                                        
                                                                        
// File: ~/actor/Message.C
//

#include "utility/actor/message/Message.h"
#include <cstring>
#include <iostream>

//! @brief Construct an empty message.
XC::Message::Message(void)
  :length(0),data(nullptr) {}

//! @brief Construct a message for sending/receiving an array containing \p
//! size doubles.
XC::Message::Message(double *Ptr,int size)
  {
    length = size*sizeof(double);
    data = reinterpret_cast<char *>(Ptr);
  }

//! @brief Construct a message for sending/receiving an array containing \p
//! size ints.
XC::Message::Message(int *Ptr, int size)
  {
    length = size*sizeof(int);
    data = reinterpret_cast<char *>(Ptr);
  }

//! To construct a message for sending/receiving a string of \p size
//! characters or a struct.
XC::Message::Message(char *Ptr, int size)
  {
    length = size*sizeof(char);
    data = Ptr;
  }

//! Will put the data given by the character pointer \p
//! theData of size endLoc -startLoc into the data array pointed to
//! by the Message starting at location $startLoc$ in this array. Returns 0 if
//! successful; an error message is printed and a -1 is returned if
//! not. The routine bcopy() is used to copy the data.
int XC::Message::putData(char *theData, int startLoc, int endLoc)
  {
    if(startLoc > 0 && startLoc <= length && endLoc <= length && endLoc > startLoc)
      {
	int theLength = endLoc - startLoc;
	char *dataPos = &data[startLoc];
	bcopy(theData, dataPos, theLength);
	return 0;
      }
    else
      {
	std::cerr << "Message::putData() - invalid length of data given\n";
	return -1;
      }
  }

//! @brief Returns a const char * pointer to the messages data.
const char *XC::Message::getData(void)
  { return data; }

//! @brief Get the size of the array. The unit of size is that of a
//! character. 
int XC::Message::getSize(void)
  { return length; }



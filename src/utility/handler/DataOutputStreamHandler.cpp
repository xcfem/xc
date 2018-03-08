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
// $Date: 2004/11/24 22:40:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/DataOutputStreamHandler.cpp,v $
                                                                        
// Written: fmk 
// Date: 10/04
//
// Description: This file contains the class implementation for
// DataOutputStreamHandler. 
//
// What: "@(#) DataOutputStreamHandler.C, revA"

#include "DataOutputStreamHandler.h"
#include <utility/matrix/Vector.h>

XC::DataOutputStreamHandler::DataOutputStreamHandler(bool echoD)
  :DataOutputHandler(DATAHANDLER_TAGS_DataOutputStreamHandler), 
   echoDescription(echoD), numColumns(0) {}

int XC::DataOutputStreamHandler::open(const std::vector<std::string> &dataDescription, int numData)
  {
    if(numData < 0)
     {
       std::cerr << "XC::DataOutputStreamHandler::open() - numColumns (" << numData << ") < 0\n";
       return -1;
     }
    else
      numColumns = numData;

    if(echoDescription == true)
      {
        for(int i=0; i<numData; i++)
          std::cerr << dataDescription[i] << " ";
        std::cerr << std::endl;
      }
   return 0;
  }

int XC::DataOutputStreamHandler::open(const std::vector<std::string> &dataDescription)
  {
    int numData= dataDescription.size();
    if(numData < 0)
      {
        std::cerr << "XC::DataOutputStreamHandler::open() - numColumns (" << numData << ") < 0\n";
        return -1;
      }
    else
      numColumns = numData;
    if(echoDescription == true)
      {
        for(int i=0; i<numData; i++)
          std::cerr << dataDescription[i] << " ";
        std::cerr << std::endl;
      }
    return 0;
  }

int XC::DataOutputStreamHandler::write(Vector &data) 
  {
    if (data.Size() == numColumns)
      outputStream.stream() << data;
  else {
    std::cerr << "XC::DataOutputStreamHandler::write() - Vector not of correct size\n";
    return -1;
  }
  
  return 0;
}

//! @brief Send the object a través del canal definido en el argumento.
int XC::DataOutputStreamHandler::sendSelf(CommParameters &cp)
  { return 0; }

//! @brief Receive the object a través del canal definido en el argumento.
int XC::DataOutputStreamHandler::recvSelf(const CommParameters &cp)
  { return 0; }

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
///////////////////////////////////////////////////////////////////////////////
//
// COPYRIGHT (C):     :-))
// PROJECT:           Object Oriented Finite XC::Element Program
// FILE:              BrickSelfWeight
// CLASS:             
// MEMBER FUNCTIONS:
//
// MEMBER VARIABLES
//
// PURPOSE:           Finite XC::Element Class
// RETURN:
// VERSION:
// LANGUAGE:          C++
// TARGET OS:         DOS || UNIX || . . .
// DESIGNER:          Zhaohui Yang
// PROGRAMMER:        Zhaohui Yang 
// DATE:              March 2002
// UPDATE HISTORY:
//
//
//
///////////////////////////////////////////////////////////////////////////////
//

#ifndef BRICKSELFWEIGHT_CPP
#define BRICKSELFWEIGHT_CPP

                                                                        
// Written: ZHYang UC Davis
// Purpose: This file contains the class definition for 8 node brick self weigth load.

#include <domain/load/volumen/BrickSelfWeight.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

XC::Vector XC::BrickSelfWeight::data(1);

XC::BrickSelfWeight::BrickSelfWeight(int tag, const XC::ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_BrickSelfWeight, theElementTags) {}

XC::BrickSelfWeight::BrickSelfWeight(int tag)
  :ElementBodyLoad(tag, LOAD_TAG_BrickSelfWeight) {}

XC::BrickSelfWeight::BrickSelfWeight(void)
  :ElementBodyLoad(LOAD_TAG_BrickSelfWeight) {}

int XC::BrickSelfWeight::getType(void)
  { return LOAD_TAG_BrickSelfWeight; }

//! @brief Send members del objeto a través del
//! canal being passed as parameter.
int XC::BrickSelfWeight::sendData(CommParameters &cp)
  {
    int res= ElementBodyLoad::sendData(cp);
    return res;
  }

//! @brief Receives members del objeto a través del
//! canal being passed as parameter.
int XC::BrickSelfWeight::recvData(const CommParameters &cp)
  {        
    int res= ElementBodyLoad::recvData(cp);
    return res;
  }

int XC::BrickSelfWeight::sendSelf(CommParameters &cp)
  {
    static ID data(5);
    int result= sendData(cp);
    
    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "BrickSelfWeight::sendSelf - failed to send data\n";
    return result;
  }

int XC::BrickSelfWeight::recvSelf(const CommParameters &cp)
  {
    static ID data(5);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BrickSelfWeight::recvSelf() - failed to recv data\n";
    else
      res+= recvData(cp);
    return res;
  }

void XC::BrickSelfWeight::Print(std::ostream &s, int flag)
  {
    s << "BrickSelfWeight...";
    s << "  elements acted on: " << this->getElementTags();
  }

#endif


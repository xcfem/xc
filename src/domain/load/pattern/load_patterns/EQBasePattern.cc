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
//EQBasePattern.cpp

#include <domain/load/pattern/load_patterns/EQBasePattern.h>
#include <domain/load/groundMotion/GroundMotion.h>


XC::EQBasePattern::EQBasePattern(int tag, int _classTag)
  :LoadPattern(tag, _classTag), theMotions() {}


int XC::EQBasePattern::addMotion(GroundMotion &theMotion)
  {
    theMotions.addMotion(theMotion); 
    return 0;
  }

void XC::EQBasePattern::Print(std::ostream &s, int flag)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
  }

//! @brief Send members through the channel being passed as parameter.
int XC::EQBasePattern::sendData(CommParameters &cp)
  {
    int res= EQBasePattern::sendData(cp);
    res+= cp.sendMovable(theMotions,getDbTagData(),CommMetaData(16));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::EQBasePattern::recvData(const CommParameters &cp)
  {
    int res= EQBasePattern::recvData(cp);
    res+= cp.receiveMovable(theMotions,getDbTagData(),CommMetaData(16));
    return res;
  }

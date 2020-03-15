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
                                                                        
// Written: Chris McGann, U.Washington
//          02.2011
//
// Description: This file contains the implementation for the SelfWeight class.

#include <domain/load/volumetric/SelfWeight.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

XC::Vector XC::SelfWeight::data(1);

XC::SelfWeight::SelfWeight(int tag, double xf, double yf, double zf, const XC::ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_SelfWeight, theElementTags),
  xFact(xf), yFact(yf), zFact(zf) {}

XC::SelfWeight::SelfWeight(int tag)
  :ElementBodyLoad(tag, LOAD_TAG_SelfWeight),
  xFact(0.0), yFact(0.0), zFact(0.0) {}

XC::SelfWeight::SelfWeight(void)
  :ElementBodyLoad(LOAD_TAG_SelfWeight),
  xFact(0.0), yFact(0.0), zFact(0.0) {}

const XC::Vector &XC::SelfWeight::getData(int &type, const double &loadFactor) const
  {
    type= getClassTag();
    data(0) = xFact;
    data(1) = yFact;
    data(2) = zFact;
    return data;
  }

//! @brief Send members of the object through the
//! communicator being passed as parameter.
int XC::SelfWeight::sendData(CommParameters &cp)
  {
    int res= ElementBodyLoad::sendData(cp);
    res+= cp.sendDoubles(xFact, yFact, zFact,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members of the object through the
//! communicator being passed as parameter.
int XC::SelfWeight::recvData(const CommParameters &cp)
  {        
    int res= ElementBodyLoad::recvData(cp);
    res+= cp.receiveDoubles(xFact, yFact, zFact,getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::SelfWeight::sendSelf(CommParameters &cp)
  {
    static ID data(4);
    int result= sendData(cp);
    
    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "SelfWeight::sendSelf - failed to send data\n";
    return result;
  }

int XC::SelfWeight::recvSelf(const CommParameters &cp)
  {
    static ID data(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "SelfWeight::recvSelf() - failed to recv data\n";
    else
      res+= recvData(cp);
    return res;
  }

void XC::SelfWeight::Print(std::ostream &s, int flag) const
  {
    s << "SelfWeight...";
    s << "  elements acted on: " << this->getElementTags();
  }




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
//CorotTrussBase.cc

#include "CorotTrussBase.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "utility/actor/actor/MovableVector.h"

// constructor:
//  responsible for allocating the necessary space needed by each object
//  and storing the tags of the XC::CorotTrussBase end nodes.
XC::CorotTrussBase::CorotTrussBase(int tag, int classTag, int dim,int Nd1, int Nd2)
  :ProtoTruss(tag,classTag,Nd1,Nd2,0,dim), Lo(0.0), Ln(0.0), R(3,3)
  {}

//! @brief Constructor.
XC::CorotTrussBase::CorotTrussBase(int tag, int classTag,int dimension)
  :ProtoTruss(tag,classTag,0,0,0,dimension), Lo(0.0), Ln(0.0), R(3,3)
  {}

//! @brief Copy constructor.
XC::CorotTrussBase::CorotTrussBase(const CorotTrussBase &other)
  : ProtoTruss(other), Lo(other.Lo), Ln(other.Ln), R(other.R)
  {}

//! @brief Assignment operator.
XC::CorotTrussBase &XC::CorotTrussBase::operator=(const CorotTrussBase &other)
  {
    ProtoTruss::operator=(other);
    Lo= other.Lo;
    Ln= other.Ln;
    R= other.R;
    return *this;
  }

//! @brief Return the initial length of the element.
const double &XC::CorotTrussBase::getLo(void) const
  { return Lo; }

//! @brief Return the current length of the element.
const double &XC::CorotTrussBase::getLn(void) const
  { return Ln; }

//! @brief Send members through the channel being passed as parameter.
int XC::CorotTrussBase::sendData(CommParameters &cp)
  {
    int res= ProtoTruss::sendData(cp);
    res+= cp.sendDoubles(Lo,Ln,d21[0],d21[1],d21[2],getDbTagData(),CommMetaData(16));
    res+= cp.sendMatrix(R,getDbTagData(),CommMetaData(17)); 
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::CorotTrussBase::recvData(const CommParameters &cp)
  {
    int res= ProtoTruss::recvData(cp);
    res+= cp.receiveDoubles(Lo,Ln,d21[0],d21[1],d21[2],getDbTagData(),CommMetaData(16));
    res+= cp.receiveMatrix(R,getDbTagData(),CommMetaData(17)); 
    return res;
  }

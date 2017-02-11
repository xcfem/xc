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
//beam2d.cc

#include "beam2d.h"
#include "utility/actor/actor/MovableVector.h"

//! @brief Default constructor.
XC::beam2d::beam2d(int tag,int class_tag)
  :ProtoBeam2d(tag,class_tag), L(0)
  {}

//! @brief Constructor.
XC::beam2d::beam2d(int tag, int class_tag, double a, double e, double i, int Nd1, int Nd2)
  :ProtoBeam2d(tag,class_tag,a,e,i,Nd1,Nd2), L(0)   
  {}

//! @brief Send members through the channel being passed as parameter.
int XC::beam2d::sendData(CommParameters &cp)
  {
    int res= ProtoBeam2d::sendData(cp);
    res+= cp.sendDoubles(L,sn,cs,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::beam2d::recvData(const CommParameters &cp)
  {
    int res= ProtoBeam2d::recvData(cp);
    res+= cp.receiveDoubles(L,sn,cs,getDbTagData(),CommMetaData(9));
    return res;
  }

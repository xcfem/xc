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
//SteelBase.cc


#include <material/uniaxial/steel/SteelBase.h>

#include "utility/actor/actor/MovableVector.h"

//! @brief Constructor.
XC::SteelBase::SteelBase(int tag,int classTag,const double &Fy,const double &e0,const double &B,const double &A1,const double &A2,const double &A3,const double &A4)
  : UniaxialMaterial(tag,classTag), fy(Fy),E0(e0),b(B),a1(A1),a2(A2),a3(A3),a4(A4) {}

XC::SteelBase::SteelBase(int tag,int classTag)
  :UniaxialMaterial(tag,classTag), fy(0.0),E0(0.0),b(0.0),a1(0.0), a2(0.0), a3(0.0), a4(0.0) {}

//! @brief Assigns intial Young's modulus.
void XC::SteelBase::setInitialTangent(const double &d)
  {
    E0= d;
    setup_parameters(); //Initialize history variables.
  }

//! @brief Returns intial Young's modulus.
double XC::SteelBase::getInitialTangent(void) const
  { return E0; }

//! @brief Assigns yield stress.
void XC::SteelBase::setFy(const double &d)
  {
    fy= d;
    setup_parameters(); //Initialize history variables.
  }

//! @brief Returns yield stress.
double XC::SteelBase::getFy(void) const
  { return fy; }


//! @brief Send object members through the channel being passed as parameter.
int XC::SteelBase::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendDoubles(fy,E0,b,getDbTagData(),CommMetaData(2));
    res+= cp.sendDoubles(a1,a2,a3,a4,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::SteelBase::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveDoubles(fy,E0,b,getDbTagData(),CommMetaData(2));
    res+= cp.receiveDoubles(a1,a2,a3,a4,getDbTagData(),CommMetaData(3));
    return res;
  }

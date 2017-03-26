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
//PYBase.cc

#include "material/uniaxial/PY/PYBase.h"


/////////////////////////////////////////////////////////////////////
//! @brief Constructor with data
XC::PYBase::PYBase(int tag, int classtag, int soil, double v_ult, double v_50, double dash_pot)
  :UniaxialMaterial(tag,classtag), initialTangent(0.0), soilType(soil), matCapacity(v_ult), v50(v_50), dashpot(dash_pot)
  {}

//! @brief Default constructor.
XC::PYBase::PYBase(int tag,int classtag)
:UniaxialMaterial(tag,classtag), initialTangent(0.0), soilType(0), matCapacity(0.0), v50(0.0), dashpot(0.0)
  {}

/////////////////////////////////////////////////////////////////////
//! @brief Default constructor.
XC::PYBase::PYBase(void)
  :UniaxialMaterial(0,0), initialTangent(0.0),soilType(0), matCapacity(0.0), v50(0.0), dashpot(0.0)
  {}

void XC::PYBase::getFarField(const double &v)
  { TFar.getField(v); }

double XC::PYBase::getTangent(void) const
  { return T.tang(); }

double XC::PYBase::getInitialTangent(void) const
  { return initialTangent; }

double XC::PYBase::getStrain(void) const
  { return T.z(); }

double XC::PYBase::getStrainRate(void) const
  { return TvRate; }

/////////////////////////////////////////////////////////////////////
int XC::PYBase::commitState(void)
  {
    C= T; // Commit trial history variable -- Combined element
    CFar= TFar; // Commit trial history variables for the Far Field
    return 0;
  }

/////////////////////////////////////////////////////////////////////
int XC::PYBase::revertToLastCommit(void)
  {
    // Reset to committed values
    T= C;
    TFar= CFar;
    return 0;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::PYBase::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendMovable(C,getDbTagData(),CommMetaData(2));
    res+= cp.sendMovable(T,getDbTagData(),CommMetaData(3));
    res+= cp.sendDouble(TvRate,getDbTagData(),CommMetaData(4));
    res+= cp.sendMovable(CFar,getDbTagData(),CommMetaData(5));
    res+= cp.sendMovable(TFar,getDbTagData(),CommMetaData(6));
    const double st= soilType;
    res+= cp.sendDoubles(initialTangent,st,matCapacity,v50,getDbTagData(),CommMetaData(7));
    res+= cp.sendDoubles(vRef,np,dashpot,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::PYBase::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveMovable(C,getDbTagData(),CommMetaData(2));
    res+= cp.receiveMovable(T,getDbTagData(),CommMetaData(3));
    res+= cp.receiveDouble(TvRate,getDbTagData(),CommMetaData(4));
    res+= cp.receiveMovable(CFar,getDbTagData(),CommMetaData(5));
    res+= cp.receiveMovable(TFar,getDbTagData(),CommMetaData(6));
    double st;
    res+= cp.receiveDoubles(initialTangent,st,matCapacity,v50,getDbTagData(),CommMetaData(7));
    soilType= st;
    res+= cp.receiveDoubles(vRef,np,dashpot,getDbTagData(),CommMetaData(8));
    return res;
  }

/////////////////////////////////////////////////////////////////////
void XC::PYBase::Print(std::ostream &s, int flag)
  {
    s << "PYBase, tag: " << this->getTag() << std::endl;
    s << "  soilType: " << soilType << std::endl;
    s << "  matCapacity: " << matCapacity << std::endl;
    s << "  v50: " << v50 << std::endl;
    s << "  dashpot: " << dashpot << std::endl;
  }

/////////////////////////////////////////////////////////////////////

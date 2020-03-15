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
  :UniaxialMaterial(tag,classtag), initialTangent(0.0), soilType(soil),
   matCapacity(v_ult), v50(v_50), dashpot(dash_pot)
  {}

//! @brief Default constructor.
XC::PYBase::PYBase(int tag,int classtag)
:UniaxialMaterial(tag,classtag), initialTangent(0.0), soilType(0),
 matCapacity(0.0), v50(0.0), dashpot(0.0)
  {}

/////////////////////////////////////////////////////////////////////
//! @brief Default constructor.
XC::PYBase::PYBase(void)
  :UniaxialMaterial(0,0), initialTangent(0.0),soilType(0), matCapacity(0.0), v50(0.0), dashpot(0.0)
  {}

//! @brief Sets the soil type to represent:
//! soilType= 1 Backbone of p-y curve approximates Matlock (1970) soft clay relation.
//! soilType= 2 Backbone of p-y curve approximates API (1993) sand relation. 
void XC::PYBase::setSoilType(const int &st)
  { soilType= st; }

//! @brief Gets the soil type to represent:
//! soilType= 1 Backbone of p-y curve approximates Matlock (1970) soft clay relation.
//! soilType= 2 Backbone of p-y curve approximates API (1993) sand relation. 
int XC::PYBase::getSoilType(void) const
  { return soilType; } 

//! @brief Set the ultimate capacity of the p-y material. Note that "p" or
//! "pult" are distributed loads [force per length of pile] in common design
//! equations, but are both loads for this uniaxialMaterial [i.e.,
//! distributed load times the tributary length of the pile]. 
void XC::PYBase::setUltimateCapacity(const double &pult)
  { matCapacity= pult; }
//! @brief Get the ultimate capacity of the p-y material. Note that "p" or
//! "pult" are distributed loads [force per length of pile] in common design
//! equations, but are both loads for this uniaxialMaterial [i.e.,
//! distributed load times the tributary length of the pile].
double XC::PYBase::getUltimateCapacity(void) const
  { return matCapacity; }
//! @brief Set the displacement at which 50% of pult is mobilized in
//! monotonic loading. 
void XC::PYBase::setY50(const double &Y50)
  { v50= Y50;}
//! @brief Get the displacement at which 50% of pult is mobilized in
//! monotonic loading. 
double XC::PYBase::getY50(void) const
  { return v50; }
//! @brief Set the viscous damping term (dashpot) on the far-field (elastic)
//! component of the displacement rate (velocity). (optional Default = 0.0).
//! Nonzero dp values are used to represent radiation damping effects. 
void XC::PYBase::setDashPot(const double &dp)
  { dashpot= dp; }
//! @brief Get the viscous damping term (dashpot) on the far-field (elastic)
//! component of the displacement rate (velocity). (optional Default = 0.0).
//! Nonzero dp values are used to represent radiation damping effects.
double XC::PYBase::getDashPot(void) const
  { return dashpot; }

void XC::PYBase::getFarField(const double &v)
  { TFar.getField(v); }

double XC::PYBase::getTangent(void) const
  { return T.tang(); }

//! @brief Get initial stiffness.
double XC::PYBase::getInitialTangent(void) const
  { return initialTangent; }

//! @brief Set initial stiffness.
void XC::PYBase::setInitialTangent(const double &t)
  { initialTangent= t; }

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
void XC::PYBase::Print(std::ostream &s, int flag) const
  {
    s << "PYBase, tag: " << this->getTag() << std::endl;
    s << "  soilType: " << soilType << std::endl;
    s << "  matCapacity: " << matCapacity << std::endl;
    s << "  v50: " << v50 << std::endl;
    s << "  dashpot: " << dashpot << std::endl;
  }

/////////////////////////////////////////////////////////////////////

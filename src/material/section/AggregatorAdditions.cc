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
//AggregatorAdditions.cc

#include "AggregatorAdditions.h"


#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
XC::AggregatorAdditions::AggregatorAdditions(CommandEntity *owner)
  : DqUniaxialMaterial(owner), matCodes(0) {}

//! @brief Constructor.
XC::AggregatorAdditions::AggregatorAdditions(CommandEntity *owner,const UniaxialMaterial &um, int c)
  : DqUniaxialMaterial(owner,um), matCodes(1)
  {
    matCodes(0)= c;
  }

//! @brief Copy constructor.
XC::AggregatorAdditions::AggregatorAdditions(const AggregatorAdditions &other)
  : DqUniaxialMaterial(other), matCodes(other.matCodes)
  {}

//! @brief Copy constructor.
XC::AggregatorAdditions::AggregatorAdditions(const AggregatorAdditions &other,SectionForceDeformation *s)
  : DqUniaxialMaterial(other,s), matCodes(other.matCodes)
  {}

//! @brief Destructor.
XC::AggregatorAdditions::~AggregatorAdditions(void)
  {}


void XC::AggregatorAdditions::putMatCodes(const ResponseId &codes)
  { matCodes= codes; }

//! @brief Response identifiers for material stiffness contributions.
void XC::AggregatorAdditions::getType(ResponseId &retval,const size_t &offset) const
  {
    const size_t n= size();
    for(register size_t i=0;i<n;i++)
      retval(offset+i)= matCodes(i);
  }

//! @brief Print stuff.
void XC::AggregatorAdditions::Print(std::ostream &s, int flag) const
  {
    DqUniaxialMaterial::Print(s,flag);
    s << "\tUniaxial codes " << matCodes << std::endl;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::AggregatorAdditions::sendData(CommParameters &cp)
  {
    int res= DqUniaxialMaterial::sendData(cp);
    res+= cp.sendID(matCodes,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::AggregatorAdditions::recvData(const CommParameters &cp)
  {
    int res= DqUniaxialMaterial::recvData(cp);
    res+= cp.receiveID(matCodes,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::AggregatorAdditions::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::AggregatorAdditions::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

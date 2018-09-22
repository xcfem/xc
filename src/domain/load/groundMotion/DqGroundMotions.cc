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
//DqGroundMotions.cc

#include "DqGroundMotions.h"
#include "GroundMotion.h"
#include "boost/any.hpp"



//! @brief Constructor.
XC::DqGroundMotions::DqGroundMotions(const size_t &num)
  : CommandEntity(), MovableObject(0), gMotions(num,static_cast<GroundMotion *>(nullptr)) {}

//! @brief Copy constructor.
XC::DqGroundMotions::DqGroundMotions(const DqGroundMotions &other)
  : CommandEntity(other), MovableObject(0), gMotions(other.gMotions) {}

//! @brief Assignment operator.
XC::DqGroundMotions &XC::DqGroundMotions::operator=(const DqGroundMotions &other)
  {
    CommandEntity::operator=(other);
    gMotions= other.gMotions;
    return *this;
  }

void XC::DqGroundMotions::addMotion(GroundMotion &gm)
  {
    const_iterator i= std::find(begin(),end(),&gm);
    if(i == end())
      gMotions.push_back(&gm);
  }

void XC::DqGroundMotions::clear(void)
  {
    iterator i= gMotions.begin();
    for(;i!= gMotions.end();i++)
      if(*i)
        delete (*i);
    gMotions.erase(gMotions.begin(),gMotions.end());
  }

//! @brief Destructor:
XC::DqGroundMotions::~DqGroundMotions(void)
  { clear(); }

//! @brief Sends object through the channel being passed as parameter.
int XC::DqGroundMotions::sendData(CommParameters &cp)
  {
    const size_t sz= getNumGroundMotions();
    setDbTagDataPos(0,sz);
    DbTagData cpGM(sz*2);
    int res= 0;
    for(size_t i= 0;i<sz;i++)
      res+= cp.sendBrokedPtr((*this)[i],cpGM,BrokedPtrCommMetaData(i,0,i+sz));
    res+= cpGM.send(getDbTagData(),cp,CommMetaData(1));
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DqGroundMotions::recvData(const CommParameters &cp)
  {
    const size_t sz= getDbTagDataPos(0);
    DbTagData cpGM(sz*2);
    int res= cpGM.receive(getDbTagData(),cp,CommMetaData(1));

    for(size_t i= 0;i<sz;i++)
      {
        // Receive the ground motion
        (*this)[i]= cp.getBrokedGroundMotion((*this)[i],cpGM,BrokedPtrCommMetaData(i,0,i+sz));
        if(!(*this)[i])
          std::cerr << "DqGroundMotions::recvSelf() - ground motion "
                    << i << "failed to recv itself\n";
      }
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::DqGroundMotions::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING DqGroundMotions::sendSelf() - " 
                << dataTag << " failed to send.";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DqGroundMotions::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "WARNING DqGroundMotions::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(cp);
    return res;
  }

void XC::DqGroundMotions::Print(std::ostream &s,const int &flag)
  {
    s << "\tNumber of GroundMotions: " << getNumGroundMotions() << std::endl;
  }

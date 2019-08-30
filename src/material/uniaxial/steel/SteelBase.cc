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
#include "domain/component/Parameter.h"
#include <domain/mesh/element/utils/Information.h>

//! @brief Constructor.
XC::SteelBase::SteelBase(int tag,int classTag,const double &Fy,const double &e0,const double &B,const double &A1,const double &A2,const double &A3,const double &A4)
  : UniaxialMaterial(tag,classTag), fy(Fy),E0(e0),b(B),a1(A1),a2(A2),a3(A3),a4(A4) {}

XC::SteelBase::SteelBase(int tag,int classTag)
  :UniaxialMaterial(tag,classTag), fy(0.0),E0(0.0),b(0.0),a1(0.0), a2(0.0), a3(0.0), a4(0.0) {}

//! @brief Assigns initial Young's modulus.
void XC::SteelBase::setInitialTangent(const double &d)
  {
    E0= d;
    setup_parameters(); //Initialize history variables.
  }

//! @brief Returns initial Young's modulus.
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

// AddingSensitivity:BEGIN ///////////////////////////////////
int XC::SteelBase::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const size_t argc= argv.size();
    if(argc < 1) return -1;
    if((argv[0]=="sigmaY") || (argv[0]=="fy") || (argv[0]=="Fy"))
      {
        param.setValue(fy);
        return param.addObject(1, this);
      }
    if(argv[0]=="E")
      {
        param.setValue(E0);
        return param.addObject(2, this);
      }
    if(argv[0]=="b")
      {
        param.setValue(b);
        return param.addObject(3, this);
      }
    if(argv[0]=="a1")
      {
        param.setValue(a1);
        return param.addObject(4, this);
      }
    if(argv[0]=="a2")
      {
        param.setValue(a2);
        return param.addObject(5, this);
      }
    if(argv[0]=="a3")
      {
        param.setValue(a3);
        return param.addObject(6, this);
      }
    if(argv[0]=="a4")
      {
        param.setValue(a4);
        return param.addObject(7, this);
      }
    return -1;
  }

int XC::SteelBase::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
      case -1:
        return -1;
      case 1:
        this->fy= info.theDouble;
        break;
      case 2:
        this->E0 = info.theDouble;
        break;
      case 3:
        this->b = info.theDouble;
        break;
      case 4:
        this->a1 = info.theDouble;
        break;
      case 5:
        this->a2 = info.theDouble;
        break;
      case 6:
        this->a3 = info.theDouble;
        break;
      case 7:
        this->a4 = info.theDouble;
        break;
      default:
        return -1;
      }
    return 0;
  }

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
//BeamStrainLoad.cpp

#include <domain/load/beam_loads/BeamStrainLoad.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "material/section/ResponseId.h"

XC::BeamStrainLoad::BeamStrainLoad(int tag, const XC::ID &theElementTags)
  :BeamLoad(tag, LOAD_TAG_BeamStrainLoad, theElementTags) {}

XC::BeamStrainLoad::BeamStrainLoad(int tag)
  :BeamLoad(tag, LOAD_TAG_BeamStrainLoad) {}

//! @brief Send data through the channel being passed as parameter.
int XC::BeamStrainLoad::sendData(CommParameters &cp)
  {
    int res= BeamLoad::sendData(cp);
    res+= cp.sendMovable(backEndDeformationsPlane,getDbTagData(),CommMetaData(5));
    res+= cp.sendMovable(frontEndDeformationPlane,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::BeamStrainLoad::recvData(const CommParameters &cp)
  {
    int res= BeamLoad::recvData(cp);
    res+= cp.receiveMovable(backEndDeformationsPlane,getDbTagData(),CommMetaData(5));;
    res+= cp.receiveMovable(frontEndDeformationPlane,getDbTagData(),CommMetaData(6));;
    return res;
  }

int XC::BeamStrainLoad::sendSelf(CommParameters &cp)
  {
    static ID data(3);
    int res= sendData(cp);
    const int dataTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BeamStrainLoad::sendSelf() - failed to send extra data\n";    
    return res;
  }

int XC::BeamStrainLoad::recvSelf(const CommParameters &cp)
  {
    static ID data(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BeamStrainLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }

const XC::Vector &XC::BeamStrainLoad::getSection1Deformation(const size_t &order,const ResponseId &code) const
  { return backEndDeformationsPlane.getDeformation(order,code); }

const XC::Vector &XC::BeamStrainLoad::getSection2Deformation(const size_t &order,const ResponseId &code) const
  { return frontEndDeformationPlane.getDeformation(order,code); }

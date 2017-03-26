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
//NDMaterialPhysicalProperties.cc

#include "SolidMech2D.h"

//! @brief Constructor.
XC::SolidMech2D::SolidMech2D(const size_t &nMat,const NDMaterial *ptr_mat, const double &t, const double &r)
  :NDMaterialPhysicalProperties(nMat,ptr_mat), thickness(t), rho(r) {}

//! @brief Constructor.
XC::SolidMech2D::SolidMech2D(const size_t &nMat, NDMaterial &theMat,const std::string &type, const double &t, const double &r)
  : NDMaterialPhysicalProperties(nMat,nullptr), thickness(t), rho(r)
  {
    if(check_material_type(type))
      theMaterial.setMaterial(&theMat,type);
  }

//! @brief Returns densities for each position.
XC::Vector XC::SolidMech2D::getRhoi(void) const
  { return NDMaterialPhysicalProperties::getRhoi(rho); }

#define DBTAGS_SIZE 3
//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::SolidMech2D::getDbTagData(void) const
  {
    static DbTagData retval(DBTAGS_SIZE); 
    return retval;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::SolidMech2D::sendData(CommParameters &cp)
  {
    int res= NDMaterialPhysicalProperties::sendData(cp);
    res+= cp.sendDoubles(thickness,rho,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::SolidMech2D::recvData(const CommParameters &cp)
  {
    int res= NDMaterialPhysicalProperties::recvData(cp);
    res+= cp.receiveDoubles(thickness,rho,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object.
int XC::SolidMech2D::sendSelf(CommParameters &cp)
  {
    inicComm(DBTAGS_SIZE);

    int res= this->sendData(cp);

    const int dataTag= getDbTag();
    res += cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "SolidMech2D::sendSelf -- failed to send ID data\n";
    return res;
  }

//! @brief Receives object
int XC::SolidMech2D::recvSelf(const CommParameters &cp)
  {
    inicComm(DBTAGS_SIZE);

    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "SolidMech2D::recvSelf -- failed to receive ID data\n";
    else
      res+= this->recvData(cp);
    return res;
  }

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
//EPPBaseMaterial.cc

#include <material/uniaxial/EPPBaseMaterial.h>


//! @brief Constructor.
XC::EPPBaseMaterial::EPPBaseMaterial(int tag, int classtag, double e,double e0)
  :ElasticBaseMaterial(tag,classtag,e,e0), trialStress(0.0),
   trialTangent(e), commitStrain(0.0) {}

//! @brief Send object members through the communicator argument.
int XC::EPPBaseMaterial::sendData(Communicator &comm)
  {
    int res= ElasticBaseMaterial::sendData(comm);
    res+= comm.sendDoubles(trialStress,trialTangent,commitStrain,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::EPPBaseMaterial::recvData(const Communicator &comm)
  {
    int res= ElasticBaseMaterial::recvData(comm);
    res+= comm.receiveDoubles(trialStress,trialTangent,commitStrain,getDbTagData(),CommMetaData(3));
    return res;
  }

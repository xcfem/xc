//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//! @param[in] tag material identifier.
//! @param[in] classTag material class identifier.
//! @param[in] e material elastic modulus.
//! @param[in] e0 initial strain.
XC::EPPBaseMaterial::EPPBaseMaterial(int tag, int classtag, double e,double e0)
  :ElasticBaseMaterial(tag,classtag,e,e0), commitStrain(0.0),
   trialStress(0.0), trialTangent(e)
  {}

//! @brief Commit the state of the material.
int XC::EPPBaseMaterial::commitState(void)
  {
    commitStrain= trialStrain;
    return 0;
  }

//! @brief Revert to the last commited state.
int XC::EPPBaseMaterial::revertToLastCommit(void)
  {
    trialStrain = commitStrain;
    return 0;
  }

//! @brief Returns the material to its initial state.
int XC::EPPBaseMaterial::revertToStart(void)
  {
    int retval= ElasticBaseMaterial::revertToStart();
    commitStrain= 0.0;
    trialStress= 0.0;
    trialTangent= this->E;
    return retval;
  }

//! @brief Set the value of the material tangent modulus.
void XC::EPPBaseMaterial::setTangent(const double &d)
  {
    ElasticBaseMaterial::setE(d);
    trialTangent= d;
  }

//! @brief Send object members through the communicator argument.
int XC::EPPBaseMaterial::sendData(Communicator &comm)
  {
    int res= ElasticBaseMaterial::sendData(comm);
    res+= comm.sendDoubles(commitStrain, trialStress,trialTangent,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::EPPBaseMaterial::recvData(const Communicator &comm)
  {
    int res= ElasticBaseMaterial::recvData(comm);
    res+= comm.receiveDoubles(commitStrain, trialStress,trialTangent,getDbTagData(),CommMetaData(3));
    return res;
  }

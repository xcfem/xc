// -*-c++-*-
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

#include "material/uniaxial/concrete/CreepShrinkageParameters.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Default constructor.
XC::CreepShrinkageParameters::CreepShrinkageParameters(void)
  : CommandEntity(), MovableObject(0),
    tcr(0.0),
    epsshu(0.0), epssha(0.0), epscra(0.0), epscru(0.0),
    epscrd(0.0)
  {}

//! @brief Constructor.
//! @param _epsshu: ultimate shrinkage strain εsh,u, as per ACI 209R-92.
//! @param _epssha: fitting parameter within the shrinkage time evolution function as per ACI 209R-92.
//! @param _tcr: creep model age in days.
//! @param _epscru: ultimate creep coefficient φu, as per ACI 209R-92.
//! @param _epscra: fitting constant within the creep time evolution function as per ACI 209R-92.
//! @param _epscrd: fitting constant within the creep time evolution function as per ACI 209R-92.
XC::CreepShrinkageParameters::CreepShrinkageParameters(const double &_tcr, const double &_epsshu, const double &_epssha, const double &_epscru, const double &_epscra, const double &_epscrd)
  : CommandEntity(), MovableObject(0),
    tcr(_tcr),
    epsshu(_epsshu), epssha(_epssha), epscra(_epscra), epscru(_epscru),
    epscrd(_epscrd)
  {
      
    //sigCr= fabs(sigCr);

    //Change inputs into the proper sign convention:
    this->setup_parameters();
  }

//! @brief Sets initial values for the concrete parameters.
void XC::CreepShrinkageParameters::setup_parameters(void)
  {
    //Change inputs into the proper sign convention:
    epsshu= -std::abs(epsshu);
    epscru= std::abs(epscru);
  }

//! @brief Return the f2 value used in setPhi method of creep materials.
double XC::CreepShrinkageParameters::getF2(const double &time, const double &tp) const
  {	
    // ACI Equation:
    const double tmtp= time-tp;
    return pow(tmtp,epscra)/(epscrd+pow(tmtp,epscra))*epscru;
  }

double XC::CreepShrinkageParameters::getShrink(const double &age, const double &time) const
  {
    const double shrinkTime= time-age; // Time from initiation of drying.
    double retval= 0.0;
    if(shrinkTime>=0)
      { retval= (shrinkTime) / (epssha + (shrinkTime)) * epsshu; }
    return retval;
  }

//! @brief Assigns ultimate shrinkage.
void XC::CreepShrinkageParameters::setUltimateShrinkage(const double &d)
  { epsshu= d; }

//! @brief Return ultimate shrinkage.
double XC::CreepShrinkageParameters::getUltimateShrinkage(void) const
  { return epsshu; }

//! @brief Assigns shrinkage parameter.
void XC::CreepShrinkageParameters::setShrinkageParameter(const double &d)
  { epssha= d; }

//! @brief Return shrinkage parameter.
double XC::CreepShrinkageParameters::getShrinkageParameter(void) const
  { return epssha; }

//! @brief Assigns creep relationship age.
void XC::CreepShrinkageParameters::setCreepRelationshipAge(const double &d)
  { tcr= d; }

//! @brief Return creep relationship age.
double XC::CreepShrinkageParameters::getCreepRelationshipAge(void) const
  { return tcr; }

//! @brief Assigns creep exponent parameter.
void XC::CreepShrinkageParameters::setUltimateConcreteCreep(const double &d)
  { epscru= d; }

//! @brief Return creep exponent parameter.
double XC::CreepShrinkageParameters::getUltimateConcreteCreep(void) const
  { return epscru; }

//! @brief Assigns creep exponent parameter.
void XC::CreepShrinkageParameters::setCreepExponentParameter(const double &d)
  { epscra= d; }

//! @brief Return creep exponent parameter.
double XC::CreepShrinkageParameters::getCreepExponentParameter(void) const
  { return epscra; }

//! @brief Assigns creep exponent parameter.
void XC::CreepShrinkageParameters::setCreepDParameter(const double &d)
  { epscrd= d; }

//! @brief Return creep exponent parameter.
double XC::CreepShrinkageParameters::getCreepDParameter(void) const
  { return epscrd; }

//! @brief Send object members through the communicator argument.
int XC::CreepShrinkageParameters::sendData(Communicator &comm)
  {
    const int res= comm.sendDoubles(tcr, epsshu, epssha, epscra, epscru, epscrd, getDbTagData(),CommMetaData(1));
    return res;
  }
//! @brief Receives object members through the communicator argument.
int XC::CreepShrinkageParameters::recvData(const Communicator &comm)
  {
    const int res= comm.receiveDoubles(tcr, epsshu, epssha, epscra, epscru, epscrd, getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::CreepShrinkageParameters::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
		<< Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::CreepShrinkageParameters::recvSelf(const Communicator &comm)
  {
    inicComm(1);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids."
		<< Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data."
		    << Color::def << std::endl;
      }
    return res;
  }

void XC::CreepShrinkageParameters::Print(std::ostream &s, int flag) const
  {
    s << "CreepShrinkageParameters: "
      << tcr << " "
      << epsshu << " " << epssha
      << epscru << " " << epscra
      << epscrd << " " << epscrd
      << std::endl;
  }



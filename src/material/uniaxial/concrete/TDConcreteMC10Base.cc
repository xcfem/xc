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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */


#include "TDConcreteMC10Base.h"
#include "domain/mesh/element/utils/Information.h"
#include <cmath>
#include <float.h>
#include "domain/domain/Domain.h" //Added by AMK to get current Domain time;
#include "utility/recorder/response/MaterialResponse.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Sets initial values for the concrete parameters.
void XC::TDConcreteMC10Base::setup_parameters(void)
  {
    TDConcreteBase::setup_parameters();

    //ntosic: SPLIT INTO BASIC AND DRYING!
    creepShrinkageState.setup_parameters(this->Ec); //Added by AMK
	
    //Change inputs into the proper sign convention: ntosic: changed
    creepShrinkageParameters.setup_parameters();
  }

//! @brief Constructor.
XC::TDConcreteMC10Base::TDConcreteMC10Base(int tag, int classTag)
  : TDConcreteBase(tag, classTag)
  {
    setup_parameters();
  }

//! @brief Constructor.
//! @param _fc: cylinder compressive strength (this is a dummy parameter since compression behavior is linear).
//! @param _ft: the tensile strength (splitting or axial tensile strength should be input, rather than the flexural).
//! @param _Ets : tension softening stiffness (absolute value) (slope of the linear tension softening branch).
//! @param _Ec: modulus of elasticity (preferably at time of loading if there is a single loading age).
//! @param _Ecm: 28-day modulus, necessary for normalizing creep coefficient.
//! @param _beta: tension softening parameter.
//! @param _age: analysis time at initiation of drying (in days).
//! @param _tcast: analysis time corresponding to concrete casting in days (note: concrete will not be able to take on loads until the age of 2 days).
XC::TDConcreteMC10Base::TDConcreteMC10Base(int tag, int classTag, double _fc, double _ft, double _Ets, double _Ec, double _Ecm, double _beta, double _age, double _tcast, const MC10CreepShrinkageParameters &csp)
  : TDConcreteBase(tag, classTag, _fc, _ft, _Ets, _Ec, _beta),
    Ecm(_Ecm), 
    creepShrinkageState(_age, _tcast, _Ec),
    creepShrinkageParameters(csp)
  {
    setup_parameters();
  }

//! @brief Get 28-day modulus, necessary for normalizing creep coefficient.
double XC::TDConcreteMC10Base::getEcm(void) const
  { return Ecm; }

//! @brief Set 28-day modulus, necessary for normalizing creep coefficient.
void XC::TDConcreteMC10Base::setEcm(const double &d)
  { Ecm= d; }

//! @brief Assign the values of the creep and shrinkage parameters.
void XC::TDConcreteMC10Base::setCreepShrinkageParameters(const MC10CreepShrinkageParameters &csParameters)
  { this->creepShrinkageParameters= csParameters; }

//! @brief Retrieve the values of the creep and shrinkage parameters.
const XC::MC10CreepShrinkageParameters &XC::TDConcreteMC10Base::getCreepShrinkageParameters(void) const
  { return this->creepShrinkageParameters; }

//ntosic
double XC::TDConcreteMC10Base::setPhiBasic(double time, double tp) const
  { return creepShrinkageParameters.getPhiBasic(time, tp); }

//ntosic
double XC::TDConcreteMC10Base::setPhiDrying(double time, double tp) const
  { return creepShrinkageParameters.getPhiDrying(time, tp); }

//ntosic
double XC::TDConcreteMC10Base::setShrinkBasic(double time)
  { return creepShrinkageParameters.getShrinkBasic(time); }

//ntosic
double XC::TDConcreteMC10Base::setShrinkDrying(double time)
  { return creepShrinkageParameters.getShrinkDrying(this->creepShrinkageState.getAge(), time); }

//ntosic
double XC::TDConcreteMC10Base::getPHIB_i(void) const
  { return phib_i; }
//ntosic
double XC::TDConcreteMC10Base::getPHID_i(void) const
  { return phid_i; }


//ntosic
//! @brief Return the creep occurring in a sealed (no drying) specimen,
//! driven by internal moisture movement.
double XC::TDConcreteMC10Base::setCreepBasicStrain(double time, double stress)
  {
    //ntosic: changed to Ecm from Ec (according to Model Code formulation of phi basic)
    const double creepBasic= creepSteps.computeBasicPhi(*this, this->Ecm, time);
    phib_i = creepSteps.getLastPhiB();
    return creepBasic;
  }

//ntosic
double XC::TDConcreteMC10Base::setCreepDryingStrain(double time, double stress)
  {
    //ntosic: changed to Ecm from Ec (according to Model Code formulation of phi basic)
    const double creepDrying= creepSteps.computeDryingPhi(*this, this->Ecm, time);
    phib_i = creepSteps.getLastPhiD();
    return creepDrying;
  }

int XC::TDConcreteMC10Base::revertToLastCommit(void)
  {
    this->creepShrinkageState.revert_to_last_commit();

    hstv= hstvP;
    return 0;
  }

int XC::TDConcreteMC10Base::revertToStart(void)
  {
    this->creepShrinkageState.revert_to_start(this->Ec);
    
    hstvP.revertToStart(Ec);

    hstv.setup_parameters(Ec);

    creepSteps.revertToStart();

    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::TDConcreteMC10Base::sendData(Communicator &comm)
  {
    int res= TDConcreteBase::sendData(comm);
    res+= comm.sendDouble(Ecm,getDbTagData(),CommMetaData(4));
    res+= comm.sendMovable(creepShrinkageParameters, getDbTagData(),CommMetaData(5));
    res+= comm.sendMovable(creepShrinkageState, getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::TDConcreteMC10Base::recvData(const Communicator &comm)
  {
    int res= TDConcreteBase::recvData(comm);
    res+= comm.receiveDouble(Ecm,getDbTagData(),CommMetaData(4));
    res+= comm.receiveMovable(creepShrinkageParameters, getDbTagData(),CommMetaData(5));
    res+= comm.receiveMovable(creepShrinkageState, getDbTagData(),CommMetaData(6));
    return res;
  }

int XC::TDConcreteMC10Base::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << __FUNCTION__
		<< "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

int XC::TDConcreteMC10Base::recvSelf(const Communicator &comm)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << __FUNCTION__
		<< "; failed to receive ids."
	        << Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << Color::red << getClassName() << __FUNCTION__
		    << "; failed to receive data."
		    << Color::def << std::endl;
      }
    return res;
  }


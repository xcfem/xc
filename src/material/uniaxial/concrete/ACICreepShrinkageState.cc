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
                                                                        
#include "material/uniaxial/concrete/ACICreepShrinkageState.h"
#include "utility/utils/misc_utils/colormod.h"

XC::ACICreepShrinkageState::ACICreepShrinkageState(void)
  : CreepShrinkageStateBase(),
    eps_cr(0.0), epsP_cr(00),
    eps_sh(0.0), epsP_sh(0.0)
  {}

XC::ACICreepShrinkageState::ACICreepShrinkageState(const double &_age, const double &_tcast, const double &_Et)
  : CreepShrinkageStateBase(_age, _tcast, _Et),
    eps_cr(0.0), epsP_cr(00),
    eps_sh(0.0), epsP_sh(0.0)
  {}

//! @brief Sets initial values for the concrete parameters.
void XC::ACICreepShrinkageState::setup_parameters(const double &_Et)
  {
    CreepShrinkageStateBase::setup_parameters(_Et);
    eps_cr= 0.0;
    epsP_cr= 0.0;
    eps_sh= 0.0;
    epsP_sh= 0.0; 
  }

int XC::ACICreepShrinkageState::commit_state(const int &count, const double &hstvP_sig, const double &currentTime)
  {
    CreepShrinkageStateBase::commit_state(count, hstvP_sig, currentTime);
	
    //Added by AMK:
    epsP_sh= eps_sh; // Commit shrinkage strain.
    epsP_cr= eps_cr; // Commit creep strain.
    
    return 0;
  }

int XC::ACICreepShrinkageState::revert_to_last_commit(void)
  {
    CreepShrinkageStateBase::revert_to_last_commit();
    eps_sh= epsP_sh;
    eps_cr= epsP_cr;
  
    return 0;
  }

int XC::ACICreepShrinkageState::revert_to_start(const double &Et)
  {
    CreepShrinkageStateBase::revert_to_start(Et);
    eps_sh= 0.0;
    epsP_sh= 0.0;
    eps_cr= 0.0;
    epsP_cr= 0.0;
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::ACICreepShrinkageState::sendData(Communicator &comm)
  {
    int res= CreepShrinkageStateBase::sendData(comm);
    res+= comm.sendDoubles(epsP_cr, epsP_sh, getDbTagData(),CommMetaData(4));
    return res;
  }
//! @brief Receives object members through the communicator argument.
int XC::ACICreepShrinkageState::recvData(const Communicator &comm)
  {
    int res= CreepShrinkageStateBase::recvData(comm);
    res+= comm.receiveDoubles(epsP_cr, epsP_sh, getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::ACICreepShrinkageState::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(5);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
		<< Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ACICreepShrinkageState::recvSelf(const Communicator &comm)
  {
    inicComm(5);
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


void XC::ACICreepShrinkageState::Print(std::ostream &s, int flag) const
  {
    s << "age= " << this->age
      << " tcast= " << this->tcast
      << " epsInit= " << this->epsInit
      << " sigInit= " << this->sigInit
      << " eps_cr= " << this->eps_cr
      << " epsP_cr= " << this->epsP_cr
      << " eps_sh= " << this->eps_sh
      << " epsP_sh= " << this->epsP_sh
      << " eps_m= " << this->eps_m
      << " epsP_m= " << this->epsP_m
      << " eps_total= " << this->eps_total
      << " epsP_total= " << this->epsP_total
      << " t= " << this->t
      << " t_load= " << this->t_load
      << " Et= " << this->Et
      << " crack_flag= " << this->crack_flag
      << " crackP_flag= " << this->crackP_flag
      << std::endl;
  }


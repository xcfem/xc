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

#include "material/uniaxial/concrete/CreepShrinkageStateBase.h"
#include "utility/utils/misc_utils/colormod.h"

XC::CreepShrinkageStateBase::CreepShrinkageStateBase(void)
  : CommandEntity(), MovableObject(0),
    age(0.0), tcast(0.0),
    epsInit(0.0), sigInit(0.0),
    eps_m(0.0), epsP_m(0.0),
    eps_total(0.0), epsP_total(0.0),
    t(0.0), t_load(-1.0), Et(0.0),
    crack_flag(0), crackP_flag(0)
  {}

XC::CreepShrinkageStateBase::CreepShrinkageStateBase(const double &_age, const double &_tcast, const double &_Et)
  : CommandEntity(), MovableObject(0),
    age(_age), tcast(_tcast),
    epsInit(0.0), sigInit(0.0),
    eps_m(0.0), epsP_m(0.0),
    eps_total(0.0), epsP_total(0.0),
    t(0.0), t_load(-1.0), Et(_Et),
    crack_flag(0), crackP_flag(0)
  {}

//! @brief Sets initial values for the concrete parameters.
void XC::CreepShrinkageStateBase::setup_parameters(const double &_Et)
  {
    this->epsP_m= 0.0;

    this->Et= _Et;
    this->epsInit= 0.0; //Added by AMK
    this->sigInit= 0.0; //Added by AMK
    this->eps_total= 0.0; //Added by AMK
    this->epsP_total= 0.0; //Added by AMK
    
    this->t_load= -1.0; //Added by AMK
    this->crack_flag= 0;
    this->crackP_flag= 0; // Added by LP
  }

void XC::CreepShrinkageStateBase::commit_eps_and_sig_init(const int &count, const double &hstvP_sig, const double &currentTime)
  {
    if(count==0)
      {
	this->epsInit= this->epsP_total;
	this->sigInit= hstvP_sig;
      }

    if(this->t_load<0.0) // if t_load not initialized yet.
      {
	if(this->sigInit<0.0) // if compression
	  {
	    this->t_load= currentTime;
	    this->sigInit= hstvP_sig;
	    this->epsInit= this->epsP_m;
	  }
	else if(this->sigInit>0.0 && hstvP_sig<0.0) // if tension after compression.
	  {
	    this->t_load= currentTime;
	    this->sigInit= hstvP_sig;
	    this->epsInit= this->epsP_m;
	  }
      }
  }

int XC::CreepShrinkageStateBase::commit_state(const int &count, const double &hstvP_sig, const double &currentTime)
  {
    //Added by AMK:
    epsP_total= eps_total; //Added by AMK;
    epsP_m= eps_m;
    
    //Cracking flags:
    crackP_flag= crack_flag; // Commit crack flag.
  
    //cracked reloading/unloading stiffness:
    if(this->crackP_flag==1)
      {
	if(hstvP_sig/this->eps_m<this->Et)
	  { this->Et= hstvP_sig/this->eps_m; }
      }
  
    this->commit_eps_and_sig_init(count, hstvP_sig, currentTime);
    
    return 0;
  }

int XC::CreepShrinkageStateBase::revert_to_last_commit(void)
  {
    eps_total= epsP_total; //Added by AMK;
    eps_m= epsP_m;  

    crack_flag= crackP_flag; // Revert crack flag. Added by LP
    return 0;
  }

int XC::CreepShrinkageStateBase::revert_to_start(const double &Et)
  {
    this->setup_parameters(Et);
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::CreepShrinkageStateBase::sendData(Communicator &comm)
  {
    int res= comm.sendDoubles(age, tcast, epsInit, sigInit, getDbTagData(),CommMetaData(1));
    res+= comm.sendDoubles(epsP_m, epsP_total, t, t_load, Et, getDbTagData(),CommMetaData(2));
    res+= comm.sendInt(crackP_flag, getDbTagData(),CommMetaData(3));
    return res;
  }
//! @brief Receives object members through the communicator argument.
int XC::CreepShrinkageStateBase::recvData(const Communicator &comm)
  {
    int res= comm.receiveDoubles(age, tcast, epsInit, sigInit, getDbTagData(),CommMetaData(1));
    res+= comm.receiveDoubles(epsP_m, epsP_total, t, t_load, Et, getDbTagData(),CommMetaData(2));
    res+= comm.receiveInt(crackP_flag, getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::CreepShrinkageStateBase::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
		<< Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::CreepShrinkageStateBase::recvSelf(const Communicator &comm)
  {
    inicComm(4);
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


void XC::CreepShrinkageStateBase::Print(std::ostream &s, int flag) const
  {
    s << "age= " << this->age
      << " tcast= " << this->tcast
      << " epsInit= " << this->epsInit
      << " sigInit= " << this->sigInit
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



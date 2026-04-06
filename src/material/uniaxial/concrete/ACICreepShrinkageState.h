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

#ifndef ACICreepShrinkageState_h
#define ACICreepShrinkageState_h

#include "material/uniaxial/concrete/CreepShrinkageStateBase.h"

namespace XC {

//! @brief Class that groups the variables that keep track of the
//! creep and shrinkage strains according to the ACI code.
//! @ingroup MatUnx
class ACICreepShrinkageState: public CreepShrinkageStateBase
  {
  private:

    double eps_cr; //!< Creep strain.
    double epsP_cr; //!< Commited creep strain.
    
    double eps_sh; //!< Shrinkage strain.    
    double epsP_sh; //!< Commited shrinkage strain.
    
  protected:
    void commit_eps_and_sig_init(void);
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    ACICreepShrinkageState(void);
    ACICreepShrinkageState(const double &_age, const double &_tcast, const double &_Et);
    void setup_parameters(const double &);

    
    double getCreep(void) const //Added by AMK
      { return eps_cr; }
    void setCreep(const double &d)
      { this->eps_cr= d; }
    double getShrink(void) const //Added by AMK
      { return eps_sh; }
    void setShrink(const double &d)
      { this->eps_sh= d; }
    
    void set_eps_cr_sh_m_total(const double &_eps_cr, const double &_eps_sh, const double &_eps_m, const double &_eps_total)
      {
	CreepShrinkageStateBase::set_eps_m_total(_eps_m, _eps_total);
	this->eps_cr= _eps_cr; // Creep strain.
	this->eps_sh= eps_sh; // Shrinkage strain.
      }
    void update_mech_strain(void)
      { this->eps_m= this->eps_total - this->eps_cr - this->eps_sh; }
    void use_creep_shrinkage_from_last_commit(void)
      {
	this->eps_cr= this->epsP_cr; // Creep strain.
	this->eps_sh= this->epsP_sh; // Shrinkage strain.
	this->update_mech_strain(); // Mechanical strain.
      }

    int commit_state(const int &count, const double &hstvP_sig, const double &currentTime);
    int revert_to_last_commit(void);    
    int revert_to_start(const double &_Et);        

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace


#endif


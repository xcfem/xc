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

#ifndef MC10CreepShrinkageState_h
#define MC10CreepShrinkageState_h

#include "material/uniaxial/concrete/CreepShrinkageStateBase.h"

namespace XC {

//! @brief Class that groups the variables that keep track of the
//! creep and shrinkage strains according to the Model Code 1010.
//! @ingroup MatUnx
class MC10CreepShrinkageState: public CreepShrinkageStateBase
  {
  private:

    double eps_crb; //!< trial basic creep strain (ntosic).
    double eps_crd; //!< trial drying creep strain (ntosic).
    double eps_shb; //!< trial basic shrinkage strain (ntosic).
    double eps_shd; //!< trial drying shrinkage strain (ntosic).
    
    double epsP_crb; //!< commited basic creep strain (ntosic).
    double epsP_crd; //!< commited drying creep strain (ntosic).
    double epsP_shb; //!< commited basic shrinkage strain (ntosic).
    double epsP_shd; //!< commited drying shrinkage strain (ntosic).
    
  protected:
    void commit_eps_and_sig_init(void);
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    MC10CreepShrinkageState(void);
    MC10CreepShrinkageState(const double &_age, const double &_tcast, const double &_Et);
    void setup_parameters(const double &);


    double getCreepBasic(void) const
      { return eps_crb; }
    double getCreepDrying(void) const
      { return eps_crd; }
    double getShrinkBasic(void) const
      { return eps_shb; }
    double getShrinkDrying(void) const
      { return eps_shd; }
    
    void setCreepBasic(const double &d)
      { this->eps_crb= d; }
    void setCreepDrying(const double &d)
      { this->eps_crd= d; }
    void setShrinkBasic(const double &d)
      { this->eps_shb= d; }
    void setShrinkDrying(const double &d)
      { this->eps_shd= d; }
    
    void set_eps_cr_sh_m_total(const double &_eps_crb, const double &_eps_crd, const double &_eps_shb, const double &_eps_shd, const double &_eps_m, const double &_eps_total)
      {
	CreepShrinkageStateBase::set_eps_m_total(_eps_m, _eps_total);
	this->eps_crb= _eps_crb; // Basic creep strain.
	this->eps_crd= _eps_crd; // Drying creep strain.
	this->eps_shb= _eps_shb; // Basic shrinkage strain.
	this->eps_shd= _eps_shd; // Drying shrinkage strain.
      }
    void update_mech_strain(void)
      {
	const double shrinkage_strain= this->eps_shb + this->eps_shd;
	const double creep_strain= this->eps_crb + this->eps_crd;
	const double non_mech_strain= shrinkage_strain + creep_strain;
	this->eps_m= this->eps_total - non_mech_strain;
      }
    
    void use_creep_shrinkage_from_last_commit(void)
      {
	this->eps_crb= this->epsP_crb; // Basic creep strain.
	this->eps_crd= this->epsP_crd; // Drying creep strain.
	this->eps_shb= this->epsP_shb; // Basic shrinkage strain.
	this->eps_shd= this->epsP_shd; // Drying shrinkage strain.
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


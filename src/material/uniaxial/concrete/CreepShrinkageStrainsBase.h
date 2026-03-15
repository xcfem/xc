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

#ifndef CreepShrinkageStrainsBase_h
#define CreepShrinkageStrainsBase_h

#include "utility/kernel/CommandEntity.h"
#include "utility/actor/actor/MovableObject.h"
namespace XC {


//! @ingroup MatUnx
//
//! @brief Creep material.
class CreepShrinkageStrainsBase: public CommandEntity, public MovableObject
  {
  protected:

    double age; //!< concrete age at first loading, Added by AMK
    double tcast; //!< the analysis time corresponding to concrete casting in days.
    //Added by AMK:
    double epsInit;
    double sigInit;
    
    double eps_m; //!< Mechanical strain.
    double epsP_m; //!< Commited mechanical strain.
    
    double eps_total; //!< Total strain.
    double epsP_total; //!< Commited total strain.
    
    double t; //!< Time.
    double t_load; //!< Loaded time.
    double Et; //!< Stiffness at time t.
    
    int crack_flag;
    int crackP_flag;

    void commit_eps_and_sig_init(const int &count, const double &hstvP_sig, const double &currentTime);
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    CreepShrinkageStrainsBase(void);
    CreepShrinkageStrainsBase(const double &_age, const double &_tcast, const double &_Et);
    void setup_parameters(const double &);

    void set_eps_m_total(const double &_eps_m, const double &_eps_total)
      {
	this->eps_m= _eps_m; // Mechanical strain.
	this->eps_total= eps_total; // Total strain.
      }
    double get_total_strain(void) const
      { return this->eps_total; }
    void set_total_strain(const double &d)
      { this->eps_total= d; }
    double getStrain(void) const // Added by AMK
      { return eps_total; }
    
    double getMech(void) const // Added by AMK
      { return eps_m; }

    double getAge(void) const
      { return this->age; }
    
    double getCastingAge(const double &t) const
      { return t-this->tcast; }

    int commit_state(const int &count, const double &hstvP_sig, const double &currentTime);
    int revert_to_last_commit(void);    
    int revert_to_start(const double &_Et);        

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace


#endif


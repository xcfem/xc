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

// Keep track of the creep history variables.

#ifndef CreepSteps_h
#define CreepSteps_h

#include <vector>

namespace XC {

class CreepConcreteHistoryVars;

class CreepSteps
  {
  protected:
    //Added by AMK:
    int count;
    
    std::vector<float> E_i;
    std::vector<float> DSIG_i;
    // std::vector<float> dsig_i; // NOT USED.
    std::vector<float> TIME_i; //Time from the previous time step
    std::vector<float> DTIME_i;

    static int creepControl; //!< Controls creep calculation (see setTrialStrain).
    static double creepDt; 

    virtual std::size_t resize(void);
    
  public:
    CreepSteps(void);

    const int &getCount(void) const
      { return this->count;}
    const int &next(void)
      {
        this->count++;
	this->resize();
	return count;
      }

    const float &getLastTime(void) const
      { return this->TIME_i[this->count]; }
    void initTime(const float &time)
      { this->TIME_i[0]= time; }

    void setCreepDt(void)
      { this->DTIME_i[this->count]= creepDt; }
    
    void assignNextStep(const CreepConcreteHistoryVars &hstv, const CreepConcreteHistoryVars &hstvP, const double &Ec, const double &eps_m, const double &currentTime);

    static void setCreepOn(void);
    static void setCreepOff(void);
    static bool isCreepOn(void);
    static void setCreepDt(const double &);
    static double getCreepDt(void);
  };

inline void set_creep_on(void)
  { CreepSteps::setCreepOn(); }
inline void set_creep_off(void)
  { CreepSteps::setCreepOff(); }
inline bool is_creep_on(void)
  { return CreepSteps::isCreepOn(); }
inline bool is_creep_off(void)
  { return !is_creep_on(); }
inline void set_creep_dt(const double &d)
  { CreepSteps::setCreepDt(d); }
inline double get_creep_dt(void)
  { return CreepSteps::getCreepDt(); }

} // end of XC namespace

#endif


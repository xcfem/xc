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
   
#ifndef MC10CreepSteps_h
#define MC10CreepSteps_h

#include "material/uniaxial/concrete/CreepSteps.h"

namespace XC {
  
class MC10CreepSteps: public CreepSteps
  {
  protected:
    std::vector<float> PHIB_i; //!< split into basic and drying creep (ntosic)
    std::vector<float> PHID_i; //!< split into basic and drying creep (ntosic)

  protected:
    std::size_t resize(void);
  public:
    MC10CreepSteps(void);

    double getLastPhiB(void) const
      { return this->PHIB_i[this->count]; }
    double getLastPhiD(void) const
      { return this->PHID_i[this->count]; }
    
    template <class CreepConcrete>
    double computeBasicPhi(const CreepConcrete &concrete, const double &Ec, const double &time)
      {    
	this->setCreepDt();

	double retval = 0.0;
	for(int i = 1; i<=count; i++)
	  {
	    this->PHIB_i[i] = concrete.setPhiBasic(time,this->TIME_i[i]); // Determine PHI
	    retval+= this->PHIB_i[i]*this->DSIG_i[i]/Ec; // CONSTANT STRESS within Time interval
	  }

	return retval;
      }
    
    template <class CreepConcrete>
    double computeDryingPhi(const CreepConcrete &concrete, const double &Ec, const double &time)
      {    
	this->setCreepDt();

	double retval = 0.0;
	for(int i = 1; i<=count; i++)
	  {
	    this->PHID_i[i] = concrete.setPhiDrying(time,this->TIME_i[i]); // Determine PHI
	    retval+= this->PHID_i[i]*this->DSIG_i[i]/Ec; // CONSTANT STRESS within Time interval
	  }

	return retval;
      }
      
    int revertToStart(void);        
  };

} // end of XC namespace

#endif


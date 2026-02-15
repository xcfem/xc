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

#ifndef ACICreepSteps_h
#define ACICreepSteps_h

#include "material/uniaxial/concrete/CreepSteps.h"
#include <iostream>

namespace XC {

class ACICreepSteps : public CreepSteps
  {
  private:
    std::vector<float> PHI_i;

  protected:
    std::size_t resize(void);
    
  public:
    ACICreepSteps(void);

    const float &getLastPhi(void) const
      { return this->PHI_i[this->count]; }
    
    template <class CreepConcrete>
    double computePhi(const CreepConcrete &concrete, const double &Ec, const double &time)
      {
	this->setCreepDt();

	double retval = 0.0;
	for(int i = 1; i<=count; i++)
	  {
	    this->PHI_i[i] = concrete.setPhi(time,this->TIME_i[i]); // Determine PHI
	    retval+= this->PHI_i[i]*this->DSIG_i[i]/Ec; // CONSTANT STRESS within Time interval
	  }

	return retval;
      }

    int revertToStart(void);
  };

} // end of XC namespace

#endif


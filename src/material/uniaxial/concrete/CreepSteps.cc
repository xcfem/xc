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

#include "material/uniaxial/concrete/CreepSteps.h" //Changed by AMK
#include "material/uniaxial/concrete/ConcreteHistoryVars.h"

int XC::CreepSteps::creepControl= 0;
double XC::CreepSteps::creepDt= 0.0;

//! @brief Resize the vectors that store the creep history.
std::size_t XC::CreepSteps::resize(void)
  {
    const size_t currentSize= E_i.size();
    if(this->count<2) // restart.
      {
	const std::size_t newSize= 10;
	E_i.resize(newSize);
	DSIG_i.resize(newSize);
	// dsig_i.resize(newSize);
	TIME_i.resize(newSize);
	DTIME_i.resize(newSize);
      }
    else
      {
	const size_t minSize= static_cast<std::size_t>(this->count+1);
	if(minSize>=currentSize)
	  {
            const size_t newSize= 2*(count+1);
	    E_i.resize(newSize);
	    DSIG_i.resize(newSize);
	    // dsig_i.resize(newSize);
	    TIME_i.resize(newSize);
	    DTIME_i.resize(newSize);
	  }
      }
    return E_i.size();
  }

//! @brief Constructor.
XC::CreepSteps::CreepSteps(void)
  : count(0)
  {
    resize();
  }

//! @brief Assign the values of the next step and resize the vectors if needed.
//| @param hstv: Concrete history variables current step.
//! @param hstvP: Concrete history variables last committed step.
//! @param Ec: concrete stiffness.
//! @param eps_m: concrete mechanical strain.
void XC::CreepSteps::assignNextStep(const CreepConcreteHistoryVars &hstv, const CreepConcreteHistoryVars &hstvP, const double &Ec, const double &eps_m, const double &currentTime)
  {
    // dsig_i[count]=hstv.sig-hstvP.sig; NOT USED
    DSIG_i[count+1] = hstv.sig-hstvP.sig;

    //Secant stiffness for determination of creep strain:
    E_i[count+1]= hstv.getSecantStiffness(Ec, eps_m);
    
    TIME_i[count+1]= currentTime;
  }
void XC::CreepSteps::setCreepOn(void)
  { creepControl= 1; }

void XC::CreepSteps::setCreepOff(void)
  { creepControl= 0; }

bool XC::CreepSteps::isCreepOn(void)
  { return (creepControl==1); }

void XC::CreepSteps::setCreepDt(const double &d)
  { creepDt= d; }

double XC::CreepSteps::getCreepDt(void)
  { return creepDt; }


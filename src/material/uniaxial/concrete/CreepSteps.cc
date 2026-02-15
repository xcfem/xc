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

int XC::CreepSteps::creepControl= 0;
double XC::CreepSteps::creepDt= 0.0;

//! @brief Resize the vectors that store the creep history.
std::size_t XC::CreepSteps::resize(void)
  {
    std::size_t newSize= 10;
    if(count<2) // restart.
      {
	E_i.resize(newSize);
	DSIG_i.resize(newSize);
	dsig_i.resize(newSize);
	TIME_i.resize(newSize);
	DTIME_i.resize(newSize);
      }
    else
      { 
	if(static_cast<std::size_t>(count+1)>=E_i.size())
	  {
            newSize= 2*(count+1);
	    E_i.resize(newSize);
	    DSIG_i.resize(newSize);
	    dsig_i.resize(newSize);
	    TIME_i.resize(newSize);
	    DTIME_i.resize(newSize);
	  }
      }
    return newSize;
  }

//! @brief Constructor.
XC::CreepSteps::CreepSteps(void)
  : count(0)
  {
    resize();
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


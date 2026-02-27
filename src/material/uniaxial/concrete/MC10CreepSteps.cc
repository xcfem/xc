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

#include "MC10CreepSteps.h"

std::size_t XC::MC10CreepSteps::resize(void)
  {
    const std::size_t newSize= CreepSteps::resize();
    if(newSize!=PHIB_i.size()) // restart.
      {
	PHIB_i.resize(newSize);
    	PHID_i.resize(newSize);
      }
    return newSize;
  }

//! @brief Constructor.
XC::MC10CreepSteps::MC10CreepSteps(void)
  : CreepSteps()
  { }


int XC::MC10CreepSteps::revertToStart(void)
  {
    if(creepControl==0)
      { count= 0; }
    else
      { count= 1; }
    this->resize();
    return 0;
  }


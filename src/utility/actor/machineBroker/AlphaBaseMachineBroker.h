// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//AlphaMachineBroker.h

#ifndef AlphaBaseMachineBroker_h
#define AlphaBaseMachineBroker_h

#include <utility/actor/machineBroker/MachineBroker.h>

namespace XC {
//! @ingroup IPComm
//
//! @brief Base class for DEC Alpha machine brokers.
class AlphaBaseMachineBroker: public MachineBroker
  {
  protected:
    int currentMachine;
    int maxNumMachines;
    std::vector<std::string> machines;
  public:
    AlphaBaseMachineBroker(FEM_ObjectBroker *theBroker, int currentMachine,int maxNumMachines);
  };
} // end of XC namespace

#endif

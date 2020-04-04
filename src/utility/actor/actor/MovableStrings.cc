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
//MovableStrings.cc

#include "MovableStrings.h"

//! @brief Constructor.
XC::MovableStrings::MovableStrings(std::deque<std::string> &v)
  : MovableContainer<std::deque<std::string> >(v) {}

//! @brief Sends a container intem.
int XC::MovableStrings::sendItem(const std::string &s,Communicator &comm,DbTagData &dt, const CommMetaData &meta)
  { return comm.sendString(s,dt,meta); }

//! @brief Receives a container item.
int XC::MovableStrings::receiveItem(std::string &s,const Communicator &comm,DbTagData &dt, const CommMetaData &meta)
  { return comm.receiveString(s,dt,meta); }

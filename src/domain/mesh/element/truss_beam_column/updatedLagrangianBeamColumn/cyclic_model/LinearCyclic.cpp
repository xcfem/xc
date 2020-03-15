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
//LinearCyclic.cpp

#include "LinearCyclic.h"
#include <cmath>

XC::LinearCyclic::LinearCyclic(int tag)
  :CyclicModel(tag, -1) {}


XC::CyclicModel *XC::LinearCyclic::getCopy()
  { return new LinearCyclic(*this); }

double XC::LinearCyclic::getTaskFactor(void)
  { return resFactor; }

void XC::LinearCyclic::Print(std::ostream &s, int flag) const
  {
    this->XC::CyclicModel::Print(s, flag);
    s << "+LinearCyclic\n";
    s << "   taskFactor = " << resFactor << std::endl;
    s << "----------------------------------------"
      << "----------------------------------------"
      << std::endl;
  }




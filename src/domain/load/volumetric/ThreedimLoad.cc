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
//ThreedimLoad.cpp

#include "ThreedimLoad.h"

XC::ThreedimLoad::ThreedimLoad(int tag,int classTag,const ID &theElementTags)
  :ElementBodyLoad(tag, classTag, theElementTags) {}

XC::ThreedimLoad::ThreedimLoad(int tag,int classTag)
  :ElementBodyLoad(tag, classTag) {}

//! @brief Print stuff.
void XC::ThreedimLoad::Print(std::ostream &s, int flag) const
  {
    s << "ThreedimLoad - Reference load" << std::endl;
    ElementBodyLoad::Print(s,flag);
  }

std::string XC::ThreedimLoad::Category(void) const
  { return "threedim_undef"; }

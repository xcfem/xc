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
//BidimLoad.cpp

#include "BidimLoad.h"

#include "utility/matrix/ID.h"

XC::BidimLoad::BidimLoad(int tag,int classTag,const ID &theElementTags)
  :ElementBodyLoad(tag, classTag, theElementTags) {}

XC::BidimLoad::BidimLoad(int tag,int classTag)
  :ElementBodyLoad(tag, classTag) {}

//! @brief Print stuff.
void XC::BidimLoad::Print(std::ostream &s, int flag) const
  {
    s << "BidimLoad - Reference load" << std::endl;
    ElementBodyLoad::Print(s,flag);
  }

std::string XC::BidimLoad::Category(void) const
  { return "bidim_nodef"; }

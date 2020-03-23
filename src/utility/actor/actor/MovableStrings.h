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
//MovableStrings
                                                                        
                                                                        
#ifndef MovableStrings_h
#define MovableStrings_h

#include "MovableContainer.h"
#include <deque>

namespace XC {

//! @ingroup IPComm
//
//! @brief Strings that can move between processes.
class MovableStrings: public MovableContainer<std::deque<std::string> >
  {
  public:
    explicit MovableStrings(std::deque<std::string> &);
    int sendItem(const std::string &s,CommParameters &,DbTagData &, const CommMetaData &);
    int receiveItem(std::string &s,const CommParameters &,DbTagData &, const CommMetaData &);

    const std::deque<std::string> &getStrings(void) const
      { return container; }
  };

} // end of XC namespace

#endif


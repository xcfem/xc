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
                                                                        
                                                                        
// Description: This file contains the class definition for IDVarSize.
// IDVarSize integer array with variable size (see [] operators).

#ifndef IDVarSize_h
#define IDVarSize_h

#include "ID.h"

namespace XC {
//! @ingroup Matrix
//
class IDVarSize: public ID
  {
  public:
    // constructors and destructor
    IDVarSize(const int &,const int &);

    int &operator[](const int &i);
    const int &operator[](const int &i) const
      { return this->at(i); }
  };

} // end of XC namespace

#endif




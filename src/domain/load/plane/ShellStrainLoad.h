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
//ShellStrainLoad.h
                                                                        
#ifndef ShellStrainLoad_h
#define ShellStrainLoad_h

#include "BidimStrainLoad.h"

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Carga debida a deformación impuesta en un elemento Truss.
class ShellStrainLoad : public BidimStrainLoad
  {
  public:
    ShellStrainLoad(int tag, const ID &theElementTags);
    ShellStrainLoad(int tag= 0);
  };
} // end of XC namespace

#endif


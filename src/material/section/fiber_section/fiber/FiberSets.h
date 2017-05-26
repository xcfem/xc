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
//FiberSets.h

#ifndef FiberSets_h
#define FiberSets_h

#include "FiberDeque.h"

class Poligono2d;

namespace XC {

//! \ingroup MATSCCFibers
//
//! @brief Fiber sets container.
class FiberSets: public std::map<std::string,FiberDeque>, public EntCmd
  {
  public:
    FiberSets(void); 

    FiberDeque &create_fiber_set(const std::string &nombre);
    iterator get_fiber_set(const std::string &nmb_set);
    iterator sel_mat_tag(FiberDeque &fibers, const std::string &nmb_set,const int &matTag);
    iterator resel_mat_tag(const std::string &nmb_set,const std::string &nmb_set_org,const int &matTag);
  };
} // end of XC namespace

#endif

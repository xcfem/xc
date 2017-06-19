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
//FiberSets.cc

#include "FiberSets.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

//! @brief Constructor.
XC::FiberSets::FiberSets(void)
  : std::map<std::string,FiberDeque>()
  {}

//! @brief Creates a new fiber set.
XC::FiberDeque &XC::FiberSets::create_fiber_set(const std::string &nmb)
  {
    if(find(nmb) == end()) //Set doesn't exists
      (*this)[nmb]= FiberDeque();
    else
      std::cerr << "Fiber set: '" << nmb
                << "' already exists. Command ignored." << std::endl;
    return (*this)[nmb];
  }

//! @brief Creates a fiber set which name is being passed as parameter.
XC::FiberSets::iterator XC::FiberSets::get_fiber_set(const std::string &nmb_set)
  {
    iterator i= find(nmb_set);
    if(i == end())
      {
        if(verbosity>1)
          std::clog << "Fiber set: '" << nmb_set
                    << "' doesn't exists; it is created." << std::endl;
        create_fiber_set(nmb_set);
        i= find(nmb_set);
      }
    return i;
  }

//! @brief Creates a fiber set which material has the tag being passed as parameter.
XC::FiberSets::iterator XC::FiberSets::sel_mat_tag(FiberDeque &fibers, const std::string &nmb_set,const int &matTag)
  {
    iterator i= get_fiber_set(nmb_set);
    fibers.SelMatTag(matTag,(*i).second);
    return i;
  }

//! @brief Creates a fiber set with the fibers that belongs to the set named
//! nmb_set_org, and have the material identified with the tag being passed as parameter.
//! @param nmb_set: name of the new set.
//! @param nmb_set_org: name of the set that contains the fibers.
//! @matTag: material tag.
XC::FiberSets::iterator XC::FiberSets::resel_mat_tag(const std::string &nmb_set,const std::string &nmb_set_org,const int &matTag)
  {
    iterator i= end();
    if(nmb_set != nmb_set_org)
      {
        i= get_fiber_set(nmb_set);
        iterator j= find(nmb_set_org);
        if(j == end())
          {
            std::clog << "Origin fiber set: '" << nmb_set_org
                      << "' doesn't exists; command ignored." << std::endl;
          }
        else
          (*j).second.SelMatTag(matTag,(*i).second);
      }
    return i;
  }



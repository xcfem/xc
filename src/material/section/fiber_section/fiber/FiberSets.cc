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
#include "xc_utils/src/geom/d2/2d_polygons/Polygon2d.h"

//! @brief Constructor.
XC::FiberSets::FiberSets(void)
  : std::map<std::string,FiberSet>()
  {}

//! @brief Return true if the set already exists.
bool XC::FiberSets::exists(const std::string &name)
  {
    bool retval= false;
    if(find(name) != end()) //Set exists
      retval= true;
    return retval;
  }

//! @brief Creates a new fiber set.
XC::FiberSet &XC::FiberSets::create_fiber_set(const std::string &name)
  {
    FiberSet *retval= nullptr;
    if(!exists(name)) //Set doesn't exists
      retval= &((*this)[name]= FiberSet());
    else
      {
	retval= &((*this)[name]);
	retval->clear(); //Set redefinition.
      }
    return *retval;
  }

//! @brief Creates a fiber set which name is being passed as parameter.
XC::FiberSets::iterator XC::FiberSets::get_fiber_set(const std::string &set_name)
  {
    iterator i= find(set_name);
    if(i == end())
      {
        if(verbosity>1)
          std::clog << "Fiber set: '" << set_name
                    << "' doesn't exists; it is created." << std::endl;
        create_fiber_set(set_name);
        i= find(set_name);
      }
    return i;
  }

//! @brief Creates a fiber set which material has the tag being passed as parameter.
XC::FiberSets::iterator XC::FiberSets::sel_mat_tag(FiberSet &fibers, const std::string &set_name,const int &matTag)
  {
    iterator i= get_fiber_set(set_name);
    fibers.SelMatTag(matTag,(*i).second);
    return i;
  }

//! @brief Creates a fiber set with the fibers that belongs to the set named
//! origin_set_name, and have the material identified with the tag being passed as parameter.
//! @param set_name: name of the new set.
//! @param origin_set_name: name of the set that contains the fibers.
//! @matTag: material tag.
XC::FiberSets::iterator XC::FiberSets::resel_mat_tag(const std::string &set_name,const std::string &origin_set_name,const int &matTag)
  {
    iterator i= end();
    if(set_name != origin_set_name)
      {
        i= get_fiber_set(set_name);
        iterator j= find(origin_set_name);
        if(j == end())
          {
            std::clog << "Origin fiber set: '" << origin_set_name
                      << "' doesn't exists; command ignored." << std::endl;
          }
        else
          (*j).second.SelMatTag(matTag,(*i).second);
      }
    return i;
  }



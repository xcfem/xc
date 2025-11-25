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

#include <set>
#include <string>
#include "load_class_names.h"

const std::set<std::string> nodal_load_class_names= {"XC::NodalLoad"};
  
//! @brief Return true if the given class name corresponds to a nodal load.
bool XC::is_nodal_load(const std::string &className)
  { return (nodal_load_class_names.find(className)!= nodal_load_class_names.end()); }
  
const std::set<std::string> elemental_load_class_names= {"XC::Beam2dPointLoad", "XC::Beam3dPointLoad", "XC::Beam2dPartialUniformLoad", "XC::Beam2dUniformLoad", "XC::Beam3dUniformLoad", "XC::BeamStrainLoad", "XC::QuadRawLoad", "XC::ShellRawLoad", "XC::ShellUniformLoad", "XC::ShellStrainLoad", "XC::QuadStrainLoad", "XC::SelfWeight", "XC::SurfaceLoad", "XC::BrickSelfWeight", "XC::BrickRawLoad", "XC::BrickStrainLoad", "XC::TrussStrainLoad", "XC::TrussPrestressLoad"};
  
//! @brief Return true if the given class name corresponds to an elemental load.
bool XC::is_elemental_load(const std::string &className)
  { return (elemental_load_class_names.find(className)!= elemental_load_class_names.end()); }

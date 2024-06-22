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
#ifndef coordinate_transformation_class_names_h
#define coordinate_transformation_class_names_h

#include <set>

namespace XC {

  const std::set<std::string> crd_transf_class_names= {"XC::CrdTransf", "XC::CrdTransf2d", "XC::CorotCrdTransf2d", "XC::SmallDispCrdTransf2d", "XC::LinearCrdTransf2d", "XC::PDeltaCrdTransf2d", "XC::CrdTransf3d", "XC::CorotCrdTransf3d", "XC::SmallDispCrdTransf3d", "XC::LinearCrdTransf3d", "XC::PDeltaCrdTransf3d", "XC::ASDShellQ4CorotationalTransformation", "XC::ShellCrdTransf3dBase", "XC::ShellLinearCrdTransf3d", "XC::ShellUpBasisCrdTransf3d", "XC::ShellNLCrdTransf3d"};
  
//! @brief Return true if the given class name corresponds to a coordinate transformation.
bool is_coordinate_transformation(const std::string &className)
  { return (XC::crd_transf_class_names.find(className)!= XC::crd_transf_class_names.end()); }
  
} // end of XC namespace

#endif


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
#include "section_material_class_names.h"

const std::set<std::string> section_material_class_names= {"XC::SectionForceDeformation", "XC::Bidirectional", "XC::GenericSectionNd", "XC::Isolator2spring", "XC::PlateBase", "XC::ElasticPlateBase", "XC::ElasticPlateProto<>", "XC::ElasticMembranePlateSection", "XC::ElasticPlateSection", "XC::MembranePlateFiberSectionBase", "XC::LayeredShellFiberSection", "XC::MembranePlateFiberSection", "XC::PrismaticBarCrossSection", "XC::BaseElasticSection", "XC::BaseElasticSection1d", "XC::ElasticSection1d", "XC::BaseElasticSection2d", "XC::ElasticSection2d", "XC::ElasticShearSection2d", "XC::BaseElasticSection3d", "XC::ElasticSection3d", "XC::ElasticShearSection3d", "XC::FiberSectionBase", "XC::FiberSection2d", "XC::FiberSectionShear2d", "XC::FiberSection3dBase", "XC::FiberSection3d", "XC::FiberSectionShear3d", "XC::FiberSectionGJ", "XC::GenericSection1d", "XC::SectionAggregator", "XC::YieldSurfaceSection2d", "XC::YS_Section2D01", "XC::YS_Section2D02"};
  
//! @brief Return true if the given class name corresponds to a coordinate transformation.
bool XC::is_section_material(const std::string &className)
  { return (section_material_class_names.find(className)!= section_material_class_names.end()); }
  


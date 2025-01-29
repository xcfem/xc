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
#include "constraint_class_names.h"

const std::set<std::string> sp_constraint_class_names= {"XC::SFreedom_Constraint", "XC::ImposedMotionSP", "XC::ImposedMotionSP1"};
  
//! @brief Return true if the given class name corresponds to a SP constraint.
bool XC::is_sp_constraint(const std::string &className)
  { return (sp_constraint_class_names.find(className)!= sp_constraint_class_names.end()); }

const std::set<std::string> mp_constraint_class_names={"XC::EqualDOF", "XC::MFreedom_Joint2D", "XC::MFreedom_Joint3D", "XC::RigidBeam", "XC::RigidRod", "XC::RigidDiaphragm", "XC::Skew_Constraint", "XC::SkewPlane", "XC::SymmetryConstraint", "XC::AntiSymmetryConstraint"};
  
//! @brief Return true if the given class name corresponds to a MP constraint.
bool XC::is_mp_constraint(const std::string &className)
  { return (mp_constraint_class_names.find(className)!= mp_constraint_class_names.end()); }
  
const std::set<std::string> mrmp_constraint_class_names= {"XC::MRMFreedom_Constraint", "XC::GlueNodeToElement"};
  
//! @brief Return true if the given class name corresponds to a MRMF constraint.
bool XC::is_mrmp_constraint(const std::string &className)
  { return (mrmp_constraint_class_names.find(className)!= mrmp_constraint_class_names.end()); }
  

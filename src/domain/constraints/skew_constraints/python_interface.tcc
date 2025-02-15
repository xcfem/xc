//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
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
//python_interface.tcc

class_<XC::Skew_Constraint, bases<XC::MFreedom_ConstraintBase>, boost::noncopyable >("Skew_Constraint", no_init);

class_<XC::SkewPlane, bases<XC::Skew_Constraint>, boost::noncopyable >("SkewPlane", no_init)
  .add_property("normal", make_function(&XC::SkewPlane::getNormal, return_internal_reference<>() ),"Returns the vector normal to the plane or line.")
  ;

class_<XC::SymmetryConstraint, bases<XC::SkewPlane>, boost::noncopyable >("SymmetryConstraint", no_init)
  ;

class_<XC::AntiSymmetryConstraint, bases<XC::SkewPlane>, boost::noncopyable >("AntiSymmetryConstraint", no_init)
  ;

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

// class_<XC::IntegratorVectors, bases<XC::MovableObject>, boost::noncopyable >("IntegratorVectors", no_init);

#include "arc-length/python_interface.tcc"

class_<XC::BaseControl , bases<XC::StaticIntegrator>, boost::noncopyable >("BaseControl", no_init)
  .def("setNumIncr", &XC::BaseControl::setNumIncr,"Set the number of iterations the user would like to occur in the solution algorithm.")
  
  ;

class_<XC::DispBase, bases<XC::BaseControl>, boost::noncopyable >("DispBase", no_init);

class_<XC::DisplacementControl, bases<XC::DispBase>, boost::noncopyable >("DisplacementControl", no_init)
  .add_property("nodeTag",&XC::DisplacementControl::getNodeTag,&XC::DisplacementControl::setNodeTag,"assign/retrieve node tag value.")
  .add_property("dof",&XC::DisplacementControl::getDof,&XC::DisplacementControl::setDof,"assign/retrieve dof index.")
  .add_property("dU1",&XC::DisplacementControl::getIncrement,&XC::DisplacementControl::setIncrement,"assign/retrieve increment value.")
  .add_property("increment",&XC::DisplacementControl::getIncrement,&XC::DisplacementControl::setIncrement,"assign/retrieve increment value.")
  .add_property("minIncrement",&XC::DisplacementControl::getMinIncrement,&XC::DisplacementControl::setMinIncrement,"assign/retrieve the min stepsize the user will allow.")
  .add_property("maxIncrement",&XC::DisplacementControl::getMaxIncrement,&XC::DisplacementControl::setMaxIncrement,"assign/retrieve the max stepsize the user will allow.")
  ;

class_<XC::MinUnbalDispNorm, bases<XC::DispBase>, boost::noncopyable >("MinUnbalDispNorm", no_init);

// class_<XC::DistributedDisplacementControl, bases<XC::DisplacementControl>, boost::noncopyable >("DistributedDisplacementControl", no_init);

class_<XC::LoadControl , bases<XC::BaseControl>, boost::noncopyable >("LoadControl", no_init)
  .add_property("dLambda1",&XC::LoadControl::getDeltaLambda,&XC::LoadControl::setDeltaLambda,"assign/retrieve dLambda value.")
  ;

class_<XC::LoadPath, bases<XC::StaticIntegrator>, boost::noncopyable >("LoadPath", no_init);





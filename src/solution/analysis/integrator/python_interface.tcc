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
class_<XC::Integrator, bases<XC::MovableObject,CommandEntity>, boost::noncopyable >("Integrator", no_init)
  .def("domainChanged", &XC::Integrator::domainChanged,"Make required changes when a change in the domain occurs.")
  .add_property("currentModelTime",&XC::Integrator::getCurrentModelTime,&XC::Integrator::setCurrentModelTime,"Get/set the current time of the analysis model.")
  ;

class_<XC::EigenIntegrator, bases<XC::Integrator>, boost::noncopyable >("EigenIntegrator", no_init);

class_<XC::IncrementalIntegrator, bases<XC::Integrator>, boost::noncopyable >("IncrementalIntegrator", no_init)
  .add_property("tangFlag",&XC::IncrementalIntegrator::getTangFlag,&XC::IncrementalIntegrator::setTangFlag,"Get/set the value of the flag to compute the tangent stiffness: CURRENT_TANGENT: 0; INITIAL_TANGENT: 1; CURRENT_SECANT: 2; INITIAL_THEN_CURRENT_TANGENT: 3; NO_TANGENT: 4; SECOND_TANGENT: 5; HALL_TANGENT: 6")
  ;

class_<XC::StaticIntegrator, bases<XC::IncrementalIntegrator>, boost::noncopyable >("StaticIntegrator", no_init);

class_<XC::TransientIntegrator, bases<XC::IncrementalIntegrator>, boost::noncopyable >("TransientIntegrator", no_init);

#include "eigen/python_interface.tcc"
#include "static/python_interface.tcc"
#include "transient/python_interface.tcc"

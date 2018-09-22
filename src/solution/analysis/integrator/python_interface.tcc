//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
class_<XC::Integrator, bases<XC::MovableObject,CommandEntity>, boost::noncopyable >("Integrator", no_init);

class_<XC::EigenIntegrator, bases<XC::Integrator>, boost::noncopyable >("EigenIntegrator", no_init);

class_<XC::IncrementalIntegrator, bases<XC::Integrator>, boost::noncopyable >("IncrementalIntegrator", no_init);

class_<XC::StaticIntegrator, bases<XC::IncrementalIntegrator>, boost::noncopyable >("StaticIntegrator", no_init);

class_<XC::TransientIntegrator, bases<XC::IncrementalIntegrator>, boost::noncopyable >("TransientIntegrator", no_init);

#include "eigen/python_interface.tcc"
#include "static/python_interface.tcc"
#include "transient/python_interface.tcc"

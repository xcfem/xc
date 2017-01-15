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

class_<XC::FrictionResponse, bases<XC::Response>, boost::noncopyable >("FrictionResponse", no_init);

class_<XC::FrictionModel , bases<XC::TaggedObject>, boost::noncopyable >("FrictionModel", no_init);

class_<XC::CoulombFriction, bases<XC::FrictionModel>, boost::noncopyable >("CoulombFriction", no_init);

class_<XC::VDependentFriction, bases<XC::CoulombFriction>, boost::noncopyable >("VDependentFriction", no_init);

class_<XC::VPDependentFriction, bases<XC::VDependentFriction>, boost::noncopyable >("VPDependentFriction", no_init);




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

class_<XC::DomainComponent, bases<XC::TaggedObject>, boost::noncopyable >("DomainComponent", no_init)
  .add_property("getIdx", &XC::DomainComponent::getIdx,"Returns an index for use in VTK arrays.")
  .add_property("getDomain", make_function( &XC::DomainComponent::getDomain, return_internal_reference<>() ),"Returns a reference to the domain.")
  ;

class_<XC::ContinuaReprComponent, bases<XC::DomainComponent>, boost::noncopyable >("ContinuaReprComponent", no_init)
  .add_property("isDead", &XC::ContinuaReprComponent::isDead,"true if domain component is not active.")
  .add_property("isAlive", &XC::ContinuaReprComponent::isAlive,"true if domain component is not active.")
  .add_property("kill", &XC::ContinuaReprComponent::kill,"deactivates the domain component.")
  .add_property("alive", &XC::ContinuaReprComponent::alive,"activates the domain component.")
   ;

